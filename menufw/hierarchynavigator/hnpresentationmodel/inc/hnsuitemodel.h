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
* Description:   suite presentation model
*
*/


#ifndef C_HNSUITEMODEL_H
#define C_HNSUITEMODEL_H

#include <e32base.h>
#include <e32hashtab.h>
#include "hnsuiteobserver.h"
#include "hnglobals.h"
#include "hnitemmodel.h"

class CHnSuiteModelContainer;
class CHnItemModel;
class MHnMenuItemModelIterator;
class CHnActionModel;
class CHnEventHandler;
class MHnSuiteObserver;
class CHnItemsOrder;
class CHnItemFocusHandler;

/**
 *  Suite model class.
 *
 *  Represents suite. Evaluated from CHnMdSuite class, contains
 *  all data from services and xml configuration. It is used to fill data
 *  displayed on the screen.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnSuiteModel ) : public CBase
    {
    
public:
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aEventHandler Event handler.
     * @param aGenreName Suite genre identifier.
     */
    IMPORT_C static CHnSuiteModel* NewL( CHnEventHandler& aEventHandler,
            const TDesC& aGenreName );
    
    /**
    * Virtual Destructor.
    * 
    * @since S60 5.0
    */
    IMPORT_C ~CHnSuiteModel();
 
    /**
     * Return Suite's genre name.
     * Used for identifying the Suite.
     * 
     * @since S60 v5.0
     * @return The genre name.
     */
    IMPORT_C const TDesC& SuiteName() const;

    /**
     * Return Suite's name
     * This should be displayed in Status Pane
     * 
     * @since S60 v5.0
     * @return the name
     */
    IMPORT_C const TDesC& Title() const;
    
    /**
     * Sets the Suite's name
     * 
     * @since S60 v5.0
     * @param aName the name
     */
    IMPORT_C void SetTitleL( const TDesC& aName );
    
    /**
     * Return Suite's empty text
     * 
     * @since S60 v5.0
     * @return empty text if defined o
     */
    IMPORT_C const TDesC& EmptyText() const;
    
    /**
     * Sets the Suite's empty text
     * 
     * @since S60 v5.0
     * @param aName the name
     */
    IMPORT_C void SetEmptyTextL( const TDesC& aName );

    /**
     * Notifies all of the observers.
     * 
     * @since S60 v5.0
     * @param aEventType Event to be pushed to all observers.
     */    
    IMPORT_C void NotifyObserversL( THnCustomSuiteEvent aEventType );

    /**
     * Method used to indicate that item evaluation has finished.
     * 
     * @since S60 v5.0
     * @param aModelItemNumber Index of the item model.
     * @return True if observers were notified.
     */    
    IMPORT_C TBool ItemEvaluationFinishedL( TInt aModelItemNumber );
    
    /**
     * Method used to indicate that item evaluation has finished.
     * 
     * @since S60 v5.0
     */    
    IMPORT_C TBool SuiteEvaluationFinishedL();
    
    /**
     * Return Suite's Widget type
     * 
     * @since S60 v5.0
     * @return the Widget type or EUnspecified if not set
     */
    IMPORT_C TInt WidgetType() const;
    
    /**
     * Sets the Suite's Widget type
     * Also sets the correct template to the Widget Model
     * 
     * @since S60 v5.0
     * @param aWidgetType Suite's Widget type
     */
    IMPORT_C void SetWidgetType( THnSuiteWidgetType aWidgetType );
    
    /**
     * Gets menu structure for certain model
     *  
     * @since S60 v5.0
     * @param aItemId Id of the item model
     * @return Pointer to menu model
     */
    IMPORT_C MHnMenuItemModelIterator* GetMenuStructureL( TInt aItemId );
    
    /**
     * Adds item both to Aakash model and Matrix model.
     * 
     * @since S60 v5.0
     * @param aItemModel Item model.
     * @param aId Item id
     * @param aItemModelNumber Index of data model that creates the item.
     * @param aItemModelPosition Position in the particular data model
     */
    IMPORT_C void AddItemL( TInt aId, CHnItemModel* aItemModel,
            TInt aItemModelNumber, TInt aItemModelPosition );
    
    /**
     * Updates item template with template from suite model.
     * 
     * @since S60 v5.0
     * @param aId Id of the item to update template
     */
    IMPORT_C void UpdateItemTemplateL( TInt aId );
    
    /**
     * Updates item in Aakash model.
     * 
     * @since S60 v5.0
     * @param aPreviousItemCount Previous number of items in the suite.
     */
    IMPORT_C void RefreshMulModelL( TInt aPreviousItemCount = 0 );
    
    /**
     * Sets template for suite.
     * 
     * @since S60 v5.0
     * @param aValue template of the suite
     */
    IMPORT_C void SetTemplateL( const TDesC8& aValue );
    
    /**
     * Gets template.
     * 
     * @since S60 v5.0
     * @return template of the suite
     */
    IMPORT_C const TDesC8& GetTemplate() const;
    
    /**
     * Remove model by id both from Aakash model and Matrix model.
     * 
     * @since S60 v5.0
     * @param aId Id of the item to remove
     */
    IMPORT_C void RemoveItemL( TInt aId );
    
    /**
     * Removes all items from the suite model.
     * 
     * @since S60 v5.0
     */
    IMPORT_C void RemoveAllItems();
    
    /**
     * Gets item model id by its position in Aakash model.
     * 
     * @since S60 v5.0
     * @param aIndex Index of visual item in Aakash model
     * @return Id of the item model for input parameter
     */
    IMPORT_C TInt IdByIndex( TInt aIndex );
    
    /**
     * Gets item model count.
     * 
     * @since S60 v5.0
     * @return Count of item models.
     */
    IMPORT_C TInt GetItemModelsCount();
    
    /**
     * Gets actions models.
     * 
     * @return List of all actions for every event.
     */
    IMPORT_C RHashMap< TInt, CArrayPtr<CHnActionModel>* >& GetActions();
    
    /**
     * Gets actions models.
     * 
     * @param aId Event id
     * @return Array of actions models corresponding to event id
     */
    IMPORT_C CArrayPtr<CHnActionModel>* GetActions( TInt aId );
    
    /**
     * Gets item model.
     * 
     * @param aId Item model id
     * @return Item model corresponding to id, if not found returns NULL
     */
    IMPORT_C CHnItemModel* GetItemModel( TInt aId );
    
    /**
     * Returns visibility of toolbar for suite
     * 
     * @return TRUE if visible, otherwise FALSE
     */
    IMPORT_C TBool HasToolbar() const;
    
    /**
     * Registers suite observer.
     * 
     * @param aObserver observer
     * @param aPriority priority
     */
    IMPORT_C void RegisterSuiteObserverL( MHnSuiteObserver* aObserver, 
            TThreadPriority aPriority = EPriorityNormal);
    
    /**
     * Unregisters suite observer
     *
     * @param aObserver observer 
     */
    IMPORT_C void UnregisterSuiteObserver( MHnSuiteObserver* aObserver);
    
    /**
     * Checks if one of items between aIndexStart and
     * aIndexEnd is move-locked
     * 
     * @param aIndexStart starting item
     * @param aIndexEnd ending item
     * @return true if one of the preceeding items is locked 
     */
    IMPORT_C TBool IsItemBetweenMoveLocked( TInt aIndexStart, 
            TInt aIndexEnd );
    
    /**
     * Handles event.
     *
     * @since S60 5.0
     * @param aEventId Event id.
     * @param aRecipientId Recipient Id.
     * @param aEventParameters Parameters to event.
     * @return Status code.
     */
    TInt OfferHnEventL( 
            const TInt aEventId, const TInt aRecipientId, 
             CLiwGenericParamList* aEventParameters  );
    
    /**
     * Reorders item.
     * 
     * @param aFromIndex Index of the item to move.
     * @param aToIndex Index of the item to shift to.
     */
    IMPORT_C TInt ReorderItemsL( TInt aFromIndex, TInt aToIndex );
    
    /**
     * Returns items order object.
     * 
     * @return items order object from suite model
     */
    IMPORT_C CHnItemsOrder* GetItemsOrder();
    
public:
    
    /**
     * Returns true if an item is move locked.
     * 
     * @param aIndex item's index
     * @return true if an item is move-locked otherwise false
     */
    IMPORT_C TBool IsItemMoveLocked( TInt aIndex );
    
    /**
     * Returns type of an item.
     * 
     * @param aIndex an item's index
     * @return item's type
     */
    IMPORT_C TMcsItemType GetItemType( TInt aIndex );
    
    /**
     * Returns highlighted item's id.
     * 
     * @since S60 v5.0
     * @return highlighted item's id
     */
    IMPORT_C TInt GetSuiteHighlight();
    
    /**
     * Set highlighted item's id.
     * 
     * @since S60 v5.0
     * @param aHighLight sets highlighted item's id
     */
    IMPORT_C void SetSuiteHighlightL( TInt aHighLight );
	
    /**
     * Add items index to focus queue. Item will be highlighted/focused
     * during evaluation.
     * 
     * @since S60 v5.0
     * @param aCustomId Custom Id of an item which will be highlighted
     */
    IMPORT_C void QueueFocus( TInt aCustomId );
    
    /**
     * Sets suite's custom id.
     * 
     * @since S60 v5.0
     * @param aCustomId Custom identifier to be set.
     */
    IMPORT_C void SetCustomId( TInt64 aCustomId );
    
    /**
     * Returns custom identifier of the suite.
     * 
     * @since S60 v5.0
     * @return Custom identifier of the suite.
     */
    IMPORT_C TInt64 CustomId();

    /**
     * Returns item model matching given criteria.
     * 
     * @param aCustomId Item's custom id.
     * @param aIndex
     * @return Item model matching given custom_id, otherwise null.
     */
    IMPORT_C CHnItemModel* GetMatchingItemModelL( TInt64 aCustomId, TInt& aIndex );
    
    /**
     * Returns the suite's exit mode flag.
     */
    IMPORT_C TExitMode ExitMode();
    
    /**
     * Sets the exid mode.
     */
    IMPORT_C void SetExitMode( TExitMode aExitMode );
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    void RemoveLiwObjects();
    
private:

    /**
     * Struct holding highlight information.
     * iWidgetIndex - index of the visible item in the UI widget.
     * iItemId - presentation model ID of the item.
     * iCustomId - custom ID from data provider.
     * 
     * @since S60 v5.0
     */
    struct TSuiteHighlight
        {
        /** Widget index. */
        TInt iWidgetIndex;
        /** Item Id. */
        TInt iItemId;
        /** Custom Id. */
        TInt64 iCustomId;
        };

    /**
     * C++ default constructor.
     * 
     * @since S60 v5.0
     * @param aEventHandler Event handler.
     */
    CHnSuiteModel( CHnEventHandler& aEventHandler );
    
    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 v5.0
     * @param aGenreName Genre name for model
     */
    void ConstructL( const TDesC& aGenreName );

    /**
     * Set highlight on item.
     * 
     * @since S60 v5.0
     * @param aCustomId Custom id of the item.
     */
    void SetSuiteHighlightByCustomIdL( TInt64 aCustomId );
    
private:

    /**
     * Genre name.
     * Own.
     */
    HBufC* iSuiteName;
    
    /**
     * Suite name.
     * Own.
     */
    HBufC* iTitle;

    /**
     * Suite empty text
     * Own.
     */
    HBufC* iEmptyText;
    
    /**
     * Suite Widget type
     */
    THnSuiteWidgetType iWidgetType;
    
    /**
     * Template.
     */
    RBuf8 iTemplate;

    /**
     * Holds models evaluated from CHnMdItem objects
     */
    RHashMap< TInt, CHnItemModel* > iItemsModels;

    /**
     * Holds positions of item ids and suite id.
     * Own.
     */
    CHnItemsOrder* iItemsOrder;

    /**
     * Actions corresponding to events
     * event id <=> array of actions models
     */
    RHashMap< TInt, CArrayPtr<CHnActionModel>* > iActions;
    
    /**
     * Id of "empty" item
     */
    TInt iEmptyItemId;
    
    /**
     * Suite observers.
     * Not own.
     */
    RPointerArray<MHnSuiteObserver> iSuiteObservers;
    
    /**
     * Observer priorities array.
     */
    RArray<TThreadPriority> iObserverPriorities;
    
    /**
     * Stores IDs of currently higlighted item.
     */ 
    TSuiteHighlight iSuiteHighlight;
    
    /**
     * Focus queue.
     */
    TInt iFocusQueue;
    
    /**
     * Custom id.
     */
    TInt64 iCustomId;
    
    /**
     * Event Handler from Suite Container.
     * Used for executing item focused/unfocused events.
     * 
     * Not Own.
     */ 
    CHnEventHandler& iEventHandler;
    
    /**
     * Exit mode.
     */
    TExitMode iExitMode;
    /**
     * Active Focus Handler.
     */
    CHnItemFocusHandler* iItemFocusHandler;
    };

#endif // C_HNSUITEMODEL_H
