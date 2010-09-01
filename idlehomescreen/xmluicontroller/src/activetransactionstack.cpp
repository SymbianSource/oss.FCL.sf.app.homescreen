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


#include    "activetransactionstack.h"
#include    "transaction.h"
#include    "xmluicontrollerpanic.h"



using namespace AiXmlUiController;

// ========================== MEMBER FUNCTIONS ===============================

CActiveTransactionStack::CActiveTransactionStack()
    : iTransactions( CTransaction::LinkOffset() )
    {
    }

CActiveTransactionStack* CActiveTransactionStack::NewL()
    {
    CActiveTransactionStack* self = new( ELeave ) CActiveTransactionStack;
    
    return self;
    }
    
CActiveTransactionStack::~CActiveTransactionStack()
    {
    __ASSERT_DEBUG( IsEmpty(), Panic( ETransactionStackNotEmpty ) );
    }

TBool CActiveTransactionStack::IsEmpty() const
    {
    return iTransactions.IsEmpty();
    }
            
void CActiveTransactionStack::Push( MTransaction* aTransaction )
    {
    CTransaction* tr = dynamic_cast< CTransaction* >( aTransaction );
    
    if ( tr )
        {
        iTransactions.AddFirst( *tr );
        }
    }

MTransaction* CActiveTransactionStack::Pop()
    {
    CTransaction* tr = DoTop();
    if ( tr )
        {
        tr->Deque();
        }
        
    return tr;
    }
    
MTransaction* CActiveTransactionStack::Top() const
    {
    return DoTop();
    }
            
MTransaction* CActiveTransactionStack::Lookup( TInt aTxId ) const
    {
    return DoLookup( aTxId );
    }
            
MTransaction* CActiveTransactionStack::Remove( TInt aTxId )
    {
    CTransaction* tr = DoLookup( aTxId );
    
    if ( tr )
        {
        tr->Deque();
        }
    
    return tr;
    }

CTransaction* CActiveTransactionStack::DoTop() const
    {
    __ASSERT_ALWAYS( !iTransactions.IsEmpty(), Panic( ETransactionStackEmpty ) );

    CTransaction* tr = iTransactions.First();
    return tr;
    }

CTransaction* CActiveTransactionStack::DoLookup( TInt aTxId ) const
    {
    // Iterate over transactions and search transaction with aTxId
    TDblQueIter< CTransaction >
        iter( const_cast< TDblQue< CTransaction >& >( iTransactions ) );
    iter.SetToFirst();
    
    while ( iter )
        {
        CTransaction* tr = iter++;
        
        if ( tr->Id() == aTxId )
            {
            // Match. Return transaction 
            return tr;
            }        
        }
        
    return NULL;
    }
    
//  End of File
