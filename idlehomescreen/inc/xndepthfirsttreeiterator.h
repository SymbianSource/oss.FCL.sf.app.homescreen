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
* Description:  Iterates over a tree, depth first
*
*/

#ifndef XNDEPTHFIRSTTREEITERATOR_H
#define XNDEPTHFIRSTTREEITERATOR_H

//  INCLUDES
#include <e32base.h>
#include "xnchilditerator.h"

// Constants
/** 
 * Queue granularity value
 */
const TInt KMemoryAlocS = 64;


template < class T >
    class CXnDepthFirstTreeIterator: public CBase, public MXnChildIterator
    {
public:
    /**
    * Two-phased constructor.
    * @param aStartNode Object to iterate over
    */
    static CXnDepthFirstTreeIterator< T >* NewL( T& aStartNode );

    /**
    * Destructor.
    */
    virtual ~CXnDepthFirstTreeIterator();

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

    /**
    * Get the current iterator level in the tree.
    * @return current level
    */
    TUint Level() const;

protected:

    CXnDepthFirstTreeIterator( T* aStartNode );

    void ConstructL();

private:
    //Stack
    RPointerArray< T > iStack;
    //Object to iterate over
    T* iStart;
    // Current object
    T* iCurrent;

    };

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::NewL()
// -----------------------------------------------------------------------------
//
template< class T > CXnDepthFirstTreeIterator< T >*
    CXnDepthFirstTreeIterator<T>::NewL( T& aStartNode )
    {
    CXnDepthFirstTreeIterator<T>* self = 
        new ( ELeave )CXnDepthFirstTreeIterator< T >( &aStartNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::CXnDepthFirstTreeIterator()
// C++ default constructor
// -----------------------------------------------------------------------------
//
template< class T > CXnDepthFirstTreeIterator< T >::CXnDepthFirstTreeIterator(
    T* aStartNode ):
    iStart( aStartNode ),
    iCurrent( aStartNode ),
    iStack( KMemoryAlocS )
    {
    }

// -----------------------------------------------------------------------------
//  CXnDepthFirstTreeIterator< T >::~CXnDepthFirstTreeIterator()
//  C++ default destructor.  
// -----------------------------------------------------------------------------
//  
template< class T >
    CXnDepthFirstTreeIterator< T >::~CXnDepthFirstTreeIterator()
    {
    iStack.Close();
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
template< class T > void CXnDepthFirstTreeIterator< T >::ConstructL()
    {
    iStack.AppendL( iStart );
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::NextL()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnDepthFirstTreeIterator< T >::NextL()
    {
    if( iStack.Count() )
        {
        // Pop
        TInt topIndex( iStack.Count() - 1 );
        iCurrent = iStack[topIndex];
        iStack.Remove( topIndex );
        
        RPointerArray< T >& currentChildNodes( iCurrent->Children() );
        TInt currentChildCount( currentChildNodes.Count() );
       
        // Push left most child to top
        for( TInt i = currentChildCount - 1; i >= 0; --i )
            {
            iStack.AppendL( currentChildNodes[i] );
            }
        }
    else
        {
        iCurrent = NULL;
        iStack.Reset();
        iStack.AppendL( iStart );
        }
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::Value()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnDepthFirstTreeIterator< T >::Value()
    {
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::PreviousL()
// -----------------------------------------------------------------------------
//
template< class T > T* CXnDepthFirstTreeIterator< T >::PreviousL()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::Index()
// -----------------------------------------------------------------------------
//
template< class T > TUint CXnDepthFirstTreeIterator< T >::Index() const
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CXnDepthFirstTreeIterator< T >::Level()
// Get the current iterator level in the tree.
// -----------------------------------------------------------------------------
template< class T > TUint CXnDepthFirstTreeIterator< T >::Level() const
    {
    return iStack.Count();
    }


#endif    // CXNDEPTHFIRSTTREEITERATOR_H

// End of File
