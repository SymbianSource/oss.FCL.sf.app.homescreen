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
* Description:  Creates an iterator for an object's children
*
*/

#ifndef MXNCHILDITERATORBUILDER_H
#define MXNCHILDITERATORBUILDER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MXnChildIterator;

/**
* @ingroup group_xnutils
* 
*  Creates an iterator for an object's children
*  @since Series 60 3.1
*/
class MXnChildIteratorBuilder
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
    	virtual ~MXnChildIteratorBuilder() {};
        
    public: // New functions
        
        /**
        * Builds a child iterator for an object
        * @since Series 60 3.1
        * @param aObject Object for which the iterator is built
        * @return Child iterator for the object. Ownership is transferred to the caller.
        */
			virtual MXnChildIterator* BuildL(CBase& aObject) = 0;
        
    };

#endif      // MXnCHILDITERATORBUILDER_H
            
// End of File
