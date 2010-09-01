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
#include <connect/sbdefs.h>
#include <e32property.h>
#include <xnuiengine.rsg>

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
#include "xnwaitdialog.h"
#include "xnitemactivator.h"

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
    
    iEffectManager = CXnEffectManager::NewL( iAdapter );   
    
    iBackupRestoreObserver = CXnPropertySubscriber::NewL( 
            KUidSystemCategory, conn::KUidBackupRestoreKey, *this );

    iActivator = CXnItemActivator::NewL( iAdapter );
    
    __TIME_ENDMARK( "CXnAppUiAdapterImpl::ConstructL, done", time );    
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::ReloadUiL
// Reloads UI when application root configuration is changed
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::ReloadUiL()
    {
    // If the UI hasn't been loaded yet, ignore the event
    if( !iViewAdapter || !iViewManager )
        {
        return;
        }
    
    __PRINTS( "*** CXnAppUiAdapterImpl::ReloadUiL" );
    __TIME_MARK( time );

    delete iActivator;
    iActivator = NULL;
           
    iViewAdapter->ReloadUiL();
           
    iViewManager->ReloadUiL();
    
    iActivator = CXnItemActivator::NewL( iAdapter );
    
    __TIME_ENDMARK( "CXnAppUiAdapterImpl::ReloadUiL, done", time );
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CXnAppUiAdapterImpl::~CXnAppUiAdapterImpl()
    {
    if ( iBURWaitDialog )
        {
        TRAP_IGNORE( iBURWaitDialog->ProcessFinishedL(); );
        }
    
    delete iActivator;
    
    delete iBackupRestoreObserver;
    
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
// CXnAppUiAdapterImpl::ItemActivator
// Gets Item activator
// -----------------------------------------------------------------------------
//
CXnItemActivator& CXnAppUiAdapterImpl::ItemActivator() const
    {
    return *iActivator;
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

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::DisplayWaitDialogL
// Displays wait dialog during backup/restore. 
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::DisplayWaitDialogL()
    {
    if ( !iBURWaitDialog )
       {
       iBURWaitDialog = new( ELeave ) CXnWaitDialog(
               reinterpret_cast<CEikDialog**>( &iBURWaitDialog ), ETrue );       
       iBURWaitDialog->SetCallback( this );
       iBURWaitDialog->ExecuteLD( R_BACKUP_RESTORE_WAIT_DIALOG );
       }
    }

// -----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::BackupRestoreEvent
// Property changed notification callback
// -----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::PropertyChangedL( const TUint32 aKey, 
    const TInt aValue )
    {
    const TUint mask( conn::KBURPartTypeMask ^ conn::EBURNormal );
    if ( aKey == conn::KUidBackupRestoreKey )
        {
        if ( aValue & mask )
            {
            // Any type of backup or restore operation started
            DisplayWaitDialogL();
            }
        else if ( iBURWaitDialog )
            {
            // Any type of backup or restore operation ended
            iBURWaitDialog->ProcessFinishedL();
            // ProcessFinishedL() will NULL iBURWaitDialog
            }
        }
    }

// ----------------------------------------------------------------------------
// CXnAppUiAdapterImpl::DialogDismissedL()
// Callback method from MProgressDialogCallback interface.
// ----------------------------------------------------------------------------
//
void CXnAppUiAdapterImpl::DialogDismissedL(TInt /*aButtonId*/)
    {
    // No implementation required.
    }

// End of file
