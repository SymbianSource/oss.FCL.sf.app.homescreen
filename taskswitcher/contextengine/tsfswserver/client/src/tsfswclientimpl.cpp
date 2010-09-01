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
* Description:  Client API, private implementation
 *
*/


#include "tsfswclientimpl.h"
#include "tsfswclientobserver.h"
#include "tsfswcommon.h"
#include <s32mem.h>

#include "clientlogging.h"

// --------------------------------------------------------------------------
// StartServer
// --------------------------------------------------------------------------
//   
static TInt StartServer()
    {
    RProcess server;
    const TUidType uid( KNullUid, KNullUid, KTsFswServerUid );
    TInt err = server.Create( KTsFswServerImg, KNullDesC, uid );
    if ( err != KErrNone )
        {
        return err;
        }
    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 );
        }
    else
        {
        server.Resume();
        }
    User::WaitForRequest( stat );
    err = server.ExitType() == EExitPanic ? KErrGeneral : stat.Int();
    server.Close();
    return err;
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::Connect
// --------------------------------------------------------------------------
//
TInt CTsFswClientImpl::RTsFswClient::Connect()
    {
    const TInt KAsyncMessageSlots = 4;
    const TInt KMaxRetry = 4;
    
    TInt retry = KMaxRetry;
    for ( ; ; )
        {
        TInt err = CreateSession( KTsFswServerName, TVersion( 0, 0, 0 ), KAsyncMessageSlots );
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            return err;
            }
        if ( !--retry )
            {
            return err;
            }
        err = StartServer();
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            return err;
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::Subscribe
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::RTsFswClient::Subscribe( TRequestStatus& aStatus )
    {
    SendReceive( ETsFswSubscribe, aStatus );
    }
    
// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::CancelSubscribe
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::RTsFswClient::CancelSubscribe()
    {
    SendReceive( ETsFswCancel );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::GetContentL
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::RTsFswClient::GetContentL( RTsFswArray& aDst )
    {
    TSLOG_CONTEXT( GetContentL, TSLOG_LOCAL );
    TSLOG_IN();

    for ( ; ; )
        {
        TPckgBuf<TInt> bufSize;
        User::LeaveIfError( SendReceive( ETsFswGetBufferSize,
            TIpcArgs( &bufSize ) ) );
        HBufC8* buf = HBufC8::NewLC( bufSize() );
        TPtr8 p( buf->Des() );
        TInt err = SendReceive( ETsFswGetBuffer,
            TIpcArgs( &p, bufSize() ) );
        if ( err == KErrNone )
            {
            RDesReadStream strm( p );
            CleanupClosePushL( strm );
            CTsFswEntry::InternalizeArrayL( strm, aDst );
            CleanupStack::PopAndDestroy( &strm );
            }
        else if ( err != KErrArgument )
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy( buf );
        if ( err == KErrNone )
            {
            break;
            }
        // If result was KErrArgument then the size received from GetBufferSize is
        // not valid anymore so restart the whole procedure.
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::CloseApp
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::RTsFswClient::CloseApp( TInt aWgId )
    {
    SendReceive( ETsFswCloseApp, TIpcArgs( aWgId ) );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::SwitchToApp
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::RTsFswClient::SwitchToApp( TInt aWgId )
    {
    SendReceive( ETsFswSwitchToApp, TIpcArgs( aWgId ) );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RTsFswClient::ForegroundAppUid
// --------------------------------------------------------------------------
//   
TUid CTsFswClientImpl::RTsFswClient::ForegroundAppUid( TInt aType )
    {
    TUid result = KNullUid;
    TPckgBuf<TInt> uidBuf;
    if ( SendReceive( ETsFswForegroundAppUid,
            TIpcArgs( &uidBuf, &aType ) ) == KErrNone )
        {
        result = TUid::Uid( uidBuf() );
        }
    return result;
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::NewL
// --------------------------------------------------------------------------
//   
CTsFswClientImpl* CTsFswClientImpl::NewL()
    {
    CTsFswClientImpl* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::NewLC()
// --------------------------------------------------------------------------
//   
CTsFswClientImpl* CTsFswClientImpl::NewLC()
    {
    CTsFswClientImpl* self = new ( ELeave ) CTsFswClientImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::CTsFswClientImpl()
// --------------------------------------------------------------------------
//   
CTsFswClientImpl::CTsFswClientImpl()
        : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::~CTsFswClientImpl()
// --------------------------------------------------------------------------
//   
CTsFswClientImpl::~CTsFswClientImpl()
    {
    Cancel();
    iClient.Close();
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::ConstructL()
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::ConstructL()
    {
    User::LeaveIfError( iClient.Connect() );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::GetContentL
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::GetContentL( RTsFswArray& aDst )
    {
    iClient.GetContentL( aDst );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::Subscribe
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::Subscribe( MTsFswObserver& aObserver )
    {
    Cancel();
    iObserver = &aObserver;
    iClient.Subscribe( iStatus );
    SetActive();
    }
    
// --------------------------------------------------------------------------
// CTsFswClientImpl::CancelSubscribe
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::CancelSubscribe()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::CloseApp
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::CloseApp( TInt aWgId )
    {
    iClient.CloseApp( aWgId );
    }
    
// --------------------------------------------------------------------------
// CTsFswClientImpl::SwitchToApp
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::SwitchToApp( TInt aWgId )
    {
    iClient.SwitchToApp( aWgId );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::ForegroundAppUid
// --------------------------------------------------------------------------
//   
TUid CTsFswClientImpl::ForegroundAppUid( TInt aType )
    {
    return iClient.ForegroundAppUid( aType );
    }

// --------------------------------------------------------------------------
// CTsFswClientImpl::RunL
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::RunL()
    {
    TSLOG_CONTEXT( RunL, TSLOG_LOCAL );
    TSLOG1_IN( "%d", iStatus.Int() );

    if ( iStatus == KErrNone && iObserver )
        {
        iClient.Subscribe( iStatus );
        SetActive();
        iObserver->HandleFswContentChanged();
        }

    TSLOG_OUT();
    }
    
// --------------------------------------------------------------------------
// CTsFswClientImpl::DoCancel
// --------------------------------------------------------------------------
//   
void CTsFswClientImpl::DoCancel()
    {
    iClient.CancelSubscribe();
    }


// end of file
