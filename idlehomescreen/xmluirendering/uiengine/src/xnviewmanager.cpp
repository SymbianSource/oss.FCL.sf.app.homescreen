/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View Manager
*
*/

// System includes
#include <AknUtils.h>
#include <AknsWallpaperUtils.h>
#include <AknSkinsInternalCRKeys.h>
#include <StringLoader.h>
#include <xnuiengine.rsg>
#include <aknnotewrappers.h>

#if 0 // MSK icon change
#include <AknsConstants.h>
#include <avkon.mbg>
#endif // MSK icon change

// User includes
#include "xnapplication.h"
#include "xnuiengine.h"
#include "xnproperty.h"
#include "xndomnode.h"
#include "xndomlist.h"
#include "xnnode.h"
#include "xncomposer.h"
#include "xneditor.h"
#include "xnrootdata.h"
#include "xnviewdata.h"
#include "xnappuiadapter.h"
#include "xncontroladapter.h"
#include "xnviewnodeimpl.h"
#include "xnviewadapter.h"
#include "xnodt.h"
#include "xndomdocument.h"
#include "xntype.h"
#include "xntext.h"
#include "xnpanic.h"
#include "xnmenu.h"
#include "xnmenuadapter.h"
#include "xncomponentnodeimpl.h"
#include "xnnodepluginif.h"
#include "xnoomsyshandler.h"

// Constants
_LIT8( KEmptyWidgetUid, "0x2001f47f" );
_LIT8( KViewSwitcherText, "switcher_text" );
_LIT8( KStateError, "Error" );
_LIT8( KMax, "max" );

_LIT8( KTemplateViewUID, "0x20026f50" );

enum
    {
    EFirstPassDrawComplete,
    EDataPluginsComplete,
    EViewIsValid,       
    };

const TInt KMinPages = 1;


#if 0 // MSK icon change

const TInt KIconIds[] = {
        EMbmAvkonQgnQgn_home_page_11,
        EMbmAvkonQgnQgn_home_page_21,
        EMbmAvkonQgnQgn_home_page_22,
        EMbmAvkonQgnQgn_home_page_31,
        EMbmAvkonQgnQgn_home_page_32,
        EMbmAvkonQgnQgn_home_page_33,
        EMbmAvkonQgnQgn_home_page_41,
        EMbmAvkonQgnQgn_home_page_42,
        EMbmAvkonQgnQgn_home_page_43,
        EMbmAvkonQgnQgn_home_page_44,
        EMbmAvkonQgnQgn_home_page_51,
        EMbmAvkonQgnQgn_home_page_52,
        EMbmAvkonQgnQgn_home_page_53,
        EMbmAvkonQgnQgn_home_page_54,
        EMbmAvkonQgnQgn_home_page_55,
        EMbmAvkonQgnQgn_home_page_61,
        EMbmAvkonQgnQgn_home_page_62,
        EMbmAvkonQgnQgn_home_page_63,
        EMbmAvkonQgnQgn_home_page_64,
        EMbmAvkonQgnQgn_home_page_65,
        EMbmAvkonQgnQgn_home_page_66
        };

const TInt KMaskIds[] = {
        EMbmAvkonQgnQgn_home_page_11_mask,
        EMbmAvkonQgnQgn_home_page_21_mask,
        EMbmAvkonQgnQgn_home_page_22_mask,
        EMbmAvkonQgnQgn_home_page_31_mask,
        EMbmAvkonQgnQgn_home_page_32_mask,
        EMbmAvkonQgnQgn_home_page_33_mask,
        EMbmAvkonQgnQgn_home_page_41_mask,
        EMbmAvkonQgnQgn_home_page_42_mask,
        EMbmAvkonQgnQgn_home_page_43_mask,
        EMbmAvkonQgnQgn_home_page_44_mask,
        EMbmAvkonQgnQgn_home_page_51_mask,
        EMbmAvkonQgnQgn_home_page_52_mask,
        EMbmAvkonQgnQgn_home_page_53_mask,
        EMbmAvkonQgnQgn_home_page_54_mask,
        EMbmAvkonQgnQgn_home_page_55_mask,
        EMbmAvkonQgnQgn_home_page_61_mask,
        EMbmAvkonQgnQgn_home_page_62_mask,
        EMbmAvkonQgnQgn_home_page_63_mask,
        EMbmAvkonQgnQgn_home_page_64_mask,
        EMbmAvkonQgnQgn_home_page_65_mask,
        EMbmAvkonQgnQgn_home_page_66_mask
        };

const TAknsItemID KSkinIds[] = {            
        KAknsIIDQgnHomePage11,
        KAknsIIDQgnHomePage21,
        KAknsIIDQgnHomePage22,
        KAknsIIDQgnHomePage31,
        KAknsIIDQgnHomePage32,
        KAknsIIDQgnHomePage33,
        KAknsIIDQgnHomePage41,
        KAknsIIDQgnHomePage42,
        KAknsIIDQgnHomePage43,
        KAknsIIDQgnHomePage44,
        KAknsIIDQgnHomePage51,
        KAknsIIDQgnHomePage52,
        KAknsIIDQgnHomePage53,
        KAknsIIDQgnHomePage54,
        KAknsIIDQgnHomePage55,
        KAknsIIDQgnHomePage61,
        KAknsIIDQgnHomePage62,
        KAknsIIDQgnHomePage63,
        KAknsIIDQgnHomePage64,
        KAknsIIDQgnHomePage65,
        KAknsIIDQgnHomePage66
        };

#endif // MSK icon change


// ============================ LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// BuildTriggerL
// Builds a trigger node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerL( CXnUiEngine& aUiEngine,
    const TDesC8& aStringValue )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );

    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );

    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );

    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );

    CXnDomStringPool& sp( aUiEngine.ODT()->DomDocument().StringPool() );

    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName, aStringValue,
        CXnDomPropertyValue::EString, sp );

    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );

    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        KNullDesC8, CXnDomPropertyValue::EString, sp );

    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );

    CleanupStack::Pop( node ); 

    return node;
    }

// -----------------------------------------------------------------------------
// SetOnlineStateL
// 
// -----------------------------------------------------------------------------
//
static void SetOnlineStateL( CXnAppUiAdapter& aAdapter, 
    CXnViewData& aViewData )
    {
    if( !aViewData.Active() )
        {
        // Only active view can change online/offline state
        return;
        }
    
    RPointerArray< CXnNode > nodes;
    CleanupClosePushL( nodes );

    RPointerArray< CXnNodeAppIf > list;
    CleanupClosePushL( list );

    aViewData.ContentSourceNodesL( nodes );

    for ( TInt i = 0; i < nodes.Count(); i++ )
        {
        list.AppendL( &nodes[i]->AppIfL() );
        }

    aAdapter.SetOnlineStateL( list );

    CleanupStack::PopAndDestroy( 2, &nodes ); // &list                                                 
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnViewManager::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnViewManager* CXnViewManager::NewL( CXnAppUiAdapter& aAdapter )    
    {
    CXnViewManager* self = CXnViewManager::NewLC( aAdapter );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnViewManager* CXnViewManager::NewLC( CXnAppUiAdapter& aAdapter )
    {
    CXnViewManager* self = new( ELeave ) CXnViewManager( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::CXnViewManager()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnViewManager::CXnViewManager( CXnAppUiAdapter& aAdapter ) 
    : iAppUiAdapter( aAdapter ), 
      iApplicationUid( iAppUiAdapter.Application()->AppDllUid() )
    {
    }

// -----------------------------------------------------------------------------
// CXnViewManager::~CXnViewManager()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnViewManager::~CXnViewManager()
    {
    iObservers.Reset();
    
    delete iRootData;
    
    delete iWidgetAmountTrigger;       

    if ( iResources )
        {
        iResources->Reset();
        }
    
    delete iResources;

    iControls.Reset();
    iAppearanceNodes.Reset();
       
    iFailedPlugins.Reset();
    
    delete iComposer;
    delete iEditor;
    delete iOomSysHandler;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnViewManager::ConstructL()
    {
    iOomSysHandler = CXnOomSysHandler::NewL();
    
    // Create resource list
    iResources = new ( ELeave ) CArrayPtrSeg< CXnResource > ( 16 );
       
    const TInt KMaxUidLength( 10 );
    _LIT8( KUint, "%u" );

    TBuf8< KMaxUidLength > uid;
    uid.Format( KUint, iApplicationUid.iUid );
    
    iEditor = CXnEditor::NewL( *this, uid );

    iHspsWrapper = &iEditor->HspsWrapper();
    
    iComposer = CXnComposer::NewL( *iHspsWrapper );
    }

// -----------------------------------------------------------------------------
// CXnViewManager::LoadUiL()
// Loads the application UI and initial view
// -----------------------------------------------------------------------------
//
void CXnViewManager::LoadUiL()
    {
    iUiEngine->DisableRenderUiLC();
    
    // Load application root configuration
    iRootData = CXnRootData::NewL( *this, iApplicationUid );
           
    // Load root configuration and initial view.
    iRootData->LoadL();
    
    CleanupStack::PopAndDestroy(); // DisableRenderUiLC();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ReloadUiL()
// Reloads the application UI and initial ivew
// -----------------------------------------------------------------------------
void CXnViewManager::ReloadUiL()  
    {
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CXnViewManager::ReloadUiL() - start" ) );
#endif //_XN_PERFORMANCE_TEST_
    
    NotifyViewDeactivatedL( ActiveViewData() );
    
    delete iWidgetAmountTrigger;
    iWidgetAmountTrigger = NULL;
          
    // Destroy active view data
    ActiveViewData().Destroy();
         
    // Schedule application configuration destroyal
    iRootData->Destroy();
    iRootData = NULL;
    
    iFlags.ClearAll();
    
    User::Heap().Compress();
    
    LoadUiL();
    
    // Activate first view from the new configuration
    iAppUiAdapter.ViewAdapter().ActivateContainerL( ActiveViewData() );    
   
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CXnViewManager::ReloadUiL() - end" ) );
#endif //_XN_PERFORMANCE_TEST_
    }

// -----------------------------------------------------------------------------
// CXnViewManager::LoadWidgetToPluginL()
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::LoadWidgetToPluginL( CHsContentInfo& aContentInfo,
    CXnPluginData& aPluginData )
    {
    // Plugin must not have widget when about to add
    if ( aPluginData.Occupied() )
        {
        return KErrAlreadyExists;
        }

    CXnViewData& viewData( 
        static_cast< CXnViewData& >( *aPluginData.Parent() ) );

    TBool emptyInUse( viewData.UseEmptyWidget() );
            
    // By default widget is added to active view configuration
    const TDesC8& configurationId( viewData.ConfigurationId() );

    TInt retval( KErrGeneral );
              
    if ( emptyInUse && aPluginData.Empty() )
        {
        retval = iHspsWrapper->ReplacePluginL( 
            aPluginData.PluginId(), aContentInfo.Uid() );   
        
        if( retval == KErrDiskFull )
            {
            ShowDiskFullMessageL();
            }
        }
    else
        {
        TInt index( viewData.PluginData().Find( &aPluginData ) );
        
        CAddPluginResult* result = iHspsWrapper->AddPluginL( 
            configurationId, aContentInfo.Uid(), index );
    
        CleanupStack::PushL( result );
        
        retval = result->Status();        
        if ( retval == KErrNone )
            {
            aPluginData.SetPluginIdL( result->PluginId() );
            }
        else if( retval == KErrDiskFull )
            {
            ShowDiskFullMessageL();
            }        
        
        CleanupStack::PopAndDestroy( result ); 
        }
             
    if ( retval == KErrNone )
        {              
        iUiEngine->DisableRenderUiLC();
                                     
        NotifyConfigureWidgetL( aContentInfo, aPluginData );
    
        aPluginData.LoadL();
                   
        UpdateCachesL();
    
        NotifyWidgetAdditionL( aPluginData );

        // Report widget amount in the view
        ReportWidgetAmountL( viewData );
        
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC
        }
           
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::UnloadWidgetFromPluginL()
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::UnloadWidgetFromPluginL( CXnPluginData& aPluginData )    
    {          
    TBool error( aPluginData.PluginState().CompareF( KStateError ) == 0 );
    
    // Plugins in error state are always removed
    if ( !error && !aPluginData.Occupied() )
        {
        // Plugin must have widget when about to remove
        return KErrNotFound;            
        }
    
    CXnViewData& viewData( 
        static_cast< CXnViewData& >( *aPluginData.Parent() ) );
    
    TBool emptyInUse( viewData.UseEmptyWidget() );
    
    TInt retval( KErrGeneral );
    
    // Save temporary
    TBuf8< 32 > id( aPluginData.PluginId() );
           
    if ( emptyInUse )
        {
        retval = iHspsWrapper->ReplacePluginL( 
            aPluginData.PluginId(), KEmptyWidgetUid );                       
        }
    else 
        {        
        retval = iHspsWrapper->RemovePluginL( aPluginData.PluginId() );
        }
       
    if ( retval == KErrNone )
        {
        TBool active( aPluginData.Active() );
        
        iUiEngine->DisableRenderUiLC();
        
        NotifyWidgetRemovalL( aPluginData );
                 
        aPluginData.Destroy();
        
        if ( emptyInUse )
            {
            // Write id back for for later usage
            aPluginData.SetEmptyL( id );
            }
        else
            {
            // Reorder plugindata array
            RPointerArray< CXnPluginData >& plugins( viewData.PluginData() ); 
                
            TInt index( plugins.Find( &aPluginData ) );

            plugins.Remove( index );
            plugins.Insert( &aPluginData, plugins.Count() - 1 );                                           
            }
                                                           
        UpdateCachesL();

        // Report widget amount in the view 
        ReportWidgetAmountL( viewData );
        
        if ( active )
            {            
            iUiEngine->RenderUIL();
            SetOnlineStateL( iAppUiAdapter, ActiveViewData() );
            }
               
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC               
        }

    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ReplaceWidgetToPluginL
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::ReplaceWidgetToPluginL( CHsContentInfo& aContentInfo,
    CXnPluginData& aPluginData, TBool aUseHsps )
    {
    TInt ret = KErrNone;
    
    // if aUseHsps is false, the plugin was already replaced by
    // another process
    if( aUseHsps )
        {
        ret = iHspsWrapper->ReplacePluginL( aPluginData.PluginId(),
                                           aContentInfo.Uid() );
        }

    if ( ret == KErrNone )
        {
        iUiEngine->DisableRenderUiLC();
        
        NotifyWidgetRemovalL( aPluginData );

        TBuf8< 32 > id( aPluginData.PluginId() );
        TBuf8< 32 > uid( aContentInfo.Uid() );
        
        aPluginData.Destroy();
                        
        UpdateCachesL();
        aPluginData.SetPluginIdL( id );
        
        NotifyConfigureWidgetL( aContentInfo, aPluginData );
        
        aPluginData.LoadL();
                                                                  
        UpdateCachesL();
        
        // notify addition if not replaced with empty widget
        // NotifyWidgetAdditionL will call RenderUIL()
        if( uid != KEmptyWidgetUid )
            {
            NotifyWidgetAdditionL( aPluginData );
            }
        else
            {
            if( aPluginData.Active() )
                {
                iUiEngine->RenderUIL();
                }
            }
        
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ODT()
// Get the ODT
// -----------------------------------------------------------------------------
//
CXnODT* CXnViewManager::ODT() const
    {
    return iRootData->ODT();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::RootNode()
// Finds the root node 
// -----------------------------------------------------------------------------
//
CXnNode* CXnViewManager::RootNode() const
    {
    return iRootData->RootNode();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ViewNode()
// Finds the view node from active view data
// -----------------------------------------------------------------------------
//
CXnNode* CXnViewManager::ViewNode() const
    {
    return ActiveViewData().ViewNode();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::Parser()
// Gets ODT parser
// -----------------------------------------------------------------------------
//
CXnODTParser& CXnViewManager::Parser() const
    {
    return iRootData->Parser();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::EcomHandler()
// Gets Ecom handler
// -----------------------------------------------------------------------------
//
CXnEcomHandler& CXnViewManager::EcomHandler() const
    {
    return iRootData->EcomHandler();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::Controls()
// Finds the controls from active view data
// -----------------------------------------------------------------------------
//
const RPointerArray< CXnControlAdapter >& CXnViewManager::Controls() const
    {
    return iControls;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::PluginNodes()
// Finds the plugin nodes from active view data
// -----------------------------------------------------------------------------
//
RPointerArray< CXnNode >& CXnViewManager::PluginNodes() const
    {
    // These must be fetched from layout tree to get correct order of plugins
    CXnViewData& activeViewData( ActiveViewData() );

    // Get first plugin data
    CXnPluginData* pluginData( activeViewData.PluginData()[0] );
    CXnNode* parent( pluginData->Owner()->LayoutNode()->Parent() );

    // This assumes that the parent has only "plugin" nodes as its children
    RPointerArray< CXnNode >& children( parent->Children() );

    return children;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::PluginDataL()
// 
// -----------------------------------------------------------------------------
//
void CXnViewManager::PluginDataL( RPointerArray< CXnPluginData >& aList, 
    TBool aGlobal ) const
    {
    if ( aGlobal )
        {
        RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
        
        for ( TInt i = 0; i < views.Count(); i++ )
            {
            RPointerArray< CXnPluginData >& plugins( views[i]->PluginData() );
            
            for ( TInt j = 0; j < plugins.Count(); j++ )
                {
                aList.AppendL( plugins[ j ] );
                }
            }            
        }
    else
        {
        RPointerArray< CXnPluginData >& 
            plugins( ActiveViewData().PluginData() ); 

        for ( TInt j = 0; j < plugins.Count(); j++ )
            {
            aList.AppendL( plugins[ j ] );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::Resources()
// Finds the resources from the active view data
// -----------------------------------------------------------------------------
//
CArrayPtrSeg< CXnResource >& CXnViewManager::Resources() const
    {
    return *iResources;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::AppearanceNodes()
// -----------------------------------------------------------------------------
//
RPointerArray< CXnNode >& CXnViewManager::AppearanceNodes() const
    {
    return iAppearanceNodes;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ViewData()
// Finds a view data based on a view node
// -----------------------------------------------------------------------------
//
CXnViewData* CXnViewManager::ViewData( CXnNode& aNode ) const
    {    
    if ( !aNode.ViewNodeImpl() )
        {
        return NULL;
        }

    RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
    
    for ( TInt i = 0; i < views.Count(); i++ )
        {
        if ( views[i]->Occupied() )
            {
            if ( views[i]->Node()->LayoutNode() == &aNode )
                {
                return static_cast< CXnViewData* >( views[ i ] );                
                }
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActiveAppData()
// Gets the active app data
// -----------------------------------------------------------------------------
//
CXnRootData& CXnViewManager::ActiveAppData() const
    {
    return *iRootData;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActiveViewData()
// Gets the active view data
// -----------------------------------------------------------------------------
//
CXnViewData& CXnViewManager::ActiveViewData() const
    {
    return iRootData->ActiveViewData();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::PreviousViewData()
// Gets the previous view data 
// -----------------------------------------------------------------------------
//
CXnViewData& CXnViewManager::PreviousViewData() const
    {
    return iRootData->PreviousViewData();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NextViewData()
// Gets the next view data 
// -----------------------------------------------------------------------------
//
CXnViewData& CXnViewManager::NextViewData() const
    {
    return iRootData->NextViewData();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActivateNextViewL()
// Activates the next view
// -----------------------------------------------------------------------------
//
void CXnViewManager::ActivateNextViewL()
    {
    CXnViewData& current( ActiveViewData() );
    CXnViewData& next( NextViewData() );

    if ( !next.Occupied() )
        {
        next.LoadL();
        }
        
    // Activate view
    if ( next.Occupied() )
        {            
        iAppUiAdapter.ViewAdapter().ActivateContainerL( next );                
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActivatePreviousViewL()
// Activates the previous view
// -----------------------------------------------------------------------------
//
void CXnViewManager::ActivatePreviousViewL()
    {
    CXnViewData& current( ActiveViewData() );
    CXnViewData& prev( PreviousViewData() );

    if ( !prev.Occupied() )
        {
        prev.LoadL();
        }
        
    // Activate view
    if ( prev.Occupied() )
        {   
        iAppUiAdapter.ViewAdapter().ActivateContainerL( prev );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::AddViewL()
// Adds a new view based on info
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::AddViewL( CHsContentInfo& aInfo )
    {    
    if ( iRootData->PluginData().Count() >= iRootData->MaxPages() )
        {
        return KErrGeneral;
        }

    // Add new view (template view) to hsps
    CAddPluginResult* result = iHspsWrapper->AddPluginL( 
        iRootData->ConfigurationId(), aInfo.Uid(),
        ViewIndex() + 1 ); 
    CleanupStack::PushL( result );

    TInt retval( result->Status() );
    
    if( retval == KErrDiskFull )
        {
        ShowDiskFullMessageL();
        }        
    else if ( retval == KErrNone )
        {                             
        // Create new view        
        CXnViewData* newView = CXnViewData::NewLC( *iRootData );    
        
        newView->SetPluginIdL( result->PluginId() );
        
        newView->SetOwner( iRootData->Node() );
        
        newView->LoadL();
                
        if ( newView->Occupied() )
            {
            // Load succeed, add the new view behind the current view               
            RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
            
            TInt index( views.Find( &ActiveViewData() ) );
            
            views.InsertL( newView, index + 1 );
            
            // Root data owns the new view now
            CleanupStack::Pop( newView );            

            // Inform observers about added view
            NotifyViewAdditionL( *newView );
            
            UpdatePageManagementInformationL();
            
            iUiEngine->RenderUIL();
            }
        else
            {                                  
            // creation failed, remove it from hsps
            iHspsWrapper->RemovePluginL( result->PluginId() );
            
            // ... and destroy it
            CleanupStack::PopAndDestroy( newView );            
            
            retval = KErrGeneral;

            }
        }

    CleanupStack::PopAndDestroy( result );   
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::AddViewL()
// Adds a new view
// -----------------------------------------------------------------------------
//
void CXnViewManager::AddViewL()
    {
    if ( iRootData->PluginData().Count() >= iRootData->MaxPages() ) 
        { 
        HBufC* msg( StringLoader::LoadLC( 
            R_QTN_HS_MAX_AMOUNT_OF_PAGES_NOTE ) ); 

        CAknErrorNote* note = new ( ELeave ) CAknErrorNote; 
        CleanupStack::PushL( note ); 
               
        note->ExecuteLD( *msg ); 
               
        CleanupStack::Pop( note ); 
        CleanupStack::PopAndDestroy( msg ); 

        return; 
        }

    // Add new view (template view) to hsps
    CAddPluginResult* result = iHspsWrapper->AddPluginL( 
        iRootData->ConfigurationId(), KTemplateViewUID,
        ViewIndex() + 1 ); 
    CleanupStack::PushL( result );
        
    TInt status( result->Status() );
    
    if( status == KErrDiskFull )
        {
        ShowDiskFullMessageL();
        }            
    else if ( status == KErrNone )
        {                             
        // Create new view        
        CXnViewData* newView = CXnViewData::NewLC( *iRootData );    
        
        newView->SetPluginIdL( result->PluginId() );
        
        newView->SetOwner( iRootData->Node() );
        
        newView->LoadL();
                
        if ( newView->Occupied() )
            {
            // Load succeed, add the new view behind the current view               
            RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
            
            TInt index( views.Find( &ActiveViewData() ) );
            
            views.InsertL( newView, index + 1 );
            
            // Root data owns the new view now
            CleanupStack::Pop( newView );
            
            // Activate view
            iAppUiAdapter.ViewAdapter().ActivateContainerL( *newView, ETrue );
            
            // Inform observers about added view
            NotifyViewAdditionL( *newView );
            }
        else
            {                      
            // creation failed, remove it from hsps
            iHspsWrapper->RemovePluginL( result->PluginId() );
            
            // ... and destroy it
            CleanupStack::PopAndDestroy( newView );            
            }
        }

    CleanupStack::PopAndDestroy( result );       
    }

// -----------------------------------------------------------------------------
// CXnViewManager::RemoveViewL()
// Removes view based info
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::RemoveViewL( const CHsContentInfo& aInfo )
    {
    TInt retval( KErrGeneral );
    
    if ( iRootData->PluginData().Count() <= 1 )
        {
        // Only one page left, not allowed to remove
        return retval;        
        }
    
    RPointerArray< CXnPluginData >& views( iRootData->PluginData() );

    for ( TInt i = 0; i < views.Count(); i++ )
        {
        CXnViewData* view( static_cast< CXnViewData* >( views[i] ) );
        
        if ( view->PluginId() == aInfo.PluginId() )
            {
            if ( !view->Removable() )
                {
                return retval;
                }
            
            if ( view->Active() )
                {
                // Activate the next view, or first if in the last view 
                CXnViewData& next( NextViewData() );
                iAppUiAdapter.ViewAdapter().ActivateContainerL( next );
                }

            retval = iHspsWrapper->RemovePluginL( view->PluginId() );
            
            // Notify observers of view list change
            NotifyViewRemovalL( *view );

            iRootData->DestroyViewData( view );
                                                                   
            // Need to update after view is removed
            UpdatePageManagementInformationL();                
                                   
            break;
            }
        }
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::RemoveViewL()
// Removes active view if more than one view.
// -----------------------------------------------------------------------------
//
void CXnViewManager::RemoveViewL()
    {   
    if ( iRootData->PluginData().Count() <= 1 || 
        !ActiveViewData().Removable() )
        {
        // Only one page left, this should not happen,  
        // or active view is not removable
        return;        
        }
          
    RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
    
    TInt index( views.Find( &ActiveViewData() ) );
           
    CAknQueryDialog* query = CAknQueryDialog::NewL(); 
    query->PrepareLC( R_YES_NO_HS_REMOVE_VIEW );
    
    HBufC* queryText( StringLoader::LoadLC( R_QTN_HS_DELETE_PAGE ) ); 
    
    query->SetPromptL( queryText->Des() );
    
    CleanupStack::PopAndDestroy( queryText );

    if ( query->RunLD() )
        { 
        // Activate the next view, or first if in the last view 
        CXnViewData& next( NextViewData() );

        iAppUiAdapter.ViewAdapter().ActivateContainerL( next );
                
        CXnViewData* view( static_cast< CXnViewData* >( views[ index ] ) );

        // Remove plugin from HSPS
        iHspsWrapper->RemovePluginL( view->PluginId() );
        
        // Notify observers of view list change
        NotifyViewRemovalL( *view );

        iRootData->DestroyViewData( view );
        
        // Need to update after view is removed
        UpdatePageManagementInformationL();        
        }       
    }

// -----------------------------------------------------------------------------
// CXnViewManager::AddObserver()
// -----------------------------------------------------------------------------
//
void CXnViewManager::AddObserver( const MXnViewObserver& aObserver )
    {
    TInt index( iObservers.Find( &aObserver ) );

    if ( index == KErrNotFound )
        {
        iObservers.Append( &aObserver );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::RemoveObserver()
// -----------------------------------------------------------------------------
//
void CXnViewManager::RemoveObserver( const MXnViewObserver& aObserver )
    {
    TInt index( iObservers.Find( &aObserver ) );

    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActivateAppL()
// Activates application configuration
// -----------------------------------------------------------------------------
TInt CXnViewManager::ActivateAppL( const TDesC8& aPluginUid )
    {
    if ( aPluginUid == KNullDesC8 )
        {
        return KErrArgument;
        }

    if ( iRootData->PluginUid().Compare( aPluginUid ) == 0 )
        {
        // Nothing to do
        return KErrNone;
        }
    
    return iHspsWrapper->SetAppConfigurationL( aPluginUid );
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActivateViewL()
// Activates view
// -----------------------------------------------------------------------------
TInt CXnViewManager::ActivateViewL( const TDesC8& aPluginId )
    {
    if ( aPluginId == KNullDesC8 )
        {
        return KErrArgument;
        }
           
    CXnViewData& current( ActiveViewData() );
       
    if ( current.PluginId() == aPluginId )
        {
        // Nothing to do
        return KErrNone;
        }

    TInt retval( KErrNotFound );
    
    RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
        
    for ( TInt i = 0; i < views.Count(); i++ )
        {
        CXnViewData* view( static_cast< CXnViewData* >( views[ i ] ) );

        if( view->Occupied() && view->PluginId() == aPluginId )
            {
            iAppUiAdapter.ViewAdapter().ActivateContainerL( *view );
            
            retval = KErrNone;
            
            break;
            }
        }
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ViewAmount()
// Gets current view amount
// -----------------------------------------------------------------------------
TInt CXnViewManager::ViewAmount() const
    {
    return iRootData->PluginData().Count();
    }


// -----------------------------------------------------------------------------
// CXnViewManager::ViewIndex()
// Gets index of current view
// -----------------------------------------------------------------------------
TInt CXnViewManager::ViewIndex() const
    {
    TInt index( KErrNotFound );
    CXnViewData* view = &( ActiveViewData() );
    RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
    index = views.Find( view );
    return index;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyContainerChangedL()
// Notifies container is changed, this is called always by CXnViewAdapter
// -----------------------------------------------------------------------------
void CXnViewManager::NotifyContainerChangedL( CXnViewData& aViewToActivate )
    {
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CXnViewManager::NotifyContainerChangedL - start" ) );
#endif //_XN_PERFORMANCE_TEST_        
              
    InvalidateActiveView();
    
    CXnViewData& viewToDeactivate( ActiveViewData() );
    
    if ( &aViewToActivate != &viewToDeactivate )
        {
        // Store focus
        CXnNode* focused( iUiEngine->FocusedNode() );
        
        if ( focused )
            {
            viewToDeactivate.SetFocusedNode( focused );
            }
        
        NotifyViewDeactivatedL( viewToDeactivate );

        // Switch active view data
        iAppUiAdapter.HandlePageSwitch();
        
        viewToDeactivate.SetActiveL( EFalse );
        aViewToActivate.SetActiveL( ETrue );
        
        iHspsWrapper->SetActivePluginL( aViewToActivate.PluginId() );

        // Cache update is needed after view activation
        UpdateCachesL();        
        }
    else
        {
        // Activate first view
        aViewToActivate.SetActiveL( ETrue );

        // Cache update is needed after view activation
        UpdateCachesL();
        }
    
    NotifyViewActivatedL( aViewToActivate );
    UpdatePageManagementInformationL();
    
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CXnViewManager::NotifyContainerChangedL - end" ) );
#endif //_XN_PERFORMANCE_TEST_
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyViewActivatedL()
// Notifies view is activated
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyViewActivatedL( const CXnViewData& aViewData )
    {
    // Report widget amount in the view
    ReportWidgetAmountL( aViewData );

    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->NotifyViewActivatedL( aViewData );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyViewDeactivatedL()
// Notifies view is deactivated
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyViewDeactivatedL( const CXnViewData& aViewData )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->NotifyViewDeactivatedL( aViewData );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyViewAdditionL()
// Notifies view is added
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyViewAdditionL( const CXnViewData& aViewData )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->NotifyViewAdditionL( aViewData );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyViewRemovalL()
// Notifies view is removed
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyViewRemovalL( const CXnViewData& aViewData )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->NotifyViewRemovalL( aViewData );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyConfigureWidgetL()
// Notifies to configure widget
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyConfigureWidgetL( const CHsContentInfo& aContentInfo,
    CXnPluginData& aPluginData )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->NotifyConfigureWidgetL( aContentInfo, aPluginData );
        }
    
    if ( aPluginData.Active() )
        {
        // Active view configuration is about to change
        InvalidateActiveView();
        }    
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyWidgetAdditionL()
// Notifys widget is added
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyWidgetAdditionL( const CXnPluginData& aPluginData )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->NotifyWidgetAdditionL( aPluginData );
        }
    
    if ( aPluginData.Active() )
        {
        // Update UI appearance after active view configuration is changed
        iUiEngine->RenderUIL();
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyWidgetRemovalL()
// Notifys widget is removed
// -----------------------------------------------------------------------------
//
void CXnViewManager::NotifyWidgetRemovalL( const CXnPluginData& aPluginData )
    {
    // This loop is intend to go from "count - 1 to 0", because CXnEditor is
    // the first registered observer and it must be notified as the last one
    for ( TInt i = iObservers.Count() - 1; i >= 0 ; i-- )
        {
        iObservers[i]->NotifyWidgetRemovalL( aPluginData ); 
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::UpdateCachesL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::UpdateCachesL()
    {
    CXnViewData& activeViewData( ActiveViewData() );
    iControls.Reset();
    activeViewData.ControlsL( iControls );
    iAppearanceNodes.Reset();
    activeViewData.AppearanceNodesL( iAppearanceNodes );
    iResources->Reset();
    activeViewData.ResourcesL( *iResources );
    }

// -----------------------------------------------------------------------------
// CXnViewManager::SetFirstPassDrawCompleteL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::SetFirstPassDrawCompleteL()
    {
    if ( iFlags.IsClear( EFirstPassDrawComplete ) )
        {
        iFlags.Set( EFirstPassDrawComplete );
        
        ValidateActiveViewL();
        }    
    }

// -----------------------------------------------------------------------------
// CXnViewManager::SetDataPluginLoadCompleteL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::SetDataPluginLoadCompleteL( 
    const CXnPluginData& aPluginData )
    {                             
    if ( !aPluginData.Active() )
        {
        // Not interested
        return;
        }
    
    if ( ActiveViewData().DataPluginsLoaded() )
        {
        if ( iFlags.IsClear( EDataPluginsComplete ) )
            {
            iFlags.Set( EDataPluginsComplete );
            
            ValidateActiveViewL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ValidateActiveViewL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::ValidateActiveViewL()
    {
    if ( iFlags.IsSet( EViewIsValid ) )
        {
        // Already ok
        return;
        }
    
    if ( iFlags.IsClear( EFirstPassDrawComplete ) || 
         iFlags.IsClear( EDataPluginsComplete ) )
        {
        // Not able to confirm yet
        return;        
        }

    RPointerArray< CXnPluginData >& plugins( ActiveViewData().PluginData() );

    TInt count( iFailedPlugins.Count() );
        
    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );
                
        const TDesC8& state( plugin->PluginState() );
        
        if ( state.CompareF( KStateError ) == 0 && plugin->Removable() )
            {
            if ( iFailedPlugins.Find( plugin ) == KErrNotFound )
                {
                iFailedPlugins.AppendL( plugin );
                }
            }            
        }

    // This condition prevents recursion
    if ( iFailedPlugins.Count() > 0 && count == 0 )
        {
        // Disable layout and redraw until all plugins are removed
        iUiEngine->DisableRenderUiLC();
        
        for ( TInt i = 0; i < iFailedPlugins.Count(); i++ )
            {
            UnloadWidgetFromPluginL( *iFailedPlugins[i] );
            }        
        
        HBufC* msg( StringLoader::LoadLC( R_QTN_HS_ERROR_WIDGETS_REMOVED ) );
            
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
        CleanupStack::PushL( note );
        
        note->ExecuteLD( *msg );
        
        CleanupStack::Pop( note );
        CleanupStack::PopAndDestroy( msg );
        
        iUiEngine->RenderUIL();

        CleanupStack::PopAndDestroy(); // DisableRenderUiLC()
        
        iFailedPlugins.Reset();               
        }
    
    // All failed plugins are handled
    if( iFailedPlugins.Count() == 0 )
        {
        iFlags.Set( EViewIsValid );
        
        SetOnlineStateL( iAppUiAdapter, ActiveViewData() );
        
        // Remaining views can be now loaded
        iRootData->LoadRemainingViews();        
        }       
    }

// -----------------------------------------------------------------------------
// CXnViewManager::InvalidateActiveView()
// -----------------------------------------------------------------------------
//
void CXnViewManager::InvalidateActiveView()
    {
    // Need to cancel async activities while view is invalidated
    iRootData->CancelLoadRemainingViews();

    iFlags.Clear( EFirstPassDrawComplete );
    iFlags.Clear( EDataPluginsComplete );
    iFlags.Clear( EViewIsValid );
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ReportWidgetAmountL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::ReportWidgetAmountL( const CXnViewData& aViewData )
    {
    CXnNode* node( aViewData.ViewNode() );

    if ( !iWidgetAmountTrigger )
        {
        iWidgetAmountTrigger = BuildTriggerL( *iUiEngine,              
            XnPropertyNames::action::trigger::name::KWidgetAmount );
        }

    RPointerArray< CXnPluginData >& plugins( aViewData.PluginData() );
    TInt max( plugins.Count() );
    TInt count( 0 );

    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );
        
        if ( plugin->Occupied() )
            {
            if( plugin->Removable() )
                {
                count++;
                }
            else
                {
                // non-removable widget consumes max amount
                max--;
                }                      
            }
        }

    CXnProperty* prop(
        iWidgetAmountTrigger->GetPropertyL( XnPropertyNames::action::KValue ) );

    if ( prop )
        {
        if ( count == max )
            {
            // All plugins occupied, report max widget amount reached
            _LIT8( KMax, "max" );
            
            static_cast< CXnDomPropertyValue* >(
                prop->Property()->PropertyValueList().Item( 0 ) )
                ->SetStringValueL( CXnDomPropertyValue::EString, KMax() );
            }
        else
            {
            // Report number of widgets
            TBuf8< 8 > value;
            
            value.AppendNum( count );
            
            static_cast< CXnDomPropertyValue* >(
                prop->Property()->PropertyValueList().Item( 0 ) )
                ->SetStringValueL( CXnDomPropertyValue::EString, value );
            }
        }

    node->ReportXuikonEventL( *iWidgetAmountTrigger );
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ShowOperationFailedMessageL
// -----------------------------------------------------------------------------
//
void CXnViewManager::ShowDiskFullMessageL() const
    {
    HBufC* msg( StringLoader::LoadLC( R_QTN_HS_OPERATION_FAILED_NO_DISK ) ); 

    CAknErrorNote* note = new ( ELeave ) CAknErrorNote; 
    CleanupStack::PushL( note ); 
           
    note->ExecuteLD( *msg );
           
    CleanupStack::Pop( note ); 
    CleanupStack::PopAndDestroy( msg );     
    }

// -----------------------------------------------------------------------------
// CXnViewManager::OOMSysHandler
// -----------------------------------------------------------------------------
//
CXnOomSysHandler& CXnViewManager::OomSysHandler() const
    {
    return *iOomSysHandler;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::UpdateViewSwitcherInformationL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::UpdatePageManagementInformationL()
    { 
    // Update MSK info
    // Obtain menu bar
    CXnNode* menuBar( iUiEngine->MenuBarNode() );

    if ( menuBar )
        {
        // Get object implementing MXnMenuInterface
        XnMenuInterface::MXnMenuInterface* menuIf( NULL );
        XnComponentInterface::MakeInterfaceL( menuIf, menuBar->AppIfL() );
        
        if ( menuIf )
            {
#if 0 // MSK icon change
            // Get count of views in array and index of actual view
            TInt count( ViewAmount() );
            TInt current( ViewIndex() + 1 );

            // Update MSK icon
            TInt index = ResolveIconIndex( count, current );
            menuIf->SetSoftKeyImageL( KSkinIds[index],
                                      AknIconUtils::AvkonIconFileName(), 
                                      KIconIds[index],
                                      KMaskIds[index],
                                      XnMenuInterface::MXnMenuInterface::ECenter, 
                                      ETrue );

#else // MSK icon change
            // Get array of views
            RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
            // Get count of views in array and index of actual view
            TInt count( views.Count() );
            TInt actual( views.Find( &ActiveViewData() ) + 1 );
            
            // Construct a text info
            _LIT( KSeparator, "/" );
            TBuf< 32 >info( KNullDesC );
            
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                info.AppendNum( actual );
                info.Append( KSeparator );
                info.AppendNum( count );
                }
            else
                {
                info.AppendNum( actual );
                info.Append( KSeparator );
                info.AppendNum( count );
                }
      
            AknTextUtils::LanguageSpecificNumberConversion( info );

            // Update info
            menuIf->SetSoftKeyTextL( info, 
                XnMenuInterface::MXnMenuInterface::ECenter );
#endif // MSK icon change
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::UpdatePluginStateL()
// -----------------------------------------------------------------------------
//
void CXnViewManager::UpdatePluginStateL( CXnPluginData& aPluginData )
    {
    if ( aPluginData.ConfigurationId().Length() == 0 && 
         aPluginData.PluginId().Length() )
        {
        CHspsConfiguration* configuration( 
            iHspsWrapper->GetPluginConfigurationL( aPluginData.PluginId() ) );
        CleanupStack::PushL( configuration );
        aPluginData.SetConfigurationIdL( configuration->ConfId() );
        CleanupStack::PopAndDestroy( configuration );
        }

    if ( aPluginData.ConfigurationId().Length() && 
         aPluginData.PluginId().Length() )
        {
        iHspsWrapper->SetConfStateL( 
                aPluginData.ConfigurationId(), aPluginData.PluginState() );        
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::MaxPages()
// 
// -----------------------------------------------------------------------------
//
TInt32 CXnViewManager::MaxPages()
    {
    return iRootData->MaxPages();
    }


#if 0 // MSK icon change


// -----------------------------------------------------------------------------
// CXnViewManager::ResolveIconIndex
// 
// -----------------------------------------------------------------------------
TInt CXnViewManager::ResolveIconIndex( TInt aPageCount, TInt aPageNum ) const
    {
    TInt iconCount( 0 );

    // Calculate total amount of icons. Each page has a aPage amount of icons.
    for ( TInt i( aPageCount ); 0 < i; i-- )
        {
        iconCount += i;
        }

    TInt index( iconCount - aPageCount + aPageNum - 1 );

    return index;
    }

#endif // MSK icon change
// End of file
