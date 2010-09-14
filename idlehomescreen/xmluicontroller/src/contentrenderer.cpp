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
#include <e32hashtab.h>
#include <utf.h>
#include <gulicon.h>

// User includes
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
#include "contentrenderer.h"
#include "appui.h"
#include "activetransactionstack.h"
#include "transactionfactoryimpl.h"
#include "mtransaction.h"
#include "mtransactionelement.h"
#include "aixmluiutils.h"
#include "xmluicontroller.h"
#include "xmlnodeidgenerator.h"
#include "aixmluiconstants.h"
#include "aifweventhandler.h"
#include "databuffertransactionelement.h"
#include "newstickertransactionelement.h"
#include "csspropertymap.h"
#include "policyevaluator.h"
#include "debug.h"
#include "xnuiengineappif.h"
#include "xnnodeappif.h"
#include "xnproperty.h"
#include "mxncomponentinterface.h"
#include "xntext.h"
#include "xnbitmap.h"
#include "xntype.h"
#include "xnmenuadapter.h"
#include "xnlistquerydialogadapter.h"
#include "xnnewsticker.h"
#include "mxncomponentinterface.h"
#include "aistrcnv.h"
#include "contentprioritymap.h"
#include "ainativeuiplugins.h"

using namespace AiXmlUiController;
using namespace AiUiDef::xml;
using namespace XnTextInterface;
using namespace XnImageInterface;

/**
 * Cleanup item for cleanup of TPtrHashMapIter
 */            
class TMapCleanupItem
    {
public:
    /**
     * C++ consturctor
     */
    TMapCleanupItem( TPtrHashMapIter< TDesC, TInt >& aIterator );
    
    /**
     * Removes the pointers in the map and deletes the objects
     * referenced by the pointers.
     */ 
    void Release();
            
private:        
    TPtrHashMapIter< TDesC, TInt > iIterator;    
    };

// ----------------------------------------------------------------------------
// TMapCleanupItem::TMapCleanupItem
//  
// ----------------------------------------------------------------------------
//
TMapCleanupItem::TMapCleanupItem( TPtrHashMapIter< TDesC, TInt >& aIterator )
    : iIterator( aIterator )
    {
    }

// ----------------------------------------------------------------------------
// TMapCleanupItem::Release
//  
// ----------------------------------------------------------------------------
//
void TMapCleanupItem::Release()
    {
    // Delete current key and value
    const TDesC* key( iIterator.CurrentKey() );
    const TInt* value( iIterator.CurrentValue() );
    
    delete key;
    delete value;
    
    // Remove mapping from the map.
    iIterator.RemoveCurrent();
    }

// ============================ LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// ContentItemIterator
// Gets content item iterator
// ----------------------------------------------------------------------------
//
static MAiContentItemIterator* ContentItemIterator( CHsContentPublisher& aPlugin,
    CHsContentPublisher::TProperty aType )
    {       
    return static_cast< 
        MAiContentItemIterator* >( aPlugin.GetProperty( aType ) );                                     
    }

// ----------------------------------------------------------------------------
// ContentPriority
// Gets the content priority associated in the property element
// ----------------------------------------------------------------------------
//
static TInt ContentPriority( CXnNodeAppIf& aPropertyElement )
    {
    TInt32 priority( KErrNotFound );
    
    const TDesC8* name( 
        PropertyValue( aPropertyElement, property::KName ) );
                                                                                   
    if ( name && *name == name::KPriority )
        {
        const TDesC8* value( 
            PropertyValue( aPropertyElement, property::KValue ) );
                                             
        if ( value )
            {
            AiUtility::ParseInt( priority, *value );
            }
        }
        
    return priority;
    }
    
// ----------------------------------------------------------------------------
// RemoveNonPriorityElements
// Remove elements from array which do not contain priority or priority is 
// not lower than the given value.
// ----------------------------------------------------------------------------
//
static void RemoveNonPriorityElements( RPointerArray< CXnNodeAppIf>& aElements,
    TInt aLastPriority )
    {
    //  Remove non priority elements and higher priority elements
    TInt elementCount( aElements.Count() );    
    
    for ( TInt i = 0; i < elementCount; )
        {
        CXnNodeAppIf* current( aElements[i] );
        
        // Check name attribute
        const TDesC8* name( PropertyValue( *current, property::KName ) );
        
        if ( !name || *name != name::KPriority )
            {
            // Remove current
            aElements.Remove( i );
            --elementCount;
            }
        else
            {
            // Check current priority
            const TDesC8* value( 
                PropertyValue( *current, property::KValue ) );
            
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
    
// ----------------------------------------------------------------------------
// DescendingPriorityOrder
// Descending priority order for prioritized content selectors. 
// ----------------------------------------------------------------------------
//
static TInt DescendingPriorityOrder( const CXnNodeAppIf& aNode1,
    const CXnNodeAppIf& aNode2 )
    {
    /*
    * @param aNode1 First node to compare
    * @param aNode2 Second node to compare
    * @return 0 nodes have equal priority
    * @return >0 aNode1 has lower priority
    * @return <0 aNode2 has lower priority
    */
    // Array content has been validated, so no checks are needed
    const TDesC8* value1( 
        PropertyValue( aNode1, property::KValue ) );
                                              
    const TDesC8* value2( 
        PropertyValue( aNode2, property::KValue ) );
                                              
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

// ----------------------------------------------------------------------------
// RemoveDuplicateContentChangesL
// Removes duplicate entries in content change array 
// ----------------------------------------------------------------------------
//
static void RemoveDuplicateContentChangesL( RAiPolicyElementArray& aArray )
    {
    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        HBufC* id( PropertyValueL( aArray[i].Target(),  
            AiUiDef::xml::property::KId ) ); 
                                        
        if ( id )
            {
            CleanupStack::PushL( id );
            
            for ( TInt j = i; j < aArray.Count(); ++j )
                {
                HBufC* id2( PropertyValueL( aArray[j].Target(), 
                   AiUiDef::xml::property::KId ) );
                CleanupStack::PushL( id2 );
                
                if ( id2 )
                    {
                    // Same id and same policy
                    if ( i != j && id->Compare( *id2 ) == 0 &&                          
                     ( aArray[i].Policy().Compare( aArray[j].Policy()) == 0 ) ) 
                            
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

// ----------------------------------------------------------------------------
// CleanupReleaseMapItem
// Helper to handle cleanup of map iterator 
// ----------------------------------------------------------------------------
//
static void CleanupReleaseMapItem( TAny* aMapCleanupItem )
    {
    if ( aMapCleanupItem )
        {
        static_cast< TMapCleanupItem* >( aMapCleanupItem )->Release();
        }
    }

// ----------------------------------------------------------------------------
// CleanupReleasePushL
// Helper to push map iterator into cleanup stack. 
// ----------------------------------------------------------------------------
//
static void CleanupReleasePushL( TMapCleanupItem& aCleanupItem )
    {
    CleanupStack::PushL( TCleanupItem( CleanupReleaseMapItem, &aCleanupItem ) );
    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CContentRenderer::CContentRenderer
// 
// ----------------------------------------------------------------------------
//
CContentRenderer::CContentRenderer( CAppUi& aAppUi )
    : iAppUi( aAppUi )
    {
    }

// ----------------------------------------------------------------------------
// CContentRenderer::ConstructL
// 
// ----------------------------------------------------------------------------
//
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

// ----------------------------------------------------------------------------
// CContentRenderer::NewL
// 
// ----------------------------------------------------------------------------
//
CContentRenderer* CContentRenderer::NewL( CAppUi& aAppUi )
    {
    CContentRenderer* self = new( ELeave ) CContentRenderer( aAppUi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::~CContentRenderer
// 
// ----------------------------------------------------------------------------
//
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
    
    delete iPropertyMap;
    
    delete iContentPriorityMap;    
    }

// ----------------------------------------------------------------------------
// CContentRenderer::SetEventHandler
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::SetEventHandler( MAiFwEventHandler& aFwEventHandler )
    {
    iFwEventHandler = &aFwEventHandler;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::StartTransaction
// 
// ----------------------------------------------------------------------------
//
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

// ----------------------------------------------------------------------------
// CContentRenderer::Commit
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::Commit( TInt aTxId )
    {
    // Remove transaction with aTxId from stack
    MTransaction* tr( iStack->Remove( aTxId ) );

    if ( tr )
        {
        // Commit transaction        
        RPropertyHashMap propertyHashMap;
        
        TBool layoutChanged( EFalse );
        
        TRAPD( error, tr->CommitL( iFactory->PolicyArray(), propertyHashMap ) );

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
        iFactory->ResetPolicyArray();
        
        return error;
        }
        
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::ProcessContentChangesL
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::ProcessContentChangesL( MTransaction& aTr )
    {
    TDblQueIter<CTransactionElement> iter( aTr.ElementIter() );
    RAiPolicyElementArray contentChangedArray;
    CleanupClosePushL( contentChangedArray );

    while ( iter )
          {
          CTransactionElement* element = iter++;
          CXnNodeAppIf& target = element->Target();
          
          // Find own and parents policy/ContentChanged nodes 
          iPolicyEvaluator->EvaluateContentChangedPolicyL( target, 
                  contentChangedArray);

          iPolicyEvaluator->EvaluateContentChangedPolicyL( *(target.ParentL() ), 
                  contentChangedArray);
          }
       
    ::RemoveDuplicateContentChangesL( contentChangedArray );
        
    for ( TInt i = 0; i < contentChangedArray.Count(); ++i )
        {
        ProcessContentChangeL( contentChangedArray[i] );
        }
    
    CleanupStack::PopAndDestroy();
    }

// ----------------------------------------------------------------------------
// CContentRenderer::ProcessContentChangeL
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::ProcessContentChangeL( TAiPolicyElement& aElement )
    {
    const TDesC8* id( PropertyValue( 
        aElement.Target(), AiUiDef::xml::property::KId ) ); 
                                
    if ( id )
        {
        CXnNodeAppIf* targetNode( 
            FindNodeByIdL( *id, aElement.Target().Namespace() ) );
        
        if ( targetNode )
            {                     
            if ( aElement.Policy() == AiUiDef::xml::value::KShowTooltips )
                {
                targetNode->ShowTooltipsL();
                }                   
            }        
        }
    }

// ----------------------------------------------------------------------------
// CContentRenderer::CancelTransaction
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::CancelTransaction( TInt aTxId )
    {
    // Remove transaction with aTxId from stack
    MTransaction* tr( iStack->Remove( aTxId ) );

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

// ----------------------------------------------------------------------------
// CContentRenderer::CanPublish
// 
// ----------------------------------------------------------------------------
//
TBool CContentRenderer::CanPublish( CHsContentPublisher& aPlugin,
    TInt aContent, TInt aIndex )                                   
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
        
    __PRINTS("*** XML UI: CContentRenderer::CanPublish ***");
    
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x" ),          
        &aPlugin.PublisherInfo().Name(), aPlugin.PublisherInfo().Uid().iUid ); 
                                  
	__TIME("UC: Content Validation",
        TRAP( error, retval = CanPublishL( aPlugin, aContent, aIndex ) ) );
            
	__HEAP("UC: Content Validation");
    
	TBool ret( error == KErrNone && retval == KErrNone );
	
	__PRINT( __DBG_FORMAT("*** XML UI: CContentRenderer::CanPublish - done, CanPublish: %d ***"), ret );
    
    return ret;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::Publish
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::Publish( CHsContentPublisher& aPlugin, TInt aContent, 
    TInt aResource, TInt aIndex )
    {
	TInt error( KErrNone );
	TInt retval( KErrNone );
	
	__PRINTS("*** XML UI: CContentRenderer::Publish (Resource) ***");
	
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x" ),          
        &aPlugin.PublisherInfo().Name(), aPlugin.PublisherInfo().Uid().iUid ); 
		
	__TIME("UC: Content Publishing (Resource)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aResource, aIndex ) ) );
    
    __HEAP("UC: Content Publishing (Resource)");
    
    if( !error && retval )
        {
        error = retval;
        }
    
    __PRINT( __DBG_FORMAT("*** XML UI: CContentRenderer::Publish (Resource) - done, error: %d ***"), error );
            
    return error;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::Publish
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::Publish( CHsContentPublisher& aPlugin, TInt aContent, 
    const TDesC16& aText, TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    
    __PRINTS("*** XML UI: CContentRenderer::Publish (Value-Text) ***");
    
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x" ),          
        &aPlugin.PublisherInfo().Name(), aPlugin.PublisherInfo().Uid().iUid ); 
    
   	__TIME("UC: Content Publishing (Value-Text)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aText, aIndex ) ) );
       	
    __HEAP("UC: Content Publishing (Value-Text)");
        
    if( !error && retval )
        {
        error = retval;
        }

    __PRINT( __DBG_FORMAT("*** XML UI: CContentRenderer::Publish (Value-Text) - done, error: %d ***"), error );
    
    return error;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::Publish
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::Publish( CHsContentPublisher& aPlugin, TInt aContent, 
    const TDesC8& aBuf, TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    
    __PRINTS("*** XML UI: CContentRenderer::Publish (Value-Buf) ***");
    
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x" ),          
        &aPlugin.PublisherInfo().Name(), aPlugin.PublisherInfo().Uid().iUid ); 
    
    __TIME("UC: Content Publishing (Value-Buf)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aBuf, aIndex ) ) );
    
    __HEAP("UC: Content Publishing (Value-Buf)");
       
    if( !error && retval )
        {
        error = retval;
        }
    
    __PRINT( __DBG_FORMAT("*** XML UI: CContentRenderer::Publish (Value-Buf) - done, error: %d ***"), error );
    
    return error;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::Publish
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::Publish( CHsContentPublisher& aPlugin, TInt aContent,                                
    RFile& aFile, TInt aIndex )                                
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
    	
	__PRINTS("*** XML UI: CContentRenderer::Publish (Value-RFile) ***");
	
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x" ),          
        &aPlugin.PublisherInfo().Name(), aPlugin.PublisherInfo().Uid().iUid ); 
	
    __TIME("UC: Content Publishing (Value-RFile)",
    	TRAP( error, retval = DoPublishL( aPlugin, aContent, aFile, aIndex ) ) );

    __HEAP("UC: Content Publishing (Value-RFile)");
        
    if( !error && retval )
        {
        error = retval;
        }
    
    __PRINT( __DBG_FORMAT("*** XML UI: CContentRenderer::Publish (Value-RFile) - done, error: %d ***"), error );
    
    return error;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::Clean
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::Clean( CHsContentPublisher& aPlugin, TInt aContent, 
    TInt aIndex )
    {
    TInt error( KErrNone );
    TInt retval( KErrNone );
            
    __PRINTS("*** XML UI: CContentRenderer::Clean (Clean) ***");
    
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x" ),          
        &aPlugin.PublisherInfo().Name(), aPlugin.PublisherInfo().Uid().iUid ); 
        
    __TIME("UC: Content Publishing (Clean)",
    	TRAP( error, retval = DoCleanL( aPlugin, aContent, aIndex ) ) );
    
    __HEAP("UC: Content Publishing (Clean)");
       
    if( !error && retval )
        {
        error = retval;
        }
    
    __PRINT( __DBG_FORMAT("*** XML UI: CContentRenderer::Clean (Clean) - done, error: %d ***"), error );
    
    return error;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::CContentRenderer
// 
// ----------------------------------------------------------------------------
//
TAny* CContentRenderer::Extension( TUid /*aUid*/ )
    {
    // No extensions supported
    return NULL;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::RequiresSubscription
// 
// ----------------------------------------------------------------------------
//
TBool CContentRenderer::RequiresSubscription( 
    const THsPublisherInfo& aPublisherInfo ) const
    {
    if ( aPublisherInfo.Namespace() == KNativeUiNamespace )
        {
        // Not targeted to this content renderer
        return EFalse;
        }
        
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::DoStartTransactionL
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::DoStartTransactionL( TInt aTxId )
    {
    MTransaction* tr( iFactory->CreateTransactionL( aTxId ) );
    iStack->Push( tr );
    
    SetImmediateMode( EFalse );
    }

// ----------------------------------------------------------------------------
// CContentRenderer::CanPublishL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::CanPublishL( CHsContentPublisher& aPlugin,
    TInt aContent, TInt aIndex )                                    
    {
    // Get content item for aContent
    MAiContentItemIterator* iter( ContentItemIterator(  
        aPlugin, CHsContentPublisher::EPublisherContent ) );
    
    if ( !iter )
        {
        return KErrNotSupported;
        }

    const TAiContentItem& item( iter->ItemL( aContent ) );

    // Lookup ui element
    const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

    CXnNodeAppIf* property( FindNodeByClassL( nodeId, aIndex, 
        aPlugin.PublisherInfo().Namespace() ) ); 
 
    if( !property )
        {
        return KErrNotFound;
        }
    
    // Check content priority
    TInt priority( ContentPriority( *property ) );

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

// ----------------------------------------------------------------------------
// CContentRenderer::DoPublishL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::DoPublishL( CHsContentPublisher& aPlugin, TInt aContent,
    TInt aResource, TInt aIndex )                                  
    {
    TInt retval( KErrNotSupported );
    
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );

    // Read ref value.
    MAiContentItemIterator* resIter( ContentItemIterator( 
        aPlugin, CHsContentPublisher::EPublisherResources ) );
    
    if ( !resIter )
        {
        return retval;
        }
    
    const TAiContentItem& ref( resIter->ItemL( aResource ) );

    const TDesC8& refType( ContentType( ref ) );

    // Resolve source node
    const TDesC& nodeId( iNodeIdGenerator->ResourceNodeIdL( aPlugin, ref ) );
    
    HBufC8* nodeId8( CnvUtfConverter::ConvertFromUnicodeToUtf8L( nodeId ) );
    CleanupStack::PushL( nodeId8 );
    
    CXnNodeAppIf* source( NULL );
    
    __TIME_MARK( xmlOverhead );
    
    source = FindNodeByIdL( *nodeId8, info.Namespace() );
    
    __TIME_ENDMARK("XML UI: Lookup node by id", xmlOverhead);
    __PRINT(__DBG_FORMAT("\t[I]\tXML UI: Lookup node by id=%S"), &nodeId);
    
    CleanupStack::PopAndDestroy( nodeId8 );
    
    // Fetch content id
    MAiContentItemIterator* iter( ContentItemIterator( 
       aPlugin, CHsContentPublisher::EPublisherContent ) );
    
    if ( !iter )
        {
        return retval;
        }
    
    const TAiContentItem& item( iter->ItemL( aContent ) );

    const TDesC& targetId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );
    
    // Check types
    if ( refType == KContentTypeText )
        {
        // Fetch text
        const TDesC8& text( source->GetPCData() );
        
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
        CGulIcon* icon( LoadIconLC( *source ) ); 
        
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

// ----------------------------------------------------------------------------
// CContentRenderer::DoPublishL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::DoPublishL( CHsContentPublisher& aPlugin, TInt aContent,                                  
    const TDesC16& aText, TInt aIndex )                                   
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );

    // Resolve content item
    MAiContentItemIterator* iter( ContentItemIterator( 
        aPlugin, CHsContentPublisher::EPublisherContent ) ); 
            
    if ( !iter )
        {
        return KErrNotSupported;
        }
    
    const TAiContentItem& item( iter->ItemL( aContent ) );

    const TDesC8& type( ContentType( item ) );

    if ( type == KContentTypeText )
        {
        // Find ui element
        const TDesC& nodeId( 
            iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) ); 
            
        __TIME_MARK( xmlOverhead );

        CXnNodeAppIf* property( FindNodeByClassL(  
            nodeId, aIndex, info.Namespace() ) );

        if( !property )
            {
            return KErrNotFound;
            }
        
        TInt priority( ContentPriority( *property ) );
        
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
                CXnNodeAppIf *parent( target->ParentL() );
                
                if( !parent )
                    {
                    return KErrNotFound;
                    }
                                
                element = iFactory->CreateNewsTickerTransactionElementL( 
                        *target, aText, priority, aIndex );
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
            
            iPolicyEvaluator->EvaluateContentPolicyL( 
                    *target, iFactory->PolicyArray() );
                                                     
            iPolicyEvaluator->EvaluateVisibilityPolicyL( 
                    *target, iFactory->PolicyArray() );
                                                         
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

// ----------------------------------------------------------------------------
// CContentRenderer::DoPublishL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::DoPublishL( CHsContentPublisher& aPlugin, TInt aContent,
    const TDesC8& aBuf, TInt aIndex )                                   
    {
    TInt retval( KErrNotSupported );
    
    // resolve content item
    MAiContentItemIterator* iter( ContentItemIterator( 
        aPlugin, CHsContentPublisher::EPublisherContent ) ); 
            
    if ( !iter )
        {
        return retval;
        }
    
    const TAiContentItem& item( iter->ItemL( aContent ) );

    const TDesC8& type( ContentType( item ) );
    
    const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

    if( type == KContentTypeBitmap )
        {
        // Unpack icon from pointer
        CGulIcon* icon( LeaveIfNull( UnpackPtr<CGulIcon>( aBuf ), KErrArgument ) );
                
        // Publish icon
        retval = PublishIconL( aPlugin, nodeId, icon, aIndex );
        }
    else if ( type == KContentTypeImageSvg || type == KContentTypeData )        
        {
        // Publish data
        retval = PublishDataL( aPlugin, nodeId, aBuf, type, aIndex );
        }

    return retval;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::DoPublishL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::DoPublishL( CHsContentPublisher& aPlugin, TInt aContent,
    RFile& aFile, TInt aIndex )                                   
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    //Resolve content item
    MAiContentItemIterator* iter( ContentItemIterator( 
        aPlugin, CHsContentPublisher::EPublisherContent ) ); 

    if ( !iter )
        {
        return KErrNotSupported;
        }
    
    const TAiContentItem& item( iter->ItemL( aContent ) );

    const TDesC8& type( ContentType( item ) );

    // Image support
    if ( type.Find( KContentTypeImage ) != KErrNotFound )
        {
        // Find ui element
        const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

        CXnNodeAppIf* property( FindNodeByClassL( 
            nodeId, aIndex, info.Namespace() ) );
                                        
        if( !property )
            {
            return KErrNotFound;
            }
        
        // Check priority
        TInt priority( ContentPriority( *property ) );
        
        CXnNodeAppIf* target( property->ParentL() );
        
        if( !target )
            {
            return KErrNotSupported;
            }

        if ( AllowPublishByPriority( *target, priority ) )
            {            
            // Create transaction element for file
            MTransactionElement* element =
                iFactory->CreateImageTransactionElementL( 
                        *target, aFile, priority );
                                                          
            iPolicyEvaluator->EvaluateContentPolicyL( 
                    *target, iFactory->PolicyArray() );
                                                      
            iPolicyEvaluator->EvaluateVisibilityPolicyL( 
                    *target, iFactory->PolicyArray() );
                                                                     
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

// ----------------------------------------------------------------------------
// CContentRenderer::DoCleanL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::DoCleanL( CHsContentPublisher& aPlugin, TInt aContent,
    TInt aIndex )                                                
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    // Resolve content item
    MAiContentItemIterator* iter( ContentItemIterator( 
        aPlugin, CHsContentPublisher::EPublisherContent ) ); 
            
    if ( !iter )
        {
        return KErrNotSupported;
        }
    
    const TAiContentItem& item( iter->ItemL( aContent ) );

    const TDesC& nodeId( iNodeIdGenerator->ContentNodeIdL( aPlugin, item ) );

    CXnNodeAppIf* property( FindNodeByClassL( 
        nodeId, aIndex, info.Namespace() ) ); 
            
    if( !property )
        {
        return KErrNotFound;
        }
    
    TInt priority( ContentPriority( *property ) );

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
        iFactory->CreateEmptyContentTransactionElementL( 
                *target, aIndex );

    iPolicyEvaluator->EvaluateEmptyContentPolicyL( 
            *target, iFactory->PolicyArray() );
                                                   
    iPolicyEvaluator->EvaluateVisibilityPolicyL( 
            *target, iFactory->PolicyArray() );
                                                   
    ProcessTransactionElementL( element );
      
    if ( priority > KErrNotFound ) // Only for prioritized elements
        {
        // Add current ui element into content refresh map
        HBufC* uiElementId( PropertyValueL( 
            *target, XnPropertyNames::common::KId ) );
                                         
        return RefreshContentL( uiElementId, priority );
        }
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::SetImmediateMode
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::SetImmediateMode( TBool aImmediateMode )
    {
    iImmediateMode = aImmediateMode;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::IsImmediateMode
// 
// ----------------------------------------------------------------------------
//
TBool CContentRenderer::IsImmediateMode() const
    {
    return iImmediateMode;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::ProcessTransactionElementL
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::ProcessTransactionElementL( 
    MTransactionElement* aElement )
    {
    LeaveIfNull( aElement, KErrArgument );
    
    __PRINTS("*** XML UI: CContentRenderer::ProcessTransactionElementL ***");
    
    if ( IsImmediateMode() || iStack->IsEmpty() )
        {      
        __PRINTS("* Immediate transaction mode, or transaction stack is empty" );
        
        // No transaction. Commit element immediately
        TBool layoutChanged( EFalse );
        
        RPropertyHashMap propertyHashMap;
        CleanupClosePushL( propertyHashMap );
        
        aElement->CommitL( iFactory->PolicyArray(), propertyHashMap );
        SetPropertyArraysL( propertyHashMap );
        
        CleanupStack::PopAndDestroy( &propertyHashMap );
        
        iFactory->ReleaseTransactionElement( aElement );
        StartContentRefresh();
        
        // Re-layout
        iAppUi.UiEngineL()->RenderUIL();
        }
    else
        {
        // Append transaction element to transaction
        __PRINTS("* Adding transaction element to stack");
        
        MTransaction* tr( iStack->Top() );
        tr->Append( *aElement );
        }
    
    __PRINTS("*** XML UI: CContentRenderer::ProcessTransactionElementL - done ***");
    }

// ----------------------------------------------------------------------------
// CContentRenderer::FindNodeByClassL
// 
// ----------------------------------------------------------------------------
//
CXnNodeAppIf* CContentRenderer::FindNodeByClassL( const TDesC& aCid,
    TInt aIndex, const TDesC8& aNs )                                                  
    {
    // Find node
    HBufC8* classId( CnvUtfConverter::ConvertFromUnicodeToUtf8L( aCid ) );
    CleanupStack::PushL( classId );
    
    RPointerArray<CXnNodeAppIf> nodes( 
        iAppUi.UiEngineL()->FindNodeByClassL( *classId, aNs ) );

    CleanupStack::PopAndDestroy( classId );

    CleanupClosePushL( nodes );

    for ( TInt i = 0; i < nodes.Count(); ++i )
        {
        CXnNodeAppIf* node = nodes[i];

        const TDesC8* name(
            PropertyValue( *node, AiUiDef::xml::property::KName ) );

        if ( name && ( *name == AiUiDef::xml::name::KOrdinal ) )
            {
            const TDesC8* value(
                PropertyValue( *node, AiUiDef::xml::property::KValue ) );

            if ( value )
                {
                // Try to parse index from string either
                TInt32 index( 0 );

                User::LeaveIfError( AiUtility::ParseInt( index, *value ) );

                if ( index == aIndex )
                    {
                    CleanupStack::PopAndDestroy( &nodes );
                    
                    return node;
                    }
                }
            }
        else if ( name && ( *name == AiUiDef::xml::name::KTarget ) )
            {            
            const TDesC8* target(
               PropertyValue( *node, AiUiDef::xml::property::KValue ) );
            
            CXnNodeAppIf* targetNode( 
                FindNodeByIdL( *target, node->Namespace() ) );
            
            if ( targetNode )
                {
                CleanupStack::PopAndDestroy( &nodes );
                
                return targetNode;
                }
            }
        else if ( nodes.Count() == 1 ) // Only one node in class
            {
            node = nodes[ 0 ];
            
            // No ordinal specified			
			CleanupStack::PopAndDestroy( &nodes );
			
            return node;
            }
        }
    
    CleanupStack::PopAndDestroy( &nodes );
    
    return NULL; // Never reached. Needed to omit compiler warning
    }

// ----------------------------------------------------------------------------
// CContentRenderer::FindNodeByIdL
// 
// ----------------------------------------------------------------------------
//
CXnNodeAppIf* CContentRenderer::FindNodeByIdL( const TDesC& aCid, 
    const TDesC& aNs )
    {
    // Find node
    return LeaveIfNull( 
        iAppUi.UiEngineL()->FindNodeByIdL( aCid, aNs ), KErrNotFound );                        
    }

// ----------------------------------------------------------------------------
// CContentRenderer::FindNodeByIdL
// 
// ----------------------------------------------------------------------------
//
CXnNodeAppIf* CContentRenderer::FindNodeByIdL( const TDesC8& aCid, 
    const TDesC8& aNs )
    {
    // Find node
    return LeaveIfNull( 
        iAppUi.UiEngineL()->FindNodeByIdL( aCid, aNs ), KErrNotFound );                        
    }

// ----------------------------------------------------------------------------
// CContentRenderer::PublishIconL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::PublishIconL( CHsContentPublisher& aPlugin,
    const TDesC& aCid, CGulIcon* aIcon, TInt aIndex, CXnNodeAppIf* aResource )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    // Find proiperty element by class
    CXnNodeAppIf* property( 
        FindNodeByClassL( aCid, aIndex, info.Namespace() ) );                             
    
    if( !property )
        {
        return KErrNotFound;
        }
    
    // Get priority information
    TInt priority( ContentPriority( *property ) );
    
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
    
    MTransactionElement* element =
        iFactory->CreateImageTransactionElementL( 
            *target, aIcon, priority );

    if ( aResource )
        {
        iPolicyEvaluator->EvaluateResourcePolicyL( 
            *target, *aResource, iFactory->PolicyArray() );

        iPolicyEvaluator->EvaluateContentPolicyL( 
            *target, iFactory->PolicyArray() );
                                                  
        iPolicyEvaluator->EvaluateVisibilityPolicyL( 
            *target, iFactory->PolicyArray() );                                                     
        }

    else
        {
        iPolicyEvaluator->EvaluateContentPolicyL( 
            *target, iFactory->PolicyArray() );
                                                  
        iPolicyEvaluator->EvaluateVisibilityPolicyL( 
            *target, iFactory->PolicyArray() );                                                    
        }

    ProcessTransactionElementL( element );
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::PublishDataL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::PublishDataL( CHsContentPublisher& aPlugin,
    const TDesC& aCid, const TDesC8& aData, const TDesC8& aContentType,
    TInt aIndex, CXnNodeAppIf* aResource )                                     
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    CXnNodeAppIf* property( 
        FindNodeByClassL( aCid, aIndex, info.Namespace() ) );                             

    if( !property )
        {
        return KErrNotFound;
        }
    
    TInt priority( ContentPriority( *property ) );
    
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
        
    MTransactionElement* element( NULL );
    
    if ( aContentType == KContentTypeData )
        {
        element = iFactory->CreateDataBufferTransactionElementL( 
            *target, aData, priority, aCid, aIndex );
        }
    else
        {
        element = iFactory->CreateDataBufferTransactionElementL(     
            *target, aData, priority );    
        }
                                                       
    if ( aResource )
        {
        iPolicyEvaluator->EvaluateResourcePolicyL( 
            *target, *aResource, iFactory->PolicyArray() );
                                                   
        iPolicyEvaluator->EvaluateContentPolicyL( 
            *target, iFactory->PolicyArray() );
                                                  
        iPolicyEvaluator->EvaluateVisibilityPolicyL( 
            *target, iFactory->PolicyArray() );                                                     
        }
    else
        {
        iPolicyEvaluator->EvaluateContentPolicyL( 
            *target, iFactory->PolicyArray() );
                                                  
        iPolicyEvaluator->EvaluateVisibilityPolicyL( 
            *target, iFactory->PolicyArray() );                                                     
        }

    ProcessTransactionElementL( element );
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::SetProperty
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::SetProperty( CHsContentPublisher& aPlugin,
    const TDesC8& aElementId, const TDesC8& aPropertyName, 
    const TDesC8& aPropertyValue )
    {
   return ( SetProperty( aPlugin, aElementId, 
               aPropertyName, aPropertyValue,
               MAiContentObserver::EValueString ) );
    }

// ----------------------------------------------------------------------------
// CContentRenderer::SetProperty
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::SetProperty( CHsContentPublisher& aPlugin,
    const TDesC8& aElementId, const TDesC8& aPropertyName,        
    const TDesC8& aPropertyValue, MAiContentObserver::TValueType aValueType )     
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

// ----------------------------------------------------------------------------
// CContentRenderer::SetPropertyL
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::SetPropertyL( CHsContentPublisher& aPlugin,
    const TDesC8& aElementId, const TDesC8& aPropertyName,    
    const TDesC8& aPropertyValue, MAiContentObserver::TValueType aValueType )         
    {
    TInt err( KErrNone ); 
    // Find node
    
    CXnNodeAppIf* targetNode( 
        FindNodeByIdL( aElementId, aPlugin.PublisherInfo().Namespace() ) );
    
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

// ----------------------------------------------------------------------------
// CContentRenderer::DomPropertyValueType
// 
// ----------------------------------------------------------------------------
//
CXnDomPropertyValue::TPrimitiveValueType CContentRenderer::DomPropertyValueType(     
    MAiContentObserver::TValueType aValueType )
    {
    CXnDomPropertyValue::TPrimitiveValueType type(
            CXnDomPropertyValue::EUnknown );
    
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

// ----------------------------------------------------------------------------
// CContentRenderer::AllowPublishByPriority
// 
// ----------------------------------------------------------------------------
//
TBool CContentRenderer::AllowPublishByPriority( CXnNodeAppIf& aUiElement,
    TInt aPriority ) const
    {
    // Get ui element id
    const TDesC8* uiElementId( 
        PropertyValue( aUiElement, XnPropertyNames::common::KId ) );
                                                                                        
    if ( uiElementId )
        {
        // compare given priority with the current value of ui element
        return iContentPriorityMap->OverrideContent( *uiElementId, aPriority );
        }
    
    // priority cannot be used, because ui element does not have id
    return EFalse; 
    }

// ----------------------------------------------------------------------------
// CContentRenderer::StartContentRefresh
// 
// ----------------------------------------------------------------------------
//
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

// ----------------------------------------------------------------------------
// CContentRenderer::RefreshContentL
// 
// ----------------------------------------------------------------------------
//
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
    TInt* oldPriority( iRefreshableUiElements.Find( *aUiElementId ) );
    
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

// ----------------------------------------------------------------------------
// CContentRenderer::RefreshContentCallback
// 
// ----------------------------------------------------------------------------
//
TInt CContentRenderer::RefreshContentCallback( TAny* aContentRenderer )
    {
    if ( !aContentRenderer )
        {
        return KErrArgument;
        }
        
    CContentRenderer* self = 
        static_cast< CContentRenderer* >( aContentRenderer );
    
    TRAP_IGNORE( self->SendRefreshContentEventL() );
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CContentRenderer::SendRefreshContentEventL
// 
// ----------------------------------------------------------------------------
//
void CContentRenderer::SendRefreshContentEventL()
    {
    /**
     * Sends Refresh content event to framework.
     * Event is sent for content selectors with lower priority than the
     * last content which has been cleaned from ui element.
     */ 

    // Cancel periodic timer.
    iTimer->Cancel();
    
    // Get ui element id and old content priority
    TPtrHashMapIter< TDesC, TInt> iter( iRefreshableUiElements );
    iter.Reset();
    
    const TDesC* uiElementId( iter.NextKey() ); // Never NULL
    TInt priority( *( iter.CurrentValue() ) );
    
    //  Cleanup item for iterator 
    TMapCleanupItem cleanup( iter );
    CleanupReleasePushL( cleanup );
    
    // Lookup ui element
    CXnNodeAppIf* uiElement( FindNodeByIdL( *uiElementId ) );
    
    // Remove current ui element from the map
    CleanupStack::PopAndDestroy( &cleanup );
    
    // Find lower priority content elements associated to this ui element
    RPointerArray< CXnNodeAppIf > children( uiElement->ChildrenL() );
    
    // Remove higher priority content elements
    RemoveNonPriorityElements( children, priority );
        
    // Sort array to descending order
    children.Sort( TLinearOrder< CXnNodeAppIf >( DescendingPriorityOrder ) );
    
    // Send event for content selectors in descending priority order.
    // Continue until first content gets published or array exhausts.
    for ( TInt i = 0; i < children.Count(); ++i )
        {
        CXnNodeAppIf* current( children[ i ] );
        
        // Get content selector
        const HBufC* contentSelector( 
            PropertyValueL( *current, XnPropertyNames::common::KClass ) );
                                                               
        if ( contentSelector && 
            iFwEventHandler->RefreshContent( *contentSelector ) )
            {
            break;
            }
        }
    
    // Free content selector array
    children.Reset();
    
    // Continue content refresh for next ui element.
    StartContentRefresh();    
    }
	
// ----------------------------------------------------------------------------
// CContentRenderer::IsParentNewsticker
// 
// ----------------------------------------------------------------------------
//
TBool CContentRenderer::IsParentNewsticker( CXnNodeAppIf& aTarget )
    {
    CXnNodeAppIf* parent( NULL );
    
    TRAP_IGNORE( parent = aTarget.ParentL() );
    
    if ( !parent )
        {
        return EFalse;
        }
    
    const TDesC8& type( parent->Type()->Type() ) ;
            
    return ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() );
    }
	

//  End of File
