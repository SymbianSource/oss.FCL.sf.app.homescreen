/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface of the Ecom observer
*
*/

#include <ecom/ecom.h>
#include "mmecomobserver.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMEcomObserver::CMMEcomObserver() :
    CActive(EPriorityStandard) // Standard priority
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMEcomObserver* CMMEcomObserver::NewLC( 
                                    MHNEcomInstallNotifierCallback* aCallback )
    {
    CMMEcomObserver* self = new ( ELeave ) CMMEcomObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aCallback );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMEcomObserver* CMMEcomObserver::NewL( 
                                    MHNEcomInstallNotifierCallback* aCallback )
    {
    CMMEcomObserver* self = CMMEcomObserver::NewLC( aCallback );
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMMEcomObserver::ConstructL( MHNEcomInstallNotifierCallback* aCallback )
    {
    iCallback = aCallback;
    iSession = &REComSession::OpenL();
    iSession->NotifyOnChange( iStatus );
    CActiveScheduler::Add( this );
    SetActive();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMEcomObserver::~CMMEcomObserver()
    {
    Cancel(); // Cancel any request, if outstanding
    if( iSession )
        {
        iSession->Close();
        }
    REComSession::FinalClose( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMMEcomObserver::DoCancel()
    {
    iSession->CancelNotifyOnChange( iStatus );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMMEcomObserver::RunL()
    {
    iCallback->EcomChangedL();
    iSession->NotifyOnChange( iStatus );
    SetActive(); // Tell scheduler a request is active
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMMEcomObserver::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
