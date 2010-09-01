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
* Description:  Iterates over a containers children
*
*/

#ifndef MXNCHILDITERATOR_H
#define MXNCHILDITERATOR_H

//  INCLUDES
#include <e32base.h>

/**
* @ingroup group_xnutils
* 
*  Iterates over a containers children
*  @since Series 60 3.1
*/
class MXnChildIterator
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
    	virtual ~MXnChildIterator() {};
        
    public: // New functions
        
        /**
        * Get the current iterator value.
        * @since Series 60 3.1
        * @return Current object or NULL if iterator does not point anywhere
        */
			virtual CBase* Value() = 0 ;

        /**
        * Get the next iterator value.
        * @since Series 60 3.1
        * @return Next object or NULL if no more values. If NULL, the iterator is not advanced.
        * Therefore, given that iterator->Value() == x and iterator->Next() == NULL, next invocation
        * of iterator->Value() will return x.
        */
			virtual CBase* NextL() = 0;

        /**
        * Get the previous iterator value.
        * @since Series 60 3.1
        * @return Previous object or NULL if no more values. If NULL, the iterator is not advanced.
        * Therefore, given that iterator->Value() == x and iterator->Previous() == NULL, next invocation
        * of iterator->Value() will return x.
        */
			virtual CBase* PreviousL() = 0;

        /**
        * Get the current iterator index.
        * @since Series 60 3.1
        * @return Current index
        */
			virtual TUint Index() const = 0;
        
    };

#endif      // MXnCHILDITERATOR_H
            
// End of File
