/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Version     : 2 << Don't touch! Updated by Synergy at check-out.
*
*/

#include "mmitemsdatacache.h"
#include "mmcacheforitem.h"

const TInt KItemCacheArrGranularity = 20;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmItemsDataCache* CMmItemsDataCache::NewLC()
    {
    CMmItemsDataCache* self = new (ELeave) CMmItemsDataCache();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmItemsDataCache* CMmItemsDataCache::NewL()
    {
    CMmItemsDataCache* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmCacheForItem* CMmItemsDataCache::GetItemCacheL( TInt aItemIndex )
    {
    ASSERT( aItemIndex >= 0 );
    while ( iItemCacheArr.Count() <= aItemIndex )
        {
        CMmCacheForItem* cacheForItem = CMmCacheForItem::NewLC( *this );
        iItemCacheArr.AppendL( cacheForItem );
        CleanupStack::Pop( cacheForItem );
        }
    return iItemCacheArr[aItemIndex];
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmItemsDataCache::GetTemplateIdentifierL( const TDesC8& aTemplateName )
    {
    TInt identifier = KErrNotFound;
    
    // this looks very primitive, but RPointerArray::FindL just compares pointers
    // so I guess there is no other way
    TInt templateCount = iTemplateNames.Count();
    for ( TInt i = 0; i < templateCount; ++i )
        {
        if ( iTemplateNames[i]->Compare( aTemplateName ) == 0 )
            {
            identifier = i;
            break;
            }
        }
    
    if ( identifier == KErrNotFound )
        {
        HBufC8* templateNameCopy = HBufC8::NewLC( aTemplateName.Length() );
        templateNameCopy->Des().Copy( aTemplateName );
        iTemplateNames.AppendL( templateNameCopy );
        CleanupStack::Pop( templateNameCopy );
        identifier = iTemplateNames.Count() - 1;
        }
    return identifier;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CMmItemsDataCache::GetTemplateNameByIdentifier(
        TInt aTemplateIdentifier ) const
    {
    return *( iTemplateNames[aTemplateIdentifier] );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmItemsDataCache::Invalidate()
    {
    const TInt count = iItemCacheArr.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iItemCacheArr[i]->MarkAsInvalid();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmItemsDataCache::Trim( TInt aItemCount )
    {
    __ASSERT_ALWAYS( aItemCount >= 0, User::Invariant() );
    for ( TInt i = iItemCacheArr.Count() - 1; i >= aItemCount; --i )
        {
        CMmCacheForItem* cache = iItemCacheArr[i];
        iItemCacheArr.Remove( i );
        delete cache;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmItemsDataCache::~CMmItemsDataCache()
    {
    iItemCacheArr.ResetAndDestroy();
    iItemCacheArr.Close();
    iTemplateNames.ResetAndDestroy();
    iTemplateNames.Close();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmItemsDataCache::CMmItemsDataCache()
    : iItemCacheArr( KItemCacheArrGranularity )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmItemsDataCache::ConstructL()
    {
    }
