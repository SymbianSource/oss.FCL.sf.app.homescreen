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
* Description:  Class that checks if chosen applications should be hidden
*                from the task swap.
*
*/


#ifndef TSFSHIDDENAPPLIST_H_
#define TSFSHIDDENAPPLIST_H_

#include <e32base.h>    // For CActive, link against: euser.lib
#include <e32std.h>     // For RTimer, link against: euser.lib
#include <e32property.h>// For RProperty
#include "tsfswobservers.h"

//FORWARD DECLARATIONS
class RWsSession;

/**
 * This class is handling the application which are hidden
 * from fast swap feature.
 */
NONSHARABLE_CLASS( CTsFsHiddenAppList ) : public CActive
    {
public:

    /**
     * Symbian based two phased constructor.
     */
    static CTsFsHiddenAppList* NewL( MTsFsHiddenAppListObserver& aObserver );

    /**
     * Symbian based two phased constructor.
     */
    static CTsFsHiddenAppList* NewLC( MTsFsHiddenAppListObserver& aObserver );

    /**
     * Destructor
     */
    ~CTsFsHiddenAppList();

    /**
     * Whether application is hidden from FSW.
     */
    TBool IsHiddenL( TUid aAppUid, const RWsSession& aWsSession, TInt aWgId );

    /**
     * Updating application list
     */
    void UpdateListL();

protected:
    /**
     * Default constructor.
     */
    CTsFsHiddenAppList( MTsFsHiddenAppListObserver& aObserver );

    /**
     * Second phase construction
     */
    void ConstructL();

    /**
     * Reset application array list.
     */
    void ResetArray();

    /**
     * Derived from CActive
     */
    virtual void DoCancel();

    /**
     * Derived from CActive
     */
    virtual void RunL();

private:
    /**
     * Property reader for reading Hiden app list from FSW
     */
    RProperty iProperty;

    /**
     * Hidden App list
     */
    CArrayFixFlat<TInt>* iHiddenAppList;

    /**
     * hidden apps list observer
     */
    MTsFsHiddenAppListObserver& iObserver;
    };

#endif /*TSFSHIDDENAPPLIST_H_*/
