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
* Description:   Class to listen SWI operations (install/uninstall/restore).
*
*/
#include <swi/swiutils.h>
#include <sacls.h> // KSWIUidsCurrentlyBeingProcessed

#include "hscontentcontrolswilistener.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CHsContentControlSwiListener::NewL()
// ---------------------------------------------------------------------------
//
CHsContentControlSwiListener* CHsContentControlSwiListener::NewL( 
            MHsContentControlSwiObserver& aObs )
    {
    CHsContentControlSwiListener* self = 
                new( ELeave ) CHsContentControlSwiListener( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CHsContentControlSwiListener::~CHsContentControlSwiListener()
// ---------------------------------------------------------------------------
//
CHsContentControlSwiListener::~CHsContentControlSwiListener()
    {
    Cancel();
    iSwInstallKey.Close();
    }

// ---------------------------------------------------------------------------
// CHsContentControlSwiListener::DoCancel()
// ---------------------------------------------------------------------------
//
void CHsContentControlSwiListener::DoCancel()
    {
    if ( IsActive() )
        {
        iSwInstallKey.Cancel();
        }    
    }

// ---------------------------------------------------------------------------
// CHsContentControlSwiListener::RunL()
// ---------------------------------------------------------------------------
//
void CHsContentControlSwiListener::RunL()
    {    
    RArray<TUid> uidList;
    
    // get list of uids being processed.
    if ( Swi::GetAllUids( uidList ) == KErrNone )
        {
        iObs.HandleSwiEvent( uidList );
        uidList.Reset();
        }

    // close array
    uidList.Close();

    // keep monitoring
    iSwInstallKey.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CHsContentControlSwiListener::CHsContentControlSwiListener()
// ---------------------------------------------------------------------------
//
CHsContentControlSwiListener::CHsContentControlSwiListener( 
    MHsContentControlSwiObserver& aObs )
    : CActive( CActive::EPriorityStandard ),
    iObs( aObs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CHsContentControlSwiListener::ConstructL()
// ---------------------------------------------------------------------------
//
void CHsContentControlSwiListener::ConstructL()
    {
    if ( KErrNone == iSwInstallKey.Attach( 
        KUidSystemCategory, KSWIUidsCurrentlyBeingProcessed ) )
        {
        iSwInstallKey.Subscribe( iStatus );
        SetActive();
        }
    }

