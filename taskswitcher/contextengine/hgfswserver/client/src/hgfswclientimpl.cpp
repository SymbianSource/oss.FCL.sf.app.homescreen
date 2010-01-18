/*
 * ===========================================================================
 *  Name        : hgfswclientimpl.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Client API, private implementation
 *  Version     : %version: sa1spcx1#10 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#include "hgfswclientimpl.h"
#include "hgfswclientobserver.h"
#include "hgfswcommon.h"
#include <s32mem.h>

#include "clientlogging.h"

static TInt StartServer()
    {
    RProcess server;
    const TUidType uid( KNullUid, KNullUid, KHgFswServerUid );
    TInt err = server.Create( KHgFswServerImg, KNullDesC, uid );
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

TInt CHgFswClientImpl::RHgFswClient::Connect()
    {
    const TInt KAsyncMessageSlots = 4;
    const TInt KMaxRetry = 4;
    
    TInt retry = KMaxRetry;
    for ( ; ; )
        {
        TInt err = CreateSession( KHgFswServerName, TVersion( 0, 0, 0 ), KAsyncMessageSlots );
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

void CHgFswClientImpl::RHgFswClient::Subscribe( TRequestStatus& aStatus )
    {
    SendReceive( EHgFswSubscribe, aStatus );
    }
    
void CHgFswClientImpl::RHgFswClient::CancelSubscribe()
    {
    SendReceive( EHgFswCancel );
    }

void CHgFswClientImpl::RHgFswClient::GetContentL( RHgFswArray& aDst )
    {
    HGLOG_CONTEXT( GetContentL, HGLOG_LOCAL );
    HGLOG_IN();

    for ( ; ; )
        {
        TPckgBuf<TInt> bufSize;
        User::LeaveIfError( SendReceive( EHgFswGetBufferSize,
            TIpcArgs( &bufSize ) ) );
        HBufC8* buf = HBufC8::NewLC( bufSize() );
        TPtr8 p( buf->Des() );
        TInt err = SendReceive( EHgFswGetBuffer,
            TIpcArgs( &p, bufSize() ) );
        if ( err == KErrNone )
            {
            RDesReadStream strm( p );
            CleanupClosePushL( strm );
            CHgFswEntry::InternalizeArrayL( strm, aDst );
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

    HGLOG_OUT();
    }

void CHgFswClientImpl::RHgFswClient::CloseApp( TInt aWgId )
    {
    SendReceive( EHgFswCloseApp, TIpcArgs( aWgId ) );
    }

void CHgFswClientImpl::RHgFswClient::SwitchToApp( TInt aWgId )
    {
    SendReceive( EHgFswSwitchToApp, TIpcArgs( aWgId ) );
    }

TUid CHgFswClientImpl::RHgFswClient::ForegroundAppUid( TInt aType )
    {
    TUid result = KNullUid;
    TPckgBuf<TInt> uidBuf;
    if ( SendReceive( EHgFswForegroundAppUid,
            TIpcArgs( &uidBuf, &aType ) ) == KErrNone )
        {
        result = TUid::Uid( uidBuf() );
        }
    return result;
    }

CHgFswClientImpl* CHgFswClientImpl::NewL()
    {
    CHgFswClientImpl* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CHgFswClientImpl* CHgFswClientImpl::NewLC()
    {
    CHgFswClientImpl* self = new ( ELeave ) CHgFswClientImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CHgFswClientImpl::CHgFswClientImpl()
        : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

CHgFswClientImpl::~CHgFswClientImpl()
    {
    Cancel();
    iClient.Close();
    }

void CHgFswClientImpl::ConstructL()
    {
    User::LeaveIfError( iClient.Connect() );
    }

void CHgFswClientImpl::GetContentL( RHgFswArray& aDst )
    {
    iClient.GetContentL( aDst );
    }

void CHgFswClientImpl::Subscribe( MHgFswObserver& aObserver )
    {
    Cancel();
    iObserver = &aObserver;
    iClient.Subscribe( iStatus );
    SetActive();
    }
    
void CHgFswClientImpl::CancelSubscribe()
    {
    Cancel();
    }

void CHgFswClientImpl::CloseApp( TInt aWgId )
    {
    iClient.CloseApp( aWgId );
    }
    
void CHgFswClientImpl::SwitchToApp( TInt aWgId )
    {
    iClient.SwitchToApp( aWgId );
    }

TUid CHgFswClientImpl::ForegroundAppUid( TInt aType )
    {
    return iClient.ForegroundAppUid( aType );
    }

void CHgFswClientImpl::RunL()
    {
    HGLOG_CONTEXT( RunL, HGLOG_LOCAL );
    HGLOG1_IN( "%d", iStatus.Int() );

    if ( iStatus == KErrNone && iObserver )
        {
        iClient.Subscribe( iStatus );
        SetActive();
        iObserver->HandleFswContentChanged();
        }

    HGLOG_OUT();
    }
    
void CHgFswClientImpl::DoCancel()
    {
    iClient.CancelSubscribe();
    }


// end of file
