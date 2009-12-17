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
* Description:  Data class to hold widget info
*
*/

// INCLUDES
#include <e32base.h>

#include "xncontroladapter.h"
#include "xnhittest.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnHitTest::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnHitTest* CXnHitTest::NewL()
    {
    CXnHitTest* self = CXnHitTest::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnHitTest::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnHitTest* CXnHitTest::NewLC()
    {
    CXnHitTest* self = new ( ELeave ) CXnHitTest;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnHitTest::CXnHitTest()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnHitTest::CXnHitTest()
    {
    }

// -----------------------------------------------------------------------------
// CXnHitTest::~CXnHitTest()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnHitTest::~CXnHitTest()
    {
    CCoeEnv::Static()->RemoveMessageMonitorObserver( *this );    
    iHitRegion.Reset();
    }

// -----------------------------------------------------------------------------
// CXnHitTest::ConstructL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CXnHitTest::ConstructL()
    {
    CCoeEnv::Static()->AddMessageMonitorObserverL( *this );    
    }

// -----------------------------------------------------------------------------
// CXnHitTest::MonitorWsMessage()
// -----------------------------------------------------------------------------
//
void CXnHitTest::MonitorWsMessage( const TWsEvent& aEvent )
    {
    TInt type( aEvent.Type() );
    
    if ( type == EEventPointer )
        {
        iPointerEvent = *aEvent.Pointer();
        }
    }

// -----------------------------------------------------------------------------
// CXnHitTest::AddControl()
// -----------------------------------------------------------------------------
//
void CXnHitTest::AddControl( CXnControlAdapter* aControl )
    {
    if ( iHitRegion.Find( aControl ) == KErrNotFound )
        {
        iHitRegion.Append( aControl );
        }
    }

// -----------------------------------------------------------------------------
// CXnHitTest::RemoveControl()
// -----------------------------------------------------------------------------
//
void CXnHitTest::RemoveControl( CXnControlAdapter* aControl )
    {
    TInt index( iHitRegion.Find( aControl ) );

    if ( index != KErrNotFound )
        {
        iHitRegion.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CXnHitTest::SetFlags()
// -----------------------------------------------------------------------------
//
void CXnHitTest::SetFlags( const TInt aFlags )
    {
    iFlags |= aFlags;
    }

// -----------------------------------------------------------------------------
// CXnHitTest::ClearFlags()
// -----------------------------------------------------------------------------
//
void CXnHitTest::ClearFlags( const TInt aFlags )
    {
    iFlags &= ~aFlags;
    }

// -----------------------------------------------------------------------------
// CXnHitTest::Flags()
// -----------------------------------------------------------------------------
//
TInt CXnHitTest::Flags() const
    {
    return iFlags;
    }

// -----------------------------------------------------------------------------
// CXnHitTest::Flush()
// -----------------------------------------------------------------------------
//
void CXnHitTest::Flush()
    {
    iFlags = 0;

    iHitRegion.Reset();
    }

// -----------------------------------------------------------------------------
// CXnHitTest::HitRegionCount()
// -----------------------------------------------------------------------------
//
TInt CXnHitTest::HitRegionCount() const
    {
    return iHitRegion.Count();
    }

// -----------------------------------------------------------------------------
// CXnHitTest::HitRegion()
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnHitTest::HitRegion() const
    {
    TInt count( HitRegionCount() );

    if ( count > 0 )
        {
        return iHitRegion[count - 1];
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnHitTest::PointerEvent()
// -----------------------------------------------------------------------------
//
const TPointerEvent& CXnHitTest::PointerEvent() const
    {
    return iPointerEvent;
    }

// End of file
