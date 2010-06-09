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
* Description:  Image transaction element
*
*/


#ifndef C_IMAGETRANSACTIONELEMENT_H
#define C_IMAGETRANSACTIONELEMENT_H

#include "transactionelement.h"

class CGulIcon;
class CXnNodeAppIf;

namespace AiUtility
    {
    class CContentPriorityMap;
    }

namespace AiXmlUiController
{    
    
/**
 *  @ingroup group_xmluicontroller
 * 
 *  CImageTransactionElement is a transaction element for publishing images.
 *
 *  @lib AiXmlUiMain
 */
class CImageTransactionElement : public CTransactionElement
    {

public:

    static CImageTransactionElement*
        NewL( AiUtility::CContentPriorityMap& aContentPriorityMap );

    virtual ~CImageTransactionElement();

    /**
     * Initializes this transaction element.
     *
     * @param aTarget the node to publish bitmap and mask.
     * @param aIcon the icon to publish. Ownership is transferred.
     */
    void InitializeL(CXnNodeAppIf& aTarget, CGulIcon* aIcon );
    
    /**
     * Initializes this transaction element from file.
     *
     * @param aTarget the node to publish bitmap.
     * @param aFile the open file handle to read bitmap data.
     */
    void InitializeL(CXnNodeAppIf& aTarget,
                     RFile& aFile);
    
    static TBool IsSupported( CXnNodeAppIf& aTarget );
    
    void Reset();

    void UpdateDataL();

            
private:

    CImageTransactionElement( AiUtility::CContentPriorityMap& aContentPriorityMap );
    
    void CheckTypeL( CXnNodeAppIf& aTarget );
    
private: // data


    /**
     * Icon to be published.
     * Own.
     */
    CGulIcon* iIcon;    
    
    /**
     * Icon filename.     
     */
    TFileName iFilename;
    };
    
} // namespace AiXmlUiController

#endif // C_IMAGETRANSACTIONELEMENT_H

// End of file

