/*
 * ===========================================================================
 *  Name        : hgfswserver.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : server implementation
 *  Version     : %version: sa1spcx1#14 %
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

#include "hgfswserver.h"
#include "hgfswsession.h"
#include "hgfswappui.h"
#include "hgfswengine.h"

#include <s32mem.h>
#include <eikenv.h>
#include <apgwgnam.h>

// --------------------------------------------------------------------------
// CHgFswServer::CHgFswServer
// --------------------------------------------------------------------------
//
CHgFswServer::CHgFswServer( TInt aPriority, CHgFswAppUi& aAppUi )
        : CServer2( aPriority ), iAppUi( aAppUi )
    {
    }

// --------------------------------------------------------------------------
// CHgFswServer::NewLC
// --------------------------------------------------------------------------
//
CHgFswServer* CHgFswServer::NewLC( CHgFswAppUi& aAppUi )
    {
    CHgFswServer* self = new ( ELeave ) CHgFswServer(
        EPriorityNormal, aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CHgFswServer::ConstructL
// --------------------------------------------------------------------------
//
void CHgFswServer::ConstructL()
    {
    StartL( KHgFswServerName );
    iEngine = CHgFswEngine::NewL( *this );
    iAppUi.SetTaskListObserver( *iEngine );
    iAppUi.SetResourceObserver( *iEngine );
    }

// --------------------------------------------------------------------------
// CHgFswServer::~CHgFswServer
// --------------------------------------------------------------------------
//
CHgFswServer::~CHgFswServer()
    {
    delete iBuffer;
    delete iEngine;
    }

// --------------------------------------------------------------------------
// CHgFswServer::NewSessionL
// --------------------------------------------------------------------------
//
CSession2* CHgFswServer::NewSessionL( const TVersion&,
        const RMessage2& ) const
    {
    return new ( ELeave ) CHgFswSession;
    }

// --------------------------------------------------------------------------
// CHgFswServer::AddSession
// --------------------------------------------------------------------------
//
void CHgFswServer::AddSession()
    {
    ++iSessionCount;
    }

// --------------------------------------------------------------------------
// CHgFswServer::DropSession
// --------------------------------------------------------------------------
//
void CHgFswServer::DropSession()
    {
    --iSessionCount;
    }

// --------------------------------------------------------------------------
// CHgFswServer::SerializeAndGetBufferSizeL
// --------------------------------------------------------------------------
//
void CHgFswServer::SerializeAndGetBufferSizeL( const RMessage2& aMessage )
    {
    const TInt KBufferExpandSize = 128;
    const RHgFswArray& array( iEngine->FswDataL() );
    delete iBuffer; iBuffer = NULL;
    iBuffer = CBufFlat::NewL( KBufferExpandSize );
    RBufWriteStream ws;
    CleanupClosePushL( ws );
    ws.Open( *iBuffer );
    CHgFswEntry::ExternalizeArrayL( ws, array );
    CleanupStack::PopAndDestroy( &ws );
    TPtr8 p( iBuffer->Ptr( 0 ) );
    TPckg<TInt> size( p.Length() );
    aMessage.WriteL( 0, size );
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CHgFswServer::GetBufferL
// --------------------------------------------------------------------------
//
void CHgFswServer::GetBufferL( const RMessage2& aMessage )
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
// CHgFswServer::CloseAppL
// --------------------------------------------------------------------------
//
void CHgFswServer::CloseAppL( TInt aWgId )
    {
    iEngine->CloseAppL( aWgId );
    }

// --------------------------------------------------------------------------
// CHgFswServer::SwitchToAppL
// --------------------------------------------------------------------------
//
void CHgFswServer::SwitchToAppL( TInt aWgId )
    {
    iEngine->SwitchToAppL( aWgId );
    }

// --------------------------------------------------------------------------
// CHgFswServer::ForegroundAppUidL
// --------------------------------------------------------------------------
//
void CHgFswServer::ForegroundAppUidL( const RMessage2& aMessage )
    {
    TUid uid = iEngine->ForegroundAppUidL( aMessage.Int1() );
    TPckg<TInt> uidPckg( uid.iUid );
    aMessage.WriteL( 0, uidPckg );
    }

// --------------------------------------------------------------------------
// CHgFswServer::FswDataChanged
// callback from engine
// --------------------------------------------------------------------------
//
void CHgFswServer::FswDataChanged()
    {
    // notify all sessions about the change
    iSessionIter.SetToFirst();
    while ( CHgFswSession* session = static_cast<CHgFswSession*>( iSessionIter++ ) )
        {
        if ( session->IsListening() )
            {
            session->FswDataChanged();
            }
        }
    }

// --------------------------------------------------------------------------
// CHgFswServer::FswDataListenersCount
// callback from engine
// --------------------------------------------------------------------------
//
TInt CHgFswServer::FswDataListenerCount()
    {
    TInt n = 0;
    iSessionIter.SetToFirst();
    while ( CHgFswSession* session = static_cast<CHgFswSession*>( iSessionIter++ ) )
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
static void RunServerL( CHgFswAppUi& aAppUi, CEikonEnv& aEnv )
    {
    // change thread name
    User::LeaveIfError( RThread::RenameMe( KHgFswServerName ) );

    // set app as hidden
    CApaWindowGroupName* wgName =
        CApaWindowGroupName::NewL( aEnv.WsSession(),
            aEnv.RootWin().Identifier() );
    wgName->SetHidden( ETrue );
    wgName->SetWindowGroupName( aEnv.RootWin() );
    delete wgName;

    // start server
    CHgFswServer* server = CHgFswServer::NewLC( aAppUi );
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
                CHgFswAppUi* ui = new CHgFswAppUi;
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
