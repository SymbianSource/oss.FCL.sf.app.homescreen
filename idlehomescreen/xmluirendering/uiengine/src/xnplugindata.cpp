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

// User includes
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

// Constants
_LIT8( KStateWaitConfirmation, "WaitForConfirmation" );
_LIT8( KStateConfirmed, "Confirmed" );
_LIT8( KStateError, "Error" );

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
    // Assume plugins ready
    iFlags.Set( EIsDataPluginsReady );
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
    // Assume plugins are ready
    iFlags.Set( EIsDataPluginsReady );    
    }

// -----------------------------------------------------------------------------
// CXnPluginData::~CXnPluginData()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnPluginData::~CXnPluginData()
    {
    RevertPluginState();
        
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
// CXnPluginData::LoadL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::LoadL()
    {        
    if ( Occupied() || PluginState().CompareF( KStateError ) == 0 )
        {
        return;
        }
   
    if ( !CXnOomSysHandler::HeapAvailable( WIDGET_MIN_MEM ) )
        {
        ViewManager().OomSysHandler().HandlePotentialOomL();        
        return;
        }

    iFlags.Clear( EIsEmpty );
    iFlags.Set( EIsRemovable );
    
    TRAPD( error,    
            
        if ( iManager.Composer().ComposeWidgetL( *this ) == KErrNone )
            {
            iManager.Parser().LoadWidgetL( *this );
            }                 
        );

    if ( Empty() )
        {
        // All done
        return;
        }
    
    if ( error || !Occupied() )
        {
        if( error == KErrNoMemory )
            {
            ViewManager().OomSysHandler().HandlePotentialOomL();
            }
        
        // Mark this plugin ready, View Manager will remove it
        DataPluginsLoadCompletedL( KErrGeneral );
        }    
    else
        {        
        if ( Active() )
            {
            // Load data plugins will set confirmation state to "wait"
            LoadDataPluginsL();  
            }
        else
            {
            // Composed succesfully to inactive page. Set confirmed
            SetPluginStateL( KStateConfirmed );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Destroy()
// -----------------------------------------------------------------------------
//
void CXnPluginData::Destroy()
    {       
    if ( Occupied() )
        {
        TRAP_IGNORE( 
                DestroyDataPluginsL();
                iManager.Parser().DestroyWidgetL( *this );
                );
        }
    
    Flush();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::LoadDataPluginsL
// Loads data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnPluginData::LoadDataPluginsL()
    {                     
    if ( !Active() || !Occupied() )
        {
        iFlags.Set( EIsDataPluginsReady );
        
        return;
        }
                         
    if ( iContentSourceNodes.Count() == 0 )
        {        
        DataPluginsLoadCompletedL( KErrNone );
        }
    else
        {
        iLoader->Cancel();
        
        iFlags.Clear( EIsDataPluginsReady );
        
        iLoadIndex = 0;               
                       
        iLoader->Start( TTimeIntervalMicroSeconds32( 0 ),
                        TTimeIntervalMicroSeconds32( 0 ),
                        TCallBack( RunL, this ) );       
        }        
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DataPluginsLoadCompletedL
// Indicates that all data plugins are loaded
// -----------------------------------------------------------------------------
//
void CXnPluginData::DataPluginsLoadCompletedL( TInt aStatus )
    {
    iFlags.Set( EIsDataPluginsReady );    

    const TDesC8& state( aStatus == KErrNone 
            ? KStateConfirmed() : KStateError() ); 
    
    SetPluginStateL( state );
            
    iManager.SetDataPluginLoadCompleteL( *this );          
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DataPluginsLoaded
// Queries whether all data plugins are loaded
// -----------------------------------------------------------------------------
//
TBool CXnPluginData::DataPluginsLoaded() const
    {
    return ( iFlags.IsSet( EIsDataPluginsReady ) ? ETrue : EFalse );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DestroyDataPluginsL
// Remove data plugins associated to the plugin
// -----------------------------------------------------------------------------
//
void CXnPluginData::DestroyDataPluginsL()
    {              
    if ( Occupied() )
        {
        iFlags.Set( EIsDataPluginsReady );
        
        if ( iLoader->IsActive() )
            {
            // Not all plugins loaded yet               
            iLoader->Cancel();
            
            RevertPluginState();            
            }
        
        // Create list of data plugins to be removed
        RPointerArray< CXnNodeAppIf > list;
        CleanupClosePushL( list );
        
        for ( TInt i = 0; i < iContentSourceNodes.Count(); i++ )
            {
            list.AppendL( &iContentSourceNodes[i]->AppIfL() );
            }
        
        // Destruction is synchronous
        iManager.AppUiAdapter().DestroyDataPluginsL( list );
        
        CleanupStack::PopAndDestroy( &list );
        
        User::Heap().Compress();
        }    
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
// CXnPluginData::SetPluginStateL()
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetPluginStateL( const TDesC8& aPluginState )     
    {    
    if ( PluginState().CompareF( aPluginState ) != 0 )
        {
        delete iPluginState;
        iPluginState = NULL;

        iPluginState = aPluginState.AllocL();

        // Tell new state to HSPS as well
        iManager.UpdatePluginStateL( *this );                           
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::RevertPluginState()
// -----------------------------------------------------------------------------
//
void CXnPluginData::RevertPluginState()
    {
    // Need to change state if it is not error
    if ( Occupied() && PluginState().CompareF( KStateError ) != 0 )
        {
        // For now on it is confirmed
        TRAP_IGNORE( SetPluginStateL( KStateConfirmed ) );                
        }                
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
    iFlags.Set( EIsDataPluginsReady );
           
    iNode = NULL;

    iLoadIndex = 0;
    
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

    delete iPluginState;
    iPluginState = NULL;

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
    iFlags.Clear( EIsRemovable );
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
// CXnPluginData::RunL()
// 
// -----------------------------------------------------------------------------
//
/* static */ TInt CXnPluginData::RunL( TAny* aAny )
    {
    CXnPluginData* self = static_cast< CXnPluginData* >( aAny );
    
    self->iLoader->Cancel();
    
    RPointerArray< CXnNodeAppIf >list;
    CleanupClosePushL( list );

    for ( TInt i = 0; i < self->iContentSourceNodes.Count(); i++ )
        {
        list.AppendL( &self->iContentSourceNodes[i]->AppIfL() );
        }
        
    // State is "wait" until data plugins are loaded
    self->SetPluginStateL( KStateWaitConfirmation );
       
    TRAPD( err, self->iManager.AppUiAdapter().LoadDataPluginsL( list ) );

    if ( err && self->Removable() )
        {               
        self->DataPluginsLoadCompletedL( err );            
        }
    else
        {
        self->DataPluginsLoadCompletedL( KErrNone );                 
        }
    
    CleanupStack::PopAndDestroy( &list );
        
    return KErrNone;       
    }

// End of file
