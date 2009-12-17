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
* Description:
* Subscribes properties from P&S
 *
*/

#ifndef XNPROPERTYSUBSCRIBER_H
#define XNPROPERTYSUBSCRIBER_H

// System includes
#include <e32base.h>
#include <e32property.h>

// Class declaration
/**
 * Observer for property change.
 */
class MXnPropertyChangeObserver
    {
public:
    /**
     * Property changed
     *
     * @since S60 5.1
     * @param aKey key value
     * @param aValue property value
     */
    virtual void PropertyChangedL( const TUint32 aKey, const TInt aValue ) = 0;
    };

/**
 * Subcribes property from P&S and listen property changes
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 5.0
 */
NONSHARABLE_CLASS( CXnPropertySubscriber ) : public CActive
    {
public:
    /**
     * Cancel and destroy
     */
    ~CXnPropertySubscriber();

    /**
    * Two-phased constructor.
    *
    * @param aUid P&S property category
    * @param aKey P&S property key
    * @param aObserver Given property change observer
    */
    static CXnPropertySubscriber* NewL(
        const TUid aUid,
        const TUint32 aKey,
        MXnPropertyChangeObserver& aObserver );

    /**
    * Two-phased constructor.
    *
    * @param aUid P&S property category
    * @param aKey P&S property key
    * @param aObserver Given property change observer
    */
    static CXnPropertySubscriber* NewLC(
        const TUid aUid,
        const TUint32 aKey,
        MXnPropertyChangeObserver& aObserver );

private:
    /**
     * C++ constructor
     */
    CXnPropertySubscriber(
        MXnPropertyChangeObserver& aObserver,
        const TUint32 aKey );

    /**
     * Second-phase constructor
     */
    void ConstructL( const TUid aUid, const TUint32 aKey );

private:
    /**
     * From CActive.
     * Handle completion
     */
    void RunL();

    /**
     * From CActive.
     * How to cancel me
     */
    void DoCancel();

private:
    // Data

    /**
     * Observer for property changes.
     * Not own
     */
    MXnPropertyChangeObserver& iPropertyChangeObserver;

    /**
     * Property to listen
     */
    RProperty iProperty;

    /**
     * P&S property key
     */
    TUint32 iKey;
    };

#endif
