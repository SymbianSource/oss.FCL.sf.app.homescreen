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
* Description:  Iterates over a tree, breadth first search algorithm
*
*/


#ifndef XNBREADTHFIRSTTREEITERATOR_H
#define XNBREADTHFIRSTTREEITERATOR_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>
#include "xnchilditerator.h"


// Constants
/** 
 * Queue granularity value
 */
const TInt KMemoryAlocQ = 64;

template < class T >
    class CXnBreadthFirstTreeIterator: public CBase, public MXnChildIterator
    {
public:

    /**
     * Two-phased constructor.
     * @param aStartNode object to iterate over
     * @param aDepth with default parameter value 0.
     */
    static CXnBreadthFirstTreeIterator< T >* NewL( T& aStartNode,
        TUint aDepth = 0 );

    /**
     * Destructor.
     */
    virtual ~CXnBreadthFirstTreeIterator();

    /**
     * Get the next iterator value.
     * @return Next object or NULL if no more values. 
     */
    T* NextL();

    /**
     * Get the current iterator value.
     * @return Current object 
     */     
    T* Value();

    /**
     * Get the previous iterator value. Obsolete!
     * @return NULL 
     */
    T* PreviousL();

    /**
     * Get the current iterator index. Obsolete!
     * @return value 0
     */
    TUint Index() const;

protected:
    
    /**
     * C++ default constructor. 
     */
    CXnBreadthFirstTreeIterator( T* aStartNode, TUint aDepth = 0 );

    /**
     * 2nd phase constructor. 
     */
    void ConstructL();

private: //Data
    // Queue
    RPointerArray< T > iQueue;
    // Object to iterate over
    T* iStart;
    // Current object
    T* iCurrent;
    // Tree depth
    TUint iDepth;
    // Current index 
    TUint iIndex;
    };



// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::NewL()
// -----------------------------------------------------------------------------
//
template< class T > CXnBreadthFirstTreeIterator< T >*
    CXnBreadthFirstTreeIterator< T >::NewL( T& aStartNode, TUint aDepth )
    {
    CXnBreadthFirstTreeIterator< T >* p = 
        new ( ELeave )CXnBreadthFirstTreeIterator< T >( &aStartNode,
            aDepth );
    CleanupStack::PushL( p );
    p->ConstructL();
    CleanupStack::Pop();
    return p;
    }


// -----------------------------------------------------------------------------
// XnBreadthFirstTreeIterator< T >::CXnBreadthFirstTreeIterator()
// C++ default constructor
// -----------------------------------------------------------------------------
//
template< class T >
    CXnBreadthFirstTreeIterator< T >::CXnBreadthFirstTreeIterator(
    T* aStartNode, TUint aDepth ) : iQueue( KMemoryAlocQ ), iStart( 
    aStartNode ),iCurrent( aStartNode ),iDepth( aDepth )
    {

    }

// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::~CXnBreadthFirstTreeIterator()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
template< class T >
    CXnBreadthFirstTreeIterator< T >::~CXnBreadthFirstTreeIterator()
    { 
    iQueue.Close();
    }

// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
template< class T >
    void CXnBreadthFirstTreeIterator< T >::ConstructL()
    {
    iQueue.AppendL( iStart );
    iIndex = 0; 
    }

// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::NextL()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnBreadthFirstTreeIterator< T >::NextL()
    { 
    if( iQueue.Count() )
        {
        // Dequeue
        if ( !iDepth )
            {
            iCurrent = iQueue[0];
            iQueue.Remove(0);
            }
        
        RPointerArray< T > currentChildNodes = iCurrent->ChildrenL();
        TInt currentChildCount( currentChildNodes.Count() );
                
        if ( iDepth )
            {
             if ( (iIndex + 1) < currentChildCount )
                 {
                     for ( TUint tmp = iIndex; (tmp + 1) < currentChildCount; )
                         {
                         T* tmpObject = currentChildNodes[++tmp];
                         iIndex = tmp;
                         currentChildNodes.Reset(); 
                         return tmpObject;
                         }
                  }
             else
                 {
                 currentChildNodes.Reset();
                 return NULL;
                  }
            }      
        // Enqueue
        for( TInt i = 0; i < currentChildCount; ++i )
            {
            iQueue.AppendL( currentChildNodes[i] );
            }
        currentChildNodes.Reset();
        }
    else
        {
        iCurrent = NULL;
        iQueue.Reset();
        iQueue.AppendL( iStart );
        }
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::Value()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnBreadthFirstTreeIterator< T >::Value()
    {
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::PreviousL()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnBreadthFirstTreeIterator< T >::PreviousL()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnBreadthFirstTreeIterator< T >::Index()
// -----------------------------------------------------------------------------
//
template< class T > TUint CXnBreadthFirstTreeIterator< T >::Index() const
    {
    return 0;
    }

#endif    // CXNBREADTHFIRSTTREEITERATOR_H

// End of File