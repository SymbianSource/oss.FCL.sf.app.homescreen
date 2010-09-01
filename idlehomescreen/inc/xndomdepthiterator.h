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
* Description:  Depth iterator walks the dom tree by depth first.
*
*/



#ifndef XN_DOM_DEPTH_ITERATOR_H
#define XN_DOM_DEPTH_ITERATOR_H

//  INCLUDES
#include <e32base.h>
#include "mxndomiterator.h"


// FORWARD DECLARATIONS
class CXnDomNode;


// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Depth iterator walks through the dom tree using the depth first
*  principle.
*
*  @lib xndomdocument.lib
*/
class CXnDomDepthIterator: public CBase, public MXnDomIterator
    {
public:
    /**
     * Two-phased constructor.
     * @param aStartNode object to iterate over
     */
    IMPORT_C static CXnDomDepthIterator* NewL( CXnDomNode& aStartNode );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnDomDepthIterator();

    /**
     * Documented in MXnDomIterator::First
     */
    IMPORT_C CXnDomNode* First();

    /**
     * Documented in MXnDomIterator::NextL
     */
    IMPORT_C CXnDomNode* NextL();

private:
    /**
     * C++ default constructor
     */
    CXnDomDepthIterator( CXnDomNode* aRootNode );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Push node to stack.
     * @param aNode Node to push 
     */
    //void PushL( CXnDomNode& aNode );

    /**
     * Pop node from the stack.
     * @return Pointer to node
     */
    //CXnDomNode* Pop();

private:
    // Follow tree hierarchy with stack
    RPointerArray< CXnDomNode > iStack;   
    // First node
    CXnDomNode* iStart;
    //Current node
    CXnDomNode* iCurrent;
    };

#endif    // XN_DOM_DEPTH_ITERATOR_H  

// End of File
