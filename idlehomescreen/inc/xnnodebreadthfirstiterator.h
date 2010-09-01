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
* Description:  Iterates over a tree, breadth first
*
*/

#ifndef XNNODEBREADTHFIRSTITERATOR_H
#define XNNODEBREADTHFIRSTITERATOR_H

#include <e32base.h>
#include <e32cmn.h>
#include "xnchilditerator.h"

// Constants
/** 
 * Queue granularity value
 */
const TInt KMemoryAloc = 128;

template< class T > class CXnNodeBreadthFirstIterator : public CBase,
    public MXnChildIterator
    {
public:

    /**
    * Two-phased constructor.
    * @param aStartNode Object to iterate over
    */
    static CXnNodeBreadthFirstIterator< T >* NewL( T& aStartNode );

    /**
    * Destructor.
    */
    virtual ~CXnNodeBreadthFirstIterator();

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
    CXnNodeBreadthFirstIterator( T* aStartNode );

    /**
    * 2nd phase constructor. 
    */
    void ConstructL();

private:   
    // Queue
    RPointerArray< T > iQueue;
    // Object to iterate over
    T* iStart;   
    // Current object
    T* iCurrent;
    };


// -----------------------------------------------------------------------------
// CXnNodeBreadthFirstIterator< T >::NewL()
// -----------------------------------------------------------------------------
//
template< class T > CXnNodeBreadthFirstIterator< T >* 
    CXnNodeBreadthFirstIterator< T >::NewL( T& aStartNode )
    {
    CXnNodeBreadthFirstIterator< T >* p = 
        new ( ELeave )CXnNodeBreadthFirstIterator< T >( &aStartNode );
    CleanupStack::PushL( p );
    p->ConstructL();
    CleanupStack::Pop();
    return p;
    }

// -----------------------------------------------------------------------------
// CXnNodeBreadthFirstIterator< T >::CXnNodeBreadthFirstIterator()
// C++ default constructor
// -----------------------------------------------------------------------------
//
template< class T >
    CXnNodeBreadthFirstIterator < T >::CXnNodeBreadthFirstIterator(
    T* aStartNode ): iStart( aStartNode ), iCurrent( aStartNode ),
    iQueue( KMemoryAloc )
    {
    }

// -----------------------------------------------------------------------------
//  CXnNodeBreadthFirstIterator< T >::~CXnNodeBreadthFirstIterator()
//  C++ default destructor.     
// -----------------------------------------------------------------------------
//     
template< class T >
    CXnNodeBreadthFirstIterator< T >::~CXnNodeBreadthFirstIterator()
    {
    iQueue.Close();
    }

// -----------------------------------------------------------------------------
// CXnNodeBreadthFirstIterator< T >::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
template< class T > void CXnNodeBreadthFirstIterator< T >::ConstructL()
    {
    iQueue.AppendL( iStart );
    }

// -----------------------------------------------------------------------------
// CXnNodeBreadthFirstIterator< T >::NextL()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnNodeBreadthFirstIterator< T >::NextL()
    {
    if( iQueue.Count() )
        {
        // Dequeue
        iCurrent = iQueue[0];
        iQueue.Remove(0);

        RPointerArray< T >& currentChildNodes = iCurrent->Children();
        TInt currentChildCount( currentChildNodes.Count() );

        // Enqueue
        for( TInt i = 0; i < currentChildCount; ++i )
            {
            iQueue.AppendL( currentChildNodes[i] );
            }
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
// CXnNodeBreadthFirstIterator< T >::Value()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnNodeBreadthFirstIterator< T >::Value()
    {
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// CXnNodeBreadthFirstIterator< T >::PreviousL()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnNodeBreadthFirstIterator< T >::PreviousL()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnNodeBreadthFirstIterator< T >::Index()
// -----------------------------------------------------------------------------
//
template< class T > TUint CXnNodeBreadthFirstIterator< T >::Index() const
    {
    return 0;
    }

#endif    //CXNNODEBREADTHFIRSTITERATOR_H

// End of File