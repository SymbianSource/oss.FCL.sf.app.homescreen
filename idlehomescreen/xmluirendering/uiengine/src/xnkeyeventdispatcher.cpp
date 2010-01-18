/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event dispatcher
*
*/

// System includes
#include <AknUtils.h>

// User includes
#include "xnappuiadapter.h"
#include "xnfocuscontrol.h"
#include "xnkeyeventdispatcher.h"
#include "xntype.h"
#include "xncomponentnodeimpl.h"
#include "xnproperty.h"
#include "xnuiengine.h"
#include "xnodt.h"
#include "xndomdocument.h"
#include "xndomnode.h"
#include "xnmenuadapter.h"
#include "xneditmode.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xnnode.h"

// Local macros
#define IS_ARROW_KEY( k ) \
    ( k == EStdKeyLeftArrow || k == EStdKeyRightArrow || \
      k == EStdKeyUpArrow || k == EStdKeyDownArrow ) 

// -----------------------------------------------------------------------------
// SetInitialFocusL
// -----------------------------------------------------------------------------
//
static void SetInitialFocusL( RPointerArray< CXnNode >& aArray )
    {
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        CXnNode* node( aArray[i] );

        node->SetStateWithoutNotificationL(
            XnPropertyNames::style::common::KFocus );

        if ( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// BuildTriggerNodeL
// Builds a trigger node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerNodeL(
    CXnUiEngine& aUiEngine,
    const TDesC8& aTriggerName )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );

    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );

    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );

    CleanupStack::Pop( type );

    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );

    CXnDomPropertyValue* nameValue =
        CXnDomPropertyValue::NewL( aUiEngine.ODT()->DomDocument().StringPool() );

    CleanupStack::PushL( nameValue );

    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );

    CXnProperty* name =
        CXnProperty::NewL( XnPropertyNames::action::trigger::KName, nameValue,
                           *aUiEngine.ODT()->DomDocument().StringPool() );

    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );

    CleanupStack::Pop( node );

    return node;
    }

// -----------------------------------------------------------------------------
// MenuAdapter
// Gets menuadapter from node, NULL if not available
// -----------------------------------------------------------------------------
//
static CXnMenuAdapter* MenuAdapter( CXnNode* aNode )
    {
    if ( aNode )
        {        
        return static_cast< CXnMenuAdapter* >( aNode->Control() );
        }

    return NULL;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CXnKeyEventDispatcher* CXnKeyEventDispatcher::NewL( CXnUiEngine& aUiEngine )
    {
    CXnKeyEventDispatcher* self = new ( ELeave ) CXnKeyEventDispatcher( aUiEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::~CXnKeyEventDispatcher
// Destructor.
// -----------------------------------------------------------------------------
//
CXnKeyEventDispatcher::~CXnKeyEventDispatcher()
    {
    iCoeEnv->RemoveMessageMonitorObserver( *this );
    
    iUiEngine.ViewManager()->RemoveObserver( *this );
    
    delete iLoseFocus;
    delete iGainFocus;
    iPassiveFocusedNodes.Reset();
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::CXnKeyEventDispatcher
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnKeyEventDispatcher::CXnKeyEventDispatcher( CXnUiEngine& aUiEngine )
    : iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::ConstructL
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::ConstructL()
    {
    MakeVisible( EFalse );

    iUiEngine.ViewManager()->AddObserver( *this );
           
    iCoeEnv->AddMessageMonitorObserverL( *this );    
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::MonitorWsMessage
// 
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::MonitorWsMessage( const TWsEvent& aEvent )
    {
    TInt type( aEvent.Type() );
    
    if ( type == EEventPointer && AknLayoutUtils::PenEnabled() )
        {    
        const TUint handle( aEvent.Handle() );
        
        CCoeControl* destination = reinterpret_cast< CCoeControl* >( handle );        
                                       
        TPointerEvent& event( *aEvent.Pointer() );
                
        if ( iCbaContainer )
            {
            CEikCba* cba = 
                static_cast< CEikCba* >( iCbaContainer->ButtonGroup() ); 
                    
            if ( destination == cba && iCbaContainer->IsVisible() )
                {
                CXnMenuAdapter* adapter( MenuAdapter( iMenuNode ) );
                
                if ( adapter )
                    {
                    TRAP_IGNORE( 
                        adapter->HandlePointerEventL( *aEvent.Pointer() ) );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::OfferKeyEventL
// Handles key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CXnKeyEventDispatcher::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse resp( EKeyWasNotConsumed );
    
    iEventCode = aType;
    
    CXnNode* node( NULL );
    
    TBool keyYesNoApps( EFalse );
    
    if ( iUiEngine.IsMenuDisplaying() ||
         aKeyEvent.iScanCode == EStdKeyDevice0 ||
         aKeyEvent.iScanCode == EStdKeyDevice1 )
        {
        iFocusChanged = EFalse;
        // When menu is focused or LSK/RSK is pressed,
        // then forward events to menuadapter.
        // Note that MSK is handled directly in the base class as it used only
        // for activating
        node = iMenuNode;
        }
    else if ( aKeyEvent.iScanCode == EStdKeyApplication0 ||
              aKeyEvent.iScanCode == EStdKeyYes ||
              aKeyEvent.iScanCode == EStdKeyNo )
        {
        keyYesNoApps = ETrue;
        
        iFocusChanged = EFalse;
        // AppsKey, YesKey, NoKey events must be always handled, and if we don't
        // have a focused node, then let the view node do the job
        node = ( !iNode ) ? iUiEngine.ActiveView() : iNode;
        }
    else
        {
        CXnAppUiAdapter& appui( iUiEngine.AppUiAdapter() );
        
        if ( IS_ARROW_KEY( aKeyEvent.iScanCode ) && aType == EEventKey )
            {
            if ( !appui.FocusShown() )
                {
                appui.ShowFocus();
                
                if ( !iNode )
                    {
                    // Find initial location for focus
                    ResolveAndSetFocusL(); 
                                            
                    return EKeyWasConsumed;
                    }
                }
            }
        
        CCoeControl* editmode( iUiEngine.EditMode() );
        
        if ( editmode->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )
            {
            return EKeyWasConsumed;
            }
        
        // Offer keyevent to the focused node        
        node = iNode;

        if ( aType == EEventKeyDown )
            {
            // Reset state
            iFocusChanged = EFalse;
            iKeyEventNode = iNode;
            }

        if ( iFocusChanged && ( aType == EEventKeyUp ) )
            {
            // Pass keyup event to
            // previously focused node
            node = iKeyEventNode;
            }
        }

    if ( !keyYesNoApps )
        {
        if ( iEventCode == EEventNull && aType != EEventKeyDown )
            {    
            // We are waiting for down event
            return resp;
            }        
        }
        
    if ( !node )
        {
        return resp;
        }

    iUiEngine.DisableRenderUiLC();
    
    CXnControlAdapter* adapter( node->Control() );

    if( adapter && adapter->IsVisible() )                            
        {
        resp = adapter->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( aType != EEventKeyUp && iKeyEventNode != iNode )
        {
        // Focused node is changed during keyevent
        iFocusChanged = ETrue;
        }
    
    if ( aType == EEventKeyUp )
        {
        iEventCode = EEventNull;
        }

    iUiEngine.RenderUIL();
    CleanupStack::PopAndDestroy(); // DisableRenderUiLC
    
    return resp;
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::SetNode
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::SetNodeL( CXnNode* aNode, TInt aSource )
    {
    if ( iNode == aNode )
        {
        return;
        }

    iPreviousNode = iNode;
    iNode = aNode;

    SetNodeL( iPreviousNode, iNode, ETrue, aSource );
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::SetNodeWithoutNotificationL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::SetNodeWithoutNotificationL( CXnNode* aNode )
    {
    if ( iNode == aNode )
        {
        return;
        }

    iPreviousNode = iNode;
    iNode = aNode;

    SetNodeL( iPreviousNode, iNode, EFalse );
    }

// -----------------------------------------------------------------------------
// SetNodeL
// Changes focused node, runs gain focus/lose focus triggers if needed
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::SetNodeL( CXnNode* aToLose, CXnNode* aToGain,
    TBool aNotify, TInt aSource )
    {
    if ( aToLose )
        {
        aToLose->SetDirtyL( XnDirtyLevel::ERender );

        aToLose->UnsetStateL( XnPropertyNames::style::common::KFocus );
        aToLose->UnsetStateL( XnPropertyNames::style::common::KHold );
        aToLose->UnsetStateL( XnPropertyNames::style::common::KActive );

        if ( aNotify )
            {
            if ( !iLoseFocus )
                {
                iLoseFocus = BuildTriggerNodeL( iUiEngine,
                    XnPropertyNames::action::trigger::name::KLoseFocus );
                }

            aToLose->ReportXuikonEventL( *iLoseFocus );
            }

        CXnControlAdapter* adapter( aToLose->Control() );

        if ( adapter )
            {
            adapter->SetFocus( EFalse );
            }
        }

    if ( aToGain )
        {
        aToGain->SetDirtyL( XnDirtyLevel::ERender );

        if ( aNotify )
            {
            if ( !iGainFocus )
                {
                iGainFocus = BuildTriggerNodeL( iUiEngine,
                        XnPropertyNames::action::trigger::name::KGainFocus );
                }

            aToGain->ReportXuikonEventL( *iGainFocus, aSource );
            }

        CXnControlAdapter* adapter( aToGain->Control() );

        if ( adapter )
            {
            adapter->SetFocus( ETrue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::FocusedNode
// -----------------------------------------------------------------------------
//
CXnNode* CXnKeyEventDispatcher::FocusedNode() const
    {
    if ( iNode && iNode->IsStateSet( XnPropertyNames::style::common::KFocus ) )
        {
        return iNode;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::RefreshMenu
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::RefreshMenuL()
    {
    CXnMenuAdapter* menuAdapter( MenuAdapter( iMenuNode ) );

    if ( menuAdapter )
        {
        menuAdapter->SetContainerL( *iCbaContainer );
        }
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::SetMenuNode
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::SetMenuNodeL( CXnNode* aNode )
    {
    CXnMenuAdapter* menuAdapter( MenuAdapter( aNode ) );
    
    iCbaContainer = iAvkonAppUi->Cba();

    if ( menuAdapter )
        {
        iMenuNode = aNode;
                
        menuAdapter->SetContainerL( *iCbaContainer );

        CXnProperty* prop( iMenuNode->DisplayL() );

        if ( prop && prop->StringValue() == 
            XnPropertyNames::style::common::display::KNone )
            {
            iCbaContainer->MakeVisible( EFalse );
            }
        else
            {
            iCbaContainer->MakeVisible( ETrue );
            }
        }
    else
        {
        iCbaContainer->MakeVisible( EFalse );

        // The node passed in wasn't a valid menu node
        ResetMenuNodeL();
        }
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::ResetMenuNodeL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::ResetMenuNodeL()
    {
    iMenuNode = NULL;

    iCbaContainer = NULL;
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::IsMenuFocused
// -----------------------------------------------------------------------------
//
TBool CXnKeyEventDispatcher::IsMenuFocused() const
    {
    CXnMenuAdapter* menuAdapter( MenuAdapter( iMenuNode ) );

    if ( menuAdapter )
        {
        return menuAdapter->IsMenuFocused();
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::AddPassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::AddPassiveFocusedNodeL( CXnNode* aNode )
    {
    if ( !aNode->IsStateSet( XnPropertyNames::style::common::KFocus ) &&
         !aNode->IsStateSet( XnPropertyNames::style::common::KPassiveFocus ) )
        {
        iPassiveFocusedNodes.AppendL( aNode );
        aNode->SetStateL( XnPropertyNames::style::common::KPassiveFocus );
        }
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::RemovePassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::RemovePassiveFocusedNodeL( CXnNode* aNode )
    {
    for ( TInt i = 0; i < iPassiveFocusedNodes.Count(); i++ )
        {
        CXnNode* node( iPassiveFocusedNodes[i] );

        if ( node == aNode )
            {
            iPassiveFocusedNodes.Remove( i );
            
            aNode->UnsetStateL( 
                    XnPropertyNames::style::common::KPassiveFocus );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::AddPassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::ClearPassiveFocusedNodesL()
    {
    for ( TInt i = 0; i < iPassiveFocusedNodes.Count(); i++ )
        {
        iPassiveFocusedNodes[i]->UnsetStateL(
            XnPropertyNames::style::common::KPassiveFocus );
        }

    iPassiveFocusedNodes.Reset();
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::NotifyViewActivatedL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::NotifyViewActivatedL(
    const CXnViewData& /*aViewData*/ )
    {    
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::NotifyViewDeactivatedL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::NotifyViewDeactivatedL(
    const CXnViewData& /*aViewData*/ )
    {    
    iMenuNode = NULL;
    ClearPassiveFocusedNodesL();
    ClearStateL();
    
    iUiEngine.AppUiAdapter().HideFocus();
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::NotifyWidgetAdditionL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::NotifyWidgetAdditionL(
    const CXnPluginData& /*aPluginData*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::NotifyWidgetRemovalL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::NotifyWidgetRemovalL(
    const CXnPluginData& aPluginData )
    {
    if ( aPluginData.Active() )
        {
        ClearPassiveFocusedNodesL();

        if ( iNode )
            {                              
            CXnViewData& activeViewData(
                iUiEngine.ViewManager()->ActiveViewData() );
    
            const CXnPluginData& pluginData(
                activeViewData.Plugin( iNode ) );
    
            if ( &pluginData == &aPluginData )
                {
                // The plugin is removed which was holding focus
                ClearStateL();
                }
            }
        }    
    }

// -----------------------------------------------------------------------------
// CCXnKeyEventDispatcher::NotifyConfigureWidgetL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::NotifyConfigureWidgetL( 
    const CHsContentInfo& /*aContentInfo*/, CXnPluginData& /*aPluginData*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::ResolveAndSetFocusL
// Used to bring focus visible in the initial location when focus is invisible
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::ResolveAndSetFocusL() 
    {
    // <plugin> elements are always kept in appearance order
    RPointerArray< CXnNode >& list( *iUiEngine.Plugins() );
    
    if ( iUiEngine.IsEditMode() )
        {               
        SetInitialFocusL( list );        
        }
    else
        {
        RPointerArray< CXnNode > initial;
        CleanupClosePushL( initial );
        
        CXnViewData& activeView( iUiEngine.ViewManager()->ActiveViewData() );

        // first, search only in plugins which have popup window open
        for ( TInt i = 0; i < list.Count(); i++ )
            {
            CXnPluginData& plugin( activeView.Plugin( list[i] ) );
            if ( plugin.IsDisplayingPopup() )
                {
                plugin.InitialFocusNodesL( initial );
                }
            }
        
        // if no inital focus nodes were found in plugins with
        // open popups, search again with all plugins
        if ( initial.Count() == 0 )
            {        
            for ( TInt i = 0; i < list.Count(); i++ )
                {
                CXnPluginData& plugin( activeView.Plugin( list[i] ) );
                plugin.InitialFocusNodesL( initial );
                }
            }
        
        // set initial focus
        SetInitialFocusL( initial );
        
        CleanupStack::PopAndDestroy( &initial );        
        }    
    }

// -----------------------------------------------------------------------------
// CXnKeyEventDispatcher::ClearStateL
// -----------------------------------------------------------------------------
//
void CXnKeyEventDispatcher::ClearStateL()
    {
    SetNodeL( NULL );
    
    iNode = NULL;
    iPreviousNode = NULL;
    iKeyEventNode = NULL;
    iFocusChanged = EFalse;
    iEventCode = EEventNull;
    }

// End of file
