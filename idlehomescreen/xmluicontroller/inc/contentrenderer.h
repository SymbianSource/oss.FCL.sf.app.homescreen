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
* Description:  MAiContentObserver implementation for XML UI controller
*
*/


#ifndef C_CONTENTRENDERER_H
#define C_CONTENTRENDERER_H

// System includes
#include <e32base.h>
#include <e32hashtab.h>

// User includes
#include "aicontentobserver.h"
#include "xnproperty.h"

// Forward declarations
class TXnUiEngineAppIf;
class CXnNodeAppIf;
class CGulIcon;
class MAiFwEventHandler;
class CHsContentPublisher;
class THsPublisherInfo;

namespace AiUtility
    {
    class CContentPriorityMap;
    }

namespace AiXmlUiController
    {
    class CActiveTransactionStack;
    class CTransactionFactoryImpl;
    class MTransactionElement;
    class MTransaction;
    class CAppUi;
    class CXmlNodeIdGenerator;
    class CCssPropertyMap;
    class CPolicyEvaluator;
    class TAiPolicyElement;
    }

namespace AiXmlUiController
{

/**
*  @ingroup group_xmluicontroller
* 
*  CContentRenderer renders the content of the content publisher plug-ins
*
*  @lib AiXmlUiMain
*/
class CContentRenderer : public CBase,
    public MAiContentObserver    
    {
public:  
    // Constructors and destructor
    
    static CContentRenderer* NewL( CAppUi& aAppUi );
    
    ~CContentRenderer();

public:   
    // New functions
    
    /**
     * Sets event handler.
     * 
     * @param aFwEventHandler the event handler to use.
     */
    void SetEventHandler( MAiFwEventHandler& aFwEventHandler );

    /**
     * Cleans all content items published by aPlugin.
     */
    void CleanPluginFromUi( CHsContentPublisher& aPlugin );

public:    
    // from MAiContentObserver

    /**
     * @see MAiContentObserver 
     */
    TInt StartTransaction( TInt aTxId );

    /**
     * @see MAiContentObserver 
     */    
    TInt Commit( TInt aTxId );

    /**
     * @see MAiContentObserver 
     */    
    TInt CancelTransaction( TInt aTxId );

    /**
     * @see MAiContentObserver 
     */    
    TBool CanPublish( CHsContentPublisher& aPlugin,
          TInt aContent, TInt aIndex );
                      
    /**
     * @see MAiContentObserver 
     */    
    TInt Publish( CHsContentPublisher& aPlugin,
        TInt aContent, TInt aResource, TInt aIndex );
                                   
    /**
     * @see MAiContentObserver 
     */    
    TInt Publish( CHsContentPublisher& aPlugin,
        TInt aContent, const TDesC16& aText, TInt aIndex );

    /**
     * @see MAiContentObserver 
     */        
    TInt Publish( CHsContentPublisher& aPlugin,
        TInt aContent, const TDesC8& aBuf, TInt aIndex );

    /**
     * @see MAiContentObserver 
     */        
    TInt Publish( CHsContentPublisher& aPlugin,
        TInt aContent, RFile& aFile, TInt aIndex );

    /**
     * @see MAiContentObserver 
     */        
    TInt Clean( CHsContentPublisher& aPlugin, 
        TInt aContent, TInt aIndex );

    /**
     * @see MAiContentObserver 
     */        
    TAny* Extension( TUid aUid );
    
    /**
     * @see MAiContentObserver 
     */        
    TBool RequiresSubscription( 
        const THsPublisherInfo& aPublisherInfo ) const;

    /**
     * @see MAiContentObserver 
     */            
    TInt SetProperty( CHsContentPublisher& aPlugin,
        const TDesC8& aElementId, const TDesC8& aPropertyName,           
        const TDesC8& aPropertyValue );
       
    /**
     * @see MAiContentObserver 
     */            
    TInt SetProperty( CHsContentPublisher& aPlugin,
        const TDesC8& aElementId, const TDesC8& aPropertyName,        
        const TDesC8& aPropertyValue, 
        MAiContentObserver::TValueType aValueType );  
                  
private:
    // Constructors
    
    /**
     * C++ default constructor
     */
    CContentRenderer( CAppUi& aAppUi );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
private:
    // New functions

    void DoStartTransactionL( TInt aTxId );

    TInt CanPublishL( CHsContentPublisher& aPlugin,
        TInt aContent, TInt aIndex );
                      
    TInt DoPublishL( CHsContentPublisher& aPlugin,
        TInt aContent, TInt aResource, TInt aIndex );
    
    TInt DoPublishL( CHsContentPublisher& aPlugin,
        TInt aContent, const TDesC16& aText, TInt aIndex );
    
    TInt DoPublishL( CHsContentPublisher& aPlugin,
        TInt aContent, const TDesC8& aBuf, TInt aIndex );

    TInt DoPublishL( CHsContentPublisher& aPlugin,
        TInt aContent, RFile& aFile, TInt aIndex );

    TInt DoCleanL( CHsContentPublisher& aPlugin,
        TInt aContent, TInt aIndex );
                       
    void SetImmediateMode( TBool aImmediateMode );
    
    TBool IsImmediateMode() const;
    
    void ProcessTransactionElementL( 
        MTransactionElement* aElement );

    CXnNodeAppIf* FindNodeByClassL( 
        const TDesC& aCid, TInt aIndex, const TDesC8& aNs );

    CXnNodeAppIf* FindNodeByIdL( 
        const TDesC& aCid, const TDesC& aNs = KNullDesC );

    CXnNodeAppIf* FindNodeByIdL( 
        const TDesC8& aCid, const TDesC8& aNs = KNullDesC8 );
    
    TInt PublishIconL( CHsContentPublisher& aPlugin,
        const TDesC& aCid, CGulIcon* aIcon, TInt aIndex, 
        CXnNodeAppIf* aResource = NULL );
                       
    TInt PublishDataL( CHsContentPublisher& aPlugin,
        const TDesC& aCid, const TDesC8& aData,
        const TDesC8& aContentType, TInt aIndex,
        CXnNodeAppIf* aResource = NULL );
    
    TBool AllowPublishByPriority( 
        CXnNodeAppIf& aUiElement, TInt aNewPriority ) const;
                                      
    void StartContentRefresh();
    
    TInt RefreshContentL( HBufC* aUiElementId, TInt aOldPriority ); 
                                      
    static TInt RefreshContentCallback( TAny* aContentRenderer );
    
    void SendRefreshContentEventL();
    
    TBool IsParentNewsticker( CXnNodeAppIf& aTarget );

    void ProcessContentChangesL( MTransaction& aTr );
    void ProcessContentChangeL( TAiPolicyElement& aElement );         
    
    TInt SetPropertyL( CHsContentPublisher& aPlugin,
       const TDesC8& aElementId, const TDesC8& aPropertyName,               
       const TDesC8& aPropertyValue, MAiContentObserver::TValueType aValueType );  
                   
    CXnDomPropertyValue::TPrimitiveValueType DomPropertyValueType( 
        MAiContentObserver::TValueType aValueType ); 
            
private:    
    // data
    
    /** Transaction stack, Owned. */
    CActiveTransactionStack* iStack;    
    /** Factory for transacations, Owned. */
    CTransactionFactoryImpl* iFactory;    
    /** Immediate mode flag, Owned. */
    TBool iImmediateMode;    
    /** AppUI, Not owned. */
    CAppUi& iAppUi;    
    /** Node lookup id generator, Owned. */
    CXmlNodeIdGenerator* iNodeIdGenerator;    
    /** Content priority map, Owned. */
    AiUtility::CContentPriorityMap* iContentPriorityMap;    
    /** Map of refreshable ui elements. */
    RPtrHashMap< TDesC, TInt > iRefreshableUiElements;    
    /** Timer for asynchronous content refreshing, Owned. */
    CPeriodic* iTimer;    
    /** Fw event handler, Not owned. */
    MAiFwEventHandler* iFwEventHandler;
    /** CSS property map, Owned. */
    CCssPropertyMap* iPropertyMap;    
    /** Publishing policy evaluator, Owned. */
    CPolicyEvaluator* iPolicyEvaluator;
    };

} // namespace AiXmlUiController

#endif      // C_CONTENTRENDERER_H  
           
//  End of File
