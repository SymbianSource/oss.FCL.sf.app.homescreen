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
* Description:  Content renderer implementation
*
*/

// System includes
#include    <e32hashtab.h>
#include    <utf.h>
#include    <gulicon.h>

// User includes

#include    "contentrenderer.h"
#include    "appui.h"
#include    "activetransactionstack.h"
#include    "transactionfactoryimpl.h"
#include    "mtransaction.h"
#include    "mtransactionelement.h"
#include    "aixmluiutils.h"
#include    "xmluicontroller.h"
#include    "xmlnodeidgenerator.h"
#include    "aixmluiconstants.h"
#include    "aifweventhandler.h"
#include    "databuffertransactionelement.h"
#include    "newstickercallbackhandler.h"
#include    "newstickertransactionelement.h"
#include    "csspropertymap.h"
#include    "policyevaluator.h"
#include    "debug.h"
#include    "xnuiengineappif.h"
#include    "xnnodeappif.h"
#include    "xnproperty.h"
#include    "mxncomponentinterface.h"
#include    "xntext.h"
#include    "xnbitmap.h"
#include    "xntype.h"
#include    "xnmenuadapter.h"
#include    "xnlistquerydialogadapter.h"
#include    "mxncomponentinterface.h"
#include    "aistrcnv.h"
#include    "contentprioritymap.h"
#include    "ainativeuiplugins.h"

using namespace AiXmlUiController;
using namespace AiUiDef::xml;
using namespace XnTextInterface;
using namespace XnImageInterface;

namespace
    {
    typedef RPointerArray<CXnNodeAppIf> RXnNodeArray;

    /**
     * Gets content item iterator.
     *
     * @param aPlugin plugin to fetch the iterator
     * @param aContentType type of iterator (content/resource/event)
     * @return the iterator
     * @leave KErrNotSupported if plugin does not provide iterator
     */ 
    static MAiContentItemIterator& ContentItemIteratorL( MAiPropertyExtension& aPlugin,
                                                         TInt aContentType )
        {
        MAiContentItemIterator* iter =
            static_cast< MAiContentItemIterator* >( aPlugin.GetPropertyL( aContentType ) );

        if ( !iter )
            {
            User::Leave( KErrNotSupported );
            }

        return *iter;
        }

    /**
     * Gets the content priority associated in the property element.
     *
     * @param aPropertyElement the property element
     * @return priority value or KErrNotFound for non-prioritized element
     */
    TInt GetContentPriority( CXnNodeAppIf& aPropertyElement )
        {
        TInt32 priority( KErrNotFound );
        
        const TDesC8* name = PropertyValue( aPropertyElement,
                                            property::KName );
                                            
        
        if ( name && *name == name::KPriority )
            {
            const TDesC8* value = PropertyValue( aPropertyElement,
                                                 property::KValue );
            if ( value )
                {
                AiUtility::ParseInt( priority, *value );
                }
            }
            
        return priority;
        }
    
    /**
     * Remove elements from array which do not contain priority or priority is not lower than
     * the given value.
     * 
     * @param aElements array of elements
     * @param aLastPriority upper bound of priority value (excluded from the array)
     */
    void RemoveNonPriorityElements( RPointerArray< CXnNodeAppIf>& aElements,
                                    TInt aLastPriority )
        {
        //  Remove non priority elements and higher priority elements

        TInt elementCount = aElements.Count();    
        for ( TInt i = 0; i < elementCount; )
            {
            CXnNodeAppIf* current = aElements[i];
            
            // Check name attribute
            const TDesC8* name = PropertyValue( *current, property::KName );
            
            if ( !name || *name != name::KPriority )
                {
                // Remove current
                aElements.Remove( i );
                --elementCount;
                }
            else
                {
                // Check current priority
                const TDesC8* value = PropertyValue( *current, property::KValue );
                
                if ( !value ) // value not present
                    {
                    aElements.Remove( i );
                    --elementCount;
                    continue;
                    }
                
                TInt32 currentPriority( KErrNotFound );
                if ( AiUtility::ParseInt( currentPriority, *value ) != KErrNone )
                    {
                    // value is not integer
                    aElements.Remove( i );
                    --elementCount;
                    continue;
                    }
                    
                if ( currentPriority < aLastPriority )
                    {
                    // Keep element and iterate further
                    ++i;
                    }
                else
                    {
                    // priority is too high
                    aElements.Remove( i );
                    --elementCount;
                    }
                }
            }
        }
    
    /**
     * Descending priority order for prioritized content selectors.
     *
     * @param aNode1 First node to compare
     * @param aNode2 Second node to compare
     * @return 0 nodes have equal priority
     * @return >0 aNode1 has lower priority
     * @return <0 aNode2 has lower priority
     */
    TInt DescendingPriorityOrder( const CXnNodeAppIf& aNode1,
                                  const CXnNodeAppIf& aNode2 )
        {
        // Array content has been validated, so no checks are needed
        const TDesC8* value1 = PropertyValue( aNode1,
                                              property::KValue );
        
        const TDesC8* value2 = PropertyValue( aNode2,
                                              property::KValue );
        
        TInt32 priority1( KErrNotFound );
        AiUtility::ParseInt( priority1, *value1 );
        
        TInt32 priority2( KErrNotFound );
        AiUtility::ParseInt( priority2, *value2 );
        
        if ( priority1 == priority2 )
            {
            return 0;
            }
            
        return ( priority1 < priority2 ) ? 1 : -1;
        }
    
    /**
     * Removes duplicate entries in content change array
     */
    void RemoveDuplicateContentChangesL( RAiPolicyElementArray& aArray )
        {
        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            HBufC* id = PropertyValueL( aArray[i].Target(), 
                                            AiUiDef::xml::property::KId );
            if ( id )
                {
                CleanupStack::PushL( id );
                for ( TInt j = i; j < aArray.Count(); ++j )
                    {
                    HBufC* id2 = PropertyValueL( aArray[j].Target(), 
                                                           AiUiDef::xml::property::KId );
                    if ( id2)
                        {
                        CleanupStack::PushL( id2 );
                        // Same id and same policy
                        if ( i != j &&
                             id->Compare( *id2 ) == 0 && 
                             ( aArray[i].Policy().Compare( aArray[j].Policy()) == 0) 
                                )
                            {
                            aArray.Remove( j );
                            --j;
                            }
                        }
                    CleanupStack::PopAndDestroy( id2 );
                    
                    }
                CleanupStack::PopAndDestroy( id );
                }
            }
        }
    /**
     * Cleanup item for cleanup of TPtrHashMapIter
     */            
    class TMapCleanupItem
        {
    public:
        
        TMapCleanupItem( TPtrHashMapIter< TDesC, TInt >& aIterator );
        
        /**
         * Removes the pointers in the map and deletes the objects
         * referenced by the pointers.
         */ 
        void Release();
                
    private:
            
        TPtrHashMapIter< TDesC, TInt > iIterator;
        
        };

    /**
     * Helper to handle cleanup of map iterator
     * @param aMapCleanupItem 
     */
    void CleanupRelease( TAny* aMapCleanupItem )
        {
        if ( aMapCleanupItem )
            {
            static_cast< TMapCleanupItem* >( aMapCleanupItem )->Release();
            }
        }

    /**
     * Helper to push map iterator into cleanup stack.
     *
     * @param aCleanupItem item to push into cleanup stack
     **/
    void CleanupReleasePushL( TMapCleanupItem& aCleanupItem )
        {
        CleanupStack::PushL( TCleanupItem( CleanupRelease,
                                           &aCleanupItem ) );
        }

    
    }

TMapCleanupItem::TMapCleanupItem( TPtrHashMapIter< TDesC, TInt >& aIterator )
    : iIterator( aIterator )
    {
    }

void TMapCleanupItem::Release()
    {
    // Delete current key and value
    const TDesC* key = iIterator.CurrentKey();
    const TInt* value = iIterator.CurrentValue();
    
    delete key;
    delete value;
    
    // Remove mapping from the map.
    iIterator.RemoveCurrent();
    }



// ============================ MEMBER FUNCTIONS ===============================

CContentRenderer::CContentRenderer( CAppUi& aAppUi )
    : iAppUi( aAppUi )
    {
    }

void CContentRenderer::ConstructL()
    {
    iContentPriorityMap = AiUtility::CContentPriorityMap::NewL();
    iPropertyMap = CCssPropertyMap::NewL();
    iFactory = CTransactionFactoryImpl::NewL(*iContentPriorityMap,
                                             *iPropertyMap);
    iStack = CActiveTransactionStack::NewL();
    iNodeIdGenerator = CXmlNodeIdGenerator::NewL();
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iPolicyEvaluator = CPolicyEvaluator::NewL();
    }

CContentRenderer* CContentRenderer::NewL( CAppUi& aAppUi )
    {
    CContentRenderer* self = new( ELeave ) CContentRenderer( aAppUi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CContentRenderer::~CContentRenderer()
    {
    delete iPolicyEvaluator;
    
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }
        
    delete iNodeIdGenerator;
    
    if ( iStack )
        {
        while ( !iStack->IsEmpty() )
            {
            MTransaction* tr = iStack->Pop();
            iFactory->ReleaseTransaction( tr );
            }

        delete iStack;
        }
    
    delete iFactory;
    
    iRefreshableUiElements.ResetAndDestroy();
    
    delete iNTClass;
    
    delete iPropertyMap;
    
    delete iContentPriorityMap;
    delete iCallbackHandler;
    }

void CContentRenderer::SetEventHandler( MAiFwEventHandler& aFwEventHandler )
    {
    iFwEventHandler = &aFwEventHandler;
    }

void CContentRenderer::Reset()
    {
    iContentPriorityMap->Reset();
    iPropertyMap->Reset();
    }

void CContentRenderer::CleanPluginFromUi( MAiPropertyExtension& aPlugin )
    {
    // Create transaction to clean UI
    TInt txId = reinterpret_cast< TInt >( &aPlugin );
    
    TBool txCreated = ( StartTransaction( txId ) == KErrNone );
    
    // Clean plugin
    TRAPD( cleanError, CleanPluginFromUiL( aPlugin ) );
    
    // Commit transaction
    if ( txCreated )
        {
        if (cleanError )
            {
            CancelTransaction( txId );
            }
        else
            {
            Commit( txId );
            }
        }
    }

TInt CContentRenderer::StartTransaction( TInt aTxId )
    {
    __PRINT(__DBG_FORMAT("\t[I]\tXML UI: Start transaction id=%d"), aTxId);
    __HEAP("XML UI: Start transaction");
    __TICK("XML UI: Start transaction");
    TRAPD( error, DoStartTransactionL( aTxId ) );

    if ( error )
        {
        SetImmediateMode( ETrue );
        }

    return error;
    }

TInt CContentRenderer::Commit( TInt aTxId )
    {
    // Remove transaction with aTxId from stack
    MTransaction* tr = iStack->Remove( aTxId );

    if ( tr )
        {
        // Commit transaction        
        RPropertyHashMap propertyHashMap;
        
        TBool layoutChanged( EFalse );
        
        TRAPD( error, tr->CommitL( layoutChanged, propertyHashMap ) );
        if( error == KErrNone )
            {
            TRAP_IGNORE( SetPropertyArraysL( propertyHashMap ) );
            }
        propertyHashMap.Close();

        __TICK("XML UI: Commit transaction");
        __HEAP("XML UI: Commit transaction");
        __PRINT(__DBG_FORMAT("\t[I]\tXML UI: Commit transaction id=%d"), aTxId);
       
        StartContentRefresh();
        
        TRAP_IGNORE
            (
            iAppUi.UiEngineL()->RenderUIL();                                  //jtm+++
            ProcessContentChangesL( *tr );
            );
                
        
        iFactory->ReleaseTransaction( tr );  
        
        return error;
        }
        
    return KErrNotSupported;
    }

void CContentRenderer::ProcessContentChangesL( MTransaction& aTr )
    {
    TDblQueIter<CTransactionElement> iter = aTr.ElementIter();
    RAiPolicyElementArray contentChangedArray;
    CleanupClosePushL( contentChangedArray );

    while ( iter )
          {
          CTransactionElement* element = iter++;
          CXnNodeAppIf& target = element->Target();
          
          // Find own and parents policy/ContentChanged nodes 
          iPolicyEvaluator->EvaluateContentChangedPolicyL( target, 
                  contentChangedArray);

          iPolicyEvaluator->EvaluateContentChangedPolicyL( *(target.ParentL()), 
                  contentChangedArray);
          }
       
    ::RemoveDuplicateContentChangesL( contentChangedArray );
        
    for ( TInt i = 0; i < contentChangedArray.Count(); ++i )
        {
        ProcessContentChangeL( contentChangedArray[i] );
        }
    CleanupStack::PopAndDestroy();
    }

void CContentRenderer::ProcessContentChangeL( TAiPolicyElement& aElement )
    {
    const TDesC8* id = PropertyValue( aElement.Target(), 
                                AiUiDef::xml::property::KId );
    if ( id )
        {
        CXnNodeAppIf* targetNode = FindNodeByIdL( *id, aElement.Target().Namespace() );
        if ( targetNode )
            {                     
            if ( aElement.Policy() == AiUiDef::xml::value::KShowTooltips )
                {
                targetNode->ShowTooltipsL();
                }                   
            }        
        }
    }

TInt CContentRenderer::CancelTransaction( TInt aTxId )
    {
    // Remove transaction with aTxId from stack
    MTransaction* tr = iStack->Remove( aTxId );

    if ( tr )
        {
        // Release transaction to factory
        __TICK("XML UI: Cancel transaction");
        __HEAP("XML UI: Cancel transaction");
        __PRINT(__DBG_FORMAT("\t[I]\tXML UI: Cancel transaction id=%d"), aTxId);

        iFactory->ReleaseTransaction( tr );

        return KErrNone;
        }

    return KErrNotSupported;
    }

TBool CContentRenderer::CanPublish( MAiPropertyExtension& aPlugin,
                                    TInt aContent,
                                    TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    __PRINTS("*** UC: Init - Content Validation ***");
	__TIME("UC: Content Validation",
        TRAP( error, retval = CanPublishL( aPlugin, aContent, aIndex ) );
        );
    __HEAP("UC: Content Validation");
    __PRINTS("*** UC: Done - Content Validation ***");

    return ( error == KErrNone && retval == KErrNone );
    }

TInt CContentRenderer::Publish( MAiPropertyExtension& aPlugin,
                                TInt aContent,
                                TInt aResource,
                                TInt aIndex )
    {
	TInt error( KErrNone );
	TInt retval( KErrNone );
	__PRINTS("*** UC: Init - Content Publishing (Resource) ***");
	__TIME("UC: Content Publishing (Resource)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aResource, aIndex ) );
    );
    __HEAP("UC: Content Publishing (Resource)");
    __PRINTS("*** UC: Done - Content Publishing (Resource) ***");

    if( !error && retval )
        {
        error = retval;
        }
    
    return error;
    }

TInt CContentRenderer::Publish( MAiPropertyExtension& aPlugin,
                                TInt aContent,
                                const TDesC16& aText,
                                TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    
    __PRINTS("*** UC: Init - Content Publishing (Value-Text) ***");
   	__TIME("UC: Content Publishing (Value-Text)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aText, aIndex ) );
    );
    __HEAP("UC: Content Publishing (Value-Text)");
    __PRINTS("*** UC: Done - Content Publishing (Value-Text) ***");

    if( !error && retval )
        {
        error = retval;
        }

    return error;
    }

TInt CContentRenderer::Publish( MAiPropertyExtension& aPlugin,
                                TInt aContent,
                                const TDesC8& aBuf,
                                TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    
    __PRINTS("*** UC: Init - Content Publishing (Value-Buf) ***");
    __TIME("UC: Content Publishing (Value-Buf)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aBuf, aIndex ) );
    )
    __HEAP("UC: Content Publishing (Value-Buf)");
    __PRINTS("*** UC: Done - Content Publishing (Value-Buf) ***");

    if( !error && retval )
        {
        error = retval;
        }
    
    return error;
    }

TInt CContentRenderer::Publish( MAiPropertyExtension& aPlugin,
                                TInt aContent,
                                RFile& aFile,
                                TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    	
	__PRINTS("*** UC: Init - Content Publishing (Value-RFile) ***");
    __TIME("UC: Content Publishing (Value-RFile)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aFile, aIndex ) );
    );
    __HEAP("UC: Content Publishing (Value-RFile)");
    __PRINTS("*** UC: Done - Content Publishing (Value-RFile) ***");

    if( !error && retval )
        {
        error = retval;
        }
    
    return error;
    }

TInt CContentRenderer::Clean( MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
            
    __PRINTS("*** UC: Init - Content Publishing (Clean) ***");
    __TIME("UC: Content Publishing (Clean)",
    	TRAP( error, retval = DoCleanL( aPlugin, aContent, aIndex ) );
    );
    __HEAP("UC: Content Publishing (Clean)");
    __PRINTS("*** UC: Done - Content Publishing (Clean) ***");

    if( !error && retval )
        {
        error = retval;
        }
    
    return error;
    }

TAny* CContentRenderer::Extension( TUid /*aUid*/ )
    {
    // No extensions supported
    return NULL;
    }

TBool CContentRenderer::RequiresSubscription( 
    const TAiPublisherInfo& aPublisherInfo ) const
    {
    if ( aPublisherInfo.iNamespace == KNativeUiNamespace )
        {
        // Not targeted to this content renderer
        return EFalse;
        }
        
    return ETrue;
    }

TInt CContentRenderer::SetProperty( MAiPropertyExtension& aPlugin,
        const TDesC8& aElementId,
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue ) 
    {
   return ( SetProperty( aPlugin, aElementId, 
               aPropertyName, aPropertyValue,
               MAiContentObserver::EValueString ) );
    }

TInt CContentRenderer::SetProperty( MAiPropertyExtension& aPlugin,
        const TDesC8& aElementId,
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue,
        MAiContentObserver::TValueType aValueType) 
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );

    TRAP( error, retval = SetPropertyL( aPlugin, aElementId, aPropertyName,
            aPropertyValue, aValueType ) );
    
    if( !error && retval )
     {
     error = retval;
     }
    
    return error;
    }

TInt CContentRenderer::SetPropertyL( MAiPropertyExtension& aPlugin,
        const TDesC8& aElementId,
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue,
        MAiContentObserver::TValueType aValueType) 
    {
    TInt err = KErrNone;
    // Find node
    CXnNodeAppIf* targetNode = FindNodeByIdL( aElementId, aPlugin.PublisherInfoL()->iNamespace );
    if ( targetNode )
       {
       CXnDomStringPool& sp( targetNode->UiEngineL()->StringPool() );
              
       CXnProperty* prop = CXnProperty::NewL( 
               aPropertyName,
               aPropertyValue,
               DomPropertyValueType(aValueType), sp );
                      
       CleanupStack::PushL( prop );
    
       targetNode->SetPropertyL( prop );
      
       CleanupStack::Pop( prop );
       }
    else
       {
       err = KErrNotFound;
       }
    return err;
    }


CXnDomPropertyValue::TPrimitiveValueType 
    CContentRenderer::DomPropertyValueType(
            MAiContentObserver::TValueType aValueType)
    {
    CXnDomPropertyValue::TPrimitiveValueType type = 
            CXnDomPropertyValue::EUnknown;
    switch ( aValueType )
        {
        case MAiContentObserver::EValueNumber :
            {
            type = CXnDomPropertyValue::ENumber;
            }
            break;
        case MAiContentObserver::EValuePercentage :
            {
            type = CXnDomPropertyValue::EPercentage;
            }
            break;
        case MAiContentObserver::EValuePx :
            {
            type = CXnDomPropertyValue::EPx;
            }
            break;
        case MAiContentObserver::EValueString :
            {
            type = CXnDomPropertyValue::EString;
            }
            break;
        case MAiContentObserver::EValueRgbColor :
            {
            type = CXnDomPropertyValue::ERgbColor;
            }
            break;
        case MAiContentObserver::EValueUnitValue :
            {
            type = CXnDomPropertyValue::EUnitValue;
            }
            break;
        default:
            {
            type = CXnDomPropertyValue::EUnknown;
            }
        }
    return type;
    }

void CContentRenderer::DoStartTransactionL( TInt aTxId )
    {
    MTransaction* tr = iFactory->CreateTransactionL( aTxId );
    iStack->Push(tr);
    SetImmediateMode(EFalse);
    }

TInt CContentRenderer::CanPublishL( MAiPropertyExtension& aPlugin,
                                    TInt aContent,
                                    TInt aIndex )
    {
    // Get content item for aContent
    MAiContentItemIterator& iter( 
            ContentItemIteratorL( aPlugin, EAiPublisherContent ) );
    
    const TAiContentItem& item( iter.ItemL( aContent ) );

    // Lookup ui element
    const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

    CXnNodeAppIf* property( FindNodeByClassL( nodeId, aIndex, 
                            aPlugin.PublisherInfoL()->iNamespace ) ); 
 
    if( !property )
        {
        return KErrNotFound;
        }
    
    // Check content priority
    TInt priority( GetContentPriority( *property ) );

    CXnNodeAppIf* target( property->ParentL() );
    
    if( !target )
        {
        return KErrNotSupported;
        }

    if ( !AllowPublishByPriority( *target, priority ) )
        {
        return KErrAccessDenied;        
        }

    // Check if content type is supported by target
    const TDesC8& contentType( ContentType( item ) );

    if ( !iFactory->IsSupported( *target, contentType ) &&
          target->Type()->Type() != XnPropertyNames::listquerydialog::KListQueryDialog )
        {
        return KErrNotSupported;
        }
    
    return KErrNone;
    }

TInt CContentRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                   TInt aContent,
                                   TInt aResource,
                                   TInt aIndex )
    {
    TAiPublisherInfo *info = static_cast<TAiPublisherInfo*>(aPlugin.GetPropertyL( EAiPublisherInfo ));
    if (!info)
        {
        return KErrNotFound;
        }
    // Read ref value.
    MAiContentItemIterator& refIter( ContentItemIteratorL( aPlugin, EAiPublisherResources ) );
    const TAiContentItem& ref( refIter.ItemL( aResource ) );

    const TDesC8& refType( ContentType( ref ) );

    // Resolve source node
    const TDesC& nodeId = iNodeIdGenerator->ResourceNodeIdL(aPlugin, ref);
    HBufC8* nodeId8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(nodeId);
    CleanupStack::PushL(nodeId8);
    CXnNodeAppIf* source = NULL;
    __TIME_MARK(xmlOverhead);
    source = FindNodeByIdL( *nodeId8, info->iNamespace );
    __TIME_ENDMARK("XML UI: Lookup node by id", xmlOverhead);
    __PRINT(__DBG_FORMAT("\t[I]\tXML UI: Lookup node by id=%S"), &nodeId);
    
    CleanupStack::PopAndDestroy(nodeId8);
    
    // Fetch content id
    MAiContentItemIterator& iter = ContentItemIteratorL( aPlugin, EAiPublisherContent );
    const TAiContentItem& item = iter.ItemL( aContent );

    const TDesC& targetId = iNodeIdGenerator->ContentNodeIdL(aPlugin, item);

    TInt retval( KErrNotSupported );

    // Check types
    if ( refType == KContentTypeText )
        {
        // Fetch text
        const TDesC8& text = source->GetPCData();
        
        // Delegate to data publishing function
        retval = PublishDataL( aPlugin,
                               targetId,
                               text,
                               refType,
                               aIndex,
                               source );
        }
    else if ( refType.Find( KContentTypeImage ) != KErrNotFound )
        {
        // Fetch icon
        CGulIcon* icon = LoadIconLC( *source ); 
        
       // Delegate to icon publishing function
        retval = PublishIconL( aPlugin,
                               targetId,
                               icon,
                               aIndex,
                               source );
        
        CleanupStack::Pop( icon );
        }
    
    return retval;
    }

TInt CContentRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                   TInt aContent,
                                   const TDesC16& aText,
                                   TInt aIndex )
    {
    // Resolve content item
    MAiContentItemIterator& iter( 
            ContentItemIteratorL( aPlugin, EAiPublisherContent ) );
    
    const TAiContentItem& item( iter.ItemL( aContent ) );

    const TDesC8& type( ContentType( item ) );

    if ( type == KContentTypeText )
        {
        // Find ui element
        const TDesC& nodeId = iNodeIdGenerator->ContentNodeIdL( aPlugin, item );

        __TIME_MARK(xmlOverhead);

        CXnNodeAppIf* property( FindNodeByClassL( nodeId, aIndex, 
                                                  aPlugin.PublisherInfoL()->iNamespace ) );

        if( !property )
            {
            return KErrNotFound;
            }
        
        TInt priority( GetContentPriority( *property ) );
        
        __TIME_ENDMARK("XML UI: Lookup node by class", xmlOverhead);
    	__PRINT(__DBG_FORMAT("\t[I]\tXML UI: Lookup node by class=%S"), &nodeId);

        //Navigate to parent
        CXnNodeAppIf* target( property->ParentL() );

        if( !target )
            {
            return KErrNotSupported;
            }
        
        // Check priority
        if ( AllowPublishByPriority( *target, priority ) )
            {
            // Check if target is newsticker
            MTransactionElement* element( NULL );
            
            if ( IsParentNewsticker( *target ) )
                {
                // Register callback interface for newsticker
                CXnNodeAppIf *parent( target->ParentL() );
                
                if( !parent )
                    {
                    return KErrNotFound;
                    }
                                
                RegisterNewstickerCallbackInterfaceL( *parent );
                
                const TAiPublisherInfo* info( aPlugin.PublisherInfoL() );
                
                if ( info )
                    {
                    iNTPublisher.Set( info->iName );
                    iNTClass = AiUtility::CopyToBufferL( iNTClass, nodeId );
                    }
                
                element = iFactory->CreateNewsTickerTransactionElementL( *target,
                                                                    aText,
                                                                    priority, 
                                                                    aIndex );
                }
            else if( target->Type()->Type() == 
                     XnListQueryDialogInterface::MXnListQueryDialogInterface::Type())
                {
                // Get the data interface for dialog and publish data
                XnListQueryDialogInterface::MXnListQueryDialogInterface* listQuery( NULL );
                XnComponentInterface::MakeInterfaceL( listQuery, *target );
                LeaveIfNull( listQuery, KErrNotSupported );
                listQuery->ReplaceItemL( aText, aIndex -1 );// plugins publish ordinals not indexes
                return KErrNone;
                }
            else
                {
                // Create transaction element for text
                // Not put to cleanupstack, because element is from our pool!
                element = iFactory->CreateTextTransactionElementL( *target,
                                                                    aText,
                                                                    priority );
                }
            
            iPolicyEvaluator->EvaluateContentPolicyL( *target,
                                                      element->PolicyArray() );
            iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                         element->PolicyArray() );

            ProcessTransactionElementL( element );
            }
        else
            {
            return KErrAccessDenied;            
            }
        }
    else
        {
        return KErrNotSupported;
        }
    
    return KErrNone;
    }

TInt CContentRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                   TInt aContent,
                                   const TDesC8& aBuf,
                                   TInt aIndex )
    {
    // resolve content item
    MAiContentItemIterator& iter( 
            ContentItemIteratorL( aPlugin, EAiPublisherContent ) );
    
    const TAiContentItem& item( iter.ItemL( aContent ) );

    const TDesC8& type( ContentType( item ) );

    TInt retval( KErrNotSupported );
    
    if( type == KContentTypeBitmap )
        {
        // Unpack icon from pointer
        CGulIcon* icon( LeaveIfNull( UnpackPtr<CGulIcon>( aBuf ), KErrArgument ) );

        const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );
        
        // Publish icon
        retval = PublishIconL( aPlugin, nodeId, icon, aIndex );
        }
    else if ( type == KContentTypeImageSvg )
        {
        // Get node id
        const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );
        
        // Publish SVG data
        retval = PublishDataL( aPlugin, nodeId, aBuf, KContentTypeImageSvg, aIndex );
        }

    return retval;
    }

TInt CContentRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                   TInt aContent,
                                   RFile& aFile,
                                   TInt aIndex)
    {
    //Resolve content item
    MAiContentItemIterator& iter( 
            ContentItemIteratorL( aPlugin, EAiPublisherContent ) );
    
    const TAiContentItem& item( iter.ItemL( aContent ) );

    const TDesC8& type( ContentType( item ) );

    // Image support
    if ( type.Find( KContentTypeImage ) != KErrNotFound )
        {
        // Find ui element
        const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

        CXnNodeAppIf* property( FindNodeByClassL( nodeId, aIndex, 
                                aPlugin.PublisherInfoL()->iNamespace ) );
        
        if( !property )
            {
            return KErrNotFound;
            }
        
        // Check priority
        TInt priority( GetContentPriority( *property ) );
        
        CXnNodeAppIf* target( property->ParentL() );
        
        if( !target )
            {
            return KErrNotSupported;
            }

        if ( AllowPublishByPriority( *target, priority ) )
            {
            // Check if target is newsticker
            if ( IsParentNewsticker( *target ) )
                {
                // Register callback interface
                CXnNodeAppIf *parent( target->ParentL() );
                
                if( !parent )
                    {
                    return KErrNotFound;
                    }
                                
                RegisterNewstickerCallbackInterfaceL( *parent );
                
                const TAiPublisherInfo* info( aPlugin.PublisherInfoL() );
                
                if( info )
                    {
                    iNTPublisher.Set( info->iName );
                    iNTClass = AiUtility::CopyToBufferL( iNTClass, nodeId );
                    }
                }
            
            // Create transaction element for file
            MTransactionElement* element =
                iFactory->CreateImageTransactionElementL( *target,
                                                          aFile,
                                                          priority );
            
            iPolicyEvaluator->EvaluateContentPolicyL( *target,
                                                      element->PolicyArray() );
            iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                         element->PolicyArray() );
            
            ProcessTransactionElementL( element );
            }
        else
            {
            return KErrAccessDenied;
            }
        }
    else 
        {
        return KErrNotSupported;        
        }
    
    return KErrNone;
    }

TInt CContentRenderer::DoCleanL( MAiPropertyExtension& aPlugin,
                                 TInt aContent,
                                 TInt aIndex )
    {
    // Resolve content item
    MAiContentItemIterator& iter( 
            ContentItemIteratorL( aPlugin, EAiPublisherContent ) );
    
    const TAiContentItem& item( iter.ItemL( aContent ) );

    const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

    CXnNodeAppIf* property( FindNodeByClassL( nodeId, aIndex, 
                            aPlugin.PublisherInfoL()->iNamespace ) );

    if( !property )
        {
        return KErrNotFound;
        }
    
    TInt priority( GetContentPriority( *property ) );

    // Navigate to parent
    CXnNodeAppIf* target( property->ParentL() );

    if( !target )
        {
        return KErrNotSupported;
        }
    
    if ( !AllowPublishByPriority( *target, priority ) )
        {
        return KErrAccessDenied;
        }

    if ( IsParentNewsticker( *target ) )
        {
        CXnNodeAppIf *parent( target->ParentL() );
        
        if( !parent )
            {
            return KErrNotFound;
            }
               
        RegisterNewstickerCallbackInterfaceL( *parent );
        
        const TAiPublisherInfo* info( aPlugin.PublisherInfoL() );
        
        if( info )
            {
            iNTPublisher.Set( info->iName );
            iNTClass = AiUtility::CopyToBufferL( iNTClass, nodeId );
            }
        }
    if( target->Type()->Type() ==
        XnListQueryDialogInterface::MXnListQueryDialogInterface::Type())
        {
        // Get the data interface for dialog and delete data
        XnListQueryDialogInterface::MXnListQueryDialogInterface* listQuery( NULL );
        XnComponentInterface::MakeInterfaceL( listQuery, *target );
        LeaveIfNull( listQuery, KErrNotSupported );
        listQuery->DeleteItem( aIndex -1 );// plugins publish ordinals not indexes
        return KErrNone;
        }
    
    // Create transaction element for empty content
    MTransactionElement* element =
        iFactory->CreateEmptyContentTransactionElementL( *target, aIndex );

    iPolicyEvaluator->EvaluateEmptyContentPolicyL( *target,
                                                   element->PolicyArray() );
    iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                   element->PolicyArray() );

    ProcessTransactionElementL( element );
      
    if ( priority > KErrNotFound ) // Only for prioritized elements
        {
        // Add current ui element into content refresh map
        HBufC* uiElementId = PropertyValueL( *target,
                                         XnPropertyNames::common::KId );
        return RefreshContentL( uiElementId, priority );
        }
    
    return KErrNone;
    }

void CContentRenderer::SetImmediateMode( TBool aImmediateMode )
    {
    iImmediateMode = aImmediateMode;
    }

TBool CContentRenderer::IsImmediateMode() const
    {
    return iImmediateMode;
    }

void CContentRenderer::ProcessTransactionElementL( MTransactionElement* aElement )
    {
    LeaveIfNull( aElement, KErrArgument );
    
    if ( IsImmediateMode() || iStack->IsEmpty() )
        {
        // No transaction. Commit element immediately
        TBool layoutChanged = EFalse;
        
        RPropertyHashMap propertyHashMap;
        aElement->CommitL(layoutChanged, propertyHashMap);
        SetPropertyArraysL( propertyHashMap );
        propertyHashMap.Close();
        
        iFactory->ReleaseTransactionElement( aElement );
        StartContentRefresh();
        
        // Re-layout
        iAppUi.UiEngineL()->RenderUIL();
        }
    else
        {
        // Append transaction element to transaction
        MTransaction* tr = iStack->Top();
        tr->Append( *aElement );
        }
    }

CXnNodeAppIf* CContentRenderer::FindNodeByClassL( const TDesC& aCid,
                                                  TInt aIndex,
                                                  const TDesC8& aNs )
    {
    // Find node
    HBufC8* classId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aCid);
    CleanupStack::PushL(classId);
    
    RPointerArray<CXnNodeAppIf> nodes = iAppUi.UiEngineL()->FindNodeByClassL( *classId, aNs );

    CleanupStack::PopAndDestroy(classId);

    CleanupClosePushL( nodes );

    for ( TInt i = 0; i < nodes.Count(); ++i )
        {
        CXnNodeAppIf* node = nodes[i];

        const TDesC8* name =
            PropertyValue( *node, AiUiDef::xml::property::KName );

        if ( name && ( *name == AiUiDef::xml::name::KOrdinal ) )
            {
            const TDesC8* value =
                PropertyValue( *node, AiUiDef::xml::property::KValue );

            if ( value )
                {
                // Try to parse index from string either

                TInt32 index( 0 );
                User::LeaveIfError( AiUtility::ParseInt( index, *value ) );

                if ( index == aIndex )
                    {
                    CleanupStack::PopAndDestroy(&nodes);
                    return node;
                    }
                }
            }
        else if ( name && ( *name == AiUiDef::xml::name::KTarget ) )
            {            
            const TDesC8* target =
                           PropertyValue( *node, AiUiDef::xml::property::KValue );
            
            CXnNodeAppIf* targetNode = FindNodeByIdL( *target, node->Namespace() );
            if ( targetNode )
                {
                CleanupStack::PopAndDestroy( &nodes );
                return targetNode;
                }
            }
        else if ( nodes.Count() == 1 ) // Only one node in class
            {
            // No ordinal specified
			node = nodes[ 0 ];
			CleanupStack::PopAndDestroy(&nodes);
            return node;
            }
        }
    
    CleanupStack::PopAndDestroy( &nodes );
    
    return NULL; // Never reached. Needed to omit compiler warning
    }

CXnNodeAppIf* CContentRenderer::FindNodeByIdL( const TDesC& aCid, const TDesC& aNs )
    {
    // Find node
    return LeaveIfNull( iAppUi.UiEngineL()->FindNodeByIdL( aCid, aNs ),
                        KErrNotFound );
    }

CXnNodeAppIf* CContentRenderer::FindNodeByIdL( const TDesC8& aCid, const TDesC8& aNs )
    {
    // Find node
    return LeaveIfNull( iAppUi.UiEngineL()->FindNodeByIdL( aCid, aNs ),
                        KErrNotFound );
    }

TInt CContentRenderer::PublishIconL( MAiPropertyExtension& aPlugin,
                                     const TDesC& aCid,
                                     CGulIcon* aIcon,
                                     TInt aIndex,
                                     CXnNodeAppIf* aResource )
    {
    // Find proiperty element by class
    CXnNodeAppIf* property( FindNodeByClassL( aCid, aIndex, 
                            aPlugin.PublisherInfoL()->iNamespace ) );
    
    if( !property )
        {
        return KErrNotFound;
        }
    
    // Get priority information
    TInt priority( GetContentPriority( *property ) );
    
    // Navigate to parent
    CXnNodeAppIf* target( property->ParentL() );
    
    if( !target )
        {
        return KErrNotFound;
        }

    // Check priority
    if ( !AllowPublishByPriority( *target, priority ) )
        {
        return KErrAccessDenied;        
        }
    
    // Special handling of newsticker
    if ( IsParentNewsticker( *target ) )
        {
        // Register callback interface
        CXnNodeAppIf *parent( target->ParentL() );
        
        if( !parent )
            {
            return KErrNotFound;
            }
                
        RegisterNewstickerCallbackInterfaceL( *parent );
        
        const TAiPublisherInfo* info( aPlugin.PublisherInfoL() );
        
        if( info )
            {
            iNTPublisher.Set( info->iName );
            iNTClass = AiUtility::CopyToBufferL( iNTClass, aCid );
            }
        }

    MTransactionElement* element =
        iFactory->CreateImageTransactionElementL( *target,
                                                  aIcon,
                                                  priority );

    if ( aResource )
        {
        iPolicyEvaluator->EvaluateResourcePolicyL( *target,
                                                   *aResource,
                                                   element->PolicyArray() );
        iPolicyEvaluator->EvaluateContentPolicyL( *target,
                                                  element->PolicyArray() );
        iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                     element->PolicyArray() );
        }

    else
        {
        iPolicyEvaluator->EvaluateContentPolicyL( *target,
                                                  element->PolicyArray() );
        iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                     element->PolicyArray() );
        }

    ProcessTransactionElementL( element );
    
    return KErrNone;
    }

TInt CContentRenderer::PublishDataL( MAiPropertyExtension& aPlugin,
                                     const TDesC& aCid,
                                     const TDesC8& aData,
                                     const TDesC8& aContentType,
                                     TInt aIndex,
                                     CXnNodeAppIf* aResource )
    {
    CXnNodeAppIf* property( FindNodeByClassL( aCid, aIndex, 
                            aPlugin.PublisherInfoL()->iNamespace ) );

    if( !property )
        {
        return KErrNotFound;
        }
    
    TInt priority( GetContentPriority( *property ) );
    
    // Navigate to parent
    CXnNodeAppIf* target( property->ParentL() );
    
    if( !target )
        {
        return KErrNotFound;
        }

    if ( !AllowPublishByPriority( *target, priority ) )
        {
        return KErrAccessDenied;        
        }

    if ( !CDataBufferTransactionElement::IsSupported( *target, aContentType ) )
        {
        return KErrNotSupported;        
        }
        
    // Handle newsticker 
    if ( IsParentNewsticker( *target ) )
        {
        CXnNodeAppIf *parent( target->ParentL() );
        
        if( !parent )
            {
            return KErrNotFound;
            }
                
        RegisterNewstickerCallbackInterfaceL( *parent );
        
        const TAiPublisherInfo* info( aPlugin.PublisherInfoL() );
        
        if( info )
            {
            iNTPublisher.Set( info->iName );
            iNTClass = AiUtility::CopyToBufferL( iNTClass, aCid );
            }
        }
        
    MTransactionElement* element =
        iFactory->CreateDataBufferTransactionElementL( *target,
                                                       aData,
                                                       priority );
    
    if ( aResource )
        {
        iPolicyEvaluator->EvaluateResourcePolicyL( *target,
                                                   *aResource,
                                                   element->PolicyArray() );
        iPolicyEvaluator->EvaluateContentPolicyL( *target,
                                                  element->PolicyArray() );
        iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                     element->PolicyArray() );
        }
    else
        {
        iPolicyEvaluator->EvaluateContentPolicyL( *target,
                                                  element->PolicyArray() );
        iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                     element->PolicyArray() );
        }

    ProcessTransactionElementL( element );
    
    return KErrNone;
    }

TBool CContentRenderer::AllowPublishByPriority( CXnNodeAppIf& aUiElement,
                                                TInt aPriority ) const
    {
    // Get ui element id
    const TDesC8* uiElementId = PropertyValue( aUiElement,
                                               XnPropertyNames::common::KId );
                                         
    if ( uiElementId )
        {
        // compare given priority with the current value of ui element
        return iContentPriorityMap->OverrideContent( *uiElementId, aPriority );
        }
    
    return EFalse; // priority cannot be used, because ui element does not have id
    }

void CContentRenderer::StartContentRefresh()
    {
    // Cancel ongoing refresh
    iTimer->Cancel();
    
    if ( iRefreshableUiElements.Count() > 0 )
        {
        // Refreshable elements exist. Start timer to make refresh asynchronous 
        iTimer->Start( TTimeIntervalMicroSeconds32( 0 ),
                       TTimeIntervalMicroSeconds32( 0 ),
                       TCallBack( RefreshContentCallback, this ) );
        }
    }

/**
 * Adds ui element to list of refreshable elements
 *
 * @param aUiElementId ui element id
 * @param aOldPriority old priority value
 */                         
TInt CContentRenderer::RefreshContentL( HBufC* aUiElementId,
                                        TInt aOldPriority )
    {
    if( !aUiElementId )
        {
        return KErrArgument;
        }
        
    // Take ownership of aUiElementId
    CleanupStack::PushL( aUiElementId );
    
    if ( !iFwEventHandler )
        {
        // Content refresh event cannot be sent
        CleanupStack::PopAndDestroy( aUiElementId );
        return KErrNotReady;
        }
    
    // Find current mapping
    TInt* oldPriority = iRefreshableUiElements.Find( *aUiElementId );
    
    if ( oldPriority )
        {
        // Update mapping
        *oldPriority = aOldPriority;
        CleanupStack::PopAndDestroy( aUiElementId );
        }
    else
        {
        // Create new mapping
        oldPriority = new( ELeave ) TInt( aOldPriority );
        CleanupStack::PushL( oldPriority );
    
        User::LeaveIfError( iRefreshableUiElements.Insert( aUiElementId,
                                                           oldPriority ) );
                                                       
        CleanupStack::Pop( 2, aUiElementId );
        }
    
    return KErrNone;
    }

/**
 * Callback function to make content refreshing asynchronous
 */    
TInt CContentRenderer::RefreshContentCallback( TAny* aContentRenderer )
    {
    if ( !aContentRenderer )
        {
        return KErrArgument;
        }
        
    CContentRenderer* renderer = static_cast< CContentRenderer* >( aContentRenderer );
    TRAP_IGNORE( renderer->SendRefreshContentEventL() );
    return KErrNone;
    }

/**
 * Sends Refresh content event to framework.
 * Event is sent for content selectors with lower priority than the
 * last content which has been cleaned from ui element.
 */ 
void CContentRenderer::SendRefreshContentEventL()
    {
    // Cancel periodic timer.
    iTimer->Cancel();
    
    // Get ui element id and old content priority
    TPtrHashMapIter< TDesC, TInt> iter( iRefreshableUiElements );
    iter.Reset();
    
    const TDesC* uiElementId = iter.NextKey(); // Never NULL
    TInt priority = *( iter.CurrentValue() );
    
    //  Cleanup item for iterator 
    TMapCleanupItem cleanup( iter );
    CleanupReleasePushL( cleanup );
    
    // Lookup ui element
    CXnNodeAppIf* uiElement = FindNodeByIdL( *uiElementId );
    
    // Remove current ui element from the map
    CleanupStack::PopAndDestroy( &cleanup );
    
    // Find lower priority content elements associated to this ui element
    RPointerArray< CXnNodeAppIf > children = uiElement->ChildrenL();
    
    // Remove higher priority content elements
    RemoveNonPriorityElements( children, priority );
        
    // Sort array to descending order
    children.Sort( TLinearOrder< CXnNodeAppIf >( DescendingPriorityOrder ) );
    
    // Send event for content selectors in descending priority order.
    // Continue until first content gets published or array exhausts.
    for ( TInt i = 0; i < children.Count(); ++i )
        {
        CXnNodeAppIf* current = children[ i ];
        
        // Get content selector
        const HBufC* contentSelector = PropertyValueL( *current,
                                                       XnPropertyNames::common::KClass );
        
        if ( contentSelector && iFwEventHandler->RefreshContent( *contentSelector ))
            {
            break;
            }
        }
    
    // Free content selector array
    children.Reset();
    
    // Continue content refresh for next ui element.
    StartContentRefresh();    
    }

void CContentRenderer::TitleScrolled( TInt aTitleIndex )
    {
    if ( iCallbackHandler )
        {
        TRAP_IGNORE( iCallbackHandler->TitleScrolledL( iNTPublisher,
                                                       *iNTClass,
                                                       aTitleIndex ) );
        }
    }
    
void CContentRenderer::TitleToScroll( TInt aTitleIndex )
    {
    if ( iCallbackHandler )
        {
        TRAP_IGNORE( iCallbackHandler->TitleToScrollL( iNTPublisher,
                                                       *iNTClass,
                                                       aTitleIndex ) );        
        }
    }

TBool CContentRenderer::IsParentNewsticker( CXnNodeAppIf& aTarget )
    {
    CXnNodeAppIf* parent = NULL;
    TRAP_IGNORE( parent = aTarget.ParentL() );
    if ( !parent )
        {
        return EFalse;
        }
    CXnType* typeInfo = parent->Type();
    
    if ( !typeInfo )
        {
        return EFalse;
        }
        
    return ( typeInfo->Type() == XnNewstickerInterface::MXnNewstickerInterface::Type());
    }
    
void CContentRenderer::RegisterNewstickerCallbackInterfaceL( CXnNodeAppIf& aTarget )
    {
    if ( !iCallbackHandler )
        {
        // Instantiate callback handler
        CNewstickerCallbackHandler* handler = CNewstickerCallbackHandler::NewLC( *iFwEventHandler );

        // Set callback handler
        iCallbackHandler = handler;
        CleanupStack::Pop( handler );
        }
        // Obtain newsticker component interface
        XnNewstickerInterface::MXnNewstickerInterface* newsticker = NULL;
        XnComponentInterface::MakeInterfaceL( newsticker, aTarget );
        
        LeaveIfNull( newsticker, KErrGeneral );
        
        // Set callback interface
        newsticker->SetCallbackInterfaceL( this );
    }

void CContentRenderer::CleanPluginFromUiL( MAiPropertyExtension& aPlugin )
    {
    TInt itemCount = 0;
    
    // Resolve content items
    MAiContentItemIterator& iter = ContentItemIteratorL( aPlugin, EAiPublisherContent );
    iter.Reset();
    
    // Clean all content items
    while ( iter.HasNext() )
        {
        const TAiContentItem& item = iter.NextL();

        const TDesC& nodeId = iNodeIdGenerator->ContentNodeIdL( aPlugin, item );
        
        // Find nodes
        HBufC8* nodeId8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(nodeId);
        CleanupStack::PushL(nodeId8);

        RPointerArray<CXnNodeAppIf> nodes = iAppUi.UiEngineL()->FindNodeByClassL( 
            *nodeId8, aPlugin.PublisherInfoL()->iNamespace );

        CleanupStack::PopAndDestroy(nodeId8);

        itemCount += nodes.Count(); 
        
        CleanupClosePushL( nodes );

        for ( TInt i = 0; i < nodes.Count(); ++i )
            {
            CXnNodeAppIf* property = nodes[i];
            
            TInt priority = GetContentPriority( *property );

            // Navigate to parent
            CXnNodeAppIf* target = LeaveIfNull( property->ParentL(), KErrNotFound );

            if ( AllowPublishByPriority( *target, priority ) )
                {
                // Create transaction element for empty content
                MTransactionElement* element =
                    iFactory->CreateEmptyContentTransactionElementL( *target, 0 );

                iPolicyEvaluator->EvaluateEmptyContentPolicyL( *target,
                                                               element->PolicyArray() );
                iPolicyEvaluator->EvaluateVisibilityPolicyL( *target,
                                                               element->PolicyArray() );
                                                      
                ProcessTransactionElementL( element );
                  
                if ( priority > KErrNotFound ) // Only for prioritized elements
                    {
                    // Add current ui element into content refresh map
                    HBufC* uiElementId = PropertyValueL( *target,
                                                         XnPropertyNames::common::KId );
                    RefreshContentL( uiElementId, priority );
                    }
                }
            }
            
        CleanupStack::PopAndDestroy(&nodes);
        }
        
    if ( itemCount == 0 )
        {
        //Nothing to clean from UI. Cancel transaction outside current trap harness
        User::Leave( KErrNotFound );
        }
    }

//  End of File
