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
*  Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
*/
#ifndef HNITEMFOCUSHANDLER_H
#define HNITEMFOCUSHANDLER_H

#include <e32base.h>
#include <e32std.h>

class CHnSuiteModel;

/**
 *  Class representing the focus item to be queued.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
class TFocusQueueItem
	{
public:
	
    /**
     * Constructor. Sets the trigger time on construction.
     * 
     * @since S60 v5.0
     * @param aItemId Index of the item in the suite model.
     */
	TFocusQueueItem( TInt aItemId, TInt aEventId );
	
    /**
     * Fetches the focus item trigger time.
     * 
     * @since S60 v5.0
     * @return Time when the event was triggered.
     */
	TTime TriggerTime();
	
    /**
     * Fetches the focus/unfocus item id.
     * 
     * @since S60 v5.0
     * @return Suite model item id of the focused/unfocused item.
     */
	TInt ItemId();
	
    /**
     * Fetches the focus/unfocus item id.
     * 
     * @since S60 v5.0
     * @return Suite model item id of the focused/unfocused item.
     */
	TInt EventId();
	
    /**
     * Compares queued items by triggered time.
     * 
     * @since S60 v5.0
     * @param aItem1 First item to compare.
     * @param aItem2 Second item to compare.
     * @return 1 if aItem1 was triggered earlier than aItem2.
     * -1 if aItem1 was triggered later than aItem2.
     * 0 if aItem1 was triggered at the same time as aItem2.
     */
	static TInt CompareByTriggerTime( const TFocusQueueItem& aItem1,
			const TFocusQueueItem& aItem2 );
    /**
     * Compares queued items by triggered time.
     * 
     * @since S60 v5.0
     * @param aItem1 First item to compare.
     * @param aItem2 Second item to compare.
     * @return ETrue if aItem1 is equal to aItem2 regarding the item id.
     * EFalse if aItem1 is not equal to aItem2 regarding the item id.
     */
	static TBool CompareByItemId( const TFocusQueueItem& aItem1,
			const TFocusQueueItem& aItem2 );

private:
	
    /**
     * Item id.
     * Own.
     */
	TInt iItemId;
    
	/**
     * Trigger time.
     * Own.
     */
	TTime iTriggerTime;
    
	/**
     * Event type.
     */
	TInt iEventId;
	
	};

/**
 *  Item focus handler class.
 *
 *  This is an active object which handles focus/unfocus event triggering.
 *  This class causes focus/unfocus events to be run without performance impact
 *  on the UI. The focus/unfocus events are queued. If incoming events logically 
 *  cancel the queued events, the appropriate events are removed from the queue.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnItemFocusHandler ) : public CActive
	{
public:
	
    /**
    * Virtual Destructor.
    * 
    * @since S60 5.0
    */
	~CHnItemFocusHandler();

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aSuiteModel The suite model for which focus events are handled.
     */
	static CHnItemFocusHandler* NewL( CHnSuiteModel* aSuiteModel );

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aSuiteModel The suite model for which focus events are handled.
     */
	static CHnItemFocusHandler* NewLC( CHnSuiteModel* aSuiteModel );

public:
	
    /**
     * Called to initiate item focus action handling.
     * 
     * @since S60 v5.0
     * @param aEventId The type of event. Can be KIdFocusGain or KIdFocusLost. Otherwise focus actions are not triggered.
     * @param aItemId Index of the item in the suite model.
     */
	void SetFocusL( TInt aEventId, TBool aItemId );

private:
	
    /**
     * C++ default constructor.
     * 
     * @since S60 v5.0
     */
	CHnItemFocusHandler();

    /**
     * Second-phase constructor.
     * 
     * @since S60 v5.0
     * @param aSuiteModel The suite model for which focus events are handled.
     */
	void ConstructL( CHnSuiteModel* aSuiteModel );

private:
	
    /**
     * Handles completion. In fact the request is completed immediately after activating the object.
     * This method calls focus/unfocus action events.
     * 
     * @since S60 v5.0
     */
	void RunL();

    /**
     * Run when request is cancelled.
     */
	void DoCancel();

    /**
     * Overriden to handle leaves from RunL(). Default implementation causes
     * the active scheduler to panic.
     * 
     * @since S60 v5.0
     * @param aError The Error code.
     * @return Id The eroor code.
     */
	TInt RunError(TInt aError);
	
    /**
     * Removes duplicated items. E.g. if a request to focus an item has been triggered
     * when there is still a request to unfocus that item pending in the queue. Then the unfocus 
     * event will be removed from the queue and the event and only the focus event is triggered.
     * 
     * @since S60 v5.0
     * @param aFocusItem The item to be compared.
     * @param aEventId focus or unfocus event.
     */
	void RemoveDuplicatedItem( const TFocusQueueItem& aFocusItem );

private:
	
    /**
     * The suite model for which focus events are handled.
     * Not own.
     */
	CHnSuiteModel* iSuiteModel;
	
    /**
     * Focus Queue.
     * Own.
     */
	RArray<TFocusQueueItem> iFocusQueue;
	
	};

#endif // HNITEMFOCUSHANDLER_H
