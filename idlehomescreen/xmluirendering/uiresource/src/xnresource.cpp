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
* Description:  Resource definition of Xuikon Application Theme Management Services.
*                See XnResource.h.
*
*
*/


#include "xnresource.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnResource::CXnResource()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnResource::CXnResource()
    {
    }

// -----------------------------------------------------------------------------
// CXnResource::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnResource::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnResource::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnResource* CXnResource::NewL()
    {
    CXnResource* self = new( ELeave ) CXnResource;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnResource::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnResource* CXnResource::NewLC( RReadStream& aStream )
    {
    CXnResource* resource = CXnResource::NewL();
    CleanupStack::PushL( resource );
    resource->InternalizeL( aStream );
    return resource;
    }
   
   
// -----------------------------------------------------------------------------
// CXnResource::CloneL()
// Returns an exact copy of this CXnResource object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CXnResource* CXnResource::CloneL()
    {
    CXnResource* clone = CXnResource::NewL();
    CleanupStack::PushL( clone );
    clone->SetLockingPolicy( iLockingPolicy );
    clone->SetCacheType( iCacheType );
    clone->SetResourceType( iResourceType );
      
    clone->SetResourceIdL( *iResourceID );
    clone->SetNameSpaceL( *iNameSpace );
    clone->SetFileNameL( *iFileName );
    clone->SetMimeTypeL( iMimeType );
    
    CleanupStack::Pop( clone );
    return clone;
    }

// Destructor
CXnResource::~CXnResource()
    {
    iWriteStream.Close();
    iReadStream.Close();
    delete iResourceID;
    delete iNameSpace;
    delete iFileName;
    }


// -----------------------------------------------------------------------------
// CXnResource::ExternalizeL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iLockingPolicy );
    aStream.WriteUint32L( iCacheType );
    aStream.WriteUint32L( iResourceType );
    
    if ( iResourceID )
        {
        aStream << *iResourceID;
        }
    else
        {
        aStream << KNullDesC;
        }
  
    if ( iNameSpace )
        {
        aStream << *iNameSpace;
        }
    else 
        {
        aStream << KNullDesC;
        }
        
    if ( iFileName )
        {
        aStream << *iFileName;
        }
    else 
        {
        aStream << KNullDesC;
        }
    
     
    iMimeType.ExternalizeL(aStream);    
 
    // replacement of unimplemented buffer size
    TInt size = 0;
    aStream.WriteUint32L( size );  
    
    aStream.WriteUint32L( iOffset );
    }
       
// -----------------------------------------------------------------------------
// CXnResource::InternalizeL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::InternalizeL( RReadStream& aStream )
    {
    iLockingPolicy = (TXnLockingPolicy)aStream.ReadUint32L();
    iCacheType = (TXnCacheType)aStream.ReadUint32L();
    iResourceType = (TXnResourceType)aStream.ReadUint32L();

    delete iResourceID;
    iResourceID = NULL;
    iResourceID = HBufC::NewL(aStream, KMaxFileName );
 
    delete iNameSpace;
    iNameSpace = NULL;
    iNameSpace = HBufC::NewL(aStream, KMaxFileName );
    
    delete iFileName;
    iFileName = NULL;
    iFileName = HBufC::NewL(aStream, KMaxFileName );
    

    iMimeType.InternalizeL(aStream);
    
    TInt size = aStream.ReadUint32L();
    iOffset = aStream.ReadUint32L();
    }
    
// -----------------------------------------------------------------------------
// CXnResource::SetLockingPolicy().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetLockingPolicy( TXnLockingPolicy aLockingPolicy )
    {
    iLockingPolicy = aLockingPolicy;
    }
    
// -----------------------------------------------------------------------------
// CXnResource::LockingPolicy().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TXnLockingPolicy CXnResource::LockingPolicy() const
    {
    return iLockingPolicy;        
    }

// -----------------------------------------------------------------------------
// CXnResource::SetCacheType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetCacheType( TXnCacheType aCacheType )
    {
    iCacheType = aCacheType;
    }
    
// -----------------------------------------------------------------------------
// CXnResource::CacheType()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TXnCacheType CXnResource::CacheType() const
    {
    return iCacheType;        
    }

// -----------------------------------------------------------------------------
// CXnResource::SetResourceType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetResourceType( TXnResourceType aResourceType )
    {
    iResourceType = aResourceType;
    }
    
// -----------------------------------------------------------------------------
// CXnResource::ResourceType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TXnResourceType CXnResource::ResourceType() const
    {
    return iResourceType;        
    }


// -----------------------------------------------------------------------------
// CXnResource::SetResourceIdL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetResourceIdL( const TDesC& aResourceId )
    {
    delete iResourceID;
    iResourceID = NULL;
    iResourceID = aResourceId.AllocL();
    }
    
// -----------------------------------------------------------------------------
// CXnResource::ResourceId().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CXnResource::ResourceId() const
    {
    if ( iResourceID )
        {
        return *iResourceID;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CXnResource::SetNameSpaceL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetNameSpaceL( const TDesC& aNameSpace )
    {
    delete iNameSpace;
    iNameSpace = NULL;
    iNameSpace = aNameSpace.AllocL();
    }
    
// -----------------------------------------------------------------------------
// CXnResource::NameSpace().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CXnResource::NameSpace() const
    {
    if ( iNameSpace )
        {
        return *iNameSpace;
        }
    else
        {
        return KNullDesC;
        }
    }
    
// -----------------------------------------------------------------------------
// CXnResource::SetFileNameL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetFileNameL( const TDesC& aFileName )
    {
    delete iFileName;
    iFileName = NULL;
    iFileName = aFileName.AllocL();
    }
    
// -----------------------------------------------------------------------------
// CXnResource::FileName().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CXnResource::FileName() const
    {
    if ( iFileName )
        {
        return *iFileName;
        }
    else
        {
        return KNullDesC;
        }
    }
    
    
// -----------------------------------------------------------------------------
// CXnResource::SetMimeTypeL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetMimeTypeL( const TDataType aDataType )
    {
    iMimeType = aDataType;
    }
    
// -----------------------------------------------------------------------------
// CXnResource::MimeType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDataType CXnResource::MimeType() const
    {
    return iMimeType;
    }    
    
// -----------------------------------------------------------------------------
// CXnResource::WriteStream().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RBufWriteStream& CXnResource::WriteStream( TUint aOffset )
    {
    iWriteStream.Open( *iBuffer, aOffset );
    return iWriteStream;
    }

// -----------------------------------------------------------------------------
// CXnResource::ReadStream().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RBufReadStream& CXnResource::ReadStream(TUint aOffset )
    {
    // constructing read stream on buffer 
    iReadStream.Open( *iBuffer, aOffset );
    return iReadStream;
    }

// -----------------------------------------------------------------------------
// CXnResource::SetOffset().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnResource::SetOffset( TUint aOffset )
    {
    iOffset = aOffset;
    }


// -----------------------------------------------------------------------------
// CXnResource::Offset().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CXnResource::Offset() const
    {
    return iOffset;
    }

// -----------------------------------------------------------------------------
// CXnResource::GetDataStreamLength().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnResource::GetDataStreamLength() const
    {
    TInt len = sizeof( iLockingPolicy );
    len += sizeof(iCacheType);
    len += sizeof(iResourceType);
    len += iResourceID->Size();
    len += iNameSpace->Size();
    len += iFileName->Size();
    len += sizeof(iMimeType);
    len += sizeof(iOffset);
    return len;
    }
    
//  End of File  
