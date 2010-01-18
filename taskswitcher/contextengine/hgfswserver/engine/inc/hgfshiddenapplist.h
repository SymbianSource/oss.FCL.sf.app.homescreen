/*
* ============================================================================
*  Name        : hgfshiddenapplist.h
*  Part of     : Hg Teleport / Fast Swap module
*  Description : Class that checks if chosen applications should be hidden
*                from the task swap.
*  Version     : %version: 1 %
*
*  Copyright 2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

#ifndef HGFSHIDDENAPPLIST_H_
#define HGFSHIDDENAPPLIST_H_

#include <e32base.h>    // For CActive, link against: euser.lib
#include <e32std.h>     // For RTimer, link against: euser.lib
#include <e32property.h>// For RProperty
#include "hgfswobservers.h"

//FORWARD DECLARATIONS
class RWsSession;

/**
 * This class is handling the application which are hidden
 * from fast swap feature.
 */
NONSHARABLE_CLASS( CHgFsHiddenAppList ) : public CActive
    {
public:

    /**
     * Symbian based two phased constructor.
     */
    static CHgFsHiddenAppList* NewL( MHgFsHiddenAppListObserver& aObserver );

    /**
     * Symbian based two phased constructor.
     */
    static CHgFsHiddenAppList* NewLC( MHgFsHiddenAppListObserver& aObserver );

    /**
     * Destructor
     */
    ~CHgFsHiddenAppList();

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
    CHgFsHiddenAppList( MHgFsHiddenAppListObserver& aObserver );

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
    MHgFsHiddenAppListObserver& iObserver;
    };

#endif /*HGFSHIDDENAPPLIST_H_*/
