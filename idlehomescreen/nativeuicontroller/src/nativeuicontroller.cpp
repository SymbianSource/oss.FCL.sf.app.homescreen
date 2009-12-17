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
* Description:  Native UI controller.
*
*/

// System includes
#include <bautils.h>
#include <e32std.h>
#include <e32property.h>
#include <coemain.h>
#include <avkon.rsg>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <activeidle2domainpskeys.h>
#include <aipspropertyobserver.h>
#include <eikstart.h>
#include <AknLayout2ScalableDef.h>
#include <avkondomainpskeys.h>
#include <AknDlgShut.h> 

// User includes
#include "nativeuicontroller.h"
#include "ainativeui.hrh"
#include "application.h"
#include "appui.h"
#include "aiutility.h"
#include "aistrparser.h"
#include "aidevicestatuscontentmodel.h"
#include "aipropertyextension.h"
#include "aistatuspanel.h"
#include "ainativerenderer.h"
#include "aititlepanerenderer.h"
#include "ainavipanerenderer.h"
#include "aidialogrenderer.h"
#include "aisoftkeyrenderer.h"
#include "ainotifierrenderer.h"
#include "aitoolbarrenderer.h"
#include "ainativeuiplugins.h"
#include "activeidle2domaincrkeys.h"
#include "aistatuspanetouchui.h"
#include "ainativeuistrings.h" // string literals
#include "aistrcnv.h"
#include <aiscutplugindomaincrkeys.h>

using namespace AiNativeUiController;

// Constants
_LIT( KResourceDrive, "Z:" );
_LIT( KResourceFile, "ainativeui.rsc" );
_LIT( KSettingsDummyData, "" );
_LIT( KSettingsIdSeparator, "/" );
_LIT( KSettings, "Settings" );

const TInt KOneSecondInMicroS = 1000*1000;
const TInt KAI2CrKeyIncrementBy2 = 2;

#define KResourcePath KDC_APP_RESOURCE_DIR

const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidNativeUiController, CNativeUiController::NewL)
    };


// ----------------------------------------------------------------------------
// CNativeUiController::NewL()
// ----------------------------------------------------------------------------
//
CNativeUiController* CNativeUiController::NewL()
    {
    CNativeUiController* self = new (ELeave) CNativeUiController;

    CleanupStack::PushL( self );
    
    // Set Native UI Controller object to TLS for access in static
    // CNativeUiController::NewApplication
    User::LeaveIfError( Dll::SetTls( self ) );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Exit()
// ----------------------------------------------------------------------------
//
void CNativeUiController::Exit()
    {
    if ( iExitTimer )
        {
        iExitTimer->Cancel();
        
        iExitTimer->Start( 0, KOneSecondInMicroS,                       
           TCallBack( ExitTimerCallBack, this ) );
        }

    }

// ----------------------------------------------------------------------------
// CNativeUiController::ExitTimerCallBack()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::ExitTimerCallBack( TAny *aSelf )
    {
    CNativeUiController* self =
        static_cast<CNativeUiController*>( aSelf );
        
    if ( self )
        {
        self->iExitTimer->Cancel();
        self->PrepareToExit();
        self->iAppUi->Exit();
        }
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::~CNativeUiController()
// ----------------------------------------------------------------------------
//
CNativeUiController::~CNativeUiController()
    {
    iPlugins.Close();

    delete iExitTimer;

    PrepareToExit();

    // Reset TLS pointer to this set in NewL
    Dll::SetTls( NULL );
    }

// ----------------------------------------------------------------------------
// CNativeUiController::PrepareToExit()
// ----------------------------------------------------------------------------
//
void CNativeUiController::PrepareToExit()
    {
    iRenderers.ResetAndDestroy();
    
    delete iStatusPanel;
    iStatusPanel = NULL;
    
    Release( iIdleStatusObserver );
    iIdleStatusObserver = NULL;
    
    Release( iKeylockStatusObserver );
    iKeylockStatusObserver = NULL;
	
    Release( iExtHsPluginConfChange );
	iExtHsPluginConfChange = NULL;
	
    //If we are not the
    if( !iRunningAsMain && iResourceOffset > 0 )
        {
        iCoeEnv->DeleteResourceFile( iResourceOffset );
        iResourceOffset = KErrNotFound;
        }
    }

// ----------------------------------------------------------------------------
// CNativeUiController::CNativeUiController()
// ----------------------------------------------------------------------------
//
CNativeUiController::CNativeUiController() 
    : iResourceOffset( KErrNotFound )
    {
    }

// ----------------------------------------------------------------------------
// CNativeUiController::AddRendererL()
// ----------------------------------------------------------------------------
//
void CNativeUiController::AddRendererL( CAiNativeRenderer* aRenderer )
    {
    User::LeaveIfError( iRenderers.Append( aRenderer ) );
    }

// ----------------------------------------------------------------------------
// CNativeUiController::RemoveRenderer()
// ----------------------------------------------------------------------------
//
TBool CNativeUiController::RemoveRenderer( CAiNativeRenderer *aRenderer, 
    TBool aDelete )
    {
    for ( TInt i = 0; i < iRenderers.Count(); i++ )
        {
        if ( aRenderer == iRenderers[i] )
            {
            if ( aDelete )
                {
                delete aRenderer;
                }
            iRenderers.Remove(i);
    
            return ETrue;
            }
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::ExitTimerCallBack()
// ----------------------------------------------------------------------------
//
void CNativeUiController::DeleteToolbarRenderer()
    {
    if ( iToolbarRenderer )
        {
        RemoveRenderer( iToolbarRenderer );
        delete iToolbarRenderer;
        iToolbarRenderer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CNativeUiController::RecreateToolbarRendererL()
// ----------------------------------------------------------------------------
//
TBool CNativeUiController::RecreateToolbarRendererL()
    {
    TBool created( EFalse );
    // Remove the old renderer...
    DeleteToolbarRenderer();

    CAknToolbar* toolbar( iAvkonAppUi->CurrentFixedToolbar() );
    // ...and create the new if there is an existing toolbar that
    // we can use
    if ( toolbar )
        {
        iToolbarRenderer = CAiToolbarRenderer::NewL( *iFwEventHandler, *toolbar);
        AddRendererL( iToolbarRenderer );
        created = ETrue;
        }
    
    return created;
    }


// ----------------------------------------------------------------------------
// CNativeUiController::DoPublish()
// ----------------------------------------------------------------------------
//
template<class T>
TInt CNativeUiController::DoPublish( MAiPropertyExtension& aPlugin,
    TInt aContent, T& aData, TInt aIndex )
    {
    const TAiPublisherInfo* info( NULL );
    
    TRAP_IGNORE( info = aPlugin.PublisherInfoL() );
       
    if ( !info || info->iNamespace != KNativeUiNamespace )
        {
        return KErrNotSupported;
        }
    
    const TInt count( iRenderers.Count() );
    TInt err( KErrNone );
    
    for( TInt i( 0 ); i < count; i++ )
        {
        err = iRenderers[i]->Publish( aPlugin, aContent, aData, aIndex );

        if( err == KErrNone )
            {
            //data published
            return KErrNone;
            }

        if( err != KErrNotFound )
            {
            return err;
            }
        }
    
    return err;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::HandleIdleStateEvent()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::HandleIdleStateEvent( TAny* aPtr )
    {
    CNativeUiController* self = 
        reinterpret_cast< CNativeUiController* >( aPtr );

    if( self )
        {
        TInt idleState = EPSAiBackground;
        self->iIdleStatusObserver->Get( idleState );

        const TInt count = self->iRenderers.Count();

        if( idleState == EPSAiForeground )
            {
            for( TInt i( 0 ); i < count; i++ )
                {
                TRAP_IGNORE( self->iRenderers[i]->FocusObtainedL() );
                }
            }
        else
            {
            for( TInt j( 0 ); j < count; j++ )
                {
                TRAP_IGNORE( self->iRenderers[j]->FocusLostL() );
                }
            }
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::HandleKeylockStateEvent()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::HandleKeylockStateEvent( TAny* aPtr )
    {
    CNativeUiController* self = 
        reinterpret_cast< CNativeUiController* >( aPtr );

    if( self )
        {
        TInt keylockState = EKeyguardNotActive;
        self->iKeylockStatusObserver->Get( keylockState );

        const TInt count = self->iRenderers.Count();

        if( keylockState != EKeyguardNotActive )
            {
            TRAP_IGNORE(
                // Run dialog shutter to shut any open dialogs
                AknDialogShutter::ShutDialogsL( 
                    *static_cast< CEikonEnv* >( CCoeEnv::Static() ) ); 
                
                TKeyEvent key;
                key.iCode = 0;
                key.iModifiers = 0;
                key.iScanCode = EStdKeyNo;
                CCoeEnv::Static()->SimulateKeyEventL( key, EEventKey );
                );
            
            for( TInt i( 0 ); i < count; i++ )
                {
                TRAP_IGNORE( self->iRenderers[i]->KeylockEnabledL() );
                }
            }
        else
            {
            for( TInt i( 0 ); i < count; i++ )
                {
                TRAP_IGNORE( self->iRenderers[i]->KeylockDisabledL() );
                }
            }
        }
    
    return KErrNone;
	}

// ----------------------------------------------------------------------------
// CNativeUiController::HandlePluginConfChange()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::HandlePluginConfChange( TAny* aPtr )
	{
	CNativeUiController* self = 
        reinterpret_cast< CNativeUiController* >( aPtr );

    TInt value( 0 );
    
    RProperty::Get( KPSUidAiInformation, 
        KActiveIdleExtHS_PluginConfChange, value );
    
    if( self && value )
        {
        self->iAppUi->ExtHSThemeChanged();
        }
    
    return KErrNone;
	}

// ----------------------------------------------------------------------------
// CNativeUiController::LoadUIDefinitionL()
// ----------------------------------------------------------------------------
//
void CNativeUiController::LoadUIDefinitionL()
    {
    if ( iUiLoaded )
        {
        TInt count( iRenderers.Count() );
    
        for( TInt i( 0 ); i < count; i++ )
            {
            iRenderers[i]->LoadUIDefinitionL();
            }
        return;
        }
    
    if ( !iExitTimer )
        {
        iExitTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        }
    
    // This method is called upon startup. We'll need to:
    iRenderers.ResetAndDestroy();

    if( !iStatusPanel )
        {
        // 2) Initialize status pane for publishing in general
        iStatusPanel = CAiStatusPanel::NewL();
        }

    // 3) Create renderers that handle publishing to title pane...
    AddRendererL( CAiTitlePaneRenderer::NewLC( *iStatusPanel ) );
    CleanupStack::Pop();//CAiTitlePaneRenderer

    // ...navi pane...
    AddRendererL( CAiNaviPaneRenderer::NewLC( *iStatusPanel ) );
    CleanupStack::Pop();//CAiNaviPaneRenderer

    // ...dialogs and.
    AddRendererL( CAiDialogRenderer::NewLC() );
    CleanupStack::Pop();//CAiDialogRenderer

    AddRendererL( CAiNotifierRenderer::NewLC() );
    CleanupStack::Pop();//CAiNotifierRenderer

    // ...touch ui.
    AddRendererL( CAiStatusPaneTouchUi::NewLC( *iStatusPanel, *iFwEventHandler ) );
    CleanupStack::Pop();//CAiStatusPaneTouchUi

    if( iRunningAsMain )
        {
        AddRendererL( CAiSoftKeyRenderer::NewLC() );
        CleanupStack::Pop();//CAiSoftKeyRenderer

        // Toolbar shown only when we are not the main ui controller
        DeleteToolbarRenderer();
	    
	    TRAP_IGNORE(
	        iAppUi->StartL(); )
        }

    // We need to load the resource file here if we are not main controller.
    // If we are the main controller the loading is done in appui
    // Also the toolbar renderer is loaded here.
    else
        {
        //Load resources file for resource publishing
        TFullName resourceFile( KResourceDrive );
        resourceFile.Append( KResourcePath );
        resourceFile.Append( KResourceFile );
        BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resourceFile );
        if( iCoeEnv && iResourceOffset == KErrNotFound )
            {
            iResourceOffset = iCoeEnv->AddResourceFileL( resourceFile );
            }
        // Create the toolbar renderer
        RecreateToolbarRendererL();
        }

    // 4) Add an observer that informs us about the focus transition changes
    if( !iIdleStatusObserver )
        {
        iIdleStatusObserver = AiUtility::CreatePSPropertyObserverL(
                                    TCallBack( HandleIdleStateEvent, this ),
                                    KPSUidAiInformation,
                                    KActiveIdleState );
        }
    if( !iKeylockStatusObserver )
        {
        iKeylockStatusObserver = AiUtility::CreatePSPropertyObserverL(
                                    TCallBack( HandleKeylockStateEvent, this ),
                                    KPSUidAvkonDomain,
                                    KAknKeyguardStatus );
        }
	if( !iExtHsPluginConfChange )
		{
		iExtHsPluginConfChange = AiUtility::CreatePSPropertyObserverL( 
		                            TCallBack( HandlePluginConfChange, this ),
       		                    	KPSUidAiInformation, 
       		                    	KActiveIdleExtHS_PluginConfChange );
		}
	
    iUiLoaded = ETrue;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::GetPluginsL()
// ----------------------------------------------------------------------------
//
void CNativeUiController::GetPluginsL( RAiPublisherInfoArray& aPlugins )
    {
    iPlugins.Reset();
    
    if( iRunningAsMain )
        {               
        TAiPublisherInfo shortcutPublisherInfo;
        shortcutPublisherInfo.iUid = KShortcutPluginUid;
        shortcutPublisherInfo.iName.Copy( KShortcutPluginName );
        shortcutPublisherInfo.iNamespace.Copy( KNativeUiNamespace );        
    
        User::LeaveIfError( iPlugins.Append( shortcutPublisherInfo ) );
    
        TAiPublisherInfo deviceStatusPublisherInfo;
        deviceStatusPublisherInfo.iUid = KDeviceStatusPluginUid;
        deviceStatusPublisherInfo.iName.Copy( KDeviceStatusPluginName );
        deviceStatusPublisherInfo.iNamespace.Copy( KNativeUiNamespace );
        
        User::LeaveIfError( iPlugins.Append( deviceStatusPublisherInfo ) );
    
        TAiPublisherInfo profilePublisherInfo;
        profilePublisherInfo.iUid = KProfilePluginUid;
        profilePublisherInfo.iName.Copy( KProfilePluginName );
        profilePublisherInfo.iNamespace.Copy( KNativeUiNamespace );
        
        User::LeaveIfError( iPlugins.Append( profilePublisherInfo ) );
    
        // Copy iPlugins to aPlugins
        aPlugins.ReserveL( aPlugins.Count() + iPlugins.Count() );
        
        for( TInt i = 0; i < iPlugins.Count(); ++i )
            {
            aPlugins.Append( iPlugins[i] );
            }
        }
    }

// ----------------------------------------------------------------------------
// CNativeUiController::GetSettingsL()
// ----------------------------------------------------------------------------
//
void CNativeUiController::GetSettingsL( const TAiPublisherInfo& aPubInfo,
    RAiSettingsItemArray& aSettings )
    {
    if ( aPubInfo.iNamespace != KNativeUiNamespace )
        {
        return;
        }
    
    if( iRunningAsMain && aPubInfo.iUid == KShortcutPluginUid )        
        {
        RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_PluginConfChange, 0 );
        
        MAiPluginSettings* settings = AiUtility::CreatePluginSettingsL();
        CleanupDeletePushL( settings );

        MAiPluginSettingsItem& item = settings->AiPluginSettingsItem();
        item.SetPublisherId( aPubInfo.iUid );

        HBufC* appBuf;
        appBuf = HBufC::NewLC( RProperty::KMaxPropertySize );
        TPtr appPtr = appBuf->Des();
        RProperty::Get( KPSUidAiInformation, KActiveIdleExtHS_LSKLocked, appPtr );
        if( appPtr.Length() > 0 )
            {
            item.SetKey( KNativeUiKeyIdLockedLSK );
            item.SetValueL( appPtr, EFalse );
            }
        else
            {
            item.SetKey( KNativeUiKeyIdLSK );
            item.SetValueL( KAiLocalAppAppShell, EFalse );
            }

        // Append settings into array.
        // This can not fail because space has been reserved.
        aSettings.Append( settings );

        CleanupStack::PopAndDestroy( appBuf );
        appBuf = NULL;

        CleanupStack::Pop( settings );

        settings = AiUtility::CreatePluginSettingsL();
        CleanupDeletePushL( settings );

        MAiPluginSettingsItem& item2 = settings->AiPluginSettingsItem();
        item2.SetPublisherId( aPubInfo.iUid );

        HBufC* app2Buf;
        app2Buf = HBufC::NewLC( RProperty::KMaxPropertySize );
        TPtr app2Ptr = app2Buf->Des();
        RProperty::Get( KPSUidAiInformation, KActiveIdleExtHS_RSKLocked, app2Ptr );
        if( app2Ptr.Length() > 0 )
            {
            item2.SetKey( KNativeUiKeyIdLockedRSK );
            item2.SetValueL( app2Ptr, EFalse );
            }
        else
            {
            item2.SetKey( KNativeUiKeyIdRSK );
            item2.SetValueL( KAiLocalAppContacts, EFalse );
            }
        
        // Append settings into array.
        // This can not fail because space has been reserved.
        aSettings.Append( settings );

        CleanupStack::PopAndDestroy( app2Buf );
        app2Buf = NULL;
        
        // In case there are settings in the cenrep the settings
        // here are overwritten by them
        GetSettingsFromCRL( aPubInfo, aSettings );
        CleanupStack::Pop( settings );
        }
    }

// ----------------------------------------------------------------------------
// CNativeUiController::GetSettingsFromCRL()
// ----------------------------------------------------------------------------
//
void CNativeUiController::GetSettingsFromCRL( const TAiPublisherInfo& aPubInfo,                            
    RAiSettingsItemArray &aPluginSettings )
    {
    if ( aPubInfo.iNamespace != KNativeUiNamespace )
        {
        return;
        }
    
    /*
    * The settings are stored in the cenrep starting from 0x1000.
    * 0x1000 is the name of the plugin which this setting belongs to (for example Settings/Shortcut)
    * 0x1001 is the id of the setting (for example 1)
    * 0x1002 is the value of the setting (for example localapp:0x012345678)
    *
    * So three keys per setting.
    */

    TUint32 crKey = KAIPluginSettingsKeyRangeStart;

    TBool moreSettings = ETrue;

    HBufC* pluginId = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    HBufC* settingValue = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    HBufC* settingKey = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);

    TPtr pluginIdPtr = pluginId->Des();
    TPtr settingValuePtr = settingValue->Des();
    TPtr settingKeyPtr = settingKey->Des();

    TInt32 settingId = 0;

    TInt err = KErrNone;
    TBool settingFound = EFalse;

    CRepository *settingsRepository = CRepository::NewLC( TUid::Uid( KCRUidActiveIdleLV ) );

    while ( moreSettings )
        {
        settingFound = EFalse;

        pluginIdPtr.Zero();
        settingValuePtr.Zero();
        settingKeyPtr.Zero();
        //Get the name of plugin with the Settings/ prefix
        err = settingsRepository->Get(crKey++, pluginIdPtr);

        // remove the Settings/ prefix if it is located at the start of the string
        if ( pluginIdPtr.FindC(KSettings) == 0 )
            {

            pluginIdPtr.Delete(0,
                KSettingsIdSeparator().Length() +
                KSettings().Length());
            }

        // does the setting belong to this plugin
        if ( err == KErrNone && pluginIdPtr == aPubInfo.iName )
            {
            // Get the settings id
            err = settingsRepository->Get(crKey++, settingKeyPtr);
            if ( err == KErrNone )
                {
                err = AiUtility::ParseInt(settingId,settingKeyPtr);
                }
            if ( err == KErrNone )
                {
                // Get the actual value of the setting
                err = settingsRepository->Get(crKey++, settingValuePtr);

                // Ignore possible placeholder data in cenrep
                if ( err == KErrNone && settingValuePtr.Compare( KSettingsDummyData ) != 0 )
                    {
                    // Try to find an existing setting for this
                    for ( TInt j = 0; j < aPluginSettings.Count(); j++  )
                        {
                        MAiPluginSettings *setting = aPluginSettings[j];
                        MAiPluginSettingsItem& item = setting->AiPluginSettingsItem();

                        // Existing setting found => replace it
                        if ( item.Key() == settingId && item.PublisherId() == aPubInfo.iUid )
                            {
                            item.SetValueL( settingValuePtr, EFalse );
                            settingFound = ETrue;
                            break;
                            }
                        }
                    // Existing setting not found => append new one ONLY if we
                    // are dealing with the icon overrides
                    if ( !settingFound && ( settingId & KScutFlagBitIconOverride ) )
                        {
                        MAiPluginSettings* settings = AiUtility::CreatePluginSettingsL();
                        CleanupDeletePushL( settings );
                        MAiPluginSettingsItem& item = settings->AiPluginSettingsItem();

                        item.SetPublisherId( aPubInfo.iUid );
                        item.SetKey( settingId );
                        item.SetValueL( settingValuePtr, EFalse );
                        aPluginSettings.Append( settings );

                        CleanupStack::Pop( settings );
                        }
                    }
                }
            }
        // name of the plugin not found => no more settings
        else if (err != KErrNone )
            {
            moreSettings = EFalse;

            }
        // not the correct setting for this plugin
        else
            {
            crKey += KAI2CrKeyIncrementBy2;
            }
        }
    
    CleanupStack::PopAndDestroy(settingsRepository);
    CleanupStack::PopAndDestroy(settingKey);
    CleanupStack::PopAndDestroy(settingValue);
    CleanupStack::PopAndDestroy(pluginId);
    }

// ----------------------------------------------------------------------------
// CNativeUiController::ActivateUI()
// ----------------------------------------------------------------------------
//
void CNativeUiController::ActivateUI()
    {
    if( iAppUi )
        {        
        TRAP_IGNORE( iAppUi->StartL() );            
        }
    }

// ----------------------------------------------------------------------------
// CNativeUiController::GetContentObserver()
// ----------------------------------------------------------------------------
//
MAiContentObserver& CNativeUiController::GetContentObserver()
    {
    return *this;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::SetEventHandler()
// ----------------------------------------------------------------------------
//
void CNativeUiController::SetEventHandler( MAiFwEventHandler& aEventHandler )
    {
    iFwEventHandler = &aEventHandler;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::FwEventHandler()
// ----------------------------------------------------------------------------
//
MAiFwEventHandler* CNativeUiController::FwEventHandler()
    {
    return iFwEventHandler;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::RemovePluginFromUI()
// ----------------------------------------------------------------------------
//
void CNativeUiController::RemovePluginFromUI( 
    MAiPropertyExtension& /*aPlugin*/ )
    {
    }

// ----------------------------------------------------------------------------
// CNativeUiController::MainInterface()
// ----------------------------------------------------------------------------
//
MAiMainUiController* CNativeUiController::MainInterface()
    {
    return this;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::SecondaryInterface()
// ----------------------------------------------------------------------------
//
MAiSecondaryUiController* CNativeUiController::SecondaryInterface()
    {
    return this;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::RunApplicationL()
// ----------------------------------------------------------------------------
//
void CNativeUiController::RunApplicationL()
    {
    VariateToMainUiController();

    User::LeaveIfError(
        EikStart::RunApplication( &CNativeUiController::NewApplication ) );
    }

// ----------------------------------------------------------------------------
// CNativeUiController::CoeEnv()
// ----------------------------------------------------------------------------
//
CCoeEnv& CNativeUiController::CoeEnv()
    {
    return *CCoeEnv::Static();
    }

// ----------------------------------------------------------------------------
// CNativeUiController::SetUiFrameworkObserver()
// ----------------------------------------------------------------------------
//
void CNativeUiController::SetUiFrameworkObserver( 
    MAiUiFrameworkObserver& aObserver )
    {
    iUiFrameworkObserver = &aObserver;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::IsMenuOpen()
// ----------------------------------------------------------------------------
//
TBool CNativeUiController::IsMenuOpen()
    {
    return CoeEnv().AppUi()->IsDisplayingMenuOrDialog();
    }

// ----------------------------------------------------------------------------
// CNativeUiController::SetCoeEnv()
// ----------------------------------------------------------------------------
//
void CNativeUiController::SetCoeEnv( CCoeEnv& aCoeEnv )
    {
    iCoeEnv = &aCoeEnv;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::UiFrameworkObserver()
// ----------------------------------------------------------------------------
//
MAiUiFrameworkObserver* CNativeUiController::UiFrameworkObserver()
    {
    return iUiFrameworkObserver;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::HandleResourceChange()
// ----------------------------------------------------------------------------
//
void CNativeUiController::HandleResourceChange( TInt /*aType*/ )
    {
    }

// ----------------------------------------------------------------------------
// CNativeUiController::HandleForegroundEvent()
// ----------------------------------------------------------------------------
//
void CNativeUiController::HandleForegroundEvent( TBool /*aForeground*/ )
    {
    }

// ----------------------------------------------------------------------------
// CNativeUiController::StartTransaction()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::StartTransaction( TInt /*aTxId*/ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Commit()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::Commit( TInt /*aTxId*/ )
    {
    TInt result( KErrNone );
    const TInt count( iRenderers.Count() );
    
    for( TInt i( 0 ); i < count; i++ )
        {
        TRAP( result, iRenderers[i]->TransactionCommittedL() );
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::CancelTransaction()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::CancelTransaction( TInt /*aTxId*/ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::CanPublish()
// ----------------------------------------------------------------------------
//
TBool CNativeUiController::CanPublish( MAiPropertyExtension& aPlugin,
    TInt /*aContent*/, TInt /*aIndex*/ )                                      
    {    
    const TAiPublisherInfo* info( NULL );
    
    TRAP_IGNORE( info = aPlugin.PublisherInfoL() );
       
    if ( !info || info->iNamespace != KNativeUiNamespace )
        {
        return EFalse;
        }
    
    for( TInt i = 0; i < iPlugins.Count(); i++ )
        {               
        if ( iPlugins[i] == *info )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Publish()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::Publish( MAiPropertyExtension& aPlugin,
    TInt aContent, TInt aResource, TInt aIndex )
    {
    TInt err = DoPublish( aPlugin, aContent, aResource, aIndex );
    return err;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Publish()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::Publish( MAiPropertyExtension& aPlugin,
    TInt aContent, const TDesC16& aText, TInt aIndex )        
    {
    TInt err = DoPublish( aPlugin, aContent, aText, aIndex );
    return err;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Publish()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::Publish( MAiPropertyExtension& aPlugin,
    TInt aContent, const TDesC8& aBuf, TInt aIndex )
    {
    TInt err = DoPublish( aPlugin, aContent, aBuf, aIndex );
    return err;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Publish()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::Publish( MAiPropertyExtension& aPlugin,                                     
    TInt aContent, RFile& aFile, TInt aIndex )                                   
    {
    TInt err = DoPublish( aPlugin, aContent, aFile, aIndex );
    return err;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Clean()
// ----------------------------------------------------------------------------
//
TInt CNativeUiController::Clean( MAiPropertyExtension& aPlugin, 
    TInt aContent, TInt /*aIndex*/ )
    {
    const TInt count( iRenderers.Count() ); 
    
    for( TInt i( 0 ); i < count; i++ )
        {
        iRenderers[i]->Clean( aPlugin, aContent );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::Extension()
// ----------------------------------------------------------------------------
//
TAny* CNativeUiController::Extension( TUid /*aUid*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::RequiresSubscription()
// ----------------------------------------------------------------------------
//
TBool CNativeUiController::RequiresSubscription( 
    const TAiPublisherInfo& aPublisherInfo ) const
    {
    if ( aPublisherInfo.iNamespace == KNativeUiNamespace )
        {
        // Targeted to this content renderer
        return ETrue;
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CNativeUiController::NewApplication()
// ----------------------------------------------------------------------------
//
CApaApplication* CNativeUiController::NewApplication()
    {
    CNativeUiController* self = 
        static_cast< CNativeUiController* >( Dll::Tls() );
  
    return CApplication::New( self );
    }

// ----------------------------------------------------------------------------
// CNativeUiController::VariateToMainUiController()
// ----------------------------------------------------------------------------
//
void CNativeUiController::VariateToMainUiController()
    {
    iRunningAsMain = ETrue;
    }

// ======== GLOBAL FUNCTIONS ========
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
    return KImplementationTable;
    }

// End of File.
