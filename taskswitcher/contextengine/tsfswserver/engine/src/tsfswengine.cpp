/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Task monitor engine
 *
*/


#include "tsfswengine.h"
#include "tsfsalwaysshownapplist.h"
#include "tsfshiddenapplist.h"
#include "tsfswidgetlist.h"
#include "tsfswiconcache.h"
#include "tspreviewprovider.h"
#include "tsfswclient.h"
#include <apgtask.h>
#include <AknDef.h>
#include <apgwgnam.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <featmgr.h>
#include <swi/swispubsubdefs.h>
#include <s32mem.h>
#include <bitmaptransforms.h>
#include "enginelogging.h"

// time to wait before refreshing content
const TInt KContentRefreshDelay = 500000; // 0.5 sec

// for screenshots, they are scaled down to (screensize/this_factor).
const TInt KScreenSizeFactor = 2;

// format to get a lowercase hex string prefixed with 0x
_LIT( KHexFmt, "0x%x" );

// --------------------------------------------------------------------------
// CTsFswEngine::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CTsFswEngine* CTsFswEngine::NewL( MTsFswEngineObserver& aObserver )
    {
    CTsFswEngine* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CTsFswEngine* CTsFswEngine::NewLC( MTsFswEngineObserver& aObserver )
    {
    CTsFswEngine* self = new ( ELeave ) CTsFswEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::CTsFswEngine
// --------------------------------------------------------------------------
//
CTsFswEngine::CTsFswEngine( MTsFswEngineObserver& aObserver )
        : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CTsFswEngine::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswEngine::ConstructL()
    {
    iEnv = CEikonEnv::Static();

    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iAppArcSession.Connect() );

    iHiddenAppList = CTsFsHiddenAppList::NewL( *this );
    iAlwaysShownAppList = CTsFsAlwaysShownAppList::NewL();

    iWidgetsSupported = FeatureManager::FeatureSupported( KFeatureIdWebWidgets );
    if ( iWidgetsSupported )
        {
        iWidgetList = CTsFsWidgetList::NewL();
        }

    // create app icon retriever instance
    iAppIcons = CTsFswIconCache::NewL();

    // get an initial list of tasks
    iAppDataRefreshNeeded = ETrue;
    CollectTasksL();

    // listen for app screenshots
    iPreviewProvider = CTsFastSwapPreviewProvider::NewL( *this );
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
// CTsFswEngine::~CTsFswEngine
// --------------------------------------------------------------------------
//
CTsFswEngine::~CTsFswEngine()
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

    for ( TInt i = 0; i < iRotaTasks.Count(); i++ )
        {
        iRotaTasks[i]->Cancel();
        delete iRotaTasks[i];
        }
    iRotaTasks.Close();
//    delete iContextUtility;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::FswDataL
// --------------------------------------------------------------------------
//
EXPORT_C const RTsFswArray& CTsFswEngine::FswDataL()
    {
    TSLOG_CONTEXT( FswDataL, TSLOG_LOCAL );
    TSLOG1_IN( "dirty flag = %d", iTaskListDirty );

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

    TSLOG_OUT();
    return iData;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::CloseAppL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEngine::CloseAppL( TInt aWgId )
    {
    TSLOG_CONTEXT( CloseAppL, TSLOG_LOCAL );
    TSLOG1_IN( "aWgId = %d", aWgId );

    if( iWidgetsSupported && 0 > aWgId )
        {
        // convert aWgId to an index in the list of running widgets and close widget
        CloseWidgetL( -aWgId -1 );
        }
    else if( !iAlwaysShownAppList->IsAlwaysShownApp( AppUidForWgIdL( aWgId ) ) )
        {
        // send window group event to kill the app
        TWsEvent event;
        event.SetTimeNow();
        event.SetType( KAknShutOrHideApp );
        iEnv->WsSession().SendEventToWindowGroup( aWgId, event );
        TSLOG0( TSLOG_INFO, "event sent to wg" );
        }
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::SwitchToAppL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEngine::SwitchToAppL( TInt aWgId )
    {
    TSLOG_CONTEXT( SwitchToAppL, TSLOG_LOCAL );
    TSLOG1_IN( "aWgId = %d", aWgId );

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
            TSLOG1_OUT( "switched to widget %d", idx );
            return;
            }
        }

    TApaTask task( iEnv->WsSession() );
    task.SetWgId( aWgId );
    task.BringToForeground();

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::SwitchToWidgetL
// --------------------------------------------------------------------------
//
void CTsFswEngine::SwitchToWidgetL( TInt aWidgetIndex )
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
// CTsFswEngine::UpdateTaskList
// Callback from appui
// --------------------------------------------------------------------------
//
void CTsFswEngine::UpdateTaskList()
    {
    TSLOG_CONTEXT( UpdateTaskList, TSLOG_LOCAL );
    TSLOG_IN();

    // If no clients are subscribed for fsw content change notifications
    // then there is no need to waste time with rebuilding the task list,
    // just set the dirty flag.
    TInt listenerCount = iObserver.FswDataListenerCount();
    TSLOG1( TSLOG_INFO, "listener count = %d", listenerCount );
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
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::ForegroundAppUidL
// --------------------------------------------------------------------------
//
EXPORT_C TUid CTsFswEngine::ForegroundAppUidL( TInt aType )
    {
    // Do not use GetFocusWindowGroup or others.
    // We want to see only "real" application uids in FgApp,
    // i.e. stuff that would also be shown in the task swapper
    // area of Taskswitcher.

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
                && aType == CTsFswClient::EUseStandaloneUid )
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
// CTsFswEngine::PublishFgAppUidL
// --------------------------------------------------------------------------
//
void CTsFswEngine::PublishFgAppUidL()
    {
    TSLOG_CONTEXT( PublishFgAppUidL, TSLOG_LOCAL );
    TSLOG_IN();

    TUid newUid = ForegroundAppUidL( CTsFswClient::EUseEmbeddedUid );
    TSLOG2( TSLOG_INFO, "ws says: %x we have: %x", newUid.iUid, iFgAppUid.iUid );
    if ( iFgAppUid != newUid && newUid.iUid )
        {
        iFgAppUid = newUid;
        iFgAppUidStr.Format( KHexFmt, iFgAppUid.iUid );
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::UpdateStarterCallback
// Callback for the timer
// --------------------------------------------------------------------------
//
TInt CTsFswEngine::UpdateStarterCallback( TAny* aParam )
    {
    CTsFswEngine* self = static_cast<CTsFswEngine*>( aParam );
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
// CTsFswEngine::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CTsFswEngine::CollectTasksL()
    {
    TSLOG_CONTEXT( CollectTasksL, TSLOG_LOCAL );
    TSLOG_IN();

    // clear dirty flag
    iTaskListDirty = EFalse;

    TBool changed = EFalse;
    RTsFswArray newList;
    CleanupResetAndDestroyPushL( newList );

    // update app data if needed
    // (usually on startup and when new apps might have been installed)
    if ( iAppDataRefreshNeeded )
        {
        TSLOG0( TSLOG_INFO, "refreshing app data" );
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

        // get screen number (-1=console, 0=main screen, 1=cover ui)
        TInt appScreen = 0;
        TInt scrNumErr = iAppArcSession.GetDefaultScreenNumber( appScreen, appUid );

        // check if it is on always-shown list
        TBool mustShow = iAlwaysShownAppList->IsAlwaysShownApp( appUid );

#ifdef _DEBUG
        const TDesC& captionDbg( windowName->Caption() );
        TSLOG4( TSLOG_INFO, "[%d] wgid=%d appuid=%x (%S)", i, wgId,
            appUid.iUid, &captionDbg );
        TSLOG4( TSLOG_INFO, "hidden=%d onhiddenlist=%d mustshow=%d scrno=%d",
            windowName->Hidden(), onHiddenList, mustShow, appScreen );
#endif

        // if this is the widget app then save wgid for later use
        // and ignore it, but include running widgets instead
        if ( iWidgetsSupported && appUid.iUid == KWidgetAppUidValue )
            {
            changed = ETrue;
            }
        // add item to task list if it is always-shown OR it is not hidden
        // and it is not on cover ui
        else if ( mustShow
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
    CheckWidgetsL(newList);
    
    // if counts for old and new lists do not match then there is a change for sure,
    // probably an app has been closed
    if ( iData.Count() != newList.Count() )
        {
        changed = ETrue;
        }

    // move pointers from the temporary list into the real one
    iData.ResetAndDestroy();
    TInt newListCount = newList.Count();
    TSLOG1( TSLOG_INFO, "new list count = %d", newListCount );
    for ( TInt i = 0; i < newListCount; ++i )
        {
        TSLOG3( TSLOG_INFO, "[%d] %S wgid=%d",
            i, &newList[i]->AppName(), newList[i]->WgId() );
        iData.AppendL( newList[i] );
        newList[i] = 0;
        }
    CleanupStack::PopAndDestroy( &newList );
    
    TSLOG1_OUT( "change flag = %d", changed );
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::AddEntryL
// --------------------------------------------------------------------------
//
TBool CTsFswEngine::AddEntryL( TInt aWgId, const TUid& aAppUid,
        CApaWindowGroupName* aWgName, RTsFswArray& aNewList,
        TBool aIsWidget )
    {
    TBool changed = EFalse;
    CTsFswEntry* entry = CTsFswEntry::NewLC();
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
// CTsFswEngine::CheckIfExistsL
// --------------------------------------------------------------------------
//
TBool CTsFswEngine::CheckIfExistsL( CTsFswEntry& aEntry,
        const TUid& aAppUid,
        TBool& aChanged,
        RTsFswArray& aNewList )
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
            CTsFswEntry* oldEntry = iData[entryIdx];
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
// CTsFswEngine::CheckWidgetsL
// --------------------------------------------------------------------------
//
void CTsFswEngine::CheckWidgetsL( RTsFswArray& aNewList )
    {
    if( iWidgetsSupported )
        {
        iWidgetList->InitializeWidgetListL();
        const RWidgetInfoArray& arr( iWidgetList->RunningWidgets() );
        for ( TInt i = 0, ie = arr.Count(); i != ie; ++i )
            {
            //verify if widget is working in full screen mode
            if( arr[i]->iFileSize )
                {
                // wgid will be a special negative value
                // windowgroupname is not needed here so pass NULL
                AddEntryL( -(i+1), arr[i]->iUid, 0, aNewList, ETrue );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFswEngine::HiddenAppListUpdated
// Callback from the hidden app list watcher
// --------------------------------------------------------------------------
//
void CTsFswEngine::HiddenAppListUpdated()
    {
    UpdateTaskList();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::AppUidForWgIdL
// --------------------------------------------------------------------------
//
TUid CTsFswEngine::AppUidForWgIdL( TInt aWgId )
    {
    CApaWindowGroupName* windowName =
        CApaWindowGroupName::NewLC( iWsSession, aWgId );
    TUid appUid = windowName->AppUid();
    CleanupStack::PopAndDestroy( windowName );
    return appUid;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::FindParentWgId
// --------------------------------------------------------------------------
//
TInt CTsFswEngine::FindParentWgId( TInt aWgId )
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
            if ( info.iId == aWgId && info.iParentId > 0)
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
// CTsFswEngine::FindMostTopParentWgId
// --------------------------------------------------------------------------
//
TInt CTsFswEngine::FindMostTopParentWgId( TInt aWgId )
    {
    TInt parent( KErrNotFound );
    parent = FindParentWgId( aWgId );
    if( parent != KErrNotFound)
        {
        TInt topParent = FindMostTopParentWgId(parent);
        if( topParent != KErrNotFound )
            {
            parent = topParent;
            }
        }
    return parent;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::FindAppNameLC
// --------------------------------------------------------------------------
//
HBufC* CTsFswEngine::FindAppNameLC( CApaWindowGroupName* aWindowName,
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
// CTsFswEngine::CopyBitmapL
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEngine::CopyBitmapL( TInt aFbsHandle, TBool aKeepAspectRatio )
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
// CTsFswEngine::UpdatePreviewContent
// --------------------------------------------------------------------------
//
void CTsFswEngine::UpdatePreviewContent()
    {
    TSLOG_CONTEXT( UpdatePreviewContent, TSLOG_LOCAL );
    TSLOG_IN();

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
                TSLOG1( TSLOG_INFO, "unregister: %d", wgid );
                iPreviewProvider->Unregister( wgid );
                }
            }
        // check topmost window group
        if ( allWgIds.Count() )
            {
            if ( !iWgIds.Count() || allWgIds[0].iId != iWgIds[0] )
                {
                TInt wgid = allWgIds[0].iId;
                TSLOG1( TSLOG_INFO, "registering %d", wgid );
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

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::HandleFswPpApplicationChange
// Callback from CTsFastSwapPreviewProvider
// Note: aWgId is for the window group for which the screenshot is taken,
// it may not be the same as the application's wgid (in case of embedded apps)
// --------------------------------------------------------------------------
//
void CTsFswEngine::HandleFswPpApplicationChange( TInt aWgId, TInt aFbsHandle )
    {
    TSLOG_CONTEXT( HandleFswPpApplicationChange, TSLOG_LOCAL );
    TSLOG2_IN( "aWgId = %d aFbsHandle = %d", aWgId, aFbsHandle );

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

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::HandleFswPpApplicationUnregistered
// Callback from CTsFastSwapPreviewProvider
// --------------------------------------------------------------------------
//
void CTsFswEngine::HandleFswPpApplicationUnregistered( TInt aWgId )
    {
    TSLOG_CONTEXT( HandleFswPpApplicationUnregistered, TSLOG_LOCAL );
    TSLOG1_IN( "aWgId = %d", aWgId );

    CFbsBitmap** bmp = iScreenshots.Find( aWgId );
    if ( bmp )
        {
        delete *bmp;
        iScreenshots.Remove( aWgId );
        AssignScreenshotHandle( aWgId, 0 );
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::HandleFswPpApplicationBitmapRotation
// Callback from CTsFastSwapPreviewProvider
// --------------------------------------------------------------------------
//
void CTsFswEngine::HandleFswPpApplicationBitmapRotation( TInt aWgId, TBool aClockwise )
    {
    TSLOG_CONTEXT( HandleFswPpApplicationBitmapRotation, TSLOG_LOCAL );
    TSLOG1_IN( "aWgId = %d", aWgId );
    
    CFbsBitmap** bmp = iScreenshots.Find( aWgId );
    if ( bmp )
        {
        // Rotate bitmap
        TRAP_IGNORE( RotateL( **bmp, aWgId, aClockwise ) );
        // Bitmap in a array is invalid, remove it
        delete *bmp;
        iScreenshots.Remove( aWgId );
        AssignScreenshotHandle( aWgId, 0 );
        }
    
    TSLOG_OUT();
    }


// --------------------------------------------------------------------------
// CTsFswEngine::RotateL
// Callback from CTsFastSwapPreviewProvider
// --------------------------------------------------------------------------
//
void CTsFswEngine::RotateL( CFbsBitmap& aBitmap, TInt aWgId, TBool aClockwise )
    {
    CFbsBitmap* rotaBitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL( rotaBitmap );
    User::LeaveIfError( rotaBitmap->Duplicate( aBitmap.Handle() ) );
    CTsRotationTask* rotaTask = new (ELeave) CTsRotationTask( *this );
    CleanupStack::PushL( rotaTask );
    User::LeaveIfError( iRotaTasks.Append( rotaTask ) );
    rotaTask->StartLD( aWgId, rotaBitmap, aClockwise ); // ownership transferred
    CleanupStack::Pop( rotaTask );
    CleanupStack::Pop( rotaBitmap );
    }


// --------------------------------------------------------------------------
// CTsFswEngine::RotationComplete
// Callback from CTsFastSwapPreviewProvider
// --------------------------------------------------------------------------
//
void CTsFswEngine::RotationComplete( TInt aWgId,
        CFbsBitmap* aBitmap,
        CTsRotationTask* aCompletedTask,
        TInt aError )
    {
    TSLOG_CONTEXT( RotationComplete, TSLOG_LOCAL );
    TSLOG_IN();
    
    TSLOG1( TSLOG_INFO, "---> rotation completed with status: %d", aError );
    TInt idx = iRotaTasks.Find( aCompletedTask );
    if ( idx != KErrNotFound )
        {
        // Update task list
        iRotaTasks.Remove(idx);
        }
    
    if ( aError == KErrNone )
        {
        if ( iScreenshots.Insert( aWgId, aBitmap ) != KErrNone )
            {
            delete aBitmap;
            iScreenshots.Remove( aWgId );
            AssignScreenshotHandle( aWgId, 0 );
            }
        else
            {
            AssignScreenshotHandle( aWgId, aBitmap->Handle() );
            }
        }
    else
        {
        // Rotation failed, cleanup bitmap
        delete aBitmap;
        }
    
    TSLOG_OUT();
    }


// --------------------------------------------------------------------------
// CTsFswEngine::AssignScreenshotHandle
// Called when a screenshot arrives to check if there is a corresponding
// application in the task list. Firstly try to match screenshot into parental
// application then into standalone one.
// --------------------------------------------------------------------------
//
void CTsFswEngine::AssignScreenshotHandle( TInt aWgIdForScreenshot,
        TInt aBitmapHandle )
    {
    TBool changed = EFalse;
    TInt parentWgId = FindMostTopParentWgId( aWgIdForScreenshot );
    // now parentWgId is a valid wgid or KErrNotFound (-1)
    if (parentWgId != KErrNotFound)
		{
		AssignScreenshotHandle( parentWgId, aBitmapHandle, changed );
		}
    if (!changed)
		{
		AssignScreenshotHandle( aWgIdForScreenshot, aBitmapHandle, changed );
		}
    if ( changed )
        {
        iObserver.FswDataChanged();
        }
    }

// --------------------------------------------------------------------------
// CTsFswEngine::AssignScreenshotHandle
// Called when a screenshot arrives to check if there is a corresponding
// application in the task list. It might be tried to be match into parental 
// or standalone application.
// --------------------------------------------------------------------------
//
void CTsFswEngine::AssignScreenshotHandle(TInt aWgIdForScreenshot,
		TInt aBitmapHandle, TBool& aAsigned)
	{
	aAsigned = EFalse;
	for (TInt i = 0, ie = iData.Count(); i != ie; ++i)
		{
		if (iData[i]->Widget())
			{
			// Do not do anything for now => no screenshot for widgets.
			continue;
			}
		TInt appWgId = iData[i]->WgId();
		if (appWgId == aWgIdForScreenshot)
			{
			iData[i]->SetScreenshotHandle(aBitmapHandle);
			aAsigned = ETrue;
			break;
			}
		}
	}

// --------------------------------------------------------------------------
// CTsFswEngine::LookupScreenshotHandle
// Called to check if there is a screenshot already stored for the given
// app. (or a screenshot with a wgid for which aWgIdForApp is the parent)
// --------------------------------------------------------------------------
//
TInt CTsFswEngine::LookupScreenshotHandle( TInt aWgIdForApp )
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
// CTsFswEngine::RunL
// --------------------------------------------------------------------------
//
void CTsFswEngine::RunL()
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
// CTsFswEngine::DoCancel
// --------------------------------------------------------------------------
//
void CTsFswEngine::DoCancel()
    {
    iSwiProp.Cancel();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::RunError
// --------------------------------------------------------------------------
//
TInt CTsFswEngine::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::HandleResourceChange
// callback from appui's HandleResourceChangeL
// --------------------------------------------------------------------------
//
void CTsFswEngine::HandleResourceChange( TInt aType )
    {
    TSLOG_CONTEXT( HandleResourceChange, TSLOG_LOCAL );
    TSLOG_IN();

    if ( iPreviewProvider && aType == KEikDynamicLayoutVariantSwitch )
        {
        SetPreviewParams();
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::SetPreviewParams
// --------------------------------------------------------------------------
//
void CTsFswEngine::SetPreviewParams()
    {
    TSize screenSize = iEnv->ScreenDevice()->SizeInPixels();
    iPreviewProvider->SetPreviewParam(
        TSize( screenSize.iWidth / KScreenSizeFactor,
            screenSize.iHeight / KScreenSizeFactor ),
        EColor64K ); // displaymode is ignored
    }

// --------------------------------------------------------------------------
// CTsFswEngine::CloseWidgetL
// --------------------------------------------------------------------------
//
void CTsFswEngine::CloseWidgetL(TInt aOffset )
    {
    TSLOG_CONTEXT( CloseWidgetL, TSLOG_LOCAL );
    TSLOG1_IN( "aOffset = %d", aOffset );
    if( iWidgetList->RunningWidgets().Count() <= aOffset )
        {
        User::Leave(KErrArgument);
        }
    const CWidgetInfo* widgetInfo(iWidgetList->RunningWidgets()[aOffset]);
    const TPtrC bundleName(*widgetInfo->iBundleName);
    RApaLsSession ls;
    User::LeaveIfError( ls.Connect() );
    CleanupClosePushL( ls );
    CApaCommandLine* const cmdLine = CApaCommandLine::NewLC();
    
    HBufC8* const
        opaque( HBufC8::NewLC( bundleName.Size() + 3 * sizeof( TUint32 ) ) );
    TPtr8 des ( opaque->Des() );
    RDesWriteStream stream;
    stream.Open( des );
    CleanupClosePushL( stream );
    stream.WriteUint32L ( widgetInfo->iUid.iUid ); 
    stream.WriteUint32L ( bundleName.Length() );
    stream.WriteL ( reinterpret_cast< const TUint8* >( bundleName.Ptr() ), bundleName.Size());
    stream.WriteInt32L ( KCloseWidgetCmd );
    CleanupStack::PopAndDestroy( &stream );
    cmdLine->SetCommandL( EApaCommandBackgroundAndWithoutViews );
    cmdLine->SetOpaqueDataL( *opaque );
    CleanupStack::PopAndDestroy( opaque );
    cmdLine->SetExecutableNameL( KWidgetAppName );
    ls.StartApp( *cmdLine );
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy( &ls );
    TSLOG_OUT();
    }



// --------------------------------------------------------------------------
// CTsRotationListener::CTsRotationListener
// --------------------------------------------------------------------------
//
CTsRotationTask::CTsRotationTask(CTsFswEngine& aEngine )
: CActive(EPriorityStandard),
  iEngine(aEngine)
    {
    CActiveScheduler::Add( this );
    }


// --------------------------------------------------------------------------
// CTsRotationListener::~CTsRotationListener
// --------------------------------------------------------------------------
//
CTsRotationTask::~CTsRotationTask()
    {
    Cancel();
    delete iRotator;
    delete iBitmap;
    }


// --------------------------------------------------------------------------
// CTsRotationListener::Start
// --------------------------------------------------------------------------
//
void CTsRotationTask::StartLD( TInt aWgId,
        CFbsBitmap* aBitmap,
        TBool aClockwise )
    {
    TSLOG_CONTEXT( StartLD, TSLOG_LOCAL );
    TSLOG_IN();
    
    iWgId = aWgId;
    iBitmap = aBitmap;
    iRotator = CBitmapRotator::NewL();
    if ( aClockwise )
        {
        iRotator->Rotate(&iStatus, *iBitmap, CBitmapRotator::ERotation90DegreesClockwise);
        }
    else
        {
        iRotator->Rotate(&iStatus, *iBitmap, CBitmapRotator::ERotation270DegreesClockwise);
        }
    SetActive();
    
    TSLOG_OUT();
    }


// --------------------------------------------------------------------------
// CTsRotationListener::RunL
// --------------------------------------------------------------------------
//
void CTsRotationTask::RunL()
    {
    iEngine.RotationComplete( iWgId, iBitmap, this, iStatus.Int() ); // bitmap ownership transferred
    iBitmap = NULL;
    delete this;
    }


// --------------------------------------------------------------------------
// CTsRotationListener::DoCancel
// --------------------------------------------------------------------------
//
void CTsRotationTask::DoCancel()
    {
    iRotator->Cancel();
    }
    
// end of file
