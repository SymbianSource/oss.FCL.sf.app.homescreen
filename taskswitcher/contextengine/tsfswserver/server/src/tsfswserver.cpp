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
* Description:  server implementation
 *
*/


#include "tsfswserver.h"
#include "tsfswsession.h"
#include "tsfswappui.h"
#include "tsfswengine.h"

#include <s32mem.h>
#include <eikenv.h>
#include <apgwgnam.h>

// --------------------------------------------------------------------------
// CTsFswServer::CTsFswServer
// --------------------------------------------------------------------------
//
CTsFswServer::CTsFswServer( TInt aPriority, CTsFswAppUi& aAppUi )
        : CServer2( aPriority ), iAppUi( aAppUi )
    {
    }

// --------------------------------------------------------------------------
// CTsFswServer::NewLC
// --------------------------------------------------------------------------
//
CTsFswServer* CTsFswServer::NewLC( CTsFswAppUi& aAppUi )
    {
    CTsFswServer* self = new ( ELeave ) CTsFswServer(
        EPriorityNormal, aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswServer::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswServer::ConstructL()
    {
    StartL( KTsFswServerName );
    iEngine = CTsFswEngine::NewL( *this );
    iAppUi.SetTaskListObserver( *iEngine );
    iAppUi.SetResourceObserver( *iEngine );
    }

// --------------------------------------------------------------------------
// CTsFswServer::~CTsFswServer
// --------------------------------------------------------------------------
//
CTsFswServer::~CTsFswServer()
    {
    delete iBuffer;
    delete iEngine;
    }

// --------------------------------------------------------------------------
// CTsFswServer::NewSessionL
// --------------------------------------------------------------------------
//
CSession2* CTsFswServer::NewSessionL( const TVersion&,
        const RMessage2& ) const
    {
    return new ( ELeave ) CTsFswSession;
    }

// --------------------------------------------------------------------------
// CTsFswServer::AddSession
// --------------------------------------------------------------------------
//
void CTsFswServer::AddSession()
    {
    ++iSessionCount;
    }

// --------------------------------------------------------------------------
// CTsFswServer::DropSession
// --------------------------------------------------------------------------
//
void CTsFswServer::DropSession()
    {
    --iSessionCount;
    }

// --------------------------------------------------------------------------
// CTsFswServer::SerializeAndGetBufferSizeL
// --------------------------------------------------------------------------
//
void CTsFswServer::SerializeAndGetBufferSizeL( const RMessage2& aMessage )
    {
    const TInt KBufferExpandSize = 128;
    const RTsFswArray& array( iEngine->FswDataL() );
    delete iBuffer; iBuffer = NULL;
    iBuffer = CBufFlat::NewL( KBufferExpandSize );
    RBufWriteStream ws;
    CleanupClosePushL( ws );
    ws.Open( *iBuffer );
    CTsFswEntry::ExternalizeArrayL( ws, array );
    CleanupStack::PopAndDestroy( &ws );
    TPtr8 p( iBuffer->Ptr( 0 ) );
    TPckg<TInt> size( p.Length() );
    aMessage.WriteL( 0, size );
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CTsFswServer::GetBufferL
// --------------------------------------------------------------------------
//
void CTsFswServer::GetBufferL( const RMessage2& aMessage )
    {
    if ( !iBuffer )
        {
        User::Leave( KErrGeneral );
        }
    TPtr8 p( iBuffer->Ptr( 0 ) );
    // If the size expected by the client does not match indicate it with a leave
    // so the client can recognize it by checking the return value of SendReceive
    // and can request the buffer size again.
    if ( p.Length() != aMessage.Int1() )
        {
        User::Leave( KErrArgument );
        }
    aMessage.WriteL( 0, p );
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CTsFswServer::CloseAppL
// --------------------------------------------------------------------------
//
void CTsFswServer::CloseAppL( TInt aWgId )
    {
    iEngine->CloseAppL( aWgId );
    }

// --------------------------------------------------------------------------
// CTsFswServer::SwitchToAppL
// --------------------------------------------------------------------------
//
void CTsFswServer::SwitchToAppL( TInt aWgId )
    {
    iEngine->SwitchToAppL( aWgId );
    }

// --------------------------------------------------------------------------
// CTsFswServer::ForegroundAppUidL
// --------------------------------------------------------------------------
//
void CTsFswServer::ForegroundAppUidL( const RMessage2& aMessage )
    {
    TUid uid = iEngine->ForegroundAppUidL( aMessage.Int1() );
    TPckg<TInt> uidPckg( uid.iUid );
    aMessage.WriteL( 0, uidPckg );
    }

// --------------------------------------------------------------------------
// CTsFswServer::FswDataChanged
// callback from engine
// --------------------------------------------------------------------------
//
void CTsFswServer::FswDataChanged()
    {
    // notify all sessions about the change
    iSessionIter.SetToFirst();
    while ( CTsFswSession* session = static_cast<CTsFswSession*>( iSessionIter++ ) )
        {
        if ( session->IsListening() )
            {
            session->FswDataChanged();
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFswServer::FswDataListenersCount
// callback from engine
// --------------------------------------------------------------------------
//
TInt CTsFswServer::FswDataListenerCount()
    {
    TInt n = 0;
    iSessionIter.SetToFirst();
    while ( CTsFswSession* session = static_cast<CTsFswSession*>( iSessionIter++ ) )
        {
        if ( session->IsListening() )
            {
            ++n;
            }
        }
    return n;
    }

// --------------------------------------------------------------------------
// RunServerL
// --------------------------------------------------------------------------
//
static void RunServerL( CTsFswAppUi& aAppUi, CEikonEnv& aEnv )
    {
    // change thread name
    User::LeaveIfError( RThread::RenameMe( KTsFswServerName ) );

    // set app as hidden
    CApaWindowGroupName* wgName =
        CApaWindowGroupName::NewL( aEnv.WsSession(),
            aEnv.RootWin().Identifier() );
    wgName->SetHidden( ETrue );
    wgName->SetWindowGroupName( aEnv.RootWin() );
    delete wgName;

    // start server
    CTsFswServer* server = CTsFswServer::NewLC( aAppUi );
    RProcess::Rendezvous( KErrNone );
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( server );
    }

// --------------------------------------------------------------------------
// E32Main
// --------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt err = KErrNoMemory;
    if ( cleanup )
        {
        CEikonEnv* env = new CEikonEnv;
        if ( env )
            {
            TRAP( err, env->ConstructL() );
            if ( err == KErrNone )
                {
                env->DisableExitChecks( ETrue );
                CTsFswAppUi* ui = new CTsFswAppUi;
                if ( ui )
                    {
                    TRAP( err, ui->ConstructL() );
                    if ( err == KErrNone )
                        {
                        // hide from tasklist and prevent from coming foreground
                        env->RootWin().SetOrdinalPosition( 0,
                            ECoeWinPriorityNeverAtFront );
                        // set as system app so will not be closed when memory is low
                        env->SetSystem( ETrue );
                        
                        RFbsSession::Connect();
                        
                        TRAP( err, RunServerL( *ui, *env ) );
                        
                        RFbsSession::Disconnect();
                        
                        ui->PrepareToExit();
                        }
                    }
                }
            env->DestroyEnvironment();
            }
        delete cleanup;
        }
    __UHEAP_MARKEND;
    return err;
    }


// end of file
