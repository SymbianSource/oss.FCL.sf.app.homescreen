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
* Description:  Data class to hold widget info
*
*/

// System includes
#include <utf.h>
#include <aifwdefs.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

// User includes
#include <xnuiengine.rsg>
#include "xnappuiadapter.h"
#include "xncomposer.h"
#include "xnodtparser.h"
#include "xnresource.h"
#include "xnodt.h"
#include "xnnode.h"
#include "xnplugindata.h"
#include "xnviewdata.h"
#include "xnviewmanager.h"
#include "xnoomsyshandler.h"
#include "xnpanic.h"

#include "debug.h"

// Constants
_LIT8( KLockingStatusLocked, "locked" );

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnPluginData::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnPluginData* CXnPluginData::NewL( CXnPluginData& aParent )    
    {
    CXnPluginData* self = CXnPluginData::NewLC( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnPluginData::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnPluginData* CXnPluginData::NewLC( CXnPluginData& aParent )    
    {
    CXnPluginData* self = new ( ELeave ) CXnPluginData( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnPluginData::CXnPluginData()
// C++ constructor
// -----------------------------------------------------------------------------
//
CXnPluginData::CXnPluginData( CXnPluginData& aParent )     
    : iParent( &aParent ), iManager( iParent->ViewManager() )       
    {
    // Plugin data is removable by default    
    iFlags.Set( EIsRemovable );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::CXnPluginData()
// C++ constructor
// -----------------------------------------------------------------------------
//
CXnPluginData::CXnPluginData( CXnViewManager& aManager )     
    : iParent( NULL ), iManager( aManager )
    {
    // This constructor overload is used by CXnRootData
    }

// -----------------------------------------------------------------------------
// CXnPluginData::~CXnPluginData()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnPluginData::~CXnPluginData()
    {
    if ( iLoader )
        {
        iLoader->Cancel();
        }

    delete iLoader;
               
    Flush();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnPluginData::ConstructL()
    {    
    iLoader = CPeriodic::NewL( CActive::EPriorityStandard );    
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Load()
// -----------------------------------------------------------------------------
//
TInt CXnPluginData::Load()
    {        
    if ( Occupied() )
        {
        return KErrInUse;
        }
   
    if ( !CXnOomSysHandler::HeapAvailable( WIDGET_MIN_MEM ) )
        {                
        return KErrNoMemory;
        }

    iFlags.Clear( EIsEmpty );
              
    TInt err( KErrNone );
    
    TRAP( err, err = iManager.Composer().ComposeWidgetL( *this ) );
    
    if ( err == KErrNone )
        {
        TRAP( err, iManager.Parser().LoadWidgetL( *this ) );       
        }
    
    if ( err == KErrNone )
        {
        // Mark publishers as virgin
        iVirginPublishers = ETrue;
        
        // Succesfully composed, try schedule publishers' loading
        LoadPublishers();                  
        }    
    else if ( err == KXnErrPluginFailure )
        {
        // Widget's configuration is broken, remove it
        TRAP_IGNORE( iManager.UnloadWidgetFromPluginL( *this, ETrue ) );                        
        }      
        
    return err;
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Destroy()
// -----------------------------------------------------------------------------
//
void CXnPluginData::Destroy()
    {       
    if ( Occupied() )
        {
        DestroyPublishers();
        
        TRAP_IGNORE( iManager.Parser().DestroyWidgetL( *this ) );                                       
        }
    
    Flush();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::LoadPublishers
// Loads data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnPluginData::LoadPublishers()
    {                     
    if ( !Active() || !Occupied() || iContentSourceNodes.Count() == 0 )
        {               
        return;
        }
                         
    iLoader->Cancel();
                  
    iLoader->Start( TTimeIntervalMicroSeconds32( 50 ),
                    TTimeIntervalMicroSeconds32( 50 ),
                    TCallBack( PeriodicEventL, this ) );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::PeriodicEventL()
// 
// -----------------------------------------------------------------------------
//
/* static */ TInt CXnPluginData::PeriodicEventL( TAny* aAny )
    {
    CXnPluginData* self = static_cast< CXnPluginData* >( aAny );
    
    self->iLoader->Cancel();
    
    TInt reason( EAiFwPluginStartup );
    
    if ( self->LoadPublishers( reason ) != KErrNone )
        {
        self->iManager.UnloadWidgetFromPluginL( *self, ETrue );
        
        self->ShowContentRemovedError();
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnPluginData::LoadPublishers()
// 
// -----------------------------------------------------------------------------
//
TInt CXnPluginData::LoadPublishers( TInt aReason )
    {           
    __PRINTS( "*** CXnPluginData::LoadPublishers" );
    
    TInt err( KErrNone );

    TRAP( err,
        for ( TInt i = 0; i < iContentSourceNodes.Count(); i++ )
            {            
            CXnNodeAppIf& plugin( iContentSourceNodes[i]->AppIfL() );
                        
            TInt retval(
                iManager.AppUiAdapter().LoadPublisher( plugin, aReason ) );
                            
            if ( !err )
                {
                err = retval;
                }
            }
        );

    iVirginPublishers = EFalse;
    
    if ( !Removable() )
        {
        // Not allowed to remove even it fails
        return KErrNone;
        }
    
    __PRINTS( "*** CXnPluginData::LoadPublishers - done" );
    
    return err;        
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DestroyPublishers
// Remove data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnPluginData::DestroyPublishers()
    {    
    __PRINTS( "*** CXnPluginData::DestroyPublishers" );
    
    if ( Occupied() )
        {
        // If not all plugins loaded yet               
        iLoader->Cancel();                                  
        
        TRAP_IGNORE( DoDestroyPublishersL() );
        
        User::Heap().Compress();
        }    
    
    __PRINTS( "*** CXnPluginData::DestroyPublishers - done" );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::VirginPublishers
// 
// -----------------------------------------------------------------------------
//
TBool CXnPluginData::VirginPublishers() const
    {
    return iVirginPublishers;
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DoDestroyPublishersL
// Remove data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnPluginData::DoDestroyPublishersL()
    {
    __TIME_MARK( time );
    
    for ( TInt i = 0; i < iContentSourceNodes.Count(); i++ )
        {
        CXnNodeAppIf& plugin( iContentSourceNodes[i]->AppIfL() );
        
        // Destruction is synchronous
        iManager.AppUiAdapter().DestroyPublisher( 
            plugin, EAiFwPluginShutdown );        
        }
    
    __TIME_ENDMARK( "CXnPluginData::DoDestroyPublishersL, done", time );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetConfigurationIdL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetConfigurationIdL( const TDesC8& aConfigurationId )
    {
    delete iConfigurationId;
    iConfigurationId = NULL;

    iConfigurationId = aConfigurationId.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetPluginIdL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPluginIdL( const TDesC8& aPluginId )
    {
    delete iPluginId;
    iPluginId = NULL;

    iPluginId = aPluginId.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetPluginUidL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPluginUidL( const TDesC8& aPluginUid )
    {
    delete iPluginUid;
    iPluginUid = NULL;

    iPluginUid = aPluginUid.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetPluginNameL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPluginNameL( const TDesC8& aPluginName )
    {
    delete iPluginName;
    iPluginName = NULL;

    iPluginName = aPluginName.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetPublisherName ()
// Set the publisher name
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPublisherNameL( const TDesC8& aPublisherName )
    {
    delete iPublisherName;
    iPublisherName = NULL;
    
    iPublisherName = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( aPublisherName );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetPublisherName()
// Set the publisher name
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPublisherNameL( const TDesC& aPublisherName )
    {
    delete iPublisherName;
    iPublisherName = NULL;
    
    iPublisherName = aPublisherName.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetPluginTypeL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPluginTypeL( const TDesC8& aPluginType )
    {
    delete iPluginType;
    iPluginType = NULL;

    iPluginType = aPluginType.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetResources()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetResources( CArrayPtrSeg< CXnResource >* aResources )
    {    
    if ( iResources )
        {
        iResources->ResetAndDestroy();
        
        delete iResources;
        iResources = NULL;
        }
    
    iResources = aResources;    
    }

// -----------------------------------------------------------------------------
// CXnPluginData::ResourcesL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::ResourcesL( CArrayPtrSeg< CXnResource >& aList ) const
    {    
    for ( TInt i = 0; iResources && i < iResources->Count(); i++ )
        {
        aList.AppendL( iResources->At(i) );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetControlL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetControlL( CXnNode* aNode )
    {
    CXnControlAdapter* control( aNode->Control() );

    if ( control )
        {
        User::LeaveIfError( iControls.InsertInAddressOrder( control ) );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::ControlsL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::ControlsL( RPointerArray< CXnControlAdapter >& aList ) const
    {
    for ( TInt i = 0; i < iControls.Count(); i++ )
        {
        aList.AppendL( iControls[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetContentSourceNodeL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetContentSourceNodeL( CXnNode* aNode )
    {
    User::LeaveIfError( iContentSourceNodes.InsertInAddressOrder( aNode ) );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::ContentSourceNodesL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::ContentSourceNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iContentSourceNodes.Count(); i++ )
        {
        aList.AppendL( iContentSourceNodes[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetAppearanceNodeL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetAppearanceNodeL( CXnNode* aNode )
    {
    User::LeaveIfError( iAppearanceNodes.InsertInAddressOrder( aNode ) );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::AppearanceNodesL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::AppearanceNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iAppearanceNodes.Count(); i++ )
        {
        aList.AppendL( iAppearanceNodes[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetInitialFocusNodeL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetInitialFocusNodeL( CXnNode* aNode )
    {
    User::LeaveIfError( iInitialFocusNodes.InsertInAddressOrder( aNode ) );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::InitialFocusNodesL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::InitialFocusNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iInitialFocusNodes.Count(); i++ )
        {
        aList.AppendL( iInitialFocusNodes[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Flush()
// Flushes this plugins data
// -----------------------------------------------------------------------------
//
void CXnPluginData::Flush()
    {
    // Don't touch to iOwner, because this plugin might be reused later
           
    iFlags.ClearAll();
    
    // This is default
    iFlags.Set( EIsRemovable );    
           
    iNode = NULL;
    
    delete iConfigurationId;
    iConfigurationId = NULL;

    delete iPluginId;
    iPluginId = NULL;

    delete iPluginUid;
    iPluginUid = NULL;

    delete iPluginName;
    iPluginName = NULL;

    delete iPluginType;
    iPluginType = NULL;

    delete iPublisherName;
    iPublisherName = NULL;

    if ( iResources )
        {
        iResources->ResetAndDestroy();
        delete iResources;
        iResources = NULL;
        }
        
    iControls.Reset();
    iContentSourceNodes.Reset();
    iAppearanceNodes.Reset();
    iInitialFocusNodes.Reset();
    iPopupNodes.Reset();
    iPluginsData.ResetAndDestroy();
    
    User::Heap().Compress();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Empty()
// 
// -----------------------------------------------------------------------------
//
TBool CXnPluginData::Empty() const
    {       
    return ( iFlags.IsSet( EIsEmpty ) ? ETrue : EFalse );    
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetEmpty()
// 
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetEmptyL( const TDesC8& aPluginId ) 
    {       
    if ( Occupied() )
        {
        User::Leave( KErrInUse );
        }
    
    HBufC8* id( aPluginId.AllocL() );
    
    Flush();

    // Takes ownership
    iPluginId = id;
           
    iFlags.Set( EIsEmpty );    
    }
    
// -----------------------------------------------------------------------------
// CXnPluginData::SetIsDisplayingPopup()
// 
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetIsDisplayingPopup ( TBool aVisible, CXnNode* aNode )
    {    
    if ( aVisible )
        {
        iPopupNodes.InsertInAddressOrder( aNode );
        }
    else
        {
        TInt index( iPopupNodes.Find( aNode ) );

        if ( index != KErrNotFound )
            {
            iPopupNodes.Remove( index );
            }
        }
    }

//------------------------------------------------------------------------------
// CXnPluginData::IsDisplayingPopup()      
//
//------------------------------------------------------------------------------
//    
TBool CXnPluginData::IsDisplayingPopup() const
    {
    return ( iPopupNodes.Count() > 0 );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::PopupNodesL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::PopupNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iPopupNodes.Count(); i++ )
        {
        aList.AppendL( iPopupNodes[i] );
        }
    }

//------------------------------------------------------------------------------
// CXnPluginData::ShowContentRemovedError()      
//
//------------------------------------------------------------------------------
//    
void CXnPluginData::ShowContentRemovedError()
    {
    TRAP_IGNORE( DoShowContentRemovedErrorL() );
    }

//------------------------------------------------------------------------------
// CXnPluginData::DoShowContentRemovedErrorL()      
//
//------------------------------------------------------------------------------
//    
void CXnPluginData::DoShowContentRemovedErrorL()
    {
    HBufC* msg( StringLoader::LoadLC( R_QTN_HS_ERROR_WIDGETS_REMOVED ) );
        
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
    CleanupStack::PushL( note );
    
    note->ExecuteLD( *msg );
    
    CleanupStack::Pop( note );
    CleanupStack::PopAndDestroy( msg );                                       
    }

//------------------------------------------------------------------------------
// CXnPluginData::ShowOutOfMemErrorL()      
//
//------------------------------------------------------------------------------
//    
void CXnPluginData::ShowOutOfMemError()
    {
    TRAP_IGNORE( ViewManager().OomSysHandler().HandlePotentialOomL() );
    }

// -----------------------------------------------------------------------------
// CXnViewData::SetLockingStatus
// 
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetLockingStatus( const TDesC8& aStatus )
    {
    if ( aStatus.CompareF( KLockingStatusLocked ) == 0 )
        {
        iFlags.Clear( EIsRemovable );
        }
    else
        {
        iFlags.Set( EIsRemovable );
        }
    }

// End of file
