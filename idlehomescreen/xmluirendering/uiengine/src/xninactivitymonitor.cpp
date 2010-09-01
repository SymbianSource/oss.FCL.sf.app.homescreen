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
* Description:  Inactivity Monitor
*
*/

// System includes

// User includes
#include "xninactivitymonitor.h"

// Constants
const TInt KInactivityInterval( 6 ); 

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnInactivityMonitor::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnInactivityMonitor* CXnInactivityMonitor::NewL( 
    MXnInactivityObserver& aObserver )
    {
    CXnInactivityMonitor* self = CXnInactivityMonitor::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnInactivityMonitor* CXnInactivityMonitor::NewLC( 
    MXnInactivityObserver& aObserver )
    {
    CXnInactivityMonitor* self = 
        new ( ELeave ) CXnInactivityMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::~CXnInactivityMonitor()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnInactivityMonitor::~CXnInactivityMonitor()
    {  
    Stop();
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::CXnInactivityMonitor()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnInactivityMonitor::CXnInactivityMonitor( MXnInactivityObserver& aObserver )
    : CTimer( CActive::EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnInactivityMonitor::ConstructL()
    {   
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::RunL()
// 
// -----------------------------------------------------------------------------
//
void CXnInactivityMonitor::RunL()
    {
    iObserver.InactivityTimerExpired();
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::Start()
// 
// -----------------------------------------------------------------------------
//
void CXnInactivityMonitor::Start()
    {
    Cancel();
    
    Inactivity( KInactivityInterval );
    }

// -----------------------------------------------------------------------------
// CXnInactivityMonitor::Stop()
// 
// -----------------------------------------------------------------------------
//
void CXnInactivityMonitor::Stop()
    {
    Cancel();
    }

// End of file

