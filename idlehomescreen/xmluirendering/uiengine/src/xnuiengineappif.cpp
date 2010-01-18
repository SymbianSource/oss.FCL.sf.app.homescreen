/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Layout calculation and UI rendering mechanism implementations.
*
*/

// User includes
#include "xnuiengineappif.h"
#include "xnuiengine.h"
#include "xnnode.h"
#include "xnodt.h"
#include "xndomdocument.h"
#include "xnpointerarray.h"
#include "xneditor.h"
#include "xnnodeappif.h"
#include "xnviewdata.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::TXnUiEngineAppIf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TXnUiEngineAppIf::TXnUiEngineAppIf( CXnUiEngine& aUiEngine )
    {
    iUiEngine = &aUiEngine;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::RootNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf& TXnUiEngineAppIf::RootNodeL()
    {
    return iUiEngine->RootNode()->AppIfL();
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::FindNodeByIdL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf* TXnUiEngineAppIf::FindNodeByIdL(
    const TDesC& aNodeId,
    const TDesC& aNamespace )
    {
    CXnNode* node( iUiEngine->FindNodeByIdL( aNodeId, aNamespace ) );

    if ( node )
        {
        return &( node->AppIfL() );
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::FindNodeByIdL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf* TXnUiEngineAppIf::FindNodeByIdL(
    const TDesC8& aNodeId,
    const TDesC8& aNamespace )
    {
    CXnNode* node( iUiEngine->FindNodeByIdL( aNodeId, aNamespace ) );

    if ( node )
        {
        return &( node->AppIfL() );
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::FindNodeByClassL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CXnNodeAppIf > TXnUiEngineAppIf::FindNodeByClassL(
    const TDesC& aClassId, const TDesC& aNamespace )
    {
    CXnPointerArray* array = iUiEngine->FindNodeByClassL(
        aClassId, aNamespace );
    CleanupStack::PushL( array );
    RPointerArray< CXnNodeAppIf > interfaceArray;
    CleanupClosePushL( interfaceArray );
    const TInt count = array->Container().Count();
    interfaceArray.ReserveL( count );
    for ( TInt i = 0; i < count; ++i )
        {
        CXnNode* node = static_cast< CXnNode* >( array->Container()[i] );
        // Append cannot fail because ReserveL call before this loop has
        // allocated the array buffer
        interfaceArray.Append( &( node->AppIfL() ) );
        }
    CleanupStack::Pop( &interfaceArray );
    CleanupStack::PopAndDestroy( array );
    return interfaceArray;
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FindNodeByClassL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CXnNodeAppIf > TXnUiEngineAppIf::FindNodeByClassL(
    const TDesC8& aClassId, const TDesC8& aNamespace )
    {
    CXnPointerArray* array = iUiEngine->FindNodeByClassL(
        aClassId, aNamespace );
    CleanupStack::PushL( array );
    RPointerArray< CXnNodeAppIf > interfaceArray;
    CleanupClosePushL( interfaceArray );
    const TInt count = array->Container().Count();
    interfaceArray.ReserveL( count );
    for ( TInt i = 0; i < count; ++i )
        {
        CXnNode* node = static_cast< CXnNode* >( array->Container()[i] );
        // Append cannot fail because ReserveL call before this loop has
        // allocated the array buffer
        interfaceArray.Append( &( node->AppIfL() ) );
        }
    CleanupStack::Pop( &interfaceArray );
    CleanupStack::PopAndDestroy( array );
    return interfaceArray;
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FindNodeByClassL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CXnNodeAppIf > TXnUiEngineAppIf::FindContentSourceNodesL(
    const TDesC8& aNamespace )
    {
    RPointerArray< CXnNodeAppIf > interfaceArray;
    CleanupClosePushL( interfaceArray );

    CXnViewManager* manager( iUiEngine->ViewManager() );

    CXnPluginData* data( manager->ActiveViewData().Plugin( aNamespace ) );

    if ( data )
        {
        RPointerArray< CXnNode > nodes;
        CleanupClosePushL( nodes );

        data->ContentSourceNodesL( nodes );

        for ( TInt i = 0; i < nodes.Count(); i++ )
            {
            interfaceArray.AppendL( &nodes[i]->AppIfL() );
            }

        CleanupStack::PopAndDestroy( &nodes );
        }

    CleanupStack::Pop( &interfaceArray );

    return interfaceArray;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::RenderUIL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEngineAppIf::RenderUIL( CXnNodeAppIf* aNode )
    {
    CXnNode* node = NULL;
    if ( aNode != NULL )
        {
        node = &( aNode->Node() );
        }
    iUiEngine->RenderUIL( node );
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::StringPool
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomStringPool& TXnUiEngineAppIf::StringPool()
    {
    return *iUiEngine->ODT()->DomDocument().StringPool();
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::ActivateViewL
// Activate a view
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEngineAppIf::ActivateViewL( CXnNodeAppIf& aViewNode )
    {
    iUiEngine->ActivateViewL( aViewNode.Node() );
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::ActiveView
// Return the active view
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf* TXnUiEngineAppIf::ActiveView()
    {
    CXnNodeAppIf* ret( NULL );
    TRAP_IGNORE( ret = &( iUiEngine->ActiveView()->AppIfL() ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::RefreshMenuL
// Refresh current menu
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEngineAppIf::RefreshMenuL()
    {
    iUiEngine->RefreshMenuL();
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::IsMenuDisplaying
// Checks whether the menu is displaying or not.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEngineAppIf::IsMenuDisplaying()
    {
    return iUiEngine->IsMenuDisplaying();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEngineAppIf::GetPluginNodeArrayL(
    RPointerArray< CXnNodeAppIf >& aArray )
    {
    aArray.Reset();
    RPointerArray< CXnNode >* plugins = iUiEngine->Plugins();
    if ( !plugins )
        {
        return 0;
        }
    TInt count( plugins->Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        CXnNode* tmp = ( *plugins )[i];
        aArray.AppendL( &( tmp->AppIfL() ) );
        }
    return count;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::FocusedNode
// Gets the currently focused node
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf* TXnUiEngineAppIf::FocusedNode()
    {
    CXnNode* focused( iUiEngine->FocusedNode() );
    CXnNodeAppIf* ret( NULL );

    if ( focused )
        {
        TRAP_IGNORE( ret = &( focused->AppIfL() ) );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TXnUiEngineAppIf::IsEditMode
// Returns ETrue if the current mode is edit, otherwise EFalse.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEngineAppIf::IsEditMode()
    {
    return ( iUiEngine->IsEditMode() );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C void TXnUiEngineAppIf::SetUiEngine( CXnUiEngine* aUiEngine )
    {
    iUiEngine = aUiEngine;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TXnUiEngineAppIf::GetThemeResource(
    const TDesC& aPath, RFile& aFile )
    {
    return iUiEngine->GetThemeResource( aPath, aFile );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXnUiEngineAppIf::WidgetsVisible() const
    {
    return iUiEngine->Editor()->WidgetsVisible();
    }
