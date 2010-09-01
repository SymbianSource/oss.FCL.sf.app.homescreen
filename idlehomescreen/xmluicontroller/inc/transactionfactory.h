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
* Description:  Transaction factory
*
*/


#ifndef M_TRANSACTIONFACTORY_H
#define M_TRANSACTIONFACTORY_H

namespace AiXmlUiController
    {
    class MTransaction;
    class MTransactionElement;
    }

class CXnNodeAppIf;
class CGulIcon;
class RFile;


namespace AiXmlUiController
{

/**
*  MTransactionFactory provides life-cycle management of transactions and
*  transaction elements.
*
*  @lib AiXmlUiMain
*/
class MTransactionFactory
    {
public:

    // New functions
    
    /**
    * Creates a new transaction.
    *
    * @param aTxId the transaction identifier
    */
    virtual MTransaction* CreateTransactionL( TInt aTxId ) = 0;
    
    /**
    * Releases a transaction element.
    *
    * @param aTransaction the transaction to release
    */
    virtual void ReleaseTransaction( MTransaction* aTransaction ) = 0;

    /**
    * Creates a new text transaction element.
    *
    * @param aTarget the target node of the transaction element
    * @param aText the text to update 
    * @param aPriority Priority
    */
    virtual MTransactionElement* CreateTextTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC& aText, TInt aPriority ) = 0;                                       

    /**
    * Creates a new data buffer transaction element.
    *
    * @param aTarget the traget node of the transaction element
    * @param aData the data to update
    * @param aPriority Priority
    * @param aCid Content selector id
    * @param aIndex Content index
    */
    virtual MTransactionElement* CreateDataBufferTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC8& aData, TInt aPriority, const TDesC& aCid, TInt aIndex ) = 0;
    
    /**
    * Creates a new data buffer transaction element.
    *
    * @param aTarget the traget node of the transaction element
    * @param aData the data to update
    * @param aPriority Priority    
    */
    virtual MTransactionElement* CreateDataBufferTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC8& aData, TInt aPriority ) = 0;
        
    /**
    * Creates a new empty content transaction element.
    *
    * @param aTarget the target node of the transaction element
    * @param aText the text to update 
    */
    virtual MTransactionElement* CreateEmptyContentTransactionElementL( 
        CXnNodeAppIf& aTarget, TInt aIndex ) = 0;

    /**
    * Creates a new image transaction element.
    *
    * @param aTarget the target node of the transaction element
    * @param aIcon the icon to update. Ownership transferred.
    * @param aPriority Priority
    */
    virtual MTransactionElement* CreateImageTransactionElementL( 
        CXnNodeAppIf& aTarget, CGulIcon* aIcon, TInt aPriority ) = 0;
    
    /**
    * Creates a new image transaction element.
    *
    * @param aTarget the target node of the transaction element
    * @param aFile the file to read image data from.
    * @param aPriority Priority
    */
    virtual MTransactionElement* CreateImageTransactionElementL( 
        CXnNodeAppIf& aTarget, RFile& aFile, TInt aPriority ) = 0;

    /**
    * Creates a new newsticker transaction element.
    *
    * @param aTarget the target node of the transaction element
    * @param aText the text to update     
    * @param aPriority Priority
    * @param aIndex Newsticker title index
    */
    virtual MTransactionElement* CreateNewsTickerTransactionElementL( 
        CXnNodeAppIf& aTarget, const TDesC& aText, TInt aPriority, TInt aIndex ) = 0;
                                                        
    /**
    * Releases a transaction element.
    *
    * @param aElement the element to release.
    */
    virtual void ReleaseTransactionElement(
        MTransactionElement* aElement ) = 0;

    /**
    * Checks if given content type is supported by target element.
    *
    * @param aTarget the target UI element.
    * @param aContentType the content type
    * @return ETrue if content type is supported, otherwise EFalse.
    */
    virtual TBool IsSupported( 
        CXnNodeAppIf& aTarget, const TDesC8& aContentType ) = 0;                               

protected:

    ~MTransactionFactory(){};        
    };

} // namespace AiXmlUiController

#endif      // M_TRANSACTIONFACTORY_H  
           
//  End of File
