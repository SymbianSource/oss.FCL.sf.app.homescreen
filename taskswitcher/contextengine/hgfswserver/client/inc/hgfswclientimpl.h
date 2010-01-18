/*
 * ===========================================================================
 *  Name        : hgfswclientimpl.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Client API, private implementation
 *  Version     : %version: 5 %
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

#ifndef __HGFSWCLIENTIMPL_H
#define __HGFSWCLIENTIMPL_H

#include <e32std.h>
#include "hgfswentry.h"

class MHgFswObserver;
    
NONSHARABLE_CLASS( CHgFswClientImpl ) : public CActive
    {
public:
    static CHgFswClientImpl* NewL();
    static CHgFswClientImpl* NewLC();
    ~CHgFswClientImpl();
    
    void GetContentL( RHgFswArray& aDst );
    
    void Subscribe( MHgFswObserver& aObserver );
    void CancelSubscribe();
    
    void CloseApp( TInt aWgId );
    void SwitchToApp( TInt aWgId );

    TUid ForegroundAppUid( TInt aType );

private:
    void RunL();
    void DoCancel();

private:
    CHgFswClientImpl();
    void ConstructL();

    NONSHARABLE_CLASS( RHgFswClient ) : public RSessionBase
        {
    public:
        TInt Connect();
        void Subscribe( TRequestStatus& aStatus );
        void CancelSubscribe();
        void GetContentL( RHgFswArray& aDst );
        void CloseApp( TInt aWgId );
        void SwitchToApp( TInt aWgId );
        TUid ForegroundAppUid( TInt aType );
        };
    RHgFswClient iClient;
    
    MHgFswObserver* iObserver; // not own
    };

#endif
