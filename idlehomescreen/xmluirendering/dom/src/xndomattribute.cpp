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
* Description:  Represent single xmluiml attribute
*
*/



// INCLUDE FILES
#include    "xndomattribute.h"
#include    "xndomstringpool.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnDomAttribute::CXnDomAttribute
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomAttribute::CXnDomAttribute( CXnDomStringPool* aStringPool ):
    iStringPool( aStringPool ),
    iNameRef( KErrNotFound ),
    iValueRef( KErrNotFound )
    {
    }


// -----------------------------------------------------------------------------
// CXnDomAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomAttribute::ConstructL( const TDesC8& aName )
    {
    iNameRef = iStringPool->AddStringL( aName );
    }
// -----------------------------------------------------------------------------
// CXnDomAttribute::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomAttribute* CXnDomAttribute::NewL( 
    const TDesC8& aName,
    CXnDomStringPool* aStringPool )
    {
    CXnDomAttribute* self = new( ELeave ) CXnDomAttribute( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );

    return self;
    }    


// -----------------------------------------------------------------------------
// CXnDomAttribute::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
CXnDomAttribute* CXnDomAttribute::NewL( 
    RReadStream& aStream, 
    CXnDomStringPool* aStringPool )
    {
    CXnDomAttribute* self = new( ELeave ) CXnDomAttribute( aStringPool );
    CleanupStack::PushL( self );
    aStream >> *self;
    CleanupStack::Pop( self );

    return self;
    }    
    
// -----------------------------------------------------------------------------
// CXnDomAttribute::~CXnDomAttribute
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomAttribute::~CXnDomAttribute()
    {
    }

// -----------------------------------------------------------------------------
// CXnDomAttribute::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomAttribute* CXnDomAttribute::CloneL()
    {
    CXnDomAttribute* clone = new (ELeave)CXnDomAttribute( iStringPool );
    clone->iNameRef = iNameRef;
    clone->iValueRef = iValueRef;
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomAttribute::CloneL
// -----------------------------------------------------------------------------
//
CXnDomAttribute* CXnDomAttribute::CloneL( CXnDomStringPool& aStringPool )
    {
    const TDesC8& name = iStringPool->String( iNameRef );
    
    CXnDomAttribute* clone = CXnDomAttribute::NewL( name, &aStringPool );
    CleanupStack::PushL( clone );
    if ( iValueRef > KErrNotFound )
        {
        const TDesC8& value = iStringPool->String( iValueRef );
        clone->SetValueL( value );
        }
    CleanupStack::Pop( clone );    
    return clone;
    
    }
    
// -----------------------------------------------------------------------------
// CXnDomAttribute::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomAttribute::Name()
    {
    return iStringPool->String( iNameRef );
    }
    
// -----------------------------------------------------------------------------
// CXnDomProperty::NameStringPoolIndex
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt16 CXnDomAttribute::NameStringPoolIndex()const
    {
    return iNameRef;    
    }    
// -----------------------------------------------------------------------------
// CXnDomAttribute::Value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomAttribute::Value()
    {
    if ( iValueRef > KErrNotFound )
        {
        return iStringPool->String( iValueRef );
        }
    return KNullDesC8;
    }
    
// -----------------------------------------------------------------------------
// CXnDomProperty::ValueStringPoolIndex
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt16 CXnDomAttribute::ValueStringPoolIndex()const
    {
    return iValueRef;    
    }    

// -----------------------------------------------------------------------------
// CXnDomAttribute::SetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomAttribute::SetValueL( const TDesC8& aValue )
    {
    iValueRef = iStringPool->AddStringL( aValue );
    }

// -----------------------------------------------------------------------------
// CXnDomAttribute::SwapStringPoolL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomAttribute::SwapStringPoolL( CXnDomStringPool* aStringPool )
    {
    if( !aStringPool )
        {
        User::Leave( KErrArgument );
        }
    
    iNameRef = aStringPool->AddStringL( iStringPool->String( iNameRef ) );
    iValueRef = aStringPool->AddStringL( iStringPool->String( iValueRef ) );
    
    iStringPool = aStringPool;
    }
   
// -----------------------------------------------------------------------------
// CXnDomAttribute::Size
// -----------------------------------------------------------------------------
//
TInt CXnDomAttribute::Size() const
    {
    TInt size( 0 );
    
    size += sizeof(TInt16); // iNameRef     
      
    size += sizeof(TInt16); // iValueRef
    
    return size;    
    }

// -----------------------------------------------------------------------------
// CXnDomAttribute::ExternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomAttribute::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( iNameRef );
    aStream.WriteInt16L( iValueRef );
    }

// -----------------------------------------------------------------------------
// CXnDomAttribute::InternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomAttribute::InternalizeL( RReadStream& aStream )
    {
    iNameRef = aStream.ReadInt16L();
    iValueRef = aStream.ReadInt16L();
    }
//  End of File  
