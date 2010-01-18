/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used for handling system events (events with EventHandler-prefix).
*
*/

#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwcommon.h>
#include <UikonInternalPSKeys.h>
#include <e32property.h>

#include "aieventhandler.h"
#include "aixmluiconstants.h"
#include "xmluicontroller.h"
#include "appui.h"
#include "xndomnode.h"
#include "xnuiengineappif.h"
#include "xnnodeappif.h"
#include "aistrcnv.h"

#include <csxhelp/hmsc.hlp.hrh>
#include <hlplch.h>

using namespace AiXmlUiController;

// Action Handler LIW commands
_LIT8( KAHInterface, "IDataAction");
_LIT8( KAHService, "Service.ActionHandler");

_LIT( KLaunchApplication, "launch_application" );
_LIT( KViewActivation, "view_activation" );
_LIT( KCmdLine, "cmd_line");

_LIT8( KType, "type" );
_LIT8( KLaunchMethod, "launch_method" );
_LIT8( KApplicationUid, "application_uid" );
_LIT8( KViewId, "view_id" );
_LIT8( KViewAppUid, "view_app_uid" );
_LIT8( KMessageUid, "message_uid" );
_LIT8( KAdditionalData, "additional_data" );
_LIT8( KPluginId, "plugin_id" );
_LIT8( KData, "data");
_LIT8( KExecute, "Execute" );

const TInt32 KCASpaAppLauncherPlugin = {0x10282E5F};
const TUid KVoiceCallUidViewId = { 0x10282D81 };
const TUid KVideoCallUid = { 0x101F8681 };

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// Shows the Homescreen specific help item
// ----------------------------------------------------------------------------
//
static void ShowHelpL( const TDesC& aHelpString )
    {
    TUid fwUid = TUid::Uid( AI_UID3_AIFW_COMMON );
    TCoeContextName helpString;
    helpString.Copy( aHelpString );
    
    CArrayFixFlat<TCoeHelpContext>* array = 
    new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL( array );
    
    array->AppendL( TCoeHelpContext( fwUid, helpString ) );
    
    HlpLauncher::LaunchHelpApplicationL( 
    CCoeEnv::Static()->WsSession(), array );
    
    CleanupStack::Pop( array );
    }

// ---------------------------------------------------------------------------
// Activates Phone application either in video call or normal view
// ---------------------------------------------------------------------------
//
static void ActivatePhoneViewL( CXmlUiController& aUiController )
    {
    TInt topMostAppId = 0;
    RProperty::Get( KPSUidUikon, KUikVideoCallTopApp, topMostAppId );
    TUid appId(TUid::Uid(topMostAppId));
    if( appId == KVideoCallUid)
        {
        const TVwsViewId viewId(appId, appId);
        aUiController.AppUi()->ActivateViewL( viewId, KNullUid, KNullDesC8());  
        }
    else
        {
        const TVwsViewId viewId(appId, KVoiceCallUidViewId);
        aUiController.AppUi()->ActivateViewL( viewId, KVoiceCallUidViewId, KNullDesC8());
        }
    }
// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::CAIXuikonEventHandler()
// ----------------------------------------------------------------------------
//
CAIEventHandler::CAIEventHandler( CXmlUiController& aUiController )
    : iUiController( aUiController ) 
    {
    }

// ----------------------------------------------------------------------------
// CAISystemEventHandler::ConstructL()
// ----------------------------------------------------------------------------
//
void CAIEventHandler::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAISystemEventHandler::NewL()
// ----------------------------------------------------------------------------
//
CAIEventHandler* CAIEventHandler::NewL( CXmlUiController& aUiController )
    {
    CAIEventHandler* self =
        new ( ELeave ) CAIEventHandler( aUiController );
    CleanupStack::PushL( self );    
    self->ConstructL();    
    CleanupStack::Pop( self );    
    return self;
    }

// ----------------------------------------------------------------------------
// CAISystemEventHandler::~CAISystemEventHandler()
// ----------------------------------------------------------------------------
//
CAIEventHandler::~CAIEventHandler()
    {
    if( iServiceHandler )
        {
        iServiceHandler->Reset();
        }
    delete iServiceHandler;
    }

// ----------------------------------------------------------------------------
// Handles the restricted ( can be triggered only from view) events and forwards 
// the rest into appropriate functions.
// ----------------------------------------------------------------------------
//
void CAIEventHandler::HandleEventL( const TDesC8& aEventText, CXnDomNode& aEvent )
    {
    const TDesC8& prefix( AiUiDef::xml::event::KEventHandlerPrefix );
    const TDesC8& event( aEventText.Mid( prefix.Length() ) );
    const TDesC8& ns( iUiController.UiEngineL()->ActiveView()->Namespace() );
    // These events can be excuted from view only
    if( aEvent.Namespace() == ns)
        {
        if ( event == AiUiDef::xml::event::KEventSetOnline )
            {
            iUiController.AppUi()->SetOnlineStateL( ETrue );
            }
        else if ( event == AiUiDef::xml::event::KEventSetOffline )
            {
            iUiController.AppUi()->SetOnlineStateL( EFalse );
            }    
        else if ( event == AiUiDef::xml::event::KEventShowHelp )
            {
            if( iUiController.UiEngineL()->IsEditMode() )
                {
                ShowHelpL( KSET_HLP_HOME_SCREEN_EDIT );
                }
            else
                {
                ShowHelpL( KSET_HLP_HOME_SCREEN );
                }
            }
        else if ( event == AiUiDef::xml::event::KEventActivatePhoneView )
            {
            ActivatePhoneViewL( iUiController );
            }
        }

    if( event == AiUiDef::xml::event::KEventLaunchApp )
        {
        LaunchAppL( aEvent );
        }
    else if( event == AiUiDef::xml::event::KEventActivateView )
        {
        ActivateViewL( aEvent );
        }
    }

// ----------------------------------------------------------------------------
// Launches app using Action Handler
// ----------------------------------------------------------------------------
//
void CAIEventHandler::LaunchAppL( CXnDomNode& aEventNode )
    { 
    const TDesC8& uidStr( aEventNode.AttributeValue( AiUiDef::xml::property::KUid ));
    TInt32 uid( 0 );
    User::LeaveIfError( AiUtility::ParseInt( uid, uidStr ) );
    
    CLiwDefaultMap* map = CLiwDefaultMap::NewL(); 
    map->PushL();
    map->InsertL( KType, TLiwVariant( KLaunchApplication ));
    map->InsertL( KLaunchMethod, TLiwVariant( KCmdLine ));
    map->InsertL( KApplicationUid, TLiwVariant( uid ));

    ExecuteAppLauncherL( *map );

    CleanupStack::PopAndDestroy( map );
    }

// ----------------------------------------------------------------------------
// Activates view using Action Handler
// ----------------------------------------------------------------------------
//
void CAIEventHandler::ActivateViewL( CXnDomNode& aEventNode )
    {
    const TDesC8& uidStr( aEventNode.AttributeValue( AiUiDef::xml::property::KUid ));
    const TDesC8& viewIdStr( aEventNode.AttributeValue( AiUiDef::xml::property::KViewId ));
    const TDesC8& messageIdStr( aEventNode.AttributeValue( AiUiDef::xml::property::KMessageId ));
    const TDesC8& messageStr( aEventNode.AttributeValue( AiUiDef::xml::property::KMessage ));

    CLiwDefaultMap* map = CLiwDefaultMap :: NewL(); 
    map->PushL();
    map->InsertL( KType, TLiwVariant( KViewActivation ));
    TInt32 uid( 0 );    
    User::LeaveIfError( AiUtility::ParseInt( uid, uidStr ) );
    map->InsertL( KViewAppUid, TLiwVariant( uid ));
    
    TInt32 viewId( 0 );
    User::LeaveIfError( AiUtility::ParseInt( viewId, viewIdStr ) );
    map->InsertL( KViewId, TLiwVariant( viewId ));
    
    if( messageIdStr.Length() != 0 )
        {
        TInt32 messageId( 0 );
        User::LeaveIfError( AiUtility::ParseInt( messageId, messageIdStr ) );
        map->InsertL( KMessageUid, TLiwVariant( messageId ));
        
        map->InsertL( KAdditionalData, TLiwVariant( messageStr ));
        }

    ExecuteAppLauncherL( *map );

    CleanupStack::PopAndDestroy( map );
    }

// ----------------------------------------------------------------------------
// Calls the actual LIW interface
// ----------------------------------------------------------------------------
//
void CAIEventHandler::ExecuteAppLauncherL( CLiwDefaultMap& aMap )
    {
    MLiwInterface* msgInterface = ActionHandlerInterfaceL( );
    if( msgInterface && iServiceHandler )
        {
        CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
        CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();
        
        TLiwGenericParam pluginUid( KPluginId, TLiwVariant( TUid::Uid( KCASpaAppLauncherPlugin )));
        inparam.AppendL( pluginUid );

        TLiwGenericParam data( KData, TLiwVariant( &aMap ));
        inparam.AppendL( data );

        msgInterface ->ExecuteCmdL(KExecute, inparam, outparam);
        msgInterface->Close();

        outparam.Reset();
        inparam.Reset();
        }
    }

// ----------------------------------------------------------------------------
// Gets the Action Handler interface
// ----------------------------------------------------------------------------
//
MLiwInterface* CAIEventHandler::ActionHandlerInterfaceL( )
    {
    if( !iServiceHandler )
        {
        iServiceHandler = CLiwServiceHandler::NewL();
        }
    CLiwGenericParamList& inParam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParam = iServiceHandler->OutParamListL();

    CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( KLiwCmdAsStr, KAHInterface, KAHService);

    CleanupStack::PushL( crit );
    crit->SetServiceClass( TUid::Uid( KLiwClassBase ));
    
    RCriteriaArray a;
    a.AppendL(crit);    
    
    iServiceHandler->AttachL(a);
    iServiceHandler->ExecuteServiceCmdL( *crit, inParam, outParam ); 
    
    CleanupStack::PopAndDestroy( crit );
    a.Reset();
    
    // find service interface
    TInt pos = 0;
    MLiwInterface* msgInterface = NULL;
    outParam.FindFirst( pos, KAHInterface );
    if ( pos != KErrNotFound ) 
        {
        msgInterface = (outParam)[pos].Value().AsInterface(); 
        }
    
    outParam.Reset();
    inParam.Reset();

    return msgInterface;
    }

// End of file
