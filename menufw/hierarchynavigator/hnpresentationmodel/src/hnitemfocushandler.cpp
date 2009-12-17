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
* Description:  Application UI class
*  Version     : %version: 3 % << Don't touch! Updated by Synergy at check-out.
*/


#include "hnitemfocushandler.h"
#include "hnglobals.h"
#include "hnsuitemodel.h"

// ---------------------------------------------------------------------------
// TFocusQueueItem::TFocusQueueItem( TInt aItemId, TInt aEventId )
// ---------------------------------------------------------------------------
//
TFocusQueueItem::TFocusQueueItem( TInt aItemId, TInt aEventId ): 
	iItemId( aItemId ),  iEventId( aEventId )
	{
	iTriggerTime.HomeTime();
	}

// ---------------------------------------------------------------------------
// TFocusQueueItem::TriggerTime()
// ---------------------------------------------------------------------------
//
TTime TFocusQueueItem::TriggerTime()
	{
	return iTriggerTime;
	}

// ---------------------------------------------------------------------------
// TFocusQueueItem::ItemId()
// ---------------------------------------------------------------------------
//
TInt TFocusQueueItem::ItemId()
	{
	return iItemId;
	}

// ---------------------------------------------------------------------------
// TFocusQueueItem::EventId()
// ---------------------------------------------------------------------------
//
TInt TFocusQueueItem::EventId()
	{
	return iEventId;
	}

// ---------------------------------------------------------------------------
// TFocusQueueItem::CompareByTriggerTime( const TFocusQueueItem& aItem1,
// const TFocusQueueItem& aItem2 )
// ---------------------------------------------------------------------------
//
TInt TFocusQueueItem::CompareByTriggerTime( const TFocusQueueItem& aItem1,
		const TFocusQueueItem& aItem2 )
	{
	if ( aItem1.iTriggerTime < aItem2.iTriggerTime )
		{
		return -1;
		}
	else
		{
		return 1;
		}
	}
   
// ---------------------------------------------------------------------------
// TFocusQueueItem::CompareByItemId( const TFocusQueueItem& aItem1,
// const TFocusQueueItem& aItem2 )
// ---------------------------------------------------------------------------
//
TBool TFocusQueueItem::CompareByItemId( const TFocusQueueItem& aItem1,
		const TFocusQueueItem& aItem2 )
	{
	return aItem1.iItemId == aItem2.iItemId;
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::CHnItemFocusHandler() 
// ---------------------------------------------------------------------------
//
CHnItemFocusHandler::CHnItemFocusHandler() :
	CActive(EPriorityLow) // Standard priority
	{
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler* CHnItemFocusHandler::NewLC()
// ---------------------------------------------------------------------------
//
CHnItemFocusHandler* CHnItemFocusHandler::NewLC( CHnSuiteModel* aSuiteModel )
	{
	CHnItemFocusHandler* self = new (ELeave) CHnItemFocusHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aSuiteModel );
	return self;
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler* CHnItemFocusHandler::NewL()
// ---------------------------------------------------------------------------
//
CHnItemFocusHandler* CHnItemFocusHandler::NewL( CHnSuiteModel* aSuiteModel )
	{
	CHnItemFocusHandler* self = CHnItemFocusHandler::NewLC( aSuiteModel );
	CleanupStack::Pop(); // self;
	return self;
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::ConstructL()
// ---------------------------------------------------------------------------
//
void CHnItemFocusHandler::ConstructL( CHnSuiteModel* aSuiteModel )
	{
	CActiveScheduler::Add( this); // Add to scheduler
	iSuiteModel = aSuiteModel;
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::~CHnItemFocusHandler(
// ---------------------------------------------------------------------------
//
CHnItemFocusHandler::~CHnItemFocusHandler()
	{
	Cancel(); // Cancel any request, if outstanding
	
//	handle all pending unfocus events synchronously.
	for(TInt i = 0; i < iFocusQueue.Count(); i++ )
		{
		if ( KIdFocusLost == iFocusQueue[i].EventId() )
			{
			if( iSuiteModel )
			    {
	            TRAP_IGNORE( iSuiteModel->OfferHnEventL( 
	                    KIdFocusLost, iFocusQueue[i].ItemId(), NULL ) );
			    }
			}
		}
	iFocusQueue.Reset();
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::DoCancel()
// ---------------------------------------------------------------------------
//
void CHnItemFocusHandler::DoCancel()
	{
//	TRequestStatus* status = &iStatus;
//	User::RequestComplete( status, KErrCancel);
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::StartL(TTimeIntervalMicroSeconds32 aDelay)
// ---------------------------------------------------------------------------
//
void CHnItemFocusHandler::SetFocusL( TInt aEventId, TBool aItemId )
	{
	TLinearOrder<TFocusQueueItem> byTriggerTime( 
			TFocusQueueItem::CompareByTriggerTime );
	TFocusQueueItem focusItem(aItemId, aEventId);
	RemoveDuplicatedItem( focusItem );
	iFocusQueue.InsertInOrderL( focusItem, byTriggerTime );
	if ( !IsActive() )
		{
		SetActive();
    	TRequestStatus* status = &iStatus;
    	User::RequestComplete( status, KErrNone );
		}
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::RunL()
// ---------------------------------------------------------------------------
//
void CHnItemFocusHandler::RunL()
	{
	TRAP_IGNORE( iSuiteModel->OfferHnEventL( 
			iFocusQueue[0].EventId(), iFocusQueue[0].ItemId() , NULL ) );
	iFocusQueue.Remove(0);
	
	if ( iFocusQueue.Count() > 0)
		{
		SetActive(); // Tell scheduler a request is active		
    	TRequestStatus* status = &iStatus;
    	User::RequestComplete( status, KErrNone );
		}
	}

// ---------------------------------------------------------------------------
// CHnItemFocusHandler::RunError(TInt aError)
// ---------------------------------------------------------------------------
//
TInt CHnItemFocusHandler::RunError(TInt aError)
	{
	return aError;
	}

// ---------------------------------------------------------------------------
// RemoveDuplicatedItem( const TFocusQueueItem& aFocusItem )
// ---------------------------------------------------------------------------
//
void CHnItemFocusHandler::RemoveDuplicatedItem( const TFocusQueueItem& aFocusItem )
	{
	if ( iFocusQueue.Count() != 0 )
		{
		TIdentityRelation< TFocusQueueItem > byItemId( 
				TFocusQueueItem::CompareByItemId );
		TInt itemPosition;
		itemPosition = iFocusQueue.Find( aFocusItem, byItemId );
		if ( KErrNotFound != itemPosition )
			{
			iFocusQueue.Remove( itemPosition );
			}
		}
	}

// End of file

