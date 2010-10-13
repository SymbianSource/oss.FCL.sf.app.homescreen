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
#include "tscpsnotifier.h"
#include "tsfswdatalist.h"
#include "tsfsalwaysshownapplist.h"
#include "tsfshiddenapplist.h"
#include "tsfswidgetlist.h"
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
const TInt KContentRefreshDelay = 50000; // 0.05 sec

// for screenshots, they are scaled down to (screensize/this_factor).
const TInt KScreenSizeFactor = 2;

const TUid KTsCameraUid = { 0x101F857a };

//close command for widget
const TInt KCloseWidgetCmd(9);
//handle close cmd for s60 widgets
_LIT(KWidgetAppName, "widgetlauncher.exe");
//handle close cmd for CWRT widgets
_LIT(KWidgetAppNameWgt,"wgtwidgetlauncher.exe");

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

    iDataList = CTsFswDataList::NewL(*this);
    iWidgetsSupported = FeatureManager::FeatureSupported( KFeatureIdWebWidgets );

    // get an initial list of tasks
    iDataList->SetAppDataRefreshNeeded();
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
    iCpsWidgetPublisher = CTSCpsNotifier::NewL(*this);
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

    iWgIds.Close();

    iAppArcSession.Close();
    iWsSession.Close();

    for ( TInt i = 0; i < iRotaTasks.Count(); i++ )
        {
        iRotaTasks[i]->Cancel();
        delete iRotaTasks[i];
        }
    iRotaTasks.Close();
    delete iDataList;
    delete iCpsWidgetPublisher;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::FswDataL
// --------------------------------------------------------------------------
//
EXPORT_C const RTsFswArray& CTsFswEngine::FswDataL()
    {
    TSLOG_CONTEXT( FswDataL, TSLOG_LOCAL );
    TSLOG_OUT();
    return iDataList->FswDataL();
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
    else if( !iDataList->IsAlwaysShownAppL( aWgId ) )
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
        const RWidgetInfoArray& arr( iDataList->Widgets()->RunningWidgets() );
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
    const RWidgetInfoArray& arr( iDataList->Widgets()->RunningWidgets() );
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetCommandL( EApaCommandRun );
    TApaAppInfo info;
    iAppArcSession.GetAppInfo( info, arr[aWidgetIndex]->iUid ); // codescanner::accessArrayElementWithoutCheck2 (aWidgetIndex checked in SwitchToAppL())
    cmdLine->SetExecutableNameL( info.iFullName );
    iAppArcSession.StartApp( *cmdLine );
    CleanupStack::PopAndDestroy(cmdLine);
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

    // screenshot taking support - call Register and Unregister when needed
    UpdatePreviewContent();

    // get the foreground app uid and publish it to CFW if different than before
    TRAP_IGNORE( PublishFgAppUidL() );

    // There can be many calls in a row, use a timer to prevent degrading
    // device performance.
    if ( !iUpdateStarter->IsActive() )
        {
        iUpdateStarter->Start( KContentRefreshDelay, 0,
                TCallBack( UpdateStarterCallback, this ) );
        }
    
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
        TBool hidden = wgn->Hidden() || iDataList->HiddenApps()->IsHiddenL(
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
        iDataList->MoveEntryAtStart(newUid.iUid, EFalse);
        TBool change( iDataList->MoveEntryAtStart(newUid.iUid, EFalse) );
        if( change )
            {
            iObserver.FswDataChanged();
            }
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
    TBool changed = iDataList->CollectTasksL();
    TSLOG1_OUT( "change flag = %d", changed );
    return changed;
    }

    
// CTsFswEngine::HiddenAppListUpdated
// Callback from the hidden app list watcher
// --------------------------------------------------------------------------
//
void CTsFswEngine::HiddenAppListUpdated()
    {
    UpdateTaskList();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::CopyBitmapL
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEngine::CopyBitmapL( TInt aFbsHandle, TBool aKeepAspectRatio )
    {
    TSLOG_CONTEXT( CopyBitmapL, TSLOG_LOCAL );
    
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

    TSLOG_OUT();
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

    TUid appUid;
    TInt wgId = iDataList->FindMostTopParentWgId(aWgId);
    if ( wgId == KErrNotFound )
        {
        wgId = aWgId;
        }
    TInt err = iDataList->AppUidForWgId( wgId, appUid );
    TBool exists = iDataList->CheckForWgIdUsage( wgId );
    if ( err || appUid == KTsCameraUid || !exists )
        {
        // Dont't assign screenshot to camera app
        TSLOG0( TSLOG_LOCAL, "Screenshot for camera - ignore" );
        iPreviewProvider->AckPreview(aFbsHandle);
        TSLOG_OUT();
        return;
        }
    
    CFbsBitmap* bmp = 0;
    TRAP( err, bmp = CopyBitmapL( aFbsHandle, EFalse ) );
    iPreviewProvider->AckPreview(aFbsHandle);
    if ( err == KErrNone )
        {
        StoreScreenshot(aWgId, bmp);
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

    RemoveScreenshot(aWgId);
    
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
    
    CFbsBitmap** bmp = iDataList->FindScreenshot(aWgId);
            
    if ( bmp )
        {
        // Rotate bitmap
        TRAP_IGNORE( RotateL( **bmp, aWgId, aClockwise ) );
        // Bitmap in a array is invalid, remove it
        RemoveScreenshot(aWgId);
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
    TInt wgId = iDataList->FindMostTopParentWgId(aWgId);
    if ( wgId == KErrNotFound )
        {
        wgId = aWgId;
        }
    TBool exists = iDataList->CheckForWgIdUsage( wgId );
    if ( aError == KErrNone && exists )
        {
        StoreScreenshot(aWgId, aBitmap);
        }
    else
        {
        // Rotation failed, cleanup bitmap
        delete aBitmap;
        }
    
    TSLOG_OUT();
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
                iDataList->SetAppDataRefreshNeeded();
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
    if( iDataList->Widgets()->RunningWidgets().Count() <= aOffset )
        {
        User::Leave(KErrArgument);
        }
    const CWidgetInfo* widgetInfo(iDataList->Widgets()->RunningWidgets()[aOffset]);
    const TPtrC bundleName(*widgetInfo->iBundleName);
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
    
    if (iDataList->Widgets()->IsCWRTWidget(widgetInfo->iUid))
    	{
        cmdLine->SetExecutableNameL( KWidgetAppNameWgt);
    	}
    else
    	{
        cmdLine->SetExecutableNameL( KWidgetAppName );
    	}
    
    iAppArcSession.StartApp( *cmdLine );
    CleanupStack::PopAndDestroy( cmdLine );
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::StoreScreenshot
// --------------------------------------------------------------------------
//
TBool CTsFswEngine::StoreScreenshot(TInt aWgId, CFbsBitmap* aBmp)
    {
    TSLOG_CONTEXT( StoreScreenshot, TSLOG_LOCAL );
    //iDataList would take ownership
    TBool change = EFalse;
    change = iDataList->StoreScreenshot(aWgId, aBmp);
    if(change)
        {
        iObserver.FswDataChanged();
        }
    TSLOG1_OUT( "Screenshot for  = %d", aWgId );
    return change;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::RemoveScreenshot()
// --------------------------------------------------------------------------
//
void CTsFswEngine::RemoveScreenshot(TInt aWgId)
    {
    TBool change = EFalse;
    change = iDataList->RemoveScreenshot(aWgId);
    if( change )
        {
        iObserver.FswDataChanged();
        }
    }

// --------------------------------------------------------------------------
// CTsFswEngine::HandleWidgetUpdateL()
// --------------------------------------------------------------------------
//
void CTsFswEngine::HandleWidgetUpdateL(TInt aWidgetId, TInt aBitmapHandle)
    {
	TSLOG_CONTEXT( HandleWidgetUpdateL, TSLOG_LOCAL );
    
	TBool contentChanged( iDataList->MoveEntryAtStart(aWidgetId, ETrue) );
    
	CFbsBitmap* bmp = 0;
    if( aBitmapHandle )
    	{
		TRAPD( err, bmp = CopyBitmapL( aBitmapHandle, EFalse ) );
		if ( err == KErrNone )
			{
            contentChanged = StoreScreenshot(aWidgetId, bmp);
			}
		}
	if(!contentChanged)
		{
		iObserver.FswDataChanged();
		}
 
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
