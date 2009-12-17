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
* Description: Subscribes properties from P&S
*
*/

#include "mmnomemory.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmNoMemory::CMmNoMemory() :
    CActive( EPriorityIdle )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmNoMemory* CMmNoMemory::NewLC()
    {
    CMmNoMemory* self = new (ELeave) CMmNoMemory();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmNoMemory* CMmNoMemory::NewL()
    {
    CMmNoMemory* self = CMmNoMemory::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmNoMemory::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmNoMemory::Start()
    {
    Cancel();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmNoMemory::~CMmNoMemory()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmNoMemory::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmNoMemory::RunL()
    {
    User::Leave( KErrNoMemory );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmNoMemory::RunError(TInt aError)
    {
    return aError;
    }
