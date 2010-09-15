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
#include <aifwdefs.h>
#include "xnappuiadapter.h"
#include "xncomposer.h"
#include "xnodtparser.h"
#include "xnresource.h"
#include "xnodt.h"
#include "xnnode.h"
#include "xnplugindata.h"
#include "xnviewdata.h"
#include "xnpublisherdata.h"
#include "xnviewmanager.h"
#include "xnoomsyshandler.h"
#include "xnpanic.h"

#include "debug.h"

// Constants
_LIT8( KLockingStatusLocked, "locked" );
_LIT8( KLockingStatusPermanent, "permanent" );

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
    Flush();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnPluginData::ConstructL()
    {    
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Load()
//
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
        // Succesfully composed, try schedule publishers' loading
        LoadPublishers( EAiFwPluginStartup );                  
        }    
    else if ( err == KXnErrWidgetPluginFailure )
        {
        // Widget's configuration is broken, remove it
        TRAP_IGNORE( iManager.UnloadWidgetFromPluginL( *this, ETrue ) );                        
        }      
        
    return err;
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Destroy()
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::Destroy()
    {       
    if ( Occupied() )
        {
        DestroyPublishers( EAiFwPluginShutdown );
        
        TRAP_IGNORE( iManager.Parser().DestroyWidgetL( *this ) );                                       
        }
    
    Flush();
    }

// -----------------------------------------------------------------------------
// CXnPluginData::LoadPublishers()
// 
// -----------------------------------------------------------------------------
//
void CXnPluginData::LoadPublishers( TInt aReason )
    {           
    __PRINTS( "*** CXnPluginData::LoadPublishers" );

    if ( !Active() || !Occupied() )
        {
        __PRINTS( "*** CXnPluginData::LoadPublishers - done, !Active() || !Occupied()" );
        
        return;
        }
    
    TInt count( iPublishers.Count() );
    
    if ( count == 0 )
        {
        TRAP_IGNORE( NotifyPublisherReadyL() );
        }
    else
        {
        for ( TInt i = 0; i < count; i++ )
            {
            iPublishers[i]->Load( aReason );        
            }    
        }
    
    __PRINTS( "*** CXnPluginData::LoadPublishers - done" );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::NotifyPublisherReadyL()
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::NotifyPublisherReadyL()          
    {       
    if ( iParent )
        {
        // Forward to parent
        iParent->NotifyPublisherReadyL();
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::DestroyPublishers
// 
// -----------------------------------------------------------------------------
//
void CXnPluginData::DestroyPublishers( TInt aReason )
    {    
    __PRINTS( "*** CXnPluginData::DestroyPublishers" );
    
    if ( !Occupied() )
        {
        __PRINTS( "*** CXnPluginData::DestroyPublishers - done, !Occupied()" );
        
        return;
        }    
    
    for ( TInt i = 0; i < iPublishers.Count(); i++ )
        {
        iPublishers[i]->Destroy( aReason );        
        }
                     
    __PRINTS( "*** CXnPluginData::DestroyPublishers - done" );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::SetConfigurationIdL()
//
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
//
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
//
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
//
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
//
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
//
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
//
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
//
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
//
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
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetContentSourceNodeL( CXnNode* aNode )
    {        
    if ( !aNode )
        {
        return;
        }
    
    for ( TInt i = 0; i < iPublishers.Count(); i++ )
        {        
        if ( *iPublishers[i] == *aNode )
            {
            // Duplicate entries not allowed
            return;
            }
        }
                 
    CXnPublisherData* publisher = 
        CXnPublisherData::NewLC( *this, *aNode );       
    
    iPublishers.AppendL( publisher );    
    CleanupStack::Pop( publisher );   
    }

// -----------------------------------------------------------------------------
// CXnPluginData::ContentSourceNodesL()
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::ContentSourceNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iPublishers.Count(); i++ )
        {
        aList.AppendL( iPublishers[i]->ContentSource() );
        }
    }

// -----------------------------------------------------------------------------
// CXnPluginData::Publishers()
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::PublishersL( RPointerArray< CXnPublisherData >& aList ) const
    {
    for ( TInt i = 0; i < iPublishers.Count(); i++ )
        {
        aList.AppendL( iPublishers[i] );
        }
    }
    
// -----------------------------------------------------------------------------
// CXnPluginData::SetAppearanceNodeL()
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetAppearanceNodeL( CXnNode* aNode )
    {
    User::LeaveIfError( iAppearanceNodes.InsertInAddressOrder( aNode ) );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::AppearanceNodesL()
//
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
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetInitialFocusNodeL( CXnNode* aNode )
    {
    User::LeaveIfError( iInitialFocusNodes.InsertInAddressOrder( aNode ) );
    }

// -----------------------------------------------------------------------------
// CXnPluginData::InitialFocusNodesL()
//
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
    
    delete iDirtyRegion;
    iDirtyRegion = NULL;
    
    // clear all flags, except editable and removable
    TBool removable = iFlags.IsSet( EIsRemovable );
    TBool editable = iFlags.IsSet( EIsEditable );
    
    iFlags.ClearAll();
    
    if( removable )
        {
        iFlags.Set( EIsRemovable );
        }
    if( editable )
        {
        iFlags.Set( EIsEditable );
        }    

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
        
    iPublishers.ResetAndDestroy();
    iControls.Reset();    
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
// CXnPluginData::SetEmptyL()
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
//
// -----------------------------------------------------------------------------
//
void CXnPluginData::PopupNodesL( RPointerArray< CXnNode >& aList ) const
    {
    for ( TInt i = 0; i < iPopupNodes.Count(); i++ )
        {
        aList.AppendL( iPopupNodes[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewData::SetLockingStatus
// 
// -----------------------------------------------------------------------------
//
void CXnPluginData::SetLockingStatus( const TDesC8& aStatus )
    {
    if( aStatus.CompareF( KLockingStatusPermanent ) == 0 )
        {
        iFlags.Clear( EIsRemovable );
        iFlags.Clear( EIsEditable );
        }
    else if( aStatus.CompareF( KLockingStatusLocked ) == 0 )
        {
        iFlags.Clear( EIsRemovable );
        iFlags.Set( EIsEditable );
        }
    else
        {
        iFlags.Set( EIsRemovable );
        iFlags.Set( EIsEditable );               
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TXnDirtyRegion* CXnPluginData::CreateDirtyRegionL( CXnNode& aRootNode, 
    CCoeControl& aControl )
    {
    delete iDirtyRegion;
    iDirtyRegion = NULL;
    iDirtyRegion = new (ELeave) TXnDirtyRegion;
    iDirtyRegion->iRegion.Clear();
    iDirtyRegion->iControl = &aControl;
    iDirtyRegion->iRootNode = &aRootNode;
    iDirtyRegion->iDirtyList.Reset();
    iDirtyRegion->iLayoutControl = 0;
    return iDirtyRegion;
    }
    
// End of file
