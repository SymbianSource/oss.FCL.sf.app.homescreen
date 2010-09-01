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
* Description:  Component factory interface
*
*/

// User includes
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xncomponentfactory.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include "xncomponentnodeimpl.h"
#include "xnviewnodeimpl.h"
#include "xncontroladapter.h"
#include "xnpanic.h"
#include "xntype.h"

// Constants
_LIT8( KToolTip, "tooltip" );
_LIT8( KView, "view" );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FindParentAdapter
// Find a parent control adapter up the node chain
// -----------------------------------------------------------------------------
//
static CXnControlAdapter* FindParentAdapter( CXnNode& aNode )
    {
    CXnNode* parent( aNode.Parent() );
    
    for( ; parent; parent = parent->Parent() )
        {
        CXnControlAdapter* control( parent->Control() );
        
        if( control )
            {
            return control;
            }
        }
    
    return NULL;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MXnComponentFactory::MXnComponentFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C MXnComponentFactory::MXnComponentFactory()
    {
    }

// -----------------------------------------------------------------------------
// MXnComponentFactory::CreateXnComponentL
// Creates a component
// -----------------------------------------------------------------------------
//
EXPORT_C MXnComponentFactory::TXnComponentFactoryResponse MXnComponentFactory::CreateXnComponentL(
    CXnNodePluginIf& aNode,
    CXnComponent*& aTargetComponent )
    {
    const TDesC8& type( aNode.Type()->Type() );
    
    MXnComponentFactory::TXnComponentFactoryResponse response =
        MakeXnComponentL( aNode, aTargetComponent );
    
    CleanupStack::PushL( aTargetComponent );
    
    if ( response != EXnFactoryResponseComponentConstructed )
        {
        CleanupStack::PopAndDestroy( aTargetComponent );
        
        return response;
        }
    
    CXnControlAdapter* parentAdapter( FindParentAdapter( aNode.Node() ) );
    CXnControlAdapter* adapter( MakeXnControlAdapterL( aNode, parentAdapter ) );
        
    aTargetComponent->SetNode( aNode );
            
    if ( type == KView )
        {
        aNode.ViewNodeImpl()->SetComponent( aTargetComponent );
        }
    else
        {
        aNode.ComponentNodeImpl()->SetComponent( aTargetComponent );
        }
    
    if ( parentAdapter )
        {
        parentAdapter->AppendChildL( *adapter, aNode.Node() );
        }
      
    aTargetComponent->SetControlAdapter( adapter );
    
    if ( !adapter )
        {
        CleanupStack::Pop( aTargetComponent );
        
        return response;
        }
    
    adapter->SetComponent( aTargetComponent );
    adapter->SetComponentsToInheritVisibility( ETrue );
        
    if ( type != KToolTip )
        {
        // Don't activate tooltip yet
        adapter->ActivateL();
        }
      
    CleanupStack::Pop( aTargetComponent );
    
    return response;
    }

// -----------------------------------------------------------------------------
// MXnComponentFactory::DoesNodeNeedComponentImplL
// Check whether a node needs a component node implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MXnComponentFactory::DoesNodeNeedComponentImplL( 
    const TDesC8& /*aType*/ )
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// MXnComponentFactory::MakeXnComponentL
// Factory method to create component
// -----------------------------------------------------------------------------
//
EXPORT_C MXnComponentFactory::TXnComponentFactoryResponse MXnComponentFactory::MakeXnComponentL(
    CXnNodePluginIf& /*aNode*/,
    CXnComponent*& /*aTargetComponent*/ )
    {
    return EXnFactoryResponseComponentNotSupported;
    }

// -----------------------------------------------------------------------------
// MXnComponentFactory::MakeXnComponentL
// Factory method to create control adapter
// -----------------------------------------------------------------------------
//
EXPORT_C CXnControlAdapter* MXnComponentFactory::MakeXnControlAdapterL(
    CXnNodePluginIf& /*aNode*/,
    CXnControlAdapter* /*aParent*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// Default destructor
// -----------------------------------------------------------------------------
//
EXPORT_C  MXnComponentFactory::~MXnComponentFactory()
    {
    }
