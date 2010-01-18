/*
* ===========================================================================
*  Name        : HgCenrepListener.cpp
*  Part of     : Hg
*  Description : Active class to get notifications about changes to a cenrep key
*
*  Copyright © 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent   of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ===========================================================================
*/

#include <e32cmn.h>
#include <centralrepository.h>
#include "hgcenreplistener.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
EXPORT_C CHgCenrepListener* CHgCenrepListener::NewL(const TUid& aRep, TUint32 aKey,
                                   MHgCenrepChangeObserver& aObserver)
    {
    CHgCenrepListener* self = CHgCenrepListener::NewLC(aRep, aKey, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
EXPORT_C CHgCenrepListener* CHgCenrepListener::NewLC(const TUid& aRep, TUint32 aKey,
                                       MHgCenrepChangeObserver& aObserver)
    {
    CHgCenrepListener *self = new(ELeave) CHgCenrepListener(aKey, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aRep);
    return self;
    }

// -----------------------------------------------------------------------------
// CHgCenrepListener
// -----------------------------------------------------------------------------
CHgCenrepListener::CHgCenrepListener(TUint32 aKey, 
                                     MHgCenrepChangeObserver& aObserver)
: CActive( CActive::EPriorityStandard ), iObserver( aObserver ), iKey( aKey )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CHgCenrepListener::ConstructL(const TUid& aRep)
    {
    iRep = CRepository::NewL( aRep );
    iRep->NotifyRequest( iKey, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// ~CHgCenrepListener
// -----------------------------------------------------------------------------
EXPORT_C CHgCenrepListener::~CHgCenrepListener()
    {
    Cancel();
    delete iRep;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
void CHgCenrepListener::DoCancel()
    {
    iRep->NotifyCancelAll();
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
void CHgCenrepListener::RunL()
    {
    if ( iStatus.Int() != KErrCancel )
        {
        iObserver.CenrepChanged( iKey, Value() );
        iRep->NotifyRequest( iKey, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
TInt CHgCenrepListener::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Value
// -----------------------------------------------------------------------------
EXPORT_C TInt CHgCenrepListener::Value()
    {
    TInt val = 0;
    iRep->Get( iKey, val );
    return val;
    }

// end of file
