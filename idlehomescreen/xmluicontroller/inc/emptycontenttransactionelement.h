/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Transaction element for publishing empty content
*
*/


#ifndef C_EMPTYCONTENTTRANSACTIONELEMENT_H
#define C_EMPTYCONTENTTRANSACTIONELEMENT_H

#include "transactionelement.h"

namespace AiUtility
    {
    class CContentPriorityMap;
    }

namespace AiXmlUiController
{
    
/**
 *  @ingroup group_xmluicontroller
 * 
 *  Transaction element for publishing empty content to UI element.
 *  Empty content is published by applying empty content policy
 *
 *  @lib AiXmlUiMain
 */
class CEmptyContentTransactionElement : public CTransactionElement
    {

public:

    static CEmptyContentTransactionElement*
        NewL( AiUtility::CContentPriorityMap& aContentPriorityMap );

    virtual ~CEmptyContentTransactionElement();

    /**
     * Initializes this element to publish into the given UI element.
     *
     * @param aTarget target UI element.
     * @param aIndex Newsticker title index to clean
     */
    void InitializeL( CXnNodeAppIf& aTarget,
                      TInt aIndex );

protected:  // from base class CTransactionElement

    void Reset();
    
    void UpdateDataL();

private:

    CEmptyContentTransactionElement(
        AiUtility::CContentPriorityMap& aContentPriorityMap );

    // Index for newsticker title to clean.
    TInt iIndex;

    };
    
}   // namespace AiXmlUiController

#endif // C_EMPTYCONTENTTRANSACTIONELEMENT_H
