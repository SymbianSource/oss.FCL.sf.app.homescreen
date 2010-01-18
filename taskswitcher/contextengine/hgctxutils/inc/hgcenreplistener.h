/*
* ===========================================================================
*  Name        : HgCenrepListener.h
*  Part of     : Hg
*  Description : Class to get notifications about changes to a cenrep key
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

#ifndef __CENREPLISTENER_H__
#define __CENREPLISTENER_H__

#include <e32base.h>

class CRepository;

/**
 * Observer interface for getting notifications about a cenrep key change.
 */
class MHgCenrepChangeObserver
    {
public:
    virtual void CenrepChanged( TUint32 aKey, TInt aNewValue ) = 0;
    };

/**
 * Class to get notifications about changes to a cenrep key.
 */
NONSHARABLE_CLASS( CHgCenrepListener ) : public CActive
    {
public:
    /**
     * Fist step constructor
     */
    IMPORT_C static CHgCenrepListener* NewL(const TUid& aRep, TUint32 aKey,
                                            MHgCenrepChangeObserver& aObserver);
    
    /**
     * Fist step constructor
     */
    IMPORT_C static CHgCenrepListener* NewLC(const TUid& aRep, TUint32 aKey,
                                             MHgCenrepChangeObserver& aObserver);
    
    /**
     * Destructor.
     */
    IMPORT_C ~CHgCenrepListener();
    
    /**
     * Returns the current value of the watched key.
     */
    IMPORT_C TInt Value();
    
private:
    /**
     * Constructor.
     * Starts listening for the key.
     */
    CHgCenrepListener(TUint32 aKey,
        MHgCenrepChangeObserver& aObserver );
    
    /**
     * Second step constructor 
     */
    void ConstructL(const TUid& aRep);
    
    void DoCancel();
    void RunL(); 
    TInt RunError( TInt aError );
    
    MHgCenrepChangeObserver& iObserver;
    CRepository* iRep;
    TUint32 iKey;
    };
    
#endif
