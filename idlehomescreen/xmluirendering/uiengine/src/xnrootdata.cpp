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
* Description:  Data class to hold application root configuration info
*
*/

// System includes


// User includes
#include "xnviewmanager.h"
#include "xnviewdata.h"

#include "xnappuiadapter.h"
#include "xncontroladapter.h"
#include "xnuiengine.h"
#include "xnecomhandler.h"
#include "xncomposer.h"
#include "xnodtparser.h"

#include "xndomdocument.h"
#include "xndomnode.h"
#include "xnodt.h"

#include "xnnode.h"
#include "xntype.h"

#include "xnpanic.h"

#include "xnplugindata.h"
#include "xnrootdata.h"

#include "debug.h"

// Constants
const TInt KScheduleInterval( 2000000 );

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnRootData::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnRootData* CXnRootData::NewL( CXnViewManager& aManager, 
    TUid aApplicationUid )
    {
    CXnRootData* self = CXnRootData::NewLC( aManager, aApplicationUid );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnRootData::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnRootData* CXnRootData::NewLC( CXnViewManager& aManager, 
    TUid aApplicationUid )
    {
    CXnRootData* self = new ( ELeave ) CXnRootData( aManager, aApplicationUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnRootData::CXnRootData()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnRootData::CXnRootData( CXnViewManager& aManager, TUid aApplicationUid )     
    : CXnPluginData( aManager ), iApplicationUid( aApplicationUid )          
    {
    iFlags.Clear( EIsRemovable );
    }

// -----------------------------------------------------------------------------
// CXnRootData::~CXnRootData()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnRootData::~CXnRootData()
    {
    if ( iLoadTimer )
        {
        iLoadTimer->Cancel();
        }
    
    delete iLoadTimer;
    
    if ( iDestroyTimer )
        {
        iDestroyTimer->Cancel();
        }
    
    delete iDestroyTimer;
    
    iViewsToDestroy.ResetAndDestroy();
    
    iPluginsData.ResetAndDestroy();
    
    delete iODT->DomDocument().RootNode()->LayoutNode();
    
    delete iODT;

    delete iParser;
    delete iEcomHandler;
                  
    User::Heap().Compress();
    }

// -----------------------------------------------------------------------------
// CXnRootData::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnRootData::ConstructL()
    {   
    CXnPluginData::ConstructL();
    
    iEcomHandler = CXnEcomHandler::NewL();
    
    iParser = CXnODTParser::NewL( iManager, *iEcomHandler );   
    
    iLoadTimer = CPeriodic::NewL( CActive::EPriorityLow );           
    iDestroyTimer = CPeriodic::NewL( CActive::EPriorityLow );
    }

// -----------------------------------------------------------------------------
// CXnRootData::Load()
// 
// -----------------------------------------------------------------------------
//
TInt CXnRootData::Load()
    { 
    if ( Occupied() )
        {
        return KErrInUse;
        }
    
    TInt err( KErrNone );
        
    // Load application root configuration
    TRAP( err, iODT = iManager.Composer().ComposeRootL( *this ) );
    
    if ( !iODT )
        {
        // Can't recover
        return EXnInvalidConfiguration;                       
        }
       
    if ( !err )
        {
        TRAP( err, iManager.Parser().LoadRootL( *this, iApplicationUid ) );
        }
    
    if ( !err )
        {             
        for ( TInt i = 0; i < iPluginsData.Count(); i++ )
            {
            CXnViewData* plugin( 
                static_cast< CXnViewData* >( iPluginsData[ i ] ) ); 
            
            if ( plugin->Initial() )
                {
                err = plugin->Load();
                                                    
                if ( plugin->Occupied() )
                    {
                    // Initial view is succesfully composed. Some widget plugins
                    // may have failed but it doesn't matter as those are removed.
                    // Rest of the views will be loaded asynchronously by the RunLoadL()
                    return err;
                    }     
                                
                // Initial view failed                
                iManager.RemoveFaultyView( plugin );
                
                delete plugin;
                plugin = NULL;                
                break;
                }
            }
                                                    
        // Initial view loading failed, fallback to load any of the views
        while( iPluginsData.Count() )
            {
            CXnViewData* plugin( 
                static_cast< CXnViewData* >( iPluginsData[ 0 ] ) ); 

            plugin->SetInitial();            
            
            // Ignore error
            plugin->Load();
                            
            if ( plugin->Occupied() )
                {
                // Return error because of fallback condition
                return KXnErrViewPluginFailure;
                }

            // View failed, remove it    
            iManager.RemoveFaultyView( plugin );     
            
            delete plugin;
            plugin = NULL;                       
            }        
        }
    else if ( err == KErrNoMemory )
        {
        return err;
        }    
    
    // Configuration loading failed totally
    return EXnInvalidConfiguration;
    }

// -----------------------------------------------------------------------------
// CXnRootData::Destroy()
// 
// -----------------------------------------------------------------------------
//
void CXnRootData::Destroy()
    {
    iLoadTimer->Cancel();
    iDestroyTimer->Cancel();
    
    iFlags.Set( EIsDispose );
    
    iDestroyTimer->Start( TTimeIntervalMicroSeconds32( 0 ),
                          TTimeIntervalMicroSeconds32( 0 ),
                          TCallBack( RunDestroyL, this ) );           
    }

// -----------------------------------------------------------------------------
// CXnRootData::LoadRemainingViews()
// 
// -----------------------------------------------------------------------------
//
void CXnRootData::LoadRemainingViews()
    {
    if ( iFlags.IsClear( EIsDispose ) )
        {
        if ( !AllViewsLoaded() )
            {
            iLoadForward = ETrue;
            iLoadError = KErrNone;
            
            iLoadTimer->Cancel();
            
            iLoadTimer->Start( TTimeIntervalMicroSeconds32( KScheduleInterval ),
                               TTimeIntervalMicroSeconds32( KScheduleInterval ),
                               TCallBack( RunLoadL, this ) );
            }
        else
            {
            iManager.NotifyAllViewsLoadedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnRootData::CancelLoadRemainingViews()
// 
// -----------------------------------------------------------------------------
//
void CXnRootData::CancelLoadRemainingViews()
    {    
    if ( iFlags.IsClear( EIsDispose ) && !AllViewsLoaded() )
        {
        iLoadTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CXnRootData::ODT()
// 
// -----------------------------------------------------------------------------
//
CXnODT* CXnRootData::ODT() const
    {
    return iODT;
    }

// -----------------------------------------------------------------------------
// CXnRootData::RootNode()
// 
// -----------------------------------------------------------------------------
//
CXnNode* CXnRootData::RootNode() const
    {
    return iODT->DomDocument().RootNode()->LayoutNode();
    }

// -----------------------------------------------------------------------------
// CXnRootData::Parser()
// 
// -----------------------------------------------------------------------------
//
CXnODTParser& CXnRootData::Parser() const
    {
    return *iParser;
    }

// -----------------------------------------------------------------------------
// CXnRootData::EcomHandler()
// 
// -----------------------------------------------------------------------------
//
CXnEcomHandler& CXnRootData::EcomHandler() const
    {
    return *iEcomHandler;
    }

// -----------------------------------------------------------------------------
// CXnRootData::ActiveViewData()
// Finds the active view data
// -----------------------------------------------------------------------------
//
CXnViewData& CXnRootData::ActiveViewData() const
    {    
    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        CXnViewData& plugin( static_cast< CXnViewData& >( *iPluginsData[i] ) );
        
        if ( plugin.Active() )
            {
            return plugin;
            }
        }
    
    // If active is not set, then try initial
    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        CXnViewData& plugin( static_cast< CXnViewData& >( *iPluginsData[i] ) );
        
        if ( plugin.Initial() )
            {
            return plugin;
            }
        }
   
    Panic( EXnInvalidConfiguration );

    // Remove compiler warning
    return static_cast< CXnViewData& > ( *iPluginsData[0] );
    }

// -----------------------------------------------------------------------------
// CXnRootData::PreviousViewData()
// Finds the active view data
// -----------------------------------------------------------------------------
//
CXnViewData& CXnRootData::PreviousViewData() const
    {      
    CXnViewData& active( ActiveViewData() );
    
    TInt count( iPluginsData.Count() );

    if ( count == 1 )
        {
        // Only one view available
        return active;               
        }

    TInt index( iPluginsData.Find( &active ) );
    
    if ( index == 0 )
        {
        // Return the last
        return static_cast< CXnViewData& >( *iPluginsData[ count - 1 ] );        
        }
    else
        {
        // Return the previous
        return static_cast< CXnViewData& >( *iPluginsData[ index - 1 ] );        
        }
    }

// -----------------------------------------------------------------------------
// CXnRootData::NextViewData()
// Finds the active view data
// -----------------------------------------------------------------------------
//
CXnViewData& CXnRootData::NextViewData() const
    {       
    CXnViewData& active( ActiveViewData() );
    
    TInt count( iPluginsData.Count() );

    if ( count == 1 )
        {
        // Only one view available
        return active;
        }

    TInt index( iPluginsData.Find( &active ) );

    if ( index + 1 < count )
        {
        // Return the next
        return static_cast< CXnViewData& >( *iPluginsData[ index + 1 ] );        
        }
    else
        {
        // Return the first
        return static_cast< CXnViewData& >( *iPluginsData[ 0 ] );        
        }
    }

// -----------------------------------------------------------------------------
// CXnRootData::DestroyViewData()
// Sets view data to be destroyed
// -----------------------------------------------------------------------------
//
void CXnRootData::DestroyViewData( CXnViewData* aViewData )
    {
    TInt index( iPluginsData.Find( aViewData ) );
    
    if ( index != KErrNotFound )
        {
        iPluginsData.Remove( index );
                
        if ( iViewsToDestroy.Find( aViewData ) == KErrNotFound )
            {
            iViewsToDestroy.Append( aViewData );
            
            iDestroyTimer->Cancel();
            
            iDestroyTimer->Start( TTimeIntervalMicroSeconds32( 0 ),
                                  TTimeIntervalMicroSeconds32( 0 ),
                                  TCallBack( RunDestroyL, this ) );                       
            }        
        }       
    }

// -----------------------------------------------------------------------------
// CXnRootData::AllViewsLoaded()
// 
// -----------------------------------------------------------------------------
//
TBool CXnRootData::AllViewsLoaded() const
    {
    for ( TInt i = 0; i < iPluginsData.Count(); i++ )
        {
        if ( !iPluginsData[i]->Occupied() )
            {
            return EFalse;
            }
        }
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CXnRootData::AllViewsDestroyed()
// 
// -----------------------------------------------------------------------------
//
TBool CXnRootData::AllViewsDestroyed() const
    {
    return ( iPluginsData.Count() == 0 ) && iFlags.IsSet( EIsDispose ); 
    }

// -----------------------------------------------------------------------------
// CXnRootData::RunLoadL()
// 
// -----------------------------------------------------------------------------
//
/* static */ TInt CXnRootData::RunLoadL( TAny* aAny )
    {
    __PRINTS( "*** CXnRootData::RunLoadL" );
    
    CXnRootData* self = static_cast< CXnRootData* >( aAny );
    
    if ( self->iFlags.IsSet( EIsDispose ) )
        {
        self->iLoadTimer->Cancel();
        return KErrNone;
        }

    CXnPluginData& active( self->ActiveViewData() );
    
    CXnPluginData* toLoad( NULL );
                   
    TInt index( self->iPluginsData.Find( &active ) );
    TInt count( self->iPluginsData.Count() );
            
    if ( self->iLoadForward )
        {
        self->iLoadForward = EFalse;
        
        // Start from the next one
        index = index + 1;
        
        if ( index == count )
            {
            index = 0;
            }
                     
        for ( TInt i = index; i < self->iPluginsData.Count(); i++ )
            {
            if ( !self->iPluginsData[i]->Occupied() )
                {
                toLoad = self->iPluginsData[i];
                break;
                }
            }                              
        }
    else
        {
        self->iLoadForward = ETrue;
        
        if ( index == 0 )
            {
            index = count - 1;
            }
        else
            {
            index = index - 1;
            }
        
        for ( TInt i = index; i >= 0; i-- )
            {
            if ( !self->iPluginsData[i]->Occupied() )
                {
                toLoad = self->iPluginsData[i];
                break;
                }
            }
        }
    
    TInt err( KErrNone );
    
    if ( !self->AllViewsLoaded() && toLoad )
        {                       
        err = toLoad->Load();
        
        if ( err )
            {            
            self->iLoadError = err;            
            }
        }
        
    if ( err == KXnErrViewPluginFailure )
        {                
        // Remove the view        
        self->iManager.RemoveFaultyView( static_cast< CXnViewData* >( toLoad ) );
        
        delete toLoad;
        toLoad = NULL;        
        }
    
    if ( self->AllViewsLoaded() || err == KErrNoMemory )
        {            
        self->iLoadTimer->Cancel();
        
        // Show the error note only once
        self->iManager.HandleErrorNotes( self->iLoadError );  
        
        self->iLoadError = KErrNone;        
		self->iManager.NotifyAllViewsLoadedL();
        }
    
    __PRINTS( "*** CXnRootData::RunLoadL - done" );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnRootData::SetMaxPages( TInt32 aPages )
// 
// -----------------------------------------------------------------------------
//
void CXnRootData::SetMaxPages( TInt32 aPages )
    {
    iMaxPages = aPages;
    }

// -----------------------------------------------------------------------------
// CXnRootData::MaxPages()
// 
// -----------------------------------------------------------------------------
//
TInt32 CXnRootData::MaxPages()
    {
    return iMaxPages;
    }

// -----------------------------------------------------------------------------
// CXnRootData::RunDestroyL()
// 
// -----------------------------------------------------------------------------
//
/* static */ TInt CXnRootData::RunDestroyL( TAny* aAny )
    {
    __PRINTS( "*** CXnRootData::RunDestroyL" );
    
    CXnRootData* self = static_cast< CXnRootData* >( aAny );
    
    if( self->iFlags.IsSet( EIsDispose ) )
        {
        for ( TInt i = 0; i < self->iViewsToDestroy.Count(); i++ )
            {            
            if ( self->iPluginsData.Find( 
                self->iViewsToDestroy[i] ) == KErrNotFound )
                {
                // These must be deleted also
                self->iPluginsData.AppendL( self->iViewsToDestroy[i] );
                }            
            }
        
        self->iViewsToDestroy.Reset();
        
        if ( self->iPluginsData.Count() > 0 )
            {
            CXnPluginData* toDestroy( self->iPluginsData[0] );
            
            delete toDestroy;
            toDestroy = NULL;
            
            self->iPluginsData.Remove( 0 );

            User::Heap().Compress();            
            }
        
        if ( self->AllViewsDestroyed() )
            {
            // All done
            delete self;
            }                
        }
    else
        {
        if ( self->iViewsToDestroy.Count() > 0 )
            {
            CXnPluginData* toDestroy( self->iViewsToDestroy[0] );
            
            delete toDestroy;
            toDestroy = NULL;
            
            self->iViewsToDestroy.Remove( 0 );

            User::Heap().Compress();                                              
            }
        
        if ( self->iViewsToDestroy.Count() == 0 )
            {
            if ( self->iFlags.IsClear( EIsDispose ) )
                {
                self->iDestroyTimer->Cancel();
                }            
            }
        }
    
    __PRINTS( "*** CXnRootData::RunDestroyL - done" );
    
    return KErrNone;
    }

// End of file

