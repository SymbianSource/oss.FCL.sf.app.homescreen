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
* Description:  Subscribes properties from P&S
 *
*/

#ifndef MMPROPERTYSUBSCRIBER_H
#define MMPROPERTYSUBSCRIBER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32property.h>
// Observer for property change. Currently integer value supported
class MMmPropertyChangeObserver
    {
public:    
    virtual void PropertyChangedL(TInt aValue)=0;
    };

/**
*  Subcribes property from P&S and listen property changes
* 
*  @lib xn3layoutengine.lib
*  @since Series 60 5.0
*/
NONSHARABLE_CLASS( CMmPropertySubscriber ) : public CActive
    {
public:
    // Cancel and destroy
    ~CMmPropertySubscriber();

    /**
    * Two-phased constructor.
    * @param aUid P&S property category
    * @param aKey P&S property key
    * @param aObserver Given property change observer
    */
    static CMmPropertySubscriber* NewL(
            const TUid aUid, 
            const TUint32 aKey,
            MMmPropertyChangeObserver& aObserver);

    /**
    * Two-phased constructor.
    * @param aUid P&S property category
    * @param aKey P&S property key
    * @param aObserver Given property change observer
    */
    static CMmPropertySubscriber* NewLC(
            const TUid aUid, 
            const TUint32 aKey,
            MMmPropertyChangeObserver& aObserver);

private:
    // C++ constructor
    CMmPropertySubscriber(MMmPropertyChangeObserver& aObserver);

    // Second-phase constructor
    void ConstructL(const TUid aUid,const TUint32 aKey);

private: // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();
    
private:
    //Observer for property changes
    MMmPropertyChangeObserver& iPropertyChangeObserver;
    // Property to listen
    RProperty iProperty;
    };

#endif // MMPROPERTYSUBSCRIBER_H
