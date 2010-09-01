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



#ifndef C_HNMDITEM_H
#define C_HNMDITEM_H

#include <e32base.h>

#include "hnmdmodel.h"
#include "hnmdeventmapping.h"
#include "hnqueryresultcollector.h"

class CHnMdQuery;
class CHnMdButton;
class CHnMdQueries;
class CHnMdNotifyRequests;
class CHnSuiteModel;
class CLiwGenericParamList;
class CLiwDefaultMap;
class THnIdGenerator;
class CHnFilter;
class CHnMdUiMapping;
class CHnMdMenuItem;
class CHnItemId;
class CHnMdSuite;
class CHnXmlModelProvider;
class CHnItemModel;
class CHnMenuItemModel;
class CHnActionModel;
class CHnMdToolbar;
struct THnMdCommonPointers;
class CHnConditionInterface;

/**
 * Item.
 *
 * This is the item's class. Each item (or items' group -
 * the count attribute has a value greater then 1) is represented
 * by an object of the CHnMdItem class.
 *
 * This class comprises different classes which are responsible for
 * conditions, queries, notifications (refreshing), etc.
 *
 * @lib hnmetadatamodel
 * @since S60 v5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdItem) : public CBase,
    public MHnQueryResultCollectorNotification
    {
public:

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers container.
     * @return Fully constructed object.
     */
    static CHnMdItem* NewL( TXmlEngElement aElement,
                                      THnMdCommonPointers* aCmnPtrs );

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers container.
     * @return Fully constructed object.
     */
    static CHnMdItem* NewLC( TXmlEngElement aElement,
                                       THnMdCommonPointers* aCmnPtrs );

    /**
     * Default destructor.
     */
    virtual ~CHnMdItem();

    /**
     * Updates an item.
     *
     * @param aGraphicalList A graphical list.
     * @param aId Item's identifier.
     * @param aResultMap A results map.
     * @return An error code.
     */
    void UpdateItemL( CHnSuiteModel* aGraphicalList, TInt aId,
            RHashMap< HBufC*, CLiwGenericParamList*>& aResultMap );
    /**
     * Compares entries.
     *
     * @param aSearchCriteria Search critieria item id.
     * @return True if this item matches aSearchCriteria.
     */
    TBool CompareItems( CHnItemId* aSearchCriteria );

    /**
     * Evaluates an item.
     *
     * @since S60 v3.2
     * @param aSuiteModel A suite's model.
     * @param aParamList Paramters of the evaluation.
     * @param aItemPosition A position of an item in the aParamList.
     */
    void EvaluateItemL( CHnSuiteModel & aSuiteModel,
                        CLiwGenericParamList& aParamList,
                        TInt aItemPosition );

    /**
     * Marks according to filter which item should be evaluated.
     *
     * @since S60 5.0
     * @param aFilter Filter.
     */
    void MarkEvaluationL( CHnFilter& aFilter );

    /**
     * Sets template.
     *
     * @param aOption Option - an identifier of alternative template bo be set.
     */
    void SetAlternativeTemplateL( TInt aOption );

    /**
     * Sets edit mode version of the item.
     *
     * @param aEditModeItem Edit mode item.
     * @return Meta data model of the edit mode version.
     */
    void SetAlternativeModeItem(CHnMdItem* aEditModeItem);

    /**
     * Sets edit mode version of the item.
     * Transfers ownership.
     *
     * @return Meta data model of the edit mode version.
     */
    CHnMdItem* TakeAlternativeModeItem();

     /**
      * Checks if mode version of the item is present.
      *
      * @return True if an item has an alternative model to be set.
      */
     TBool HasAlternativeModeItem() const;

     /**
      * Template property getter.
      *
      * @return The name of the template.
      */
     IMPORT_C TDesC8& Template();

     /**
      * Type property getter.
      *
      * @return Type of an item.
      */
     IMPORT_C TDesC8& Type();
     
     /**
      * Type property getter.
      *
      * @return Type of an item.
      */
     IMPORT_C TDesC8& Uid();

     // from queries collector
     /**
      * Triggered when results have been collected.
      *
      * @param aResults Results.
      */
     virtual void ResultsCollectedL( CLiwGenericParamList* aResults );

private:

    /**
     * Sets edit mode item.
     *
     * @param aElement Xml element.
     */
    void SetEditModeItemL( TXmlEngElement aElement );

    /**
     * Sets count.
     *
     * @param aElement Xml element.
     */
    void SetCountL( TXmlEngElement aElement );

    /**
     * Sets template.
     *
     * @param aAttr Xml attribute.
     */
    void SetTemplateL( TXmlEngAttr aAttr );

    /**
     * Sets queries.
     *
     * @param aElement Xml element.
     */
    void SetQueriesL( TXmlEngElement aElement );

    /**
     * Sets item id.
     *
     * @param aItemElement Xml element.
     */
    void SetItemIdL( TXmlEngElement aItemElement );

    /**
     * Sets item id.
     *
     * @param aAttrFileName Xml attribute - file name.
     * @param aId Xml attribute - item's identifier.
     */
    void SetItemId2L( TXmlEngAttr aAttrFileName, TXmlEngAttr aId );

    /**
     * Evaluates item.
     *
     * @param aSuiteModel A model of the suite.
     * @param aItemId Item's id.
     * @param aParamList Parameters of the evaluation in the form of the CLiwGenericParamList.
     * @param aItemPosition A position of the record of the parameters list.
     * @return Error code.
     */
    TInt EvaluateItemL( CHnSuiteModel & aSuiteModel,
            const CHnItemId* aItemId, CLiwGenericParamList& aParamList,
            TInt aItemPosition );

    /**
     * Evaluates item.
     *
     * @param aSuiteModel A model of the suite.
     * @param aParamList Parameters of the evaluation in the form of the CLiwGenericParamList.
     * @param aItemPosition A position of the record of the parameters list.
     */
    void EvaluateItemItselfL( CHnSuiteModel & aSuiteModel,
            CLiwGenericParamList& aParamList, TInt aItemPosition );

    /**
     * Sets template attribute on item model.
     *
     * @param aItemModel Item's model.
     */
    void SetTemplateL( CHnItemModel* aItemModel );

    /**
     * Mode.
     *
     * @returns Mode of an item, e.g. normal / edit mode.
     */
    TMdMode Mode();
    
    /**
     * Retrieves values from outputs containing the [$index] string.
     * 
     * @param aValues Array containing found strings.
     * @param aElement The xml element.
     * @return Number of found strings.
     */
    TInt GetIndexValuesFromOutputsL( RPointerArray<HBufC8> &aValues, TXmlEngElement aElement );

private:

    /**
     * Standard constructor.
     */
    CHnMdItem();

    /**
     * EPOC default constructor for performing 2nd stage construction.
     *
    * @param aElement Xml element.
    * @param aCmnPtrs Common pointers containter.
     */
    void ConstructL( TXmlEngElement aElement,THnMdCommonPointers* aCmnPtrs );

    /**
     * Fills item model with data.
     *
     * @param aItemModel Model to be filled.
     * @param aQueriesResultsList Parameters of in the form of the CLiwGenericParamList.
     * @param aPosition A position of the record in the parameters list.
     * @return True if filling finihed with success.
     */
    TBool FillItemModelL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueriesResultsList,
            TInt aPosition );

    /**
     * Fills item model with menu items created from meta data model.
     *
     * @param aItemModel Model to be filled.
     * @param aQueriesResultsList Parameters of in the form of the CLiwGenericParamList.
     * @param aPos A position of the record in the parameters list.
     */
    void FillItemModelWithMenuItemsL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos = 0 );

    /**
     * Fills item model with toolbar created from meta data model.
     *
     * @param aItemModel Model to be filled.
     * @param aQueriesResultsList Parameters of in the form of the CLiwGenericParamList.
     * @param aPos A position of the record in the parameters list.
     */
    void FillItemModelWithToolbarL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos = 0 );

    /**
     * Fills item model with MSK created from meta data model.
     *
     * @param aItemModel Model to be filled.
     * @param aQueriesResultsList Parameters of in the form of the CLiwGenericParamList.
     * @param aPos A position of the record in the parameters list.
     */
    void FillItemModelWithMiddleSoftKeyL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos = 0 );

    /**
     * Fills menu model with data from mdmenuitem.
     *
     * @param aSuiteModel Suite's model.
     * @param aItemId Item's identifier.
     * @param aQueriesResultsList Queries results in form of the CLiwGenericParamList.
     * @param aItemModelNumber Item's model number.
     * @param aItemPos Position of an item in the queries results list.
     * @return KErrNone if an item was successfully added.
     */
    TInt AddItemL( CHnSuiteModel & aSuiteModel, TInt aItemId,
            const CLiwGenericParamList& aQueriesResultsList,
            TInt aItemModelNumber,
            TInt aItemPos );

    /**
     * Fills menu model with data from mdmenuitem.
     *
     * @param aSuiteModel Suite's model.
     * @param aItemId Item's identifier.
     * @param aQueriesResultsList Queries results in form of the CLiwGenericParamList.
     * @param aItemPos Position of an item in the queries results list.
     * @return KErrNone if an item was successfully updated.
     */
    TInt UpdateItemL( CHnSuiteModel & aSuiteModel, TInt aItemId,
            const CLiwGenericParamList& aQueriesResultsList, TInt aItemPos );

    /**
     * Fills menu model with data from mdmenuitem.
     *
     * @param aSuiteModel Suite's model.
     * @param aId Items identifier.
     */
    void RemoveItemL( CHnSuiteModel & aSuiteModel, TInt aId );

    /**
     * Sets move/remove locked attribute.
     *
     * @param aElement Xml item element.
     */
    void SetAttributesL( TXmlEngElement aElement );

    /**
     * Sets item type string.
     *
     * @param aElement Xml item element.
     */
    void SetTypeL( TXmlEngElement aElement );
    
    /**
     * Sets uid string.
     *
     * @param aElement Xml item element.
     */
    void SetUidL( TXmlEngElement aElement );

    /**
     * Sets delete / move locked attributes.
     *
     * @param aItemModel Model to be filled.
     * @param aQueriesResultsList Parameters of in the form of the CLiwGenericParamList.
     * @param aPos A position of the record in the parameters list.
     */
    void SetDeleteMoveAttributesL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos );


    /**
     * Sets custom id.
     *
     * @param aElement Xml item element.
     */
    void SetCustomIdL( TXmlEngElement aElement );

    /**
     * Sets middle soft key.
     *
     * @param aElement Xml item element.
     * @param aCmnPtrs Common pointers.
     */
    void SetMiddleSoftKeyL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Checks if an item is valid (the condition returns true).
     *
     * @param aQueryResults Query results.
     * @param aPos Position of the item in the results list.
     * @return ETrue or EFalse dependeing on whether the item is valid.
     */
    TBool IsValidL( const CLiwGenericParamList& aQueryResults, TInt aPos );

    /**
     * Sets the evaluated remove lock to the item model.
     *
     * @param aQueryResults Query results.
     * @param aPos Position of the item in the results list.
     * @param aItemModel Item model to set custom id on.
     */
    void SetRemoveLockedL( const CLiwGenericParamList& aQueryResults,
            TInt aPos, CHnItemModel* aItemModel );

    /**
     * Sets the evaluated move lock to the item model.
     *
     * @param aQueryResults Query results.
     * @param aPos Position of the item in the results list.
     * @param aItemModel Item model to set custom id on.
     */
    void SetMoveLockedL( const CLiwGenericParamList& aQueryResults,
            TInt aPos, CHnItemModel* aItemModel );

    /**
     * Sets the evaluated type to the item model.
     *
     * @param aQueryResults Query results.
     * @param aPos Position of the item in the results list.
     * @param aItemModel Item model to set custom id on.
     */
    void SetTypeL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueryResults, TInt aPos );
    
    /**
     * Sets the uid to the item model.
     *
     * @param aQueryResults Query results.
     * @param aPos Position of the item in the results list.
     * @param aItemModel Item model to set custom id on.
     */
    void SetUidL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueryResults, TInt aPos );

    /**
     * Sets custom id.
     *
     * @param aItemModel Model to be filled.
     * @param aParams Parameters of in the form of the CLiwGenericParamList.
     * @param aPos A position of the record in the parameters list.
     */
    void SetCustomIdL( CHnItemModel* aItemModel,
          const CLiwGenericParamList& aParams, TInt aPos );

    /**
     * Returns the maximum possible number of items.
     *
     * @return Maximum number of items.
     */
    TInt GetItemsMaxCountL();

private: // data

    /**
     * Own - edit mode version of the item.
     * However TakeAlternativeModeItem transfers ownership.
     */
    CHnMdItem *iAlternativeModeItem;

    /**
     * Count.
     */
    RBuf8 iCount8;

    /**
     * Template.
     */
    RBuf8 iTemplate;

    /**
     * Own - List of allowable widget types.
     */
    CDesC8Array * iAllowableTemplate;

    /**
     * Item id.
     */
    CHnItemId* iItemId;

    /**
     * Queries.
     */
    CHnMdQueries* iQueries;

    /**
     * Notify Requests.
     */
    CHnMdNotifyRequests* iNotifyRequests;

    /**
     * Event mapping.
     */
    CHnMdEventMapping* iEventMapping;

    /**
     * UI mapping.
     */
    CHnMdUiMapping* iUiMapping;

    /**
     * Menu item.
     */
    CHnMdMenuItem* iMenu;

    /**
     * Toolbar.
     */
    CHnMdToolbar* iToolbar;

    /**
     * Middle soft key button.
     */
    CHnMdButton* iMsk;

    /**
     * Delete locked indicator
     */
    RBuf8 iRemoveLocked8;

    /**
     * MoveLocked indicator.
     */
    RBuf8 iMoveLocked8;

    /**
     * Type of an item.
     */
    RBuf8 iType8;
    
    /**
     * Uid of the item.
     */
    RBuf8 iUid8;

    /**
     * Custom identifier.
     */
    RBuf8 iCustomId8;

    /**
     * Common pointers.
     */
    THnMdCommonPointers* iCmnPtrs;

    /**
     * Mode.
     */
    TMdMode iMode;

    /**
     * Query results list.
     *  Own - queries results.
     */
    CLiwGenericParamList* iQueriesResultsList;

    /**
     * Query collector.
     * Own.
     */
    CHnQueryResultCollector* iQc;

    /**
     * Suite model.
     * Not own - suite model.
     */
    CHnSuiteModel* iSuiteModel;

    /**
     * Position of item.
     * Own - item's positions.
     */
    TInt iItemPosition;

    /**
     * Own - condition.
     */
    CHnConditionInterface* iCondition;

    /**
     * Running indicator.
     */
    RBuf8 iRunning8;
    
    /**
     * DRM protection indicator.
     */
    RBuf8 iDrmProtection8;

public:
    /**
     * Marks if evaulation should take place.
     */
    TBool iEvaluationNeeded;

    };

#endif // C_HNMMITEM_H
