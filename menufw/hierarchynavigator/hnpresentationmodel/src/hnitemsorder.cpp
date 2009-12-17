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
* Description:  items order manager class
*
*/


#include "hnitemsorder.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnItemsOrder* CHnItemsOrder::NewL()
    {
    CHnItemsOrder* self = NewLC();
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::MarkSuiteUninitialized()
	{
    for (TInt i(0); i < iItemsEvaluated.Count(); i++)
         {
         iItemsEvaluated[i] = EFalse;
         }
    
    iSuiteEvaluated = EFalse;
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnItemsOrder* CHnItemsOrder::NewLC()
    {
    CHnItemsOrder* self = new( ELeave ) CHnItemsOrder();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnItemsOrder::~CHnItemsOrder()
    {
    iItemsOrderCounts.Reset();
    iComplexItemsOrder.Reset();
    iItemsEvaluated.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::SetSuiteId( TInt aId )
    {
    iSuiteId = aId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::GetSuiteId() const
    {
    return iSuiteId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::RemoveItemId( TInt aId )
    {
    TInt position = iComplexItemsOrder.Find( aId );
    if (position != KErrNotFound)
        {
        TInt modelNumber = CountModelNumberFor( position );
        --iItemsOrderCounts[ modelNumber ] ;    
        iComplexItemsOrder.Remove( position );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::InsertItemIdToL( TInt aDataModelItem, 
        TInt aIndex, TInt aId )
    {
    TInt position = CountStartIndexFor( aDataModelItem ) + aIndex;
    if (position < iComplexItemsOrder.Count() )
        {
        iComplexItemsOrder.InsertL( aId, position ); 
        }
    else
        {
        iComplexItemsOrder.AppendL( aId );
        }
    
    TInt modelsCount = iItemsOrderCounts.Count();
    if (modelsCount <= aDataModelItem)
        {
        for (TInt i(modelsCount);  i <= aDataModelItem; i++)
            {
            iItemsOrderCounts.AppendL( 0 );
            }
        }
    ++iItemsOrderCounts[ aDataModelItem ] ;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::GetItemIdAt( TInt aDataModelItem, TInt aIndex )
    {
    TInt globalItemPosition = CountStartIndexFor( aDataModelItem ) + aIndex;
    return (iComplexItemsOrder.Count() > globalItemPosition)
        ? iComplexItemsOrder[globalItemPosition] : KErrNotFound;
    }  

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::GetItemIdAt( TInt aPosition )
    {
    return (iComplexItemsOrder.Count() > aPosition) 
        ? iComplexItemsOrder[aPosition] : KErrNotFound;
    }  

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::GetIdsForL( TInt aDataModelItem,
        RArray< TInt >& aIds )
    {
    if ( CountStartIndexFor( aDataModelItem ) == KErrNotFound 
            || iItemsOrderCounts.Count() <= aDataModelItem )
        {
        return;
        }
    
    TInt startIndex = CountStartIndexFor( aDataModelItem );
    
    for ( TInt i = 0; 
            i < iItemsOrderCounts[ aDataModelItem ]; i++ )
        {
        aIds.AppendL( iComplexItemsOrder[ startIndex + i ] );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::SetItemIdAtL( TInt aPosition, TInt aId )
    {
    if ( aPosition >= iComplexItemsOrder.Count() )
        {
        iComplexItemsOrder.AppendL( aId );
        }
    else
        {
        iComplexItemsOrder[ aPosition ] = aId;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::DefineModelItemCountL( TInt aItemCounts )
    {
    // Don't update this information on a initialized suitemodel.
    if (iItemsEvaluated.Count() == 0 && !iSuiteEvaluated)
        {
        iSuiteEvaluated = EFalse;
        for (TInt i(0); i < aItemCounts; i++)
            {
            iItemsOrderCounts.AppendL( 0 );
            iItemsEvaluated.AppendL(EFalse);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::SuiteEvaluated()
    {
    iSuiteEvaluated = ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemsOrder::ItemEvaluated( TInt aIndex )
    {
    iItemsEvaluated[aIndex] = ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnItemsOrder::IsSuiteReadyToShow()
    {
    TBool ready = ETrue;
    for (TInt i(0); ready && i < iItemsEvaluated.Count(); i++)
         {
         ready = iItemsEvaluated[i];
         }
    
    return ready && iSuiteEvaluated;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::GetModelItemCount( )
    {
    return iItemsOrderCounts.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::operator[]( TInt aPosition )
    {
    return GetItemIdAt( aPosition );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::GetItemPosition( TInt aId )
    {
    return iComplexItemsOrder.Find( aId );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnItemsOrder::CountStartIndexFor( TInt aDataModelItem )
    {
    TInt startIndex( 0 );
    for( TInt i = 0; i < aDataModelItem && i < iItemsOrderCounts.Count(); i++ )
        {
        startIndex += iItemsOrderCounts[ i ];
        }
    return startIndex;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnItemsOrder::CountModelNumberFor( TInt aIndex )
    {
    TInt modelNumber( 0 );
    TInt countLow( 0 );
    TInt countHigh( 0 );
    for( ; modelNumber < iItemsOrderCounts.Count(); modelNumber++ )
        {
        countHigh += iItemsOrderCounts[ modelNumber ];
        if ( aIndex >= countLow && aIndex < countHigh )
            {
            break;
            }
        countLow = countHigh;
        }
    return modelNumber;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnItemsOrder::Count()
    {
    return iComplexItemsOrder.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnItemsOrder::CHnItemsOrder() : iSuiteId( KErrNotFound )
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnItemsOrder::ConstructL()
    {
    
    }

