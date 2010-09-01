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
* Description:  Non-closeable app list class
 *
*/


#ifndef TSFSALWAYSSHOWNAPPLIST_H
#define TSFSALWAYSSHOWNAPPLIST_H

#include <e32base.h>
#include <apgcli.h>

/**
 * List of always shown applications.
 */
NONSHARABLE_CLASS( CTsFsAlwaysShownAppList ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aParent
     */
    static CTsFsAlwaysShownAppList* NewL();

    /*
     * Default C++ constructor
     */
    CTsFsAlwaysShownAppList();

    /*
     * Destructor
     */
    ~CTsFsAlwaysShownAppList();

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
