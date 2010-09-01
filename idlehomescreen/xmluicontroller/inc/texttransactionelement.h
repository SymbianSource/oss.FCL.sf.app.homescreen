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
* Description:  Text transaction element
*
*/


#ifndef C_TEXTTRANSACTIONELEMENT_H
#define C_TEXTTRANSACTIONELEMENT_H

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
 * CTextTransactionElement corresponds one text content publishing operation.
 *
 *  @lib AiXmlUiMain
 */
class CTextTransactionElement : public CTransactionElement
    {
public:  // Constructors and destructor
    
    static CTextTransactionElement*
        NewL( AiUtility::CContentPriorityMap& aContentPriorityMap );
    
    virtual ~CTextTransactionElement();

// New functions
    
    /**
    * Initializes this transaction element to publish text content aText
    * to text interface of node aTarget.
    *
    * @param aTarget - the target of publishing.
    * @param aText - the text content. 
    */
    void InitializeL(CXnNodeAppIf& aTarget, const TDesC& aText);
    
    static TBool IsSupported( CXnNodeAppIf& aTarget );
    
// Functions from CTransactionElement

    void Reset();

    void UpdateDataL();
    
private: // New methods

    CTextTransactionElement( AiUtility::CContentPriorityMap& aContentPriorityMap );

    void CheckTypeL( CXnNodeAppIf& aTarget );
    
    void SetTextL();
    
private:    // Data

    /**
     * New text content.
     */
    
    HBufC* iNewText;
        
    };

}   // namespace AiXmlUiController

#endif      // C_TEXTTRANSACTIONELEMENT_H  
           
//  End of File
