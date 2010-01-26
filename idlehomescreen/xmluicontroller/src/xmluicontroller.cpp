/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XML UI Controller main class
*
*/

// System includes
#include <ecom/implementationproxy.h>
#include <AknQueryDialog.h>
#include <eikstart.h>
#include <apgtask.h>
#include <centralrepository.h>
#include <e32property.h>

// User includes
#include <activeidle2domainpskeys.h>
#include <activeidle2domaincrkeys.h>
#include <aiscutplugindomaincrkeys.h>
#include <aisystemuids.hrh>
#include <ai3xmlui.rsg>

#include "xmluicontroller.h"

#include "xnuiengineappif.h"
#include "xnnodeappif.h"
#include "xnproperty.h"
#include "xntype.h"
#include "aistrcnv.h"

#include "application.h"
#include "appui.h"
#include "xmlnodeidgenerator.h"
#include "aixuikoneventhandler.h"
#include "aixmluiutils.h"
#include "aiutility.h"
#include "aipluginsettings.h"
#include "aixmluiconstants.h"
#include "aifweventhandler.h"
#include "debug.h"
#include "aiconsts.h"
#include "contentrenderer.h"
#include "xmluicontrollerpanic.h"
#include "pmodtiterator.h"
#include "contentpublisher.h"
#include "xnplugindefs.h"
#include "ainativeuiplugins.h"


using namespace AiXmlUiController;

const TInt KOneSecondInMicroS = 1000*1000;
const TInt KAI2CrKeyIncrementBy2 = 2;

typedef TBuf<32> TNamespace;

_LIT( KSettingsDummyData, "" );

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// ResolveEventParameters()
// ----------------------------------------------------------------------------
//
static void ResolveEventParameters( const TDesC8& aEventString, 
    TPtrC8& aParam1, TPtrC8& aParam2 )
    {
    TInt  paramPos( aEventString.Locate( KEventParameterSeparator ) );
    
    TLex8 lex( aEventString.Mid( ++paramPos ) );
    
    aParam1.Set( lex.NextToken() );   // first ui element id
    
    TInt commaPos( aParam1.LocateReverse( KComma ) );
    
    if( commaPos != KErrNotFound )
        {
        // Strip comma
        aParam1.Set( aParam1.Left( commaPos ) );
        
        aParam2.Set( lex.NextToken() );   // second ui element id
        
        TInt rightParenthesis( aParam2.LocateReverse( KRightParenthesis ) );
        
        if( rightParenthesis != KErrNotFound )
            {
            aParam2.Set( aParam2.Left( rightParenthesis ) );
            }        
        }
    else
        {
        // Strip right parenthesis
        aParam1.Set( aParam1.Left( aParam1.Length() -1 ) );
        }
    }

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CXmlUiController::CXmlUiController()
// ----------------------------------------------------------------------------
//
CXmlUiController::CXmlUiController()
    {
    }

// ----------------------------------------------------------------------------
// CXmlUiController::ConstructL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::ConstructL()
    {       
    iNodeIdGenerator = CXmlNodeIdGenerator::NewL();  
    
    iAISettingsRepository = 
        CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) ); 
        
    // Indicate to wserv plugin if it launches logs on send key or not              
    RProperty::Set( KPSUidAiInformation, KActiveIdleActOnSendKey, ETrue );                     
    }

// ----------------------------------------------------------------------------
// CXmlUiController::NewL()
// ----------------------------------------------------------------------------
//
CXmlUiController* CXmlUiController::NewL()
    {
    CXmlUiController* self = new ( ELeave ) CXmlUiController;

    CleanupStack::PushL( self );
    self->ConstructL();
    
    // Set XML UI Controller object to TLS for access in static
    // CXmlUiController::NewApplication
    User::LeaveIfError( Dll::SetTls( self ) );
    
    CleanupStack::Pop( self ); 

    return self;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::~CXmlUiController()
// ----------------------------------------------------------------------------
//
CXmlUiController::~CXmlUiController()
    {
    delete iNodeIdGenerator;
    delete iExitTimer;
    delete iAISettingsRepository;
    delete iCPSpublisher;
    
    // Reset TLS pointer to this set in NewL
    Dll::SetTls( NULL );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::Exit()
// ----------------------------------------------------------------------------
//
void CXmlUiController::Exit()
    {
    if( iExitTimer )
        {
        iExitTimer->Cancel();
        
        iExitTimer->Start( 0, KOneSecondInMicroS,           
           TCallBack( ExitTimerCallBack, this ) );
        }
    }

// ----------------------------------------------------------------------------
// CXmlUiController::ExitTimerCallBack()
// ----------------------------------------------------------------------------
//
TInt CXmlUiController::ExitTimerCallBack(TAny *aSelf)
    {
     CXmlUiController* self =
         static_cast< CXmlUiController* >( aSelf );
    
    if( self )
        {
        self->iExitTimer->Cancel();
        
        self->iAppUi->Exit();        
        }
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::SetObserver()
// ----------------------------------------------------------------------------
//
void CXmlUiController::SetObserver( MAiContentObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::SetAppUi()
// ----------------------------------------------------------------------------
//
void CXmlUiController::SetAppUi( CAppUi& aAppUi )
    {
    iAppUi = &aAppUi;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::AppUi()
// ----------------------------------------------------------------------------
//
CAppUi* CXmlUiController::AppUi() const
    {
    return iAppUi;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::UiEngineL()
// ----------------------------------------------------------------------------
//
TXnUiEngineAppIf* CXmlUiController::UiEngineL() const
    {
    return iAppUi ? iAppUi->UiEngineL() : NULL;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::SettingsRepository()
// ----------------------------------------------------------------------------
//
CRepository& CXmlUiController::SettingsRepository() const
    {
    return *iAISettingsRepository;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::CreateXuikonEventHandlerL()
// ----------------------------------------------------------------------------
//
CAIXuikonEventHandler* CXmlUiController::CreateXuikonEventHandlerL(
    CContentRenderer& aRenderer )
    {
    CAIXuikonEventHandler* eventHandler = 
        CAIXuikonEventHandler::NewL( *this, aRenderer );
    
    eventHandler->SetFwEventHandler( iFwEventHandler );
    eventHandler->SetUiEventHandler( this );
    
    return eventHandler;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::NotifyAppEnvReadyL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::NotifyAppEnvReadyL()
    {
    if( iFwEventHandler )
        {
        iFwEventHandler->AppEnvReadyL();
        iFwEventHandler->HandleUiReadyEventL( *this );
        }
    
    if( !iCPSpublisher )
        {
        iCPSpublisher = CContentPublisher::NewL( *this );
        }
    
    if( !iExitTimer )
        {
        iExitTimer = CPeriodic::NewL( CActive::EPriorityStandard );        
        }
    }

// ----------------------------------------------------------------------------
// CXmlUiController::LoadUIDefinition()
// ----------------------------------------------------------------------------
//
void CXmlUiController::LoadUIDefinitionL()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetPluginsL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::GetPluginsL( RAiPublisherInfoArray& /*aPlugins*/ )
    {   
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CXmlUiController::PublisherInfo()
// ----------------------------------------------------------------------------
//
void CXmlUiController::PublisherInfoL( RAiPublisherInfoArray& aPlugins )
    {
    // This method returns plugins with namespace KNativeUiNamespace
    if ( iRunningAsMain )
        {
        // This plugin is used to publish data on secondary ui controller side       
        TAiPublisherInfo deviceStatus;
        deviceStatus.iUid = KDeviceStatusPluginUid;
        deviceStatus.iName.Copy( KDeviceStatusPluginName );
        deviceStatus.iNamespace.Copy( KNativeUiNamespace );
                
        aPlugins.AppendL( deviceStatus );               
        }        
    }

// ----------------------------------------------------------------------------
// CXmlUiController::PublisherInfoL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::PublisherInfoL( CXnNodeAppIf& aSource,
    TAiPublisherInfo& aInfo )
    {
    // Fetch uid from XML and convert it to TUid
    const TDesC8* uid( PropertyValue( aSource, 
        AiUiDef::xml::property::KValue ) ); 

    if ( !uid )
        {
        return;
        }
        
    _LIT8( KPrefix, "0x" );
    
    const TInt pos( uid->FindF( KPrefix ) );

    if( pos != KErrNotFound )
         {
         TLex8 lex( uid->Mid( pos + KPrefix().Length() ) );
          
         // Hex parsing needs unsigned int
         TUint32 value = 0;
         const TInt parseResult = lex.Val( value, EHex );
         
         if ( parseResult == KErrNone )
             {
             TInt32 value32( value );
             
             aInfo.iUid = TUid::Uid( value32 );            
             }
         }
     
    // Fetch plug-in name from XML
    HBufC* pluginName( PropertyValueL( aSource, 
                AiUiDef::xml::property::KName ) ); 
            
    CleanupStack::PushL( pluginName );

    if ( pluginName )
        {
        aInfo.iName.Copy( *pluginName );
        }
    
    CleanupStack::PopAndDestroy( pluginName );
        
    aInfo.iNamespace.Copy( aSource.Namespace() );    
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetSettingsL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::GetSettingsL( const TAiPublisherInfo& aPubInfo,     
    RAiSettingsItemArray& aSettings )
    {
    if ( aPubInfo.iNamespace == KNativeUiNamespace )
        {
        // This controller doesn't provide settings for native ui namespace
        return;
        }
    
    const TDesC& classId( iNodeIdGenerator->SettingsNodeIdL( aPubInfo ) );
    
    TNamespace ns;
    
    ns.Copy( aPubInfo.iNamespace );
       
    // Find settings nodes
    RPointerArray< CXnNodeAppIf > nodes( 
        UiEngineL()->FindNodeByClassL( classId, ns ) );
    
    CleanupClosePushL( nodes );
    
    // Reserve space for settings
    aSettings.ReserveL( aSettings.Count() + nodes.Count() );
    
    // Collect settings
    TInt count( nodes.Count() );
    
    for( TInt i = 0; i < count; ++i )
        {
        CXnNodeAppIf* node( nodes[i] );
        
        const TDesC8* name( 
                PropertyValue( *node, AiUiDef::xml::property::KName ) );
        
        HBufC* value( 
                PropertyValueL( *node, AiUiDef::xml::property::KValue ) );        
        
        CleanupStack::PushL( value );
        
        if( name && value )
            {
            MAiPluginSettings* settings( AiUtility::CreatePluginSettingsL() );
            CleanupDeletePushL( settings );
            
            MAiPluginSettingsItem& item( settings->AiPluginSettingsItem() ); 
            item.SetPublisherId( aPubInfo.iUid );
            
            TInt32 key( 0 );
            
            User::LeaveIfError( AiUtility::ParseInt( key, *name ) );
            
            item.SetKey( key );            
            item.SetValueL( *value, EFalse );
            
            // Append settings into array.
            // This can not fail because space has been reserved.
            aSettings.Append( settings );            
            CleanupStack::Pop( settings );
            }
            
        CleanupStack::PopAndDestroy( value );
        }
    
    CleanupStack::PopAndDestroy( &nodes );
    
    GetSettingsFromCRL( aPubInfo, aSettings );
    GetContentModelL( aPubInfo, aSettings );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetSettingsFromCRL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::GetSettingsFromCRL( const TAiPublisherInfo& aPubInfo, 
    RAiSettingsItemArray &aPluginSettings )                            
    {
    if ( aPubInfo.iNamespace == KNativeUiNamespace )
        {
        // This controller doesn't provide settings for native ui namespace
        return;
        }
    
    /*
    * The settings are stored in the cenrep starting from 0x1000. 
    * 0x1000 is the name of the plugin which this setting belongs to (for example Settings/Shortcut)
    * 0x1001 is the id of the setting (for example 1)
    * 0x1002 is the value of the setting (for example localapp:0x012345678)
    * 
    * So three keys per setting.
    * 
    * Settings from cenrep override those from XML
    */
    
    TUint32 crKey( KAIPluginSettingsKeyRangeStart );

    TBool moreSettings( ETrue );
    
    HBufC* pluginId = HBufC::NewLC(
            NCentralRepositoryConstants::KMaxUnicodeStringLength );
    
    HBufC* settingValue = HBufC::NewLC(
            NCentralRepositoryConstants::KMaxUnicodeStringLength );
    
    HBufC* settingKey = HBufC::NewLC(
            NCentralRepositoryConstants::KMaxUnicodeStringLength );
    
    TPtr pluginIdPtr( pluginId->Des() );
    TPtr settingValuePtr( settingValue->Des() );
    TPtr settingKeyPtr( settingKey->Des() );
    
    TInt32 settingId( 0 );

    TInt err( KErrNone );
    TBool settingFound( EFalse );
    
    while( moreSettings )
        {
        settingFound = EFalse;
        
        pluginIdPtr.Zero();
        settingValuePtr.Zero();
        settingKeyPtr.Zero();
        
        //Get the name of plugin with the Settings/ prefix
        err = iAISettingsRepository->Get( crKey++, pluginIdPtr );       
   
        // remove the Settings/ prefix if it is located at the start of the string
        if( pluginIdPtr.FindC( AiUiDef::xml::propertyClass::KSettings ) == 0 )
            {   
            pluginIdPtr.Delete( 0, 
                    AiUiDef::xml::id::KSettingsIdSeparator().Length() +
                    AiUiDef::xml::propertyClass::KSettings().Length() );               
            }
            
        // does the setting belong to this plugin
        if( err == KErrNone && pluginIdPtr == aPubInfo.iName )
            {
            // Get the settings id 
            err = iAISettingsRepository->Get( crKey++, settingKeyPtr );
            
            if( err == KErrNone )
                {
                err = AiUtility::ParseInt( settingId,settingKeyPtr );
                }
            
            if( err == KErrNone )
                {                
                // Get the actual value of the setting
                err = iAISettingsRepository->Get( crKey++, settingValuePtr );    
                
                // Ignore possible placeholder data in cenrep                                
                if( err == KErrNone && settingValuePtr.Compare( KSettingsDummyData ) != 0 )
                    {
                    // Try to find an existing setting for this
                    for( TInt j = 0; j < aPluginSettings.Count(); j++  )
                        {
                        MAiPluginSettings* setting( aPluginSettings[j] );                         
                        MAiPluginSettingsItem& item( setting->AiPluginSettingsItem() );                       
                        
                        // Existing setting found => replace it
                        if( item.Key() == settingId && item.PublisherId() == aPubInfo.iUid )
                            {
                            item.SetValueL( settingValuePtr, EFalse );
                            settingFound = ETrue;
                            break;
                            }
                        }
                    
                    // Existing setting not found => append new one ONLY if we
                    // are dealing with the icon overrides or toolbar shortcuts
                    if( !settingFound && 
                       ( ( settingId & KScutFlagBitToolbarShortcut ) || 
                         ( settingId & KScutFlagBitIconOverride ) ) )
                        {
                        MAiPluginSettings* settings( AiUtility::CreatePluginSettingsL() );
                        CleanupDeletePushL( settings );
                        
                        MAiPluginSettingsItem& item( settings->AiPluginSettingsItem() );
                        
                        item.SetPublisherId( aPubInfo.iUid );
                        item.SetKey( settingId );                
                        item.SetValueL( settingValuePtr, EFalse );                                            
                        
                        aPluginSettings.Append( settings );                        
                        CleanupStack::Pop( settings );
                        }
                    }
                }                                 
            else
                {
                // no settings id found => invalid settings in cenrep
                __PRINT8( __DBG_FORMAT8("CXmlUiController::GetSettingsFromCRL: ERROR: invalid settings. key: %d pluginname: %S id: %d value: %S"), crKey, pluginId, settingId, settingValue );                    
                }            
            }               
        else if( err != KErrNone )
            {
            // name of the plugin not found => no more settings
            moreSettings = EFalse;            
            }                          
        else
            {
            // not the correct setting for this plugin
            crKey += KAI2CrKeyIncrementBy2;
            }
        }

    CleanupStack::PopAndDestroy( 3, pluginId ); // settingValue, settingKey
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetContentModelL()
// ----------------------------------------------------------------------------
//
// ContentModelL()
void CXmlUiController::GetContentModelL( const TAiPublisherInfo& aPubInfo,         
    RAiSettingsItemArray& aSettings )
    {
    if ( aPubInfo.iNamespace == KNativeUiNamespace )
        {
        // This controller doesn't provide content model for native ui namespace
        return;
        }
    
    // Find the node for the publisher
	// TODO Does not work if widget is in view  
    RPointerArray<CXnNodeAppIf> list( 
        UiEngineL()->FindContentSourceNodesL( aPubInfo.iNamespace ) );
    CleanupClosePushL( list );
    
    CXnNodeAppIf* publisherNode( NULL );
    
    if ( list.Count() > 0 )
        {
        publisherNode = list[0];
        }
         
    CleanupStack::PopAndDestroy( &list );
    
    if( !publisherNode )
        {
        return;
        }
    
    // Find ui plugin node for the data plugin
    CXnNodeAppIf* parentNode( publisherNode->ParentL() );
    
    // parent must be correct type
    if( parentNode->InternalDomNodeType() == _L8("widget") )
        {
        // Get plugin configurations
        GetConfigurationsL( *parentNode, aSettings, AiUiDef::xml::element::K16Plugin());
        CPmODTIterator* iter = CPmODTIterator::NewL( *parentNode );
        CleanupStack::PushL( iter );
        CXnNodeAppIf* node = iter->First();
        while( node )
            {
            const TDesC8& nodeType = node->Type()->Type();
            if( nodeType == XnPropertyNames::action::KActions)
                {
                node = iter->SkipBranchL();
                }
            // Get the content model and configuration for the supported elements
            else if( nodeType == AiUiDef::xml::element::KImage ||
                     nodeType == AiUiDef::xml::element::KText ||
                     nodeType == AiUiDef::xml::element::KNewsTicker ||
                     nodeType == AiUiDef::xml::element::KAnimation )
                {
                // Is created in GetContenItem and used in GetConfigurationsL
                HBufC* confOwner( NULL );
                // get content item for the element, confOwner is filled
                GetContentItemL( *node, aSettings, confOwner );
                if( confOwner )
                    {
                    CleanupStack::PushL( confOwner );
                    // get configurations for the element
                    GetConfigurationsL( *node, aSettings, *confOwner );
                    CleanupStack::PopAndDestroy( confOwner );
                    }
                node = iter->SkipBranchL();
                }
            else
                {
                node = iter->NextL();
                }
            }
        CleanupStack::PopAndDestroy( iter );
        }
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetContentItemL()
// ----------------------------------------------------------------------------
//
// ContentItemL()
void CXmlUiController::GetContentItemL( CXnNodeAppIf& aNode, 
    RAiSettingsItemArray& aSettings, HBufC*& aItemName )
    {
    // Find property node
    RPointerArray<CXnNodeAppIf> childNodes( aNode.ChildrenL() );
    CleanupClosePushL( childNodes );
    
    TInt count( childNodes.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        CXnNodeAppIf* node( childNodes[i] );
        
        if( node->Type()->Type() == XnPropertyNames::action::KProperty )
            {
            HBufC* name = PropertyValueL( *node, AiUiDef::xml::property::KClass );
            CleanupStack::PushL( name );
        
            if( name )
                {
                // Content model found for the element, create content item
                MAiPluginSettings* settings = AiUtility::CreatePluginSettingsL();
                CleanupDeletePushL( settings );
                
                MAiPluginContentItem& item = settings->AiPluginContentItem();
                
                // Type of the element is needed in content model 
                HBufC* type( NULL ); 
                type = AiUtility::CopyToBufferL( type, aNode.Type()->Type());
                CleanupStack::PushL( type );
                item.SetTypeL( *type );
                CleanupStack::PopAndDestroy( type );
                
                item.SetNameL( *name );
                
                aItemName = name;
                aSettings.AppendL( settings );
                CleanupStack::Pop( settings );
                }
            
            // Ownership is given to aItemName
            CleanupStack::Pop( name );
            
            // First property element, which has class attribute is selected
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( &childNodes );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetConfigurationsL()
// ----------------------------------------------------------------------------
//
// ConfigurationItemsL()
void CXmlUiController::GetConfigurationsL( CXnNodeAppIf& aNode, 
    RAiSettingsItemArray& aSettings, const TDesC& aConfOwner  )
    {    
    // Find configuration nodes
    RPointerArray< CXnNodeAppIf > nodes( aNode.ChildrenL() );
    CleanupClosePushL( nodes );

    // Collect settings
    TInt count( nodes.Count() );
    
    for( TInt j = 0; j < count; j++ )
        {
        CXnNodeAppIf* node( nodes[j] );
        
        if( node->Type()->Type() == AiUiDef::xml::element::KConfiguration )
            {
            HBufC* name( PropertyValueL( *node, AiUiDef::xml::property::KName ) );
            CleanupStack::PushL( name );

            HBufC* value( PropertyValueL( *node, AiUiDef::xml::property::KValue ) );
            CleanupStack::PushL( value );

            MAiPluginSettings* settings = AiUtility::CreatePluginSettingsL();
            CleanupDeletePushL( settings );
               
            MAiPluginConfigurationItem& item( settings->AiPluginConfigurationItem() );
     
            item.SetOwnerL( aConfOwner );
            item.SetNameL( *name );
            item.SetValueL( *value );
            
            // Append settings into array.
            aSettings.AppendL( settings );
        
            CleanupStack::Pop( settings );
            CleanupStack::PopAndDestroy( value );    
            CleanupStack::PopAndDestroy( name );        
            }
        }
    
    CleanupStack::PopAndDestroy( &nodes );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::ActivateUI()
// ----------------------------------------------------------------------------
//
void CXmlUiController::ActivateUI()
    {
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetContentObserver()
// ----------------------------------------------------------------------------
//
MAiContentObserver& CXmlUiController::GetContentObserver()
    {
    return *iObserver;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::SetEventHandler()
// ----------------------------------------------------------------------------
//
void CXmlUiController::SetEventHandler( MAiFwEventHandler& aFwEventHandler )
    {
    iFwEventHandler = &aFwEventHandler;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::RemovePluginFromUI()
// ----------------------------------------------------------------------------
//
void CXmlUiController::RemovePluginFromUI( MAiPropertyExtension& aPlugin )
    {
    CContentRenderer* contentRenderer = 
        static_cast< CContentRenderer* >( iObserver );
    
    if ( contentRenderer )
        {
        contentRenderer->CleanPluginFromUi( aPlugin );
        }
    }

// ----------------------------------------------------------------------------
// CXmlUiController::FwEventHandler()
// ----------------------------------------------------------------------------
//
MAiFwEventHandler* CXmlUiController::FwEventHandler() const
    {
    return iFwEventHandler;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::MainInterface()
// ----------------------------------------------------------------------------
//
MAiMainUiController* CXmlUiController::MainInterface()
    {
    return this;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::SecondaryInterface()
// ----------------------------------------------------------------------------
//
MAiSecondaryUiController* CXmlUiController::SecondaryInterface()
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::RunApplicationL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::RunApplicationL()
    {
    iRunningAsMain = ETrue;
    
    User::LeaveIfError( 
        EikStart::RunApplication( &CXmlUiController::NewApplication ) );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::CoeEnv()
// ----------------------------------------------------------------------------
//
CCoeEnv& CXmlUiController::CoeEnv()
    {
    __ASSERT_ALWAYS( iAppUi, Panic(ECriticalPointerNull) );
    return *iAppUi->CoeEnv();
    }

// ----------------------------------------------------------------------------
// CXmlUiController::SetUiFrameworkObserver()
// ----------------------------------------------------------------------------
//
void CXmlUiController::SetUiFrameworkObserver( 
    MAiUiFrameworkObserver& aObserver )
    {
    iUiFrameworkObserver = &aObserver;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::IsMenuOpen()
// ----------------------------------------------------------------------------
//
TBool CXmlUiController::IsMenuOpen()
    {
    TBool menuOpen( EFalse );
    TRAP_IGNORE( menuOpen = UiEngineL()->IsMenuDisplaying() );
    return menuOpen;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::UiFrameworkObserver()
// ----------------------------------------------------------------------------
//
MAiUiFrameworkObserver* CXmlUiController::UiFrameworkObserver() const
    {
    return iUiFrameworkObserver;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::HandleUiEvent()
// ----------------------------------------------------------------------------
//
TBool CXmlUiController::HandleUiEvent( TAny* aEvent, const TDesC8& aParam )
    {
    CXnNodeAppIf* event( static_cast<CXnNodeAppIf*>( aEvent ) );
    
    TBool retval( EFalse );
    
    if( event )
        {
        TPtrC8 param1;
        TPtrC8 param2;
        const TDesC8& eventString(
                aParam.Mid( AiUiDef::xml::event::KUiEventPrefix().Length() ) );
        
        // Extract event name
        TInt  paramPos( eventString.Locate( KEventParameterSeparator ) );
        
        const TDesC8& eventName( eventString.Left( Max( 0, paramPos ) ) );
    
        if( eventName == AiUiDef::xml::event::KFocusGained )
            {
            // Resolve event parameters
            ResolveEventParameters( eventString, param1, param2 );
            
            TRAP_IGNORE( HandleFocusGainedL( param1, param2, *event ) );    
            
            retval = ETrue;
            }
        else if( eventName == AiUiDef::xml::event::KSetElementSizeToCPS )
            {
            // Resolve event parameters
            ResolveEventParameters( eventString, param1, param2 );
            
            TRAP_IGNORE( HandleSetElementSizeL( param1, *event ));
            
            retval = ETrue;
            }
        }
    
    return retval;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::HandleSetElementSizeL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::HandleSetElementSizeL( const TDesC8& aElementName, 
    CXnNodeAppIf& aOrigin )
    {
    __UHEAP_MARK;
    // Resolve ui elements
    CXnNodeAppIf* element( 
            UiEngineL()->FindNodeByIdL( aElementName, aOrigin.Namespace() ) );
    
    LeaveIfNull( element, KErrNotFound );

    TRect rect( element->Rect() );
    
    HBufC* elementName( NULL );
    elementName = AiUtility::CopyToBufferL( elementName, aElementName );
    CleanupStack::PushL( elementName );
    
    iCPSpublisher->PublishSizeL( *elementName, rect.Width(), rect.Height() );
    
    CleanupStack::PopAndDestroy( elementName );
    
    __UHEAP_MARKEND;    
    }

// ----------------------------------------------------------------------------
// CXmlUiController::HandleFocusGainedL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::HandleFocusGainedL( const TDesC8& aUiElement1,
    const TDesC8& aUiElement2, CXnNodeAppIf& aOrigin )
    {
    // Resolve ui elements
    CXnNodeAppIf* element1( 
        UiEngineL()->FindNodeByIdL( aUiElement1, aOrigin.Namespace() ) );
    
    LeaveIfNull( element1, KErrNotFound );
    
    CXnNodeAppIf* element2( 
        UiEngineL()->FindNodeByIdL( aUiElement2, aOrigin.Namespace() ) );
    
    LeaveIfNull( element2, KErrNotFound );
    
    // swap display properties between ui elements
    CXnProperty* display1( 
        element1->GetPropertyL( XnPropertyNames::style::common::KDisplay ) );
    
    LeaveIfNull( display1, KErrNotFound );
    
    CXnProperty* display2(
        element2->GetPropertyL( XnPropertyNames::style::common::KDisplay ) );
    
    LeaveIfNull( display2, KErrNotFound );
    
    display1 = display1->CloneL();
    CleanupStack::PushL( display1 );
    
    display2 = display2->CloneL();
    CleanupStack::PushL( display2 );
    
    element1->SetPropertyL( display2 );
    CleanupStack::Pop( display2 );
    
    element2->SetPropertyL( display1 );
    CleanupStack::Pop( display1 );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::NewApplication()
// ----------------------------------------------------------------------------
//
CApaApplication* CXmlUiController::NewApplication()
    {
    CXmlUiController* self = static_cast<CXmlUiController*>(Dll::Tls());
    __ASSERT_ALWAYS( self, Panic(ECriticalPointerNull) );
    return CApplication::New(*self);
    }

const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidXmlUiController, CXmlUiController::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
    }

