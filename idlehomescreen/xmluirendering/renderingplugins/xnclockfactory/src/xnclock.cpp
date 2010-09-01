/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation wrapper for CAknSkinnableClock
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>
#include "xncomponent.h"
#include "xnclock.h"
#include "xnclockadapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnClock::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnClock* CXnClock::NewL()
    {
	CXnClock* self = new( ELeave ) CXnClock;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnClock::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnClock::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnClock::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnClock::CXnClock()
    {
    }

// -----------------------------------------------------------------------------
// CXnClock::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnClock::~CXnClock()
    {
    }
    
// ---------------------------------------------------------
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnClock::MakeInterfaceL(const TDesC8& aType)
    {
    if( aType != XnClockInterface::KType )
        {
        return NULL;
        }
        
    XnClockInterface::MXnClockInterface* clockIf = static_cast< XnClockInterface::MXnClockInterface* >( this );
    
    return clockIf;
    }        

// End of file
