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
#include <avkon.rsg>                    
#include <e32property.h>                
#include <activeidle2domainpskeys.h>
#include <AknDlgShut.h>                 
#include <aknview.h>
#include <apacmdln.h>
#include <LogsUiCmdStarter.h>
#include <aknconsts.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <csxhelp/hmsc.hlp.hrh>

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

#include "xndomnode.h"
#include "xnproperty.h"
#include "xnuiengineappif.h"
#include "onlineofflinehelper.h"
#include "ainativeuiplugins.h"

#include "debug.h"

// Constants
// EAiDeviceStatusContentNetworkIdentity content id
_LIT( KNetworkIdentity, "NetworkIdentity" );
const TUid KVoiceUiUID = { 0x101F8543 };

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
    __TICK( "CAppUi::ConstructL" );
    
    // Always reset the phoneforward P&S key on startup just in case
    RProperty::Set( KPSUidAiInformation,
      KActiveIdleForwardNumericKeysToPhone, EPSAiForwardNumericKeysToPhone );
        
    iEditModeTitle = StringLoader::LoadL( R_QTN_HS_TITLE_EDITMODE );
    
    // Initialize with empty title pane so it's not shown on startup.                  
    __HEAP("XML UI: Init - Construct App UI")
    
    __TIME("XML UI: CXnAppUiAdapter::ConstructL",
        CXnAppUiAdapter::ConstructL() );       
      
    CAknAppUiBase::SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort |
                                     CAknAppUiBase::EDisableSendKeyLong );
    
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
    
    iUiCtl.NotifyAppEnvReadyL();
    
    iHelper = COnlineOfflineHelper::NewL( iUiCtl );
    
    iIdleIntegration = CAiUiIdleIntegration::NewL
        ( *iEikonEnv, keySoundConfig, iUiCtl.FwEventHandler() );
               
    __TICK( "CAppUi::ConstructL - done" );          
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
    delete iEditModeTitle;
    delete iKeyTimer;
    delete iHelper;
    delete iEventHandler;
    delete iContentRenderer;
    delete iIdleIntegration;           
    }

// ----------------------------------------------------------------------------
// CAppUi::ActivateUi()
// ----------------------------------------------------------------------------
//
void CAppUi::ActivateUi()
    {
    __PRINTS( "*** CAppUi::ActivateUi" );
       
    if ( iDeviceStatusInfo.Uid() == TUid::Null() )
        {
        __PRINTS( "*** CAppUi::ActivateUI - Loading DeviceStatus plugin" );
        
        _LIT8( KNs, "namespace" );
        
        // Load device status plugin here because it is always needed    
        iDeviceStatusInfo = THsPublisherInfo( KDeviceStatusPluginUid, 
            KDeviceStatusPluginName, KNs ); 
                           
        iUiCtl.FwStateHandler()->LoadPlugin( 
            iDeviceStatusInfo, EAiFwSystemStartup );                                           
        }        
    
    __PRINTS( "*** CAppUi::ActivateUi - done" );
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleUiReadyEventL()
// ----------------------------------------------------------------------------
//
void CAppUi::HandleUiReadyEventL()
    {
    __PRINTS( "*** CAppUi::HandleUiReadyEventL" );
    
    iUiCtl.FwEventHandler()->HandleUiReadyEventL( iUiCtl );
    
    __PRINTS( "*** CAppUi::HandleUiReadyEventL - done" );    
    }

// ----------------------------------------------------------------------------
// CAppUi::PrepareToExit()
// ----------------------------------------------------------------------------
//
void CAppUi::PrepareToExit()
    {    
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
            key.iCode = EKeyNull;
            key.iModifiers = 0;
            key.iRepeats = 0;
            
            iCoeEnv->SimulateKeyEventL( key, EEventKey );            
            }        
        }
    
    if( aEvent.Type() == EEventKeyDown )
        {
        if( aEvent.Key()->iScanCode == EStdKeyYes )
            {
            // Send key was pushed and long press recognition is started
            StartKeyTimerL();
            }
        }
    
    if( aEvent.Type() == EEventKeyUp )
        {
        if( aEvent.Key()->iScanCode == EStdKeyYes )
            {
            if( iKeyTimer && iKeyTimer->IsActive() )
                {
                // up event is coming before long press of Send key is recognized
                // Start dialled calls
                StopKeyTimer();
                LogsUiCmdStarter::CmdStartL( LogsUiCmdStarterConsts::KDialledView() );
                }
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
    iHelper->ProcessOnlineStateL( aOnline );        
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
        TVwsViewId activeViewId;
        TInt err( GetActiveViewId( activeViewId ) );

        if ( aEnter )
            {        
            iUiCtl.FwEventHandler()->SuspendContent( 
                iDeviceStatusInfo, KNetworkIdentity );
            
            if ( !err && View().ViewId() == activeViewId )
                {
                SetTitlePaneTextL( *iEditModeTitle );
                }            
            }
        else
            {
            if ( !err && View().ViewId() == activeViewId )
                {
                SetTitlePaneTextL( KNullDesC );
                }
        
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
    
    if ( err == KErrNotFound )
        {
        return;
        }
    
    // Get Xml Ui view id       
    TVwsViewId xmlViewId( View().ViewId() );
                     
    if ( xmlViewId == aNewlyActivatedViewId && activeViewId != xmlViewId )
        { 
        if ( iInEditMode )
            {
            TRAP_IGNORE( SetTitlePaneTextL( *iEditModeTitle ) );
            }
        else
            {
            // Xml Ui view became active         
            iUiCtl.FwEventHandler()->RefreshContent( 
                iDeviceStatusInfo, KNetworkIdentity );                                        
            }
        }    
    else if ( xmlViewId == aViewIdToBeDeactivated && activeViewId == xmlViewId )
        {
        if ( iInEditMode )
            {
            TRAP_IGNORE( SetTitlePaneTextL( KNullDesC ) );
            }
        else
            {
            // Xml Ui view became inactive
            iUiCtl.FwEventHandler()->SuspendContent( 
                iDeviceStatusInfo, KNetworkIdentity );                  
            }
        }
    } 

// ----------------------------------------------------------------------------
// CAppUi::StartKeyTimerL()
// Starts long press recognizer of Send key.
// ----------------------------------------------------------------------------
//
void CAppUi::StartKeyTimerL()
    {
    if ( !iKeyTimer )
        {
        iKeyTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        }
    else if ( iKeyTimer->IsActive() )
        {
        iKeyTimer->Cancel();
        }
    iKeyTimer->Start( KAknKeyboardRepeatInitialDelay,
                      KAknKeyboardRepeatInitialDelay, 
                      TCallBack ( ReportLongPressL, this ) );
    }

// ----------------------------------------------------------------------------
// CAppUi::StopKeyTimer()
// Stops Send key long press recognizer.
// ----------------------------------------------------------------------------
//
void CAppUi::StopKeyTimer()
    {
    if ( iKeyTimer && iKeyTimer->IsActive() )
        {
        iKeyTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleViewActivation()
// Launches Voice command application after long Send key is pressed.
// ----------------------------------------------------------------------------
//
void CAppUi::LaunchVoiceCommandL()
    {
    TApaTaskList apaTaskList( iCoeEnv->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( KVoiceUiUID );
    
    if ( apaTask.Exists() )
        {
        apaTask.BringToForeground();
        }
    else
        {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        CleanupClosePushL( apaLsSession );
        
        TApaAppInfo appInfo;
        
        if( apaLsSession.GetAppInfo( appInfo, KVoiceUiUID ) == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            User::LeaveIfError( apaLsSession.StartApp( *cmdLine ) );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        CleanupStack::PopAndDestroy( &apaLsSession );
        }
    }

// ----------------------------------------------------------------------------
// CAppUi::ReportLongPressL()
// Callback function for long press timer of the Send key.
// ----------------------------------------------------------------------------
//
TInt CAppUi::ReportLongPressL( TAny* aThis )
    {
    CAppUi* self = reinterpret_cast <CAppUi* > ( aThis );
    
    self->StopKeyTimer();
    
    if ( iAvkonAppUi->IsForeground() )
        {
        self->LaunchVoiceCommandL();
        }
    
    return 1; // CPeriodic ignores return value
    }

// ----------------------------------------------------------------------------
// CAppUi::SetTitlePaneTextL()
// 
// ----------------------------------------------------------------------------
//
void CAppUi::SetTitlePaneTextL( const TDesC& aText )
    {
    CEikStatusPane* sp( StatusPane() );  
    if ( !sp ) { return; }
    
    // make sure status pane is transparent.
    sp->EnableTransparent( ETrue );
    
    TUid titlePaneUid( TUid::Uid( EEikStatusPaneUidTitle ) );
           
    CEikStatusPaneBase::TPaneCapabilities subPaneTitle( 
        sp->PaneCapabilities( titlePaneUid ) ); 
                    
    if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
        {
        CAknTitlePane* title = 
            static_cast< CAknTitlePane* >( sp->ControlL( titlePaneUid ) ); 
        
        if ( title )
            {
            title->SetTextL( aText );        
            title->DrawNow();
            }               
        }
    
    // redraw statuspane
    sp->DrawNow();
    }

// -----------------------------------------------------------------------------
// CAppUi::HelpContextL
// Returns the help context
// -----------------------------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CAppUi::HelpContextL() const
    {
	TUid fwUid = TUid::Uid( AI_UID3_AIFW_COMMON );
	TCoeContextName helpString;
	if( iUiCtl.UiEngineL()->IsEditMode()) 
	    {
	    helpString.Copy( KSET_HLP_HOME_SCREEN_EDIT );
	    }
    else
	    {
	    helpString.Copy( KSET_HLP_HOME_SCREEN );
	    }
    
    CArrayFixFlat<TCoeHelpContext>* array =
                   new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( array );
    array->AppendL( TCoeHelpContext( fwUid, helpString  ) );
    CleanupStack::Pop( array );
    return array;
    }

// End of File.
