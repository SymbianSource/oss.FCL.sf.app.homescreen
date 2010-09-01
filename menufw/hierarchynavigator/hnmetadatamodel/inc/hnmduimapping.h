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



#ifndef C_HNMDUIMAPPING_H
#define C_HNMDUIMAPPING_H

#include <e32base.h>
#include <xmlengdom.h>
#include <e32hashtab.h>

#include "hnglobals.h"

class CLiwDefaultMap;
struct THnMdCommonPointers;
class CHnMdUiMappingElement;
class CLiwGenericParamList;
class CHnItemModel;

/**
 * UI mapping.
 * 
 * This class contains the list of CHnMdUiMappingElement objects.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdUiMapping) : public CBase
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdUiMapping* NewL( TXmlEngElement aElement,
                                 THnMdCommonPointers* aCmnPtrs );

    /**
     * Two-phase constructor.
     *
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdUiMapping* NewLC( TXmlEngElement aElement,
                                  THnMdCommonPointers* aCmnPtrs );

    /**
     * Standarc C++ virtual constructor.
     */
    virtual ~CHnMdUiMapping();
    
    /**
     * Fills graphical item.
     *
     * @param aItemModel Item model.
     * @param aQueriesResultsList Hash map with query results
     * @param aPos Current item number
     * @return True if an item was correctly filled. 
     */     
    TBool FillGraphicalItemL( CHnItemModel* aItemModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos );
    
    /**
     * Adds UI mapping element.
     *
     * @param aUiMappingElement UI mapping element.
     */
    void AddUiMappingElementL(
        CHnMdUiMappingElement *aUiMappingElement );
    
private:

    /**
     * Standard C++ constructor.
     */
    CHnMdUiMapping();

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void ConstructL( TXmlEngElement aElement,
                     THnMdCommonPointers* aCmnPtrs );
    
private: // data

    /**
     * Mappings.
     */
    RPointerArray<CHnMdUiMappingElement> iMappings;

    };

#endif // C_HNMMUIMAPPING_H
