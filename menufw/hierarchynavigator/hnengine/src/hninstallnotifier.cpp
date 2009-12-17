/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Sis package installation event listener
*
*/
 

#include <sacls.h>
#include "hninstallnotifier.h"

// ============================ MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CHnMulModelInstallNotifier::CHnMulModelInstallNotifier
// C++ default constructor
// -----------------------------------------------------------------------------
//
CHnInstallNotifier::CHnInstallNotifier( 
                                    MHnInstallNotifierCallback* aCallback, 
                                    TUid aCategory, 
                                    TUint aKey ) : CActive( EPriorityNormal )
    {    
    CActiveScheduler::Add( this );
    
    iCallback = aCallback;
    iCategory = aCategory;
    iKey = aKey;
    // Prepare automatically
    TInt err = iProperty.Attach( iCategory, iKey );
    SetActive();
    iStatus = KRequestPending;
    iProperty.Subscribe( iStatus );
    }

// -----------------------------------------------------------------------------
// CHnMulModelInstallNotifier::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CHnInstallNotifier::ConstructL()
    {

    }

// ---------------------------------------------------------------------------
// CHnMulModelInstallNotifier::NewL
// ---------------------------------------------------------------------------
//
CHnInstallNotifier* CHnInstallNotifier::
                              NewL( MHnInstallNotifierCallback* aCallback,
                                    TUid aCategory, 
                                    TUint aKey  )
    {
    CHnInstallNotifier* self = 
                new (ELeave) CHnInstallNotifier( aCallback,
                                                         aCategory,
                                                         aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// CHnMulModelInstallNotifier::~CHnMulModelInstallNotifier
// ---------------------------------------------------------------------------
//
CHnInstallNotifier::~CHnInstallNotifier()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CHnMulModelInstallNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CHnInstallNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CHnMulModelInstallNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CHnInstallNotifier::RunError( TInt /*aError*/ )
    {    
    // No need to do anything      
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CHnMulModelInstallNotifier::RunL
// ---------------------------------------------------------------------------
//
void CHnInstallNotifier::RunL()
    {
    // Re-issue request before notifying
    SetActive();
    iStatus = KRequestPending;
    iProperty.Subscribe( iStatus );
    
    TInt status;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory, 
                                      KSAUidSoftwareInstallKeyValue,status ));

    iCallback->InstallChangeL( status );
    }
// End of File
