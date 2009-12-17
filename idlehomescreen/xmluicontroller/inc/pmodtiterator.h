/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ODT iterator.
*
*/



#ifndef C_PMODTITERATOR_H
#define C_PMODTITERATOR_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CXnNodeAppIf;


// CLASS DECLARATION

/**
*  @ingroup group_xmluicontroller
* 
*  Depth iterator walks through the dom tree using the depth first
*  principle.
*
*  @lib presentationmanager.lib
*  @since S60 v5.0
*/
class CPmODTIterator: public CBase
    {
   public:        
        static CPmODTIterator* NewL( CXnNodeAppIf& aRootNode );
        
        virtual ~CPmODTIterator();

    public:
    	
        /**
         * Returns the first node of the dom tree.
         *
         * @since S60 v5.0
         * @return First node.
         */
        CXnNodeAppIf* First();
    	
    	  /**
         * Returns the next node according to depth first principle.
         *
         * @since S60 v5.0
         * @return Next node.
         */
        CXnNodeAppIf* NextL();
        
        /**
         * Skips the current node branch and returns next sibling
         * or parent's sibling. Returns NULL if no nodes left.
         *
         * @since S60 v5.0
         * @return Sibling node.
         */
        CXnNodeAppIf* SkipBranchL();
        
   private:
        
        CPmODTIterator( CXnNodeAppIf& aRootNode );

        void ConstructL(); 
        
        /**
        * Push node to stack.
        * @param aNode Node to push 
        */
        void PushL( CXnNodeAppIf& aNode );
        
        /**
        * Pop node from the stack.
        * @return Pointer to node
        */
        CXnNodeAppIf* Pop();
        
   private:
        /**
         * Follow tree hierarchy with stack
         * Own.
         */
        RPointerArray<CXnNodeAppIf> iDepthLevel;
        
        /**
         * First node
         * Own.
         */
        CXnNodeAppIf* iFirst;
        
        /**
         * Current node.
         * Own.
         */
        CXnNodeAppIf* iCurrent;
                   
    };

#endif      // C_PMODTITERATOR_H  
