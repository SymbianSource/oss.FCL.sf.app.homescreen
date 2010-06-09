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
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
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
#include "contentpublisher.h"
#include "xnplugindefs.h"
#include "ainativeuiplugins.h"

// Constants
const TInt KOneSecondInMicroS = 1000*1000;

// Macros
#define IS_KNOWN_CONTENT_MODEL_ELEMENT( e ) \
    ( e == AiUiDef::xml::element::KImage || \
      e == AiUiDef::xml::element::KText || \
      e == AiUiDef::xml::element::KAnimation || \
      e == AiUiDef::xml::element::KTextEditor || \
      e == AiUiDef::xml::element::KTitle || \
      e == AiUiDef::xml::element::KData )
    
using namespace AiXmlUiController;

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// BuildModelL()
// ----------------------------------------------------------------------------
//
static void BuildModelL( CXnNodeAppIf* aParent, const TDesC8& aKey, 
    const TDesC8& aNs, RPointerArray< CXnNodeAppIf >& aModel )     
    {
    if ( !aParent || aParent->Namespace() != aNs )
        {
        // No parent or traversed out from the namespace
        return;
        }

    if ( aParent->Type()->Type() == XnPropertyNames::action::KProperty )
        {
        CXnProperty* prop( 
            aParent->GetPropertyL( XnPropertyNames::common::KClass ) );
        
        if ( prop )
            {
            const TDesC8& value( prop->StringValue() );
                        
            if ( value.Find( aKey ) != KErrNotFound )
                {
                aModel.AppendL( aParent );
                }                                  
            }        
        }
    
    RPointerArray< CXnNodeAppIf > children( aParent->ChildrenL() );
    CleanupClosePushL( children );
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        // Recurse children
        BuildModelL( children[i], aKey, aNs, aModel );
        }
    
    CleanupStack::PopAndDestroy( &children );             
    }

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
    iCPSpublisher = CContentPublisher::NewL( *this );
    
    iExitTimer = CPeriodic::NewL( CActive::EPriorityStandard );    

    if( iFwEventHandler )
        {
        iFwEventHandler->AppEnvReadyL();        
        }          
    }

// ----------------------------------------------------------------------------
// CXmlUiController::LoadUIDefinition()
// ----------------------------------------------------------------------------
//
void CXmlUiController::LoadUIDefinitionL()
    {          
    }

// ----------------------------------------------------------------------------
// CXmlUiController::PublisherInfoL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::PublisherInfoL( CXnNodeAppIf& aSource,
    THsPublisherInfo& aInfo )
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

    TInt32 uidValue( 0 );
    
    if( pos != KErrNotFound )
         {
         TLex8 lex( uid->Mid( pos + KPrefix().Length() ) );
          
         // Hex parsing needs unsigned int
         TUint32 value( 0 );
         TInt err( lex.Val( value, EHex ) );
         
         if ( err == KErrNone )
             {
             uidValue = value;
             }
         }
     
    // Fetch plug-in name from XML
    HBufC* name( PropertyValueL( aSource, AiUiDef::xml::property::KName ) ); 
                
    TUid pluginuid( TUid::Uid( uidValue ) );
           
    aInfo = THsPublisherInfo( pluginuid, *name, aSource.Namespace() );
    
    delete name;
    name = NULL;
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetSettingsL()
//
// ----------------------------------------------------------------------------
//
void CXmlUiController::GetSettingsL( const THsPublisherInfo& aPublisherInfo,     
    RAiSettingsItemArray& aSettings )
    {
    if ( aPublisherInfo.Namespace() == KNativeUiNamespace )
        {
        // This controller doesn't provide settings for native ui namespace
        return;
        }
    
    const TDesC& classId( 
        iNodeIdGenerator->SettingsNodeIdL( aPublisherInfo ) ); 
               
    TBuf< KHsPublisherNamespaceMaxLength > ns;
    
    ns.Copy( aPublisherInfo.Namespace() );
    
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
            item.SetPublisherId( aPublisherInfo.Uid() );
            
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
    
    GetContentModelL( aPublisherInfo, aSettings );
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetContentModelL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::GetContentModelL( const THsPublisherInfo& aPublisherInfo,         
    RAiSettingsItemArray& aSettings )
    {
    if ( aPublisherInfo.Namespace() == KNativeUiNamespace )
        {
        // This controller doesn't provide content model for native ui namespace
        return;
        }
    
    // Find the node for the publisher	
    RPointerArray<CXnNodeAppIf> list( 
        UiEngineL()->FindContentSourceNodesL( aPublisherInfo.Namespace() ) );
    
    CleanupClosePushL( list );
    
    CXnNodeAppIf* publisher( NULL );
    
    if ( list.Count() > 0 )
        {
        publisher = list[0];
        }
         
    CleanupStack::PopAndDestroy( &list );
    
    if( !publisher )
        {
        return;
        }
    
    // Find plugin node for the <contentsource> element 
    CXnNodeAppIf* parent( publisher->ParentL() );
    
    if ( parent && parent->InternalDomNodeType() == XnPropertyNames::KWidget )
        {
        // Get <contentsource> element configurations
        GetConfigurationsL( 
            *parent, aSettings, AiUiDef::xml::element::K16Plugin() );
            
        RPointerArray< CXnNodeAppIf > model;
        CleanupClosePushL( model );
        
        HBufC8* key = HBufC8::NewLC( aPublisherInfo.Name().Length() + 1 );
        
        _LIT( KSlash, "/" );
        
        key->Des().Copy( aPublisherInfo.Name() );
        key->Des().Append( KSlash );
                
        BuildModelL( parent, *key, parent->Namespace(), model );
        
        for ( TInt i = 0; i < model.Count(); i++ )
            {            
            CXnNodeAppIf* parent( model[i]->ParentL() );
            
            if ( parent )
                {
                const TDesC8& type( parent->Type()->Type() );
                
                if ( IS_KNOWN_CONTENT_MODEL_ELEMENT( type ) )
                    {                
                    // Content model found for the element, create content item
                    MAiPluginSettings* settings( 
                        AiUtility::CreatePluginSettingsL() );
                    
                    CleanupDeletePushL( settings );
                                        
                    MAiPluginContentItem& item( 
                        settings->AiPluginContentItem() );
                                        
                    // Type of the element is needed in content model                     
                    HBufC* type16( NULL );
                    
                    type16 = AiUtility::CopyToBufferL( type16, type ); 
                                            
                    CleanupStack::PushL( type16 );
                    
                    item.SetTypeL( *type16 );
                                                           
                    HBufC* name( PropertyValueL( 
                        *model[i], AiUiDef::xml::property::KClass ) );
                    CleanupStack::PushL( name );
                    
                    item.SetNameL( *name );
                                                                                              
                    // Get content model configurations, 
                    // i.e. <property> element's <configuration> siblings
                    GetConfigurationsL( *parent, aSettings, *name );
                    
                    CleanupStack::PopAndDestroy( 2, type16 ); // name
                    
                    aSettings.AppendL( settings );
                    CleanupStack::Pop( settings );
                    }
                }           
            }
        
        CleanupStack::PopAndDestroy( 2, &model ); // key
        }   
    }

// ----------------------------------------------------------------------------
// CXmlUiController::GetConfigurationsL()
// ----------------------------------------------------------------------------
//
void CXmlUiController::GetConfigurationsL( CXnNodeAppIf& aNode, 
    RAiSettingsItemArray& aSettings, const TDesC& aConfOwner  )
    {    
    // Find configuration nodes
    RPointerArray< CXnNodeAppIf > nodes( aNode.ChildrenL() );
    CleanupClosePushL( nodes );

    // Collect settings
    TInt count( nodes.Count() );
    
    for( TInt i = 0; i < count; i++ )
        {
        CXnNodeAppIf* node( nodes[i] );
        
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
    if ( iRunningAsMain && iAppUi )
        {
        iAppUi->ActivateUi();
        }
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
// CXmlUiController::SetStateHandler()
// ----------------------------------------------------------------------------
//
void CXmlUiController::SetStateHandler( MAiFwStateHandler& aFwStateHandler )
    {
    iFwStateHandler = &aFwStateHandler;
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
// CXmlUiController::FwStateHandler()
// ----------------------------------------------------------------------------
//
MAiFwStateHandler* CXmlUiController::FwStateHandler() const
    {
    return iFwStateHandler;
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
    __PRINTS( "*** CXmlUiController::RunApplicationL" );
    
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
// CXmlUiController::IsMenuOpen()
// ----------------------------------------------------------------------------
//
TBool CXmlUiController::IsMenuOpen()
    {
    return iAppUi->IsDisplayingMenuOrDialog();
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
    __TICK( "CXmlUiController::NewApplication" );
    __TIME_MARK( time );
    
    CXmlUiController* self = static_cast< CXmlUiController* >( Dll::Tls() );
    
    __ASSERT_ALWAYS( self, Panic( ECriticalPointerNull ) );
    
    CApplication* app = CApplication::New( *self );    
    
    __TIME_ENDMARK( "CXmlUiController::NewApplication, done", time );
    
    return app;
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

