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
* Description:  List class to hold CXnDomList objects.
*
*/



// INCLUDE FILES
#include    "xndomlist.h"
#include    "xndomnode.h"
#include    "xndomattribute.h"
#include    "xndomproperty.h"
#include    "xndompropertyvalue.h"
#include    "xndomstringpool.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDomList::CXnDomList
// C++ parameter constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomList::CXnDomList( 
    TListType aListType, 
    CXnDomStringPool* aStringPool,
    TInt aGranularity ):
    iListType( aListType ),
    iList( aGranularity ), 
    iStringPool( aStringPool )
    {
    }
// -----------------------------------------------------------------------------
// CXnDomList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomList::ConstructL()
    {
   
    }

// -----------------------------------------------------------------------------
// CXnDomList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnDomList* CXnDomList::NewL( 
    TListType aListType, 
    CXnDomStringPool* aStringPool,
    TInt aGranularity)
    {
    CXnDomList* self = 
        new( ELeave ) CXnDomList( aListType, aStringPool,aGranularity );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
// -----------------------------------------------------------------------------
// CXnDomList::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
CXnDomList* CXnDomList::NewL( 
    RReadStream& aStream, 
    CXnDomStringPool* aStringPool )
    {
    CXnDomList* self = new( ELeave ) CXnDomList( ENodeList, aStringPool );
    
    CleanupStack::PushL( self );
   
    aStream >> *self;
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
CXnDomList::~CXnDomList()
    {
    iList.ResetAndDestroy();    
    }

// -----------------------------------------------------------------------------
// CXnDomList::Reset
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::Reset()
    {
    iList.Reset();
    }
    
// -----------------------------------------------------------------------------
// CXnDomList::AddItemL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::AddItemL( MXnDomListItem* aItem )
    {
    iList.AppendL( aItem );
    }
    
// -----------------------------------------------------------------------------
// CXnDomList::AddItemL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::AddItemL( MXnDomListItem* aItem, TInt aIndex )
    {
    iList.InsertL( aItem, aIndex );
    }

// -----------------------------------------------------------------------------
// CXnDomList::RemoveItem
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::RemoveItem( TInt aIndex )
    {
    iList.Remove( aIndex);
    iList.Compress();
    }
// -----------------------------------------------------------------------------
// CXnDomList::DeleteItem
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::DeleteItem( TInt aIndex )
    {
    delete iList[ aIndex ];
    iList.Remove( aIndex);
    iList.Compress();
    }

// -----------------------------------------------------------------------------
// CXnDomList::RemoveItem
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::RemoveItem( MXnDomListItem* aItem ) 
    {
    MXnDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    TBool stop( EFalse );
    for ( TInt i=0; i<count && !stop; i++ )    
        {
        tmp = iList[i];
        if ( tmp == aItem )
            {
            iList.Remove( i );
            iList.Compress();
            stop = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnDomList::DeleteItem
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::DeleteItem( MXnDomListItem* aItem ) 
    {
    MXnDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    TBool stop( EFalse );
    for ( TInt i=0; i<count && !stop ; i++ )    
        {
        tmp = iList[i];
        if ( tmp == aItem )
            {
            delete iList[i];
            iList.Remove( i );
            iList.Compress();
            stop = ETrue;
            }
        }
    }    
// -----------------------------------------------------------------------------
// CXnDomList::Item
// -----------------------------------------------------------------------------
//    
EXPORT_C MXnDomListItem* CXnDomList::Item( TInt aIndex )const
    {
    
    return iList[ aIndex ];
    }    
// -----------------------------------------------------------------------------
// CXnDomList::Length
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnDomList::Length() const
    {
   
    return iList.Count();    
    }
// -----------------------------------------------------------------------------
// CXnDomList::First
// -----------------------------------------------------------------------------
//
EXPORT_C MXnDomListItem* CXnDomList::First()
    {
    if ( iList.Count() )
        {
        return iList[ 0 ];
        }
    else
        {
        return NULL;
        }
    }    
    
// -----------------------------------------------------------------------------
// CXnDomList::Last
// -----------------------------------------------------------------------------
//
EXPORT_C MXnDomListItem* CXnDomList::Last()
    {
    TInt count( iList.Count() );
    if ( count )
        {
        return  iList[ count-1 ];
        }
    else
        {
        return NULL;
        }
   
    }    
    

// -----------------------------------------------------------------------------
// CXnDomList::FindByName
// -----------------------------------------------------------------------------
//
EXPORT_C MXnDomListItem* CXnDomList::FindByName( const TDesC8& aName )
    {
    
    if ( iListType == CXnDomList::EPropertyValueList )
        {
        return NULL;
        }
        
    MXnDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    for ( TInt i=0; i<count; i++ )    
        {
        tmp = iList[ i ];
        
        if ( aName.Compare( tmp->Name() ) == 0 )
            {
            return tmp;
            }
        }
    return NULL; 
    }
        
// -----------------------------------------------------------------------------
// CXnDomList::ItemIndex
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnDomList::ItemIndex( const MXnDomListItem& aItem ) const
    {
    
    MXnDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    for ( TInt i=0; i<count; i++ )    
        {
        tmp = iList[ i ];
        if ( tmp == &aItem )
            {
            return i;
            }
        }
    return KErrNotFound;   
    }

// -----------------------------------------------------------------------------
// CXnDomList::StringPool
// -----------------------------------------------------------------------------
//        
EXPORT_C CXnDomStringPool* CXnDomList::StringPool() const
    {
    return iStringPool;
    }

// -----------------------------------------------------------------------------
// CXnDomAttribute::SwapStringPoolL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomList::SwapStringPoolL( CXnDomStringPool* aStringPool )
    {
    if( !aStringPool )
        {
        User::Leave( KErrArgument );
        }
    
    iStringPool = aStringPool;
    }

// -----------------------------------------------------------------------------
// CXnDomList::Size
// -----------------------------------------------------------------------------
//
TInt CXnDomList::Size() const
    {
    TInt size = 0;
    
    size += sizeof(TInt8);      //Type
    size += sizeof(TInt32);     //Item count
   
    TInt count( iList.Count() );
    for ( TInt i=0; i<count; i++ )    
        {
        size += iList[ i ]->Size();
        }
    return size;
    }
// -----------------------------------------------------------------------------
// CXnDomList::ExternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomList::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iListType );
    
   
    TInt count( iList.Count() );
    aStream.WriteInt32L( count );
    
    
    for ( TInt i=0; i<count; i++ )    
        {
        aStream << *iList[ i ];
        }    
    }

// -----------------------------------------------------------------------------
// CXnDomList::InternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomList::InternalizeL( RReadStream& aStream )
    {
    iListType = static_cast<CXnDomList::TListType>( aStream.ReadInt8L() );    
    
    TInt count ( aStream.ReadInt32L() );
    for ( TInt i=0; i<count; i++ )
        {
        MXnDomListItem* item = NULL;
        
        switch( iListType )
        	{
        	case CXnDomList::ENodeList:
    			{
    			item = CXnDomNode::NewL( aStream, iStringPool );
    			}
        	break;
        	
        	case CXnDomList::EAttributeList:
    			{
    			item = CXnDomAttribute::NewL( aStream, iStringPool );
    			}
        	break;
        	case CXnDomList::EPropertyList:
        		{
        		item = CXnDomProperty::NewL( aStream, iStringPool );
        		}
        	break;
        	case CXnDomList::EPropertyValueList:
        		{
        		item = CXnDomPropertyValue::NewL( aStream, iStringPool );
        		}
        	break;
        	
        	default:
        		User::Leave(KErrArgument);
        		break;
        	}
        CleanupDeletePushL( item );	
        iList.AppendL( item );
        CleanupStack::Pop( item );
        }
    }



//  End of File  
