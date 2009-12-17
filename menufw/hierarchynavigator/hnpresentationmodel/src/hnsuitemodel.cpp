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
* Description:   suite presentation model
*
*/


#include <e32cmn.h>
#include "hnsuitemodel.h"
#include "hnsuitemodelcontainer.h"
#include "hnitemmodel.h"
#include "hnmenuitemmodel.h"
#include "hnactionmodel.h"
#include "hntoolbarmodel.h"
#include "menudebug.h"
#include "hnglobals.h"
#include "hnsuiteobserver.h"
#include "hnattributebase.h"
#include "hnattributetext.h"
#include "hnitemsorder.h"
#include "hneventhandler.h"
#include "hnitemfocushandler.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModel::NewL( CHnEventHandler& aEventHandler,
        const TDesC& aGenreName )
	{
	CHnSuiteModel* self = new (ELeave) CHnSuiteModel( aEventHandler );
	CleanupStack::PushL( self );
	self->ConstructL( aGenreName );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnSuiteModel::ConstructL( const TDesC& aSuiteName )
	{
	ASSERT( aSuiteName.Length() );
	iSuiteName = aSuiteName.AllocL();
	iTemplate.CreateL( KNullDesC8() );
	iItemsOrder = CHnItemsOrder::NewL();
	iExitMode = EExitModeNormal;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel::~CHnSuiteModel()
	{
    for (TInt i(0) ; i < iSuiteObservers.Count(); i++)
        {
        TRAP_IGNORE( iSuiteObservers[i]->HandleSuiteEventL( ESuiteModelDestroyed, this ) );
        }

    if ( iIsVisible && iIsActive )
    	{
    	TRAP_IGNORE( HandleItemFocusL( iSuiteHighlight.iItemId, EFalse ) );
    	}
    delete iItemFocusHandler;
    TRAP_IGNORE( OfferHnEventL( KIdSuiteUnLoad, 
            GetItemsOrder()->GetSuiteId(), NULL ) );

    DEBUG16(("\t_Mm_:KIdSuiteUnLoad - %S",iSuiteName));
    
	iSuiteObservers.Close();
	iObserverPriorities.Close();
	DEBUG16(("\t_MM_: CHnSuiteModel destructor: %S",iSuiteName));
	iTemplate.Close();
	delete iTitle;
	delete iEmptyText;
	delete iSuiteName;

	THashMapIter< TInt ,CHnItemModel* > modelIter( iItemsModels );
	while( modelIter.NextKey() )
		{
		const TInt* key = modelIter.CurrentKey();
		iItemsOrder->RemoveItemId( *key );
		CHnItemModel** model = modelIter.CurrentValue();
		delete *model;
		modelIter.RemoveCurrent();
		}
	iItemsModels.Close();
		    
    THashMapIter< TInt, CArrayPtr<CHnActionModel>* > actionIter( iActions );
    while( actionIter.NextKey() )
        {
        CArrayPtr<CHnActionModel>** actions = actionIter.CurrentValue();
        (*actions)->ResetAndDestroy();
        delete *actions;
        actionIter.RemoveCurrent();
        }
    iActions.Close();
    delete iItemsOrder;
    DEBUG16(("\t_MM_: CHnSuiteModel destructor OUT"));
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnSuiteModel::CHnSuiteModel( CHnEventHandler& aEventHandler ) :
        iItemsModels( &DefaultHash::Integer, &DefaultIdentity::Integer ),      
        iActions( &DefaultHash::Integer, &DefaultIdentity::Integer ),
        iEmptyItemId( KErrNotFound ),
        iFocusQueue( KErrNotFound ),
        iCustomId( KErrNotFound ),
        iEventHandler( aEventHandler ) 
	{
	iSuiteHighlight.iWidgetIndex = KErrNotFound;
	iSuiteHighlight.iItemId = KErrNotFound;
	iSuiteHighlight.iCustomId = KErrNotFound;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnSuiteModel::OfferHnEventL( 
       const TInt aEventId, const TInt aRecipientId, 
        CLiwGenericParamList* aEventParameters  )
    {
    TInt err( KErrNotFound );
        
    CArrayPtr<CHnActionModel>* actions = GetActions( aEventId );
    
    if ( aRecipientId != KErrNotFound )
        {       
        CHnItemModel** itemModel = iItemsModels.Find( aRecipientId );
        if ( itemModel && *itemModel )
            {
            err = (*itemModel)->OfferHnEventL( iEventHandler, aEventId,
                    aEventParameters);
            }
        }
    
    if ( err != KErrNone && actions) 
        {
        for( TInt i = 0; i < actions->Count(); i++ )
            {
            ASSERT( actions->At( i ) );
            err = actions->At( i )->ExecuteL( iEventHandler, aEventParameters );
            if ( err != KErrNone )
                {
                break;
                }
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CHnSuiteModel::ItemEvaluationFinishedL( TInt aModelItemNumber )
    {
    TBool observersNotified( EFalse );
    TBool alreadyNotified = iItemsOrder->IsSuiteReadyToShow();
    iItemsOrder->ItemEvaluated( aModelItemNumber );
    if ( iItemsOrder->IsSuiteReadyToShow() && !alreadyNotified )
        {
        TRAP_IGNORE( OfferHnEventL( KIdSuiteLoad, 
                GetItemsOrder()->GetSuiteId(), NULL ) );
        DEBUG16(("\t_Mm_:KIdSuiteLoad - %S",iSuiteName));
        NotifyObserversL( ESuiteModelInitialized );
        observersNotified = ETrue;
        }
    return observersNotified;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CHnSuiteModel::SuiteEvaluationFinishedL( )
    {
    TBool observersNotified( EFalse );
    TBool alreadyNotified = iItemsOrder->IsSuiteReadyToShow();
    iItemsOrder->SuiteEvaluated();
    if ( iItemsOrder->IsSuiteReadyToShow() && !alreadyNotified )
        {
        TRAP_IGNORE( OfferHnEventL( KIdSuiteLoad, 
                GetItemsOrder()->GetSuiteId(), NULL ) );
        DEBUG16(("\t_Mm_:KIdSuiteLoad - %S",iSuiteName));
        NotifyObserversL( ESuiteModelInitialized );
        observersNotified = ETrue;
        }
    return observersNotified;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnSuiteModel::GetItemModelsCount()
    {
    return iItemsModels.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnSuiteModel::SuiteName() const
	{
	return *iSuiteName;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnSuiteModel::Title() const
	{
	if ( iTitle )
		{
		return *iTitle;
		}
	else
		{
		return KNullDesC();
		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::SetTitleL( const TDesC& aName )
	{
	delete iTitle;
	iTitle = NULL;
	iTitle = aName.AllocL();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnSuiteModel::EmptyText() const
    {
    if ( iEmptyText )
        {
        return *iEmptyText;
        }
    else
        {
        return KNullDesC();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::SetEmptyTextL( const TDesC& aName )
    {
    delete iEmptyText;
    iEmptyText = NULL;
    iEmptyText = aName.AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnSuiteModel::WidgetType() const
	{
    //Following check is necessary because suite can be 
    //during evalutation -> UI Widgets can try to get and process templates
    //what causes problems. Only when suite is fully evaluated and 
    //ready to be shown it returns widget type.
	THnSuiteWidgetType ret( EUnspecified );
    if ( iItemsOrder->IsSuiteReadyToShow()  )
        {
        ret =  iWidgetType;
        }
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::SetWidgetType( THnSuiteWidgetType aWidgetType )
	{
	iWidgetType = aWidgetType;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C MHnMenuItemModelIterator* CHnSuiteModel::GetMenuStructureL( 
		TInt aItemId )
	{
	if (aItemId == KErrNotFound) 
	    {
	    // this could be used in future for empty item
	    return NULL;
	    }
	return iItemsModels.FindL( aItemId )->GetMenuStructure();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::AddItemL( TInt aId, CHnItemModel* aItemModel,
        TInt aItemModelNumber, TInt aItemModelPosition )
	{
	DEBUG16(("_MM_: CHnSuiteModel::AddItemL IN"));
	ASSERT( aItemModel );

	TInt highlightedItem( iSuiteHighlight.iCustomId );
	iItemsModels.InsertL( aId, aItemModel );
	
	// "empty" item should not be added to ordering table
	if ( !aItemModel->GetTemplate().Compare( KTemplateEmpty8 ) )
	    {
	    // store id of empty item
	    iEmptyItemId = aId;	    
	    }
	else
	    {
	    iItemsOrder->InsertItemIdToL( aItemModelNumber, aItemModelPosition, aId );
	    DEBUG16(("_MM_: Adding at index: model no. %D item no. %D, id: %D", aItemModelNumber, aItemModelPosition, aId));
	    
		// focus queue
		if ( ( iFocusQueue >= 0 ) && ( iFocusQueue == aItemModel->CustomId() ) )
			{
			SetSuiteHighlightL(  iItemsOrder->Count() - 1  );
			iFocusQueue = KErrGeneral ;
			}
		else if( aItemModel->CustomId() != KErrNotFound )
		    {
		    SetSuiteHighlightByCustomIdL( highlightedItem );
		    }
	    }
		
	DEBUG16(("_MM_: CHnSuiteModel::AddItemL OUT"));
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnSuiteModel::GetTemplate() const
    {
    //Following check is necessary because suite can be 
    //during evalutation -> UI Widgets can try to get and process templates
    //what causes problems. Only when suite is fully evaluated and 
    //ready to be shown it returns templates.
    if ( iItemsOrder->IsSuiteReadyToShow()  )
        {
        return iTemplate;
        }
    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnSuiteModel::SetTemplateL( const TDesC8& aValue )
    {
    if( aValue.Compare( iTemplate ) )
    	{
    	iTemplate.Close();
    	iTemplate.CreateL( aValue.Length() + 1);
    	iTemplate.Copy( aValue );
    	const char* templateChar = reinterpret_cast<const char* >( 
                iTemplate.PtrZ() );
    	}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::NotifyObserversL( THnCustomSuiteEvent aEventType )
    {
    RPointerArray<MHnSuiteObserver> observerFreezedArray;
    CleanupClosePushL( observerFreezedArray );
      
    for (TInt i(0); i < iSuiteObservers.Count(); i++)
        {
        observerFreezedArray.AppendL( iSuiteObservers[i] );
        }
    
    for (TInt i(0); i < observerFreezedArray.Count(); i++)
        {
        observerFreezedArray[i]->HandleSuiteEventL( aEventType, this );
        }
    
    CleanupStack::PopAndDestroy( &observerFreezedArray );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::RefreshMulModelL( TInt aPreviousItemCount )
    {   
    if (iItemsOrder->IsSuiteReadyToShow())
    	{
		TInt modelcount = iItemsOrder->Count();
		if ( aPreviousItemCount < modelcount )
			{
			NotifyObserversL(ESuiteItemsAdded);
			}
		else if ( aPreviousItemCount > modelcount )
			{
			NotifyObserversL(ESuiteItemsRemoved);
			}    
		else
			{
			NotifyObserversL(ESuiteItemsUpdated);
			}
    	}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::UpdateItemTemplateL( TInt aId )
	{
	TInt position = iItemsOrder->GetItemPosition( aId );
	if( position != KErrNotFound )
		{
		CHnItemModel** model = iItemsModels.Find( aId );
		if (model && !(*model)->GetTemplate().Compare( KNullDesC8() ) )
            {
            (*model)->SetTemplateL( GetTemplate() );
            }
		}
	}
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::RegisterSuiteObserverL( MHnSuiteObserver* aObserver, 
        TThreadPriority aPriority )
    {
    TInt i(0);
    TInt posFound = iSuiteObservers.Find( aObserver );
    if ( posFound != KErrNotFound )
        {
        iObserverPriorities.Remove( posFound );
        iSuiteObservers.Remove( posFound ); 
        }
    
    for (; i < iObserverPriorities.Count(); i++)
        {
        if (aPriority > iObserverPriorities[i])
            {
            break;
            }
        }
    
    iObserverPriorities.InsertL( aPriority , i );
    iSuiteObservers.InsertL( aObserver, i );
    
    if ( iItemsOrder->IsSuiteReadyToShow() && posFound == KErrNotFound )
        {
        aObserver->HandleSuiteEventL( ESuiteModelInitialized, this );
        }
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::UnregisterSuiteObserver( MHnSuiteObserver* aObserver)
    {
    TInt index = iSuiteObservers.Find( aObserver );
    if (index != KErrNotFound)
        {
        iSuiteObservers.Remove( index );
        iObserverPriorities.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::RemoveItemL( TInt aId )
	{
    TInt highlightedItem( KErrNotFound );
	CHnItemModel* modelToRemove = GetItemModel( aId );
	if( modelToRemove )
	    {
	    if( modelToRemove->CustomId() != KErrNotFound )
	        {
	        if (modelToRemove->CustomId() != iSuiteHighlight.iCustomId )
				{
		        highlightedItem = iSuiteHighlight.iCustomId;
				}
	        else
	        	{
//	        	set the highlight to the item before the removed item.
	        	TInt newWidgetIndex = iItemsOrder->Count() - 2;
	        	if( newWidgetIndex < 0 )
	        		{
	        		newWidgetIndex = 0;
	        		}
	        	SetSuiteHighlightL( newWidgetIndex );
	        	}
	        }
	    delete modelToRemove;
	    iItemsModels.Remove( aId );
	    }
    iItemsOrder->RemoveItemId( aId );
    SetSuiteHighlightByCustomIdL( highlightedItem );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::RemoveAllItems()
	{
	THashMapIter< TInt ,CHnItemModel* > iterator( iItemsModels );
	while( iterator.NextKey() )
		{
		CHnItemModel** model = iterator.CurrentValue();
		delete *model;
		iterator.RemoveCurrent();
		}
	}
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C RHashMap< TInt, CArrayPtr<CHnActionModel>* >& 
    CHnSuiteModel::GetActions()
    {
    return iActions;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C CArrayPtr<CHnActionModel>* CHnSuiteModel::GetActions( TInt aId )
	{
	CArrayPtr<CHnActionModel>** ret = iActions.Find( aId );
	if( ret )
		{
		return *ret;
		}
	return NULL;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CHnItemModel* CHnSuiteModel::GetItemModel( TInt aId )
	{
    TRAP_IGNORE( UpdateItemTemplateL( aId ) );
    
	CHnItemModel** model = iItemsModels.Find( aId );
	return ( model ) ? *model : NULL;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CHnSuiteModel::HasToolbar() const
	{
	TBool hasToolbar = EFalse;
	THashMapIter< TInt ,CHnItemModel* > modelIter( iItemsModels );
	while( modelIter.NextKey() )
		{
		CHnItemModel** model = modelIter.CurrentValue();
		const CHnToolbarModel * toolbarModel = ( *model )->GetToolbarModel();
		hasToolbar = (toolbarModel) ? toolbarModel->HasButtons() : EFalse;
		if( hasToolbar )
			{
			break;
			}
		}
	return hasToolbar;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CHnSuiteModel::IdByIndex( TInt aIndex )
	{
	TInt result(KErrNotFound);
	
	if (aIndex < 0)
	    {
	    result = iEmptyItemId;
	    }
    else
        {
        result = iItemsOrder->GetItemIdAt( aIndex );
	    }

    return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CHnSuiteModel::IsItemMoveLocked( TInt aIndex )
	{
	TInt ret( EFalse );

	if (aIndex >= 0)
	    {
    	CHnItemModel** itemModel = iItemsModels.Find( 
    	        iItemsOrder->GetItemIdAt( aIndex ) );
    	
    	if ( !itemModel || (*itemModel)->IsMoveLocked() )
    		{
    		ret = ETrue;
    		}
	    }
	
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TMcsItemType CHnSuiteModel::GetItemType( TInt aIndex )
	{
	TMcsItemType type = EItemTypeUnknown;
	
	CHnItemModel** itemModel = iItemsModels.Find( 
	        iItemsOrder->GetItemIdAt( aIndex ) );

	if ( itemModel )
		{
		type = (*itemModel)->GetItemType();
		}

	return type;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CHnSuiteModel::GetSuiteHighlight()
    {
    return iSuiteHighlight.iWidgetIndex;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnSuiteModel::SetSuiteHighlightL( TInt aHighLight )
    {
    DEBUG(("_MM_:CHnSuiteModel::SetSuiteHighlight - IN"));
    DEBUG(("\t_MM_:aHighLight: %d",aHighLight));
    
    TInt newItemId( IdByIndex( aHighLight ) );
    CHnItemModel* itemModel( GetItemModel( newItemId ) );
    TInt64 newCustomId( KErrNotFound );
    if ( itemModel )
    	{
	    newCustomId = itemModel->CustomId();
    	}
    
    if ( ( newCustomId != iSuiteHighlight.iCustomId ||
            newItemId != iSuiteHighlight.iItemId ) && ( iIsActive && iIsVisible ) )
        {
        HandleItemFocusL( iSuiteHighlight.iItemId, EFalse );
        HandleItemFocusL( newItemId, ETrue );
        }
    
    iSuiteHighlight.iCustomId = newCustomId;
    iSuiteHighlight.iItemId = newItemId;
    iSuiteHighlight.iWidgetIndex = aHighLight;
    
    DEBUG(("_MM_:CHnSuiteModel::SetSuiteHighlight - OUT"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CHnSuiteModel::SetSuiteHighlightByCustomIdL( TInt64 aCustomId )
    {
    DEBUG(("_MM_:CHnSuiteModel::SetSuiteHighlightByCustomIdL - IN"));
    DEBUG(("\t_MM_:aHighLight: %d",aCustomId));
    
    if( aCustomId != KErrNotFound )
        {
        THashMapIter< TInt ,CHnItemModel* > modelIter( iItemsModels );
        while( modelIter.NextKey() )
            {
            const TInt* id = modelIter.CurrentKey();
            CHnItemModel** model = modelIter.CurrentValue();
            if( (*model)->CustomId() == aCustomId )
                {
                if( *id != KErrNotFound )
                    {
                    TInt itemIndex( iItemsOrder->GetItemPosition( *id ) );
                    if( itemIndex != KErrNotFound )
                        {
                        SetSuiteHighlightL( itemIndex );
                        }
                    }
                break;
                }
            }
        }
    
    DEBUG(("_MM_:CHnSuiteModel::SetSuiteHighlightByCustomIdL - OUT"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CHnSuiteModel::IsItemBetweenMoveLocked( TInt aIndexStart, TInt aIndexEnd )
	{
	if ( aIndexEnd < KErrNone || aIndexStart < KErrNone )
	    {
	    return ETrue;
	    }
	if ( aIndexEnd >= iItemsOrder->Count() )
		{
		aIndexEnd = iItemsOrder->Count() - 1;
		}
	
	TInt start( ( aIndexStart > aIndexEnd ) ? aIndexEnd : aIndexStart );
	TInt end( ( aIndexStart > aIndexEnd ) ? aIndexStart : aIndexEnd );
	
	TInt ret( EFalse );
	
	for ( TInt index = start; index <= end; index++ )
		{
		CHnItemModel** itemModel = iItemsModels.Find( 
		        iItemsOrder->GetItemIdAt( index ) );
		if ( !itemModel || (*itemModel)->IsMoveLocked() )
			{
			ret = ETrue;
			break;
			}
		}
	return ret;
	}
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CHnSuiteModel::ReorderItemsL( TInt aFromIndex, TInt aToIndex )
    {
    DEBUG( ("_MM_: CHnSuiteModel::ReorderItems - IN" ) );

    TInt operation ( (aFromIndex < aToIndex) ? 1 : -1 );
    TInt realFromId = iItemsOrder->GetItemIdAt(aFromIndex);
    for ( int i = aFromIndex ; (operation == 1 && i < aToIndex) 
            || (operation == -1 && i > aToIndex) ; i = i + operation )
        {
        if ( (i+operation < 0) || (i+operation >= GetItemModelsCount()) )
            break;
        
        iItemsOrder->SetItemIdAtL(i, iItemsOrder->GetItemIdAt(i+operation));
        }
    
    iItemsOrder->SetItemIdAtL(aToIndex, realFromId);
    
    // recalculate current active item
    SetSuiteHighlightL( GetSuiteHighlight() );
        
    DEBUG( ("_MM_: CHnSuiteModel::ReorderItems - OUT" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CHnItemsOrder* CHnSuiteModel::GetItemsOrder()
    {
    return iItemsOrder;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnSuiteModel::QueueFocus( TInt aCustomId )
	{
	iFocusQueue = aCustomId;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnSuiteModel::SetCustomId( TInt64 aCustomId )
	{
	iCustomId = aCustomId;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt64 CHnSuiteModel::CustomId()
	{
	return iCustomId;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnSuiteModel::SetActiveL( TBool aActive )
    {
    DEBUG16(("_MM_:CHnSuiteModel::SetActiveL %S - IN",iSuiteName));
    DEBUG(("\t_MM_:aActive: %d",aActive));
    DEBUG(("\t_MM_:iIsActive: %d",iIsActive));
    
    if ( aActive != iIsActive )
        {
        iIsActive = aActive;
        if ( iIsVisible )
            {
            HandleItemFocusL( iSuiteHighlight.iItemId, aActive );
            }
        }
    
    DEBUG(("_MM_:CHnSuiteModel::SetActiveL - OUT"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CHnSuiteModel::IsActive()
    {
    return iIsActive;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::SetVisibleL( TBool aVisible )
    {
    DEBUG16(("_MM_:CHnSuiteModel::SetVisible %S - IN",iSuiteName));
    DEBUG(("\t_MM_:aVisible: %d",aVisible));
    DEBUG(("\t_MM_:iIsVisible: %d",iIsVisible));
    
    if ( aVisible != iIsVisible )
        {
        iIsVisible = aVisible;
        if ( iIsActive )
            {
            HandleItemFocusL( iSuiteHighlight.iItemId, aVisible );
            }
        }
    
    DEBUG(("_MM_:CHnSuiteModel::SetVisible - OUT"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnSuiteModel::HandleItemFocusL( TInt aItemId, TBool aFocused )
    {
    DEBUG16(("_MM_:CHnSuiteModel::HandleItemFocusL %S - IN",iSuiteName));
    DEBUG(("\t_MM_:aItemIndex: %d",aItemId));
    DEBUG(("\t_MM_:aFocused: %d",aFocused));
    
    if ( aItemId >= 0 )
        {
        if ( !iItemFocusHandler )
        	{
        	iItemFocusHandler = CHnItemFocusHandler::NewL( this );
        	}
        TInt eventId = aFocused ? KIdFocusGain : KIdFocusLost;
        iItemFocusHandler->SetFocusL( eventId, aItemId );
//        TRAP_IGNORE( OfferHnEventL( eventId, aItemId, NULL ) );
        }
    
    DEBUG(("_MM_:CHnSuiteModel::HandleItemFocusL - OUT"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnItemModel* CHnSuiteModel::GetMatchingItemModelL( TInt64 aCustomId,
		TInt& aIndex )
	{
	CHnItemModel* itemModel( NULL );
	
	// check suiteCustomId
	if ( aCustomId >= 0 )
		{
	    for ( TInt i( 0 ); i < GetItemModelsCount(); i++ )
	    	{
	    	TInt itemModelId = IdByIndex( i );
	    	CHnItemModel* tmpItemModel = GetItemModel( itemModelId );
	
	    	if ( tmpItemModel &&
	    			tmpItemModel->CustomId() == aCustomId )
	    		{
	    		aIndex = i;
	    		itemModel = tmpItemModel;
	    		break;
	    		}
	    	}
		}
	
	return itemModel;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModel::SetExitMode( TExitMode aExitMode )
	{
	iExitMode = aExitMode;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnSuiteModel::RemoveLiwObjects()
    {
    for ( TInt i = 0; i < GetItemModelsCount(); ++i )
        {
        CHnItemModel* itemModel = GetItemModel( IdByIndex( i ) );
        if ( itemModel )
            {
            itemModel->RemoveLiwObjects();
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TExitMode CHnSuiteModel::ExitMode()
	{
	return iExitMode;
	}


// End of file
