/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#include "appui.h"
#include "ainativeui.hrh"
#include "ainativeuistrings.h" // string literals
#include "ainativeuiview.h"
#include "application.h"
#include "nativeuicontroller.h"
#include "aiuiframeworkobserver.h"
#include "aifweventhandler.h"
#include <AiNativeUi.rsg>

#include "aipropertyextension.h"
#include "aicontentmodel.h"
#include "aiuiidleintegration.h"
#include "activeidle2domaincrkeys.h"
#include "activeidle2domainpskeys.h"
#include "extrstandbycontainerif.h"
#include "debug.h"

#include <startupdomainpskeys.h>
#include <aipspropertyobserver.h>
#include <centralrepository.h>
#include <e32base.h>
#include <e32property.h>
#include <avkon.rsg>                    // for status pane layout resource ids
#include <eikon.hrh>
#include <bautils.h>
#include <ecom/implementationproxy.h>
#include <data_caging_path_literals.hrh>
#include <ConeResLoader.h>
#include <featmgr.h>
#include <avkondomainpskeys.h>
#include <AknUtils.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

namespace
    {
    _LIT( KResourceDrive, "Z:" );
    _LIT( KResourceFile, "ainativeui.rsc" );
    #define KResourcePath KDC_APP_RESOURCE_DIR

    const TInt KDefaultStatusPaneLayout = R_AVKON_STATUS_PANE_LAYOUT_IDLE;

    //
    // Get status pane configuration from cenrep
    //
    TInt GetLayoutCenrep()
        {
        TInt value = EAiStatusPaneLayoutIdleNormal;
        TUid uid = { KCRUidActiveIdleLV };
        CRepository* cenRep = CRepository::NewLC( uid );
        cenRep->Get( KAiStatusPaneLayout, value );
        CleanupStack::PopAndDestroy( cenRep );
        return value;
        }

    //
    // Resolves status pane layout for CXnAppUiAdapter::ConstructL()
    //
    TInt StatusPaneLayoutResourceIdL( TInt aLayoutId )
        {
        TInt layout = aLayoutId;
        switch( aLayoutId )
            {
            case EAiStatusPaneLayoutIdleNormal:
                {
                layout = R_AVKON_STATUS_PANE_LAYOUT_IDLE;
                break;
                }
            case EAiStatusPaneLayoutIdleFlat:
                {
                layout = R_AVKON_STATUS_PANE_LAYOUT_IDLE_FLAT;
                break;
                }
            case EAiStatusPaneLayoutIdleHidden:
                {
                layout = KDefaultStatusPaneLayout;
                break;
                }
            default:
                {
                break;
                }
            }
        return layout;
        }
    
    // -----------------------------------------------------------------------------
    // 
    // Calculate touch sensitive softkey area for Top button (landscape)
    // -----------------------------------------------------------------------------         
    TRect SoftkeyRectTopTouch()
        {
        TRect screen;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

        TAknLayoutRect layoutRect;

        TAknWindowComponentLayout rightAreaLayout(
                AknLayoutScalable_Avkon::area_side_right_pane( 0 ) );


        layoutRect.LayoutRect( screen, TAknWindowComponentLayout::Compose( rightAreaLayout,
                AknLayoutScalable_Avkon::sctrl_sk_top_pane() ).LayoutLine() );
        TRect topSKRect( layoutRect.Rect() );
        
        layoutRect.LayoutRect( topSKRect, 
                        AknLayoutScalable_Avkon::aid_touch_sctrl_top().LayoutLine() );
        return layoutRect.Rect();
        }

    // -----------------------------------------------------------------------------
    // 
    // Calculate touch sensitive softkey area for right button (portrait)
    // -----------------------------------------------------------------------------         
    TRect SoftkeyRectRightTouch()
        {
        TRect screen;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

        TAknLayoutRect layoutRect;

        TAknWindowComponentLayout bottomAreaLayout(
                AknLayoutScalable_Avkon::area_bottom_pane( 0 ) );


        layoutRect.LayoutRect( screen, TAknWindowComponentLayout::Compose( bottomAreaLayout,
                AknLayoutScalable_Avkon::control_bg_pane() ).LayoutLine() );
        TRect rightSKRect( layoutRect.Rect() );
        layoutRect.LayoutRect(
                rightSKRect,
                AknLayoutScalable_Avkon::aid_touch_ctrl_right().LayoutLine() );
        return layoutRect.Rect();
        }

    
    }

using namespace AiNativeUiController;

// ========== MEMBER FUNCTIONS ================================================

CAppUi::CAppUi(CNativeUiController* aUiCtl)
    : iUiCtl(aUiCtl)
    {
    }

void CAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin|EAknEnableMSK/*|ENoAppResourceFile*/);

    // Disable CCoeEnv exit checks.
    // Active Idle Framework will perform the checks.
    iCoeEnv->DisableExitChecks(ETrue);

    if( !iResourceLoader )
        {
        iResourceLoader = new(ELeave) RConeResourceLoader( *iCoeEnv );

        // 1) Load resources file for resource publishing
        TFullName resourceFile( KResourceDrive );
        resourceFile.Append( KResourcePath );
        resourceFile.Append( KResourceFile );
        BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), resourceFile );

        iResourceLoader->OpenL( resourceFile );
        }

    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    if( !iUiCtl )
        {
        User::Leave(KErrNotFound);
        return; // for lint
        }

    iFwEventHandler = iUiCtl->FwEventHandler();

    iUiFwObserver = iUiCtl->UiFrameworkObserver();

    TAiIdleKeySoundConfig keySoundConfig;
    keySoundConfig.iKeySounds = KeySounds();
    keySoundConfig.iContextResId = R_NATIVEUI_DEFAULT_SKEY_LIST;
    iIdleIntegration = CAiUiIdleIntegration::NewL
        ( *iEikonEnv, keySoundConfig, iFwEventHandler );

    iUiCtl->SetAppUi( this );

    if( iFwEventHandler )
        {
        iFwEventHandler->AppEnvReadyL();
        }
    TBool isFullScreen = EFalse;
    //Get pointer to status pane
    CEikStatusPane* statusPane = static_cast<CAknAppUi*>( iEikonEnv->EikAppUi() )->StatusPane();
    if( statusPane )
        {
        // Update status pane layout ot the correct one.
        TInt statusPaneLayout = GetLayoutCenrep();
        iCurrentStatusPaneLayout = StatusPaneLayoutResourceIdL( statusPaneLayout );

        TInt id = statusPane->CurrentLayoutResId();
        if( id != iCurrentStatusPaneLayout &&
            id != R_AVKON_STACON_PANE_LAYOUT_IDLE_SOFTKEYS_RIGHT )
            {
            statusPane->SwitchLayoutL( iCurrentStatusPaneLayout );
            }
        else
            {
            iCurrentStatusPaneLayout = id;
            }

        if( statusPaneLayout == EAiStatusPaneLayoutIdleHidden &&
            iCurrentStatusPaneLayout != R_AVKON_STACON_PANE_LAYOUT_IDLE_SOFTKEYS_RIGHT  )
            {
            statusPane->MakeVisible( EFalse );
            isFullScreen = ETrue;
            }
        else
            {
            statusPane->MakeVisible( ETrue );
            }
        statusPane->ApplyCurrentSettingsL();
        statusPane->DrawNow();
        }

    iAppView = CAiNativeUiView::NewL( ClientRect(), this );
    iAppView->SetMopParent(this);
    RegisterViewL( *iAppView );
    AddToStackL( iAppView );
    
    // Reset softkey locked indications. Error may be ignored - not defined would be typical.
    RProperty::Define( KPSUidAiInformation, KActiveIdleExtHS_LSKLocked, RProperty::EText );
    RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_LSKLocked, KNullDesC );
    RProperty::Define( KPSUidAiInformation, KActiveIdleExtHS_RSKLocked, RProperty::EText );
    RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_RSKLocked, KNullDesC );
    RProperty::Define( KPSUidAiInformation, KActiveIdleExtHS_PluginConfChange, RProperty::EInt );
    RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_PluginConfChange, 0 );
    
    const TUid KCAIPluginContainerImplementationUid = { 0x101FD658 };

    iStanbyContainerIF = CExtrStandbyContainerIF::NewL( KCAIPluginContainerImplementationUid,
                                         iAppView );
    iStanbyContainerIF->SetRect( ClientRect() );
    iStanbyContainerIF->SetFullScreenMode( isFullScreen );
    iAppView->SetControl( iStanbyContainerIF );

    iAIRepository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
    // CenRep notifier to listen statuspane key changes in cenrep.
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                 *iAIRepository,
                                                 CCenRepNotifyHandler::EIntKey,
                                                 KAiStatusPaneLayout );
    iNotifyHandler->StartListeningL();

    if( iFwEventHandler )
        {
        iFwEventHandler->HandleUiReadyEventL(*iUiCtl);
        }

    // Check powerkey availability
    FeatureManager::InitializeLibL();
    iFeatureNoPowerKey =
        FeatureManager::FeatureSupported( KFeatureIdNoPowerkey );
    FeatureManager::UnInitializeLib();

    if(iFeatureNoPowerKey)
        {
        iPowerkeyStatusObserver = AiUtility::CreatePSPropertyObserverL(
                                    TCallBack( HandlePowerkeyEvent, this ),
                                    KPSUidAvkonDomain,
                                    KAknEndKeyEvent );
        }

    CAknAppUiBase::SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort |
                                     CAknAppUiBase::EDisableSendKeyLong );
    }

CAppUi* CAppUi::NewL(CNativeUiController* aUiCtl)
    {
    CAppUi* self = new (ELeave) CAppUi(aUiCtl );

    // ConstructL is called by the UI framework

    return self;
    }

CAppUi::~CAppUi()
    {
    Release( iPowerkeyStatusObserver );
    delete iTimer;
    delete iNotifyHandler;
    delete iAIRepository;
    delete iStanbyContainerIF;
    delete iIdleIntegration;
    TRAP_IGNORE( DeactivateActiveViewL() );
    if( iAppView )
        {
        DeregisterView( *iAppView );
        RemoveFromStack( iAppView );
        delete iAppView;
        }
    if( iResourceLoader )
        {
        iResourceLoader->Close();
        delete iResourceLoader;
        }
    }

void CAppUi::HandleForegroundEventL(TBool aForeground)
    {
    CAknAppUi::HandleForegroundEventL( aForeground );

    if ( iUiFwObserver )
        {
        iUiFwObserver->HandleForegroundEvent( aForeground );
        }
    }

void CAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EEikCmdExit:
            {
            TInt value;
            TInt err = RProperty::Get( KPSUidStartup, KPSGlobalSystemState, value );
            if( err == KErrNone )
                {
                if ( value == ESwStateShuttingDown )
                    {
                    // If all is ok we filter our exits
                    Exit();
                    }
                }
            else
                {
                // If all is not ok we still exit
                Exit();
                }
            break;
            }
        case EAknSoftkeyExit:
        case EAknCmdExit: // fallthrough
            {

#ifdef _DEBUG

            Exit();
            if( iFwEventHandler && iUiCtl )
                {
                iFwEventHandler->HandleUiShutdown( *iUiCtl );
                }
            break;

#endif // _DEBUG

            }

        case ENativeUiSoftkeyLeft:
            {
            if( iFwEventHandler ) // LSK
                {
                HBufC* appBuf;
                appBuf = HBufC::NewLC( RProperty::KMaxPropertySize );
                TPtr appPtr = appBuf->Des();
                RProperty::Get( KPSUidAiInformation, KActiveIdleExtHS_LSKLocked, appPtr );
                if( appPtr.Length() > 0 )
                    {
                    iFwEventHandler->HandlePluginEvent(KAiLskLaunchByIndexLocked);
                    }
                else
                    {
                    iFwEventHandler->HandlePluginEvent(KAiLskLaunchByIndex);
                    }
                CleanupStack::PopAndDestroy( appBuf );
                }
            break;
            }

        case ENativeUiSoftkeyRight:
            {
            break;
            }

        case ENativeUiSelectionKey:
            {
            if( iAppView )
                {
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyOK;
                keyEvent.iRepeats = 0;
                TEventCode type(EEventKey);
                iAppView->OfferKeyEventL(keyEvent, type);
                }
            break;
            }

        default:
            {
            break;
            }

        }

    }

void CAppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUi::HandleResourceChangeL(aType);

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Screen layout changed, update and draw status pane.
        //Get pointer to status pane
        CEikStatusPane* statusPane = static_cast<CAknAppUi*>( iEikonEnv->EikAppUi() )->StatusPane();
        if( statusPane )
            {
            // Update status pane layout ot the correct one.
            TInt statusPaneLayout = GetLayoutCenrep();
            TInt id = statusPane->CurrentLayoutResId();
            if( id != iCurrentStatusPaneLayout )
                {
                statusPane->SwitchLayoutL( id );
                iCurrentStatusPaneLayout = id;
                }
            if( statusPaneLayout == EAiStatusPaneLayoutIdleHidden &&
                id != R_AVKON_STACON_PANE_LAYOUT_IDLE_SOFTKEYS_RIGHT )
                {
                statusPane->MakeVisible( EFalse );
                iStanbyContainerIF->SetFullScreenMode( ETrue );
                }
            else
                {
                statusPane->MakeVisible( ETrue );
                iStanbyContainerIF->SetFullScreenMode( EFalse );
                }
            statusPane->ApplyCurrentSettingsL();
            statusPane->DrawNow();
            }
        if( iAppView )
            {
            iAppView->SetRect( ClientRect() );
            iAppView->DrawNow();
            }

        }

    if ( iUiFwObserver )
        {
        iUiFwObserver->HandleResourceChange(aType);
        }
    }

void CAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
    {
    // Pointer event handling for RSK and SIND launching  
    // LSK handled via HandleCommand
    if ( AknLayoutUtils::PenEnabled() && aEvent.Type() == EEventPointer )
        {
        TPointerEvent *event = aEvent.Pointer();
        HandlePointerEvent( *event );
        }

    if( iIdleIntegration )
        {
        // Forward window server events first to idle integration library
        iIdleIntegration->HandleWsEventL(aEvent, aDestination);
        }

    // Call base class to let the UI framework handle the event
    CAknAppUi::HandleWsEventL( aEvent, aDestination);
    }

void CAppUi::HandlePointerEvent( TPointerEvent &aEvent )
    {        
    if ( !Cba()->IsVisible() )
        {
        return;
        }

    TRect rskRect;
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        rskRect = ::SoftkeyRectTopTouch();
        }
    else
        {
        rskRect = ::SoftkeyRectRightTouch();
        }
    // pointer event done in RSK => activate sind timer.
    if ( aEvent.iType == TPointerEvent::EButton1Down && 
            rskRect.Contains( aEvent.iParentPosition )) 
        {              
        iSINDKeyDown = ETrue;         
        const TTimeIntervalMicroSeconds32 KLongKeyPress(600000);
        iTimer->Cancel();
        iTimer->Start( KLongKeyPress, 0, TCallBack( TimerDone, this ) );
        }
    // It doesn't matter where the button up is done 
    // the voice dial is still skipped
    else if ( iSINDKeyDown && 
            aEvent.iType == TPointerEvent::EButton1Up )
        {
        SkipVoiceDial();
        }
    // Dragging outside RSK cancels opening of shortcut
    // and voice dial
    else if ( iSINDKeyDown && 
            aEvent.iType == TPointerEvent::EDrag &&
            !rskRect.Contains( aEvent.iParentPosition ))
        {
        iTimer->Cancel();
        iSINDKeyDown = EFalse;
        }
    }

void CAppUi::HandleScreenDeviceChangedL()
    {
    CAknAppUi::HandleScreenDeviceChangedL();

    if ( iFwEventHandler && iUiCtl )
        {
        //iFwEventHandler->HandleUiLayoutChangeL(*iUiCtl);
        }
    }

void CAppUi::PrepareToExit()
    {
    if ( iFwEventHandler && iUiCtl )
        {
        iUiCtl->PrepareToExit();
        iFwEventHandler->HandleUiShutdown(*iUiCtl);
        }
    }

TKeyResponse CAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if( aKeyEvent.iScanCode == EStdKeyDevice1 )
        {
        if( aType == EEventKeyDown )
            {
            iSINDKeyDown = ETrue;
            const TTimeIntervalMicroSeconds32 KLongKeyPress(600000);
            iTimer->Cancel();
            iTimer->Start( KLongKeyPress, 0, TCallBack( TimerDone, this ) );
            }
        else if( aType == EEventKeyUp )
            {
            if( iSINDKeyDown )
                {
                SkipVoiceDial();
                }
            }
        return EKeyWasConsumed;
        }
    else
        {
        if( aType == EEventKeyDown && iSINDKeyDown )
            {
            iSINDKeyDown = EFalse;
            SkipVoiceDial();
            }
        if( iAppView )
            {
            return iAppView->OfferKeyEventL(aKeyEvent, aType);
            }
        }

    return EKeyWasNotConsumed;
    }

void CAppUi::SkipVoiceDial()
    {
    iTimer->Cancel();
    // Handle skip scenario only if voice dial ui hasn't been launched
    if ( iFwEventHandler )
        {
        HBufC* app2Buf;
        app2Buf = HBufC::New( RProperty::KMaxPropertySize );
        if ( app2Buf )
            {
            TPtr app2Ptr = app2Buf->Des();
            RProperty::Get( KPSUidAiInformation, KActiveIdleExtHS_RSKLocked, app2Ptr );
            if( app2Ptr.Length() > 0 )
                {
                iFwEventHandler->HandlePluginEvent(KAiRskLaunchByIndexLocked);
                }
            else
                {
                iFwEventHandler->HandlePluginEvent(KAiRskLaunchByIndex);
                }
            delete app2Buf;
            }
    	}
    iSINDKeyDown = EFalse;
    }

TInt CAppUi::TimerDone(TAny* aSelf)
    {
    CAppUi* self = static_cast<CAppUi*>( aSelf );
    if ( self )
        {
        self->iTimer->Cancel();
        self->iSINDKeyDown = EFalse;
        self->iFwEventHandler->HandlePluginEvent(KAiRskLaunchVoiceDial);
        }
    return KErrNone;
    }

void CAppUi::StartL()
    {
    if( iStanbyContainerIF )
        {
        iStanbyContainerIF->StartL();
        }
    }

void CAppUi::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    // Set status pane on the fly.
    if( aId == KAiStatusPaneLayout )
        {
        if( aNewValue == EAiStatusPaneLayoutIdleNormal ||
            aNewValue == EAiStatusPaneLayoutIdleFlat ||
            aNewValue == EAiStatusPaneLayoutIdleHidden )
            {
            //Get pointer to status pane
            CEikStatusPane* statusPane = static_cast<CAknAppUi*>( iEikonEnv->EikAppUi() )->StatusPane();
            if( statusPane )
                {
                // Update status pane layout ot the correct one.
                if( aNewValue == EAiStatusPaneLayoutIdleHidden &&
                    iCurrentStatusPaneLayout != R_AVKON_STACON_PANE_LAYOUT_IDLE_SOFTKEYS_RIGHT  )
                    {
                    statusPane->MakeVisible( EFalse );
                    if ( iStanbyContainerIF )
                        {
                        iStanbyContainerIF->SetFullScreenMode( ETrue );
                        }

                    }
                else
                    {
                    statusPane->MakeVisible( ETrue );
                    if ( iStanbyContainerIF )
                        {
                        iStanbyContainerIF->SetFullScreenMode( EFalse );
                        }
                    }
                statusPane->DrawNow();
                if( iStanbyContainerIF )
                    {
                    iStanbyContainerIF->SetRect( ClientRect() );
                    }
                if ( iAppView )
                    {
                    iAppView->DrawNow();
                    }

                }
            }
        }
    }
    
void CAppUi::ExtHSThemeChanged()
    {
    //TRAP_IGNORE( iFwEventHandler->HandleUiThemeChangeStartL( *iUiCtl ) );
    TRAP_IGNORE( iFwEventHandler->HandleUiReadyEventL( *iUiCtl ) );
    }

TInt CAppUi::HandlePowerkeyEvent( TAny* aPtr )
    {
    CAppUi* self = static_cast<CAppUi*>(aPtr);
    if( self && self->iStanbyContainerIF )
        {
        TKeyEvent keyEvent;
        keyEvent.iCode = EKeyPhoneEnd;
        TEventCode eventType;
        eventType = EEventKey;
        TRAP_IGNORE( self->iStanbyContainerIF->OfferKeyEventL( keyEvent, eventType ) );
        }
    return KErrNone;
    }

// End of File.
