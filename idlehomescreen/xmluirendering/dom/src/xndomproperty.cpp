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
* Description:  Represents a Css property.
*
*/



// INCLUDE FILES
#include    "xndomproperty.h"
#include    "xndompropertyvalue.h"
#include    "xndomlist.h"
#include    "xndomstringpool.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnDomProperty::CXnDomProperty
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomProperty::CXnDomProperty( CXnDomStringPool& aStringPool ):
    iStringPool( aStringPool )
    {
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::CXnDomProperty
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomProperty::CXnDomProperty( 
    TInt16 aStringPoolIndex, 
    CXnDomStringPool& aStringPool ):
    iStringPool( aStringPool ),
    iNameRef( aStringPoolIndex )
    {
    }    
// -----------------------------------------------------------------------------
// CXnDomProperty::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomProperty::ConstructL( const TDesC8& aName )
    {
    iNameRef = iStringPool.AddStringL( aName );
    iPropValList = CXnDomList::NewL( CXnDomList::EPropertyValueList, iStringPool );
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomProperty::ConstructL()
    {
    iPropValList = CXnDomList::NewL( CXnDomList::EPropertyValueList, iStringPool );
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomProperty* CXnDomProperty::NewL( 
    const TDesC8& aName,
    CXnDomStringPool& aStringPool )
    {
    CXnDomProperty* self = new( ELeave ) CXnDomProperty( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
CXnDomProperty* CXnDomProperty::NewL( 
    RReadStream& aStream, 
    CXnDomStringPool& aStringPool )
    {
    CXnDomProperty* self = 
        new( ELeave ) CXnDomProperty( aStringPool );
    
    CleanupStack::PushL( self );
    
    aStream >> *self;
    CleanupStack::Pop( self );

    return self;
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomProperty* CXnDomProperty::NewL( 
    TInt16 aStringPoolIndex,
    CXnDomStringPool& aStringPool )
    {
    CXnDomProperty* self = 
        new (ELeave) CXnDomProperty( aStringPoolIndex, aStringPool );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
// Destructor
EXPORT_C CXnDomProperty::~CXnDomProperty()
    {
    delete iPropValList;
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomProperty* CXnDomProperty::CloneL()
    {
    CXnDomProperty* clone = new (ELeave)CXnDomProperty( iStringPool );
    CleanupStack::PushL(clone);
    
    clone->iPropValList = CXnDomList::NewL( CXnDomList::EPropertyValueList, iStringPool );
    clone->iNameRef = iNameRef;
    
    TInt count( iPropValList->Length() );
    for (TInt i = 0; i < count; ++i)
        {
        CXnDomPropertyValue* val = static_cast<CXnDomPropertyValue*>( iPropValList->Item(i) );
        CXnDomPropertyValue* cloneVal = val->CloneL();
        CleanupStack::PushL( cloneVal );
        clone->iPropValList->AddItemL( cloneVal );
        CleanupStack::Pop( cloneVal );
        }
    clone->iInherited = iInherited;
    clone->iPseudoClass = iPseudoClass;    
    CleanupStack::Pop(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomProperty* CXnDomProperty::CloneL( CXnDomStringPool& aStringPool )
    {
    const TDesC8& name = iStringPool.String( iNameRef );
    CXnDomProperty* clone = CXnDomProperty::NewL( name, aStringPool );
    CleanupStack::PushL(clone);
    
    TInt count( iPropValList->Length() );
    for (TInt i = 0; i < count; ++i)
        {
        CXnDomPropertyValue* val = static_cast<CXnDomPropertyValue*>( iPropValList->Item(i) );
        CXnDomPropertyValue* cloneVal = val->CloneL( aStringPool );
        CleanupStack::PushL( cloneVal );
        clone->iPropValList->AddItemL( cloneVal );
        CleanupStack::Pop( cloneVal );
        }
    clone->iInherited = iInherited;
    clone->iPseudoClass = iPseudoClass;    
    CleanupStack::Pop(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomProperty::Name()
    {
    return iStringPool.String( iNameRef );
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::PropertyValue
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomList& CXnDomProperty::PropertyValueList()
    {
    return *iPropValList;
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::IsValueSet
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CXnDomProperty::IsValueSet()
    {
    return (iPropValList->Length() > 0);
    }                  
           

// -----------------------------------------------------------------------------
// CXnDomProperty::Inherited
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CXnDomProperty::Inherited()
    {
    return iInherited;
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::SetInherited
// -----------------------------------------------------------------------------
//    
EXPORT_C void CXnDomProperty::SetInherited( TBool aInherited )
    {
    iInherited = aInherited;
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::StringPool
// -----------------------------------------------------------------------------
//        
EXPORT_C CXnDomStringPool& CXnDomProperty::StringPool() const
    {
    return iStringPool;
    }                      
// -----------------------------------------------------------------------------
// CXnDomProperty::StringPoolIndex
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt16 CXnDomProperty::StringPoolIndex()const
    {
    return iNameRef;    
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::PseudoClass
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomProperty::TPseudoClass CXnDomProperty::PseudoClass() const
    {
    return iPseudoClass;
    }
    
// -----------------------------------------------------------------------------
// CXnDomProperty::SetPseudoClass
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomProperty::SetPseudoClass( TPseudoClass aPseudoClass )
    {
    iPseudoClass = aPseudoClass;
    }       
        
// -----------------------------------------------------------------------------
// CXnDomProperty::Size
// -----------------------------------------------------------------------------
//
TInt CXnDomProperty::Size() const
    {
    TInt size( 0 );
    
    size += sizeof(TInt16); //iNameRef
    size += iPropValList->Size();
    size += sizeof(TInt8);    //iInherited
    size += sizeof(TInt8);  //iPseudoClass
    
    return size;    
    }

// -----------------------------------------------------------------------------
// CXnDomProperty::ExternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomProperty::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( iNameRef );
    aStream << *iPropValList;
    aStream.WriteInt8L( iInherited );
    aStream.WriteInt8L( iPseudoClass );   
    }
// -----------------------------------------------------------------------------
// CXnDomProperty::InternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomProperty::InternalizeL( RReadStream& aStream )
    {
    iNameRef = aStream.ReadInt16L() + iStringPool.Offset();
    iPropValList = CXnDomList::NewL( aStream, iStringPool );
    
    iInherited =  TBool( aStream.ReadInt8L() );
    iPseudoClass = TPseudoClass( aStream.ReadInt8L() );
    }

//  End of File  
