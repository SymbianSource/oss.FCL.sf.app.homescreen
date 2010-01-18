/*
 * ============================================================================
 *  Name        : hgfsalwaysshownapplist.h
 *  Part of     : Hg Teleport
 *  Description : Non-closeable app list class
 *  Version     : %version: sa1spcx1#2 %
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

#ifndef HGFSALWAYSSHOWNAPPLIST_H
#define HGFSALWAYSSHOWNAPPLIST_H

#include <e32base.h>
#include <apgcli.h>

/**
 * List of always shown applications.
 */
NONSHARABLE_CLASS( CHgFsAlwaysShownAppList ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aParent
     */
    static CHgFsAlwaysShownAppList* NewL();

    /*
     * Default C++ constructor
     */
    CHgFsAlwaysShownAppList();

    /*
     * Destructor
     */
    ~CHgFsAlwaysShownAppList();

    /**
     * Initializes the list that stores applications that are always
     * shown in the fast swap provided they have been installed
     * in the phone rom
     * @return   -
     */
    void InitializeAlwaysShownListL();

    /**
     * If application uid exists in the always shown application list
     * @return   ETrue  application uid exists in the list
     *           EFalse application uid doesn't exist in the list
     * @param    TUid aAppUid the application uid
     */
    TBool IsAlwaysShownApp( TUid aAppUid );

    /**
     * Returning reference to iAppList
     */
    RArray<TUid>& AppList() { return iAppList; }

private:    
    void ConstructL();
    RArray<TUid> iAppList;
    RApaLsSession iAppArcSession;
    };

#endif
