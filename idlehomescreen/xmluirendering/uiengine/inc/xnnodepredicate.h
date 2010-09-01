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
* Description:  Node predicate header
*
*/


#ifndef XNNODEPREDICATE_H
#define XNNODEPREDICATE_H

// System includes
#include <e32base.h>

// forward declarations
class CXnNode;

// Class declarations

/**
 *  Compares two objects.
 *
 *  @ingroup group_xnlayoutengine 
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
class MXnNodePredicate
    {
public:
    // Constructors and destructor

    /**
     * C++ default constructor.
     */
    IMPORT_C MXnNodePredicate();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~MXnNodePredicate();

public:
    // New functions
    
    /**
     * Tests whether an object matches the predicate
     * 
     * @since Series 60 3.1
     * @param aItem Object to test
     * @return ETrue if matches, EFalse if not
     */
    IMPORT_C virtual TBool Match( CXnNode& aItem );
    };

#endif      // XNNODEPREDICATE_H

