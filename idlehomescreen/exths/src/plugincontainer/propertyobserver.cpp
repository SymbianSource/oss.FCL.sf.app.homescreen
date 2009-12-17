/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
 *
*/


// INCLUDE FILES
#include "propertyobserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPropertyObserver::CPropertyObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPropertyObserver::CPropertyObserver(MPropertyChangeHandler& aHandler,
                                     const TUid& aCategory,
                                     const TUint& aKey)
: CActive( CActive::EPriorityStandard ),
  iCategory( aCategory ),
  iKey( aKey ),
  iHandler( aHandler )
{
}

// -----------------------------------------------------------------------------
// CPropertyObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPropertyObserver::ConstructL(void)
{
    CActiveScheduler::Add( this );

    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );

    // Start observing...
    Subscribe();

    // Notify the initial value if needed...
//    NotifyCurrentValue();
}

// -----------------------------------------------------------------------------
// CPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPropertyObserver* CPropertyObserver::NewL(MPropertyChangeHandler& aHandler,
                                           const TUid& aCategory,
                                           const TUint& aKey)
{
    CPropertyObserver* self = new( ELeave )CPropertyObserver( aHandler, aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// Destructor
CPropertyObserver::~CPropertyObserver()
{
    Deque();
    iProperty.Cancel();
    iProperty.Close();
}

// -----------------------------------------------------------------------------
// CPropertyObserver::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::RunL(void)
{
    if( iStatus.Int() == KErrNone )
    {
        Subscribe();
        NotifyCurrentValue();
    }
}

// -----------------------------------------------------------------------------
// CPropertyObserver::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::DoCancel(void)
{
    iProperty.Cancel();
}

// -----------------------------------------------------------------------------
// CPropertyObserver::Subscribe
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::Subscribe(void)
{
    iProperty.Subscribe( iStatus );
    SetActive();
}

// -----------------------------------------------------------------------------
// CPropertyObserver::NotifyCurrentValue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::NotifyCurrentValue(void)
{
    TInt value;

    if( iProperty.Get( iCategory, iKey, value ) == KErrNone )
    {
        iHandler.HandlePropertyChange( iCategory, iKey, value );
    }
}


// End of File
