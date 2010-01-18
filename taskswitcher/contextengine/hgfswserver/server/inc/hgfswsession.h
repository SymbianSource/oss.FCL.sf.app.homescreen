/*
 * ===========================================================================
 *  Name        : hgfswsession.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : session class for server
 *  Version     : %version: 4 %
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

#ifndef __HGFSWSESSION_H
#define __HGFSWSESSION_H

#include "hgfswserver.h"
#include "hgfswentry.h"

/**
 * Session class.
 */
class CHgFswSession : public CSession2
    {
public:
    CHgFswSession();
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
    ~CHgFswSession();
    CHgFswServer& Server();
    void ServiceL( const RMessage2& aMessage );
    void ServiceError( const RMessage2& aMessage, TInt aError );
    void CompleteSubscribe( TInt aError );

    RArray<RMessage2> iSubscribeRequests; // non-completed EHgFswSubscribe messages
    };

#endif
