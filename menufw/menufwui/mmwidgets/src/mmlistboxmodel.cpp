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
*
*/


#include "mmlistboxmodel.h"
#include "mmwidgetsconstants.h"
#include <eiktxlbm.h> 
#include "hnconvutils.h"
#include "hnsuiteobserver.h"
#include "hnsuitemodel.h"
#include "hnitemmodel.h"
#include "hnattributebase.h"
#include "hnattributeimage.h"
#include "hnmenuitemmodel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//    
CMmListBoxModel::CMmListBoxModel() 
    : iSuiteModel( NULL )
	{
	
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmListBoxModel::~CMmListBoxModel()
	{
    if (iSuiteModel)
    	{
        iSuiteModel->UnregisterSuiteObserver( this );
    	}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmListBoxModel* CMmListBoxModel::NewLC()
	{
	CMmListBoxModel* self = new (ELeave) CMmListBoxModel();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmListBoxModel* CMmListBoxModel::NewL()
	{
	CMmListBoxModel* self = CMmListBoxModel::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBoxModel::SetSuiteModelL( CHnSuiteModel * aSuiteModel )
    {   
    if (iSuiteModel)
        iSuiteModel->UnregisterSuiteObserver( this );
    
    iSuiteModel = aSuiteModel;
    UpdateDummyArrayDataL();
    
    if (iSuiteModel)
        iSuiteModel->RegisterSuiteObserverL( this, EPriorityNormal );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CHnAttributeBase* CMmListBoxModel::GetAttribute( TInt aIndex,
        const TDesC8 & aAttributeName )
    {
    CHnItemModel* item = (iSuiteModel) ? iSuiteModel->GetItemModel( iSuiteModel->IdByIndex( aIndex ) ) : NULL;
    return (item) ? item->GetAttribute( aAttributeName ) : NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
const TDesC8& CMmListBoxModel::GetAttributeAsText( TInt aIndex,
		const TDesC8 & aAttributeName )
    {
    CHnAttributeBase* attribute = GetAttribute( aIndex, aAttributeName );
    return (attribute) ? attribute->Value() : KNullDesC8;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CHnIconHolder* CMmListBoxModel::GetAttributeAsRefCountedGraphics( TInt aIndex,
        const TDesC8 & aAttributeName, TSize* aDesiredIconSize)
    {
    CHnAttributeBase* attribute = GetAttribute( aIndex, aAttributeName );
    if ( !attribute || attribute->Type() != EImageAttribute )
        {
        return NULL;
        }
    return attribute->GetIconHolder( aDesiredIconSize );
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBoxModel::ConstructL()
	{
	CTextListBoxModel::ConstructL();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBoxModel::UpdateDummyArrayDataL( )
    {
    CDesCArrayFlat* dataArray = static_cast<CDesC16ArrayFlat*>( ItemTextArray() );
    dataArray->Reset();
    for( TInt i(0); iSuiteModel && i < iSuiteModel->GetItemModelsCount(); i++ )
        {
        CHnItemModel* itemModel = iSuiteModel->GetItemModel( iSuiteModel->IdByIndex( i ) );
        CHnItemModel* emptyItem = iSuiteModel->GetItemModel( iSuiteModel->IdByIndex( -1 ) );
        if (!itemModel || itemModel == emptyItem )
            {
            continue;
            }
        dataArray->AppendL( KNullDesC16() );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBoxModel::ReorderModelL(TInt aFromIndex, TInt aToIndex)
    {
    iSuiteModel->ReorderItemsL( aFromIndex, aToIndex );
    UpdateDummyArrayDataL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CHnSuiteModel* CMmListBoxModel::GetSuiteModel()
    {
    return iSuiteModel;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBoxModel::HandleSuiteEventL( THnCustomSuiteEvent aCustomSuiteEvent,
        CHnSuiteModel* aModel)
    {
    if (iSuiteModel == aModel)
        {
        switch ( aCustomSuiteEvent )
    	    {
    	    case ESuiteModelDestroyed:
    		    iSuiteModel = NULL;
    		    break;
    	    case ESuiteItemsAdded:
    	    case ESuiteItemsRemoved:
    	    case ESuiteItemsUpdated:
    	        UpdateDummyArrayDataL( );
    	    default:
    	    	break;
    	    }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
TInt CMmListBoxModel::GetNumberOfSpecificMenuItemsL( TInt aItemIndex )
    {
    TInt itemSpecificMenuItemCount = KErrNotFound;
    if ( iSuiteModel )
        {
        TInt itemId = iSuiteModel->IdByIndex( aItemIndex );
        if ( itemId != KErrNotFound )
            {
            itemSpecificMenuItemCount = 0;
            MHnMenuItemModelIterator* menuItemModelIter =
                    iSuiteModel->GetMenuStructureL( itemId );
            while ( menuItemModelIter && menuItemModelIter->HasNextSpecific() )
                {
                menuItemModelIter->GetNextSpecific();
                ++itemSpecificMenuItemCount;
                }
            }
        }
    return itemSpecificMenuItemCount;
    }

// End of file
