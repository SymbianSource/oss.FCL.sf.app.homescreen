/*
 * ============================================================================
 *  Name        : hgteleportappui.cpp
 *  Part of     : Hg Teleport
 *  Description : AppUi class
 *  Version     : %version: sa1spcx1#55 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 *
 */

#include "hgteleportappui.h"
#include "hgteleportappview.h"
#include "hgteleportapplogging.h"
#include "hgteleportdevicestate.h"
#include "hgteleportuid.hrh"
#include "hgteleportcommands.hrh"
#include <akntitle.h>
#include <hlplch.h>
#include <avkon.rsg>
#include <bautils.h>
#include <AknUtils.h>
#include <hgteleport.rsg>
#include <pslninternalcrkeys.h>
#include <oommonitorsession.h>
#include <hal.h>
#include <hal_data.h>
#include <akntranseffect.h>


// AknCapServer UID, used for P&S category
const TUid KTeleportStateCategory = { 0x10207218 };

// Teleport UI, used as P&S key
const TInt KTeleportStateKey = KHgTeleportAppUidValue;

// Values for Teleport launching P&S
const TInt KTeleportBackgroundValue = 1;
const TInt KTeleportForegroundValue = KTeleportBackgroundValue << 1;
const TInt KTeleportShortAppKeyPressed = KTeleportForegroundValue << 1;
const TInt KTeleportLongAppKeyPressed = KTeleportShortAppKeyPressed << 1;

//short/long App key values 
const TInt KAppKeyTypeShort = 1;
const TInt KAppKeyTypeLong = 2;

//values for checking the OOM
const TInt KMemoryRequestAmountInBytes = 524288;
const TInt KMinMemoryAmountInBytes = 524288;
const TInt KMemoryToBeReservedInBytes = 524288; // 512 KB

// time to wait before sending the task to background
// (must give time to animation)
const TInt KWaitBeforeGoingToBackground = 100000;

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::ConstructL()
// ConstructL is called by the application framework
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::ConstructL()
    {
    HGLOG_CONTEXT( CHgTeleportAppUi::ConstructL, HGLOG_LOCAL );
    HGLOG_IN();

#ifdef _DEBUG
    // create log folder
	_LIT( KLogPath, "c:\\logs\\teleport\\" );
	RFs fs;
	if ( fs.Connect() == KErrNone )
		{
		fs.MkDirAll( KLogPath );
		fs.Close();
		}
#endif

    // Initialise app UI with standard value.
    BaseConstructL( CAknAppUi::EAknEnableSkin | CAknAppUi::EAknEnableMSK );

#ifndef _DEBUG
    // set as system application (in release build) so we never get closed
    iEikonEnv->SetSystem( ETrue );
#endif

    //Initialize effects change observation
    iThemeEffectsEnabledWatcher =
        CHgCenrepListener::NewL( KCRUidThemes, KThemesTransitionEffects, *this );
    CenrepChanged( KThemesTransitionEffects, iThemeEffectsEnabledWatcher->Value() );
    
    // Hide status pane
    StatusPane()->MakeVisible( EFalse );

    // Change CBA if needed, default is non-touch in rss
    if ( AknLayoutUtils::PenEnabled() )
        {
        Cba()->SetCommandSetL( R_HGTELEPORT_CBA_TOUCH );
        }
    Cba()->MakeVisible(EFalse);

    // Create timer
    iGoToBackgroundTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    // Create commonly used instances (device state only?)
    iDeviceState = CHgTeleportDeviceState::NewL();

    // Create UI
    iAppView = CHgTeleportAppView::NewL( ApplicationRect(), *iDeviceState );
    AddToStackL( iAppView );
    
    //Enable effects
    GfxTransEffect::Enable();
    GfxTransEffect::Register(iAppView,TUid::Uid(KHgTeleportAppUidValue));
    GfxTransEffect::SetTransitionObserver(this);

    // Listen for change in the value of the teleport state property.
    iPropListener = new ( ELeave ) CHgPropertyListener(
            KTeleportStateCategory, KTeleportStateKey, *this );

    // Initialise the application task object with the window group id of
    // our application ( so that it represent our app )
    iApplicationTask.SetWgId( iCoeEnv->RootWin().Identifier() );

    // And finally, go to background.
    MoveAppToBackground( AknTransEffect::ENone );

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::CHgTeleportAppUi()
// Perform the first phase of two phase construction
// -----------------------------------------------------------------------------
//
CHgTeleportAppUi::CHgTeleportAppUi(): iForeground( ETrue ),
                                      iApplicationTask( iCoeEnv->WsSession() )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::~CHgTeleportAppUi()
// Destructor
// -----------------------------------------------------------------------------
//
CHgTeleportAppUi::~CHgTeleportAppUi()
    {
    if( GfxTransEffect::IsRegistered( iAppView ) )
        {
        GfxTransEffect::Deregister(iAppView);
        
        }
    GfxTransEffect::SetTransitionObserver(0);
    
    delete iGoToBackgroundTimer;
    delete iPropListener;

    // destroy UI first
    if ( iAppView )
        {
        RemoveFromStack( iAppView );
        delete iAppView;
        }

    delete iDeviceState;
    delete iMemAllocBuf;
    delete iThemeEffectsEnabledWatcher;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::EffectsEnabled
// -----------------------------------------------------------------------------
//
TBool CHgTeleportAppUi::EffectsEnabled() const
    {
    return iEffectsEnabled;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::StartTransion
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::StartTransion( TUint aTransitionType )
    {
    if( !EffectsEnabled() )
        {
        return;
        }
    switch(aTransitionType)
        {
    case AknTransEffect::EApplicationStart:
        StartTransition( aTransitionType, 
                         ETrue, 
                         EFalse, 
                         CAknTransitionUtils::EForceVisible);
        break;
    case AknTransEffect::EApplicationExit:
        StartTransition( aTransitionType, 
                         EFalse, 
                         EFalse, 
                         CAknTransitionUtils::EForceInvisible );
        break;
    case AknTransEffect::EApplicationStartRect:
        StartTransition( aTransitionType, 
                         EFalse, 
                         ETrue, 
                         CAknTransitionUtils::EForceInvisible );
        break;
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::StartTransition
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::StartTransition( TUint aTranstionId,
                                        TBool aVisibility,
                                        TBool aLayers, 
                                        TUint aSubCom )
    {
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
// CHgTeleportAppUi::TransitionFinished
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::TransitionFinished(const CCoeControl* aControl, 
                                          TUint aAction)
    {
    /*if( aControl == iAppView )
        {
		@TODO IMPLEMENT
        }*/
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::HandleCommandL( TInt aCommand )
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
            MoveAppToBackground( AknTransEffect::EApplicationExit );
            break;

        case EHgTeleportCmdHelp:
            {
            MoveAppToBackground( AknTransEffect::EApplicationExit );
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
// CHgTeleportAppUi::HandleForegroundEventL
// Called by the system when the app is moved to foreground or background.
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::HandleForegroundEventL( TBool aForeground )
    {
    HGLOG_CONTEXT( HandleForegroundEventL, HGLOG_LOCAL );
    HGLOG1_IN( "aForeground = %d", aForeground );

    // These calls may be redundant but the functions will do nothing if the
    // state has already been changed.
    // Both this function and the 'manual' MoveAppTo functions must fire the events
    // because in some cases only one of them will run (e.g. when bringing to foreground
    // not with the hw key but by other means etc.)
    if ( aForeground )
        {
        HandleSwitchToForegroundEvent();
        }
    else
        {
        HandleSwitchToBackgroundEvent();
        }

    // Call Base class method
    CAknAppUi::HandleForegroundEventL( aForeground );

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::PropertyChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::PropertyChanged( TUid aCategory, TUint aKey )
    {
    HGLOG_CONTEXT( PropertyChanged, HGLOG_LOCAL );
    HGLOG_IN();

    TInt value( 0 );

    if ( RProperty::Get( aCategory, aKey, value ) == KErrNone )
        {
        if ( iForeground && (value & KTeleportBackgroundValue) )
            {
            MoveAppToBackground( AknTransEffect::EApplicationExit );
            }
        else if ( !iForeground && (value & KTeleportForegroundValue) )
            {
            MoveAppToForeground( AknTransEffect::EApplicationStart );
            }
        else if( value & KTeleportLongAppKeyPressed )
            {
            if(!iForeground)
                {
                MoveAppToBackground( AknTransEffect::EApplicationExit );
                }
            else
                {
                iAppView->HandleAppKey(KAppKeyTypeLong);
                }
            }
        else if(  value & KTeleportShortAppKeyPressed )
            {
            iAppView->HandleAppKey(KAppKeyTypeShort);
            }
        }

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::HandleResourceChangeL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::HandleResourceChangeL( TInt aType )
    {
    // Must call base class implementation first,
    // sizes from LayoutMetricsRect etc. will only be correct after this.
    CAknAppUi::HandleResourceChangeL( aType );
    if( aType == KEikDynamicLayoutVariantSwitch && iAppView )
        {
        iAppView->SetRect( ApplicationRect() );
        }
    // forward event
    iDeviceState->HandleResourceChange( aType );
    iAppView->HandleResourceChange( aType );
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::MoveAppToBackground()
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::MoveAppToBackground( TUint aTransitionType )
    {
    HGLOG_CONTEXT( MoveAppToBackground, HGLOG_LOCAL );
    HGLOG_IN();

    if ( AknTransEffect::ENone == aTransitionType || !EffectsEnabled() )
        {
        GoToBackgroundTimerCallback( this );
        }
    else
        {
        StartTransion(AknTransEffect::EApplicationExit);
        iGoToBackgroundTimer->Cancel();
        iGoToBackgroundTimer->Start( 
                KWaitBeforeGoingToBackground, 
                0,
                TCallBack( GoToBackgroundTimerCallback, this ) );
        }
    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::CenrepChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::CenrepChanged( TUint32 aKey, TInt aNewValue )
    {
    iEffectsEnabled = !(aNewValue & AknTransEffect::EFullScreenTransitionsOff);
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::GoToBackgroundTimerCallback
// -----------------------------------------------------------------------------
//
TInt CHgTeleportAppUi::GoToBackgroundTimerCallback( TAny* aParam )
    {
    CHgTeleportAppUi* self = static_cast<CHgTeleportAppUi*>( aParam );
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
// CHgTeleportAppUi::MoveAppToForeground()
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::MoveAppToForeground( TUint  /*aTransitionType*/ )
    {
    HGLOG_CONTEXT( MoveAppToForeground, HGLOG_LOCAL );
    HGLOG_IN();

    // Request window server to bring our application
    // to foreground
    iApplicationTask.BringToForeground();

    // Notify
    HandleSwitchToForegroundEvent();

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::HandleSwitchToBackgroundEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::HandleSwitchToBackgroundEvent()
    {
    HGLOG_CONTEXT( HandleSwitchToBackgroundEvent, HGLOG_LOCAL );
    HGLOG_IN();

    // must not do anything if iForeground is already up-to-date
    // exclude cases with dialogs like power menu, memory card
    if( iForeground && !IsFaded() )  
        {
        iForeground = EFalse;
        SetTeleportStateProperty( KTeleportBackgroundValue );

        //allocating extra memory space
        if ( !iMemAllocBuf )
            {
            iMemAllocBuf =
                (TUint8*) User::Alloc( KMemoryToBeReservedInBytes );
            }

        // notify view
        iAppView->HandleSwitchToBackgroundEvent();
        }

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::HandleSwitchToForegroundEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::HandleSwitchToForegroundEvent()
    {
    HGLOG_CONTEXT( HandleSwitchToForegroundEvent, HGLOG_LOCAL );
    HGLOG_IN();

    // must not do anything if iForeground is already up-to-date
    if ( !iForeground )
        {
        //freeing extra memory space
        delete iMemAllocBuf;
        iMemAllocBuf = NULL;

        TInt freeRamMemory;
        HAL::Get( HALData::EMemoryRAMFree, freeRamMemory );
        if ( freeRamMemory <= KMinMemoryAmountInBytes )
            {
            FreeMemoryRequest();
            }

        iForeground = ETrue;
        SetTeleportStateProperty( KTeleportForegroundValue );

        // notify view
        iAppView->HandleSwitchToForegroundEvent();
        }

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::SetTeleportShowProperty
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::SetTeleportStateProperty( TInt aValue )
    {
    HGLOG_CONTEXT( CHgTeleportAppUi::SetTeleportShowProperty, HGLOG_LOCAL );
    HGLOG_IN();

    if ( RProperty::Set(
            KTeleportStateCategory, KTeleportStateKey, aValue ) != KErrNone )
        {
        TInt error = RProperty::Define(
                KTeleportStateCategory, KTeleportStateKey, RProperty::EInt );
        if ( error != KErrNone )
            {
            HGLOG1( HGLOG_INFO, "RProperty::Define Error: %d", error );
            }

        error = RProperty::Set( KTeleportStateCategory, KTeleportStateKey, aValue );
        if ( error != KErrNone )
            {
            HGLOG1( HGLOG_INFO, "RProperty::Set Error: %d", error );
            }
        }

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppUi::FreeMemoryRequest
// -----------------------------------------------------------------------------
//
void CHgTeleportAppUi::FreeMemoryRequest()
    {
    HGLOG_CONTEXT( CHgTeleportAppUi::FreeMemoryRequest, HGLOG_LOCAL );
    HGLOG_IN();
    ROomMonitorSession oomMs;
    TInt error = oomMs.Connect();
    if ( error != KErrNone)
        {
        HGLOG1( HGLOG_INFO, "Could not connect to ROomMonitorSession, %d", error );
        }
    else
        {
        oomMs.RequestFreeMemory( KMemoryRequestAmountInBytes );
        oomMs.Close();
        }
    HGLOG_OUT();
    }


// End of file
