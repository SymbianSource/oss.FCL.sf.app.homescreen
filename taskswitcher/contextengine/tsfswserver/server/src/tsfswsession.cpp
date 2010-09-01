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
* Description:  session implementation
 *
*/


#include "tsfswsession.h"

// --------------------------------------------------------------------------
// CTsFswSession::CTsFswSession
// --------------------------------------------------------------------------
//
CTsFswSession::CTsFswSession()
    {
    }

// --------------------------------------------------------------------------
// CTsFswSession::Server
// --------------------------------------------------------------------------
//
CTsFswServer& CTsFswSession::Server()
    {
    return *static_cast<CTsFswServer*>(
        const_cast<CServer2*>( CSession2::Server() ) );
    }

// --------------------------------------------------------------------------
// CTsFswSession::CreateL
// --------------------------------------------------------------------------
//
void CTsFswSession::CreateL() // codescanner::LFunctionCantLeave (virtual)
    {
    Server().AddSession();
    }

// --------------------------------------------------------------------------
// CTsFswSession::~CTsFswSession
// --------------------------------------------------------------------------
//
CTsFswSession::~CTsFswSession()
    {
    CompleteSubscribe( KErrCancel );
    iSubscribeRequests.Close();
    Server().DropSession();
    }

// --------------------------------------------------------------------------
// CTsFswSession::CompleteSubscribe
// --------------------------------------------------------------------------
//
void CTsFswSession::CompleteSubscribe( TInt aError )
    {
    for ( TInt i = 0, ie = iSubscribeRequests.Count(); i != ie; ++i )
        {
        iSubscribeRequests[i].Complete( aError );
        }
    iSubscribeRequests.Reset();
    }

// --------------------------------------------------------------------------
// CTsFswSession::ServiceL
// --------------------------------------------------------------------------
//
void CTsFswSession::ServiceL( const RMessage2& aMessage )
    {
    if ( !aMessage.HasCapability( ECapabilityLocalServices ) )
        {
        aMessage.Panic( KTsFswPanicName, EPanicAccessDenied );
        return;
        }

    switch ( aMessage.Function() )
        {
    case ETsFswSubscribe:
        iSubscribeRequests.AppendL( aMessage );
        break;

    case ETsFswCancel:
        CompleteSubscribe( KErrCancel );
        aMessage.Complete( KErrNone );
        break;

    case ETsFswGetBufferSize:
        Server().SerializeAndGetBufferSizeL( aMessage );
        break;

    case ETsFswGetBuffer:
        Server().GetBufferL( aMessage );
        break;

    case ETsFswCloseApp:
        Server().CloseAppL( aMessage.Int0() );
        aMessage.Complete( KErrNone );
        break;

    case ETsFswSwitchToApp:
        Server().SwitchToAppL( aMessage.Int0() );
        aMessage.Complete( KErrNone );
        break;
        
    case ETsFswForegroundAppUid:
        Server().ForegroundAppUidL( aMessage );
        aMessage.Complete( KErrNone );
        break;

    default:
        aMessage.Panic( KTsFswPanicName, EPanicIllegalFunction );
        break;
        }
    }

// --------------------------------------------------------------------------
// CTsFswSession::ServiceError
// --------------------------------------------------------------------------
//
void CTsFswSession::ServiceError( const RMessage2& aMessage,
        TInt aError )
    {
    if ( aError == KErrBadDescriptor )
        {
        aMessage.Panic( KTsFswPanicName, EPanicBadDescriptor );
        }
    CSession2::ServiceError( aMessage, aError );
    }

// --------------------------------------------------------------------------
// CTsFswSession::FswDataChanged
// called by server when notification is received from engine
// --------------------------------------------------------------------------
//
void CTsFswSession::FswDataChanged()
    {
    CompleteSubscribe( KErrNone );
    }

// --------------------------------------------------------------------------
// CTsFswSession::IsListening
// --------------------------------------------------------------------------
//
TBool CTsFswSession::IsListening()
    {
    return iSubscribeRequests.Count() > 0;
    }


// end of file
