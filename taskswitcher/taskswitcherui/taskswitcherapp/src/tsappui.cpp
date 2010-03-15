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
const TInt KWaitBeforeGoingToBackground = 100000;

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

    // Create commonly used instances (device state only?)
    iDeviceState = CTsDeviceState::NewL();

    // Create custom window group
    iWg = RWindowGroup(CCoeEnv::Static()->WsSession());
    iWg.Construct((TUint32)&iWg, ETrue);
    
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
    MoveAppToBackground( ENoneTransition );
    
    iEikonEnv->RootWin().SetOrdinalPosition(-1);
    iEikonEnv->RootWin().EnableReceiptOfFocus(EFalse);
    
    iIsPopUpShown = EFalse;
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppUi::CTsAppUi()
// Perform the first phase of two phase construction
// -----------------------------------------------------------------------------
//
CTsAppUi::CTsAppUi(): iForeground( ETrue ),
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
    if( GfxTransEffect::IsRegistered( iAppView ) )
        {
        GfxTransEffect::Deregister(iAppView);
        
        }
    GfxTransEffect::SetTransitionObserver(0);
    
    delete iGoToBackgroundTimer;
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
void CTsAppUi::StartTransion( TUint aTransitionType )
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
        StartTransition( aTransitionType, 
                         EFalse, 
                         ETrue, 
                         CAknTransitionUtils::EForceInvisible );
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
    TRAP_IGNORE( RequestPopUpL() );
    const TDesC8* ptr = reinterpret_cast<const TDesC8*>(iAppView);
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
// CTsAppUi::TransitionFinished
// -----------------------------------------------------------------------------
//
void CTsAppUi::TransitionFinished(const CCoeControl* aControl, 
                                  TUint /*aAction*/)
    {
    if ( aControl == iAppView )
        {
        TRAP_IGNORE( DisablePopUpL() );
        }
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
    if ( aForeground )
        {
        HandleSwitchToForegroundEvent();
        }
    // exclude cases with dialogs like power menu, memory card
    else if( !IsFaded())
        {
        HandleSwitchToBackgroundEvent();
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
                else
                    {
                    iAppView->HandleAppKey(KAppKeyTypeLong);
                    }
                }
            else if(  value & KTaskswitcherShortAppKeyPressed )
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
    if( aType == KEikDynamicLayoutVariantSwitch && iAppView )
        {
        // Check if layout switch is necessary
        TSizeMode mode = iEikonEnv->ScreenDevice()->GetCurrentScreenModeAttributes();
        TBool isLandscape = mode.iScreenSize.iWidth > mode.iScreenSize.iHeight;
        TRect appRect = ApplicationRect();
        TBool isAppLandscape = appRect.Width() > appRect.Height();
        if(isLandscape != isAppLandscape)
            {
            // Keep displayed orientation
            return;
            }
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
void CTsAppUi::MoveAppToBackground( TUint aTransitionType )
    {
    TSLOG_CONTEXT( MoveAppToBackground, TSLOG_LOCAL );
    TSLOG_IN();

    if ( ENoneTransition == aTransitionType || !EffectsEnabled() )
        {
        GoToBackgroundTimerCallback( this );
        }
    else
        {
        StartTransion(aTransitionType);
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

    // Request window server to bring our application
    // to foreground
    iApplicationTask.BringToForeground();

    // Notify
    HandleSwitchToForegroundEvent();

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

        // notify view
        iAppView->HandleSwitchToBackgroundEvent();
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
    if ( !iForeground )
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

// End of file
