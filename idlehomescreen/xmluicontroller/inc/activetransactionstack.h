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
* Description:  Active transaction stack
*
*/


#ifndef C_ACTIVETRANSACTIONSTACK_H
#define C_ACTIVETRANSACTIONSTACK_H

#include <e32base.h>


namespace AiXmlUiController
    {
    class MTransaction;
    class CTransaction;
    }


namespace AiXmlUiController
{

/**
*  @ingroup group_xmluicontroller
* 
*  CActiveTransactionStack is a stack for CTransactions
*
*  @lib AiXmlUiMain
*/
class CActiveTransactionStack : public CBase
    {
public:  // Constructors and destructor
    
    static CActiveTransactionStack* NewL();
    
    virtual ~CActiveTransactionStack();

    // New functions
    
    /**
    * Checks if stack is empty.
    * @return ETrue if stack is empty
    */
    TBool IsEmpty() const;
    
    /**
    * Pushes a transaction into the stack.
    * @param aTransaction the transaction
    */
    void Push( MTransaction* aTransaction );
    
    /**
    * Pops the topmost transaction from the stack.
    * @return the topmost transaction from the stack
    */
    MTransaction* Pop();
    
    /**
    * Returns the topmost transaction from the stack.
    * @return the topmost transaction from the stack
    */
    MTransaction* Top() const;
    
    /**
    * Lookups a transaction from the stack.
    * @param aTxId the transaction id of the transaction
    * @return the transaction with aTxId from the stack
    */
    MTransaction* Lookup( TInt aTxId ) const;
    
    /**
    * Removes a transaction from the stack.
    * @param aTxId the transaction id of the transaction
    * @return the transaction with aTxId from the stack
    */
    MTransaction* Remove( TInt aTxId );
    
private:

    CActiveTransactionStack();
    
    CTransaction* DoTop() const;
    
    CTransaction* DoLookup( TInt aTxId ) const;

private:    // Data
    
    /**
     * Doubly linked list of transactions
     */ 
    TDblQue< CTransaction > iTransactions;
    };
    
} // namespace AiXmlUiController

#endif      // C_ACTIVETRANSACTIONSTACK_H
           
//  End of File
