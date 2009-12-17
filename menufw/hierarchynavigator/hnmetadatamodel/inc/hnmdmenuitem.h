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



#ifndef C_HNMDMENUITEM_H
#define C_HNMDMENUITEM_H

#include <e32hashtab.h>
#include <e32base.h>
#include <xmlengelement.h> 

struct THnMdCommonPointers;
class CHnCondition;
class CLiwGenericParamList;
class CHnMenuItemModel;
class CHnConditionInterface;

/**
 * Menu Item.
 * 
 * This class represents items in the Options menu.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdMenuItem) : public CBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdMenuItem* NewL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdMenuItem* NewLC( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    ~CHnMdMenuItem( );
    
    /**
     * Gets position.
     *
     * @since S60 5.0
     * @param aMenuModel Menu model.
     * @param aQueriesResultsList Queries results list.
     * @param aPos Position of the record in the results list.
     */
    void EvaluateL( CHnMenuItemModel* aMenuModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos );

public:    
    // from MHnMdMenuItemBase
         
    /**
     * Gets the menu item's name
     *
     * @return Name of menu item.
     */
    const TDesC& NameL();
        
    /**
     * Checks if the menu item is valid.
     *
     * @since S60 5.0
     * @param aQueryResults Query results.
     * @param aPos Position.
     * @return True if a condition is satisfied.
     */
    TBool IsValidL( const CLiwGenericParamList& aQueryResults, 
                    TInt aPos = 0 );

private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdMenuItem();

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void ConstructL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );
    
    /**
     * Creates children.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void CreateChildrenL( TXmlEngElement aElement, 
                             THnMdCommonPointers* aCmnPtrs );

    /**
     * Appends child item.
     *
     * @since S60 5.0
     * @param aMenuItem Menu item.
     */
    void AppendChildItemL( CHnMdMenuItem* aMenuItem );
    
    /**
     * Creates properties.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void CreatePropertiesL( TXmlEngElement aElement, 
                            THnMdCommonPointers* aCmnPtrs );

private: // data

    /**
     * Own - Name.
     */
    RBuf iName;

    /**
     * Position.
     */
    TInt iPosition;

    /**
     * Event element.
     */
    TInt iEvent;    
    
    /**
     * Own - Children pointer array.
     */
    RPointerArray<CHnMdMenuItem> iChildren;

    /**
     * Own - Name value cache. 
     */
    RBuf iNameValueCache;
    
    /**
     * Own - Condition.
     */
    CHnConditionInterface* iCondition;
    
    /**
     * Is menu item "item specific".
     */
    TBool iSpecific;

    };

#endif // C_HNMULMENUITEM_H
