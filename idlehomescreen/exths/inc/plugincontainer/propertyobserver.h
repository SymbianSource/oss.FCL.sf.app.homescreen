/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPROPERTYOBSERVER_H
#define CPROPERTYOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// CLASS DECLARATION

/**
 * MPropertyChangeHandler.
 * Abstract interface for handling property change events.
 **/
NONSHARABLE_CLASS( MPropertyChangeHandler )
{
 public:
    /**
     * This is a callback function which is called when a property is changed.
     *
     * @param aCategory  UID of the category.
     * @param aKey       changed key
     * @param aValue     the new value
     *
     * Note! references aKey and aValue are only valid while executing
     * HandlePropertyChange(). After that the data in which they refer can change.
     **/
    virtual void HandlePropertyChange(const TUid& aCategory,
                                      const TUint& aKey,
                                      const TInt& aValue) = 0;
};


// CLASS DECLARATION

/**
 * ?one_line_short_description.
 * ?other_description_lines
 *
 * @lib ?
 * @since Series 60 3.0
 **/
NONSHARABLE_CLASS(CPropertyObserver) : public CActive
{
 public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     * @since Series 60 3.0
     * @param MPropertyChangeHandler aHandler
     * @param aCategory
     * @param aKey
     **/
    static CPropertyObserver* NewL(MPropertyChangeHandler& aHandler,
                                   const TUid& aCategory,
                                   const TUint& aKey);

    /**
     * Destructor.
     **/
    ~CPropertyObserver();

 private:
    /**
     * C++ default constructor overload.
     **/
    CPropertyObserver(MPropertyChangeHandler& aHandler,
                      const TUid& aCategory,
                      const TUint& aKey);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void ConstructL();

 protected:  // Functions from base classes
    /**
     * From CActive
     **/
    void RunL();

    /**
     * From CActive
     **/
    void DoCancel();

 private:  // New functions
    /**
     * @since Series 60 3.0
     **/
    void Subscribe();

    /**
     * @since Series 60 3.0
     **/
    void NotifyCurrentValue();

 private:  // Data
    // Owns
    RProperty iProperty;

    // Observed PubSub category
    const TUid iCategory;

    // Observed key
    const TUint iKey;

    // Refs. Interface towards observants
    MPropertyChangeHandler& iHandler;

};


#endif  // CPROPERTYOBSERVER_H


// End of File
