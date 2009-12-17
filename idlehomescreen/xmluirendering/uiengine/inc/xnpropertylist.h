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
* Description:  Property list implementation
*
*/


#ifndef __XNPROPERTYLIST_H__
#define __XNPROPERTYLIST_H__

// System includes
#include <e32base.h>

// User includes
#include "xndomproperty.h"

// Forward declarations
class CXnPropertyComparator;
class CXnMap;
class CXnProperty;

struct CXnPropertyKey : public CBase
    {
    const TDesC8* iString;
    CXnDomProperty::TPseudoClass iPseudoClass;
    };

// Class declaration

/**
 *  Property list
 *
 *  @ingroup group_xnlayoutengine 
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnPropertyList ) : public CBase
    {
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     * 
     * @return Property list object.
     */
    static CXnPropertyList* NewL();

    /**
     * Destructor.
     */
    ~CXnPropertyList();

public:
    // New functions

    /**
     * Set a property. The list will assume ownership of the property. 
     * If an equal property is already in the list, it will be replaced with 
     * the new property and the old property will be destroyed.
     * 
     * @since Series 60 3.1
     * @param aProperty Property to set. 
     */
    void SetPropertyL( CXnProperty* aProperty );

    /**
     * Gets a property. The list will retain ownership of the property, 
     * the caller must not destroy the property. The returned pointer will 
     * become invalid if the property is replaced in the list by a subsequent 
     * SetL().
     * 
     * @since Series 60 3.1
     * @param aProperty Property key
     * @return The property mapped to the key or NULL if not found. 
     */
    CXnProperty* GetProperty( const TDesC8& aKey ) const;
    
    /**
     * Set a state
     * 
     * @since Series 60 3.1
     * @param aState State to set.
     * @return ETrue if pseudoclass found, EFalse otherwise 
     */
    TBool SetStateL( const TDesC8& aState );

    /**
     * Check whether a state is set
     * 
     * @since Series 60 3.1
     * @param aState State to be checked
     * @return ETrue if set, EFalse if not
     */
    TBool IsStateSet( const TDesC8& aState );
    
    /**
     * Unset a state
     * 
     * @since Series 60 3.1
     * @param aState State to unset.
     * @return ETrue if pseudoclass found, EFalse otherwise
     */
    TBool UnsetState( const TDesC8& aState );

    /**
     * Clone the list
     * 
     * @since Series 60 3.1
     * @return Clone of the list
     */
    CXnPropertyList* CloneL();
protected:
    // New functions
    
    /**
     * C++ default constructor.
     */
    CXnPropertyList();

private:
    
    /**
     * Two-phased constructor.
     * @param aComparator Propety comparator.
     */
    void ConstructL();

private:
    // Data
    
    /** Map of property objects. */
    CXnMap* iMap;
    /** Currently used pseudoclasses (states) */
    RArray< CXnDomProperty::TPseudoClass > iCurrentPseudoClasses;
    };

#endif //__XNPROPERTYLIST_H__

