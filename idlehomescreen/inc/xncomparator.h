/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MXNCOMPARATOR_H
#define MXNCOMPARATOR_H

//  INCLUDES
#include <e32base.h>

/**
* @ingroup group_xnutils
* 
*  Compares two objects.
*  @since Series 60 3.1
*/
class MXnComparator
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MXnComparator() {};
        
    public: // New functions
        
        /**
        * Compares two objects for equality.
        * @since Series 60 3.1
        * @param aItem1 First item
        * @param aItem1 Second item
        * @return ETrue if equal, EFalse if not equal
        */
		    virtual TBool Equals(CBase& aItem1, CBase& aItem2) = 0;
    };

#endif      // MXnCOMPARATOR_H
            
// End of File
