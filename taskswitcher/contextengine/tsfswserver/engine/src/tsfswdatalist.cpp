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
* Description:  File containing application list classes
 *
*/


//INCLUDES:
#include "tsfswdatalist.h"
#include "tsfswengine.h"
#include "tsfsalwaysshownapplist.h"
#include "tsfshiddenapplist.h"
#include "tsfswidgetlist.h"
#include "tsfswiconcache.h"
#include "enginelogging.h"
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <featmgr.h>
#include <apgwgnam.h>

// Special case: ovi store secure widget UID
const TUid KTsOviStoreSecureWidgetUid = { 0x2001A93E };

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CTsFswDataList::NewL
// --------------------------------------------------------------------------
//
CTsFswDataList* CTsFswDataList::NewL(CTsFswEngine& aEngine)
    {
    CTsFswDataList* self = new (ELeave) CTsFswDataList(aEngine);
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::CTsFswDataList(CTsFswEngine& aEngine) :
    iEngine(aEngine)
    {
    }

// --------------------------------------------------------------------------
// CTsFswDataList::~CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::~CTsFswDataList( )
    {
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

    delete iHiddenAppList;
    delete iAlwaysShownAppList;
    delete iWidgetList;
    iAppArcSession.Close();
    iWsSession.Close();
    }

// --------------------------------------------------------------------------
// CTsFswDataList::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswDataList::ConstructL( )
    {
    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iAppArcSession.Connect() );
    iHiddenAppList = CTsFsHiddenAppList::NewL( iEngine );
    iAlwaysShownAppList = CTsFsAlwaysShownAppList::NewL();

    iWidgetsSupported = FeatureManager::FeatureSupported( KFeatureIdWebWidgets );
    if ( iWidgetsSupported )
        {
        iWidgetList = CTsFsWidgetList::NewL();
        }

    // create app icon retriever instance
    iAppIcons = CTsFswIconCache::NewL();
    }

// --------------------------------------------------------------------------
// CTsFswDataList::FswDataL
// --------------------------------------------------------------------------
//
const RTsFswArray& CTsFswDataList::FswDataL()
    {
    TSLOG_CONTEXT( FswDataL, TSLOG_LOCAL );
    TSLOG1_IN( "dirty flag = %d", iTaskListDirty );

    // check the dirty flag and refresh if needed
    if ( iTaskListDirty )
        {
        CollectTasksL();
        // clear dirty flag
		SetDirty(EFalse);
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
// CTsFswDataList::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::CollectTasksL()
    {
    // clear dirty flag
    TBool changed = iTaskListDirty;
    
    RTsFswArray newAppsList;
    RTsFswArray newWidgetsList;
    
    CleanupResetAndDestroyPushL(newAppsList);
    CleanupResetAndDestroyPushL(newWidgetsList);
    
    CollectAppsL(newAppsList);
    CollectWidgetsL(newWidgetsList);
    
    changed |= FitDataToListL(newAppsList, EFalse);
    changed |= FitDataToListL(newWidgetsList, ETrue);
    
    CleanupStack::PopAndDestroy( &newWidgetsList );
    CleanupStack::PopAndDestroy( &newAppsList );
    
    return changed;
    }


// --------------------------------------------------------------------------
// CTsFswDataList::AlwaysShown()
// --------------------------------------------------------------------------
//
CTsFsAlwaysShownAppList* CTsFswDataList::AlwaysShownApps()
    {
    return iAlwaysShownAppList;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::HiddenApps()
// --------------------------------------------------------------------------
//
CTsFsHiddenAppList* CTsFswDataList::HiddenApps()
    {
    return iHiddenAppList;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::Widgets()
// --------------------------------------------------------------------------
//
CTsFsWidgetList* CTsFswDataList::Widgets()
    {
    return iWidgetList;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::SetDirty()
// --------------------------------------------------------------------------
//
void CTsFswDataList::SetDirty(TBool aDirty)
    {
    iTaskListDirty = aDirty;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::SetAppDataRefreshNeeded()
// --------------------------------------------------------------------------
//
void CTsFswDataList::SetAppDataRefreshNeeded()
    {
    iAppDataRefreshNeeded = ETrue;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::ForwardScreenshot
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::StoreScreenshot(TInt aId, CFbsBitmap* aBmp)
    {
    TBool changed = EFalse;
    CFbsBitmap** oldbmp = iScreenshots.Find( aId );
    if ( oldbmp )
        {
        delete *oldbmp;
        }
    if ( iScreenshots.Insert( aId, aBmp ) != KErrNone )
        {
        delete aBmp;
        iScreenshots.Remove( aId );
        }
    else
        {
        changed = AssignScreenshotHandle( aId, aBmp->Handle() );
        }
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::RemoveScreenshotByWgId
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::RemoveScreenshot(TInt aId)
    {
    TBool changed = EFalse;
    CFbsBitmap** bmp = iScreenshots.Find( aId );

    if ( bmp )
        {
        changed |= AssignScreenshotHandle( aId, 0 );
        changed |= RemoveScreenshotFromParent( (*bmp)->Handle());
        delete *bmp;
        iScreenshots.Remove( aId );
        }
    return changed;
    }



// --------------------------------------------------------------------------
// CTsFswDataList::FindScreenshotByWgId
// --------------------------------------------------------------------------
//
CFbsBitmap** CTsFswDataList::FindScreenshot(TInt aId)
    {
    CFbsBitmap** result = 0;
	result = iScreenshots.Find( aId );
    return result; 
    }

// --------------------------------------------------------------------------
// CTsFswDataList::MoveEntryAtStart
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::MoveEntryAtStart(TInt aAppId, TBool aWidget)
    {
    TSLOG_CONTEXT( MoveEntryAtStart, TSLOG_LOCAL );
    
    TBool wasMoved(EFalse);
    TInt appId(0);
    //check embeded case
    if( !aWidget )
    	{
		TInt wgId(0);
		CApaWindowGroupName::FindByAppUid(TUid::Uid(aAppId), iWsSession, wgId);
		TInt parentWgId = FindMostTopParentWgId( wgId );
		if( parentWgId != KErrNotFound )
			{
			TUid appUid = TUid::Uid(0);
			AppUidForWgId(parentWgId, appUid); 
			appId = appUid.iUid;
			}
    	}
    
    if( !appId )
    	{
		appId = aAppId;
    	}
 
    for ( TInt i = 0; i < iData.Count(); ++i )
        {
        if( iData[i]->AppUid().iUid == appId &&
            ((iData[i]->Widget() == aWidget) || (iData[i]->AppUid() == KTsOviStoreSecureWidgetUid )))
            {
            CTsFswEntry* entry = iData[i];
            iData.Remove(i);
            iData.Insert(entry, 0);
            SetDirty(ETrue);
            wasMoved = ETrue;
            break;
            }
        }
    TSLOG_OUT();
    return wasMoved;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::
// --------------------------------------------------------------------------
//
void CTsFswDataList::CollectAppsL(RTsFswArray& aAppsList)
    {
    TSLOG_CONTEXT( CollectTasksL, TSLOG_LOCAL );
    TSLOG_IN();
    
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
        if ( iWidgetsSupported && appUid.iUid == KWidgetAppUidValue )
            {
            //continue, leave widgetui, do nothing
            }
        // add item to task list if it is always-shown OR it is not hidden
        // and it is not on cover ui
        else if (mustShow || (!onHiddenList && !windowName->Hidden() && (appScreen
                == 0 || appScreen == -1) && scrNumErr == KErrNone))
            {
            AddEntryL(wgId, appUid, windowName, aAppsList, EFalse);
            }
        CleanupStack::PopAndDestroy( windowName );
        }
    CleanupStack::PopAndDestroy( &allWgIds );
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AddEntryL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::AddEntryL( TInt aWgId, const TUid& aAppUid,
        CApaWindowGroupName* aWgName, RTsFswArray& aNewList,
        TBool aIsWidget )
    {
    TBool changed = EFalse;
    CTsFswEntry* entry = CTsFswEntry::NewLC();
    entry->SetWgId( aWgId );
    entry->SetAppUid( aAppUid );
    entry->SetWidget( aIsWidget );

    // check if present in old list and if yes then take some of the old data
    TBool found = ConsiderOldDataL( *entry, aAppUid, changed, aNewList );

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

        entry->SetAlwaysShown( iAlwaysShownAppList->IsAlwaysShownApp( aAppUid ) );
        if ( aWgName )
            {
            entry->SetSystemApp( aWgName->IsSystem() );
            }
        }
    TInt h = LookupScreenshotHandle( aWgId );
    if ( h )
        {
        entry->SetScreenshotHandle( h );
        }
    // add to new list, ownership is transferred
    aNewList.AppendL( entry );
    CleanupStack::Pop( entry );
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::ConsiderOldDataL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::ConsiderOldDataL( CTsFswEntry& aEntry,
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
            iData[entryIdx]->SetWgId( aEntry.WgId());
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
// CTsFswDataList::CollectWidgetsL
// --------------------------------------------------------------------------
//
void CTsFswDataList::CollectWidgetsL(RTsFswArray& aWidgetsList)
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
                AddEntryL( -(i+1), arr[i]->iUid, 0, aWidgetsList, ETrue );
                }
            }
        }
	}

// --------------------------------------------------------------------------
// CTsFswDataList::FindParentWgId
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::FindParentWgId( TInt aWgId )
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
// CTsFswDataList::FindMostTopParentWgId
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::FindMostTopParentWgId( TInt aWgId )
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
// CTsFswDataList::FindAppNameLC
// --------------------------------------------------------------------------
//
HBufC* CTsFswDataList::FindAppNameLC( CApaWindowGroupName* aWindowName,
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
// CTsFswDataList::AssignScreenshotHandle
// Called when a screenshot arrives to check if there is a corresponding
// application in the task list. Firstly try to match screenshot into parental
// application then into standalone one.
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::AssignScreenshotHandle( TInt aWgIdForScreenshot,
        TInt aBitmapHandle )
    {
//            aWgIdForScreenshot, aBitmapHandle);
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
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AssignScreenshotHandle
// Called when a screenshot arrives to check if there is a corresponding
// application in the task list. It might be tried to be match into parental 
// or standalone application.
// --------------------------------------------------------------------------
//
void CTsFswDataList::AssignScreenshotHandle(TInt aWgIdForScreenshot,
        TInt aBitmapHandle, TBool& aAsigned)
    {
    aAsigned = EFalse;
    for (TInt i = 0, ie = iData.Count(); i != ie; ++i)
        {
        if (iData[i]->Widget() || iData[i]->AppUid() == KTsOviStoreSecureWidgetUid )
            {
            TInt widgetId = iData[i]->AppUid().iUid;
            if (widgetId == aWgIdForScreenshot)
                {
                iData[i]->SetScreenshotHandle(aBitmapHandle);
                aAsigned = ETrue;
                SetDirty(ETrue);
                break;
                }  
			continue;          
            }
        TInt appWgId = iData[i]->WgId();
        if (appWgId == aWgIdForScreenshot)
            {
            iData[i]->SetScreenshotHandle(aBitmapHandle);
            aAsigned = ETrue;
            SetDirty(ETrue);          
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFswDataList::LookupScreenshotHandle
// Called to check if there is a screenshot already stored for the given
// app. (or a screenshot with a wgid for which aWgIdForApp is the parent)
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::LookupScreenshotHandle( TInt aWgIdForApp )
    {
    CFbsBitmap** bmp = NULL;

    if ( aWgIdForApp > 0 )
        {
        // must check if there is a screenshot for which aWgIdForApp is the parent
        THashMapIter<TInt, CFbsBitmap*> iter( iScreenshots );
        while ( const TInt* wgid = iter.NextKey() )
            {
            if ( FindMostTopParentWgId( *wgid ) == aWgIdForApp )
                {
                THashMapIter<TInt, CFbsBitmap*> iter2( iScreenshots );
                TBool hasChild = false;
                while ( const TInt* wgid2 = iter2.NextKey() )
                    {
                    if ( FindParentWgId( *wgid2 ) == *wgid )
                        {
                        hasChild = true;
                        break;
                        }
                    }
                if ( !hasChild )
                    {
                    bmp = iter.CurrentValue();
                    if ( bmp )
                        {
                        return (*bmp)->Handle();
                        }
                    }
                }
            }
        if ( !bmp )
            {
            bmp = iScreenshots.Find( aWgIdForApp );
            if ( bmp )
                {
                // there is a screenshot stored for this wgid
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
// CTsFswDataList::FitDataToListL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::FitDataToListL(RTsFswArray& aListToFit,
        TBool aConsiderWidgets)
    {
    TSLOG_CONTEXT( FitDataToListL, TSLOG_LOCAL );
    TBool changed = EFalse;
    TInt listCount = aListToFit.Count();
    TInt dataCount = iData.Count();
    
    //remove items that dont't exists in newly collected list
    //consider widget and non-widget type
    for (TInt i = dataCount -1; i >= 0; --i)
        {
        if ( (iData[i]->Widget() == aConsiderWidgets) &&
            !CheckIfExists(*iData[i], aListToFit) )
            {
            if( aConsiderWidgets )
                {
                RemoveScreenshot( iData[i]->AppUid().iUid );
                }
            delete iData[i];
            iData.Remove(i);
            changed = ETrue;
            }
        }
    
    //add new item at start
    for (TInt i = 0; i < aListToFit.Count(); ++i)
        {
        if (!CheckIfExists(*aListToFit[i], iData))
            {
            iData.Insert(aListToFit[i], 0);
            aListToFit[i] = 0;
            changed = ETrue;
            }
        }
    
    TSLOG1_OUT( "change flag = %d", changed );
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::CheckIfExists
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::CheckIfExists(const CTsFswEntry& aEntry,
        const RTsFswArray& aList) const
    {
    TSLOG_CONTEXT( FitDataToListL, TSLOG_LOCAL );
    TBool exists(EFalse);
    TInt dataCount = aList.Count();
    for (TInt entryIdx = 0; entryIdx < dataCount; ++entryIdx)
        {
        if (aList[entryIdx]->AppUid() == aEntry.AppUid())
            {
            exists = ETrue;
            break;
            }
        }
    TSLOG_OUT();
    return exists;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AppUidForWgIdL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::IsAlwaysShownAppL( TInt aWgId )
	{
	return iAlwaysShownAppList->IsAlwaysShownApp( AppUidForWgIdL(aWgId) );
	}

// --------------------------------------------------------------------------
// CTsFswDataList::AppUidForWgId
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::AppUidForWgId( TInt aWgId, TUid& aUid )
    {
	TRAPD(err, aUid = AppUidForWgIdL( aWgId ) );
	return err;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AppUidForWgIdL
// --------------------------------------------------------------------------
//
TUid CTsFswDataList::AppUidForWgIdL( TInt aWgId )
    {
    CApaWindowGroupName* windowName =
        CApaWindowGroupName::NewLC( iWsSession, aWgId );
    TUid appUid = windowName->AppUid();
    CleanupStack::PopAndDestroy( windowName );
    return appUid;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::RemoveScreenshotFromParent
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::RemoveScreenshotFromParent( TInt aBmpHandle )
    {
    TBool changed(EFalse);
    for( TInt i = 0; i < iData.Count(); ++i)
        {
        if ( !iData[i]->Widget() && iData[i]->ScreenshotHandle() == aBmpHandle )
            {
            changed = ETrue;
            iData[i]->SetScreenshotHandle(
                    LookupScreenshotHandle( iData[i]->WgId()) );
            }
        }
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::RemoveScreenshotFromParent
// --------------------------------------------------------------------------
//
void CTsFswDataList::RemoveHiddenAppsScrenshotsL()
    {
    RArray<TInt> hiddenWgIds;
    THashMapIter<TInt, CFbsBitmap*> iter( iScreenshots );
    while ( const TInt* wgIdkey = iter.NextKey() )
        {
        TInt wgId = *wgIdkey;
        TBool hidden = HiddenApps()->IsHiddenL( AppUidForWgIdL( wgId),
                iWsSession, wgId );
        if( hidden )
            {
            hiddenWgIds.Append( wgId );
            }
        }
    for ( TInt i = 0; i < hiddenWgIds.Count(); i++)
        {
        RemoveScreenshot( hiddenWgIds[i] );
        }
    hiddenWgIds.Reset();
    }

// end of file
