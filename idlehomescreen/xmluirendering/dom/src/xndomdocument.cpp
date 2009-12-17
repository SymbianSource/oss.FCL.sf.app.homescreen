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
* Description:  Represents the entire xmluiml specific xml and css data. 
*
*/



// INCLUDE FILES
#include    "xndomdocument.h"
#include    "xndomnode.h"
#include    "xndomlist.h"
#include    "xndomstringpool.h"
#include    <s32mem.h>

    
// ============================ MEMBER FUNCTIONS ===============================    
// -----------------------------------------------------------------------------
// CXnDomDocument::CloneL()
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDocument* CXnDomDocument::CloneL()
    {
    CXnDomDocument* clone = new (ELeave) CXnDomDocument;
    CleanupStack::PushL( clone );
    
    clone->iDomStringPool = iDomStringPool->CloneL();
    if ( iRootNode )
        {
        clone->iRootNode = iRootNode->CloneL( *clone->iDomStringPool );
        }
    
    CleanupStack::Pop( clone );
    return clone;
    }
    
// -----------------------------------------------------------------------------
// CXnDomDocument::CreateElementNSL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomDocument::CreateElementNSL( 
    const TDesC8& aName, 
    const TDesC8& aNamespace )
    {
    return CXnDomNode::NewL( aName, aNamespace, *iDomStringPool );
    }
// -----------------------------------------------------------------------------
// CXnDomDocument::CXnDomDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomDocument::CXnDomDocument()
    {
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomDocument::ConstructL()
    {
    iDomStringPool = CXnDomStringPool::NewL();
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDocument* CXnDomDocument::NewL()
    {
    CXnDomDocument* self = new( ELeave ) CXnDomDocument;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// CXnDomDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDocument* CXnDomDocument::NewL( 
    RReadStream& aStream )
    {
    CXnDomDocument* self = new( ELeave ) CXnDomDocument;
    
    CleanupStack::PushL( self );
    aStream >> *self;
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::NewL
// Constructs CXnDomDocument from streamed HBufC8.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDocument* CXnDomDocument::NewL( 
    const HBufC8* aBufStream )
    {
    RDesReadStream readStream( *aBufStream );
    CleanupClosePushL( readStream );
    CXnDomDocument* self = CXnDomDocument::NewL( readStream );
    CleanupStack::PopAndDestroy( &readStream );
    return self;   
    }

// Destructor
EXPORT_C CXnDomDocument::~CXnDomDocument()
    {
    if ( iRootNode )
        {
        delete iRootNode;
        }
    
    if ( iDomStringPool )
        {
        delete iDomStringPool;
        }
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::DomNodeCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnDomDocument::DomNodeCount() const
    {
    TInt count( 0 );
    if ( iRootNode )
        {
        count = iRootNode->DescendantCount();
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomDocument::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream << *iDomStringPool;
        
    if ( iRootNode )
        {
        aStream.WriteInt8L( ETrue );    //Root node exist
        aStream << *iRootNode;
        }
    else
        {
        aStream.WriteInt8L( EFalse );
        }    
    }
    
// -----------------------------------------------------------------------------
// CXnDomDocument::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomDocument::InternalizeL( RReadStream& aStream )
    {
    if(iDomStringPool)
        {
        delete iDomStringPool;
        iDomStringPool = NULL;    
        }
    iDomStringPool = CXnDomStringPool::NewL( aStream );
        
    if ( iRootNode )
        {
        delete iRootNode;
        iRootNode = NULL;
        }
    
    TBool rootNodeExist( aStream.ReadInt8L() );
    if ( rootNodeExist )
        {    
        iRootNode = CXnDomNode::NewL( aStream, *iDomStringPool );
        }
    }          

// -----------------------------------------------------------------------------
// CXnDomDocument::SetRootNode
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomDocument::SetRootNode( CXnDomNode* aRootNode )
    {
    iRootNode = aRootNode;
    iRootNode->SetParent( NULL );
    }          

// -----------------------------------------------------------------------------
// CXnDomDocument::RootNode
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomDocument::RootNode() const
    {
    return iRootNode;
    }          

// -----------------------------------------------------------------------------
// CXnDomDocument::LastNode
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomDocument::LastNode() const
    {
    CXnDomNode* last = NULL;
    if ( iRootNode )
        {
        last = iRootNode;
        CXnDomNode* tmp = iRootNode;
        while( tmp )
            {
            last = tmp;
            tmp = static_cast<CXnDomNode*>( last->ChildNodes().Last() );
            }
        }
    return last;
    }          

// -----------------------------------------------------------------------------
// CXnDomDocument::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnDomDocument::Size() const
    {
   	TInt size( 1 ); //Root node information takes one byte
    size += iDomStringPool->Size();
        
    if ( iRootNode )
        {
        
        size += iRootNode->Size();
        }
    
    return size;
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::MarshallL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CXnDomDocument::MarshallL()
    {
    TInt dataLength = Size();
    HBufC8* writeBuf = HBufC8::NewLC( dataLength );
    TPtr8 p( writeBuf->Des() );
    RDesWriteStream writeStream( p );     //stream over the buffer
    CleanupClosePushL( writeStream );
    writeStream << *this; //Stream object
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::Pop( writeBuf );
    return writeBuf;
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::StringPool
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomStringPool& CXnDomDocument::StringPool() const
    {
    return *iDomStringPool;
    }

// -----------------------------------------------------------------------------
// CXnDomDocument::ReadL
// Read contents from a stream.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomDocument::ReadL( 
    RReadStream& aStream )
    { 
    CXnDomNode* rootNode = NULL;
    
    aStream >> *iDomStringPool;
    
    TBool rootNodeExist( aStream.ReadInt8L() );
    if ( rootNodeExist )
        {
        rootNode = CXnDomNode::NewL( aStream, *iDomStringPool );
        }
    
    return rootNode;
    }

//  End of File  
