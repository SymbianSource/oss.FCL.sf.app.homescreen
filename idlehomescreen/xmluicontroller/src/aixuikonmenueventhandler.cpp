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
* Description:  Used for handling XUIKON menu events
*
*/


#include "aixuikonmenueventhandler.h"
#include "aiconsts.h"
#include "aixmluiconstants.h"
#include "aixmluiutils.h"

#include "mxncomponentinterface.h"
#include "xnmenuadapter.h"
#include "xnappuiadapter.h"
#include "xntext.h"
#include "xnnodeappif.h"
#include "xndomnode.h"
#include "xndomlist.h"
#include "xntype.h"
#include <gulicon.h>
#include <aiutility.h>
#include <aistrparser.h>
#include "aifweventhandler.h"
#include "xnproperty.h"
#include "xnuiengineappif.h"
#include "xndompropertyvalue.h"
#include <aipspropertyobserver.h>
#include <avkondomainpskeys.h> 
#include <aknviewappui.h>		//CaknViewAppui
#include "xnplugindefs.h"

_LIT8( KMenubar, "menubar" );
_LIT8( KClass, "class" );
_LIT8( KId, "id" );
_LIT8( KEditWidget, "edit_widget" );
_LIT8( KRemoveItem, "removeitem" );
_LIT8( KWidget, "widget" );
_LIT8( KRemovable, "removable" );
_LIT ( KMenuItem, "menuitem");

using namespace AiXmlUiController;


// ---------------------------------------------------------------------------
// FindFromContentModelL
// ---------------------------------------------------------------------------
// 
static CXnNodeAppIf* FindFromContentModelL( CXnNodeAppIf* aNode )
    {
    if ( !aNode )
        {
        return NULL;
        }

    CXnProperty* node = aNode->GetPropertyL( KClass ); 
    if ( node && node->StringValue() == KContentSource )
        {
        return aNode;
        }

    // Recurse children        
    RPointerArray< CXnNodeAppIf > children( aNode->ChildrenL() );
    CleanupClosePushL( children );

    CXnNodeAppIf* retval( NULL );

    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNodeAppIf* node( FindFromContentModelL( children[i] ) );

        if ( node )
            {
            retval = node;
            break;
            }
        }

    CleanupStack::PopAndDestroy( &children );

    return retval;
    }

// ---------------------------------------------------------------------------
// FindFromConfigurationL
// ---------------------------------------------------------------------------
// 
static CXnNodeAppIf* FindFromConfigurationL( CXnNodeAppIf* aNode )
    {
    if ( !aNode )
        {
        return NULL;
        }
    // Is this node type of settingsconfiguration
    if ( aNode->Type()->Type() == KSettingsConfiguration )
        {
        return aNode;
        }

    // Recurse children        
    RPointerArray< CXnNodeAppIf > children( aNode->ChildrenL() );
    CleanupClosePushL( children );

    CXnNodeAppIf* retval( NULL );

    for ( TInt i = 0; i < children.Count(); i++ )
        {               
        CXnNodeAppIf* node( FindFromConfigurationL( children[i] ) );
        
        if ( node )
            {
            retval = node;
            break;
            }
        }

    CleanupStack::PopAndDestroy( &children );

    return retval;
    }

// ---------------------------------------------------------------------------
// DeletePluginNames
// ---------------------------------------------------------------------------
// 
static void DeletePluginNames( TAny* aObject )
    {
    reinterpret_cast<
        RPointerArray<HBufC16>*>( aObject )->ResetAndDestroy();
    }

CAIXuikonMenuEventHandler::CAIXuikonMenuEventHandler( TXnUiEngineAppIf& aUiEngine,
                                                      MAiFwEventHandler* aFwEventHandler,
                                                      CXnAppUiAdapter* aAppUi )
  : iCurrentCBA( EAIMenuOff ),
    iUiEngine( &aUiEngine ),
    iFwEventHandler( aFwEventHandler ),
    iAppUi( aAppUi )
    {
    }

void CAIXuikonMenuEventHandler::ConstructL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    iKeylockObserver = AiUtility::CreatePSPropertyObserverL
        ( TCallBack( HandleKeylockEvent, this ),
            KPSUidAvkonDomain, KAknKeyguardStatus );  
    }

void CAIXuikonMenuEventHandler::SetObserverL( const TDesC8& aParam, CXnDomNode* aEvent )
    {
    if ( aEvent )
        {
        CXnNodeAppIf* node( iUiEngine->FindNodeByIdL( aParam, aEvent->Namespace() ) );
        
        if ( node && node->Type()->Type() == KMenubar )
            {
            XnMenuInterface::MXnMenuInterface* menuIf( NULL );
            XnComponentInterface::MakeInterfaceL( menuIf, *node );
    
            if ( menuIf )
                {
                menuIf->SetObserver( *this );
                }
            }
        }
    }

TInt CAIXuikonMenuEventHandler::HandleKeylockEvent( TAny* aSelf )
    {
    // Stop displaying menu when keylock is activated.
    CAIXuikonMenuEventHandler* self = 
        static_cast<CAIXuikonMenuEventHandler*>( aSelf );
    TInt err = KErrNone;
    TInt value = EKeyguardNotActive;
    if ( self )
        {
        TInt err = self->iKeylockObserver->Get( value );
        if ( err == KErrNone )
            {
			if ( value > EKeyguardNotActive )
				{
		        if ( self->iAppUi && self->iCurrentCBA == EAIMenuOn )
		        	{
		        	self->iAppUi->StopDisplayingMenuBar();
		        	TRAP_IGNORE( self->iUiEngine->RefreshMenuL() );
		        	}			
				}        				
            }
        }
    return err;
    }

CAIXuikonMenuEventHandler* CAIXuikonMenuEventHandler::NewL( TXnUiEngineAppIf& aUiEngine,
                                                            MAiFwEventHandler* aFwEventHandler,
                                                            CXnAppUiAdapter* aAppUi )
    {
    CAIXuikonMenuEventHandler* self = 
        new( ELeave ) CAIXuikonMenuEventHandler( aUiEngine, aFwEventHandler, aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAIXuikonMenuEventHandler::~CAIXuikonMenuEventHandler()
    {
    Release ( iKeylockObserver );
    Release( iStrParser );
    delete iActivationEvent;
    delete iSettingsEvent;
                
    // Delete stored label id's
    delete iMenuModeLskLabel;
    delete iMenuModeRskLabel;
    delete iNormalModeLskLabel;
    delete iNormalModeRskLabel;  
    }

TBool CAIXuikonMenuEventHandler::HandleEventL( const TDesC8& aEventString8, CXnNodeAppIf* aOrigin, CXnDomNode* aEvent )
    {
    if ( aEventString8 == AiUiDef::xml::event::KScreenDeviceChange )
        {
        iUiEngine = iAppUi->UiEngineL();
        return ETrue;
        }
    else if ( aEventString8.Left(KAIDynamicMenuPrefixLen).Compare( KAIDynamicMenuPrefix ) != 0 )
        {
        // For optimization when dynamic menu prefix is not found from event string start
        // return immediate from here.
        return EFalse;
        }

    if ( (iCurrentCBA == EAIMenuOff && aEventString8.Find(KAIMenuEventIfMenuOff) >= 0 ) ||
        (iCurrentCBA == EAIMenuOn && aEventString8.Find(KAIMenuEventIfMenuOn) >= 0) )
        {
        TPtrC8 eventString;
        ParseParameterStringL(aEventString8, eventString);
        if ( !HandleEventL(eventString, aOrigin, aEvent ) )
            {
            // Default action from the event string
            HBufC16* eventString16 = NULL;
            eventString16 = iStrParser->CopyToBufferL( eventString16, eventString );
            CleanupStack::PushL( eventString16 );
            if ( iFwEventHandler && eventString16 )
                {
                iFwEventHandler->HandlePluginEvent( *eventString16 );
                }
            CleanupStack::PopAndDestroy( eventString16 );
            }
        return ETrue;
        }
    else if ( aEventString8 == KAIMenuEventInitPluginItems )
        {
        if ( !aEvent )
            {
            return ETrue;
            }

        CXnDomList& children( aEvent->ChildNodes() );

        TInt count( children.Length() );

        for ( TInt i = 0; i < count; i++ )
            {
            CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );

            const TDesC8& type( node->Name() );

            if ( type == XnPropertyNames::action::KProperty )
                {
                const TDesC8& id( node->AttributeValue( 
                                    XnPropertyNames::action::KValue ) );

                if ( id != KNullDesC8 )
                    {
                    CXnNodeAppIf* nodeById( iUiEngine->FindNodeByIdL( id, aEvent->Namespace() ) );
                    
                    if ( nodeById )
                        {                            
                        TBool showItem( DynInitItemL( *nodeById, aOrigin ) ); 
                                                    
                        const TDesC8 *value( NULL );
                        
                        if ( showItem )
                            {
                            value = &XnPropertyNames::style::common::display::KBlock;
                            }
                        else
                            {
                            value = &XnPropertyNames::style::common::display::KNone;
                            }            

                        CXnDomStringPool& sp( aEvent->StringPool() );

                        // create property
                        CXnProperty* prop = CXnProperty::NewL( XnPropertyNames::style::common::KDisplay, 
                                                               *value, 
                                                               CXnDomPropertyValue::EString, 
                                                               sp );

                        CleanupStack::PushL( prop );

                        // and set it
                        nodeById->SetPropertyL( prop );

                        CleanupStack::Pop( prop );
                        }
                    }
                }
            }

        return ETrue;
        }
    else if ( aEventString8 == KAIMenuEventInitEmptyItems  ||
              aEventString8 == KAIMenuEventInitWidgetItems )
        {
        if ( !aEvent )
            {
            return ETrue;
            }

        CXnProperty* pluginId( aOrigin->GetPropertyL( KPluginId ) );
        CXnProperty* pluginName( aOrigin->GetPropertyL( KPluginName ) );

        TBool showItem( EFalse );

        if ( aEventString8 == KAIMenuEventInitWidgetItems )
            {
            if( pluginId && pluginName && pluginName->StringValue() != KDummy8 )                        
                {
                // check if plugin can be removed
                CXnProperty* removable = GetWidgetPropertyL(*aOrigin, KRemovable);
                if ( removable && removable->StringValue() == XnPropertyNames::KFalse )
                    {
                    showItem = EFalse;
                    }
                else
                    {
                    showItem = ETrue;
                    }
                }                                
            }
        else
            {
            if ( pluginId && pluginName && pluginName->StringValue() == KDummy8 )
                {
                showItem = ETrue;
                }
            }

        CXnDomList& children( aEvent->ChildNodes() );

        TInt count( children.Length() );

        for ( TInt i = 0; i < count; i++ )
            {
            CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );

            const TDesC8& type( node->Name() );

            if ( type == XnPropertyNames::action::KProperty )
                {
                const TDesC8& id( node->AttributeValue( 
                                    XnPropertyNames::action::KValue ) );

                if ( id != KNullDesC8 )
                    {
                    CXnNodeAppIf* nodeById( iUiEngine->FindNodeByIdL( id, aEvent->Namespace() ) );

                    if ( nodeById )
                        {
                        const TDesC8 *value( NULL );

                        if ( showItem )
                            {
                            value = &XnPropertyNames::style::common::display::KBlock;
                            }
                        else
                            {
                            value = &XnPropertyNames::style::common::display::KNone;
                            }

                        CXnDomStringPool& sp( aEvent->StringPool() );

                        // create property
                        CXnProperty* prop = CXnProperty::NewL( XnPropertyNames::style::common::KDisplay, 
                                                               *value, 
                                                               CXnDomPropertyValue::EString, 
                                                               sp );

                        CleanupStack::PushL( prop );

                        // and set it
                        nodeById->SetPropertyL( prop );

                        CleanupStack::Pop( prop );
                        }
                    }
                }
            }

        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventIfMenuOff ) >= 0 )
        {
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventIfMenuOn ) >= 0 )
        {
        return ETrue;
        }
    else if ( aEventString8.Compare( KAIMenuEventGenerateSetActivationEvent ) == 0 )
        {
        // Generate set plugin event
        if ( iFwEventHandler && iActivationEvent )
            {
            iFwEventHandler->HandlePluginEvent( *iActivationEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventSetActivationEvent ) >= 0 )
        {
        // find event string
        TPtrC8 eventString;
        ParseParameterStringL( aEventString8, eventString );
        delete iActivationEvent;
        iActivationEvent = NULL;
        // store activation event (plugin event string)
        iActivationEvent = iStrParser->CopyToBufferL( iActivationEvent, eventString );
        return ETrue;
        }
    else if (aEventString8.Compare( KAIMenuEventGenerateOpenSettingsEvent ) == 0)
        {
        // Generate set plugin event
        if ( iFwEventHandler && iSettingsEvent )
            {
            iFwEventHandler->HandlePluginEvent( *iSettingsEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventSetOpenSettingsnEvent ) >= 0 )
        {
        // find event string
        TPtrC8 eventString;
        ParseParameterStringL( aEventString8, eventString );
        delete iSettingsEvent;
        iSettingsEvent = NULL;
        // store activation event (plugin event string)
        iSettingsEvent = iStrParser->CopyToBufferL( iSettingsEvent, eventString );
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventShowMenuItem ) >= 0 )
        {
        if (iCurrentCBA == EAIMenuOn)
            {
            // find node by id
            TPtrC8 menuItem;
            ParseParameterStringL( aEventString8, menuItem );
            // set visibility property to visible
            ShowMenuItemL( menuItem, ETrue, aEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventHideMenuItem ) >= 0 )
        {
        if ( iCurrentCBA == EAIMenuOn )
            {
            // find node by id
            TPtrC8 menuItem;
            ParseParameterStringL( aEventString8, menuItem );
            // set visibility property to visible
            ShowMenuItemL( menuItem, EFalse, aEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventShowHideMenuItem ) >= 0 )
            {
            if ( iCurrentCBA == EAIMenuOn )
                {
                // find node by id
                TPtrC8 menuItem;
                ParseParameterStringL( aEventString8, menuItem );
                // set visibility property
                if ( IsMenuItemVisibleL( menuItem, aEvent ) )
                    {
                    ShowMenuItemL( menuItem, EFalse, aEvent );
                    }
                else
                    {
                    ShowMenuItemL( menuItem, ETrue, aEvent );
                    }
                }
            return ETrue;
            }
    else if ( aEventString8.Compare( KAIMenuEventMenuOn ) == 0 )
        {
        if ( iCurrentCBA != EAIMenuOn )
            {
            // also updates current state
            SetCBALabelsL( EAIMenuOn, aEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Compare( KAIMenuEventMenuOff ) == 0 )
        {
        if ( iCurrentCBA != EAIMenuOff )
            {
            // also updates current state
            SetCBALabelsL( EAIMenuOff, aEvent );
            }
        return ETrue;
        }
    else if ( ( aEventString8.Find( KAIMenuEventShowAll ) ) >= 0 )
        {
        if ( iCurrentCBA == EAIMenuOn )
            {
            // Show all menu items. Xuikon actually open the menu it self..
            TPtrC8 menu;
            ParseParameterStringL( aEventString8, menu );
            ShowMenuL( ETrue, menu, aEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventRefresh ) >= 0 )
        {
        if ( iCurrentCBA == EAIMenuOn )
            {
            iUiEngine->RefreshMenuL();
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventHideAll ) >= 0 )
        {
        // Hide all menu items
        TPtrC8 menu;
        ParseParameterStringL( aEventString8, menu );
        ShowMenuL( EFalse, menu, aEvent );
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventSetCloseAction ) >= 0 )
        {
        // Set menu mode close action / RSK action
        TPtrC8 closeActionId;
        ParseParameterStringL( aEventString8, closeActionId );

        // Store close action macroset node id
        iCloseActionId.Set( closeActionId );
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventCloseAction ) >= 0 )
        {
        // Decide action
        if ( iCurrentCBA == EAIMenuOff )
            {
            TPtrC8 eventString;
            ParseParameterStringL( aEventString8, eventString );
            
            // Default action
            HBufC16* eventString16 = NULL;
            eventString16 = iStrParser->CopyToBufferL( eventString16, eventString );
            CleanupStack::PushL( eventString16 );
            if ( iFwEventHandler && eventString16 )
                {
                iFwEventHandler->HandlePluginEvent( *eventString16 );
                }
            CleanupStack::PopAndDestroy( eventString16 );
            }
        else
            {
            DoMacrosetL( iCloseActionId, aEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventSetSoftKeyId ) >= 0 )
        {
        // Store softkey labels and label node id's
        TPtrC8 softkeyString;
        ParseParameterStringL( aEventString8, softkeyString );
        TPtrC8 softkeyId;
        ParseParameterStringL( softkeyString, softkeyId );
        if ( softkeyString.Find( KAIMenuModeLsk ) >= 0)
            {
            GetTextFromElementL( iMenuModeLskLabel, softkeyId, aEvent );
            if ( iMenuModeLskLabel && iMenuModeLskLabel->Length() <= 0 )
                {
                delete iMenuModeLskLabel;
                iMenuModeLskLabel = NULL;
                }
            iLskMenuModeNode.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAIMenuModeRsk ) >= 0 )
            {
            GetTextFromElementL( iMenuModeRskLabel, softkeyId, aEvent );
            if ( iMenuModeLskLabel && iMenuModeRskLabel->Length() <= 0 )
                {
                delete iMenuModeRskLabel;
                iMenuModeRskLabel = NULL;
                }
            iRskMenuModeNode.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAINormalModeLskLabel ) >= 0)
            {
            GetTextFromElementL( iNormalModeLskLabel, softkeyId, aEvent );
            iNormalModeLskLabelId.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAINormalModeRskLabel ) >= 0)
            {
            GetTextFromElementL( iNormalModeRskLabel, softkeyId, aEvent );
            iNormalModeRskLabelId.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAINormalModeLskIcon ) >= 0)
            {
            iNormalModeLskIconId.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAINormalModeRskIcon ) >= 0)
            {
            iNormalModeRskIconId.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAIMenuLsk ) >= 0)
            {
            iLskId.Set( softkeyId );
            }
        else if ( softkeyString.Find( KAIMenuRsk ) >= 0)
            {
            iRskId.Set( softkeyId );
            }

        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventSettingsReady ) >= 0)
        {
        SetCBALabelsL( EAIMenuOff, aEvent );
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventDoMacroset ) >= 0 )
        {
        // Do defined macro set
        
        // Parse macroset node id
        TPtrC8 macroNodeId;
        ParseParameterStringL( aEventString8, macroNodeId );
        if ( macroNodeId != KNullDesC8 )
            {
            DoMacrosetL( macroNodeId, aEvent );
            }
        return ETrue;
        }
    else if ( aEventString8.Find( KAIMenuEventSetDynMenuItemObserver ) >= 0 )
        {
        TPtrC8 eventString;
        ParseParameterStringL( aEventString8, eventString );
        SetObserverL( eventString, aEvent );
        return ETrue;
        }
    else if ( aEventString8 ==  KAIToggleWidgetsState )
        {
        iFwEventHandler->SetPluginsVisibility( iUiEngine->WidgetsVisible() );
        return ETrue;
        }
    else
        {
        // Handlers for macroset operations
        TPtrC8 targetNodeName;
        ParseParameterStringL( aEventString8, targetNodeName );
        TBool handledEvent = EFalse;
        
        if ( targetNodeName != KNullDesC8 )
            {
            CXnNodeAppIf* eventTargetNode = NULL;
            if ( aEvent )
                {
                eventTargetNode = iUiEngine->FindNodeByIdL( targetNodeName, aEvent->Namespace() );
                }
            if ( eventTargetNode )
                {
                handledEvent = HandleMacroOperationL(
                                    aEventString8, *eventTargetNode, *aEvent );
                iUiEngine->RenderUIL( NULL );
                }
            }
        return handledEvent;
        }
    }

void CAIXuikonMenuEventHandler::ParseParameterStringL( const TDesC8& aSourcePtr,
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

void CAIXuikonMenuEventHandler::SetCBALabelsL( TAICBA aCBA, CXnDomNode* aEvent )
    {
    // Set CBA labels according to mode, to the predefined CBA nodes
    if ( (iLskId == KNullDesC8) || (iRskId == KNullDesC8) || !aEvent )
        {
        return;
        }
    CXnNodeAppIf* lskNode = iUiEngine->FindNodeByIdL( iLskId, aEvent->Namespace() );
    CXnNodeAppIf* rskNode = iUiEngine->FindNodeByIdL( iRskId, aEvent->Namespace() );

    TBool propertyUpdated = EFalse;

    if ( rskNode && lskNode )
        {
        CXnNodeAppIf* menuBar = lskNode->ParentL();

        XnMenuInterface::MXnMenuInterface* lskControl = NULL;
        XnMenuInterface::MXnMenuInterface* rskControl = NULL;

        if ( menuBar )
            {
            XnComponentInterface::MakeInterfaceL( lskControl, *menuBar );
            XnComponentInterface::MakeInterfaceL( rskControl, *menuBar );
            }
        if ( !lskControl || !rskControl )
            {
            return;
            }
        switch ( aCBA )
            {
            case EAIMenuOff:
                {
                CXnNodeAppIf* lskIconNode = NULL;
                CXnNodeAppIf* rskIconNode = NULL;
                XnImageInterface::MXnImageInterface* lskIconControl = NULL;
                XnImageInterface::MXnImageInterface* rskIconControl = NULL;
                if ( iNormalModeLskIconId != KNullDesC8 )
                    {
                    lskIconNode = iUiEngine->FindNodeByIdL( iNormalModeLskIconId, aEvent->Namespace() );
                    XnComponentInterface::MakeInterfaceL( lskIconControl, *lskIconNode );
                    }

                if ( iNormalModeRskIconId != KNullDesC8 )
                    {
                    rskIconNode = iUiEngine->FindNodeByIdL( iNormalModeRskIconId, aEvent->Namespace() );
                    XnComponentInterface::MakeInterfaceL( rskIconControl, *rskIconNode );
                    }

                // Try to set the icon, if the icon setting fails
                // try to set the text
                if ( !AttemptRestoreSoftkeyIconL( lskControl,
                     lskIconControl,
                     lskIconNode,
                     XnMenuInterface::MXnMenuInterface::ELeft ) )
                    {
                    if ( iNormalModeLskLabelId != KNullDesC8 )
                        {
                        GetTextFromElementL( iNormalModeLskLabel, iNormalModeLskLabelId, aEvent );
                        }

                    SetSoftkeyLabelL( lskControl, iNormalModeLskLabel,
                            XnMenuInterface::MXnMenuInterface::ELeft );
                    }

                if ( !AttemptRestoreSoftkeyIconL( rskControl,
                     rskIconControl,
                     rskIconNode,
                     XnMenuInterface::MXnMenuInterface::ERight ) )
                    {
                    if ( iNormalModeRskLabelId != KNullDesC8 )
                        {
                        GetTextFromElementL( iNormalModeRskLabel, iNormalModeRskLabelId, aEvent );
                        }

                    SetSoftkeyLabelL( rskControl, iNormalModeRskLabel,
                            XnMenuInterface::MXnMenuInterface::ERight );
                    }
                propertyUpdated = ETrue;
                break;
                }
            case EAIMenuOn:
                {
                // Need to set the bitmaps back after
                // the menu is closed
                iMenumodeBitmapLsk = NULL;
                iMenumodeBitmapRsk = NULL;
                if ( !iMenuModeLskLabel )
                    {
                    GetTextFromElementL( iMenuModeLskLabel, iLskMenuModeNode, aEvent );
                    }
                SetSoftkeyLabelL( lskControl, iMenuModeLskLabel,
                        XnMenuInterface::MXnMenuInterface::ELeft );

                if ( !iMenuModeRskLabel )
                    {
                    GetTextFromElementL( iMenuModeRskLabel, iRskMenuModeNode, aEvent );
                    }
                SetSoftkeyLabelL( rskControl, iMenuModeRskLabel,
                        XnMenuInterface::MXnMenuInterface::ERight );
                propertyUpdated = ETrue;
                break;
                }
            default:
                break;
            }
        }
    iCurrentCBA = aCBA;
    if ( propertyUpdated )
        {
    	iUiEngine->RefreshMenuL();
        }
    }

void CAIXuikonMenuEventHandler::ShowMenuL( TBool aShow, const TDesC8& aMenuId, CXnDomNode* aEvent )
    {
    if ( aEvent )
        {
        // Show or hide all menu items
        CXnNodeAppIf* lskNode = iUiEngine->FindNodeByIdL( aMenuId, aEvent->Namespace() );

        if ( lskNode )
            {
            RecurseChildrenL( aShow, lskNode );
            }
        }
    }

void CAIXuikonMenuEventHandler::RecurseChildrenL( TBool aShow, CXnNodeAppIf* aNode )
    {
    RPointerArray<CXnNodeAppIf> children = aNode->ChildrenL();
    CleanupClosePushL( children );

    for ( TInt i=0; i<children.Count(); i++ )
        {
        CXnNodeAppIf* node = children[i];
        if ( node && ( node->Type()->Type() == KAIMenuItem || 
                       node->Type()->Type() == KAIMenu ) )
            {
            if ( aShow )
                {
                SetPropertyToNodeL( *node, XnPropertyNames::style::common::KVisibility,
                    XnPropertyNames::style::common::visibility::KVisible );
                }
            else
                {
                SetPropertyToNodeL( *node, XnPropertyNames::style::common::KVisibility,
                    XnPropertyNames::style::common::visibility::KHidden );
                }

            if ( node->Type()->Type() == KAIMenu )
                {
                RecurseChildrenL(aShow, node);
                }
            }
        }

    CleanupStack::PopAndDestroy( &children );
    }

void CAIXuikonMenuEventHandler::DoMacrosetL( const TDesC8& aMacrosetNodeId, CXnDomNode* aEvent )
    {
    if ( !aEvent )
        {
        return;
        }
    // Do operations in a macroset
    CXnNodeAppIf* actionNode = iUiEngine->FindNodeByIdL( aMacrosetNodeId, aEvent->Namespace() );
    if ( !actionNode )
        {
        return;
        }
    RPointerArray<CXnNodeAppIf> children = actionNode->ChildrenL();
    for ( TInt i=0; i<children.Count(); i++ )
        {
        CXnNodeAppIf* node = children[i];
        const TDesC8* value =
            PropertyValue( *node, XnPropertyNames::action::event::KName );
        if ( value )
            {
            // Set display none
            TPtrC8 targetNodeName;
            ParseParameterStringL( *value, targetNodeName );
            
            if ( targetNodeName != KNullDesC8 )
                {
                CXnNodeAppIf* eventTargetNode = iUiEngine->FindNodeByIdL( targetNodeName, aEvent->Namespace() );
                if ( eventTargetNode )
                    {
                    HandleMacroOperationL( *value, *eventTargetNode, *aEvent );
                    }
                }
            }
        }
    children.Reset();
    }

void CAIXuikonMenuEventHandler::ShowMenuItemL( const TDesC8& aMenuItemId, TBool aShow, CXnDomNode* aEvent )
    {
    if ( aEvent )
        {
        // Show a single menuitem
        CXnNodeAppIf* itemNode = iUiEngine->FindNodeByIdL( aMenuItemId, aEvent->Namespace() );
        if ( itemNode && ( itemNode->Type()->Type() == KAIMenuItem ||
                           itemNode->Type()->Type() == KAIMenu ) )
            {
            if ( aShow )
                {
                SetPropertyToNodeL( *itemNode, XnPropertyNames::style::common::KVisibility,
                        XnPropertyNames::style::common::visibility::KVisible );
                }
            else
                {
                SetPropertyToNodeL( *itemNode, XnPropertyNames::style::common::KVisibility,
                        XnPropertyNames::style::common::visibility::KHidden );
                }
            }
        }
    }

TBool CAIXuikonMenuEventHandler::IsMenuItemVisibleL( const TDesC8& aMenuItemId, CXnDomNode* aEvent )
    {
    TBool ret = EFalse;
    if ( aEvent )
        {
        CXnNodeAppIf* itemNode = iUiEngine->FindNodeByIdL( aMenuItemId, aEvent->Namespace() );
        if ( itemNode && ( itemNode->Type()->Type() == KAIMenuItem ||
                           itemNode->Type()->Type() == KAIMenu ) )
            {
            CXnProperty* visibility = itemNode->GetPropertyL( XnPropertyNames::style::common::KVisibility );
            if ( visibility && (visibility->StringValue() == XnPropertyNames::style::common::visibility::KVisible) )
                {
                ret = ETrue;
                }
            }
        }
    return ret;
    }

void CAIXuikonMenuEventHandler::GetTextFromElementL( HBufC8*& aMemberPtr,
                                                     const TDesC8& aItemId,
                                                     CXnDomNode* aEvent )
    {
    delete aMemberPtr;
    aMemberPtr = NULL;
    if ( aEvent )
        {
        CXnNodeAppIf* labelNode = iUiEngine->FindNodeByIdL( aItemId, aEvent->Namespace() );
        
        if ( labelNode )
            {
            if ( labelNode->Type() )
                {
                const TDesC8& type = labelNode->Type()->Type();
                if ( type == XnTextInterface::MXnTextInterface::Type() )
                    {
                    XnTextInterface::MXnTextInterface* textControl = NULL;
                    XnComponentInterface::MakeInterfaceL( textControl, *labelNode );
                    if ( textControl )
                        {
                        // Set text
                        aMemberPtr = iStrParser->CopyToBufferL( aMemberPtr,
                                                                *(textControl->Text()) );
                        }
                    }
                }
            }
        }
    }

TBool CAIXuikonMenuEventHandler::HandleMacroOperationL( const TDesC8& aSourcePtr,
                                                        CXnNodeAppIf& aTargetNode,
                                                        CXnDomNode& aEvent )
    {
    if ( aSourcePtr.Find( KAIMenuEventSetFocus ) >= 0 )
        {
        // Set focus
        aTargetNode.SetStateL( XnPropertyNames::style::common::KFocus );
        return ETrue;
        }
    else if ( aSourcePtr.Find( KAIMenuEventSetDisplayNone ) >= 0 )
        {
        SetPropertyToNodeL( aTargetNode, XnPropertyNames::style::common::KDisplay,
                XnPropertyNames::style::common::display::KNone );
        return ETrue;
        }
    else if ( aSourcePtr.Find( KAIMenuEventSetDisplayBlock) >= 0 )
        {
        SetPropertyToNodeL( aTargetNode, XnPropertyNames::style::common::KDisplay,
                XnPropertyNames::style::common::display::KBlock );
        return ETrue;
        }
    else 
        {
        HandleEventL( aSourcePtr, &aTargetNode, &aEvent );
        return ETrue;
        }
    }

TBool CAIXuikonMenuEventHandler::AttemptRestoreSoftkeyIconL(
                            XnMenuInterface::MXnMenuInterface* aControl,
                            XnImageInterface::MXnImageInterface* aIconControl,
                            CXnNodeAppIf* aSourceNode,
                            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos )
    {
    if ( aControl && aSourceNode )
        {
        const TDesC8* isVisible =
                    PropertyValue( *aSourceNode,
                            XnPropertyNames::style::common::KVisibility );

        // node not "visible" so assume that text has been published
        // after image
        if ( !isVisible || *isVisible == XnPropertyNames::style::common::visibility::KHidden )
            {
            return EFalse;
            }

        CGulIcon* icon = NULL;
        TBool transferOwnership = ETrue;
        // First try to get the icon from image interface
        if ( aIconControl )
            {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            aIconControl->ContentBitmaps( bitmap, mask );
            if ( bitmap )
                {
                icon = CGulIcon::NewL( bitmap, mask );
                icon->SetBitmapsOwnedExternally( ETrue );
                // image adapter wants to keep ownership
                transferOwnership = EFalse;
                CleanupStack::PushL( icon );
                }
            }
        // Then try to load it ourselves.
        if ( !icon )
            {
            const TDesC8* value =
                PropertyValue( *aSourceNode, XnPropertyNames::image::KPath );

            if ( value )
                {
                icon = LoadIconLC( *aSourceNode );
                icon->SetBitmapsOwnedExternally( ETrue );
                // transfer ownerhsip to menunode
                transferOwnership = ETrue;
                }
            }
        // This icon is already in menu, no need to set it
        // again
        if ( icon &&
             (icon->Bitmap() == iMenumodeBitmapLsk ||
             icon->Bitmap() == iMenumodeBitmapRsk) )
            {
            CleanupStack::PopAndDestroy( icon );
            return ETrue;
            }
        else if ( icon )
            {
            switch ( aPos )
                {
                case XnMenuInterface::MXnMenuInterface::ELeft:
                    iMenumodeBitmapLsk = icon->Bitmap();
                    break;
                case XnMenuInterface::MXnMenuInterface::ERight:
                    iMenumodeBitmapRsk = icon->Bitmap();
                    break;
                default:
                    break;
                }
            aControl->SetSoftKeyImageL(
                icon->Bitmap(),
                icon->Mask(),
                aPos,
                ETrue, /* preserve aspect ratio */
                EFalse, /* !invert mask */
                transferOwnership
                );
            CleanupStack::PopAndDestroy( icon );
            return ETrue;
            }
        }
    return EFalse;
    }

void CAIXuikonMenuEventHandler::SetSoftkeyLabelL(
                            XnMenuInterface::MXnMenuInterface* aControl,
                            const TDesC8* aLabel,
                            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos )
    {
    if ( aControl && aLabel )
        {
        HBufC16* string16 = NULL;
        string16 = iStrParser->CopyToBufferL( string16, *aLabel );
        CleanupStack::PushL( string16 );
        // Trapped because this will leave when setting the same again
        TRAP_IGNORE(
            aControl->SetSoftKeyTextL( *string16, aPos );
            )
        CleanupStack::PopAndDestroy( string16 );
        }
    }

TBool CAIXuikonMenuEventHandler::InitEditModeMenuItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aFocused, const TDesC8& aSource )
    {
    TBool showItem( EFalse );
    CXnNodeAppIf* focused( aFocused );
    
    if ( focused && aSource == KContentModel )
        {
        CXnNodeAppIf* node = FindFromContentModelL( focused );
        if ( !node ) 
            {
            // no plugin or no launch action
            return showItem;
            }
        CXnProperty* plugin ( node->GetPropertyL( KName ) ) ;
        if ( !plugin )
            {
            // no plugin or no launch action
            return showItem;
            }
        HBufC* pluginName( plugin->StringValueL() );
        CleanupStack::PushL( pluginName );
        HBufC* item ( aMenuItem.GetPropertyL( KName )->StringValueL() ); 
        CleanupStack::PushL( item );

        showItem = iFwEventHandler->HasMenuItem( *pluginName , *item );

        if ( showItem )
            {
            // pluginName/menuitem(item)
            // Create launch event and store it to menuitem's property
            HBufC8* eventstr = HBufC8::NewLC( pluginName->Length() 
                                              + 15 // / ( ) 
                                              + item->Length() );
            TPtr8 str ( eventstr->Des() );
            str.Copy( pluginName->Des() );
            str.Append( TChar( '/' ));
            str.Append( KMenuItem );
            str.Append( TChar( '(' ));
            str.Append( item->Des() );
            str.Append( TChar( ')' ));

            CXnDomStringPool& sp( iUiEngine->StringPool() );

            CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( sp );
            CleanupStack::PushL( newValue );

            newValue->SetStringValueL( CXnDomPropertyValue::EString, str );

            CXnProperty* prop = CXnProperty::NewL( XnPropertyNames::menu::KEvent, 
                                                   newValue, sp );

            CleanupStack::Pop( newValue );

            CleanupStack::PushL( prop );

            // and set it
            aMenuItem.SetPropertyL( prop );
            CleanupStack::Pop( prop );

            CleanupStack::PopAndDestroy(eventstr ); // eventstr, children
            }

        CleanupStack::PopAndDestroy( item );
        CleanupStack::PopAndDestroy( pluginName );
        }
    else if ( focused && aSource == KConfigurationModel ) 
        {
        CXnNodeAppIf* node( FindFromConfigurationL( focused ) );

        if ( node )
            {
            CXnProperty* launch( node->GetPropertyL( KLaunch ) );
            CXnProperty* plugin( node->GetPropertyL( KName ) );

            if ( !launch || !plugin )
                {
                // no plugin or no launch action
                return showItem;
                }

            HBufC* pluginName( plugin->StringValueL() );
            CleanupStack::PushL( pluginName );

            CXnProperty* prop( aMenuItem.GetPropertyL( KName ) );

            if ( prop )
                {
                HBufC* item( prop->StringValueL() );
                CleanupStack::PushL( item );

                showItem = iFwEventHandler->HasMenuItem( *pluginName, *item );

                CleanupStack::PopAndDestroy( item );
                }
            else
                {
                showItem = iFwEventHandler->HasMenuItem( *pluginName, KNullDesC );
                }

            CleanupStack::PopAndDestroy( pluginName );

            if ( showItem )
                {
                // Create launch event and store it to menuitem's property
                RPointerArray<CXnNodeAppIf> children( node->ChildrenL() );
                CleanupClosePushL( children );

                TInt count( children.Count() );

                TInt length( launch->StringValue().Length() + 2 ); // +2 for '(' and ')'

                for ( TInt i = 0; i < count; i++ )
                    {
                    if ( i > 0 )
                        {
                        length += 1;
                        }

                    CXnProperty* prop( children[i]->GetPropertyL( KName ) );

                    if ( prop )
                        {
                        length += prop->StringValue().Length();
                        }
                    }

                HBufC8* eventstr = HBufC8::NewLC( length );

                TPtr8 str( eventstr->Des() );

                str.Append( launch->StringValue() );

                str.Append( TChar( '(' ) );

                for ( TInt i = 0; i < count; i++ )
                    {
                    if ( i > 0 )
                        {
                        str.Append( TChar( ',' ) ); 
                        }

                    CXnProperty* prop( children[i]->GetPropertyL( KName ) );

                    if ( prop )
                        {
                        str.Append( prop->StringValue() );
                        }
                    }

                str.Append( TChar( ')' ) );

                CXnDomStringPool& sp( iUiEngine->StringPool() );

                CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( sp );
                CleanupStack::PushL( newValue );

                newValue->SetStringValueL( CXnDomPropertyValue::EString, str );

                CXnProperty* prop = CXnProperty::NewL( XnPropertyNames::menu::KEvent, 
                                                       newValue, sp );

                CleanupStack::Pop( newValue );

                CleanupStack::PushL( prop );

                // and set it
                aMenuItem.SetPropertyL( prop );
                CleanupStack::Pop( prop );

                CleanupStack::PopAndDestroy( 2, &children ); // eventstr, children
                }
            }
        }

    return showItem;
    }

TBool CAIXuikonMenuEventHandler::InitDefaultModeMenuItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aFocused, const TDesC8& aSource )
    {
    TBool showItem( EFalse );
    TInt len( 0 );
    if ( aSource == KContentModel )
        {
        CXnProperty* prop( aMenuItem.GetPropertyL( KName ) );
        if ( !prop )
            {
            return showItem;
            }

        RPointerArray<HBufC16> plnNames;
        CleanupStack::PushL( TCleanupItem( DeletePluginNames, &plnNames ) );

        CXnNodeAppIf* focused( aFocused );
        HBufC* item ( prop->StringValueL() );
	    CleanupStack::PushL( item );
	   
	    if( item->Des() == KHSOnLine_OffLine() ) 
	    	{
	    	TBool webContent = EFalse;
	    	RPointerArray<CXnNodeAppIf> plugins = iUiEngine->FindNodeByClassL( KContentSource16 ) ;
	    	CleanupClosePushL( plugins );
			if( plugins.Count() <= 0  )
				{
				CleanupStack::PopAndDestroy(&plugins);
				CleanupStack::PopAndDestroy( item );
				CleanupStack::PopAndDestroy(&plnNames);
				return showItem;
				}
			for (TInt i = 0; i < plugins.Count(); i++)
				{
				 CXnProperty* property ( plugins[i]->GetPropertyL( KName ) ) ;
				 if ( property )
					 {
					 HBufC16* pluginName ( property->StringValueL() );
					 CleanupStack::PushL( pluginName ); 
					 if( iFwEventHandler->HasMenuItem( *pluginName , *item ) )
						 {
                         webContent = ETrue;	 
                         CleanupStack::PopAndDestroy( pluginName );
						 break;
						 }
				     CleanupStack::PopAndDestroy( pluginName );
					 }
				}
			CleanupStack::PopAndDestroy(&plugins);
			
			// At least one widget using web content
	    	if ( webContent )
	    		{
	    		CXnProperty* status( aMenuItem.GetPropertyL(KStatus) );
				if ( status )
					{
					HBufC* statusValue ( status->StringValueL() );
					CleanupStack::PushL( statusValue );
					if ( (statusValue->Des() == KHSOnLine()) && !iFwEventHandler->IsPluginsOnline() )
					   {
					   plnNames.AppendL( KHSOnLine().Alloc() );
					   len = KHSOnLine().Length();
					   }
					else if ( (statusValue->Des() == KHSOffLine()) && iFwEventHandler->IsPluginsOnline() )
					   {
					   plnNames.AppendL( KHSOffLine().Alloc() );
					   len = KHSOffLine().Length();
					   }
					 CleanupStack::PopAndDestroy( statusValue );
					}
	    		}
	    	}
	    else
	    	{
			if( focused )  
				{
				RPointerArray< CXnNodeAppIf > plugins;
				CleanupClosePushL( plugins );
				
				iUiEngine->GetPluginNodeArrayL( plugins );
							
				CXnNodeAppIf* node( NULL );
				
				for( ; focused; focused = focused->ParentL() )
					{
					if( plugins.Find( focused ) != KErrNotFound )
						{                    
						node = FindFromContentModelL( focused );                    
						break;
						}
					}
					
				CleanupStack::PopAndDestroy( &plugins );
							 
				 if( !node ) 
					 {
					 // no plugin or no launch action
					 CleanupStack::PopAndDestroy(item );
					 CleanupStack::PopAndDestroy(&plnNames);
					 return showItem;
					 }
				 CXnProperty* plugin ( node->GetPropertyL( KName ) ) ;
				 if( !plugin )
					{
					// no plugin or no launch action
					CleanupStack::PopAndDestroy(item );
					CleanupStack::PopAndDestroy(&plnNames);
					return showItem;
					}
				 HBufC16* pluginName( plugin->StringValueL() );
				 CleanupStack::PushL( pluginName );
				 if( iFwEventHandler->HasMenuItem( *pluginName , *item ) )
					 {
					 plnNames.AppendL( pluginName );
					 len += pluginName->Length();
					 CleanupStack::Pop( pluginName );
					 } 
				 else
					 {
					 CleanupStack::PopAndDestroy( pluginName );
					 }
				}
		   else
			   {
				RPointerArray<CXnNodeAppIf> plugins = iUiEngine->FindNodeByClassL( KContentSource16 ) ;
				if( plugins.Count() <= 0  )
					{
					CleanupStack::PopAndDestroy( item );
					CleanupStack::PopAndDestroy(&plnNames);
					return showItem;
					}
				CleanupClosePushL( plugins );
				for (TInt i = 0; i < plugins.Count(); i++)
					{
					 CXnProperty* property ( plugins[i]->GetPropertyL( KName ) ) ;
					 if ( property )
						 {
						 HBufC16* pluginName ( property->StringValueL() );
						 CleanupStack::PushL( pluginName ); 
						 if( iFwEventHandler->HasMenuItem( *pluginName , *item ) )
							 {
							 plnNames.AppendL( pluginName );	 
							 len += pluginName->Length();
							 CleanupStack::Pop( pluginName );
							 }
						 else
							 {
							 CleanupStack::PopAndDestroy( pluginName );
							 }
						 }
					}
				 CleanupStack::PopAndDestroy(&plugins);
			   }
	    	}
	    
         TInt count = plnNames.Count();
    	 if( count > 0 )
            {
            showItem = ETrue;
            // [pluginName1,...]/menuitem(item)
            // Create launch event and store it to menuitem's property
            HBufC8* eventstr = HBufC8::NewLC( len + 
                                              + 15 // / ( ) [ ]
                                              + item->Length() );
            TPtr8 str ( eventstr->Des() );
            str.Copy( KOpenBrace );
            for ( TInt i = 0; i < count; i++ )
                {
                str.Append( plnNames[i]->Des() );
                if ( i+1 != count ) str.Append( TChar( ',' ) );
                }
            str.Append( KCloseBrace );
            str.Append( TChar( '/' ) );
            str.Append( KMenuItem );
            str.Append( TChar( '(' ) );
            str.Append( item->Des() );
            str.Append( TChar( ')' ) );

            CXnDomStringPool& sp( iUiEngine->StringPool() );

            CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( sp );
            CleanupStack::PushL( newValue );

            newValue->SetStringValueL( CXnDomPropertyValue::EString, str );

            CXnProperty* prop = CXnProperty::NewL( XnPropertyNames::menu::KEvent, 
                                                   newValue, sp );

            CleanupStack::Pop( newValue );

            CleanupStack::PushL( prop );

            // and set it
            aMenuItem.SetPropertyL( prop );
            CleanupStack::Pop( prop );

            CleanupStack::PopAndDestroy( eventstr ); // eventstr, children
            }

        CleanupStack::PopAndDestroy( item );
        CleanupStack::PopAndDestroy( &plnNames );
        }
    else if ( aFocused && aSource == KConfigurationModel ) 
        {
        CXnNodeAppIf* node( FindFromConfigurationL( aFocused ) );

        if ( node )
            {
            CXnProperty* launch( node->GetPropertyL( KLaunch ) );
            CXnProperty* plugin( node->GetPropertyL( KName ) );

            if ( !launch || !plugin )
                {
                // no plugin or no launch action
                return showItem;
                }

            HBufC* pluginName( plugin->StringValueL() );
            CleanupStack::PushL( pluginName );

            CXnProperty* prop( aMenuItem.GetPropertyL( KName ) );

            if ( prop )
                {
                HBufC* item( prop->StringValueL() );
                CleanupStack::PushL( item );

                showItem = iFwEventHandler->HasMenuItem( *pluginName, *item );

                CleanupStack::PopAndDestroy( item );
                }
            else
                {
                showItem = iFwEventHandler->HasMenuItem( *pluginName, KNullDesC );
                }

            CleanupStack::PopAndDestroy( pluginName );

            if ( showItem )
                {
                // Create launch event and store it to menuitem's property
                RPointerArray<CXnNodeAppIf> children( node->ChildrenL() );
                CleanupClosePushL( children );

                TInt count( children.Count() );

                TInt length( launch->StringValue().Length() + 2 ); // +2 for '(' and ')'

                for ( TInt i = 0; i < count; i++ )
                    {
                    if ( i > 0 )
                        {
                        length += 1;
                        }

                    CXnProperty* prop( children[i]->GetPropertyL( KName ) );

                    if ( prop )
                        {
                        length += prop->StringValue().Length();
                        }
                    }

                HBufC8* eventstr = HBufC8::NewLC( length );

                TPtr8 str( eventstr->Des() );

                str.Append( launch->StringValue() );

                str.Append( TChar( '(' ) );

                for ( TInt i = 0; i < count; i++ )
                    {
                    if ( i > 0 )
                        {
                        str.Append( TChar( ',' ) ); 
                        }

                    CXnProperty* prop( children[i]->GetPropertyL( KName ) );

                    if ( prop )
                        {
                        str.Append( prop->StringValue() );
                        }
                    }

                str.Append( TChar( ')' ) );

                CXnDomStringPool& sp( iUiEngine->StringPool() );

                CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( sp );
                CleanupStack::PushL( newValue );

                newValue->SetStringValueL( CXnDomPropertyValue::EString, str );

                CXnProperty* prop = CXnProperty::NewL( XnPropertyNames::menu::KEvent, 
                                                       newValue, sp );

                CleanupStack::Pop( newValue );

                CleanupStack::PushL( prop );

                // and set it
                aMenuItem.SetPropertyL( prop );
                CleanupStack::Pop( prop );

                CleanupStack::PopAndDestroy( 2, &children ); // eventstr, children
                }
            }
        }

    // Currently we don't support default mode dynamic menu item for ConfigurationModel
    return showItem;
    }

CXnProperty* CAIXuikonMenuEventHandler::GetWidgetPropertyL(CXnNodeAppIf& aPlugin, const TDesC8& aProperty)
    {
    CXnProperty* retval = NULL;
    RPointerArray<CXnNodeAppIf> children(aPlugin.ChildrenL());
    CleanupClosePushL( children );
    TInt childCount = children.Count();
    for (TInt i = 0; i < childCount; i++)
        {
        //search for widget element and get given property
        CXnNodeAppIf* widget = children[i];
        if (widget && widget->InternalDomNodeType() == KWidget)
            {
            retval = widget->GetPropertyL(aProperty);
            i = childCount;
            }
        }
    CleanupStack::PopAndDestroy( &children );
    return retval;
    }

TBool CAIXuikonMenuEventHandler::DynInitItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aFocused )
    {
    CXnProperty* prop( aMenuItem.GetPropertyL( KSource ) );
    if ( prop )
        {
        if ( prop && prop->StringValue().Length() > 0 )
            {
            if ( iUiEngine->IsEditMode() )
                {
                return InitEditModeMenuItemL( aMenuItem, aFocused, prop->StringValue() );
                }
            else
                {
                return InitDefaultModeMenuItemL( aMenuItem, aFocused, prop->StringValue() ); 
                }
            }
        }
    else
        {
        CXnProperty* idProp( aMenuItem.GetPropertyL( KId ) );
        if ( idProp )
            {
            const TDesC8& id = idProp->StringValue();
            if ( id == KEditWidget )
                {                
                if ( aFocused )
                    {
                    for ( CXnNodeAppIf* candidate = aFocused; candidate; candidate = candidate->ParentL() )
                        {
                        if ( candidate->InternalDomNodeType() == KPlugin && !iUiEngine->IsEditMode() )
                            {
                            return ETrue;
                            }
                        }
                    }
                }
            else if( id == KRemoveItem && iUiEngine->IsEditMode() )
                {
                // get plugin node from focused
                CXnNodeAppIf* node = aFocused;
                CXnNodeAppIf* pluginNode = NULL;
                for(; node && node->Type()->Type() != KView; node = node->ParentL() )
                    {
                    CXnProperty* configurationId( node->GetPropertyL( KConfigurationId ) );
                    CXnProperty* pluginId( node->GetPropertyL( KPluginId ) );
                    
                    if( configurationId && pluginId )
                        {           
                        pluginNode = node;
                        break;
                        }
                    }
                
                if( pluginNode )
                    {
                    // check if plugin can be removed
                    CXnProperty* removable = GetWidgetPropertyL(*pluginNode, KRemovable);
                    if ( removable && removable->StringValue() == XnPropertyNames::KFalse )
                        {
                        return EFalse;
                        }
                    // hide menu item if the focused plugin is an "empty" widget
                    CXnProperty* pluginName = pluginNode->GetPropertyL( KPluginName );
                    if( pluginName )
                        {
                        const TDesC8& nameStr = pluginName->StringValue();
                        if( nameStr == KDummy8 )
                            {
                            return EFalse;
                            }
                        else
                            {
                            return ETrue;
                            }
                        }
                    }
                }
            }
        }
        
    return EFalse;        
    }


TBool CAIXuikonMenuEventHandler::DynInitMenuItemL( CXnNodeAppIf& aMenuItem )
    {      
    TBool ret = EFalse;
    if( !iUiEngine->WidgetsVisible() )
        {
        // only online/offline dynamic menu item is allowed to show now
        CXnProperty* prop( aMenuItem.GetPropertyL(KName) );
        if ( prop )
        	{
        	HBufC* item ( prop->StringValueL() );
        	CleanupStack::PushL( item );
        	if (item->Des() == KHSOnLine_OffLine() )
        		{
        		ret = DynInitItemL( aMenuItem, iUiEngine->FocusedNode() );
        		}
        	CleanupStack::PopAndDestroy( item );
        	}
        }
    else
    	{
    	ret = DynInitItemL( aMenuItem, iUiEngine->FocusedNode() );
    	}
    return ret;
    }
    
