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
* Description:  Depth iterator walks the dom tree using depth first principle.
*
*/



// INCLUDE FILES
#include    "xndomdepthiterator.h"
#include    "xndomlist.h"
#include    "xndomnode.h"


// Queue granularity value
/**
*/
const TInt KMemoryAloc = 64;

// ============================ LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDomDepthIterator::CXnDomDepthIterator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomDepthIterator::CXnDomDepthIterator( CXnDomNode* aRootNode ):
    iStack( KMemoryAloc ), iStart( aRootNode ), iCurrent( aRootNode )
    {
    }

// -----------------------------------------------------------------------------
// CXnDomDepthIterator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomDepthIterator::ConstructL()
    {
      iStack.AppendL( iStart );
    }

// -----------------------------------------------------------------------------
// CXnDomDepthIterator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDepthIterator* CXnDomDepthIterator::NewL( CXnDomNode& aRootNode )
    {
    CXnDomDepthIterator* self = new( ELeave ) CXnDomDepthIterator( &aRootNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnDomDepthIterator::~CXnDomDepthIterator()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDepthIterator::~CXnDomDepthIterator()
    {
     iStack.Close();
    }

// -----------------------------------------------------------------------------
// CXnDomDepthIterator::First
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomDepthIterator::First()
    {
    return iStart;
    }
    
// -----------------------------------------------------------------------------
// CXnDomDepthIterator::Next
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomDepthIterator::NextL()
    {
    if( iStack.Count() )
        {
        // Pop
        TInt topIndex( iStack.Count() - 1 );
        iCurrent = iStack[topIndex];
        iStack.Remove( topIndex );
        
        CXnDomList& currentChildNodes( iCurrent->ChildNodes() );
        TInt currentChildCount( currentChildNodes.Length() );
       
        // Push left most child to top
        for( TInt i = currentChildCount - 1; i >= 0; --i )
            {
            iStack.AppendL( static_cast< CXnDomNode* >(
                    currentChildNodes.Item( i ) ) );
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

/*
// -----------------------------------------------------------------------------
// CXnDomDepthIterator::PushL
// -----------------------------------------------------------------------------
//
void CXnDomDepthIterator::PushL( CXnDomNode& aNode )
    {
    iDepthLevel.AppendL( &aNode );
    }
*/
// -----------------------------------------------------------------------------
// CXnDomDepthIterator::Pop
// -----------------------------------------------------------------------------
//
/*
CXnDomNode* CXnDomDepthIterator::Pop()
    {
    CXnDomNode* pop = NULL;
    TInt count( iDepthLevel.Count() );
    if ( count )
        {
        pop = iDepthLevel[ count-1 ];
        iDepthLevel.Remove( count-1 );
        }
    return pop;    
    }
*/     
    
//  End of File  
