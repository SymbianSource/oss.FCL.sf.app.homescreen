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
* Description:  News ticker transaction element
*
*/


#ifndef C_NEWSTICKERTRANSACTIONELEMENT_H
#define C_NEWSTICKERTRANSACTIONELEMENT_H

#include "transactionelement.h"


namespace AiUtility
    {
    class CContentPriorityMap;
    }

class CXnNodeAppIf;

namespace AiXmlUiController
{

/**
 *  @ingroup group_xmluicontroller
 * 
 * CNewTickerTransactionElement corresponds news ticker content publishing operation.
 *
 *  @lib AiXmlUiMain
 */
class CNewsTickerTransactionElement : public CTransactionElement
    {
public:  // Constructors and destructor
    
    static CNewsTickerTransactionElement*
        NewL( AiUtility::CContentPriorityMap& aContentPriorityMap );
    
    virtual ~CNewsTickerTransactionElement();

// New functions
    
    /**
    * Initializes this transaction element to publish text content aText
    * to text interface of node aTarget.
    *
    * @param aTarget - the target of publishing.
    * @param aText - the text content. 
    * @param aIndex - index of the news ticker element to be updated
    */
    void InitializeL(CXnNodeAppIf& aTarget, const TDesC& aText, TInt aIndex);
 
    /**
     * @return ETrue if aTarget node is supported 
     */
    static TBool IsSupported( CXnNodeAppIf& aTarget );
    
    /**
     * from CTransactionElement
     */
    void Reset();

    /**
     * from CTransactionElement
     */
    void UpdateDataL();
    
private: // New methods

    /**
     * constructor 
     */
    CNewsTickerTransactionElement( AiUtility::CContentPriorityMap& aContentPriorityMap );

    /**
     * 
     */
    void CheckTypeL( CXnNodeAppIf& aTarget );
    /**
     * 
     */
    void SetTextL();
    
private:    // Data

    /**
     * New text content.
     */
    TPtrC iNewText;
    
    /**
     * Index of the news ticker element to be updated
     */
    TInt iIndex;
        
    };

}   // namespace AiXmlUiController

#endif      // C_NEWSTICKERTRANSACTIONELEMENT_H  
           
//  End of File
