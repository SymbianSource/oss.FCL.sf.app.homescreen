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

#include <e32base.h>
#include <e32hashtab.h>
#include "xnnewsticker.h"
#include "aicontentobserver.h"

class TXnUiEngineAppIf;
class CXnNodeAppIf;
class CGulIcon;
class MAiFwEventHandler;
struct TAiPublisherInfo;

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
    class CNewstickerCallbackHandler;
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
                         public MAiContentObserver,
                         public XnNewstickerInterface::MXnNewstickerCallbackInterface
    {
public:  // Constructors and destructor
    
    static CContentRenderer* NewL( CAppUi& aAppUi );
    
    virtual ~CContentRenderer();

    /**
     * Sets event handler.
     * 
     * @param aFwEventHandler the event handler to use.
     */
    void SetEventHandler( MAiFwEventHandler& aFwEventHandler );

    /**
     * Resets content renderer for new theme.
     */
    void Reset();

    /**
     * Cleans all content items published by aPlugin.
     */
    void CleanPluginFromUi( MAiPropertyExtension& aPlugin );

// Functions from MAiContentObserver

    TInt StartTransaction( TInt aTxId );

    TInt Commit( TInt aTxId );

    TInt CancelTransaction( TInt aTxId );

    TBool CanPublish( MAiPropertyExtension& aPlugin,
                      TInt aContent,
                      TInt aIndex );

    TInt Publish( MAiPropertyExtension& aPlugin,
                  TInt aContent,
                  TInt aResource,
                  TInt aIndex );

    TInt Publish( MAiPropertyExtension& aPlugin,
                  TInt aContent,
                  const TDesC16& aText,
                  TInt aIndex );

    TInt Publish( MAiPropertyExtension& aPlugin,
                  TInt aContent,
                  const TDesC8& aBuf,
                  TInt aIndex );

    TInt Publish( MAiPropertyExtension& aPlugin,
                  TInt aContent,
                  RFile& aFile,
                  TInt aIndex );

    TInt Clean( MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex );

    TAny* Extension( TUid aUid );
    
    TBool RequiresSubscription( const TAiPublisherInfo& aPublisherInfo ) const;
	        
private:

    CContentRenderer( CAppUi& aAppUi );
    
    void ConstructL();
    
    void DoStartTransactionL( TInt aTxId );

    TInt CanPublishL( MAiPropertyExtension& aPlugin,
                      TInt aContent,
                      TInt aIndex );

    TInt DoPublishL( MAiPropertyExtension& aPlugin,
                     TInt aContent,
                     TInt aResource,
                     TInt aIndex);
    
    TInt DoPublishL( MAiPropertyExtension& aPlugin,
                     TInt aContent,
                     const TDesC16& aText,
                     TInt aIndex );
    
    TInt DoPublishL( MAiPropertyExtension& aPlugin,
                     TInt aContent,
                     const TDesC8& aBuf,
                     TInt aIndex );
    
    TInt DoPublishL( MAiPropertyExtension& aPlugin,
                     TInt aContent,
                     RFile& aFile,
                     TInt aIndex );
    
    TInt DoCleanL( MAiPropertyExtension& aPlugin,
                   TInt aContent,
                   TInt aIndex );
    
    void SetImmediateMode( TBool aImmediateMode );
    
    TBool IsImmediateMode() const;
    
    void ProcessTransactionElementL( MTransactionElement* aElement );

    CXnNodeAppIf* FindNodeByClassL( const TDesC& aCid,
                                    TInt aIndex,
                                    const TDesC8& aNs );

    CXnNodeAppIf* FindNodeByIdL( const TDesC& aCid, const TDesC& aNs = KNullDesC );

    CXnNodeAppIf* FindNodeByIdL( const TDesC8& aCid, const TDesC8& aNs = KNullDesC8 );
    
    TInt PublishIconL( MAiPropertyExtension& aPlugin,
                       const TDesC& aCid,
                       CGulIcon* aIcon,
                       TInt aIndex,
                       CXnNodeAppIf* aResource = NULL );
                       
    TInt PublishDataL( MAiPropertyExtension& aPlugin,
                       const TDesC& aCid,
                       const TDesC8& aData,
                       const TDesC8& aContentType,
                       TInt aIndex,
                       CXnNodeAppIf* aResource = NULL );
    
    TBool AllowPublishByPriority( CXnNodeAppIf& aUiElement,
                                  TInt aNewPriority ) const;
    
    void StartContentRefresh();
    
    TInt RefreshContentL( HBufC* aUiElementId,
                          TInt aOldPriority );
    
    static TInt RefreshContentCallback( TAny* aContentRenderer );
    
    void SendRefreshContentEventL();
    
    TBool IsParentNewsticker( CXnNodeAppIf& aTarget );
    
    void RegisterNewstickerCallbackInterfaceL( CXnNodeAppIf& aTarget );
    
// From XnNewstickerInterface::MXnNewstickerCallbackInterface    

    void TitleScrolled(TInt aTitleIndex);
    
    void TitleToScroll(TInt aTitleIndex);
    
    void CleanPluginFromUiL( MAiPropertyExtension& aPlugin );
    
    void ProcessContentChangesL( MTransaction& aTr );
    void ProcessContentChangeL( TAiPolicyElement& aElement );     

private:    // Data
    
    /**
     * Transaction stack. Own.
     */
    CActiveTransactionStack* iStack;
    
    /**
     * Factory for transacations. Own.
     */
    CTransactionFactoryImpl* iFactory;
    
    /**
     * Immediate mode flag. Own.
     */
    TBool iImmediateMode;
    
    /**
     * App UI
     */
    CAppUi& iAppUi;
    
    /**
     * Node lookup id generator. Own.
     */
    CXmlNodeIdGenerator* iNodeIdGenerator;
    
    /**
     * Content priority map. Own.
     */
    AiUtility::CContentPriorityMap* iContentPriorityMap;
    
    /**
     * Map of refreshable ui elements.
     */
    RPtrHashMap< TDesC, TInt > iRefreshableUiElements;
    
    /**
     * Timer for asynchronous content refreshing
     */
    CPeriodic* iTimer;
    
    /**
     * Fw event handler needed for content refresh event notifications.
     * Not own.
     */
    MAiFwEventHandler* iFwEventHandler;
    
    /**
     * Newsticker callback handler
     */
    CNewstickerCallbackHandler* iCallbackHandler;
    
    /**
     * Pointer descriptor to newsticker plugin name
     */
    TPtrC iNTPublisher;
    
    /**
     * Newsticker element property class.
     */
    HBufC* iNTClass;
    
    /**
     * CSS property map. Own
     */
    CCssPropertyMap* iPropertyMap;
    
    /**
     * Publishing policy evaluator. Own.
     */
    CPolicyEvaluator* iPolicyEvaluator;
    
    };

} // namespace AiXmlUiController

#endif      // C_CONTENTRENDERER_H  
           
//  End of File
