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
* Description:  Data class to hold view data info
*
*/

// System includes
#include <babitflags.h>
#include <aifwdefs.h>

// User includes
#include "xnappuiadapter.h"
#include "xncomposer.h"
#include "xnodtparser.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xncontroladapter.h"
#include "xndomnode.h"
#include "xnnode.h"
#include "xnoomsyshandler.h"
#include "xnpanic.h"

#include "debug.h"

// Constants
const TInt KLoadDelay( 10000 );
const TInt KInterval( 10000 );

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnViewData::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnViewData* CXnViewData::NewL( CXnPluginData& aParent )
    {
    CXnViewData* self = CXnViewData::NewLC( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewData::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnViewData* CXnViewData::NewLC( CXnPluginData& aParent )
    {
    CXnViewData* self = new ( ELeave ) CXnViewData( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewData::CXnViewData()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnViewData::CXnViewData( CXnPluginData& aParent )
    : CXnPluginData( aParent )
    {
    // By default view data is removable
    iFlags.Set( EIsRemovable );      
    }

// -----------------------------------------------------------------------------
// CXnViewData::~CXnViewData()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnViewData::~CXnViewData()
    {
    Destroy();
    }

// -----------------------------------------------------------------------------
// CXnViewData::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnViewData::ConstructL()
    {
    CXnPluginData::ConstructL();
    }

// -----------------------------------------------------------------------------
// CXnViewData::SetActive()
// 
// -----------------------------------------------------------------------------
//
void CXnViewData::SetActive( TBool aActive )
    {    
    TBool active( Active() );
    
    if ( active == aActive || !Occupied() )
        {
        return;
        }
       
    if ( aActive )
        {
        iFlags.Set( EIsActive );
        iFlags.Clear( EIsInitial );
        
        LoadPublishers();                             
        }
    else
        {                              
        DestroyPublishers( EAiFwPageShutdown );

        iFlags.Clear( EIsActive );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::Load()

// -----------------------------------------------------------------------------
//
TInt CXnViewData::Load()
    {               
    if ( Occupied() )
        {
        return KErrInUse;
        }
       
    if ( !CXnOomSysHandler::HeapAvailable( VIEW_MIN_MEM ) )
        {                                
        return KErrNoMemory;
        }
                   
    TInt err( KErrNone );
    
    TRAP( err, err = iManager.Composer().ComposeViewL( *this ) );

    if ( err == KErrNone )
        {
        TRAP( err, iManager.Parser().LoadViewL( *this ) );
        }
        
    if ( err == KErrNone )
        {    
        iVirginPublishers = ETrue;
        
        // Load view's initial widgets
        for ( TInt i = 0; i < iPluginsData.Count(); i++ )
            {
            if ( iPluginsData[i]->PluginId() != KNullDesC8 )
                {                    
                TInt err2( iPluginsData[i]->Load() );
                
                if ( err2 == KXnErrPluginFailure )
                    {
                    // Content removed error note must be   
                    // displayed once when this view is activated
                    iShowContentRemoved = ETrue;
                                        
                    err = err2;                                        
                    }          
                else if ( err2 == KErrNoMemory )
                    {                                          
                    err = err2;                                                                  
                    break;
                    }
                }
            }
                       
        // Succesfully enough composed, publishers 
        // will be loaded when view is activated
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CXnViewData::Destroy()

// -----------------------------------------------------------------------------
//
void CXnViewData::Destroy()
    {
    if ( Occupied() )
        {        
        // View is deleted, pretend plugin is removed from UI
        DestroyPublishers( EAiFwPluginShutdown );
        
        iManager.Parser().DestroyView( *this );
        }
        
    delete iBgImage;
    iBgImage = NULL;
    
    delete iBgImagePath;
    iBgImagePath = NULL;
              
    Flush();      
    }

// -----------------------------------------------------------------------------
// CXnViewData::Plugin()
// Finds plugin or view data based on node
// -----------------------------------------------------------------------------
//
CXnPluginData* CXnViewData::Plugin( CXnNode* aNode )
    {
    if ( !aNode ) 
        { 
        return NULL; 
        }

    CXnDomNode* view( Node() );
    
    if ( aNode->ViewNodeImpl() )
        {
        // Reached view, return self
        if ( view && view->LayoutNode() == aNode )
            {
            return this;
            }
        }

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        CXnDomNode* plugin( iPluginsData[i]->Owner() );
        
        if ( plugin && plugin->LayoutNode() == aNode )
            {
            return iPluginsData[i];
            }
        }

    // Not found, try parent
    CXnNode* parent( aNode->Parent() );

    return Plugin( parent );
    }

// -----------------------------------------------------------------------------
// CXnViewData::Plugin()
// Finds plugin or view data based on namespace
// -----------------------------------------------------------------------------
//
CXnPluginData* CXnViewData::Plugin( const TDesC8& aNamespace )
    {
    if ( aNamespace == KNullDesC8 )
        {
        // No namespace, return self for global access
        return this;
        }

    CXnDomNode* view( Node() );

    if ( view && view->LayoutNode()->Namespace() == aNamespace )
        {
        return this;
        }

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        CXnDomNode* widget( iPluginsData[i]->Node() );

        if ( widget && widget->LayoutNode()->Namespace() == aNamespace )
            {
            return iPluginsData[i];
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnViewData::ViewNode()
// Returns this view data's view node
// -----------------------------------------------------------------------------
//
CXnNode* CXnViewData::ViewNode() const
    {
    return iNode->LayoutNode();
    }

// -----------------------------------------------------------------------------
// CXnViewData::SetWallpaperImage
// Store background image. Ownership is transferred.
// -----------------------------------------------------------------------------
//
void CXnViewData::SetWallpaperImage( CFbsBitmap* aBitmap )
    {
    delete iBgImage;
    iBgImage = aBitmap;
    }

// -----------------------------------------------------------------------------
// CXnViewData::WallpaperImage
// Get wallpaper image.
// -----------------------------------------------------------------------------
//
CFbsBitmap* CXnViewData::WallpaperImage() const
    {
    return iBgImage;
    }

// -----------------------------------------------------------------------------
// CXnViewData::SetWallpaperImagePathL
// -----------------------------------------------------------------------------
//
void CXnViewData::SetWallpaperImagePathL( const TDesC& aFileName )
    {
    if( !iBgImagePath || iBgImagePath->Des() != aFileName )
        {
        delete iBgImagePath;
        iBgImagePath = NULL;
        iBgImagePath = aFileName.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::WallpaperImagePath
// -----------------------------------------------------------------------------
//
const TDesC& CXnViewData::WallpaperImagePath() const
    {
    if( iBgImagePath )
        {
        return *iBgImagePath;        
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CXnViewData::ResourcesL()
// Gets this view data's resources
// -----------------------------------------------------------------------------
//
void CXnViewData::ResourcesL( CArrayPtrSeg< CXnResource >& aList ) const
    {
    // Get my resources
    CXnPluginData::ResourcesL( aList );

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And resources which my plugin holds
        iPluginsData[i]->ResourcesL( aList );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::ControlsL()
// Gets this view data's controls
// -----------------------------------------------------------------------------
//
void CXnViewData::ControlsL( RPointerArray< CXnControlAdapter >& aList ) const
    {
    // Get my controls
    CXnPluginData::ControlsL( aList );

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And controls which my plugin holds
        iPluginsData[i]->ControlsL( aList );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::PluginNodesL()
// Gets this view data's appearance nodes
// -----------------------------------------------------------------------------
//
void CXnViewData::AppearanceNodesL( RPointerArray< CXnNode >& aList ) const
    {
    // Get my appearance nodes
    CXnPluginData::AppearanceNodesL( aList );

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And appearance nodes which my plugin holds
        iPluginsData[i]->AppearanceNodesL( aList );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::PopupNodesL()
// Gets this view data's popup nodes
// -----------------------------------------------------------------------------
//
void CXnViewData::PopupNodesL( RPointerArray< CXnNode >& aList ) const
    {
    // Get my Popup nodes
    CXnPluginData::PopupNodesL( aList );

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And Popup nodes which my plugin holds
        iPluginsData[i]->PopupNodesL( aList );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::InitialFocusNodesL()
// Gets this view data's initial focus nodes
// -----------------------------------------------------------------------------
//
void CXnViewData::InitialFocusNodesL( RPointerArray< CXnNode >& aList ) const
    {
    // Offer widgets initial focus nodes by default first
    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And appearance nodes which my plugin holds
        iPluginsData[i]->InitialFocusNodesL( aList );
        }

    // Get my initial focus nodes
    CXnPluginData::InitialFocusNodesL( aList );
    }

// -----------------------------------------------------------------------------
// CXnViewData::ContentSourceNodesL()
// Gets this view data's content source nodes
// -----------------------------------------------------------------------------
//
void CXnViewData::ContentSourceNodesL( RPointerArray< CXnNode >& aList ) const
    {
    // Get my data publisher nodes
    CXnPluginData::ContentSourceNodesL( aList );

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And publisher nodes which my plugin holds
        iPluginsData[i]->ContentSourceNodesL( aList );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::PluginNodesL()
// Gets this view data's plugin nodes
// -----------------------------------------------------------------------------
//
void CXnViewData::PluginNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And controls which my plugin holds
        aList.AppendL( iPluginsData[i]->Owner()->LayoutNode() );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::LoadPublishers
// Loads data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnViewData::LoadPublishers()
    {
    if( !Active() || !Occupied() )
        {               
        return;
        }
    
    iLoader->Cancel();
    
    iLoadIndex = 0;
                                
    iLoader->Start( TTimeIntervalMicroSeconds32( KLoadDelay ),
                    TTimeIntervalMicroSeconds32( KInterval ),
                    TCallBack( DoLoadPublishersL, this ) );           
    }

// -----------------------------------------------------------------------------
// CXnViewData::DoLoadPublishersL()
// 
// -----------------------------------------------------------------------------
//
/* static */ TInt CXnViewData::DoLoadPublishersL( TAny* aAny )
    {
    __PRINTS( "*** CXnViewData::DoLoadPublishersL" );
    
    CXnViewData* self = static_cast< CXnViewData* >( aAny );
    
    CXnAppUiAdapter* appui = static_cast< CXnAppUiAdapter* >( iAvkonAppUi );         
    
    RPointerArray< CXnPluginData >& plugins( self->PluginData() );
    
    for ( TInt i = self->iLoadIndex; i < plugins.Count(); i++ )
        {        
        if ( !plugins[i]->Occupied() )
            {
            self->iLoadIndex++;
            }
        else
            {
            break;
            }
        }
        
    if ( self->iLoadIndex < plugins.Count() )
        {
        CXnPluginData* plugin( plugins[self->iLoadIndex] );
             
        self->iLoadIndex++;
        
        TInt reason( plugin->VirginPublishers() ? 
            EAiFwSystemStartup : EAiFwPageStartup );         
        
        TInt ret( plugin->LoadPublishers( reason ) );
        
        if ( ret == KErrAlreadyExists )
            {
            ret = KErrNone;
            }
        
        if( ret != KErrNone )
            {
            self->iManager.UnloadWidgetFromPluginL( *plugin, ETrue );            
            self->iShowContentRemoved = ETrue;
            }        
        }
    else
        {                
        TInt reason( self->VirginPublishers() ? 
            EAiFwSystemStartup : EAiFwPageStartup ); 
        
        self->iVirginPublishers = EFalse;
        
        self->iLoader->Cancel();
        
        self->iLoadIndex = 0;
        
        for ( TInt i = 0; i < self->iContentSourceNodes.Count(); i++ )
            {            
            CXnNodeAppIf& plugin( self->iContentSourceNodes[i]->AppIfL() ); 
                    
            appui->LoadPublisher( plugin, reason );                          
            }                
                             
        if ( self->iShowContentRemoved )
            {
            self->ShowContentRemovedError();
            self->iShowContentRemoved = EFalse;
            }      
                
        // Fire UI ready blindly here, it will be handled in AiFw if needed
        appui->HandleUiReadyEventL();
        }                  
            
    __PRINTS( "*** CXnViewData::DoLoadPublishersL - done" );
    
    return KErrNone;       
    }

// -----------------------------------------------------------------------------
// CXnViewData::DestroyPublishers
// Remove data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnViewData::DestroyPublishers( TInt aReason )
    {
    __PRINTS( "*** CXnViewData::DestroyPublishers" );
    
    if ( Occupied() )
        {
        // If not all plugins loaded yet               
        iLoader->Cancel();                                  
        
        TRAP_IGNORE( DoDestroyPublishersL( aReason ) );
        
        User::Heap().Compress();        
        }
    
    __PRINTS( "*** CXnViewData::DestroyPublishers - done" );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DoDestroyPublishersL
// Remove data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnViewData::DoDestroyPublishersL( TInt aReason )
    {
    __TIME_MARK( time );
    
    // Create list of data plugins to be removed    
    RPointerArray< CXnNode > publishers;
    CleanupClosePushL( publishers );
    
    TRAP_IGNORE( ContentSourceNodesL( publishers ) );
    
    for ( TInt i = 0; i < publishers.Count(); i++ )
        {
        // Destruction is synchronous
        iManager.AppUiAdapter().DestroyPublisher( 
            publishers[i]->AppIfL(), aReason );        
        }
        
    CleanupStack::PopAndDestroy( &publishers );
    
    __TIME_ENDMARK( "CXnViewData::DoDestroyPublishersL, done", time );
    }

// End of file
