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
* Description:  Listens to changes in a Central Repository key 
*               KCurrentCellularDataUsage in Repository KCRUidCmManager.
*/

#include <commsdat.h>
#include <centralrepository.h>
#include <cmmanagerkeys.h>

#include "cenrepobserver.h"
#include "ai3.hrh"
using namespace AiXmlUiController;

#ifdef HS_NETWORK_MONITOR
#include <flogger.h>
_LIT( KLogFolder,"xnnetwork" );
_LIT( KLogDom, "networksettings.log" );

#define _LOG1( a ) RFileLogger::Write( \
    KLogFolder, KLogDom, EFileLoggingModeAppend, ( a ) );
#define _LOG2( a, b ) RFileLogger::WriteFormat( \
    KLogFolder, KLogDom, EFileLoggingModeAppend, ( a ), ( b ) )
#else
#define _LOG1
#define _LOG2
#endif

// -----------------------------------------------------------------------------
// CCenRepObserver::NewL
// -----------------------------------------------------------------------------
//
CCenRepObserver* CCenRepObserver::NewL( MCenRepObserver* aObserver )
    {
    CCenRepObserver* self = 
             CCenRepObserver::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::NewLC
// -----------------------------------------------------------------------------
//
CCenRepObserver* CCenRepObserver::NewLC( MCenRepObserver* aObserver )
    {
    CCenRepObserver* self = 
             new( ELeave ) CCenRepObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::~CCenRepObserver
// -----------------------------------------------------------------------------
//
CCenRepObserver::~CCenRepObserver()
    {
    // Cancel outstanding request, if exists
    Cancel();
    delete iRepository;
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::CCenRepObserver
// -----------------------------------------------------------------------------
//
CCenRepObserver::CCenRepObserver( MCenRepObserver* aObserver )
        :
        CActive( CActive::EPriorityStandard ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CCenRepObserver::ConstructL()
    {    
    TRAPD( err, iRepository = CRepository::NewL( KCRUidCmManager ) )
     
    if ( err == KErrNone )
        {
        CActiveScheduler::Add( this );
        RequestNotifications();
        }
    else
        {
        _LOG2( _L("FAILED to open KCRUidCmManager repository <%d>"), err ); 
        }
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::RequestNotifications
// -----------------------------------------------------------------------------
//
void CCenRepObserver::RequestNotifications()
    {
     _LOG1( _L("CCenRepObserver::RequestNotifications") );

    TInt err = iRepository->NotifyRequest( KCurrentCellularDataUsage, iStatus );

    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        _LOG2( _L("ERROR, iRepository->NotifyRequest() %d"), err ); 
        }
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::DoCancel
// -----------------------------------------------------------------------------
//
void CCenRepObserver::DoCancel()
    {
    iRepository->NotifyCancel( KCurrentCellularDataUsage );
    }

// -----------------------------------------------------------------------------
// CCenRepObserver::RunL
// -----------------------------------------------------------------------------
//
void CCenRepObserver::RunL()
    {
    _LOG2( _L("CCenRepObserver::RunL() %d"), iStatus.Int() );

    if ( iStatus.Int() < KErrNone )
        {
        iErrorCounter++;
        if ( iErrorCounter > KCenRepErrorRetryCount )
            {
            _LOG2( _L("Over %d consecutive errors, stopping notifications permanently"), 
                   KCenRepErrorRetryCount );    
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        
        TInt value( 0 );
        TInt err = iRepository->Get( KCurrentCellularDataUsage, value );
            
        if ( err == KErrNone )
            {
            _LOG2( _L("KCurrentCellularDataUsage: %d"), value );
             iObserver->CurrentCellularDataUsageChangedL( value );
            }
        else
            {
            _LOG2( _L("ERROR, iRepository->Get(KCurrentCellularDataUsage) %d"), err );
            }
        }

    RequestNotifications();
    }
// End-of-file
