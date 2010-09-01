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


#ifndef TSFSWCLIENTIMPL_H
#define TSFSWCLIENTIMPL_H

#include <e32std.h>
#include "tsfswentry.h"

class MTsFswObserver;
    
NONSHARABLE_CLASS( CTsFswClientImpl ) : public CActive
    {
public:
    static CTsFswClientImpl* NewL();
    static CTsFswClientImpl* NewLC();
    ~CTsFswClientImpl();
    
    void GetContentL( RTsFswArray& aDst );
    
    void Subscribe( MTsFswObserver& aObserver );
    void CancelSubscribe();
    
    void CloseApp( TInt aWgId );
    void SwitchToApp( TInt aWgId );

    TUid ForegroundAppUid( TInt aType );

private:
    void RunL();
    void DoCancel();

private:
    CTsFswClientImpl();
    void ConstructL();

    NONSHARABLE_CLASS( RTsFswClient ) : public RSessionBase
        {
    public:
        TInt Connect();
        void Subscribe( TRequestStatus& aStatus );
        void CancelSubscribe();
        void GetContentL( RTsFswArray& aDst );
        void CloseApp( TInt aWgId );
        void SwitchToApp( TInt aWgId );
        TUid ForegroundAppUid( TInt aType );
        };
    RTsFswClient iClient;
    
    MTsFswObserver* iObserver; // not own
    };

#endif
