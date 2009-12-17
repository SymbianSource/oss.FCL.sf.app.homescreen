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
* Description:  Node of the layout tree
*
*/

// INCLUDES
#include "xnnode.h"
#include "xnnodeappif.h"
#include "xntype.h"
#include "xnuiengine.h"
#include "xndomnode.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNodeAppIf::CXnNodeAppIf()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNodeAppIf::CXnNodeAppIf( CXnNode& aNode )
    {
    iNode = &aNode;
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::Type()
// Returns control type.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnType* CXnNodeAppIf::Type()
    {
    return iNode->Type();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::SetPCDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::SetPCDataL( const TDesC8& aData )
    {
    iNode->SetPCDataL( aData );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::GetPCData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnNodeAppIf::GetPCData() const
    {
    return iNode->GetPCData();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::SetPropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::SetPropertyL( CXnProperty* aProperty )
    {
    iNode->SetPropertyL( aProperty );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::SetPropertyArrayL
// Set a property array.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::SetPropertyArrayL(
    RPointerArray< CXnProperty >* aPropertyArray )
    {
    iNode->SetPropertyArrayL( aPropertyArray );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::GetProperty
// Gets a property.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodeAppIf::GetPropertyL( CXnProperty& /*aProperty*/ ) const
    {
    return NULL;//return iNode->GetPropertyL(aProperty);
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::GetProperty
// Gets a property.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodeAppIf::GetPropertyL( const TDesC8& aKey ) const
    {
    return iNode->GetPropertyL( aKey );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::SetStateL
// Set a state
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::SetStateL( const TDesC8& aState )
    {
    iNode->SetStateL( aState );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::IsStateSet
// Check whether a state is set or not
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnNodeAppIf::IsStateSet( const TDesC8& aState )
    {
    return iNode->IsStateSet( aState );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::UnsetState
// Unset a pseudoclass
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::UnsetStateL( const TDesC8& aState )
    {
    iNode->UnsetStateL( aState );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::Rect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodeAppIf::Rect()
    {
    return iNode->Rect();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::BorderRect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodeAppIf::BorderRect()
    {
    return iNode->BorderRect();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::MarginRect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodeAppIf::MarginRect()
    {
    return iNode->MarginRect();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::PaddingRect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodeAppIf::PaddingRect()
    {
    return iNode->PaddingRect();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::UiEngine
// -----------------------------------------------------------------------------
//
EXPORT_C TXnUiEngineAppIf* CXnNodeAppIf::UiEngineL()
    {
    return &(iNode->UiEngine()->AppIfL());
    }

// -----------------------------------------------------------------------------
// Get internal node
// -----------------------------------------------------------------------------
//
CXnNode& CXnNodeAppIf::Node()
    {
    return *iNode;
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::MakeInterfaceL
// Create a component interface according to the given type.
// -----------------------------------------------------------------------------
//
EXPORT_C XnComponentInterface::MXnComponentInterface* CXnNodeAppIf::MakeInterfaceL(
    const TDesC8& aType )
    {
    return iNode->MakeInterfaceL( aType );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::Children
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CXnNodeAppIf > CXnNodeAppIf::ChildrenL()
    {
    RPointerArray< CXnNode >& nodes = iNode->Children();
    RPointerArray< CXnNodeAppIf > appNodes;
    CleanupClosePushL( appNodes );
    TInt count = nodes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        appNodes.AppendL( &nodes[i]->AppIfL() );
        }
    CleanupStack::Pop( &appNodes );
    // Compiler will generate bitwise copy ctor, thus
    // caller of this function must call reset or close for copied RPointerArrays,
    // which is very nasty thing to do...
    return appNodes;
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::ParentL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf* CXnNodeAppIf::ParentL() const
    {
    CXnNode* parent = iNode->Parent();
    if ( !parent )
        {
        return NULL;
        }
    return &( parent->AppIfL() );
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::InternalDomNodeType
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnNodeAppIf::InternalDomNodeType() const
    {
    CXnDomNode* node = iNode->DomNode();
    if ( node )
        {
        return node->Name();
        }
    return KNullDesC8;
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::ShowTooltipsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::ShowTooltipsL()
    {
    iNode->ShowTooltipsL();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::HideTooltipsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodeAppIf::HideTooltipsL()
    {
    iNode->HideTooltipsL();
    }

// -----------------------------------------------------------------------------
// CXnNodeAppIf::Namespace
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnNodeAppIf::Namespace()
    {
    CXnDomNode* node = iNode->DomNode();
    if ( node )
        {
        return node->Namespace();
        }
    return KNullDesC8;
    }
