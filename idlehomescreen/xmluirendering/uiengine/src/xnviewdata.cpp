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

// User includes
#include "xncomposer.h"
#include "xnodtparser.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xncontroladapter.h"
#include "xndomnode.h"
#include "xnnode.h"
#include "xnoomsyshandler.h"

// Constants
_LIT8( KStateConfirmed, "Confirmed" );
_LIT8( KStateError, "Error" );
_LIT8( KLockingStatusLocked, "locked" );

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
// CXnViewData::SetActiveL()
// 
// -----------------------------------------------------------------------------
//
void CXnViewData::SetActiveL( TBool aActive )
    {    
    TBool active( Active() );
    
    if ( active == aActive || !Occupied() )
        {
        return;
        }

    if ( aActive )
        {     
        iFlags.Clear( EIsInitial );

        iFlags.Set( EIsActive );

        LoadDataPluginsL();
        }
    else
        {                              
        DestroyDataPluginsL();

        iFlags.Clear( EIsActive );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::LoadL()

// -----------------------------------------------------------------------------
//
void CXnViewData::LoadL()
    {               
    if ( Occupied() || PluginState().CompareF( KStateError ) == 0 )
        {
        return;
        }
       
    if ( !CXnOomSysHandler::HeapAvailable( VIEW_MIN_MEM ) )
        {
        ViewManager().OomSysHandler().HandlePotentialOomL();        
        return;
        }
        
    TRAPD( error,
                        
        if( iManager.Composer().ComposeViewL( *this ) == KErrNone )
            {
            iManager.Parser().LoadViewL( *this );
            }                  
        );                              

    if ( error || !Occupied() )
        {                       
        if( error == KErrNoMemory )
            {
            ViewManager().OomSysHandler().HandlePotentialOomL();
            }        
        
        // Set error state
        iFlags.Clear( EIsInitial );
        
        SetPluginStateL( KStateError );               
        }    
    else
        {
        // Must confirm here, as widgets loading may fail, which  
        // then results his view to be doomed unnceccesarily
        SetPluginStateL( KStateConfirmed );
        
        // Load view initial widgets
        for ( TInt i = 0; i < iPluginsData.Count(); i++ )
            {
            if ( iPluginsData[i]->PluginId() != KNullDesC8 )
                {
                iPluginsData[i]->LoadL();                        
                }
            }  
        
        if ( Active() )
            {
            LoadDataPluginsL();
            }
        }       
    }

// -----------------------------------------------------------------------------
// CXnViewData::Destroy()

// -----------------------------------------------------------------------------
//
void CXnViewData::Destroy()
    {
    if ( Occupied() )
        {
        TRAP_IGNORE( DestroyDataPluginsL() );
        
        iManager.Parser().DestroyView( *this );
        }
    delete iBgImage;
    iBgImage = NULL;
    Flush();  
    delete iBgImagePath;
    iBgImagePath = NULL;
    }

// -----------------------------------------------------------------------------
// CXnViewData::Plugin()
// Finds plugin or view data based on node
// -----------------------------------------------------------------------------
//
CXnPluginData& CXnViewData::Plugin( CXnNode* aNode )
    {    
    if ( aNode->ViewNodeImpl() )
        {
        // Reached view, return self
        if ( Node()->LayoutNode() == aNode )
            {
            return *this;
            }
        }

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        if ( iPluginsData[i]->Owner()->LayoutNode() == aNode )
            {
            return *iPluginsData[i];
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
    delete iBgImagePath;
    iBgImagePath = NULL;
    iBgImagePath = aFileName.AllocL();
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
    // Get my data provider nodes
    CXnPluginData::ContentSourceNodesL( aList );

    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        // And appearance nodes which my plugin holds
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
// CXnViewData::LoadDataPluginsL
// Loads data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnViewData::LoadDataPluginsL()
    {
    if( Occupied() && Active() )
        {               
        // Load own plugins first, and after that data plugins for widgets
        CXnPluginData::LoadDataPluginsL();
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DataPluginsLoadCompletedL
// Indicates that all data plugins are loaded
// -----------------------------------------------------------------------------
//
void CXnViewData::DataPluginsLoadCompletedL( TInt aStatus )
    {       
    if ( aStatus == KErrNone )
        {
        for( TInt i = 0; i < iPluginsData.Count(); i++ )
            {
            iPluginsData[i]->LoadDataPluginsL();
            }                
        }
    
    CXnPluginData::DataPluginsLoadCompletedL( aStatus );
    }

// -----------------------------------------------------------------------------
// CXnViewData::DestroyDataPluginsL
// Remove data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnViewData::DestroyDataPluginsL()
    {
    CXnPluginData::DestroyDataPluginsL();
    
    for( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        iPluginsData[i]->DestroyDataPluginsL();
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DataPluginsLoaded()
// -----------------------------------------------------------------------------
//
TBool CXnViewData::DataPluginsLoaded() const
    {
    TBool loaded( CXnPluginData::DataPluginsLoaded() );
    
    for( TInt i = 0; loaded && i < iPluginsData.Count(); i++ )
        {
        CXnPluginData* plugin( iPluginsData[i] );
        
        if( plugin->Occupied() )
            {
            loaded = plugin->DataPluginsLoaded();
            }               
        }
    
    return loaded;    
    }

// -----------------------------------------------------------------------------
// Sets view's locking_status attribute ("locked"/"none")
// -----------------------------------------------------------------------------
//
void CXnViewData::SetLockingStatus( const TDesC8& aLockingStatusString )
    {
    if( ( aLockingStatusString != KNullDesC8 ) && 
            ( aLockingStatusString.Match( KLockingStatusLocked ) == 0 ) )
        {
        iFlags.Clear( EIsRemovable );
        }
    else
        {
        iFlags.Set( EIsRemovable );
        }
    }
// End of file
