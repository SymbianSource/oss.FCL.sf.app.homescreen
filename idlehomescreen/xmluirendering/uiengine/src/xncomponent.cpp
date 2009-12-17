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
* Description:  Xuikon component source
*
*/

// INCLUDES
#include "xncomponent.h"
#include "xncontroladapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnComponent::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponent* CXnComponent::NewL()
    {
    CXnComponent* self = new ( ELeave ) CXnComponent;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnComponent::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponent::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnComponent::CXnComponent()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponent::CXnComponent()
    {
    }

// -----------------------------------------------------------------------------
// CXnComponent::~CXnComponent()
// C++ default destructor.
// -----------------------------------------------------------------------------
// 
EXPORT_C CXnComponent::~CXnComponent()
    {
    delete iAdapter;
    }

// -----------------------------------------------------------------------------
// CXnComponent::SetControlAdapter()
// Sets component control adapter.
// -----------------------------------------------------------------------------
// 
EXPORT_C void CXnComponent::SetControlAdapter( CXnControlAdapter* aAdapter )
    {
    if ( iAdapter )
        {
        return;
        }
    iAdapter = aAdapter;
    if ( iAdapter )
        {
        iAdapter->SetComponent( this );
        }
    }

// -----------------------------------------------------------------------------
// CXnComponent::ControlAdapter()
// Returns pointer to component control adapter.
// -----------------------------------------------------------------------------
// 
EXPORT_C CXnControlAdapter* CXnComponent::ControlAdapter()
    {
    return iAdapter;
    }

// -----------------------------------------------------------------------------
// CXnComponent::Node
// Returns node associated with the component
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf* CXnComponent::Node()
    {
    return iNode;
    }

// -----------------------------------------------------------------------------
// CXnComponent::SetNode
// Sets node associated with the component
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponent::SetNode( CXnNodePluginIf& aNode )
    {
    iNode = &aNode;
    }

// -----------------------------------------------------------------------------
// CXnComponent::EnterPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponent::EnterPowerSaveModeL()
    {
    if ( iAdapter )
        {
        iAdapter->EnterPowerSaveModeL();
        }
    }

// -----------------------------------------------------------------------------
// CXnComponent::ExitPowerSaveModeL
// Exit power save mode
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponent::ExitPowerSaveModeL()
    {
    if ( iAdapter )
        {
        iAdapter->ExitPowerSaveModeL();
        }
    }

// -----------------------------------------------------------------------------
// CXnComponent::MakeInterfaceL
// Create a component interface according to the given type.
// -----------------------------------------------------------------------------
//
EXPORT_C XnComponentInterface::MXnComponentInterface* CXnComponent::MakeInterfaceL(
    const TDesC8& /*aType*/ )
    {
    return NULL;
    }
