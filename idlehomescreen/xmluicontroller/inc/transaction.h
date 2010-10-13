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
* Description:  Transaction
*
*/


#ifndef C_TRANSACTION_H
#define C_TRANSACTION_H

#include <e32base.h>
#include <e32cmn.h>
#include <e32hashtab.h>
#include "xnproperty.h"
#include "xnnodeappif.h"
#include "mtransaction.h"


namespace AiXmlUiController
    {
    class MTransactionElement;
    class CTransactionElement;
    class MTransactionFactory;
    }


namespace AiXmlUiController
{

/**
*  @ingroup group_xmluicontroller
* 
*  CTransaction collects the transaction elements of one transaction
*
*  @lib AiXmlUiMain
*/
class CTransaction : public CBase, public MTransaction
    {
public:  // Constructors and destructor
    
    static CTransaction* NewL();
    
    virtual ~CTransaction();

// From MTransaction
    
    void Append(MTransactionElement& aElement);
    
    void CommitL(TBool& aLayoutChanged, RPropertyHashMap& aPropertyHashMap);
    
    void Reset(MTransactionFactory& aFactory);
    
    TDblQueIter<CTransactionElement> ElementIter();

    
// New functions
    
    /**
     * Returns the transaction id of this transaction
     */
    TInt Id() const;
    
    /**
     * Initializes transaction.
     * @param aTxId transaction id.
     */
    void Initialize(TInt aTxId);
    
    /**
     * Returns the doubly linked list link offset.
     */
    inline static TInt LinkOffset();
    
    /**
     * Returns the singly linked list link offset
     */
    inline static TInt FactoryLinkOffset();
    
    /**
     * Deques this transaction from a doubly linked list
     */
    void Deque();
    
private:

    CTransaction();

private:    // Data
    
    /**
     * Transaction identifier.
     */
    TInt iTxId;
    
    /**
     * Doubly linked list of transaction elements.
     */
    TDblQue<CTransactionElement> iTransactionElements;
   
    /**
     * Doubly linked list link item.
     */
    TDblQueLink iLink;
    
    /**
     * Singly linked list link item.
     */
    TSglQueLink iFactoryLink;
        
    };
        
// Inline implementations

TInt CTransaction::LinkOffset()
    {
    return _FOFF(CTransaction, iLink);
    }

TInt CTransaction::FactoryLinkOffset()
    {
    return _FOFF(CTransaction, iFactoryLink);
    }

} // namespace AiXmlUiController

#endif      // C_TRANSACTION_H 
           
//  End of File
