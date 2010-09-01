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
* Description:  View node implementaton
*
*/

// User includes
#include "xnviewnodeimpl.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnViewNodeImpl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnViewNodeImpl* CXnViewNodeImpl::NewL( CXnType* aType )
    {
    CXnViewNodeImpl* self = new( ELeave ) CXnViewNodeImpl;
    CleanupStack::PushL( self );
    self->ConstructL( aType );
    CleanupStack::Pop();

    return self;    
    }

// -----------------------------------------------------------------------------
// CXnViewNodeImpl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnViewNodeImpl::ConstructL( CXnType* aType )
    {
    CXnNodeImpl::ConstructL( aType );
    }

// -----------------------------------------------------------------------------
// CXnViewNodeImpl::CXnViewNodeImpl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnViewNodeImpl::CXnViewNodeImpl()
    {
    }

// -----------------------------------------------------------------------------
// CXnViewNodeImpl::~CXnViewNodeImpl()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnViewNodeImpl::~CXnViewNodeImpl()
    {    
    }

// End of file
