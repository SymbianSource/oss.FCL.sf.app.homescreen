/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Xuikon layout engine plugin interface
*
*/

// User includes
#include "xnuienginepluginif.h"
#include "xnuiengine.h"
#include "xnnode.h"
#include "xnnodepluginif.h"
#include "xnproperty.h"
#include "xnpointerarray.h"
#include "xnodt.h"
#include "xndomdocument.h"
#include "xneditmode.h"
#include "xneditor.h"
#include "xnviewdata.h"
#include "xndomnode.h"
#include "xnhittest.h"
#include "xnpanic.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::TXnUiEnginePluginIf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TXnUiEnginePluginIf::TXnUiEnginePluginIf( CXnUiEngine& aUiEngine )
    {
    iUiEngine = &aUiEngine;
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::RootNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf& TXnUiEnginePluginIf::RootNodeL()
    {
    CXnNode* node = iUiEngine->RootNode();
    return node->PluginIfL();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::FindNodeByIdL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf* TXnUiEnginePluginIf::FindNodeByIdL(
    const TDesC& aNodeId,
    const TDesC& aNamespace )
    {
    CXnNode* node = iUiEngine->FindNodeByIdL( aNodeId, aNamespace );
    if ( !node )
        {
        return NULL;
        }
    return &( node->PluginIfL() );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::FindNodeByIdL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf* TXnUiEnginePluginIf::FindNodeByIdL(
    const TDesC8& aNodeId,
    const TDesC8& aNamespace )
    {
    CXnNode* node = iUiEngine->FindNodeByIdL( aNodeId, aNamespace );
    if ( !node )
        {
        return NULL;
        }
    return &( node->PluginIfL() );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::FindNodeByClassL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CXnNodePluginIf > TXnUiEnginePluginIf::FindNodeByClassL(
    const TDesC& aClassId )
    {
    CXnPointerArray* array = iUiEngine->FindNodeByClassL( aClassId );
    CleanupStack::PushL( array );
    RPointerArray< CXnNodePluginIf > interfaceArray;
    // Push interfaceArray.Close() on cleanup stack to free the array buffer in
    // case of a leave
    CleanupClosePushL( interfaceArray );
    const TInt count = array->Container().Count();
    // Reserve the needed array buffer in one allocation to increase
    // excution speed and decrease heap fragmentation
    interfaceArray.ReserveL( count );
    for ( TInt i = 0; i < count; ++i )
        {
        CXnNode* node = static_cast< CXnNode* >( array->Container()[i] );
        // Append cannot fail because ReserveL call before this loop has
        // allocated the array buffer
        interfaceArray.Append( &( node->PluginIfL() ) );
        }
    CleanupStack::Pop( &interfaceArray );
    CleanupStack::PopAndDestroy( array );
    return interfaceArray;
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::RenderUIL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::RenderUIL( CXnNodePluginIf* aNode )
    {
    CXnNode* node = NULL;
    if ( aNode )
        {
        node = &( aNode->Node() );
        }
    iUiEngine->RenderUIL( node );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::Resources
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CArrayPtrSeg< CXnResource >& TXnUiEnginePluginIf::Resources()
    {
    return iUiEngine->Resources();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::FocusedNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf* TXnUiEnginePluginIf::FocusedNodeL()
    {
    CXnNodePluginIf* ret = NULL;
    if ( iUiEngine->FocusedNode() )
        {
        ret = &( iUiEngine->FocusedNode()->PluginIfL() );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::SetFocusedNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::SetFocusedNode(
    CXnNodePluginIf* aFocusedNode )
    {
    CXnNode* node( NULL );

    if ( aFocusedNode )
        {
        node = &( aFocusedNode->Node() );
        }

    TRAP_IGNORE
        (               
        node->SetStateWithoutNotificationL(
            XnPropertyNames::style::common::KFocus );        
        );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::RestorePreviousFocusedNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::RestorePreviousFocusedNode()
    {    
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::StringPool
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomStringPool& TXnUiEnginePluginIf::StringPool()
    {
    return *iUiEngine->ODT()->DomDocument().StringPool();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::VerticalPixelValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEnginePluginIf::VerticalPixelValueL(
    CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngine->VerticalPixelValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::HorizontalPixelValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEnginePluginIf::HorizontalPixelValueL(
    CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngine->HorizontalPixelValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::VerticalTwipValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEnginePluginIf::VerticalTwipValueL(
    CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngine->VerticalTwipValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::HorizontalTwipValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEnginePluginIf::HorizontalTwipValueL(
    CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngine->HorizontalTwipValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::ActivateViewL
// Activate a view
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::ActivateViewL( CXnNodePluginIf& aViewNode )
    {
    iUiEngine->ActivateViewL( aViewNode.Node() );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::ScreenDeviceSize
// Get the size of the current screen device
// -----------------------------------------------------------------------------
//
EXPORT_C TSize TXnUiEnginePluginIf::ScreenDeviceSize()
    {
    return iUiEngine->ScreenDeviceSize();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::IsDialogDisplaying
// Checks whether the dialog is displaying or not.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEnginePluginIf::IsDialogDisplaying()
    {
    return iUiEngine->IsDialogDisplaying();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::AddDirtyNodeL
// Sets the given node dirty.
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::AddDirtyNodeL( CXnNodePluginIf& aNode )
    {
    aNode.SetDirtyL();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::IsMenuDisplaying
// Checks whether the menu is displaying or not.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEnginePluginIf::IsMenuDisplaying()
    {
    return iUiEngine->IsMenuDisplaying();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::LayoutUIL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::LayoutUIL( CXnNodePluginIf* aNode )
    {
    CXnNode* node = NULL;
    if ( aNode )
        {
        node = &( aNode->Node() );
        }
    iUiEngine->LayoutUIL( node );
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::EditState
// Checks wheter editmode is active or not
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEnginePluginIf::EditMode()
    {
    return iUiEngine->EditMode()->EditState();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::WidgetsVisible
// Checks widgets' visibility state
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEnginePluginIf::WidgetsVisible() const
    {
    return iUiEngine->Editor()->WidgetsVisible();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::HitRegion
// -----------------------------------------------------------------------------
//
EXPORT_C CXnControlAdapter* TXnUiEnginePluginIf::HitRegion() const
    {
    return iUiEngine->HitTest().HitRegion();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::DeactivateFocusedNodeL
// Deactivates focused node
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEnginePluginIf::DeactivateFocusedNodeL()
    {    
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::ShowFocus
// Queries wheter focus is visible
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEnginePluginIf::ShowFocus()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::PluginNodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf& TXnUiEnginePluginIf::PluginNodeL(
    CXnNodePluginIf* aNode )
    {
    CXnViewManager* manager( iUiEngine->ViewManager() );
    
    CXnNode* node( NULL );
    
    if ( manager )
        {
        CXnPluginData& pluginData( 
            manager->ActiveViewData().Plugin( &aNode->Node() ) );
            
        CXnDomNode* domNode( pluginData.Owner() );
        
        if ( domNode )
            {
            node = domNode->LayoutNode();
            }
        }
    
    if( !node )
        {
        User::Leave( KErrNotFound );
        }
     
    return node->PluginIfL();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::SetFocusVisibleL
// Sets focus visibility of active view
// -----------------------------------------------------------------------------
//               
EXPORT_C void TXnUiEnginePluginIf::SetFocusVisibleL( TBool /*aVisible*/ )
    {    
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::FocusVisible
// Gets focus visibility of active view
// -----------------------------------------------------------------------------
//               
EXPORT_C TBool TXnUiEnginePluginIf::FocusVisible()
    {    
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::EnableSwipeL
// Sets swipe enabled or disabled
// -----------------------------------------------------------------------------
//               
EXPORT_C void TXnUiEnginePluginIf::EnableSwipeL( TBool /*aEnable*/ )
    {    
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::SwipeEnabledL
// queries whether swipe is enabled or not
// -----------------------------------------------------------------------------
//               
EXPORT_C TBool TXnUiEnginePluginIf::SwipeEnabledL()
    {    
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::DisableRenderUiLC
// 
// -----------------------------------------------------------------------------
//               
EXPORT_C void TXnUiEnginePluginIf::DisableRenderUiLC()
    {
    iUiEngine->DisableRenderUiLC();
    }

// -----------------------------------------------------------------------------
// TXnUiEnginePluginIf::EnablePartialTouchInput
// 
// -----------------------------------------------------------------------------
//               
EXPORT_C void TXnUiEnginePluginIf::EnablePartialTouchInput( 
    CXnNodePluginIf& aNode, TBool aEnable )
    {
    iUiEngine->EnablePartialTouchInput(aNode.Node(), aEnable);
    }



// End of file

