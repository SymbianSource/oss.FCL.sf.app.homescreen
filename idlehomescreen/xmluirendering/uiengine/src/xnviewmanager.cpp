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
#include <startupdomainpskeys.h>
#include <AknUtils.h>
#include <AknsWallpaperUtils.h>
#include <AknSkinsInternalCRKeys.h>
#include <StringLoader.h>
#include <xnuiengine.rsg>
#include <aknnotewrappers.h>
#include <AknsConstants.h>
#include <aifwdefs.h>
#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>

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
#include "xnbackgroundmanager.h"
#include "xneffectmanager.h"

// Constants
_LIT8( KEmptyWidgetUid, "0x2001f47f" );
_LIT8( KTemplateViewUID, "0x20026f50" );

const TInt KPSCategoryUid( 0x200286E3 );
const TInt KPSCrashCountKey( 1 );     
const TInt KStabilityInterval( 60000000 ); // 1 minute
const TInt KCrashRestoreDefaultThreshold( 3 );
const TInt KCrashRestoreAllTreshold( 4 );

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

    CXnDomStringPool* sp( aUiEngine.ODT()->DomDocument().StringPool() );

    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName, aStringValue,
        CXnDomPropertyValue::EString, *sp );

    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );

    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        KNullDesC8, CXnDomPropertyValue::EString, *sp );

    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );

    CleanupStack::Pop( node ); 

    return node;
    }

// -----------------------------------------------------------------------------
// resolveIconId
// -----------------------------------------------------------------------------
//
static TAknsItemID ResolveIconId( TInt aIndex )
    {
    TAknsItemID ret( KAknsIIDNone );
    switch( aIndex )
        {
        case 0:
            {
            ret = KAknsIIDQgnHomePage11;
            }
            break;
        case 1:
            {
            ret = KAknsIIDQgnHomePage21;
            }
            break;
        case 2:
            {
            ret = KAknsIIDQgnHomePage22;
            }
            break;
        case 3:
            {
            ret = KAknsIIDQgnHomePage31;
            }
            break;
        case 4:
            {
            ret = KAknsIIDQgnHomePage32;
            }     
            break;
        case 5:
            {
            ret = KAknsIIDQgnHomePage33;
            }    
            break;
        case 6:
            {
            ret = KAknsIIDQgnHomePage41;
            }
            break;
        case 7:
            {
            ret = KAknsIIDQgnHomePage42;
            }   
            break;
        case 8:
            {
            ret = KAknsIIDQgnHomePage43;
            }   
            break;
        case 9:
            {
            ret = KAknsIIDQgnHomePage44;
            }     
            break;
        case 10:
            {
            ret = KAknsIIDQgnHomePage51;
            }     
            break;
        case 11:
            {
            ret = KAknsIIDQgnHomePage52;
            }     
            break;
        case 12:
            {
            ret = KAknsIIDQgnHomePage53;
            } 
            break;
        case 13:
            {
            ret = KAknsIIDQgnHomePage54;
            }     
            break;
        case 14:
            {
            ret = KAknsIIDQgnHomePage55;
            }
            break;
        case 15:
            {
            ret = KAknsIIDQgnHomePage61;
            }    
            break;
        case 16:
            {
            ret = KAknsIIDQgnHomePage62;
            }   
            break;
        case 17:
            {
            ret = KAknsIIDQgnHomePage63;
            }    
            break;
        case 18:
            {
            ret = KAknsIIDQgnHomePage64;
            }
            break;
        case 19:
            {
            ret = KAknsIIDQgnHomePage65;
            }
            break;            
        case 20:
            {
            ret = KAknsIIDQgnHomePage66;
            }            
            break;            
        default:
            {
            }
            break;
        }
    
    return ret;
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
    delete iStabilityTimer;
    
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
    
    DoRobustnessCheckL();
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
    iRootData->Load();
           
    CleanupStack::PopAndDestroy(); // DisableRenderUiLC();
    
    // Activate initial view already here to get publishers loaded        
    ActiveViewData().SetActive( ETrue );    
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
TInt CXnViewManager::LoadWidgetToPluginL( const CHsContentInfo& aContentInfo,
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
    
        retval = aPluginData.Load();
        
        if ( retval == KErrNone )
            {
            UpdateCachesL();
        
            NotifyWidgetAdditionL( aPluginData );

            // Report widget amount in the view
            ReportWidgetAmountL( viewData );           
            }  
        else if ( retval == KErrNoMemory )
            {
            aPluginData.ShowOutOfMemError();
            }
        else if ( retval == KXnErrPluginFailure )
            {            
            aPluginData.ShowContentRemovedError();
            }
        
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC
        }
           
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::UnloadWidgetFromPluginL()
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::UnloadWidgetFromPluginL( CXnPluginData& aPluginData, 
    TBool aForce )    
    {                  
    if ( !aForce )
        {
        if( !aPluginData.Occupied() )
            {
            // Plugin must have widget when about to remove
            return KErrNotFound;                    
            }
        if ( !aPluginData.Removable() )
            {
            // Not allowed to remove locked
            return KErrArgument;
            }
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
            }
               
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC               
        }

    return retval;
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ReplaceWidgetToPluginL
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::ReplaceWidgetToPluginL( const CHsContentInfo& aContentInfo,
    CXnPluginData& aPluginData )
    {
    TInt retval( KErrNone );
    
    retval = iHspsWrapper->ReplacePluginL( aPluginData.PluginId(),
                                           aContentInfo.Uid() );

    if ( retval == KErrNone )
        {
        iUiEngine->DisableRenderUiLC();
        
        NotifyWidgetRemovalL( aPluginData );

        TBuf8< 32 > id( aPluginData.PluginId() );
        TBuf8< 32 > uid( aContentInfo.Uid() );
        
        aPluginData.Destroy();
                        
        UpdateCachesL();
        aPluginData.SetPluginIdL( id );
        
        NotifyConfigureWidgetL( aContentInfo, aPluginData );
        
        retval = aPluginData.Load();
        
        if ( retval == KErrNone )
            {
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
            }
        else if ( retval == KErrNoMemory )
            {
            aPluginData.ShowOutOfMemError();
            }
        else if ( retval == KXnErrPluginFailure )
            {            
            aPluginData.ShowContentRemovedError();
            }
                        
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC
        }
    
    return retval;
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
// CXnViewManager::PluginDataL()
// Returns list of plugins from all views or from the defined view
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::PluginDataL( const TDesC8& aViewId, 
    RPointerArray< CXnPluginData >& aList ) const
    {
    TInt err( KErrNone );
    TBool found( EFalse );
    
    RPointerArray< CXnPluginData >& views( iRootData->PluginData() );
    
    for ( TInt i = 0; i < views.Count(); i++ )
        {
        if ( !aViewId.Length() || views[i]->PluginId().Compare( aViewId ) == 0 )
            {
            found = ETrue;
            RPointerArray< CXnPluginData >& plugins( views[i]->PluginData() );
            
            for ( TInt j = 0; j < plugins.Count(); j++ )
                {
                aList.AppendL( plugins[ j ] );
                }
            }
        }      
    if ( !found )
        {
        // View not found
        err = KErrArgument;
        }
    
    return err;
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
void CXnViewManager::ActivateNextViewL( TInt /*aEffectId*/ )
    { 
    CXnViewData& next( NextViewData() );
    
    if ( !next.Occupied() )
        {                
        if ( next.Load() == KErrNoMemory )
            {
            next.ShowOutOfMemError();
            
            return;
            }
        }
        
    // Activate view
    if ( next.Occupied() && !next.Active() )
        {       
        CXnControlAdapter* thisView( 
            ActiveViewData().ViewNode()->Control() );                
        
        CXnControlAdapter* nextView( 
            next.ViewNode()->Control() );
    
        GfxTransEffect::Register( thisView, KGfxContextActivateNextView );    
        GfxTransEffect::Register( nextView, KGfxContextActivateNextView );
        
        TInt ret( GfxTransEffect::BeginGroup() );
        
        CCoeControl* bg( &iAppUiAdapter.ViewAdapter().BgManager() );
        
        GfxTransEffect::Begin( bg, KGfxControlActionAppear );

        GfxTransEffect::SetDemarcation( bg, bg->Position() );
        GfxTransEffect::End( bg );        
        
        GfxTransEffect::Begin( thisView, KGfxControlActionDisappear );
        
        iAppUiAdapter.ViewAdapter().ActivateContainerL( next );
        
        GfxTransEffect::SetDemarcation( thisView, thisView->Position() );
        GfxTransEffect::End( thisView );
                
        GfxTransEffect::Begin( nextView, KGfxControlActionAppear );
        GfxTransEffect::SetDemarcation( nextView, nextView->Position() );
        GfxTransEffect::End( nextView );
                                
        GfxTransEffect::EndGroup( ret );
        
        GfxTransEffect::Deregister( thisView );
        GfxTransEffect::Deregister( nextView );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ActivatePreviousViewL()
// Activates the previous view
// -----------------------------------------------------------------------------
//
void CXnViewManager::ActivatePreviousViewL( TInt /*aEffectId*/ )
    {    
    CXnViewData& prev( PreviousViewData() );

    if ( !prev.Occupied() )
        {
        if ( prev.Load() == KErrNoMemory )
            {
            prev.ShowOutOfMemError();
            
            return;
            }
        }
        
    // Activate view
    if ( prev.Occupied() && !prev.Active() )
        {
        CXnControlAdapter* thisView( 
            ActiveViewData().ViewNode()->Control() ); 
        
        CXnControlAdapter* prevView( 
            prev.ViewNode()->Control() );

        GfxTransEffect::Register( thisView, KGfxContextActivatePrevView );    
        GfxTransEffect::Register( prevView, KGfxContextActivatePrevView );
    
        TInt ret( GfxTransEffect::BeginGroup() );
            
        CCoeControl* bg( &iAppUiAdapter.ViewAdapter().BgManager() );
        
        GfxTransEffect::Begin( bg, KGfxControlActionAppear );
    
        GfxTransEffect::SetDemarcation( bg, bg->Position() );
        GfxTransEffect::End( bg );        
        
        GfxTransEffect::Begin( thisView, KGfxControlActionDisappear );
        
        iAppUiAdapter.ViewAdapter().ActivateContainerL( prev );
        
        GfxTransEffect::SetDemarcation( thisView, thisView->Position() );
        GfxTransEffect::End( thisView );
                
        GfxTransEffect::Begin( prevView, KGfxControlActionAppear );
                              
        GfxTransEffect::SetDemarcation( prevView, prevView->Position() );
        GfxTransEffect::End( prevView );
                                
        GfxTransEffect::EndGroup( ret );
        
        GfxTransEffect::Deregister( thisView );
        GfxTransEffect::Deregister( prevView );        
        }
    }

// -----------------------------------------------------------------------------
// CXnViewManager::AddViewL()
// Adds a new view based on info
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::AddViewL( const CHsContentInfo& aInfo )
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
        
        retval = newView->Load();
                
        if ( retval == KErrNoMemory )
            {
            newView->ShowOutOfMemError();
            }
        
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
void CXnViewManager::AddViewL( TInt aEffectId )
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
        
        status = newView->Load();
        
        if ( status == KErrNoMemory )
            {
            newView->ShowOutOfMemError();
            }
        
        if ( newView->Occupied() )
            {
            // Start transition effect
            if( aEffectId )
                {
                iAppUiAdapter.EffectManager()->BeginFullscreenEffectL(
                        aEffectId, ActiveViewData() );        
                }

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
                return KErrArgument;
                }
            
            if ( view->Active() )
                {
                // Destroy publishers here, must be plugin shutdown
                view->DestroyPublishers( EAiFwPluginShutdown );
            
                // Activate the next view, or first if in the last view 
                CXnViewData& next( NextViewData() );
                iAppUiAdapter.ViewAdapter().ActivateContainerL( next );
                }

            // Remove wallpaper from cache
            iAppUiAdapter.ViewAdapter().BgManager().DeleteWallpaper( *view );

            retval = iHspsWrapper->RemovePluginL( view->PluginId() );
            
            // Notify observers of view list change
            TRAPD( err, NotifyViewRemovalL( *view ) );
            if ( err != KErrNone )
                {
                // ignored
                }            

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
void CXnViewManager::RemoveViewL( TInt aEffectId )
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
        // Start transition effect
        if( aEffectId )
            {
            iAppUiAdapter.EffectManager()->BeginFullscreenEffectL(
                    aEffectId, ActiveViewData() );        
            }

        // Activate the next view, or first if in the last view 
        CXnViewData& next( NextViewData() );

        if ( !next.Occupied() )
            {
            next.Load();
            }

        CXnViewData* view( static_cast< CXnViewData* >( views[ index ] ) );
        
        // Destroy publishers here, must be plugin shutdown
        view->DestroyPublishers( EAiFwPluginShutdown );
        
        iAppUiAdapter.ViewAdapter().ActivateContainerL( next );
                        
        // Remove wallpaper from cache
        iAppUiAdapter.ViewAdapter().BgManager().DeleteWallpaper( *view );

        // Remove plugin from HSPS
        iHspsWrapper->RemovePluginL( view->PluginId() );
        
        // Notify observers of view list change
        TRAPD( err, NotifyViewRemovalL( *view ) );
        if ( err != KErrNone )
            {
            // ignored
            }   

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

    if ( iRootData->PluginUid().CompareF( aPluginUid ) == 0 )
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
// CXnViewManager::MaxPages()
// Gets the maximum allowed page count for this app configuration
// -----------------------------------------------------------------------------
//
TInt CXnViewManager::MaxPages() const
    {
    return iRootData->MaxPages();
    }

// -----------------------------------------------------------------------------
// CXnViewManager::NotifyContainerChangedL()
// Notifies container is changed, this is called always by CXnViewAdapter
// -----------------------------------------------------------------------------
void CXnViewManager::NotifyContainerChangedL( CXnViewData& aViewToActivate )
    {
    CXnViewData& viewToDeactivate( ActiveViewData() );
    
    if ( &aViewToActivate != &viewToDeactivate )
        {        
        NotifyViewDeactivatedL( viewToDeactivate );
                      
        viewToDeactivate.SetActive( EFalse );
        aViewToActivate.SetActive( ETrue );
                             
        iHspsWrapper->SetActivePluginL( aViewToActivate.PluginId() ); 
                    
        // Cache update is needed after view activation
        UpdateCachesL();               
        }
    else
        {
        // Activate first view
        aViewToActivate.SetActive( ETrue );

        // Cache update is needed after view activation
        UpdateCachesL();
        
        // Schedule remaining views loading
        iRootData->LoadRemainingViews();
        }
    
    NotifyViewActivatedL( aViewToActivate );
    UpdatePageManagementInformationL();
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
// CXnViewManager::UpdatePageManagementInformationL()
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
            // Get count of views in array and index of actual view
            TInt count( ViewAmount() );
            TInt current( ViewIndex() + 1 );

            // Update MSK icon
            /*
            TInt index = ResolveIconIndex( count, current );
            menuIf->SetSoftKeyImageL( KSkinIds[index],
                                      KNullDesC, 
                                      0,
                                      0,
                                      XnMenuInterface::MXnMenuInterface::ECenter, 
                                      ETrue );
            */
            TInt index = ResolveIconIndex( count, current );
            TAknsItemID iconId = ResolveIconId( index );
            menuIf->SetSoftKeyImageL( iconId,
                                      KNullDesC, 
                                      0,
                                      0,
                                      XnMenuInterface::MXnMenuInterface::ECenter, 
                                      ETrue );            
            }
        }
    }

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

// -----------------------------------------------------------------------------
// CXnViewManager::SystemStabileTimerCallback 
// -----------------------------------------------------------------------------
TInt CXnViewManager::SystemStabileTimerCallback( TAny* aAny )
    {
    CXnViewManager* self = static_cast<CXnViewManager*>( aAny );
    
    self->ResetCrashCount();

    if( self->iStabilityTimer )
        {
        self->iStabilityTimer->Cancel();
        }    
    
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ResetCrashCount 
// -----------------------------------------------------------------------------
void CXnViewManager::ResetCrashCount()
    {
    RProperty::Set( TUid::Uid( KPSCategoryUid ), KPSCrashCountKey, 0 );    
    }

// -----------------------------------------------------------------------------
// CXnViewManager::ShowErrorNoteL 
// -----------------------------------------------------------------------------
void CXnViewManager::ShowErrorNoteL()
    {        
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    query->PrepareLC( R_HS_CONTENT_REMOVED_VIEW );

    HBufC* queryText( StringLoader::LoadLC( R_HS_ERROR_CONTENT_REMOVED ) );    
    query->SetPromptL( queryText->Des() );    
    CleanupStack::PopAndDestroy( queryText );

    query->RunLD();   
    }

// -----------------------------------------------------------------------------
// CXnViewManager::DoRobustnessCheckL 
// -----------------------------------------------------------------------------
void CXnViewManager::DoRobustnessCheckL()
    {
    TInt crashCount = 0;
    RProperty::Get( TUid::Uid( KPSCategoryUid ),
                    KPSCrashCountKey,
                    crashCount );    
    
    if( crashCount == KCrashRestoreDefaultThreshold )
        {    
        TInt err = iHspsWrapper->RestoreDefaultConfL();         
        ShowErrorNoteL();
        }
    else if( crashCount >= KCrashRestoreAllTreshold )
        {       
        TInt err = iHspsWrapper->RestoreRootL();              
        ResetCrashCount();
        return;
        }
    
    if( crashCount > 0 )
        {        
        // Start stability timer to ensure that
        // if system is stabile at least KStabilityInterval
        // then crash count is reset to 0. 
        iStabilityTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        iStabilityTimer->Start( KStabilityInterval,
                                KStabilityInterval,
                                TCallBack( SystemStabileTimerCallback, this ) );
        }           
    }

// End of file
