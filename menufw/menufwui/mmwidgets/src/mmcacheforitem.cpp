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

#include "mmcacheforitem.h"
#include "mmitemsdatacache.h"
#include "hniconholder.h"

const TInt KIconHolderListGranularity = 2;
const TInt KIconListGranularity = 2;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmCacheForItem* CMmCacheForItem::NewLC( CMmItemsDataCache& aParent )
    {
    CMmCacheForItem* self = new (ELeave) CMmCacheForItem( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmCacheForItem* CMmCacheForItem::NewL( CMmItemsDataCache& aParent )
    {
    CMmCacheForItem* self = NewLC( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmCacheForItem::~CMmCacheForItem()
    {
    ClearIconArray();
    iIconHolderList.Close();
    iItemText.Close();
    delete iIconList;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CMmCacheForItem::GetTemplate() const
    {
    __ASSERT_DEBUG( iIsValid, User::Invariant() );
    return iParent.GetTemplateNameByIdentifier( iTemplateIdentifier );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmCacheForItem::SetTemplateL( const TDesC8& aItemTemplate )
    {
    iTemplateIdentifier = iParent.GetTemplateIdentifierL( aItemTemplate );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CMmCacheForItem::GetIconListL()
    {
    __ASSERT_DEBUG( iIsValid, User::Invariant() );
    return iIconList;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmCacheForItem::AppendIconL( CHnIconHolder* aIconHolder )
    {
    ASSERT( aIconHolder );
    iIconHolderList.AppendL( aIconHolder );
    TInt itemIndex =  iIconHolderList.Count() - 1;
    TInt err = aIconHolder->Open();
    if ( err != KErrNone )
        {
        iIconHolderList.Remove( itemIndex );
        User::Leave( err );
        }
    return itemIndex;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmCacheForItem::ClearIconArray()
    {
    iIconList->Reset();
    const TInt count = iIconHolderList.Count();
    for (TInt i = 0; i < count; ++i )
        {
        iIconHolderList[i]->Close();
        }
    if ( count )
        {
        iIconHolderList.Reset();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmCacheForItem::InvalidateIfCacheMayNotBeUsed( TBool aIsItemCurrent,
            TMmSubcellsSetupCode aSubcellsSetupCode )
    {
    if ( iSubcellsSetupCode != aSubcellsSetupCode ||
            (!!iIsCurrent) != (!!aIsItemCurrent) )
        {
        SetValidL( EFalse );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmCacheForItem::CMmCacheForItem( CMmItemsDataCache& aParent )
    : iIconHolderList( KIconHolderListGranularity ), iParent( aParent )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmCacheForItem::ConstructL()
    {
    iIconList = new ( ELeave ) CArrayPtrFlat<CGulIcon>( KIconListGranularity );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmCacheForItem::UpdateIconListL()
    {
    iIconList->Reset();
    const TInt iconCount = iIconHolderList.Count();
    for ( TInt i = 0; i < iconCount; ++i )
        {
        iIconList->AppendL( iIconHolderList[i]->GetGulIcon() );
        }
    }
