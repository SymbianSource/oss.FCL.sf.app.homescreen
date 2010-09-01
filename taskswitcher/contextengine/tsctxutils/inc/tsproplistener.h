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

#ifndef CTSPROPERTYLISTENER_H
#define CTSPROPERTYLISTENER_H

#include <e32base.h>
#include <e32property.h>

/**
 * Observer interface for getting notifications about a P&S property change.
 */
class MTsPropertyChangeObserver
    {
public:
    virtual void PropertyChanged( TUid aCategory, TUint aKey ) = 0;
    };

/**
 * Class to get notifications about changes to a P&S property.
 */
NONSHARABLE_CLASS( CTsPropertyListener ) : public CActive
    {
public:
    /**
     * Constructor.
     * Starts listening for the property with the given category/key.
     */
    IMPORT_C CTsPropertyListener( TUid aCategory,
        TUint aKey, MTsPropertyChangeObserver& aObserver );
        
    /**
     * Destructor.
    */
    IMPORT_C ~CTsPropertyListener();
    
private:
    void DoCancel();
    void RunL(); 
    TInt RunError( TInt aError );

    MTsPropertyChangeObserver& iObserver;
    RProperty iProperty;
    TUid iCategory;
    TUint iKey;
    };

#endif /*CTSPROPERTYLISTENER_H*/

// End of file
