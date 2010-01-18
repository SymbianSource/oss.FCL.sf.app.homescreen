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
#include    "xndomstringpooloptimizer.h"

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnDomStringPool::CXnDomStringPool
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomStringPool::CXnDomStringPool( const TBool aAllowDuplicates ) :
    iAllowDuplicates( aAllowDuplicates )
    {
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
CXnDomStringPool* CXnDomStringPool::NewL( const TBool aAllowDuplicates )
    {
    CXnDomStringPool* self =
            new( ELeave ) CXnDomStringPool( aAllowDuplicates );
    
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
CXnDomStringPool* CXnDomStringPool::NewL( RReadStream& aStream,
        const TBool aAllowDuplicates )
    {
    CXnDomStringPool* self = new( ELeave ) CXnDomStringPool( aAllowDuplicates );
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
    iStringPoolOptimizer.Close();
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::CloneL
// -----------------------------------------------------------------------------
//
CXnDomStringPool* CXnDomStringPool::CloneL()
    {    
    CXnDomStringPool* clone = NULL;    
    if( iAllowDuplicates )
        {
        clone = CXnDomStringPool::NewL( ETrue );
        }
    else
        {
        clone = CXnDomStringPool::NewL( EFalse );
        }    
    CleanupStack::PushL( clone );
    
    TInt count( iStringPool.Count() );
    for ( TInt i = 0; i < count; i++ )
        {    
        HBufC8* tmp = iStringPool[i]->Des().AllocLC();
        clone->DoAddStringL( tmp );
        CleanupStack::Pop( tmp );
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AddStringL
// -----------------------------------------------------------------------------
//
TInt CXnDomStringPool::AddStringL( const TDesC8& aString )
    {   
    TInt index = iStringPoolOptimizer.GetIndex( aString );

    if( index == KErrNotFound )
        {
        HBufC8* string = aString.AllocLC();
        index = DoAddStringL( string );        
        CleanupStack::Pop( string );        
        }    
    
    return index;    
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AddStringL
// -----------------------------------------------------------------------------
//
TInt CXnDomStringPool::AddStringL( HBufC8* aString )
    {   
    if( !aString )
        {
        User::Leave( KErrArgument );
        }
    
    TInt index = iStringPoolOptimizer.GetIndex( *aString );    
    
    if( index == KErrNotFound )
        {
        index = DoAddStringL( aString );       
        }
    else
        {
        delete aString;
        }
    
    return index;    
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AddStringL
// -----------------------------------------------------------------------------
//
void CXnDomStringPool::AddAllL( CXnDomStringPool& aStringPool )
    {
    const TInt count = aStringPool.Count();
    for( TInt i = 0; i < count; i++ )
        {
        AddStringL( aStringPool.String( i ) );
        }    
    }

// -----------------------------------------------------------------------------
// CXnDomNode::String
// -----------------------------------------------------------------------------
//
const TDesC8& CXnDomStringPool::String( const TInt aStringRef )
    {       
    if( aStringRef >= 0 && aStringRef < iStringPool.Count() )
        {
        return (*iStringPool[ aStringRef ]);
        }
    else
        {
        return KNullDesC8;
        }
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
// CXnDomStringPool::Count
// -----------------------------------------------------------------------------
//
TInt CXnDomStringPool::Count() const
    {
    return iStringPool.Count();
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

    for ( TInt i=0; i<count; i++ )
        {
        len = aStream.ReadInt16L();
        HBufC8* tmp = HBufC8::NewLC( aStream, len );
        AddStringL( tmp ); // OWNERSHIP TRANSFERRED!        
        CleanupStack::Pop( tmp );
        }
    }

// -----------------------------------------------------------------------------
// CXnDomStringPool::DoAddStringL
// -----------------------------------------------------------------------------
//
TInt CXnDomStringPool::DoAddStringL( HBufC8* aNewString )
    {
    if( !aNewString )
        {
        User::Leave( KErrArgument );
        }       
    
    TInt index = iStringPool.Count();
    
    if( !iAllowDuplicates )
        {
        TXnDomStringPoolOptimizerEntry tmp( index, *aNewString );
        iStringPoolOptimizer.AddEntryL( tmp );
        }
    
    iStringPool.AppendL( aNewString );       
        
    return index;
    }

//  End of File  
