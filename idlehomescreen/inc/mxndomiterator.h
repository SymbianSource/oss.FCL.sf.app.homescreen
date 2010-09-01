/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class for dom iterators
*
*/



#ifndef MXN_DOM_ITERATOR_H
#define MXN_DOM_ITERATOR_H


// FORWARD DECLARATIONS
class CXnDomNode;


// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Mixin class to let us define different iterators for 
*  different traversal policies.
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class MXnDomIterator
    {
   public:  // Destructor
        
        /**
        * Destructor.
        */
        virtual ~MXnDomIterator(){}

    public: // Adding and removing
    	
    	/**
        * Get first i.e root node.
        * @since Series 60 3.1
        */
        virtual CXnDomNode* First()=0;
    	/**
        * Get next node.
        * @since Series 60 3.1
        */
        virtual CXnDomNode* NextL()=0;
        
    };

#endif      // MXN_DOM_ITERATOR_H  
            
// End of File
