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
 * Description: The source file of the CHsContentControlEComListener class. 
 *
 */

// System includes
#include <ecom/ecom.h>

// User includes
#include "hscontentcontrolecomlistener.h"
#include "hscontentcontrolecomobserver.h"

// ---------------------------------------------------------
// CHsContentControlEComListener::NewL
// ---------------------------------------------------------
//
CHsContentControlEComListener* CHsContentControlEComListener::NewL( 
        MHsContentControlEComObserver& aContentControlEComObserver )
    {
    CHsContentControlEComListener* self = 
            new ( ELeave ) CHsContentControlEComListener(
                    aContentControlEComObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// ---------------------------------------------------------
// CHsContentControlEComListener::~CHsContentControlEComListener
// ---------------------------------------------------------
//
CHsContentControlEComListener::~CHsContentControlEComListener()
    {
    Cancel(); // Cancel any request, if outstanding
    if( iEComSession )
        {
        // Close session to ECom server
        iEComSession->Close();
        }

    // Free up resources held by the EComSession.
    REComSession::FinalClose();
    }
 
// ---------------------------------------------------------
// CHsContentControlEComListener::CHsContentControlEComListener
// ---------------------------------------------------------
//
CHsContentControlEComListener::CHsContentControlEComListener( 
        MHsContentControlEComObserver& aContentControlEComObserver ) 
    : CActive( CActive::EPriorityStandard ),
    iHsContentControlEComObserver( aContentControlEComObserver )
    {
    }

// ---------------------------------------------------------
// CHsContentControlEComListener::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CHsContentControlEComListener::ConstructL()
    {
    // Connect to the ECOM session server.
    iEComSession = &REComSession::OpenL();
   
    // Queue the active object in the scheduler
    CActiveScheduler::Add( this );
        
    // Start the active object and listen for changes in the ECOM registry.
    iEComSession->NotifyOnChange( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CHsContentControlEComListener::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CHsContentControlEComListener::RunL()
    {
    if ( &iHsContentControlEComObserver &&
        KErrNone == iStatus.Int() )
        {
        // Notify observer
        iHsContentControlEComObserver.HandleEComChangeEvent();
        }

    // Keep listening changes
    iEComSession->NotifyOnChange( iStatus );

    // Restart the active Object
    SetActive();
    }

// ---------------------------------------------------------
// CHsContentControlEComListener::DoCancel
// ---------------------------------------------------------
//
void CHsContentControlEComListener::DoCancel()
    {
    // Cancel any outstanding requests.
    iEComSession->CancelNotifyOnChange( iStatus );
    }


// ---------------------------------------------------------
// CHsContentControlEComListener::RunError
// ---------------------------------------------------------
//
TInt CHsContentControlEComListener::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// End of file
