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
* Description:  Transaction factory implementation
*
*/


#include    "transactionfactoryimpl.h"

#include    "transaction.h"
#include    "texttransactionelement.h"
#include    "databuffertransactionelement.h"
#include    "imagetransactionelement.h"
#include    "emptycontenttransactionelement.h"
#include    "newstickertransactionelement.h"
#include    "aixmluiconstants.h"

using namespace AiXmlUiController;

// ============================ MEMBER FUNCTIONS ===============================

CTransactionFactoryImpl::CTransactionFactoryImpl(
                            AiUtility::CContentPriorityMap& aContentPriorityMap,
                            CCssPropertyMap& aPropertyMap )
    : iAvailableTransactions( CTransaction::FactoryLinkOffset() ),
      iReservedTransactions( CTransaction::FactoryLinkOffset() ),
      iAvailableElements( CTransactionElement::FactoryLinkOffset() ),
      iReservedElements( CTransactionElement::FactoryLinkOffset() ),
      iContentPriorityMap( aContentPriorityMap ),
      iPropertyMap( aPropertyMap )
    {
    }

CTransactionFactoryImpl* CTransactionFactoryImpl::NewL(
                            AiUtility::CContentPriorityMap& aContentPriorityMap,
                            CCssPropertyMap& aPropertyMap )
    {
    CTransactionFactoryImpl* self =
                new( ELeave ) CTransactionFactoryImpl( aContentPriorityMap,
                                                       aPropertyMap );
    
    return self;
    }
    
CTransactionFactoryImpl::~CTransactionFactoryImpl()
    {
    // Delete transaction objects
    DeleteTransactionList( iAvailableTransactions );
    DeleteTransactionList( iReservedTransactions );
    
    // Delete transaction elements
    DeleteElementList( iAvailableElements );
    DeleteElementList( iReservedElements );
    }

MTransaction* CTransactionFactoryImpl::CreateTransactionL( TInt aTxId )
    {
    CTransaction* tr = NULL;
    
    if ( iAvailableTransactions.IsEmpty() ) // No free transactions
        {
        tr = CTransaction::NewL();
        }
    else
        {
        // Take first available transaction and remove from queue
        tr = iAvailableTransactions.First();
        iAvailableTransactions.Remove( *tr );
        }
    
    // Initialize with id
    tr->Initialize(aTxId);
    
    // Reserve
    iReservedTransactions.AddFirst( *tr );

    return tr;
    }
            
void CTransactionFactoryImpl::ReleaseTransaction( MTransaction* aTransaction )
    {
    CTransaction* tr = dynamic_cast< CTransaction* >( aTransaction );
    if ( tr )
        {
        // Reset transaction for reuse
        tr->Reset( *this );
        
        // Deque from reserved list
        iReservedTransactions.Remove( *tr );
        
        // Enque to available list
        iAvailableTransactions.AddFirst( *tr );
        }
    }

MTransactionElement* CTransactionFactoryImpl::CreateTextTransactionElementL(
    CXnNodeAppIf& aTarget,
    const TDesC& aText,
    TInt aPriority )
    {
    // Instantiate new element
    CTextTransactionElement* element = NewInstanceL< CTextTransactionElement >();
    
    CleanupStack::PushL( element );
    
    // Initialize element
    element->InitializeL( aTarget, aText );
    
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    // Set content priority
    element->SetContentPriority( aPriority );
    
    return element;
    }

MTransactionElement*
    CTransactionFactoryImpl::CreateDataBufferTransactionElementL(
        CXnNodeAppIf& aTarget,
        const TDesC8& aData,
        TInt aPriority )
    {
    // Instantiate element
    CDataBufferTransactionElement* element = NewInstanceL< CDataBufferTransactionElement >();
    CleanupStack::PushL( element );
    
    // Initialize
    element->InitializeL( aTarget, aData );
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    // Set content priority
    element->SetContentPriority( aPriority );
    
    return element;
    }    

MTransactionElement*
    CTransactionFactoryImpl::CreateDataBufferTransactionElementL(
        CXnNodeAppIf& aTarget,
        const TDesC8& aData,
        TInt aPriority,
        const TDesC& aCid, 
        TInt aIndex )
    {
    // Instantiate element
    CDataBufferTransactionElement* element = NewInstanceL< CDataBufferTransactionElement >();
    CleanupStack::PushL( element );
    
    // Initialize
    element->InitializeL( aTarget, aData, aCid, aIndex );
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    // Set content priority
    element->SetContentPriority( aPriority );
        
    return element;
    }    

MTransactionElement*
    CTransactionFactoryImpl::CreateEmptyContentTransactionElementL(
        CXnNodeAppIf& aTarget,
        TInt aIndex)
    {
    // Instantiate
    CEmptyContentTransactionElement* element = NewInstanceL< CEmptyContentTransactionElement >();
    
    CleanupStack::PushL( element );
    
    // Initialize
    element->InitializeL( aTarget, aIndex );
    
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    return element;
    }
  
MTransactionElement* CTransactionFactoryImpl::CreateImageTransactionElementL( CXnNodeAppIf& aTarget,
                                                                              CGulIcon* aIcon,
                                                                              TInt aPriority )
    {
    // Instantiate
    CImageTransactionElement* element = NewInstanceL< CImageTransactionElement >();
    
    CleanupStack::PushL( element );
    
    // Initialize
    element->InitializeL( aTarget, aIcon );
    
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    // Set priority
    element->SetContentPriority( aPriority );
    
    return element;
    }

MTransactionElement* CTransactionFactoryImpl::CreateImageTransactionElementL(
    CXnNodeAppIf& aTarget,
    RFile& aFile,
    TInt aPriority )
    {
    // Instantiate
    CImageTransactionElement* element = NewInstanceL< CImageTransactionElement >();
    
    CleanupStack::PushL( element );
    
    // Initialize
    element->InitializeL( aTarget, aFile );
    
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    // Set priority
    element->SetContentPriority( aPriority );
    
    return element;
    }

MTransactionElement* CTransactionFactoryImpl::CreateNewsTickerTransactionElementL(
    CXnNodeAppIf& aTarget,
    const TDesC& aText,
    TInt aPriority, 
    TInt aIndex )
    {
    // Instantiate new element
    CNewsTickerTransactionElement* element = NewInstanceL< CNewsTickerTransactionElement >();
    
    CleanupStack::PushL( element );
    
    // Initialize element
    element->InitializeL( aTarget, aText, aIndex );
    
    CleanupStack::Pop( element );
    
    // Reserve
    iReservedElements.AddLast( *element );
    
    // Set content priority
    element->SetContentPriority( aPriority );
    
    return element;
    }
            
void CTransactionFactoryImpl::ReleaseTransactionElement( MTransactionElement* aElement)
    {
    CTransactionElement* element = dynamic_cast< CTransactionElement* >( aElement );
    
    if ( element )
        {
        // Reset element
        element->Reset();
        
        // Move element from reserved list to available
        iReservedElements.Remove( *element );
        iAvailableElements.AddFirst( *element );
        }
    }

TBool CTransactionFactoryImpl::IsSupported( CXnNodeAppIf& aTarget, const TDesC8& aContentType )
    {
    // Check if target node and content type is supported
    if ( aContentType == KContentTypeText )
        {
        return CTextTransactionElement::IsSupported( aTarget ) || CNewsTickerTransactionElement::IsSupported( aTarget );
        }
    else if ( aContentType == KContentTypeBitmap )
        {
        return CImageTransactionElement::IsSupported( aTarget );
        }
    else
        {
        return CDataBufferTransactionElement::IsSupported( aTarget, aContentType ); 
        }
    }

template< class T > T* CTransactionFactoryImpl::NewInstanceL()
    {
    T* instance = NULL;
    
    // Iterate list of transaction elements
    TSglQueIter< CTransactionElement > iter( iAvailableElements );
    iter.SetToFirst();
    
    while ( iter )
        {
        instance = dynamic_cast< T* >( iter++ );   // NULL if not T*
        
        if ( instance )
            {
            // Deque first instance of class T and quit
            iAvailableElements.Remove( *instance );
            return instance;
            }
        }
    
    // Not found from list. Instantiate
    instance = T::NewL(iContentPriorityMap);
    instance->SetCssPropertyMap( &iPropertyMap );    
    
    return instance;
    }

void CTransactionFactoryImpl::DeleteTransactionList( TSglQue< CTransaction >& aList )
    {
    TSglQueIter< CTransaction > trIter( aList );
    trIter.SetToFirst();
    
    while (trIter)
        {
        // Get first transaction and remove it from the list
        CTransaction* first = trIter++;
        aList.Remove( *first );
        
        // Delete
        first->Reset( *this );
        delete first;
        }
    }
            
void CTransactionFactoryImpl::DeleteElementList( TSglQue< CTransactionElement >& aList )
    {
    TSglQueIter< CTransactionElement > iter( aList );
    iter.SetToFirst();
        
    while ( iter )
        {
        CTransactionElement* element = iter++;
        aList.Remove( *element );
        
        delete element;
        }
    }

//  End of File
