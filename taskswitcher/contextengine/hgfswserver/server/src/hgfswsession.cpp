/*
 * ===========================================================================
 *  Name        : hgfswsession.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : session implementation
 *  Version     : %version: sa1spcx1#8 %
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

#include "hgfswsession.h"

// --------------------------------------------------------------------------
// CHgFswSession::CHgFswSession
// --------------------------------------------------------------------------
//
CHgFswSession::CHgFswSession()
    {
    }

// --------------------------------------------------------------------------
// CHgFswSession::Server
// --------------------------------------------------------------------------
//
CHgFswServer& CHgFswSession::Server()
    {
    return *static_cast<CHgFswServer*>(
        const_cast<CServer2*>( CSession2::Server() ) );
    }

// --------------------------------------------------------------------------
// CHgFswSession::CreateL
// --------------------------------------------------------------------------
//
void CHgFswSession::CreateL() // codescanner::LFunctionCantLeave (virtual)
    {
    Server().AddSession();
    }

// --------------------------------------------------------------------------
// CHgFswSession::~CHgFswSession
// --------------------------------------------------------------------------
//
CHgFswSession::~CHgFswSession()
    {
    CompleteSubscribe( KErrCancel );
    iSubscribeRequests.Close();
    Server().DropSession();
    }

// --------------------------------------------------------------------------
// CHgFswSession::CompleteSubscribe
// --------------------------------------------------------------------------
//
void CHgFswSession::CompleteSubscribe( TInt aError )
    {
    for ( TInt i = 0, ie = iSubscribeRequests.Count(); i != ie; ++i )
        {
        iSubscribeRequests[i].Complete( aError );
        }
    iSubscribeRequests.Reset();
    }

// --------------------------------------------------------------------------
// CHgFswSession::ServiceL
// --------------------------------------------------------------------------
//
void CHgFswSession::ServiceL( const RMessage2& aMessage )
    {
    if ( !aMessage.HasCapability( ECapabilityLocalServices ) )
        {
        aMessage.Panic( KHgFswPanicName, EPanicAccessDenied );
        return;
        }

    switch ( aMessage.Function() )
        {
    case EHgFswSubscribe:
        iSubscribeRequests.AppendL( aMessage );
        break;

    case EHgFswCancel:
        CompleteSubscribe( KErrCancel );
        aMessage.Complete( KErrNone );
        break;

    case EHgFswGetBufferSize:
        Server().SerializeAndGetBufferSizeL( aMessage );
        break;

    case EHgFswGetBuffer:
        Server().GetBufferL( aMessage );
        break;

    case EHgFswCloseApp:
        Server().CloseAppL( aMessage.Int0() );
        aMessage.Complete( KErrNone );
        break;

    case EHgFswSwitchToApp:
        Server().SwitchToAppL( aMessage.Int0() );
        aMessage.Complete( KErrNone );
        break;
        
    case EHgFswForegroundAppUid:
        Server().ForegroundAppUidL( aMessage );
        aMessage.Complete( KErrNone );
        break;

    default:
        aMessage.Panic( KHgFswPanicName, EPanicIllegalFunction );
        break;
        }
    }

// --------------------------------------------------------------------------
// CHgFswSession::ServiceError
// --------------------------------------------------------------------------
//
void CHgFswSession::ServiceError( const RMessage2& aMessage,
        TInt aError )
    {
    if ( aError == KErrBadDescriptor )
        {
        aMessage.Panic( KHgFswPanicName, EPanicBadDescriptor );
        }
    CSession2::ServiceError( aMessage, aError );
    }

// --------------------------------------------------------------------------
// CHgFswSession::FswDataChanged
// called by server when notification is received from engine
// --------------------------------------------------------------------------
//
void CHgFswSession::FswDataChanged()
    {
    CompleteSubscribe( KErrNone );
    }

// --------------------------------------------------------------------------
// CHgFswSession::IsListening
// --------------------------------------------------------------------------
//
TBool CHgFswSession::IsListening()
    {
    return iSubscribeRequests.Count() > 0;
    }


// end of file
