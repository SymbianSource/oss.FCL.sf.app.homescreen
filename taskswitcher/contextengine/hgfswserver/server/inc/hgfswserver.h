/*
 * ===========================================================================
 *  Name        : hgfswserver.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : server class
 *  Version     : %version: 6 %
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

#ifndef __HGFSWSERVER_H
#define __HGFSWSERVER_H

#include <e32base.h>
#include "hgfswcommon.h"
#include "hgfswobservers.h"

/**
 * Possible panic reasons.
 */
enum THgFswPanic
    {
    EPanicBadDescriptor,
    EPanicIllegalFunction,
    EPanicAccessDenied
    };

/**
 * Panic name.
 */
_LIT( KHgFswPanicName, "hgfsw" );

class CHgFswEngine;
class CHgFswAppUi;

/**
 * Hg FastSwap Server.
 */
class CHgFswServer : public CServer2, public MHgFswEngineObserver
    {
public:
    static CHgFswServer* NewLC( CHgFswAppUi& aAppUi );
    ~CHgFswServer();
    void AddSession();
    void DropSession();

    void SerializeAndGetBufferSizeL( const RMessage2& aMessage );
    void GetBufferL( const RMessage2& aMessage );
    void CloseAppL( TInt aWgId );
    void SwitchToAppL( TInt aWgId );
    void ForegroundAppUidL( const RMessage2& aMessage );

private:
    CHgFswServer( TInt aPriority, CHgFswAppUi& aAppUi );
    void ConstructL();
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage ) const;
        
    // from MHgFswEngineObserver
    void FswDataChanged();
    TInt FswDataListenerCount();

private:
    TInt iSessionCount;
    CHgFswEngine* iEngine;
    CBufFlat* iBuffer;
    CHgFswAppUi& iAppUi;
    };

#endif
