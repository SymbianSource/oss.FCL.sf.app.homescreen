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
* Description:  Compares two objects.
*
*/


#ifndef XNPROPERTYCOMPARATOR_H
#define XNPROPERTYCOMPARATOR_H

// User includes
#include "xncomparator.h"

// Class declaration

/**
 *  Compares two objects.
 *  
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
NONSHARABLE_CLASS( TXnPropertyComparator ) : public MXnComparator
    {
public:
    // Constructors and destructor

    /**
     * C++ default constructor.
     */
    TXnPropertyComparator();

    /**
     * Destructor.
     */
    ~TXnPropertyComparator();

public:
    // Functions from MXnComparator

    /**
     * From MXnComparator.
     * Compares two objects for equality.
     * 
     * @since Series 60 3.1
     * @param aItem1 First item
     * @param aItem1 Second item
     * @return ETrue if equal, EFalse if not equal
     */
    TBool Equals( CBase& aItem1, CBase& aItem2 );
    };

#endif      // XNPROPERTYCOMPARATOR_H
