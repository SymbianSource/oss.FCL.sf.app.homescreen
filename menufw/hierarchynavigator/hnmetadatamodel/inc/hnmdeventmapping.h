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



#ifndef C_HNMDEVENTMAPPING_H
#define C_HNMDEVENTMAPPING_H

#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengdom.h>

#include "hnmdmodel.h"

class CHnMdEventMappingElement;
class CHnMdItem;
class CHnMdAction;
struct THnMdCommonPointers;
class CHnActionModel;
class CHnItemModel;
class CHnSuiteModel;

/**
 * Event Mapping.
 * 
 * This class is to store event mapping elements defined
 * in one suite.
 *
 * @lib hnmetadatamodel
 * @since S60 v5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdEventMapping) : public CBase
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
    static CHnMdEventMapping* NewL( TXmlEngElement aElement, 
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers container.
     * @return Fully constructed object.
     */
    static CHnMdEventMapping* NewLC( TXmlEngElement aElement, 
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Fills actions models container with evaluated actions.
     *
     * since S60 v5.0
     * @param aActions Actions container to be filled with actions models.
     * @param aQueriesResultsList Query results.
     * @param  aPos  Position in the query.
     */
    void FillActionsL( RHashMap< TInt, CArrayPtr<CHnActionModel>* >& aActions, 
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos = 0 );
    
    /**
     * Standard destructor.
     */
    virtual ~CHnMdEventMapping();
    
    /**
     * Gets list of actions correspondig to given event id.
     * 
     * @param aEventId Event's identifier.
     * @return Actions list corresponding to the event.
     */
    RPointerArray< CHnMdAction > GetActionsForEventId( TInt aEventId );
    
    /**
     * Gets events ids.
     * 
     * @param aIds Array to be filled with events ids.
     */
    void GetEventIdsL( RArray< TInt >& aIds ) const;
    
private:

    /**
     * Gets action by event id.
     * 
     * @return A number of actions.
     */
    TInt Count() const;
    
    /**
     * Gets action by event id.
     * 
     * @param aActions Actions container to which action model will be added.
     * @param aActionModel An  evaluated action model.
     * @param aEventId An event id.  
     */
    void AddActionL( 
            RHashMap<TInt,CArrayPtr<CHnActionModel> *> & aActions, 
            CHnActionModel *& aActionModel, TInt aEventId );
    
    /**
     * default constructot
     */
    CHnMdEventMapping();
    
    /**
     * EPOC default constructor for performing 2nd stage construction.
     * 
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers container.
     */
    void ConstructL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );

private: // data

    /**
     * Mappings.
     */
    RPointerArray<CHnMdEventMappingElement> iMappings;
    
    /**
     * Common pointers.
     * Not own.
     */
    THnMdCommonPointers* iCmnPtrs;

    };

#endif // C_HNMMEVENTMAPPING_H
