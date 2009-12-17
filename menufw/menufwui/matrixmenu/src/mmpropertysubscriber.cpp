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


#include "mmpropertysubscriber.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CMmPropertySubscriber::CMmPropertySubscriber(MMmPropertyChangeObserver& aObserver) 
    : CActive( EPriorityStandard ),
    iPropertyChangeObserver(aObserver)
     {
     }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CMmPropertySubscriber* CMmPropertySubscriber::NewLC(
    const TUid aUid, 
    const TUint32 aKey,
    MMmPropertyChangeObserver& aObserver)
    {
    CMmPropertySubscriber* self = new ( ELeave ) CMmPropertySubscriber(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aUid,aKey);
    return self;
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CMmPropertySubscriber* CMmPropertySubscriber::NewL(
    const TUid aUid, 
    const TUint32 aKey,
    MMmPropertyChangeObserver& aObserver)
    {
    CMmPropertySubscriber* self = CMmPropertySubscriber::NewLC(aUid,aKey,aObserver);
    CleanupStack::Pop(); // self;
    return self;
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMmPropertySubscriber::ConstructL(
    const TUid aUid, 
    const TUint32 aKey)
    {
    User::LeaveIfError( iProperty.Attach( aUid, aKey ) );
    CActiveScheduler::Add( this); // Add to scheduler
    // initial subscription and process current property value
    RunL();
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CMmPropertySubscriber::~CMmPropertySubscriber()
    {
    Cancel(); // Cancel any request, if outstanding
    iProperty.Close();
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMmPropertySubscriber::DoCancel()
    {
    iProperty.Close();
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMmPropertySubscriber::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
     
    TInt intValue;
    if( iProperty.Get( intValue ) != KErrNotFound )
        {
        iPropertyChangeObserver.PropertyChangedL(intValue);
        }
    }

