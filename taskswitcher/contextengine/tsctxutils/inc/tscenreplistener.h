/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef CTSCENREPLISTENER_H
#define CTSCENREPLISTENER_H

#include <e32base.h>

class CRepository;

/**
 * Observer interface for getting notifications about a cenrep key change.
 */
class MTsCenrepChangeObserver
    {
public:
    virtual void CenrepChanged( TUint32 aKey, TInt aNewValue ) = 0;
    };

/**
 * Class to get notifications about changes to a cenrep key.
 */
NONSHARABLE_CLASS( CTsCenrepListener ) : public CActive
    {
public:
    /**
     * Fist step constructor
     */
    IMPORT_C static CTsCenrepListener* NewL(const TUid& aRep, TUint32 aKey,
                                            MTsCenrepChangeObserver& aObserver);
    
    /**
     * Fist step constructor
     */
    IMPORT_C static CTsCenrepListener* NewLC(const TUid& aRep, TUint32 aKey,
                                             MTsCenrepChangeObserver& aObserver);
    
    /**
     * Destructor.
     */
    IMPORT_C ~CTsCenrepListener();
    
    /**
     * Returns the current value of the watched key.
     */
    IMPORT_C TInt Value();
    
private:
    /**
     * Constructor.
     * Starts listening for the key.
     */
    CTsCenrepListener(TUint32 aKey,
        MTsCenrepChangeObserver& aObserver );
    
    /**
     * Second step constructor 
     */
    void ConstructL(const TUid& aRep);
    
    void DoCancel();
    void RunL(); 
    TInt RunError( TInt aError );
    
    MTsCenrepChangeObserver& iObserver;
    CRepository* iRep;
    TUint32 iKey;
    };
    
#endif /*CTSCENREPLISTENER_H*/
