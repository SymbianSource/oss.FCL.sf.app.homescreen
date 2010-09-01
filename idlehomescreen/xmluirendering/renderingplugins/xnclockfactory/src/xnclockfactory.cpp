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
* Description:  Implementation for factory creating CXnClock-element
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xnclockfactory.h"
#include "xnclockadapter.h"
#include "xntype.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include "xndomnode.h"

#include "xnclock.h"

_LIT8( KXnClock, "clock" );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CXnClockFactory::MakeXnComponentL
// Creates the Xuikon component for clock control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnClockFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
        								)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    
    // Check that the given type of a control is parent (or ancestor) of this control
    if( aNode.Type()->Type() == KXnClock )
        {
    	CXnClock* container = CXnClock::NewL();
    	
        aTargetComponent = static_cast< CXnComponent* >( container );
        
  	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;	    	    
	    }
	    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnClockFactory::MakeXnControlAdapterL
// Creates the control adapter for clock control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnClockFactory::MakeXnControlAdapterL(
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* aParent)
    {
    return CXnClockAdapter::NewL( aParent, aNode );
    }

// -----------------------------------------------------------------------------
// CXnClockFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnClockFactory* CXnClockFactory::NewL()
    {
    CXnClockFactory* self = new( ELeave ) CXnClockFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnClockFactory::CXnClockFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnClockFactory::CXnClockFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFactory::~CXnClockFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnClockFactory::~CXnClockFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnClockFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY( 0x2001CB52, CXnClockFactory::NewL ) 
#else
    { { 0x2001CB52 }, CXnClockFactory::NewL }
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// End of file
