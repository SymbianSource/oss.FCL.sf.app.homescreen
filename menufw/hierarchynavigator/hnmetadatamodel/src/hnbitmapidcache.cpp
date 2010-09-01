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
*  Version     : %version:  3 % << Don't touch! Updated by Synergy at check-out.
*
*/

#include "hnbitmapidcache.h"
#include "hnglobals.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint32 HBufC8HashCache( HBufC8* const &  aBuf )
    {
    return DefaultHash::Des8(*aBuf);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool HBufC8IdentCache( HBufC8* const & aL, HBufC8* const & aR )
    {
    return DefaultIdentity::Des8(*aL, *aR);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnBitmapIdCache::CHnBitmapIdCache() : iEntries( &HBufC8HashCache, 
        &HBufC8IdentCache )
    {
    
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnBitmapIdCache::ConstructL()
	{
	
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnBitmapIdCache::AddL( const TDesC8& aKey, TInt aValue ) 
    {
    HBufC8* key = aKey.AllocL();
    iEntries.InsertL( key, aValue );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnBitmapIdCache::ExistsL( const TDesC8& aKey )
    {
    HBufC8* buf = aKey.AllocLC();
    TInt* value = iEntries.Find( buf );
    CleanupStack::PopAndDestroy( buf );

    if ( value )
        return ETrue;
    else
        return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnBitmapIdCache::GetL( const TDesC8& aKey, TInt& value )
    {
    HBufC8* buf = aKey.AllocLC();
    const TInt* res = iEntries.Find( buf );
    
    if ( res )
        {
        value = *res;
        }

    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnBitmapIdCache* CHnBitmapIdCache::NewLC()
    {
    CHnBitmapIdCache* self = new (ELeave) CHnBitmapIdCache();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnBitmapIdCache* CHnBitmapIdCache::NewL()
    {
    CHnBitmapIdCache* self = CHnBitmapIdCache::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnBitmapIdCache::Reset()
    {
    THashMapIter<HBufC8*, TInt> iter( iEntries );
    while ( HBufC8* const * ptr = iter.NextKey() )
        {
        TInt* value = iter.CurrentValue();
        delete *ptr;
        }
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnBitmapIdCache::~CHnBitmapIdCache()
    {
    Reset();
    }
