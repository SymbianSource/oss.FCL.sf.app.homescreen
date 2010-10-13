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
* Description:  Transaction element base class
*
*/


#ifndef M_TRANSACTIONELEMENT_H
#define M_TRANSACTIONELEMENT_H

#include <e32def.h>
#include "aipolicyelement.h"
#include "transaction.h"


namespace AiXmlUiController
{

/**
 * MTransactionElement declatres the public interface for transaction elements.
 * MTransactionElement corresponds one publish operation within a
 * transaction scope.
 *
 * @lib AiXmlUiMain
 */
class MTransactionElement
    {
public: // New functions
    
    /**
     * Commits the publish operation and actually updates the UI control/element.
     *
     * @param [out] aLayoutChanged true if UI layout changes 
     * @see UpdateDataL()
     * @see ApplyPublishingPolicy()
     */
    void CommitL( TBool& aLayoutChanged, RPropertyHashMap& aPropertyHashMap );

    /**
     * Resets the transaction element and clears the references to content
     * and UI control/element.
     *
     */
    virtual void Reset() = 0;
    
    /**
     * Returns the array of associated content policy elements.
     *
     */
    virtual RAiPolicyElementArray& PolicyArray() = 0;
    
    /**
     * Return the target UI element for this transaction element
     */
    virtual CXnNodeAppIf& Target() const = 0 ;
    
protected:  // New functions

    /**
     * Updates UI control / element data.
     *
     * @since S60 3.2
     */
    virtual void UpdateDataL() = 0;
    
    /**
     * Applies current publishing policy to UI control / element.
     *
     * @param [out] aLayoutChanged true if UI layout changes
     */
    virtual void ApplyPublishingPolicy( TBool& aLayoutChanged,
                                        RPropertyHashMap& aPropertyHashMap ) = 0;
    
protected:  // Constructors and destructor
    
    ~MTransactionElement() {};
    
    };

}   // namespace AiXmlUiController

#endif      // M_TRANSACTIONELEMENT_H  
           
//  End of File