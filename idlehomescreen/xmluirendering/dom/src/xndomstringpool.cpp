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
* Description:  Represent string pool for dom strings
*
*/



// INCLUDE FILES
#include    "xndomstringpool.h"


// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnDomStringPool::CXnDomStringPool
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomStringPool::CXnDomStringPool()
    {
    iStringPoolOffsetCurrent = iStringPoolOffsetNext = 0;
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomStringPool::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnDomStringPool* CXnDomStringPool::NewL()
    {
    CXnDomStringPool* self = new( ELeave ) CXnDomStringPool;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }    


// -----------------------------------------------------------------------------
// CXnDomStringPool::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
CXnDomStringPool* CXnDomStringPool::NewL( RReadStream& aStream )
    {
    CXnDomStringPool* self = new( ELeave ) CXnDomStringPool;
    CleanupStack::PushL( self );
    aStream >> *self;
    CleanupStack::Pop(self);

    return self;
    }    
    
// -----------------------------------------------------------------------------
// CXnDomStringPool::~CXnDomStringPool
// Destructor
// -----------------------------------------------------------------------------
//
CXnDomStringPool::~CXnDomStringPool()
    {
    iStringPool.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::CloneL
// -----------------------------------------------------------------------------
//
CXnDomStringPool* CXnDomStringPool::CloneL()
    {
    CXnDomStringPool* clone = CXnDomStringPool::NewL();
    CleanupStack::PushL( clone );
    
    TInt count( iStringPool.Count() );
    for ( TInt i=0; i<count; i++ )
        {
        HBufC8* tmp = iStringPool[i]->Des().AllocLC();
        clone->iStringPool.AppendL( tmp );
        CleanupStack::Pop( tmp );
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AddStringL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnDomStringPool::AddStringL( const TDesC8& aString )
    {
    TBool found( EFalse );
    TInt index( 0 );
     
    TInt count( iStringPool.Count() );
    for (; index < count && !found;  )
        {
        if ( iStringPool[ index ]->Des().Compare( aString ) == 0 )
            {
            found = ETrue;
            }
        else
            {
            index++;
            }    
        }
    if ( !found )
        {
        HBufC8* tmp = aString.AllocLC();
        iStringPool.AppendL( tmp );
        CleanupStack::Pop( tmp );
        index = iStringPool.Count()-1;    //Last item
        iStringPoolOffsetNext++;
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::String
// -----------------------------------------------------------------------------
//
const TDesC8& CXnDomStringPool::String( const TInt aStringRef )
    {
    return (*iStringPool[ aStringRef ]);
    }
        
// -----------------------------------------------------------------------------
// CXnDomStringPool::Size
// -----------------------------------------------------------------------------
//
TInt CXnDomStringPool::Size() const
    {
    TInt size( 0 );
    
    TInt count( iStringPool.Count() );
    for ( TInt i=0; i<count; i++ )
        {
        size += sizeof(TInt16);     //Length
        size++;                     //HBufC control mark
        size++;                     //HBufC control mark
        size += iStringPool[i]->Size();  //Buffer sixe in bytes     
        }
    return size;    
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::ExternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomStringPool::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt count( iStringPool.Count() );
    aStream.WriteInt16L( count );
    
    for ( TInt i=0; i<count; i++ )
        {
        aStream.WriteInt16L( iStringPool[i]->Length() );
        aStream << *iStringPool[i];      
        }
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::InternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomStringPool::InternalizeL( RReadStream& aStream )
    {
    TInt len(0);
    TInt16 count ( aStream.ReadInt16L() );
    
    iStringPoolOffsetCurrent = iStringPoolOffsetNext;
    iStringPoolOffsetNext += count;

    for ( TInt i=0; i<count; i++ )
        {
        len = aStream.ReadInt16L();
        HBufC8* tmp = HBufC8::NewLC( aStream, len );
        iStringPool.AppendL( tmp );
        CleanupStack::Pop( tmp );
        }
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::Offset
// -----------------------------------------------------------------------------
//
TUint CXnDomStringPool::Offset() const
    {
    return iStringPoolOffsetCurrent;
    }

//  End of File  
