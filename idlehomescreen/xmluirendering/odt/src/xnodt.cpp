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
* Description:  Class represents an Object Description Tree of Xuikon.
*
*/

#include <s32strm.h>
#include <s32mem.h>
#include "xndomdocument.h"
#include "xnresource.h"
#include "xnodt.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnODT::CXnODT
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnODT::CXnODT()
    {
    }

// -----------------------------------------------------------------------------
// CXnODT::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnODT::ConstructL()
    {
    iDomDocument = CXnDomDocument::NewL();    
    }

// -----------------------------------------------------------------------------
// CXnODT::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnODT* CXnODT::NewL()
    {
    CXnODT* self = new( ELeave ) CXnODT;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CXnODT::~CXnODT()
    {
    delete iDomDocument;
    }

// -----------------------------------------------------------------------------
// CXnODT::InternalizeHeaderL
// Internalizes the ODT header
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
EXPORT_C void CXnODT::InternalizeHeaderL( RReadStream& aStream )
    {
    // Dummy internalize to support legacy for backward compatibility
    
    // iAppUid 
    (void)aStream.ReadUint32L();
    // iProviderUid
    (void)aStream.ReadUint32L();    
    // iThemeUid
    (void)aStream.ReadUint32L();
           
    // iProviderName
    HBufC::NewLC( aStream, KMaxFileName );        
    // iThemeFullName
    HBufC::NewLC( aStream, KMaxFileName );
    // iThemeShortName
    HBufC::NewLC( aStream, KMaxFileName );
    // iThemeVersion
    HBufC::NewLC( aStream, KMaxFileName );
    
    CleanupStack::PopAndDestroy( 4 );
    
    // iScreenSizeX
    (void)aStream.ReadUint32L();
    // iScreenSizeY
    (void)aStream.ReadUint32L();
    // iLanguage
    (void)aStream.ReadInt32L();
    // iFlags
    (void)aStream.ReadUint32L();
    
    // consumes header delimiter
    (void)aStream.ReadInt16L();    
    }

// -----------------------------------------------------------------------------
// CXnODT::InternalizeResourceListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CArrayPtrSeg< CXnResource >* CXnODT::InternalizeResourceListL( 
    RReadStream& aStream )
    {
    CArrayPtrSeg< CXnResource >* list = 
        new( ELeave ) CArrayPtrSeg<CXnResource>( 4 ); 

    CleanupStack::PushL( list );
    
    // stream in the resource list
    TInt count( aStream.ReadInt32L() );
    
    for ( TInt i = 0; i < count; i++ ) 
        {
        CXnResource* resource = CXnResource::NewL();
        CleanupStack::PushL( resource );
        
        resource->InternalizeL( aStream );
        
        list->AppendL( resource );
        CleanupStack::Pop( resource ); // now owned by array        
        }
    
    CleanupStack::Pop( list );
    
    return list;
    }

// -----------------------------------------------------------------------------
// CXnODT::InternalizeDomDocumentL
// Internalizes the ODT with shared document 
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
EXPORT_C CXnDomNode* CXnODT::InternalizeDomDocumentL( RReadStream& aStream )
    {    
    return ( iDomDocument->ReadL( aStream ) );
    }

// -----------------------------------------------------------------------------
// CXnODT::DomDocument
// Get DomDocument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomDocument& CXnODT::DomDocument() const
    {
    return *iDomDocument;
    }
              
//  End of File  
