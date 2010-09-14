/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Menu item operation watcher. 
*
*/

#include <mcsmenuoperation.h>
#include "mcspluginwatcher.h"
#include "MCSExampleAppView.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSExampleWatcher* CMCSExampleWatcher::NewL(  )
	{
	CMCSExampleWatcher* self = new (ELeave) CMCSExampleWatcher(  );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSExampleWatcher::CMCSExampleWatcher(  )
	: CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSExampleWatcher::~CMCSExampleWatcher()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSExampleWatcher::ConstructL()
	{
	
	}

// ---------------------------------------------------------------------------
// Watch  Async
// ---------------------------------------------------------------------------
void CMCSExampleWatcher::Watch( CMenuOperation* aOperation, CMCSExampleAppView* aObserver )
    {
    iObserver = aObserver;
    __ASSERT_DEBUG( KRequestPending == iStatus.Int(), User::Invariant() );
    //__ASSERT_DEBUG( !iOperation, User::Invariant() ); 
    iOperation = aOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSExampleWatcher::RunL()
    {
    iObserver->ListAllItemsL();
    delete iOperation;
    iOperation = NULL;  
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSExampleWatcher::DoCancel()
    {
    delete iOperation;
    iOperation = NULL;
    }

TInt CMCSExampleWatcher::GetStatus()
    {
    return iStatus.Int();
    }

