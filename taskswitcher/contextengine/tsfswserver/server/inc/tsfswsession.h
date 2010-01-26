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
* Description:  session class for server
 *
*/


#ifndef TSFSWSESSION_H
#define TSFSWSESSION_H

#include "tsfswserver.h"
#include "tsfswentry.h"

/**
 * Session class.
 */
class CTsFswSession : public CSession2
    {
public:
    CTsFswSession();
    void CreateL();
    
    /**
     * Called from the server when fsw content has changed.
     */
    void FswDataChanged();
    
    /**
     * Returns true if there is a pending subscribe request to this session.
     */
    TBool IsListening();

private:
    ~CTsFswSession();
    CTsFswServer& Server();
    void ServiceL( const RMessage2& aMessage );
    void ServiceError( const RMessage2& aMessage, TInt aError );
    void CompleteSubscribe( TInt aError );

    RArray<RMessage2> iSubscribeRequests; // non-completed ETsFswSubscribe messages
    };

#endif
