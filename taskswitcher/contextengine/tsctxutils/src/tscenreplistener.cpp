/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cmn.h>
#include <centralrepository.h>

#include "tscenreplistener.h"
// -----------------------------------------------------------------------------
// CTsCenrepListener::NewL
// -----------------------------------------------------------------------------
EXPORT_C CTsCenrepListener* CTsCenrepListener::NewL(const TUid& aRep, TUint32 aKey,
                                   MTsCenrepChangeObserver& aObserver)
    {
    CTsCenrepListener* self = CTsCenrepListener::NewLC(aRep, aKey, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CTsCenrepListener* CTsCenrepListener::NewLC(const TUid& aRep, TUint32 aKey,
                                       MTsCenrepChangeObserver& aObserver)
    {
    CTsCenrepListener *self = new(ELeave) CTsCenrepListener(aKey, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aRep);
    return self;
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::CTsCenrepListener
// -----------------------------------------------------------------------------
CTsCenrepListener::CTsCenrepListener(TUint32 aKey, 
                                     MTsCenrepChangeObserver& aObserver)
: CActive( CActive::EPriorityStandard ), iObserver( aObserver ), iKey( aKey )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::ConstructL
// -----------------------------------------------------------------------------
void CTsCenrepListener::ConstructL(const TUid& aRep)
    {
    iRep = CRepository::NewL( aRep );
    iRep->NotifyRequest( iKey, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::~CTsCenrepListener
// -----------------------------------------------------------------------------
EXPORT_C CTsCenrepListener::~CTsCenrepListener()
    {
    Cancel();
    delete iRep;
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::DoCancel
// -----------------------------------------------------------------------------
void CTsCenrepListener::DoCancel()
    {
    iRep->NotifyCancelAll();
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::RunL
// -----------------------------------------------------------------------------
void CTsCenrepListener::RunL()
    {
    if ( iStatus.Int() != KErrCancel )
        {
        iObserver.CenrepChanged( iKey, Value() );
        iRep->NotifyRequest( iKey, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::RunError
// -----------------------------------------------------------------------------
TInt CTsCenrepListener::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTsCenrepListener::Value
// -----------------------------------------------------------------------------
EXPORT_C TInt CTsCenrepListener::Value()
    {
    TInt val = 0;
    iRep->Get( iKey, val );
    return val;
    }

// end of file
