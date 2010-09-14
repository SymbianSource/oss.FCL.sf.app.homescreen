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
* Description:  Transaction base class
*
*/


#include    "transaction.h"
#include    "transactionelement.h"
#include    "transactionfactory.h"
#include    "xmluicontrollerpanic.h"

// ============================ MEMBER FUNCTIONS ===============================

using namespace AiXmlUiController;

CTransaction::CTransaction()
    : iTransactionElements(CTransactionElement::LinkOffset())
    {
    }

CTransaction* CTransaction::NewL()
    {
    CTransaction* self = new( ELeave ) CTransaction;
    
    return self;
    }
    
CTransaction::~CTransaction()
    {
    __ASSERT_DEBUG( iTransactionElements.IsEmpty(), Panic( ETransactionElementListNotEmpty ) );
    }

TInt CTransaction::Id() const
    {
    return iTxId;
    }
    
void CTransaction::Initialize(TInt aTxId)
    {
    iTxId = aTxId;
    }
    
void CTransaction::Append(MTransactionElement& aElement )
    {
    iTransactionElements.AddLast(dynamic_cast<CTransactionElement&>(aElement));
    }
    
void CTransaction::CommitL( RAiPolicyElementArray& aPolicyArray, RPropertyHashMap& aPropertyHashMap )
    {
    if ( iTransactionElements.IsEmpty() )
        {
        // Nothing to do
        return;
        }
    
    // Initialize iterator
    TDblQueIter<CTransactionElement> iter( iTransactionElements );
    iter.SetToFirst();

    // Commit transaction elements    
    while ( iter )
        {
        // Commit element and advance iterator 
        (iter++)->CommitL( aPolicyArray, aPropertyHashMap );
        }
    }

void CTransaction::Reset(MTransactionFactory& aFactory)
    {
    Deque();
    iTxId = 0;
    
    TDblQueIter<CTransactionElement> iter( iTransactionElements );
    iter.SetToFirst();
    
    while ( iter )
        {
        CTransactionElement* element = iter++;
        aFactory.ReleaseTransactionElement( element );
        }
        
    __ASSERT_DEBUG( iTransactionElements.IsEmpty(), Panic( ETransactionElementListNotEmpty ) );
    }

TDblQueIter<CTransactionElement> CTransaction::ElementIter()
    {
    TDblQueIter<CTransactionElement> iter( iTransactionElements );
    iter.SetToFirst();
    return iter;
    }

void CTransaction::Deque()
    {
    iLink.Deque();
    }

//  End of File
