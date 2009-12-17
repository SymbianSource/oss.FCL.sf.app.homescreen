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
#include <activeidle2domaincrkeys.h>
#include <aifweventhandler.h>
#include <ai3xmlui.rsg>

#include "appui.h"
#include "application.h"
#include "xmluicontroller.h"
#include "aixuikoneventhandler.h"
#include "aiuiframeworkobserver.h"
#include "aixmluiconstants.h"
#include "contentrenderer.h"
#include "psobserver.h"

#include "aipropertyextension.h"
#include "aicontentmodel.h"
#include "aiuiidleintegration.h"
#include "xmluicontrollerpanic.h"

#include "debug.h"
#include "ai3perf.h"
#include "xndomnode.h"
#include "xnproperty.h"
#include "xnuiengineappif.h"
#include "onlineofflinehelper.h"

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
    RProperty::Set(KPSUidAiInformation,
                      KActiveIdleForwardNumericKeysToPhone,
                      EPSAiForwardNumericKeysToPhone);

    // Initialize with empty title pane so it's not shown on startup.                  
    __HEAP("XML UI: Init - Construct App UI")
    __TIME("XML UI: XnAppUiAdapted::ConstructL",
        CXnAppUiAdapter::ConstructL();       
    ) 
    
    // Register for XML UI view activation & deactivation
    AddViewActivationObserverL( this );
    
    // Disable CCoeEnv exit checks.
    // Active Idle Framework will perform the checks.
    iCoeEnv->DisableExitChecks(ETrue);
    
    __TIME("XML UI: Construct Content Renderer",
        iContentRenderer = CContentRenderer::NewL( *this );
    )
    __HEAP("XML UI: Construct Content Renderer");

    TXnUiEngineAppIf* uiEngine = UiEngineL();
    __ASSERT_ALWAYS( uiEngine, Panic(ECriticalPointerNull) );
    
    iUiCtl.SetObserver( *iContentRenderer );
    
    iContentRenderer->SetEventHandler( *iUiCtl.FwEventHandler() );
    
    iUiCtl.SetAppUi( *this );
    
    iEventHandler = iUiCtl.CreateXuikonEventHandlerL( *iContentRenderer );

    iUiFwObserver = iUiCtl.UiFrameworkObserver();

    TAiIdleKeySoundConfig keySoundConfig;
    keySoundConfig.iKeySounds = KeySounds();
    keySoundConfig.iContextResId = R_XUI_DEFAULT_SKEY_LIST;
    
    iIdleIntegration = CAiUiIdleIntegration::NewL
        ( *iEikonEnv, keySoundConfig, iUiCtl.FwEventHandler() );
                 
    iHelper = COnlineOfflineHelper::NewL( iUiCtl );
    
    iUiCtl.NotifyAppEnvReadyL();
    
    LoadNativeDataPluginsL();

    CAknAppUiBase::SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort |
                                     CAknAppUiBase::EDisableSendKeyLong );
    
    iIsEditModeActive = EFalse;

   
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
    iUiCtl.FwEventHandler()->HandleUiShutdown( iUiCtl );
    
    CXnAppUiAdapter::PrepareToExit();
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleForegroundEventL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleForegroundEventL( TBool aForeground )
    {
    CXnAppUiAdapter::HandleForegroundEventL( aForeground );
    
    if( iUiFwObserver )
        {
        iUiFwObserver->HandleForegroundEvent( aForeground );
        }
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
// CAppUi::HandleResourceChangeL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleResourceChangeL( TInt aType )
    {
    CXnAppUiAdapter::HandleResourceChangeL( aType );
    
    if( iUiFwObserver )
        {
        iUiFwObserver->HandleResourceChange( aType );
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
                                           aTrigger, 
                                           aTriggerDefinition, 
                                           aEvent );
	);
	    }
	__PRINTS("*** UI: Done - Event Management");
    }
    
// ----------------------------------------------------------------------------
// CAppUi::LoadDataPluginsL()
// ----------------------------------------------------------------------------
//
void CAppUi::LoadDataPluginsL( RPointerArray< CXnNodeAppIf >& aList )
    {
    for( TInt i = 0; i < aList.Count(); i++ )
        {
        TAiPublisherInfo info;
        
        iUiCtl.PublisherInfoL( *aList[i], info );
                
        iUiCtl.FwEventHandler()->HandleLoadPluginL( info );
        }
    }

// ----------------------------------------------------------------------------
// CAppUi::LoadNativeDataPluginsL()
// ----------------------------------------------------------------------------
//
void CAppUi::LoadNativeDataPluginsL()
    {
    if ( !iNativePluginsLoaded )
        {
        RAiPublisherInfoArray plugins;
        CleanupClosePushL( plugins );
        
        iUiCtl.PublisherInfoL( plugins );
        
        for ( TInt i = 0; i < plugins.Count(); i++ )
            {
            iUiCtl.FwEventHandler()->HandleLoadPluginL( plugins[i] );
            }
        
        CleanupStack::PopAndDestroy( &plugins );
        
        iNativePluginsLoaded = ETrue;
        }              
    }

// ----------------------------------------------------------------------------
// CAppUi::DestroyDataPluginsL()
// ----------------------------------------------------------------------------
//
void CAppUi::DestroyDataPluginsL( RPointerArray< CXnNodeAppIf >& aList )
    {
    for( TInt i = 0; i < aList.Count(); i++ )
        {
        TAiPublisherInfo info;
        
        iUiCtl.PublisherInfoL( *aList[i], info );
                
        iUiCtl.FwEventHandler()->HandleDestroyPluginL( info );
        }    
    }

// ----------------------------------------------------------------------------
// CAppUi::DestroyNativeDataPluginsL()
// ----------------------------------------------------------------------------
//
void CAppUi::DestroyNativeDataPluginsL()
    {
    if ( iNativePluginsLoaded )
        {                
        RAiPublisherInfoArray plugins;
        CleanupClosePushL( plugins );
        
        iUiCtl.PublisherInfoL( plugins );
        
        for ( TInt i = 0; i < plugins.Count(); i++ )
            {
            iUiCtl.FwEventHandler()->HandleDestroyPluginL( plugins[i] );
            }
        
        CleanupStack::PopAndDestroy( &plugins );
        
        iNativePluginsLoaded = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CAppUi::DynInitMenuItemL()
// ----------------------------------------------------------------------------
//
TBool CAppUi::DynInitMenuItemL( const TDesC& aItemType, 
    RPointerArray< CXnNodeAppIf >* aList )
    {
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
            TAiPublisherInfo info;
                           
            iUiCtl.PublisherInfoL( *list[i], info );
                                   
            retval = iUiCtl.FwEventHandler()->HasMenuItemL( info, aItemType );                                                
            }        
        }
                          
    return retval;
    }

// ----------------------------------------------------------------------------
// CAppUi::SetOnlineStateL()
// Called always by view manager when view is activated.
// ----------------------------------------------------------------------------
//
void CAppUi::SetOnlineStateL( RPointerArray< CXnNodeAppIf >& aList )
    {
    iHelper->ProcessOnlineStateL( aList );
    }

// ----------------------------------------------------------------------------
// CAppUi::SetOnlineL()
// Called always when user changed HS online/offline status from option menu.
// ----------------------------------------------------------------------------
//
void CAppUi::SetOnlineStateL( TBool aOnline )
    {
    iHelper->ProcessOnlineStateL( aOnline );
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleViewActivation()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
    const TVwsViewId& aViewIdToBeDeactivated )
    {
    if ( iIsEditModeActive )
        {
        return;
        }
    
    // Get Xml Ui view id       
    TVwsViewId viewId( View().ViewId() );
    
    if ( viewId == aNewlyActivatedViewId )
        {        
        // Xml Ui view became active
        TRAP_IGNORE( LoadNativeDataPluginsL() );
        }
    else if ( viewId == aViewIdToBeDeactivated )
        {
        // Xml Ui view became inactive 
        TRAP_IGNORE( DestroyNativeDataPluginsL() );
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
// CAppUi::HandlePageSwitch()
// ----------------------------------------------------------------------------
//
void CAppUi::HandlePageSwitch()
    {
    iUiCtl.FwEventHandler()->ProcessStateChange( EAifwPageSwitch );
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleEnterEditModeL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleEnterEditModeL( TBool aEnter )
    {
    if ( aEnter )
        {
        DestroyNativeDataPluginsL();
        iIsEditModeActive = ETrue;
        }
    else
        {
        LoadNativeDataPluginsL();
        iIsEditModeActive = EFalse;
        }
    }

// End of File.
