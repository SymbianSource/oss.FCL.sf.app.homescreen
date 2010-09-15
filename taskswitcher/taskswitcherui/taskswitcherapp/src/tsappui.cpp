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
* Description:  AppUi class
 *
*/


#include "tsappui.h"
#include "tsappview.h"
#include "tsapplogging.h"
#include "tsdevicestate.h"
#include "tsuid.hrh"
#include "tscommands.hrh"
#include <akntitle.h>
#include <hlplch.h>
#include <avkon.rsg>
#include <bautils.h>
#include <AknUtils.h>
#include <taskswitcher.rsg>
#include <pslninternalcrkeys.h>
#include <oommonitorsession.h>
#include <hal.h>
#include <hal_data.h>
#include <akntranseffect.h>
#include <UikonInternalPSKeys.h>
#include <apgwgnam.h>


// AknCapServer UID, used for P&S category
const TUid KTaskswitcherStateCategory = { 0x10207218 };

// Taskswitcher UI, used as P&S key
const TInt KTaskswitcherStateKey = KTsAppUidValue;

const TUid KTransitionsUid = {0x10281F90};

// Values for Taskswitcher launching P&S
const TInt KTaskswitcherBackgroundValue = 1;
const TInt KTaskswitcherForegroundValue = KTaskswitcherBackgroundValue << 1;
const TInt KTaskswitcherShortAppKeyPressed = KTaskswitcherForegroundValue << 1;
const TInt KTaskswitcherLongAppKeyPressed = KTaskswitcherShortAppKeyPressed << 1;

//short/long App key values 
const TInt KAppKeyTypeShort = 1;
const TInt KAppKeyTypeLong = 2;

//values for checking the OOM
const TInt KMemoryRequestAmountInBytes = 524288;
const TInt KMinMemoryAmountInBytes = 524288;

// time to wait before sending the task to background
// (must give time to animation)
const TInt KWaitBeforeGoingToBackground = 200000;

const TUid KTsAppUid = { KTsAppUidValue };

const TUid KTsCameraUid = { 0x101F857a };
const TUid KTsTelephoneUid = { 0x100058b3 };

const TInt KTsMultimediaAppActivationDelay = 1500000;

// -----------------------------------------------------------------------------
// CTsAppUi::ConstructL()
// ConstructL is called by the application framework
// -----------------------------------------------------------------------------
//
void CTsAppUi::ConstructL()
    {
    TSLOG_CONTEXT( CTsAppUi::ConstructL, TSLOG_LOCAL );
    TSLOG_IN();

#ifdef _DEBUG
    // create log folder
	_LIT( KLogPath, "c:\\logs\\taskswitcher\\" );
	RFs fs;
	if ( fs.Connect() == KErrNone )
		{
		fs.MkDirAll( KLogPath );
		fs.Close();
		}
#endif

    // Initialise app UI with standard value.
    BaseConstructL( CAknAppUi::EAknEnableSkin | 
					CAknAppUi::EAknEnableMSK |
					CAknAppUi::EAknSingleClickCompatible);
    SetFullScreenApp(EFalse);

#ifndef _DEBUG
    // set as system application (in release build) so we never get closed
    iEikonEnv->SetSystem( ETrue );
#endif

    //Initialize effects change observation
    iThemeEffectsEnabledWatcher =
        CTsCenrepListener::NewL( KCRUidThemes, KThemesTransitionEffects, *this );
    CenrepChanged( KThemesTransitionEffects, iThemeEffectsEnabledWatcher->Value() );
    
    // Hide status pane
    StatusPane()->MakeVisible( EFalse );

    // Change CBA if needed, default is non-touch in rss
    if ( AknLayoutUtils::PenEnabled() )
        {
        Cba()->SetCommandSetL( R_TS_CBA_TOUCH );
        }
    Cba()->MakeVisible(EFalse);

    // Create timer
    iGoToBackgroundTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iForegroundDelayTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    // Create commonly used instances (device state only?)
    iDeviceState = CTsDeviceState::NewL();

    // Create custom window group
    iWg = RWindowGroup(CCoeEnv::Static()->WsSession());
    iWg.Construct((TUint32)&iWg, ETrue);
    iWg.EnableScreenChangeEvents(); 
    CApaWindowGroupName* rootWgName = CApaWindowGroupName::NewLC( iEikonEnv->WsSession(), iEikonEnv->RootWin().Identifier() );
    rootWgName->SetWindowGroupName( iWg );
    CleanupStack::PopAndDestroy( rootWgName );
    
    // Create UI
    iAppView = CTsAppView::NewL( ApplicationRect(), *iDeviceState, iWg );
    AddToStackL( iAppView );
    
    //Enable effects
    GfxTransEffect::Enable();
    GfxTransEffect::Register(iAppView,KTransitionsUid);
    GfxTransEffect::SetTransitionObserver(this);

    // Listen for change in the value of the ts state property.
    iPropListener = new ( ELeave ) CTsPropertyListener(
            KTaskswitcherStateCategory, KTaskswitcherStateKey, *this );
    
    // Listen for layout changes
    iLayoutListener = new ( ELeave ) CTsPropertyListener(
            KPSUidUikon, KUikLayoutState, *this );

    // Initialise the application task object with the window group id of
    // our application ( so that it represent our app )
    //iApplicationTask.SetWgId( iCoeEnv->RootWin().Identifier() );
    iApplicationTask.SetWgId( iWg.Identifier() );

    // And finally, go to background.
    iForeground = ETrue;
    MoveAppToBackground( ENoneTransition );
    
    iEikonEnv->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNeverAtFront);
    iEikonEnv->RootWin().EnableReceiptOfFocus(EFalse);
    
    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin();
    windowGroup.EnableGroupListChangeEvents();
    
    iIsPopUpShown = EFalse;
    iUiStarted = EFalse;
    iDisableAppKeyHandling = EFalse;
    
    iForegroundDelayed = EFalse;
    iDelayedForegroundInProgress = EFalse;
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::CTsAppUi()
// Perform the first phase of two phase construction
// -----------------------------------------------------------------------------
//
CTsAppUi::CTsAppUi(): iForeground( EFalse ),
                                      iApplicationTask( iCoeEnv->WsSession() )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CTsAppUi::~CTsAppUi()
// Destructor
// -----------------------------------------------------------------------------
//
CTsAppUi::~CTsAppUi()
    {
    if( iAppView && GfxTransEffect::IsRegistered( iAppView ) )
        {
        GfxTransEffect::Deregister(iAppView);
        
        }
    GfxTransEffect::SetTransitionObserver(0);
    
    delete iGoToBackgroundTimer;
    delete iForegroundDelayTimer;
    delete iPropListener;
    delete iLayoutListener;

    // destroy UI first
    if ( iAppView )
        {
        RemoveFromStack( iAppView );
        delete iAppView;
        }

    delete iDeviceState;
    delete iThemeEffectsEnabledWatcher;
    
    iWg.Close();
    }

// -----------------------------------------------------------------------------
// CTsAppView::EffectsEnabled
// -----------------------------------------------------------------------------
//
TBool CTsAppUi::EffectsEnabled() const
    {
    return iEffectsEnabled;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::StartTransion
// -----------------------------------------------------------------------------
//
void CTsAppUi::StartTransion( TUint aTransitionType, TUid aNextAppUid, TInt aWgId )
    {
    if( !EffectsEnabled() )
        {
        return;
        }
    switch(aTransitionType)
        {
    case EForegroundTransition:
        StartTransition( aTransitionType, 
                         ETrue, 
                         EFalse, 
                         CAknTransitionUtils::EForceVisible);
        break;
    case EBackgroundTransition:
        StartTransition( aTransitionType, 
                         EFalse, 
                         EFalse, 
                         CAknTransitionUtils::EForceInvisible );
        break;
    case EActivationTransition:
        StartAppActivateTransition( aNextAppUid, aWgId );
        break;
        }
    }

// -----------------------------------------------------------------------------
// CTsAppUi::StartTransition
// -----------------------------------------------------------------------------
//
void CTsAppUi::StartTransition( TUint aTranstionId,
                                TBool aVisibility,
                                TBool /*aLayers*/, 
                                TUint aSubCom )
    {
    const TDesC8* ptr = reinterpret_cast<const TDesC8*>(iAppView);
    GfxTransEffect::AbortFullScreen();
    GfxTransEffect::Abort(iAppView);
    GfxTransEffect::Begin( iAppView, aTranstionId );
    GfxTransEffect::SetDemarcation( iAppView, iAppView->Rect() );
    GfxTransEffect::NotifyExternalState( ECaptureComponentsBegin, ptr );
    iAppView->MakeVisible( aVisibility );
    CAknTransitionUtils::MakeVisibleSubComponents( 
        iAppView,
        static_cast<CAknTransitionUtils::TMakeVisibleSubComponentsInfo>(aSubCom) );
    GfxTransEffect::NotifyExternalState( ECaptureComponentsEnd, ptr );
    GfxTransEffect::End( iAppView );
    }

// -----------------------------------------------------------------------------
// CTsAppUi::StartAppActivateTransition
// -----------------------------------------------------------------------------
//
void CTsAppUi::StartAppActivateTransition( TUid aNextAppUid, TInt aWgId )
    {
    // Check what type of transition will be trigerred
    if ( aWgId == iUnderAppWgId )
        {
        // App under task switcher was launched
        StartTransition( EBackgroundTransition, 
                         EFalse, 
                         ETrue, 
                         CAknTransitionUtils::EForceInvisible );
        }
    else
        {
        // App start animation
        const TDesC8* ptr = reinterpret_cast<const TDesC8*>(iAppView);
        GfxTransEffect::AbortFullScreen();
        GfxTransEffect::Abort(iAppView);
        TInt groupId = GfxTransEffect::BeginGroup();
        GfxTransEffect::BeginFullScreen(
                 EActivationAppShowTransition, ApplicationRect(),
                 AknTransEffect::EParameterType,
                 AknTransEffect::GfxTransParam( aNextAppUid , KTsAppUid ) );
        GfxTransEffect::Begin( iAppView, EBackgroundTransition );
        GfxTransEffect::SetDemarcation( iAppView, iAppView->Rect() );
        GfxTransEffect::NotifyExternalState( ECaptureComponentsBegin, ptr );
        iAppView->MakeVisible( EFalse );
        CAknTransitionUtils::MakeVisibleSubComponents( 
            iAppView,
            static_cast<CAknTransitionUtils::TMakeVisibleSubComponentsInfo>(CAknTransitionUtils::EForceInvisible) );
        GfxTransEffect::NotifyExternalState( ECaptureComponentsEnd, ptr );
        GfxTransEffect::End( iAppView );
        GfxTransEffect::EndFullScreen();
        GfxTransEffect::EndGroup(groupId);
        }
    }

// -----------------------------------------------------------------------------
// CTsAppUi::TransitionFinished
// -----------------------------------------------------------------------------
//
void CTsAppUi::TransitionFinished(const CCoeControl* /*aControl*/, 
                                  TUint /*aAction*/)
    {
    }

// -----------------------------------------------------------------------------
// CTsAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CTsAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknCmdExit:
        case EEikCmdExit:
            Exit();
            break;

        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
            // RSK => just hide
            MoveAppToBackground( EBackgroundTransition );
            break;

        case ETsCmdHelp:
            {
            MoveAppToBackground( EBackgroundTransition );
            CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
            HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), buf );
            }
            break;

        case EAknSoftkeySelect:
            {
            // select is generated by lsk and msk (see the resource file)
            // Simulate a middle key press to the controls.
            // Note that SimulateKeyEventL must not be used here or else it
            // will end up in an infinite loop.
            TKeyEvent keyEvent;
            keyEvent.iCode = EKeyOK;
            keyEvent.iScanCode = EStdKeyDevice3;
            keyEvent.iModifiers = keyEvent.iRepeats = 0;
            iAppView->OfferKeyEventL( keyEvent, EEventKeyDown );
            iAppView->OfferKeyEventL( keyEvent, EEventKey );
            iAppView->OfferKeyEventL( keyEvent, EEventKeyUp );
            }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CTsAppUi::HandleForegroundEventL
// Called by the system when the app is moved to foreground or background.
// -----------------------------------------------------------------------------
//
void CTsAppUi::HandleForegroundEventL( TBool aForeground )
    {
    TSLOG_CONTEXT( HandleForegroundEventL, TSLOG_LOCAL );
    TSLOG1_IN( "aForeground = %d", aForeground );

    // These calls may be redundant but the functions will do nothing if the
    // state has already been changed.
    // Both this function and the 'manual' MoveAppTo functions must fire the events
    // because in some cases only one of them will run (e.g. when bringing to foreground
    // not with the hw key but by other means etc.)
    iDisableAppKeyHandling = EFalse;
    if ( !iUiStarted )
        {
        // Ignore foreground events if UI is starting
        return;
        }
    if ( aForeground )
        {
        HandleSwitchToForegroundEvent();
        }
    // exclude cases with dialogs like power menu, memory card
    else
        {
        if( !IsFaded() )
            {
            HandleSwitchToBackgroundEvent();
            }
        else
            {
            iDisableAppKeyHandling = ETrue;
            }
        }

    // Call Base class method
    CAknAppUi::HandleForegroundEventL( aForeground );

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::PropertyChanged
// -----------------------------------------------------------------------------
//
void CTsAppUi::PropertyChanged( TUid aCategory, TUint aKey )
    {
    TSLOG_CONTEXT( PropertyChanged, TSLOG_LOCAL );
    TSLOG_IN();

    TInt value( 0 );

    if ( aCategory == KTaskswitcherStateCategory )
        {
        if ( RProperty::Get( aCategory, aKey, value ) == KErrNone )
            {
            if ( iForeground && (value & KTaskswitcherBackgroundValue) )
                {
                MoveAppToBackground( EBackgroundTransition );
                }
            else if ( !iForeground && (value & KTaskswitcherForegroundValue) )
                {
                MoveAppToForeground( EForegroundTransition );
                }
            else if( value & KTaskswitcherLongAppKeyPressed )
                {
                if(!iForeground)
                    {
                    MoveAppToBackground( EBackgroundTransition );
                    }
                else if( !iDisableAppKeyHandling )
                    {
                    iAppView->HandleAppKey(KAppKeyTypeLong);
                    }
                }
            else if(  value & KTaskswitcherShortAppKeyPressed && !iDisableAppKeyHandling )
                {
                iAppView->HandleAppKey(KAppKeyTypeShort);
                }
            }
        }
    else if ( aCategory == KPSUidUikon && iIsPopUpShown )
        {
        TRAP_IGNORE(
            DisablePopUpL();
            HandleResourceChangeL(KEikDynamicLayoutVariantSwitch) );
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::HandleResourceChangeL
// -----------------------------------------------------------------------------
//
void CTsAppUi::HandleResourceChangeL( TInt aType )
    {
    TSLOG_CONTEXT( CTsAppUi::HandleResourceChangeL, TSLOG_LOCAL );
    TSLOG_IN();
    // Must call base class implementation first,
    // sizes from LayoutMetricsRect etc. will only be correct after this.
    CAknAppUi::HandleResourceChangeL( aType );
    if( aType == KEikDynamicLayoutVariantSwitch && iAppView && !LayoutChangeAllowed() )
        {
        // Keep displayed orientation
        return;
        }
    // forward event
    iDeviceState->HandleResourceChange( aType );
    if ( iAppView )
        {
        iAppView->HandleResourceChange( aType );
        }
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::MoveAppToBackground()
// -----------------------------------------------------------------------------
//
void CTsAppUi::MoveAppToBackground( TUint aTransitionType, TUid aAppUid, TInt aWgId )
    {
    TSLOG_CONTEXT( MoveAppToBackground, TSLOG_LOCAL );
    TSLOG_IN();

    if ( ENoneTransition == aTransitionType || !EffectsEnabled() )
        {
        GoToBackgroundTimerCallback( this );
        }
    else
        {
        StartTransion(aTransitionType, aAppUid, aWgId);
        iGoToBackgroundTimer->Cancel();
        iGoToBackgroundTimer->Start( 
                KWaitBeforeGoingToBackground, 
                0,
                TCallBack( GoToBackgroundTimerCallback, this ) );
        }
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::CenrepChanged
// -----------------------------------------------------------------------------
//
void CTsAppUi::CenrepChanged( TUint32 /*aKey*/, TInt aNewValue )
    {
    iEffectsEnabled = !(aNewValue & AknTransEffect::EFullScreenTransitionsOff);
    }

// -----------------------------------------------------------------------------
// CTsAppUi::GoToBackgroundTimerCallback
// -----------------------------------------------------------------------------
//
TInt CTsAppUi::GoToBackgroundTimerCallback( TAny* aParam )
    {
    TSLOG_STATIC_CONTEXT( GoToBackgroundTimerCallback, TSLOG_LOCAL, RThread().Id() );
    TSLOG_IN();
    CTsAppUi* self = static_cast<CTsAppUi*>( aParam );
    if ( self->iGoToBackgroundTimer )
        {
        self->iGoToBackgroundTimer->Cancel();
        }

    // Request window server to bring our application
    // to background
    self->iApplicationTask.SendToBackground();

    // Notify
    self->HandleSwitchToBackgroundEvent();
    TSLOG_OUT();
    return 0;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::DelayedForegroundCallback
// -----------------------------------------------------------------------------
//
TInt CTsAppUi::DelayedForegroundCallback( TAny* aParam )
    {
    CTsAppUi* self = static_cast<CTsAppUi*>( aParam );
    self->iDelayedForegroundInProgress = ETrue;
    
    if ( self->iForegroundDelayTimer )
        {
        self->iForegroundDelayTimer->Cancel();
        }
    self->iForegroundDelayed = EFalse;
    self->HandleSwitchToForegroundEvent();
    
    self->iDelayedForegroundInProgress = EFalse;
    
    return 0;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::MoveAppToForeground()
// -----------------------------------------------------------------------------
//
void CTsAppUi::MoveAppToForeground( TUint  /*aTransitionType*/ )
    {
    TSLOG_CONTEXT( MoveAppToForeground, TSLOG_LOCAL );
    TSLOG_IN();

    TRAP_IGNORE( iUnderAppWgId = WgIdOfUnderlyingAppL(EFalse) );
    
    iUiStarted = ETrue;
    
    // Request window server to bring our application
    // to foreground
    iApplicationTask.BringToForeground();

    // Notify
    TBool delayForeground(EFalse);
    TRAP_IGNORE( delayForeground = IsUnderlyingAppMultimediaL() );
    if ( delayForeground )
        {
        iForegroundDelayed = ETrue;
        iForegroundDelayTimer->Cancel();
        iForegroundDelayTimer->Start( 
                KTsMultimediaAppActivationDelay, 
                0,
                TCallBack( DelayedForegroundCallback, this ) );
        }
    else
        {
        HandleSwitchToForegroundEvent();
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::HandleSwitchToBackgroundEvent
// -----------------------------------------------------------------------------
//
void CTsAppUi::HandleSwitchToBackgroundEvent()
    {
    TSLOG_CONTEXT( HandleSwitchToBackgroundEvent, TSLOG_LOCAL );
    TSLOG_IN();

    // must not do anything if iForeground is already up-to-date

    
    if( iForeground  )  
        {
        iForeground = EFalse;
        SetTaskswitcherStateProperty( KTaskswitcherBackgroundValue );
        
        iAppView->MakeVisible( EFalse );
        // notify view
        iAppView->HandleSwitchToBackgroundEvent();
        
        iWg.SetOrdinalPosition(-1, ECoeWinPriorityNormal);
        
        iForegroundDelayed = EFalse;
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::HandleSwitchToForegroundEvent
// -----------------------------------------------------------------------------
//
void CTsAppUi::HandleSwitchToForegroundEvent()
    {
    TSLOG_CONTEXT( HandleSwitchToForegroundEvent, TSLOG_LOCAL );
    TSLOG_IN();

    // must not do anything if iForeground is already up-to-date
    if ( !iForeground && !iForegroundDelayed )
        {
        TInt freeRamMemory;
        HAL::Get( HALData::EMemoryRAMFree, freeRamMemory );
        if ( freeRamMemory <= KMinMemoryAmountInBytes )
            {
            FreeMemoryRequest();
            }

        iForeground = ETrue;
        SetTaskswitcherStateProperty( KTaskswitcherForegroundValue );

        // notify view
        iAppView->HandleSwitchToForegroundEvent();
        
        iWg.SetOrdinalPosition(iWg.OrdinalPosition(), ECoeWinPriorityAlwaysAtFront);
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::SetTaskswitcherShowProperty
// -----------------------------------------------------------------------------
//
void CTsAppUi::SetTaskswitcherStateProperty( TInt aValue )
    {
    TSLOG_CONTEXT( CTsAppUi::SetTaskswitcherShowProperty, TSLOG_LOCAL );
    TSLOG_IN();

    if ( RProperty::Set(
            KTaskswitcherStateCategory, KTaskswitcherStateKey, aValue ) != KErrNone )
        {
        TInt error = RProperty::Define(
                KTaskswitcherStateCategory, KTaskswitcherStateKey, RProperty::EInt );
        if ( error != KErrNone )
            {
            TSLOG1( TSLOG_INFO, "RProperty::Define Error: %d", error );
            }

        error = RProperty::Set( KTaskswitcherStateCategory, KTaskswitcherStateKey, aValue );
        if ( error != KErrNone )
            {
            TSLOG1( TSLOG_INFO, "RProperty::Set Error: %d", error );
            }
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::FreeMemoryRequest
// -----------------------------------------------------------------------------
//
void CTsAppUi::FreeMemoryRequest()
    {
    TSLOG_CONTEXT( CTsAppUi::FreeMemoryRequest, TSLOG_LOCAL );
    TSLOG_IN();
    ROomMonitorSession oomMs;
    TInt error = oomMs.Connect();
    if ( error != KErrNone)
        {
        TSLOG1( TSLOG_INFO, "Could not connect to ROomMonitorSession, %d", error );
        }
    else
        {
        oomMs.RequestFreeMemory( KMemoryRequestAmountInBytes );
        oomMs.Close();
        }
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsAppUi::RequestPopUpL
// -----------------------------------------------------------------------------
//
void CTsAppUi::RequestPopUpL()
    {
    TSLOG_CONTEXT( CTsAppUi::RequestPopUpL, TSLOG_LOCAL );
    TSLOG_IN();
    if(!iIsPopUpShown)
        {
        iIsPopUpShown = ETrue;
        TSizeMode mode = iEikonEnv->ScreenDevice()->GetCurrentScreenModeAttributes();
        TInt isLandscape = mode.iScreenSize.iWidth > mode.iScreenSize.iHeight;
        SetFullScreenApp(ETrue);
        if(isLandscape)
            {
            SetOrientationL(EAppUiOrientationLandscape);
            }
        else
            {
            SetOrientationL(EAppUiOrientationPortrait);
            }
        SetFullScreenApp(EFalse);
        iEikonEnv->RootWin().SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);
        }
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsAppUi::DisablePopUpL
// -----------------------------------------------------------------------------
//
void CTsAppUi::DisablePopUpL()
    {
    TSLOG_CONTEXT( CTsAppUi::DisablePopUpL, TSLOG_LOCAL );
    TSLOG_IN();
    if(iIsPopUpShown)
        {
        iIsPopUpShown = EFalse;
        iEikonEnv->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNeverAtFront);
        SetOrientationL(EAppUiOrientationAutomatic);
        SetFullScreenApp(EFalse);
        }
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsAppUi::LayoutCanBeChanged
// -----------------------------------------------------------------------------
//
TBool CTsAppUi::LayoutChangeAllowed()
    {
    // Check if layout switch is necessary
    TSizeMode mode = iEikonEnv->ScreenDevice()->GetCurrentScreenModeAttributes();
    TBool isLandscape = mode.iScreenSize.iWidth > mode.iScreenSize.iHeight;
    TRect appRect = ApplicationRect();
    TBool isAppLandscape = appRect.Width() > appRect.Height();
    TBool retVal;
    if(isLandscape != isAppLandscape)
        {
        retVal = EFalse;
        }
    else
        {
        retVal = ETrue;
        }
    return retVal;  
    }


// -----------------------------------------------------------------------------
// CTsAppUi::HandleWsEventL
// -----------------------------------------------------------------------------
//
void CTsAppUi::HandleWsEventL(const TWsEvent& aEvent,
        CCoeControl* aDestination)
    {
    TSLOG_CONTEXT( CTsAppUi::HandleWsEventL, TSLOG_LOCAL );
    CAknAppUi::HandleWsEventL(aEvent, aDestination);
    TInt eventType = aEvent.Type();
    if ( eventType == EEventWindowGroupListChanged )
        {
        TInt wgId = WgIdOfUnderlyingAppL(EFalse); 
        TSLOG2( TSLOG_INFO, "WgIdOfUnderlyingAppL: %d appId: %d",
                wgId, GetAppIdL( wgId ).iUid );
        if ( iForeground &&
             wgId &&
             wgId != iUnderAppWgId &&
             !iAppView->AppCloseInProgress(iUnderAppWgId) &&
             !iAppView->WgOnTaskList(wgId) )
            {
            MoveAppToBackground( ENoneTransition );
            }
        if ( WgIdOfUnderlyingAppL(ETrue) != iUnderAppWgId )
            {
            HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
            }
        iUnderAppWgId = wgId;
        }
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsAppUi::WgIdOfUnderlyingApp
// -----------------------------------------------------------------------------
//
TInt CTsAppUi::WgIdOfUnderlyingAppL( TBool aIgnoreParentChild )
    {
    TSLOG_CONTEXT( CTsAppUi::WgIdOfUnderlyingAppL, TSLOG_LOCAL );
    TInt retVal(0);
    TInt underlyingWg = CheckForUnderlyingHiddenAppsL();
    if ( !underlyingWg )
        {
        TApaTaskList taskList(iEikonEnv->WsSession());
        underlyingWg = taskList.FindByPos(0).WgId();
        if ( !(GetAppIdL(underlyingWg).iUid) )
            {
            TSLOG1 (TSLOG_INFO, "Incorrect  underlying window group: %d.", underlyingWg );
            TSLOG0 (TSLOG_INFO, "No association to appId, nor special handling for underlying hidden apps.Change return value into 0.");
            underlyingWg = 0;
            }
        }
    
    if ( aIgnoreParentChild )
        {
        retVal = underlyingWg;
        }
    else
        {
        TInt parentWg = GetTopParentWg( underlyingWg );
        retVal = parentWg ? parentWg : underlyingWg;
        }
    TSLOG_OUT();
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::GetTopParentWg
// -----------------------------------------------------------------------------
//
TInt CTsAppUi::GetTopParentWg( TInt aChildWg )
	{
	TInt parentWg = GetParentWg( aChildWg );
	if( parentWg )
		{
		TInt topParentWg = GetTopParentWg( parentWg );
		if( topParentWg )
			{
			parentWg = topParentWg; 
			}
		}
	return parentWg;
	}

// -----------------------------------------------------------------------------
// CTsAppUi::GetParentWg
// -----------------------------------------------------------------------------
//
TInt CTsAppUi::GetParentWg( TInt aChildWg )
    {
	TInt retVal(0);
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    // Ask for window group list from RWsSession
    TInt error = iEikonEnv->WsSession().WindowGroupList( 0, &allWgIds );
    if ( !error )
        {
        TInt count( allWgIds.Count() );
        for ( TInt i( 0 ); i < count; i++ )
            {
            RWsSession::TWindowGroupChainInfo info = allWgIds[i];
            if ( info.iId == aChildWg && info.iParentId > 0)
                {
				retVal = info.iParentId;
                break;
                }
            }
        }
    allWgIds.Close();
	return retVal;
	}

// -----------------------------------------------------------------------------
// CTsAppUi::IsForeground
// -----------------------------------------------------------------------------
//
TBool CTsAppUi::IsForeground() const
    {
    return iForeground;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::CheckForUnderlyingCameraL
// -----------------------------------------------------------------------------
//
TInt CTsAppUi::CheckForUnderlyingHiddenAppsL()
    {
    TInt wgId(0);
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL(allWgIds);
    User::LeaveIfError(iEikonEnv->WsSession().WindowGroupList(0, &allWgIds));
    TInt underlyingWg(allWgIds[0].iId);
    CleanupStack::PopAndDestroy(&allWgIds);
    
    TUid appUid = GetAppIdL( underlyingWg );
    if( appUid == KTsCameraUid ||
        appUid == KTsTelephoneUid )
        {
        wgId = underlyingWg;
        }
    return wgId;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::IsUnderlyingAppMultimediaL
// -----------------------------------------------------------------------------
//
TBool CTsAppUi::IsUnderlyingAppMultimediaL()
    {
    TBool retVal(EFalse);
    
    TApaTaskList taskList( iEikonEnv->WsSession() );
    TInt underlyingWg = taskList.FindByPos(0).WgId();
    
    TUid appUid = GetAppIdL( underlyingWg );
    if ( appUid == KTsCameraUid )
        {
        retVal = ETrue;
        }
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTsAppUi::DelayedForegroundLaunched
// -----------------------------------------------------------------------------
//
TBool CTsAppUi::DelayedForegroundLaunched()
    {
    return iDelayedForegroundInProgress;
    }

// -----------------------------------------------------------------------------
// CTsAppUi::GetAppIdL
// -----------------------------------------------------------------------------
//
TUid CTsAppUi::GetAppIdL( TInt aWgId )
    {
    TUid retVal;
    CApaWindowGroupName* windowName =
        CApaWindowGroupName::NewLC( iEikonEnv->WsSession(), aWgId );
    retVal = windowName->AppUid();
    CleanupStack::PopAndDestroy( windowName );
    return retVal;
    }

// End of file
