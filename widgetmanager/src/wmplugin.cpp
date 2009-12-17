/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Description:
* widget manager plugin implementation
*
*/

// INCLUDE FILES
#include <bautils.h>
#include <coemain.h>
#include <aknViewAppUi.h>

#include "wmcommon.h"
#include "widgetmanager.hrh"
#include "wmmaincontainer.h"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "wmplugin.h"
#include "wmeffectmanager.h"

// ---------------------------------------------------------
// CWmPlugin::NewL
// ---------------------------------------------------------
//
CWmPlugin* CWmPlugin::NewL()
    {
    CWmPlugin* self=new(ELeave) CWmPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CWmPlugin::~CWmPlugin
// ---------------------------------------------------------
//
CWmPlugin::~CWmPlugin()
    {
    if ( iViewAppUi )
        {
        Deactivate();
        iViewAppUi->RemoveView( TUid::Uid( EWmMainContainerViewId ) );
        }
    iWmMainContainer = NULL;
    delete iResourceLoader;
    delete iEffectManager;
    }

// ---------------------------------------------------------
// CWmPlugin::CWmPlugin
// ---------------------------------------------------------
//
CWmPlugin::CWmPlugin()
    {
    iPreviousViewUid.iViewUid = KNullUid;
    }

// ---------------------------------------------------------
// CWmPlugin::ConstructL
// ---------------------------------------------------------
//
void CWmPlugin::ConstructL()
    {
    iWmMainContainer = NULL;

    // store static view app ui
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    if ( !eikonEnv ) User::Leave( KErrUnknown );
    iViewAppUi = (CAknViewAppUi*)eikonEnv->EikAppUi();
    if ( !iViewAppUi ) User::Leave( KErrUnknown );

    // create resource loader
    iFs = &eikonEnv->FsSession();
    iResourceLoader = CWmResourceLoader::NewL( *eikonEnv );
    iEffectManager = CWmEffectManager::NewL( *eikonEnv );
    
    // main view
    CWmMainContainerView* mainView =
            CWmMainContainerView::NewL( *this );
    CleanupStack::PushL( mainView );
	iViewAppUi->AddViewL( mainView );	
	CleanupStack::Pop( mainView );
    }

// ---------------------------------------------------------
// CWmPlugin::Activate
// ---------------------------------------------------------
//
void CWmPlugin::Activate()
    {
    if ( !IsActive() && iHsContentController )
        {
        TRAP_IGNORE( 
            iEffectManager->BeginFullscreenEffectL( 
                KAppStartCommonDefaultStyle );
            iViewAppUi->ActivateLocalViewL( 
                TUid::Uid( EWmMainContainerViewId ) ); 
            );
        }
    }

// ---------------------------------------------------------
// CWmPlugin::IsActive
// ---------------------------------------------------------
//
TBool CWmPlugin::IsActive()
    {
    return ( iPreviousViewUid.iViewUid != KNullUid );
    }

// ---------------------------------------------------------
// CWmPlugin::Deactivate
// ---------------------------------------------------------
//
void CWmPlugin::Deactivate()
    {
    if ( IsActive() )
        {
        iWmMainContainer->SetClosingDown( ETrue );
        TRAP_IGNORE( 
            iEffectManager->BeginFullscreenEffectL(
                KAppExitCommonDefaultStyle );
            iViewAppUi->ActivateLocalViewL(
                iPreviousViewUid.iViewUid ); 
            );
        }
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewActivated
// ---------------------------------------------------------
//
void CWmPlugin::MainViewActivated( 
                    const TVwsViewId& aViewId,
                    CWmMainContainer* aWmMainContainer )
    {
    iPreviousViewUid = aViewId;
    iWmMainContainer = aWmMainContainer;
    iEffectManager->UiRendered();
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewDeactivated
// ---------------------------------------------------------
//
void CWmPlugin::MainViewDeactivated()
    {
    iPreviousViewUid.iViewUid = KNullUid;
    iWmMainContainer = NULL;
    iEffectManager->UiRendered();
    }

// ---------------------------------------------------------
// CWmPlugin::NotifyWidgetListChanged
// ---------------------------------------------------------
//
void CWmPlugin::NotifyWidgetListChanged()
    {
    if ( iWmMainContainer )
        {
        iWmMainContainer->HandleWidgetListChanged();
        }
    }

// End of file
