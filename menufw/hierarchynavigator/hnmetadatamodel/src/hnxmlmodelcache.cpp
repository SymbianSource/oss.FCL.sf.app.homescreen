/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "hnxmlmodelcache.h"
#include "hnglobals.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint32 HBufCHashCache( HBufC* const &  aBuf )
    {
    return DefaultHash::Des16(*aBuf);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool HBufCIdentCache( HBufC* const & aL, HBufC* const & aR )
    {
    return DefaultIdentity::Des16(*aL, *aR);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnXmlModelCache::CHnXmlModelCache() : iDocuments( &HBufCHashCache, 
        &HBufCIdentCache )
    {
    
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnXmlModelCache::ConstructL()
	{
	
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnXmlModelCache::RemoveUnused()
    {
    // remove unnecessary (oldest and unused) xml cached documents
    if(iDocuments.Count() > KXmlModelCacheMaxLength)
        {
        HBufC *suiteToRemove = iOrder[0];
        RXmlEngDocument* document = iDocuments.Find( suiteToRemove );
        document->Close();
        iDocuments.Remove(suiteToRemove);
        iOrder.Remove(0);
        delete suiteToRemove;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnXmlModelCache::AddL( const TDesC& aSuiteName, RXmlEngDocument& aDocument ) 
    {
    HBufC* key = aSuiteName.AllocLC();
    iDocuments.InsertL( key, aDocument );
    CleanupStack::Pop( key );
    iOrder.AppendL( key );
    RemoveUnused();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnXmlModelCache::IsCachedL( const TDesC& aSuiteName )
    {
    HBufC* buf = aSuiteName.AllocLC();
    const RXmlEngDocument* res = iDocuments.Find( buf );
    CleanupStack::PopAndDestroy( buf );

    if ( res )
        return ETrue;
    else
        return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnXmlModelCache::MoveToTopL( const TDesC & aSuiteName )
    {
    TInt pos( KErrNotFound );
    
    for ( TInt i(0); i < iOrder.Count(); i++ )
        {
        if ( !iOrder[i]->Compare( aSuiteName ) )
            {
            pos = i;
            break;
            }
        }

    // if document was found and is not the latest in the array 
    if ( pos != KErrNotFound && ( pos != iOrder.Count() - 1 ) )
        {
        HBufC *tmp = iOrder[ pos ];
        iOrder.Remove( pos );
        iOrder.AppendL( tmp );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnXmlModelCache::GetL( const TDesC& aSuiteName,
        RXmlEngDocument& aDocument )
    {
    HBufC* buf = aSuiteName.AllocLC();
    const RXmlEngDocument* res = iDocuments.Find( buf );
    
    MoveToTopL( aSuiteName );

    if ( res )
        {
        aDocument = *res;
        }

    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnXmlModelCache* CHnXmlModelCache::NewLC()
    {
    CHnXmlModelCache* self = new (ELeave) CHnXmlModelCache();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnXmlModelCache* CHnXmlModelCache::NewL()
    {
    CHnXmlModelCache* self = CHnXmlModelCache::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnXmlModelCache::Reset()
    {
    THashMapIter<HBufC*, RXmlEngDocument> iter( iDocuments );
    while ( HBufC* const * ptr = iter.NextKey() )
        {
        RXmlEngDocument* currentDocument = iter.CurrentValue();
        currentDocument->Close();
        delete *ptr;
        }
    iDocuments.Close();
    // Names have already been destroyed when destroying iDocuments so
    // it is not necessay to destroy elements of the iOrder array. 
    iOrder.Close(); 
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnXmlModelCache::~CHnXmlModelCache()
    {
    Reset();
    }
