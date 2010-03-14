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
* Description:  Used for handling XUIKON events
*
*/

// System includes
#include <e32std.h>
#include <csxhelp/hmsc.hlp.hrh>
#include <layoutmetadata.cdl.h>
#include <e32property.h>
#include <centralrepository.h>
#include <AknUtils.h>
#include <akntoolbar.h>
#include <aknappui.h>
#include <hlplch.h>
#include <aknlistquerydialog.h> 
#include <aknnotewrappers.h> 
#include <StringLoader.h>

// User includes
#include <hspublisherinfo.h>
#include <aiutility.h>
#include <aistrparser.h>
#include <ai3xmlui.rsg>
#include <activeidle2domainpskeys.h>
#include <activeidle2domaincrkeys.h>
#include <aisystemuids.hrh>

#include "aixuikoneventhandler.h"
#include "aifweventhandler.h"
#include "aiuieventhandler.h"
#include "aiapplicationeventhandler.h"
#include "aiconsts.h"
#include "xmluicontroller.h"
#include "aixmluiconstants.h"
#include "aixmluiutils.h"
#include "contentrenderer.h"
#include "xnnodeappif.h"
#include "xnuiengineappif.h"
#include "xnproperty.h"
#include "xndomnode.h"
#include "xntype.h"
#include "xndomlist.h"      // for cxndomlist
#include "xndomattribute.h" // for cxndomattribute
#include "xndompropertyvalue.h"
#include "xnnodeappif.h"
#include "appui.h"
#include "xndomproperty.h"
#include "xnplugindefs.h"
#include "aieventhandler.h"

const TInt KAiMaxNumberCharacters = 5;

namespace AiXmlUiController
    {
    class CAIXuikonEventHandler::CNullEventHandler :
            public CBase,
            public MAiFwEventHandler,
            public MAiUiEventHandler,
            public MAiApplicationEventHandler
        {
        
    private: // from MAiFwEventHandler        
        void AppEnvReadyL();
        void HandleUiReadyEventL( CAiUiController& aUiController );
        void HandleActivateUI();
        void HandleUiShutdown( CAiUiController& aUiController );       
        void HandlePluginEvent( const TDesC& aParam );
        void HandlePluginEventL( const THsPublisherInfo& aPublisherInfo, const TDesC& aParam );
        TBool HasMenuItemL( const THsPublisherInfo& aPublisherInfo, const TDesC& aMenuItem );
        TBool RefreshContent( const TDesC& aContentCid );
        TBool RefreshContent( const THsPublisherInfo& aPublisherInfo, const TDesC& aContentId ); 
        TBool SuspendContent( const THsPublisherInfo& aPublisherInfo, const TDesC& aContentId );                                     
        TBool QueryIsMenuOpen();        
        
    private: // from MAiUiEventHandler                                 
        TBool HandleUiEvent( TAny* aEvent, const TDesC8& aParam );

    private: // from MAiApplicationEventHandler                        
        TBool HandleApplicationEvent( TInt aEvent, const TDesC8& aParam );
        };
    }

using namespace AiXmlUiController;
using namespace AiUiDef::xml::event;

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::CAIXuikonEventHandler()
// ----------------------------------------------------------------------------
//
CAIXuikonEventHandler::CAIXuikonEventHandler( CXmlUiController& aUiController,
                                             CContentRenderer& aRenderer )
    : iUiController(aUiController),
      iContentRenderer(aRenderer)
    {
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::ConstructL()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::ConstructL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    iNullEventHandler = new ( ELeave ) CNullEventHandler;
    iFwEventHandler = iNullEventHandler;
    iUiEventHandler = iNullEventHandler;
    iApplicationEventHandler = iNullEventHandler;
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::NewL()
// ----------------------------------------------------------------------------
//
CAIXuikonEventHandler* CAIXuikonEventHandler::NewL(
        CXmlUiController& aUiController,
        CContentRenderer& aRenderer )
    {
    CAIXuikonEventHandler* self =
        new ( ELeave ) CAIXuikonEventHandler( aUiController, aRenderer );
    CleanupStack::PushL( self );    
    self->ConstructL();    
    CleanupStack::Pop( self );    
    return self;
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::~CAIXuikonEventHandler()
// ----------------------------------------------------------------------------
//
CAIXuikonEventHandler::~CAIXuikonEventHandler()
    {
    delete iNullEventHandler;
    delete iVolumeEvent;
    
    Release( iStrParser );
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::SetFwEventHandler()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::SetFwEventHandler( 
    MAiFwEventHandler* aFwEventHandler )
    {
    if ( aFwEventHandler )
        {
        iFwEventHandler = aFwEventHandler;
        }
    else
        {
        iFwEventHandler = iNullEventHandler;
        }
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::SetUiEventHandler()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::SetUiEventHandler( 
    MAiUiEventHandler* aUiEventHandler )
    {
    if ( aUiEventHandler )
        {
        iUiEventHandler = aUiEventHandler;
        }
    else
        {
        iUiEventHandler = iNullEventHandler;
        }
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::SetApplicationEventHandler()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::SetApplicationEventHandler(
    MAiApplicationEventHandler* aApplicationEventHandler )
    {
    if ( aApplicationEventHandler )
        {
        iApplicationEventHandler = aApplicationEventHandler;
        }
    else
        {
        iApplicationEventHandler = iNullEventHandler;
        }
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::HandleXuikonEventL()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::HandleXuikonEventL( CXnNodeAppIf& aOrigin,
    CXnNodeAppIf& /*aTrigger*/, CXnDomNode& aTriggerDefinition, 
    CXnDomNode& aEvent )                                                                                              
    {
    DispatchEventL( aEvent, aOrigin, aTriggerDefinition );
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::HandleXuikonActionL()
// ----------------------------------------------------------------------------
//
inline TBool CAIXuikonEventHandler::HandleSystemEventL(  
    const TDesC8& aEventText, CXnDomNode& aEvent )
    {
    const TDesC8& prefix( AiUiDef::xml::event::KEventHandlerPrefix );   
    TInt pos( aEventText.Find( prefix ) );
    if ( pos != 0 )
        {
        // Event is not targeted here
        return EFalse;
        }
    CAIEventHandler* handler = CAIEventHandler::NewL( iUiController );
    CleanupStack::PushL( handler );
    handler->HandleEventL( aEventText, aEvent );
    CleanupStack::PopAndDestroy( handler );
    return ETrue;
    }


// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::ParseParameterStringL()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::ParseParameterStringL( const TDesC8& aSourcePtr,
    TPtrC8& aTargetPtr )
    {
    // Parses parameter string between the first '(' and last ')'
    // Trailing and heading white spaces ARE NOT handled!
    
    const TInt separatorPos = aSourcePtr.Locate( KEventParameterSeparator );
    User::LeaveIfError( separatorPos );

    const TInt eventSeparatorPos = aSourcePtr.LocateReverse(
                                                KEventParameterSeparatorEnd );
    User::LeaveIfError( eventSeparatorPos );

    // separatorPos + 1 must be smaller than eventSeparatorPos - 1
    User::LeaveIfError( ( eventSeparatorPos ) - ( separatorPos + 1 ) );
    
    // Extract event and parameter string
    TPtrC8 menuName( aSourcePtr.Mid( separatorPos + 1,
                    ( eventSeparatorPos ) - ( separatorPos + 1 ) ) );
    
    aTargetPtr.Set( menuName );
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::HandleUiEventL()
// ----------------------------------------------------------------------------
//
inline TBool CAIXuikonEventHandler::HandleUiEventL( const TDesC8& aEventText, 
    CXnNodeAppIf& aOrigin )
    {    
    if ( aEventText.Left( KUiEventPrefix().Length() ) == KUiEventPrefix )
        {
        iUiEventHandler->HandleUiEvent( (TAny*)&aOrigin, aEventText );
        
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::HandleApplicationEventL()
// ----------------------------------------------------------------------------
//
inline TBool CAIXuikonEventHandler::HandleApplicationEventL(
    const TDesC8& aEventText )
    {
    return iApplicationEventHandler->HandleApplicationEvent( 0, aEventText );
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::VolumeChanged()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::VolumeChanged( TInt aValue )
    {
    TRAP_IGNORE(
        // Ignore errors, missing one event is not fatal enough to panic
        HBufC* volumeString( HBufC::NewLC( KAiMaxNumberCharacters ) );
                
        volumeString->Des().Num( aValue );

        HBufC* eventString( iVolumeEvent->AllocLC() );
        
        TInt foundAt( eventString->Des().Find( KTargetSliderVolumeValue ) );
        
        if( foundAt >= 0 )
            {
            eventString->Des().Replace( foundAt, 
                    KTargetSliderVolumeValue().Length(), *volumeString );
            }
        
        // TODO: implement namespace properly
        iFwEventHandler->HandlePluginEvent( *eventString );
        
        CleanupStack::PopAndDestroy( 2, volumeString );
        ); // TRAP_IGNORE
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::HandlePluginEventL()
// ----------------------------------------------------------------------------
//
inline
void CAIXuikonEventHandler::HandlePluginEventL( CXnDomNode& aEvent )                                          
    {       
    const TDesC8& event8( 
        aEvent.AttributeValue( XnPropertyNames::action::event::KName ) );    
    
    HBufC16* event( NULL );
    
    event = iStrParser->CopyToBufferL( event, event8 );
    
    if ( !event || *event == KNullDesC )
        {
        // No event defined
        return;
        }
    
    CleanupStack::PushL( event );

    TXnUiEngineAppIf* engine( iUiController.UiEngineL() );
    
    const TDesC8& broadcast( 
        aEvent.AttributeValue( XnPropertyNames::action::event::KBroadcast ) );
            
    if ( broadcast == XnPropertyNames::KTrue )
        {
        // Broadcast events are allowed only from view's namespace
        if ( engine->ActiveView()->Namespace() == aEvent.Namespace() )
            {
            _LIT8( KGlobal, "global" );
            
            // Get all plugins
            RPointerArray< CXnNodeAppIf > 
                list( engine->FindContentSourceNodesL( KGlobal ) );
            
            CleanupClosePushL( list );
        
            // Broadcast event to all plugins
            for ( TInt i = 0; i < list.Count(); i++ )
                {
                THsPublisherInfo info;
                
                iUiController.PublisherInfoL( *list[i], info );
        
                // Forward event to AiFw
                iFwEventHandler->HandlePluginEventL( info, *event );                
                }
            
            CleanupStack::PopAndDestroy( &list );                
            }                
        }
    else
        {
        const TDesC8& ns( aEvent.Namespace() );
    
        // Get plugins from the event's namespace
        RPointerArray< CXnNodeAppIf > 
            list( engine->FindContentSourceNodesL( ns ) );
        
        CleanupClosePushL( list );

        if ( list.Count() == 1 )
            {
            // Only one data plugin in this namespace,
            // it must handle this event.
            THsPublisherInfo info;
            
            iUiController.PublisherInfoL( *list[0], info );
    
            // Forward event to AiFw
            iFwEventHandler->HandlePluginEventL( info, *event );
            }
        else if ( list.Count() > 1 )
            {
            const TDesC8& destination( 
                aEvent.AttributeValue( 
                        XnPropertyNames::action::event::KDestination ) );
            
            // Multiple data plugins in this namespace,
            // resolve destination
            CXnNodeAppIf* eventHandler( 
                engine->FindNodeByIdL( destination, ns ) );
            
            if ( eventHandler )
                {            
                THsPublisherInfo info;
                
                iUiController.PublisherInfoL( *eventHandler, info );
                
                // Forward event to AiFw
                iFwEventHandler->HandlePluginEventL( info, *event );            
                }
            else
                {
                // No exact destination defined,  
                // try to find a correct plugin based on event
                const TInt pos( event->Locate( KPluginEventSeparator ) );
                
                if ( pos != KErrNotFound )
                    {
                    // Extract plugin name
                    TPtrC pluginName( event->Left( pos ) );
                    
                    for ( TInt i = 0; i < list.Count(); i++ )
                        {
                        THsPublisherInfo info;
                        
                        iUiController.PublisherInfoL( *list[i], info );
                        
                        if( info.Name() == pluginName )
                            {
                            // Forward Plug-in events to AI FW for further 
                            // dispatching to the target Plug-in
                            iFwEventHandler->HandlePluginEventL( info,*event );                                                             
                            break;
                            }
                        }
                    }                
                }
            }                        
            
        CleanupStack::PopAndDestroy( &list );    
        }        
    
    CleanupStack::PopAndDestroy( event ); 
    }

// ----------------------------------------------------------------------------
// CAIXuikonEventHandler::DispatchEventL()
// ----------------------------------------------------------------------------
//
void CAIXuikonEventHandler::DispatchEventL( CXnDomNode& aEvent,
    CXnNodeAppIf& aOrigin, CXnDomNode& /*aTriggerDefinition*/ )                                            
    {
    const TDesC8& aEventString8( 
        aEvent.AttributeValue( XnPropertyNames::action::event::KName ) );

    // First check if the event is a XUIKON-specific event
    if( !HandleSystemEventL( aEventString8, aEvent ) )
        {
        // Next check if it is a UI event
        if( !HandleUiEventL( aEventString8, aOrigin ) )
            {
            // Next check if it is an application event
            if( !HandleApplicationEventL( aEventString8 ) )
                {                
                // Assume that it is a Plug-in event
                HandlePluginEventL( aEvent );
                }
            }
        }
    }

// CAIXuikonEventHandler::CNullEventHandler
void CAIXuikonEventHandler::CNullEventHandler::AppEnvReadyL()
    {
    }

void CAIXuikonEventHandler::CNullEventHandler::HandleUiReadyEventL(
    CAiUiController& /*aUiController*/ )
    {
    }

void CAIXuikonEventHandler::CNullEventHandler::HandleActivateUI()
    {   
    }

void CAIXuikonEventHandler::CNullEventHandler::HandlePluginEvent(
    const TDesC& /*aParam*/ )
    {
    }

void CAIXuikonEventHandler::CNullEventHandler::HandlePluginEventL(
    const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aParam*/ )
    {
    }

TBool CAIXuikonEventHandler::CNullEventHandler::HasMenuItemL( 
    const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aMenuItem*/ )    
    {
    return EFalse;
    }

void CAIXuikonEventHandler::CNullEventHandler::HandleUiShutdown(
    CAiUiController& /*aUiController*/ )
    {
    }

TBool CAIXuikonEventHandler::CNullEventHandler::RefreshContent( 
    const TDesC& /*aContentCid*/ )
    {
    return EFalse;
    }

TBool CAIXuikonEventHandler::CNullEventHandler::RefreshContent( 
    const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aContentId*/ )    
    {
    return EFalse;
    }

TBool CAIXuikonEventHandler::CNullEventHandler::SuspendContent( 
    const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aContentId*/ )
    {
    return EFalse;
    }

TBool CAIXuikonEventHandler::CNullEventHandler::QueryIsMenuOpen()
    {
    return EFalse;
    }

TBool CAIXuikonEventHandler::CNullEventHandler::HandleUiEvent(
    TAny* /*aEvent*/, const TDesC8& /*aParam*/ )
    {
    return EFalse;
    }

TBool CAIXuikonEventHandler::CNullEventHandler::HandleApplicationEvent(
    TInt /*aEvent*/, const TDesC8& /*aParam*/ )
    {
    return EFalse;
    }

// End of file
