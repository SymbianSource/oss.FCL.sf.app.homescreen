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
* Description:  DT iterator.
*
*/



// INCLUDE FILES
//#include    "xndomlist.h"
#include    "xnnodeappif.h"

#include    "pmodtiterator.h"

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Finds next sibling.
// Returns NULL if there is no siblings left or there is no parent i.e this is a
// root node
// Returns: CXnDomNode* Pointer to the node or NULL
//          
// -----------------------------------------------------------------------------
//

static CXnNodeAppIf* NextSiblingL( CXnNodeAppIf& aNode )
    {
    CXnNodeAppIf* left( NULL );
    CXnNodeAppIf* parent( NULL );
    TRAP_IGNORE( parent = aNode.ParentL() );
    if ( parent )
        {
        RPointerArray<CXnNodeAppIf> nodeList;
        TRAP_IGNORE( nodeList = parent->ChildrenL() );
        CleanupClosePushL( nodeList );
        TInt currentIndex( nodeList.Find( &aNode ) );
        TInt nextIndex( ++currentIndex );
        TInt length( nodeList.Count() );
        if ( nextIndex < length )  
            {
            left = nodeList[ nextIndex ];
            }
        CleanupStack::PopAndDestroy( &nodeList );
        }
    return left;     
    }
    
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPmODTIterator::CPmODTIterator( CXnNodeAppIf& aRootNode  ):
    iFirst(&aRootNode), iCurrent(&aRootNode)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CPmODTIterator::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CPmODTIterator* CPmODTIterator::NewL( CXnNodeAppIf& aRootNode )
    {
    CPmODTIterator* self = new( ELeave ) CPmODTIterator( aRootNode );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPmODTIterator::~CPmODTIterator()
    {
    iDepthLevel.Close();
    }

// -----------------------------------------------------------------------------
// First node
// -----------------------------------------------------------------------------
//
CXnNodeAppIf* CPmODTIterator::First()
    {
    return iFirst;
    }
    
// -----------------------------------------------------------------------------
// Next node
// -----------------------------------------------------------------------------
//
CXnNodeAppIf* CPmODTIterator::NextL()
    {
    CXnNodeAppIf* firstChild( NULL );     
    
    //Initialise iCurrent if the whole tree has been walked through 
    if (!iCurrent) 
        {
        iCurrent = iFirst;
        }
    RPointerArray<CXnNodeAppIf> children = iCurrent->ChildrenL();
    if( children.Count() > 0 )
        {
        firstChild = children[0];
        }
    children.Close();
        
    // Current node has childs left
    if (firstChild)
        {
        //Keep count of depth level
        PushL( *iCurrent );
        iCurrent = firstChild;
        }
    else //If current node has siblings left
        {
        SkipBranchL();
        }
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// SkipBranch
// -----------------------------------------------------------------------------
//
CXnNodeAppIf* CPmODTIterator::SkipBranchL()
    {
    CXnNodeAppIf* sibling = NextSiblingL(*iCurrent);
    if(sibling)
        {
        iCurrent = sibling;    
        }
    else //Current node don't have any childs or siblings left
        {
        // Reverse the tree by moving up in hierarchy
        // Move up one level in hierarchy and check if siblings exists or we are in a 
        // root level
        TBool stop( EFalse );
        while( !stop && iDepthLevel.Count() )
            {
            iCurrent = Pop();                   //Reach the previous level
            sibling = NextSiblingL( *iCurrent ); //Check if siblings exist
            if ( sibling && iDepthLevel.Count() )
                {
                iCurrent = sibling;
                stop = ETrue;
                }
            else                                //We didn't find any siblings
                {
                iCurrent = NULL;
                }
            }
        }
    return iCurrent;
    }
    
// -----------------------------------------------------------------------------
// Push node into stack.
// -----------------------------------------------------------------------------
//
void CPmODTIterator::PushL( CXnNodeAppIf& aNode )
    {
    iDepthLevel.AppendL( &aNode );
    }

// -----------------------------------------------------------------------------
// Pop node from stack.
// -----------------------------------------------------------------------------
//
CXnNodeAppIf* CPmODTIterator::Pop()
    {
    CXnNodeAppIf* pop = NULL;
    TInt count( iDepthLevel.Count() );
    if ( count )
        {
        pop = iDepthLevel[ count-1 ];
        iDepthLevel.Remove( count-1 );
        }
    return pop;    
    }
