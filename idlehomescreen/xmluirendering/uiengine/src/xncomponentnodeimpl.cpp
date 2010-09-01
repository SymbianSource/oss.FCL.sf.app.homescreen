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
* Description:  Xuikon component node implementation source
*
*/

// INCLUDES
#include "xncomponentnodeimpl.h"
#include "xncomponent.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponentNodeImpl* CXnComponentNodeImpl::NewL( CXnType* aType )
    {
    CXnComponentNodeImpl* self = new ( ELeave ) CXnComponentNodeImpl;

    CleanupStack::PushL( self );
    self->ConstructL( aType );
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponentNodeImpl::ConstructL( CXnType* aType )
    {
    CXnNodeImpl::ConstructL( aType );
    iComponentNeedsCreation = ETrue;
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::CXnComponentNodeImpl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponentNodeImpl::CXnComponentNodeImpl() 
    {
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::~CXnComponentNodeImpl()
// C++ default destructor.
// -----------------------------------------------------------------------------
// 
EXPORT_C CXnComponentNodeImpl::~CXnComponentNodeImpl()
    {
    delete iComponent;
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::Component
// Return the component
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponent* CXnComponentNodeImpl::Component()
    {
    return iComponent;
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::SetComponent
// Set the component
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponentNodeImpl::SetComponent( CXnComponent* aComponent )
    {
    iComponent = aComponent;
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::DoesComponentNeedCreation
// Check whether the component is to be created
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnComponentNodeImpl::DoesComponentNeedCreation() const
    {
    return iComponentNeedsCreation;
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::SetComponentNeedsCreation
// Set whether the component is to be created
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnComponentNodeImpl::SetComponentNeedsCreation( TBool aFlag )
    {
    iComponentNeedsCreation = aFlag;
    }

// -----------------------------------------------------------------------------
// CXnComponentNodeImpl::MakeInterfaceL
// Create a component interface according to the given type.
// -----------------------------------------------------------------------------
//
EXPORT_C XnComponentInterface::MXnComponentInterface* CXnComponentNodeImpl::MakeInterfaceL(
    const TDesC8& aType )
    {
    if ( !iComponent )
        {
        return NULL;
        }
    return iComponent->MakeInterfaceL( aType );
    }
