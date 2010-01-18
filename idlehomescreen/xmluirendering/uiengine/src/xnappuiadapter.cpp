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

// User includes
#include "xnappuiadapter.h"
#include "xnappuiadapterimpl.h"
#include "xnviewadapter.h"
#include "xnuiengineappif.h"
#include "xnfocuscontrol.h"

#include "xuikon_builds_cfg.hrh"

#ifdef _XN_PERFORMANCE_TEST_
#include "xntimemon.h"
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::CXnAppUiAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnAppUiAdapter::CXnAppUiAdapter( TUid aApplicationUid )
    : iApplicationUid( aApplicationUid )
    {    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::ConstructL
// Second-phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::ConstructL()
    {
#ifdef _XN_PERFORMANCE_TEST_
    TInt start( 0 );
    TInt end( 0 );
    TInt diff( 0 );
    CXnTimeMon::PrintUserMem( _L( "CXnAppUiAdapter::ConstructL(): - Calling CAknViewAppUi::BaseConstructL().." ) );
    User::AllocSize( start );
#endif //_XN_PERFORMANCE_TEST_

    CAknViewAppUi::BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    CAknToolbar* toolbar( CurrentFixedToolbar() );

    if ( toolbar )
        {
        toolbar->SetToolbarVisibility( EFalse );

        toolbar->DisableToolbarL( ETrue );
        }
    
#ifdef _XN_PERFORMANCE_TEST_
    User::AllocSize( end );
    diff = end - start;
    RDebug::Print( _L( "CAknViewAppUi::BaseConstructL allocation: %d" ), diff );
    CXnTimeMon::PrintUserMem( _L( "CXnAppUiAdapter::ConstructL(): - Calling CXnAppUiAdapterImpl::NewL().." ) );
    User::AllocSize( start );
#endif //_XN_PERFORMANCE_TEST_
    
    iImpl = CXnAppUiAdapterImpl::NewL( iApplicationUid, *this );
    iImpl->ConstructL();
        
#ifdef _XN_PERFORMANCE_TEST_
    User::AllocSize( end );
    diff = end-start;
    RDebug::Print( _L( "Xuikon launch allocation: %d" ), diff );
    
    CXnTimeMon::PrintUserMem( _L( "CXnAppUiAdapter::ConstructL() .. all done." ) );
#endif //_XN_PERFORMANCE_TEST_
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
EXPORT_C CXnAppUiAdapter::~CXnAppUiAdapter()
    {          
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
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CXnAppUiAdapter::HandleResourceChangeL - start" ) );
#endif //_XN_PERFORMANCE_TEST_

    CAknViewAppUi::HandleResourceChangeL( aType );

    if ( iImpl )
        {
        iImpl->HandleResourceChangeL( aType );
        }

#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CXnAppUiAdapter::HandleResourceChangeL - end" ) );
#endif //_XN_PERFORMANCE_TEST_
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
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::LoadDataPluginsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::LoadDataPluginsL(
    RPointerArray< CXnNodeAppIf >& /*aList*/ )
    {
    // Default empty implementation
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapter::DestroyDataPluginsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::DestroyDataPluginsL(
    RPointerArray< CXnNodeAppIf >& /*aList*/ )
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
// CXnAppUiAdapter::SetOnlineStateL
//
// -----------------------------------------------------------------------------
//    
EXPORT_C void CXnAppUiAdapter::SetOnlineStateL( 
    RPointerArray< CXnNodeAppIf >& /*aList*/ )
    {
    // Default empty implementation
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
// CXnAppUiAdapter::HandlePageSwitch
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnAppUiAdapter::HandlePageSwitch()
    {
    // Default empty implementation
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

// End of file
