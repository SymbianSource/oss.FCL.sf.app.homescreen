/*
 * ===========================================================================
 *  Name        : hgfswengine.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Task monitor engine
 *  Version     : %version: sa1spcx1#37 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#include "hgfswengine.h"
#include "hgfsalwaysshownapplist.h"
#include "hgfshiddenapplist.h"
#include "hgfswidgetlist.h"
#include "hgfswiconcache.h"
#include "hgpreviewprovider.h"
#include "hgfswclient.h"
#include <apgtask.h>
#include <AknDef.h>
#include <apgwgnam.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <featmgr.h>
#include <swi/swispubsubdefs.h>

#include "enginelogging.h"

// time to wait before refreshing content
const TInt KContentRefreshDelay = 500000; // 0.5 sec

// for screenshots, they are scaled down to (screensize/this_factor).
const TInt KScreenSizeFactor = 2;

// format to get a lowercase hex string prefixed with 0x
_LIT( KHexFmt, "0x%x" );

// --------------------------------------------------------------------------
// CHgFswEngine::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CHgFswEngine* CHgFswEngine::NewL( MHgFswEngineObserver& aObserver )
    {
    CHgFswEngine* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CHgFswEngine* CHgFswEngine::NewLC( MHgFswEngineObserver& aObserver )
    {
    CHgFswEngine* self = new ( ELeave ) CHgFswEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::CHgFswEngine
// --------------------------------------------------------------------------
//
CHgFswEngine::CHgFswEngine( MHgFswEngineObserver& aObserver )
        : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CHgFswEngine::ConstructL
// --------------------------------------------------------------------------
//
void CHgFswEngine::ConstructL()
    {
    iEnv = CEikonEnv::Static();

    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iAppArcSession.Connect() );

    iHiddenAppList = CHgFsHiddenAppList::NewL( *this );
    iAlwaysShownAppList = CHgFsAlwaysShownAppList::NewL();

    iWidgetsSupported = FeatureManager::FeatureSupported( KFeatureIdWebWidgets );
    if ( iWidgetsSupported )
        {
        iWidgetList = CHgFsWidgetList::NewL();
        }

    // create app icon retriever instance
    iAppIcons = CHgFswIconCache::NewL();

    // get an initial list of tasks
    iAppDataRefreshNeeded = ETrue;
    CollectTasksL();

    // listen for app screenshots
    iPreviewProvider = CHgFastSwapPreviewProvider::NewL( *this );
    SetPreviewParams();
    UpdatePreviewContent();

    iUpdateStarter = CPeriodic::NewL( CActive::EPriorityStandard );
    
    // start listening for swi ps property
    if ( iSwiProp.Attach( KUidSystemCategory,
            Swi::KUidSoftwareInstallKey ) == KErrNone )
        {
        iSwiProp.Subscribe( iStatus );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CHgFswEngine::~CHgFswEngine
// --------------------------------------------------------------------------
//
CHgFswEngine::~CHgFswEngine()
    {
    Cancel();
    iSwiProp.Close();

    delete iUpdateStarter;
    delete iPreviewProvider;

    // delete the bitmaps as the hash map cannot do that
    THashMapIter<TInt, CFbsBitmap*> iter( iScreenshots );
    while ( const TInt* key = iter.NextKey() )
        {
        CFbsBitmap** value = iter.CurrentValue();
        delete *value;
        }
    iScreenshots.Close();
    delete iAppIcons;

    iData.ResetAndDestroy();
    iWgIds.Close();

    delete iHiddenAppList;
    delete iAlwaysShownAppList;
    delete iWidgetList;
    iAppArcSession.Close();
    iWsSession.Close();

//    delete iContextUtility;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::FswDataL
// --------------------------------------------------------------------------
//
EXPORT_C const RHgFswArray& CHgFswEngine::FswDataL()
    {
    HGLOG_CONTEXT( FswDataL, HGLOG_LOCAL );
    HGLOG1_IN( "dirty flag = %d", iTaskListDirty );

    // check the dirty flag and refresh if needed
    if ( iTaskListDirty )
        {
        CollectTasksL();
        // dirty flag is cleared in the above call
        }

    // Get app icon for entries without screenshot,
    // do this only here as the app icon is not needed in case a screenshot
    // is already available.
    for ( TInt i = 0, ie = iData.Count(); i != ie; ++i )
        {
        if ( !iData[i]->ScreenshotHandle() )
            {
            CFbsBitmap* bmp;
            CFbsBitmap* mask;
            TUid appUid = iData[i]->AppUid();
            // this will leave with -46 in case of widgets if we do not have AllFiles cap
            TRAPD( err, iAppIcons->GetIconL( appUid, bmp, mask ) );
            if ( err == KErrNone && bmp )
                {
                iData[i]->SetAppIconHandles( bmp->Handle(),
                    mask ? mask->Handle() : 0 );
                }
            else
                {
                iAppIcons->DefaultIcon( bmp, mask );
                iData[i]->SetAppIconHandles( bmp->Handle(),
                    mask ? mask->Handle() : 0 );
                }
            }
        }

    HGLOG_OUT();
    return iData;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::CloseAppL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEngine::CloseAppL( TInt aWgId )
    {
    HGLOG_CONTEXT( CloseAppL, HGLOG_LOCAL );
    HGLOG1_IN( "aWgId = %d", aWgId );

    if ( iWidgetsSupported && aWgId < 0 && iWidgetAppUiWgId )
        {
        // for widgets clients see a wgid that is -1*(index+1)
        const RWidgetInfoArray& arr( iWidgetList->RunningWidgets() );
        // convert aWgId to an index in the list of running widgets
        TInt idx = -aWgId - 1;
        // if index is valid then handle the widget specially
        if ( idx >= 0 && idx < arr.Count() )
            {
            TWsEvent event;
            event.SetType( EEventUser );
            TUint8* eventData = event.EventData();
            // Fill bits 0-31 with widget application uid.
            reinterpret_cast<TUint32&>( *eventData ) = KWidgetAppUidValue;
            eventData += sizeof( TUint32 );
            // Fill bits 32-63 with uid of the widget that should be closed.
            reinterpret_cast<TUint32&>( *eventData ) = arr[idx]->iUid.iUid;
            // Send the event to Widget AppUi.
            iEnv->WsSession().SendEventToWindowGroup(
                iWidgetAppUiWgId, event );
            // closing a widget may not cause a window group change so trigger
            // the update manually
            UpdateTaskList();
            HGLOG0_OUT( "widget processing finished" );
            return;
            }
        }

    TUid appUid = AppUidForWgIdL( aWgId );
    if ( !iAlwaysShownAppList->IsAlwaysShownApp( appUid ) )
        {
        // send window group event to kill the app
        TWsEvent event;
        event.SetTimeNow();
        event.SetType( KAknShutOrHideApp );
        iEnv->WsSession().SendEventToWindowGroup( aWgId, event );
        HGLOG0( HGLOG_INFO, "event sent to wg" );
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::SwitchToAppL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEngine::SwitchToAppL( TInt aWgId )
    {
    HGLOG_CONTEXT( SwitchToAppL, HGLOG_LOCAL );
    HGLOG1_IN( "aWgId = %d", aWgId );

    if ( iWidgetsSupported && aWgId < 0 )
        {
        // for widgets clients see a wgid that is -1*(index+1)
        const RWidgetInfoArray& arr( iWidgetList->RunningWidgets() );
        // convert aWgId to an index in the list of running widgets
        TInt idx = -aWgId - 1;
        // if index is valid then handle the widget specially
        if ( idx >= 0 && idx < arr.Count() )
            {
            SwitchToWidgetL( idx );
            HGLOG1_OUT( "switched to widget %d", idx );
            return;
            }
        }

    TApaTask task( iEnv->WsSession() );
    task.SetWgId( aWgId );
    task.BringToForeground();

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::SwitchToWidgetL
// --------------------------------------------------------------------------
//
void CHgFswEngine::SwitchToWidgetL( TInt aWidgetIndex )
    {
    const RWidgetInfoArray& arr( iWidgetList->RunningWidgets() );
    RApaLsSession ls;
    User::LeaveIfError( ls.Connect() );
    CleanupClosePushL( ls );
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetCommandL( EApaCommandRun );
    TApaAppInfo info;
    iAppArcSession.GetAppInfo( info, arr[aWidgetIndex]->iUid ); // codescanner::accessArrayElementWithoutCheck2 (aWidgetIndex checked in SwitchToAppL())
    cmdLine->SetExecutableNameL( info.iFullName );
    ls.StartApp( *cmdLine );
    CleanupStack::PopAndDestroy( 2, &ls );
    }

// --------------------------------------------------------------------------
// CHgFswEngine::UpdateTaskList
// Callback from appui
// --------------------------------------------------------------------------
//
void CHgFswEngine::UpdateTaskList()
    {
    HGLOG_CONTEXT( UpdateTaskList, HGLOG_LOCAL );
    HGLOG_IN();

    // If no clients are subscribed for fsw content change notifications
    // then there is no need to waste time with rebuilding the task list,
    // just set the dirty flag.
    TInt listenerCount = iObserver.FswDataListenerCount();
    HGLOG1( HGLOG_INFO, "listener count = %d", listenerCount );
    if ( listenerCount > 0 )
        {
        // There can be many calls in a row, use a timer to prevent degrading
        // device performance.
        if ( !iUpdateStarter->IsActive() )
            {
            iUpdateStarter->Start( KContentRefreshDelay, 0,
                TCallBack( UpdateStarterCallback, this ) );
            }
        }
    else
        {
        iTaskListDirty = ETrue;
        }

    // screenshot taking support - call Register and Unregister when needed
    UpdatePreviewContent();

    // get the foreground app uid and publish it to CFW if different than before
    TRAP_IGNORE( PublishFgAppUidL() );
    
    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::ForegroundAppUidL
// --------------------------------------------------------------------------
//
EXPORT_C TUid CHgFswEngine::ForegroundAppUidL( TInt aType )
    {
    // Do not use GetFocusWindowGroup or others.
    // We want to see only "real" application uids in FgApp,
    // i.e. stuff that would also be shown in the task swapper
    // area of Teleport.

    TUid result = KNullUid;
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL( allWgIds );
    User::LeaveIfError( iWsSession.WindowGroupList( 0, &allWgIds ) );
    TInt count = allWgIds.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // Depending on aType we may not need to bother with iParentId here.
        // If aType == EUseEmbeddedUid => embedded apps are treated as others.
        // If aType == EUseStandaloneUid => embedded apps must be ignored.
        if ( allWgIds[i].iParentId > 0
                && aType == CHgFswClient::EUseStandaloneUid )
            {
            continue;
            }

        CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(
            iWsSession, allWgIds[i].iId );
        TUid newUid = wgn->AppUid();
        TBool hidden = wgn->Hidden() || iHiddenAppList->IsHiddenL(
            newUid, iWsSession, allWgIds[i].iId );
        CleanupStack::PopAndDestroy( wgn );

        if ( !hidden && newUid.iUid )
            {
            result = newUid;
            break;
            }
        }
    CleanupStack::PopAndDestroy( &allWgIds );
    
    return result;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::PublishFgAppUidL
// --------------------------------------------------------------------------
//
void CHgFswEngine::PublishFgAppUidL()
    {
    HGLOG_CONTEXT( PublishFgAppUidL, HGLOG_LOCAL );
    HGLOG_IN();

    TUid newUid = ForegroundAppUidL( CHgFswClient::EUseEmbeddedUid );
    HGLOG2( HGLOG_INFO, "ws says: %x we have: %x", newUid.iUid, iFgAppUid.iUid );
    if ( iFgAppUid != newUid && newUid.iUid )
        {
        iFgAppUid = newUid;
        iFgAppUidStr.Format( KHexFmt, iFgAppUid.iUid );
        //PublishContextL( KHgCFTypeFgApp, iFgAppUidStr );
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::PublishContextL
// --------------------------------------------------------------------------
//
//void CHgFswEngine::PublishContextL( const TDesC& aType, const TDesC& aValue )
//    {
//    HGLOG_CONTEXT( PublishContextL, HGLOG_LOCAL );
//    HGLOG2_IN( "%S: %S", &aType, &aValue );
//    if ( !iContextUtility )
//        {
//        iContextUtility = CHgContextUtility::NewL();
//        iContextUtility->AllowPublishFromBackground( ETrue );
//        }
//    iContextUtility->PublishContextL( aType, aValue );
//    HGLOG_OUT();
//    }

// --------------------------------------------------------------------------
// CHgFswEngine::UpdateStarterCallback
// Callback for the timer
// --------------------------------------------------------------------------
//
TInt CHgFswEngine::UpdateStarterCallback( TAny* aParam )
    {
    CHgFswEngine* self = static_cast<CHgFswEngine*>( aParam );
    self->iUpdateStarter->Cancel();
    // get list of tasks and notify observer if there is a change in the list
    TBool changed = EFalse;
    TRAPD( err, changed = self->CollectTasksL() );
    if ( err == KErrNone && changed )
        {
        self->iObserver.FswDataChanged();
        }
    return 0;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CHgFswEngine::CollectTasksL()
    {
    HGLOG_CONTEXT( CollectTasksL, HGLOG_LOCAL );
    HGLOG_IN();

    // clear dirty flag
    iTaskListDirty = EFalse;

    TBool changed = EFalse;
    RHgFswArray newList;
    CleanupResetAndDestroyPushL( newList );

    // update app data if needed
    // (usually on startup and when new apps might have been installed)
    if ( iAppDataRefreshNeeded )
        {
        HGLOG0( HGLOG_INFO, "refreshing app data" );
        iAppArcSession.GetAllApps();
        iAlwaysShownAppList->InitializeAlwaysShownListL();
        iAppDataRefreshNeeded = EFalse;
        }

    // get all window groups
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL( allWgIds );
    User::LeaveIfError( iWsSession.WindowGroupList( 0, &allWgIds ) );
    TInt count = allWgIds.Count();

    for ( TInt i = 0; i < count; ++i )
        {
        // ignore uninteresting entries (e.g. embedded apps)
        if ( allWgIds[i].iParentId > 0 )
            {
            continue;
            }

        // get window group name
        TInt wgId = allWgIds[i].iId;
        CApaWindowGroupName* windowName =
            CApaWindowGroupName::NewLC( iWsSession, wgId );
        TUid appUid = windowName->AppUid();

        // ignore entries with null uid
        if ( !appUid.iUid )
            {
            CleanupStack::PopAndDestroy( windowName );
            continue;
            }

        // will append the task to our own list only if it is not hidden
        TBool onHiddenList = iHiddenAppList->IsHiddenL(
            appUid, iWsSession, wgId );

        // if this is the widget app then save wgid for later use
        // and ignore it, but include running widgets instead
        if ( iWidgetsSupported && appUid.iUid == KWidgetAppUidValue )
            {
            iWidgetAppUiWgId = wgId;
            onHiddenList = ETrue;
            CheckWidgetsL( newList );
            }

        // get screen number (-1=console, 0=main screen, 1=cover ui)
        TInt appScreen = 0;
        TInt scrNumErr = iAppArcSession.GetDefaultScreenNumber( appScreen, appUid );

        // check if it is on always-shown list
        TBool mustShow = iAlwaysShownAppList->IsAlwaysShownApp( appUid );

#ifdef _DEBUG
        const TDesC& captionDbg( windowName->Caption() );
        HGLOG4( HGLOG_INFO, "[%d] wgid=%d appuid=%x (%S)", i, wgId,
            appUid.iUid, &captionDbg );
        HGLOG4( HGLOG_INFO, "hidden=%d onhiddenlist=%d mustshow=%d scrno=%d",
            windowName->Hidden(), onHiddenList, mustShow, appScreen );
#endif

        // add item to task list if it is always-shown OR it is not hidden
        // and it is not on cover ui
        if ( mustShow
                || ( !onHiddenList
                    && !windowName->Hidden()
                    && ( appScreen == 0 || appScreen == -1 )
                    && scrNumErr == KErrNone ) )
            {
            if ( AddEntryL( wgId, appUid, windowName, newList, EFalse ) )
                {
                changed = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( windowName );
        }
    CleanupStack::PopAndDestroy( &allWgIds );    

    // if counts for old and new lists do not match then there is a change for sure,
    // probably an app has been closed
    if ( iData.Count() != newList.Count() )
        {
        changed = ETrue;
        }

    // move pointers from the temporary list into the real one
    iData.ResetAndDestroy();
    TInt newListCount = newList.Count();
    HGLOG1( HGLOG_INFO, "new list count = %d", newListCount );
    for ( TInt i = 0; i < newListCount; ++i )
        {
        HGLOG3( HGLOG_INFO, "[%d] %S wgid=%d",
            i, &newList[i]->AppName(), newList[i]->WgId() );
        iData.AppendL( newList[i] );
        newList[i] = 0;
        }
    CleanupStack::PopAndDestroy( &newList );
    
    HGLOG1_OUT( "change flag = %d", changed );
    return changed;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::AddEntryL
// --------------------------------------------------------------------------
//
TBool CHgFswEngine::AddEntryL( TInt aWgId, const TUid& aAppUid,
        CApaWindowGroupName* aWgName, RHgFswArray& aNewList,
        TBool aIsWidget )
    {
    TBool changed = EFalse;
    CHgFswEntry* entry = CHgFswEntry::NewLC();
    entry->SetWgId( aWgId );
    entry->SetAppUid( aAppUid );
    entry->SetWidget( aIsWidget );

    // check if present in old list and if yes then take some of the old data
    TBool found = CheckIfExistsL( *entry, aAppUid, changed, aNewList );

    // if not present previously then find out app name
    // and check if screenshot is already available
    if ( !found )
        {
        // when adding a new entry to the list it is changed for sure
        changed = ETrue;
        HBufC* name = FindAppNameLC( aWgName, aAppUid, aWgId );
        if ( name )
            {
            entry->SetAppNameL( *name );
            }
        CleanupStack::PopAndDestroy( name );
        TInt h = LookupScreenshotHandle( aWgId );
        if ( h )
            {
            entry->SetScreenshotHandle( h );
            }
        entry->SetAlwaysShown( iAlwaysShownAppList->IsAlwaysShownApp( aAppUid ) );
        if ( aWgName )
            {
            entry->SetSystemApp( aWgName->IsSystem() );
            }
        }

    // add to new list, ownership is transferred
    aNewList.AppendL( entry );
    CleanupStack::Pop( entry );
    return changed;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::CheckIfExistsL
// --------------------------------------------------------------------------
//
TBool CHgFswEngine::CheckIfExistsL( CHgFswEntry& aEntry,
        const TUid& aAppUid,
        TBool& aChanged,
        RHgFswArray& aNewList )
    {
    for ( TInt entryIdx = 0, oldCount = iData.Count();
            entryIdx < oldCount; ++entryIdx )
        {
        // Enough to check appuid, no need to bother with wgid as the
        // screenshot handle is up-to-date or will be updated later anyway.
        if ( iData[entryIdx]->AppUid() == aAppUid )
            {
            // if positions do not match then the list is different than before
            if ( entryIdx != aNewList.Count() )
                {
                aChanged = ETrue;
                }
            CHgFswEntry* oldEntry = iData[entryIdx];
            aEntry.SetAppNameL( oldEntry->AppName() );
            aEntry.SetScreenshotHandle( oldEntry->ScreenshotHandle() );
            aEntry.SetAlwaysShown( oldEntry->AlwaysShown() );
            aEntry.SetSystemApp( oldEntry->SystemApp() );
            return ETrue;
            }
        }
    return EFalse;
    }
    
// --------------------------------------------------------------------------
// CHgFswEngine::CheckWidgetsL
// --------------------------------------------------------------------------
//
void CHgFswEngine::CheckWidgetsL( RHgFswArray& aNewList )
    {
    if ( iWidgetsSupported )
        {
        iWidgetList->InitializeWidgetListL();
        const RWidgetInfoArray& arr( iWidgetList->RunningWidgets() );
        for ( TInt i = 0, ie = arr.Count(); i != ie; ++i )
            {
            // wgid will be a special negative value
            // windowgroupname is not needed here so pass NULL
            AddEntryL( -(i+1), arr[i]->iUid, 0, aNewList, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CHgFswEngine::HiddenAppListUpdated
// Callback from the hidden app list watcher
// --------------------------------------------------------------------------
//
void CHgFswEngine::HiddenAppListUpdated()
    {
    UpdateTaskList();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::AppUidForWgIdL
// --------------------------------------------------------------------------
//
TUid CHgFswEngine::AppUidForWgIdL( TInt aWgId )
    {
    CApaWindowGroupName* windowName =
        CApaWindowGroupName::NewLC( iWsSession, aWgId );
    TUid appUid = windowName->AppUid();
    CleanupStack::PopAndDestroy( windowName );
    return appUid;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::FindParentWgId
// --------------------------------------------------------------------------
//
TInt CHgFswEngine::FindParentWgId( TInt aWgId )
    {
    TInt parent( KErrNotFound );
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    // Ask for window group list from RWsSession
    TInt error = iWsSession.WindowGroupList( 0, &allWgIds );
    if ( !error )
        {
        TInt count( allWgIds.Count() );
        for ( TInt i( 0 ); i < count; i++ )
            {
            RWsSession::TWindowGroupChainInfo info = allWgIds[i];
            if ( info.iId == aWgId )
                {
                parent = info.iParentId;
                break;
                }
            }
        }
    allWgIds.Close();
    return parent;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::FindAppNameLC
// --------------------------------------------------------------------------
//
HBufC* CHgFswEngine::FindAppNameLC( CApaWindowGroupName* aWindowName,
                                  const TUid& aAppUid,
                                  TInt aWgId )
    {
    //Retrieve the app name
    TApaAppInfo info;
    iAppArcSession.GetAppInfo( info, aAppUid );
    TPtrC caption = info.iShortCaption;

    HBufC* tempName = 0;
    if ( !caption.Length() && aWindowName ) // if not set - use thread name instead
        {
        if ( aWindowName->Caption().Length() )
            {
            tempName = aWindowName->Caption().AllocL();
            //put on cleanupstack after the if
            }
        else
            {
            TThreadId threadId;
            TInt err = iWsSession.GetWindowGroupClientThreadId(
                    aWgId, threadId );
            if ( err == KErrNone )
                {
                RThread thread;
                CleanupClosePushL( thread );
                err = thread.Open ( threadId );
                if ( err==KErrNone )
                    {
                    tempName = thread.Name().AllocL(); // codescanner::forgottoputptroncleanupstack
                    // tempName put on cleanupstack after the if
                    }
                CleanupStack::PopAndDestroy( &thread );
                }
            }
        }
    else
        {
        tempName = caption.AllocL();
        //put on cleanupstack after the if
        }
    CleanupStack::PushL( tempName );
    return tempName;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::CopyBitmapL
// --------------------------------------------------------------------------
//
CFbsBitmap* CHgFswEngine::CopyBitmapL( TInt aFbsHandle, TBool aKeepAspectRatio )
    {
    CFbsBitmap* ret = new (ELeave) CFbsBitmap;
    CleanupStack::PushL( ret );
    CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
    CleanupStack::PushL( bmp );
    User::LeaveIfError( bmp->Duplicate( aFbsHandle ) );

    //Make bitmap width and height equal so that it will look better in FSW control.
    TSize size  = bmp->SizeInPixels();

    if ( !aKeepAspectRatio )
        {
        if ( size.iHeight > size.iWidth )
            {
            size.iHeight = size.iWidth;
            }
        else
            {
            size.iWidth = size.iHeight;
            }
        }

    // preserve size and mode
    User::LeaveIfError( ret->Create( size, EColor64K ) );

    CFbsBitmapDevice* doubleBufferDev = CFbsBitmapDevice::NewL( ret );
    CleanupStack::PushL( doubleBufferDev );

    CFbsBitGc* doubleBufferGc = 0;
    User::LeaveIfError( doubleBufferDev->CreateContext( doubleBufferGc ) );
    CleanupStack::PushL( doubleBufferGc );

    // preserve size
    doubleBufferGc->BitBlt( TPoint( 0, 0 ), bmp, TRect(size) );

    CleanupStack::PopAndDestroy( doubleBufferGc );
    CleanupStack::PopAndDestroy( doubleBufferDev );
    CleanupStack::PopAndDestroy( bmp );
    CleanupStack::Pop( ret );

    return ret;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::UpdatePreviewContent
// --------------------------------------------------------------------------
//
void CHgFswEngine::UpdatePreviewContent()
    {
    HGLOG_CONTEXT( UpdatePreviewContent, HGLOG_LOCAL );
    HGLOG_IN();

    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    if ( iWsSession.WindowGroupList( 0, &allWgIds ) == KErrNone )
        {
        // check window groups that do not exist anymore
        for ( TInt i = 0, ie = iWgIds.Count(); i != ie; ++i )
            {
            TInt wgid = iWgIds[i];
            TBool present = EFalse;
            for ( TInt j = 0, je = allWgIds.Count(); j != je; ++j )
                {
                if ( allWgIds[j].iId == wgid )
                    {
                    present = ETrue;
                    break;
                    }
                }
            if ( !present )
                {
                HGLOG1( HGLOG_INFO, "unregister: %d", wgid );
                iPreviewProvider->Unregister( wgid );
                }
            }
        // check topmost window group
        if ( allWgIds.Count() )
            {
            if ( !iWgIds.Count() || allWgIds[0].iId != iWgIds[0] )
                {
                TInt wgid = allWgIds[0].iId;
                HGLOG1( HGLOG_INFO, "registering %d", wgid );
                iPreviewProvider->Register( wgid );
                }
            }
        // store the new list
        iWgIds.Reset();
        for ( TInt i = 0, ie = allWgIds.Count(); i != ie; ++i )
            {
            iWgIds.Append( allWgIds[i].iId );
            }
        }
    allWgIds.Close();

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::HandleFswPpApplicationChange
// Callback from CHgFastSwapPreviewProvider
// Note: aWgId is for the window group for which the screenshot is taken,
// it may not be the same as the application's wgid (in case of embedded apps)
// --------------------------------------------------------------------------
//
void CHgFswEngine::HandleFswPpApplicationChange( TInt aWgId, TInt aFbsHandle )
    {
    HGLOG_CONTEXT( HandleFswPpApplicationChange, HGLOG_LOCAL );
    HGLOG2_IN( "aWgId = %d aFbsHandle = %d", aWgId, aFbsHandle );

    CFbsBitmap* bmp = 0;
    TRAPD( err, bmp = CopyBitmapL( aFbsHandle, EFalse ) );
    iPreviewProvider->AckPreview(aFbsHandle);
    if ( err == KErrNone )
        {
        CFbsBitmap** oldbmp = iScreenshots.Find( aWgId );
        if ( oldbmp )
            {
            delete *oldbmp;
            }
        if ( iScreenshots.Insert( aWgId, bmp ) != KErrNone )
            {
            delete bmp;
            iScreenshots.Remove( aWgId );
            }
        else
            {
            AssignScreenshotHandle( aWgId, bmp->Handle() );
            }
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::HandleFswPpApplicationUnregistered
// Callback from CHgFastSwapPreviewProvider
// --------------------------------------------------------------------------
//
void CHgFswEngine::HandleFswPpApplicationUnregistered( TInt aWgId )
    {
    HGLOG_CONTEXT( HandleFswPpApplicationUnregistered, HGLOG_LOCAL );
    HGLOG1_IN( "aWgId = %d", aWgId );

    CFbsBitmap** bmp = iScreenshots.Find( aWgId );
    if ( bmp )
        {
        delete *bmp;
        iScreenshots.Remove( aWgId );
        AssignScreenshotHandle( aWgId, 0 );
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::AssignScreenshotHandle
// Called when a screenshot arrives to check if there is a corresponding
// application in the task list.
// --------------------------------------------------------------------------
//
void CHgFswEngine::AssignScreenshotHandle( TInt aWgIdForScreenshot,
        TInt aBitmapHandle )
    {
    TBool changed = EFalse;
    TInt parentWgId = FindParentWgId( aWgIdForScreenshot );
    // now parentWgId is a valid wgid or KErrNotFound (-1)
    for ( TInt i = 0, ie = iData.Count(); i != ie; ++i )
        {
        if ( iData[i]->Widget() )
            {
            // Do not do anything for now => no screenshot for widgets.
            continue;
            }
        TInt appWgId = iData[i]->WgId();
        if ( appWgId == aWgIdForScreenshot || appWgId == parentWgId )
            {
            iData[i]->SetScreenshotHandle( aBitmapHandle );
            changed = ETrue;
            break;
            }
        }
    if ( changed )
        {
        iObserver.FswDataChanged();
        }
    }

// --------------------------------------------------------------------------
// CHgFswEngine::LookupScreenshotHandle
// Called to check if there is a screenshot already stored for the given
// app. (or a screenshot with a wgid for which aWgIdForApp is the parent)
// --------------------------------------------------------------------------
//
TInt CHgFswEngine::LookupScreenshotHandle( TInt aWgIdForApp )
    {
    CFbsBitmap** bmp = iScreenshots.Find( aWgIdForApp );
    if ( bmp )
        {
        // there is a screenshot stored for this wgid
        return (*bmp)->Handle();
        }
    else if ( aWgIdForApp > 0 )
        {
        // must check if there is a screenshot for which aWgIdForApp is the parent
        THashMapIter<TInt, CFbsBitmap*> iter( iScreenshots );
        while ( const TInt* wgid = iter.NextKey() )
            {
            if ( FindParentWgId( *wgid ) == aWgIdForApp )
                {
                CFbsBitmap** bmp = iter.CurrentValue();
                return (*bmp)->Handle();
                }
            }
        }
    else if ( aWgIdForApp < 0 )
        {
        // Must be a widget, wgid is useless in this case.
        // Do not do anything for now => no screenshot for widgets.
        }
    return 0;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::RunL
// --------------------------------------------------------------------------
//
void CHgFswEngine::RunL()
    {
    if ( iStatus != KErrCancel ) // when cancelling the subscribe it completes with KErrCancel
        {
        TInt value = 0;
        if ( iSwiProp.Get( value ) == KErrNone )
            {
            TInt operationStatus( value & Swi::KSwisOperationStatusMask );
            // When an (un)installation has finished change the flag to
            // refresh the app list during the next task list update.
            if ( operationStatus == Swi::ESwisStatusSuccess )
                {
                iAppDataRefreshNeeded = ETrue;
                }
            }
        iSwiProp.Subscribe( iStatus );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CHgFswEngine::DoCancel
// --------------------------------------------------------------------------
//
void CHgFswEngine::DoCancel()
    {
    iSwiProp.Cancel();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::RunError
// --------------------------------------------------------------------------
//
TInt CHgFswEngine::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CHgFswEngine::HandleResourceChange
// callback from appui's HandleResourceChangeL
// --------------------------------------------------------------------------
//
void CHgFswEngine::HandleResourceChange( TInt aType )
    {
    HGLOG_CONTEXT( HandleResourceChange, HGLOG_LOCAL );
    HGLOG_IN();

    if ( iPreviewProvider && aType == KEikDynamicLayoutVariantSwitch )
        {
        SetPreviewParams();
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFswEngine::SetPreviewParams
// --------------------------------------------------------------------------
//
void CHgFswEngine::SetPreviewParams()
    {
    TSize screenSize = iEnv->ScreenDevice()->SizeInPixels();
    iPreviewProvider->SetPreviewParam(
        TSize( screenSize.iWidth / KScreenSizeFactor,
            screenSize.iHeight / KScreenSizeFactor ),
        EColor64K ); // displaymode is ignored
    }

    
// end of file
