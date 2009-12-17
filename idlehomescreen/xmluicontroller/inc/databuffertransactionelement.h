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
* Description:  Data buffer transaction
*
*/


#ifndef C_DATABUFFERTRANSACTIONELEMENT_H
#define C_DATABUFFERTRANSACTIONELEMENT_H

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
 * CDataBufferTransactionElement corresponds one text content publishing operation.
 *
 *  @lib AiXmlUiMain
 */
class CDataBufferTransactionElement : public CTransactionElement
    {
public:  // Constructors and destructor
    
    static CDataBufferTransactionElement*
        NewL( AiUtility::CContentPriorityMap& aContentPriorityMap );
    
    virtual ~CDataBufferTransactionElement();

// New functions
    
    /**
    * Initializes this transaction element to publish content aData
    * to node aTarget.
    *
    * @param aTarget - the target of publishing.
    * @param aData - the content. 
    */
    void InitializeL(CXnNodeAppIf& aTarget, const TDesC8& aData);
    
    /**
     * Checks if given content type is supported by target ui element.
     *
     * @param aTarget target ui element.
     * @param aContentType content type
     * @return ETrue if content type is supported
     */
    static TBool IsSupported( CXnNodeAppIf& aTarget,
                              const TDesC8& aContentType );
    
// Functions from CTransactionElement

    void Reset();

    void UpdateDataL();
    
private:

    CDataBufferTransactionElement(
        AiUtility::CContentPriorityMap& aContentPriorityMap );

    void CheckTypeL( CXnNodeAppIf& aTarget );
    
    void SetDataL();
    
private:    // Data

    /**
     * New content.
     */
    TPtrC8 iNewData;
        
    };

}   // namespace AiXmlUiController

#endif      // C_TEXTTRANSACTIONELEMENT_H  
           
//  End of File
