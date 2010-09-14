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


#ifndef C_TRANSACTIONFACTORYIMPL_H
#define C_TRANSACTIONFACTORYIMPL_H


#include <e32base.h>
#include "transactionfactory.h"
#include "aipolicyelement.h"

namespace AiUtility
    {
    class CContentPriorityMap;
    }

namespace AiXmlUiController
{
    
class CTransaction;
class CTransactionElement;
class CCssPropertyMap;


/**
*  @ingroup group_xmluicontroller
* 
*  CTransactionFactoryImpl implements MTransactionFactory.
*  It provides a recycling pool of transaction objects.
*
*  @lib AiXmlUiMain
*/
class CTransactionFactoryImpl: public CBase, public MTransactionFactory
    {
public:  // Constructors and destructor
    
    static CTransactionFactoryImpl*
        NewL( AiUtility::CContentPriorityMap& aContentPriorityMap,
              CCssPropertyMap& aPropertyMap );
    
    virtual ~CTransactionFactoryImpl();

// Functions from MTransactionFactory

    MTransaction* CreateTransactionL( TInt aTxId );
    
    void ReleaseTransaction( MTransaction* aTransaction );

    MTransactionElement* CreateTextTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC& aText, TInt aPriority );
                                                                                                                   
    MTransactionElement* CreateDataBufferTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC8& aData, TInt aPriority );
                                                                                                                
    MTransactionElement* CreateDataBufferTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC8& aData, TInt aPriority, 
        const TDesC& aCid, TInt aIndex );
                                                        
    MTransactionElement* CreateEmptyContentTransactionElementL( 
        CXnNodeAppIf& aTarget, TInt aIndex );
                                                                
    MTransactionElement* CreateImageTransactionElementL( 
        CXnNodeAppIf& aTarget, CGulIcon* aIcon, TInt aPriority );
                                                                                                                            
    MTransactionElement* CreateImageTransactionElementL( 
        CXnNodeAppIf& aTarget, RFile& aFile, TInt aPriority );

    MTransactionElement* CreateNewsTickerTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC& aText, TInt aPriority, TInt aIndex );
    
    void ReleaseTransactionElement( MTransactionElement* aElement );

    TBool IsSupported( CXnNodeAppIf& aTarget, const TDesC8& aContentType );
    
    RAiPolicyElementArray& CTransactionFactoryImpl::PolicyArray();

    void CTransactionFactoryImpl::ResetPolicyArray();

private: // Construction

    CTransactionFactoryImpl(
        AiUtility::CContentPriorityMap& aContentPriorityMap,
        CCssPropertyMap& aPropertyMap );

    void ConstructL();
    
    template<class T> T* NewInstanceL();
    
// New methods    
    
    void DeleteTransactionList( TSglQue< CTransaction >& aList );
    
    void DeleteElementList( TSglQue< CTransactionElement >& aList );
                
private: // Data
    
    /**
     * List of available transactions.
     */
    TSglQue< CTransaction > iAvailableTransactions;
    
    /**
     * List of reserved transactions.
     */
    TSglQue< CTransaction > iReservedTransactions;
    
    /**
     * List of available transaction elements.
     */
    TSglQue< CTransactionElement > iAvailableElements;
    
    /**
     * List of reserved transaction elements.
     */
    TSglQue< CTransactionElement > iReservedElements;
    
    /**
     * Reference to UI controller's content priority map
     */
    AiUtility::CContentPriorityMap& iContentPriorityMap;
    
    /**
     * UI controller's CSS property map. Not own.
     */
    CCssPropertyMap& iPropertyMap;

    /**
     * Array of policy elements
     */
    RAiPolicyElementArray iPolicyArray;
    };

} // namespace AiXmlUiController

#endif      // C_TRANSACTIONFACTORYIMPL_H  
           
//  End of File
