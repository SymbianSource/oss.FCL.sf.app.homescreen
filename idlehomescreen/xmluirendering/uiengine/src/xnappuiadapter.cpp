/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for Avkon AppUi adapter
*
*/

// System includes
#include <aknview.h>
#include <akntoolbar.h>
#include <avkon.rsg>

// User includes
#include <hspublisherinfo.h>
#include "xnappuiadapter.h"
#include "xnappuiadapterimpl.h"
#include "xnviewadapter.h"
#include "xnuiengineappif.h"
#include "xnfocuscontrol.h"
#include "hscontentcontrolfactory.h"
#include "xnviewadapter.h"
#include "xnnode.h"
#include "xnviewswitcher.h"

#include "xuikon_builds_cfg.hrh"
#include "debug.h"

_LIT8( KActivateDefaultView, "activatedefault" );

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::CXnAppUiAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnAppUiAdapter::CXnAppUiAdapter( TUid aApplicationUid )
    : iExitingApp( EFalse ), iApplicationUid( aApplicationUid )
    {    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ConstructL
// Second-phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::ConstructL()
    {
    __TICK( "CXnAppUiAdapter::ConstructL" );    
    __TIME_MARK( time );

    __PRINTS( "*** CXnAppUiAdapter::ConstructL - BaseConstructL" );
    __TIME_MARK( time2 );
        
    EnableLocalScreenClearer( EFalse );
    
    CAknViewAppUi::BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    
    CAknToolbar* toolbar( CurrentFixedToolbar() );

    if ( toolbar )
        {
        toolbar->SetToolbarVisibility( EFalse );

        toolbar->DisableToolbarL( ETrue );
        }

    CEikStatusPane* sp( StatusPane() );
    if ( sp )
        {
        sp->EnableTransparent( ETrue );
        }

    __TIME_ENDMARK( "CXnAppUiAdapter::ConstructL - BaseConstructL, done", time2 );
    
    iImpl = CXnAppUiAdapterImpl::NewL( iApplicationUid, *this );
    iImpl->ConstructL();

    iViewSwitcher = CXnViewSwitcher::NewL(); 
    
    __TIME_ENDMARK( "CXnAppUiAdapter::ConstructL, done", time );
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
EXPORT_C CXnAppUiAdapter::~CXnAppUiAdapter()
    {       
    delete iViewSwitcher;

    delete iUiEngineAppIf;
    
    delete iImpl;    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HandleResourceChangeL
// Handles resource changes
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::HandleResourceChangeL( TInt aType )
    {
    __PRINTS( "*** CXnAppUiAdapter::HandleResourceChangeL" );
    __TIME_MARK( time );

    CAknViewAppUi::HandleResourceChangeL( aType );

    if ( iImpl )
        {
        iImpl->HandleResourceChangeL( aType );
        }

    __TIME_ENDMARK( "CXnAppUiAdapter::HandleResourceChangeL, done", time );
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::UiEngine
// Get UI engine proxy
// -----------------------------------------------------------------------------
//
EXPORT_C TXnUiEngineAppIf* CXnAppUiAdapter::UiEngineL()
    {
    if ( !iUiEngineAppIf )
        {
        iUiEngineAppIf = new ( ELeave ) TXnUiEngineAppIf( UiEngine() );
        }
    
    return iUiEngineAppIf;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HandleXuikonEventL
// Handle callbacks from the engine
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::HandleXuikonEventL(
    CXnNodeAppIf& /*aOrigin*/,
    CXnNodeAppIf& /*aTrigger*/,
    CXnDomNode& /*aTriggerDefinition*/,
    CXnDomNode& /*aEvent*/ )
    {
    // Default empty implementation
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::LoadPublisher
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::LoadPublisher( 
    const TAiFwPublisherInfo& /*aPublisher*/ )     
    {
    // Default empty implementation    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::DestroyPublisher
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::DestroyPublisher( 
    const TAiFwPublisherInfo& /*aPublisher*/ )     
    {
    // Default empty implementation    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::DynInitMenuItemL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnAppUiAdapter::DynInitMenuItemL( const TDesC& /*aItemType*/,          
    RPointerArray< CXnNodeAppIf >* /*aList*/ )
    {
    // Default empty implementation
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::View
//
// -----------------------------------------------------------------------------
//    
EXPORT_C CAknView& CXnAppUiAdapter::View() const
    {
    return iImpl->ViewAdapter();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HandleEnterEditModeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::HandleEnterEditModeL( TBool /*aEnter*/ )
    {
    // Default empty implementation
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HandleEventL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::HandleEventL( const TDesC& /*aEvent*/,
    CXnNodeAppIf& /*aDestination*/ )
    {
    // Default empty implementation
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::PublisherInfo
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnAppUiAdapter::PublisherInfo( CXnNodeAppIf& /*aNode*/, 
    THsPublisherInfo& /*aInfo*/ )
    {
    // Default empty implementation    
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::PublisherInfo
//
// -----------------------------------------------------------------------------
//
TInt CXnAppUiAdapter::PublisherInfo( CXnNode& aNode, 
    THsPublisherInfo& aInfo )
    {
    CXnNodeAppIf* node( NULL );
    
    TRAPD( err, node = &aNode.AppIfL() );
    
    if ( node )
        {
        err = PublisherInfo( *node, aInfo );    
        }
           
    return err;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::RemoveViewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::RemoveViewL( CAknView& aView )
    {
	if ( iExitingApp ) return; // framework will destroy view
    TVwsViewId activeViewId(KNullUid,KNullUid);
    TBool activateDefaultView( EFalse );
    if ( GetActiveViewId( activeViewId ) == KErrNone &&
        activeViewId.iViewUid == aView.Id() )
        {
        // currently active, deactivate        
        activateDefaultView = ETrue;
        DeactivateActiveViewIfOwnerMatchL();
        }
    
    if ( iView == &aView ) { iView = NULL; }
    CCoeAppUi::DeregisterView( aView );
    
    const TInt count( iViews->Count() );       
    for ( TInt i = 0; i < count; ++i )
        {
        CAknView* view( iViews->At( i ) );       
        if ( view == &aView && 
            view->Id() == aView.Id() )
            {
            iViews->Delete( i );           
            delete view;
            view = NULL;
            break;
            }
        }
    
    if ( activateDefaultView )
        {
        // check which view is active now.
        activeViewId = TVwsViewId(KNullUid,KNullUid);
        GetActiveViewId( activeViewId );
    
        TVwsViewId defaultViewId( KNullUid,KNullUid );    
        // activate default if needed
        if ( GetDefaultViewId( defaultViewId ) == KErrNone && 
            activeViewId != defaultViewId )
            {
            ActivateViewL( defaultViewId );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::PrepareToExit
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::PrepareToExit()
    {
    iExitingApp = ETrue;
    
    if ( iImpl )
        {
        iImpl->UiStateListener().PrepareToExit();

        iImpl->ViewAdapter().PrepareToExit();
        
        iImpl->HsContentControlFactory()->PrepareToExit();
        }

    CAknViewAppUi::PrepareToExit();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HandleUiReadyEventL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::HandleUiReadyEventL()
    {
    // Default empty implementation
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ProcessMessageL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::ProcessMessageL( TUid aUid,
    const TDesC8& /*aParams*/ )
    {
    if ( aUid.iUid == KUidApaMessageSwitchOpenFileValue )
        {
        // activate default homescreen view.
        // customcontrol is set so that window group order is not changed
        SetCustomControl( 1 );
        TRAP_IGNORE( ActivateLocalViewL(
                View().Id(), TUid::Null(), KActivateDefaultView() ) );
        SetCustomControl( 0 );
        }
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ReloadUiL
//
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapter::ReloadUiL()
    {    
    iImpl->ReloadUiL();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::UiEngine
// 
// -----------------------------------------------------------------------------
//
CXnUiEngine& CXnAppUiAdapter::UiEngine() const
    {
    return iImpl->UiEngine();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ViewManager
// 
// -----------------------------------------------------------------------------
//
CXnViewManager& CXnAppUiAdapter::ViewManager() const
    {
    return iImpl->ViewManager();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ViewAdapter
// 
// -----------------------------------------------------------------------------
//
CXnViewAdapter& CXnAppUiAdapter::ViewAdapter() const
    {
    return iImpl->ViewAdapter();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::UiStateListener
// 
// -----------------------------------------------------------------------------
//
CXnUiStateListener& CXnAppUiAdapter::UiStateListener() const
    {
    return iImpl->UiStateListener();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HsContentController
//
// -----------------------------------------------------------------------------
//
MHsContentControlUi* CXnAppUiAdapter::HsContentController( 
    const TDesC8& aType ) const
    {
    return iImpl->HsContentController( aType );
    }
	
// -----------------------------------------------------------------------------
// CXnAppUiAdapter::EffectManager
//
// -----------------------------------------------------------------------------
//
CXnEffectManager* CXnAppUiAdapter::EffectManager() const
    {
    return iImpl->EffectManager();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HsContentControlSrv
//
// -----------------------------------------------------------------------------
//
MHsContentControl* CXnAppUiAdapter::HsContentControlSrv() const
    {
    return iImpl->HsContentControlSrv();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ItemActivator
// 
// -----------------------------------------------------------------------------
//
CXnItemActivator& CXnAppUiAdapter::ItemActivator() const
    {
    return iImpl->ItemActivator();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::FocusShown
//
// -----------------------------------------------------------------------------
//
TBool CXnAppUiAdapter::FocusShown() const
    {
    return iImpl->ViewAdapter().FocusControl().IsVisible();
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::HideFocus
//
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapter::HideFocus() 
    {
    return iImpl->ViewAdapter().FocusControl().MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ShowFocus
//
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapter::ShowFocus()
    {
    return iImpl->ViewAdapter().FocusControl().MakeVisible( ETrue );
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ViewSwitcher
//
// -----------------------------------------------------------------------------
//
CXnViewSwitcher* CXnAppUiAdapter::ViewSwitcher()
    {
    return iViewSwitcher;
    }

// End of file
