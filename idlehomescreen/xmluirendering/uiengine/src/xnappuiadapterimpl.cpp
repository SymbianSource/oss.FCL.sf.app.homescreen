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
* Description:  Implementation for Avkon adapter
*
*/

// System includes
#include <hsccproviderclient.h>
#include <hscontentcontroller.h>
#include <hscontentcontrolui.h>
#include <hspluginsettings.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xnviewmanager.h"
#include "xnviewadapter.h"
#include "xnuistatelistener.h"
#include "xnbackgroundmanager.h"
#include "hscontentcontrolfactory.h"
#include "xneditor.h"
#include "xnwallpaperview.h"
#include "xneffectmanager.h"

#include "xnappuiadapterimpl.h"

#include "debug.h"

// Constants
_LIT( KResourceDrive, "z:" );
_LIT( KResourceFile, "xnuiengine.rsc" );

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnAppUiAdapterImpl* CXnAppUiAdapterImpl::NewL( TUid aApplicationUid,    
    CXnAppUiAdapter& aAdapter )
    {    
    return new ( ELeave ) CXnAppUiAdapterImpl( aApplicationUid, aAdapter );
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::CXnAppUiAdapterImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnAppUiAdapterImpl::CXnAppUiAdapterImpl( TUid aApplicationUid,    
    CXnAppUiAdapter& aAdapter )
    : iAdapter( aAdapter ), iApplicationUid( aApplicationUid )
    {
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::ConstructL
// Second-phase constructor
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::ConstructL()
    {       
    __PRINTS( "*** CXnAppUiAdapterImpl::ConstructL" );
    __TIME_MARK( time );
    
    const TInt KMaxUidLength( 10 );
    _LIT8( KUint, "%u" );
    TBuf8< KMaxUidLength > uid;
    uid.Format( KUint, iApplicationUid.iUid );    
    
    HSPluginSettingsIf::CHomescreenSettings::InitializeL( uid );
    
    TFileName resFile;
    resFile.Append( KResourceDrive );
    resFile.Append( KDC_APP_RESOURCE_DIR );
    resFile.Append( KResourceFile );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),
            resFile );      
    
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resFile );
        
    iContentControlFactory = CHsContentControlFactory::NewL( iAdapter );    
    
    iUiStateListener = CXnUiStateListener::NewL( iAdapter );
            
    iViewManager = CXnViewManager::NewL( iAdapter );
    
    iUiEngine = CXnUiEngine::NewL( iAdapter );

    iViewManager->SetUiEngine( *iUiEngine );
    
    // iViewAdapter is owned by CAknViewAppUi
    iViewAdapter = CXnViewAdapter::NewL( iAdapter );
       
    iViewManager->LoadUiL();
        
    iCcProviderClient = CHsCcProviderClient::NewL( iViewManager->Editor() );
    
    CXnWallpaperView* wallpaper = CXnWallpaperView::NewL( *iUiEngine );
    CleanupStack::PushL( wallpaper );
    
    // wallpaper view is owned by CAknViewAppUi
    iAdapter.AddViewL( wallpaper );
    CleanupStack::Pop( wallpaper );
    
    iEffectManager = CXnEffectManager::NewL();   
    
    __TIME_ENDMARK( "CXnAppUiAdapterImpl::ConstructL, done", time );    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::ReloadUiL
// Reloads UI when application root configuration is changed
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::ReloadUiL()
    {
    __PRINTS( "*** CXnAppUiAdapterImpl::ReloadUiL" );
    __TIME_MARK( time );

    iViewAdapter->ReloadUiL();
           
    iViewManager->ReloadUiL();
        
    __TIME_ENDMARK( "CXnAppUiAdapterImpl::ReloadUiL, done", time );
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CXnAppUiAdapterImpl::~CXnAppUiAdapterImpl()
    {  
    delete iUiEngine;
    
    delete iUiStateListener;
           
    delete iViewManager;
    
    delete iCcProviderClient;

    delete iEffectManager;

    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset ) ;
    HSPluginSettingsIf::CHomescreenSettings::UnInitialize();
    
    delete iContentControlFactory;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::UiEngine
// Gets UI engine
// -----------------------------------------------------------------------------
//
CXnUiEngine& CXnAppUiAdapterImpl::UiEngine() const
    {
    return *iUiEngine;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::ViewAdapter
// Gets view adapter
// -----------------------------------------------------------------------------
//
CXnViewAdapter& CXnAppUiAdapterImpl::ViewAdapter() const
    {
    return *iViewAdapter;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::ViewManager
// Gets view manager
// -----------------------------------------------------------------------------
//
CXnViewManager& CXnAppUiAdapterImpl::ViewManager() const
    {
    return *iViewManager;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::UiStateListener
// Gets UI state listener
// -----------------------------------------------------------------------------
//
CXnUiStateListener& CXnAppUiAdapterImpl::UiStateListener() const
    {
    return *iUiStateListener;
    }


// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::HsContentController
// Gets Content control UI interface
// -----------------------------------------------------------------------------
//
MHsContentControlUi* CXnAppUiAdapterImpl::HsContentController( 
    const TDesC8& aType ) const
    {
    return iContentControlFactory->GetHsContentController( aType );
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::HsContentControlFactory
// Gets Content control factory
// -----------------------------------------------------------------------------
//
CHsContentControlFactory* CXnAppUiAdapterImpl::HsContentControlFactory()
    {
    return iContentControlFactory;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::EffectManager
// Get effect manager.
// -----------------------------------------------------------------------------
//
CXnEffectManager* CXnAppUiAdapterImpl::EffectManager() const
    {
    return iEffectManager;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::HsContentControlSrv
// Gets Content control server interface
// -----------------------------------------------------------------------------
//
MHsContentControl* CXnAppUiAdapterImpl::HsContentControlSrv() const
    {
    return iCcProviderClient;
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::HandleResourceChangeL
// Handles resource changes
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::HandleResourceChangeL( TInt aType )
    {
    iUiStateListener->HandleResourceChangeL( aType );    
    }

// End of file
