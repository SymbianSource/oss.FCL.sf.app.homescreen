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
* Description:  server class
 *
*/


#ifndef TSFSWSERVER_H
#define TSFSWSERVER_H

#include <e32base.h>
#include "tsfswcommon.h"
#include "tsfswobservers.h"

/**
 * Possible panic reasons.
 */
enum TTsFswPanic
    {
    EPanicBadDescriptor,
    EPanicIllegalFunction,
    EPanicAccessDenied
    };

/**
 * Panic name.
 */
_LIT( KTsFswPanicName, "tsfsw" );

class CTsFswEngine;
class CTsFswAppUi;

/**
 * Ts FastSwap Server.
 */
class CTsFswServer : public CServer2, public MTsFswEngineObserver
    {
public:
    static CTsFswServer* NewLC( CTsFswAppUi& aAppUi );
    ~CTsFswServer();
    void AddSession();
    void DropSession();

    void SerializeAndGetBufferSizeL( const RMessage2& aMessage );
    void GetBufferL( const RMessage2& aMessage );
    void CloseAppL( TInt aWgId );
    void SwitchToAppL( TInt aWgId );
    void ForegroundAppUidL( const RMessage2& aMessage );

private:
    CTsFswServer( TInt aPriority, CTsFswAppUi& aAppUi );
    void ConstructL();
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage ) const;
        
    // from MTsFswEngineObserver
    void FswDataChanged();
private:
    TInt iSessionCount;
    CTsFswEngine* iEngine;
    CBufFlat* iBuffer;
    CTsFswAppUi& iAppUi;
    };

#endif
