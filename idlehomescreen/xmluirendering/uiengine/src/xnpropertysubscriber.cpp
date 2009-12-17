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
* Description:  Subscribes properties from P&S
*
*/

#include "xnpropertysubscriber.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CXnPropertySubscriber::CXnPropertySubscriber(
    MXnPropertyChangeObserver& aObserver,
    const TUint32 aKey )
    : CActive( EPriorityStandard ),
    iPropertyChangeObserver( aObserver ),
    iKey( aKey )
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CXnPropertySubscriber* CXnPropertySubscriber::NewLC(
    const TUid aUid,
    const TUint32 aKey,
    MXnPropertyChangeObserver& aObserver )
    {
    CXnPropertySubscriber* self =
        new ( ELeave ) CXnPropertySubscriber( aObserver, aKey );
    CleanupStack::PushL( self );
    self->ConstructL( aUid, aKey );
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CXnPropertySubscriber* CXnPropertySubscriber::NewL(
    const TUid aUid,
    const TUint32 aKey,
    MXnPropertyChangeObserver& aObserver )
    {
    CXnPropertySubscriber* self = CXnPropertySubscriber::NewLC(
        aUid, aKey, aObserver );
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnPropertySubscriber::ConstructL(
    const TUid aUid,
    const TUint32 aKey )
    {
    User::LeaveIfError( iProperty.Attach( aUid, aKey ) );
    CActiveScheduler::Add( this ); // Add to scheduler
    // initial subscription and process current property value
    RunL();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CXnPropertySubscriber::~CXnPropertySubscriber()
    {
    Cancel(); // Cancel any request, if outstanding
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnPropertySubscriber::DoCancel()
    {
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnPropertySubscriber::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    TInt intValue;
    if ( iProperty.Get( intValue ) != KErrNotFound )
        {
        iPropertyChangeObserver.PropertyChangedL( iKey, intValue );
        }
    }
