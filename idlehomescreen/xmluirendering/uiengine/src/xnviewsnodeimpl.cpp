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
* Description:  Implementation of views node
*
*/

// INCLUDES

// USER INCLUDES
#include    "xnviewsnodeimpl.h"
#include    "xncomponent.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnViewsNodeImpl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnViewsNodeImpl* CXnViewsNodeImpl::NewL( CXnType* aType )
    {
    CXnViewsNodeImpl* self = new( ELeave ) CXnViewsNodeImpl;
    CleanupStack::PushL( self );
    self->ConstructL( aType );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewsNodeImpl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnViewsNodeImpl::ConstructL( CXnType* aType )
    {
    CXnNodeImpl::ConstructL( aType );
    }

// -----------------------------------------------------------------------------
// CXnViewsNodeImpl::CXnViewsNodeImpl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnViewsNodeImpl::CXnViewsNodeImpl() 
    {
    }

// -----------------------------------------------------------------------------
// CXnViewsNodeImpl::~CXnViewsNodeImpl()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnViewsNodeImpl::~CXnViewsNodeImpl()
    {
    delete iComponent;
    }

// -----------------------------------------------------------------------------
// CXnViewsNodeImpl::Component()
// Returns the component associated with the views-node.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponent* CXnViewsNodeImpl::Component()
    {
    return iComponent;
    }

// -----------------------------------------------------------------------------
// CXnViewsNodeImpl::Component()
// Associates given component with the views-node.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnViewsNodeImpl::SetComponent( CXnComponent* aComponent )
    {
    if ( iComponent != NULL )
        {
        return;
        }
    iComponent = aComponent;
    }
