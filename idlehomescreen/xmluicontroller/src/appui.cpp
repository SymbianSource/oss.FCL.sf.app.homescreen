/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  App ui
*
*/

// System includes
#include <centralrepository.h>
#include <avkon.rsg>                    // For status pane layout resource ids
#include <startupdomainpskeys.h>
#include <e32property.h>                // For RProperty
#include <activeidle2domainpskeys.h>
#include <AknDlgShut.h>                 // Avkon dialog shutter.
#include <aknview.h>

// User includes
#include <hspublisherinfo.h>
#include <aifwdefs.h>
#include <activeidle2domaincrkeys.h>
#include <aifweventhandler.h>
#include <aifwstatehandler.h>
#include <ai3xmlui.rsg>

#include "appui.h"
#include "application.h"
#include "xmluicontroller.h"
#include "aixuikoneventhandler.h"
#include "aixmluiconstants.h"
#include "contentrenderer.h"
#include "psobserver.h"

#include "aicontentmodel.h"
#include "aiuiidleintegration.h"
#include "xmluicontrollerpanic.h"

#include "debug.h"
#include "ai3perf.h"
#include "xndomnode.h"
#include "xnproperty.h"
#include "xnuiengineappif.h"
#include "onlineofflinehelper.h"
#include "ainativeuiplugins.h"

// Constants
// EAiDeviceStatusContentNetworkIdentity content id
_LIT( KNetworkIdentity, "NetworkIdentity" );

using namespace AiXmlUiController;

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAppUi::CAppUi
// ----------------------------------------------------------------------------
//
CAppUi::CAppUi( CXmlUiController& aUiCtl, TUid aAppUid )
    : CXnAppUiAdapter( aAppUid ), iUiCtl( aUiCtl )            
    {
    }

// ----------------------------------------------------------------------------
// CAppUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CAppUi::ConstructL()
    {
    AI3_PERF_START(perfdata, "xmluicontroller: CAppUi::ConstructL")
        
    // Always reset the phoneforward P&S key on startup just in case
    RProperty::Set( KPSUidAiInformation,
      KActiveIdleForwardNumericKeysToPhone, EPSAiForwardNumericKeysToPhone );
          
    // Initialize with empty title pane so it's not shown on startup.                  
    __HEAP("XML UI: Init - Construct App UI")
    __TIME("XML UI: XnAppUiAdapted::ConstructL",
        CXnAppUiAdapter::ConstructL();       
    ) 
        
    // Register for XML UI view activation & deactivation
    AddViewActivationObserverL( this );
    
    // Disable CCoeEnv exit checks.
    // Active Idle Framework will perform the checks.
    iCoeEnv->DisableExitChecks( ETrue );
    
    __TIME("XML UI: Construct Content Renderer",
        iContentRenderer = CContentRenderer::NewL( *this );
    )
    __HEAP("XML UI: Construct Content Renderer");

    TXnUiEngineAppIf* uiEngine( UiEngineL() );
    __ASSERT_ALWAYS( uiEngine, Panic(ECriticalPointerNull) );
    
    iUiCtl.SetObserver( *iContentRenderer );
    
    iContentRenderer->SetEventHandler( *iUiCtl.FwEventHandler() );
    
    iUiCtl.SetAppUi( *this );
    
    iEventHandler = iUiCtl.CreateXuikonEventHandlerL( *iContentRenderer );
    
    TAiIdleKeySoundConfig keySoundConfig;
    keySoundConfig.iKeySounds = KeySounds();
    keySoundConfig.iContextResId = R_XUI_DEFAULT_SKEY_LIST;
    
    iIdleIntegration = CAiUiIdleIntegration::NewL
        ( *iEikonEnv, keySoundConfig, iUiCtl.FwEventHandler() );
                         
    iUiCtl.NotifyAppEnvReadyL();
        
    CAknAppUiBase::SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort |
                                     CAknAppUiBase::EDisableSendKeyLong );

    iHelper = COnlineOfflineHelper::NewL( iUiCtl );
    
    // Load device status plugin here because it is always needed    
    iDeviceStatusInfo = THsPublisherInfo( KDeviceStatusPluginUid, 
        KDeviceStatusPluginName, KNullDesC8 ); 
                
    iUiCtl.FwStateHandler()->LoadPlugin( 
            iDeviceStatusInfo, EAiFwSystemStartup );        
    
    __HEAP("XML UI: Done - Construct App UI");
    
    AI3_PERF_STOP(perfdata, "xmluicontroller: CAppUi::ConstructL")
    }

// ----------------------------------------------------------------------------
// CAppUi::NewL()
// ----------------------------------------------------------------------------
//
CAppUi* CAppUi::NewL( CXmlUiController& aUiCtl )
    {
    CAppUi* self = new ( ELeave ) CAppUi( aUiCtl, KUidXmlUiApp );

    // ConstructL is called by the UI framework

    return self;
    }

// ----------------------------------------------------------------------------
// CAppUi::~CAppUi()
// ----------------------------------------------------------------------------
//
CAppUi::~CAppUi()
    {
    delete iHelper;
    delete iEventHandler;
    delete iContentRenderer;
    delete iIdleIntegration;           
    }

// ----------------------------------------------------------------------------
// CAppUi::PrepareToExit()
// ----------------------------------------------------------------------------
//
void CAppUi::PrepareToExit()
    {
    iUiShutdown = ETrue;

    RemoveViewActivationObserver( this );
    
    iUiCtl.FwEventHandler()->HandleUiShutdown( iUiCtl );
    
    CXnAppUiAdapter::PrepareToExit();
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleCommandL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleCommandL(TInt aCommand)
    {
    switch( aCommand )
        {
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EAknSoftkeyExit:
        case EAknCmdExit:
            {
#ifdef _DEBUG

            Exit();
            break;            
#endif // _DEBUG
            }            
        default:
            {
            break;
            }            
        }        
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleWsEventL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleWsEventL( const TWsEvent& aEvent, 
    CCoeControl* aDestination )
    {    
    if( aEvent.Type() == KAknUidValueEndKeyCloseEvent )
        {
        // End key close event, run dialog shutter
        // to dismiss open dialogs
        if ( IsDisplayingMenuOrDialog() )
            {
            AknDialogShutter::ShutDialogsL( 
                *static_cast< CEikonEnv* >( CCoeEnv::Static() ) );             
            }
        else
            {
            TKeyEvent key;
            
            key.iScanCode = EStdKeyNo;
            key.iCode = EStdKeyNull;
            key.iModifiers = 0;
            key.iRepeats = 0;
            
            iCoeEnv->SimulateKeyEventL( key, EEventKey );            
            }        
        }
         	  
    if( iIdleIntegration )
        {
        // Forward window server events first to idle integration library
        iIdleIntegration->HandleWsEventL( aEvent, aDestination );
        }

    // Call base class to let the UI framework handle the event
    // Don't trap here, because there could be leaves which need to
    // handled by base classes/UI framework like KLeaveExit, KErrNoMemory etc.
    CXnAppUiAdapter::HandleWsEventL( aEvent, aDestination );
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleXuikonEventL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleXuikonEventL( CXnNodeAppIf& aOrigin, 
    CXnNodeAppIf& aTrigger, 
    CXnDomNode& aTriggerDefinition, 
    CXnDomNode& aEvent )
    {
    __PRINTS("*** UC: Init - Event Management ***");

	if( iEventHandler )
	    {
	__TIME("UC: Event Management",
    	iEventHandler->HandleXuikonEventL( aOrigin, 
           aTrigger, aTriggerDefinition, aEvent ) );	
	    }
	__PRINTS("*** UI: Done - Event Management");
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleEventL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleEventL( const TDesC& aEvent, CXnNodeAppIf& aDestination )
    {
    THsPublisherInfo info;
    
    iUiCtl.PublisherInfoL( aDestination, info );
    
    iUiCtl.FwEventHandler()->HandlePluginEventL( info, aEvent );
    }

// ----------------------------------------------------------------------------
// CAppUi::LoadDataPluginsL()
// ----------------------------------------------------------------------------
//
TInt CAppUi::LoadPublisher( CXnNodeAppIf& aPublisher, TInt aReason )
    {
    if ( iUiShutdown )
        {
        // Framework has already destroyed all publishers
        return KErrNone;
        }
    
    THsPublisherInfo info;
    
    TRAP_IGNORE( iUiCtl.PublisherInfoL( aPublisher, info ) );

    if ( info.Uid() == KDeviceStatusPluginUid )
        {
        // Update device status info
        iDeviceStatusInfo = info;
        }
    
    return iUiCtl.FwStateHandler()->LoadPlugin( 
        info, (TAiFwLoadReason) aReason );        
    }

// ----------------------------------------------------------------------------
// CAppUi::DestroyPublisher()
// ----------------------------------------------------------------------------
//
TInt CAppUi::DestroyPublisher( CXnNodeAppIf& aPublisher, TInt aReason )
    {
    if ( iUiShutdown )
        {
        // Framework has already destroyed all publishers
        return KErrNone;
        }
    
    THsPublisherInfo info;
           
    TRAP_IGNORE( iUiCtl.PublisherInfoL( aPublisher, info ) );
            
    iUiCtl.FwStateHandler()->DestroyPlugin( 
        info, (TAiFwDestroyReason) aReason );
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CAppUi::DynInitMenuItemL()
// ----------------------------------------------------------------------------
//
TBool CAppUi::DynInitMenuItemL( const TDesC& aItemType, 
    RPointerArray< CXnNodeAppIf >* aList )
    {
    if ( iUiShutdown )
        {
        // Framework has already destroyed all publishers
        return EFalse;
        }
    
    _LIT( KOnline, "hs_online" );
    _LIT( KOffline, "hs_offline" );
    
    TBool retval( EFalse );
               
    if( aItemType == KOnline )
        {
        return iHelper->ShowOnlineItem();
        }
    else if( aItemType == KOffline )
        {
        return iHelper->ShowOfflineItem();
        }
    else if( aList )
        {
        RPointerArray< CXnNodeAppIf >& list( *aList );
        
        for( TInt i = 0; !retval && i < list.Count(); i++ )
            {
            THsPublisherInfo info;
                           
            iUiCtl.PublisherInfoL( *list[i], info );
                                   
            retval = iUiCtl.FwEventHandler()->HasMenuItemL( info, aItemType );                                                
            }        
        }
                          
    return retval;
    }

// ----------------------------------------------------------------------------
// CAppUi::SetOnlineStateL()
// Called always when user changed HS online/offline status from option menu.
// ----------------------------------------------------------------------------
//
void CAppUi::SetOnlineStateL( TBool aOnline )
    {
    if ( !iUiShutdown )
        {
        iHelper->ProcessOnlineStateL( aOnline );    
        }
    }

// ----------------------------------------------------------------------------
// CAppUi::UiController()
// ----------------------------------------------------------------------------
//
CXmlUiController& CAppUi::UiController() const
    {
    return iUiCtl;
    }

// ----------------------------------------------------------------------------
// CAppUi::Helper()
// ----------------------------------------------------------------------------
//
COnlineOfflineHelper* CAppUi::Helper() const
    {
    return iHelper;
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleEnterEditModeL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleEnterEditModeL( TBool aEnter )
    {
    if ( iInEditMode != aEnter )
        {
        if ( aEnter )
            {            
            iUiCtl.FwEventHandler()->SuspendContent( 
                iDeviceStatusInfo, KNetworkIdentity );
            }
        else
            {
            iUiCtl.FwEventHandler()->RefreshContent( 
                iDeviceStatusInfo, KNetworkIdentity );            
            }
        
        iInEditMode = aEnter;
        }    
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleViewActivation()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
    const TVwsViewId& aViewIdToBeDeactivated )
    {
    TVwsViewId activeViewId;
    
    TInt err( GetActiveViewId( activeViewId ) );
    
    if ( iInEditMode || err == KErrNotFound )
        {
        return;
        }
    
    // Get Xml Ui view id       
    TVwsViewId xmlViewId( View().ViewId() );
                     
    if ( xmlViewId == aNewlyActivatedViewId && activeViewId != xmlViewId )
          {        
          // Xml Ui view became active         
          iUiCtl.FwEventHandler()->RefreshContent( 
              iDeviceStatusInfo, KNetworkIdentity );                                
          }    
    else if ( xmlViewId == aViewIdToBeDeactivated && activeViewId == xmlViewId )
          {
          // Xml Ui view became inactive
          iUiCtl.FwEventHandler()->SuspendContent( 
              iDeviceStatusInfo, KNetworkIdentity );          
          }
      }    

// End of File.
