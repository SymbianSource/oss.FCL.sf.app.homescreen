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
#include <e32property.h>
#include <activeidle2domainpskeys.h>

#include "wmcommon.h"
#include "widgetmanager.hrh"
#include "wmmaincontainer.h"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "wmplugin.h"
#include "wmeffectmanager.h"
#include <widgetmanagerview.rsg>

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
    iPostponedCommand = ENone;
    if ( iViewAppUi )
        {
        if ( iWmMainContainer && IsActive() )
            {
            iWmMainContainer->SetClosingDown( ETrue );
            TRAPD( err, iViewAppUi->ActivateLocalViewL(
                            iPreviousViewUid.iViewUid ); );
            if ( KErrNone == err )
                {
                iWait->Start();
                // remove view from appui
                iViewAppUi->RemoveView( 
                        TUid::Uid( EWmMainContainerViewId ) );
                }
            else
                {
                TRAP_IGNORE( iViewAppUi->DeactivateActiveViewL(); );
                iViewAppUi->RemoveFromViewStack( 
                        *iWmMainView, iWmMainContainer );
                delete iWmMainContainer;
                iWmMainContainer = NULL;
                }
            }
        }    
    delete iResourceLoader;
    delete iEffectManager;
    delete iPostponedContent;
    delete iWait;
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
    iWmMainView = NULL;

    // store static view app ui
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    if ( !eikonEnv ) User::Leave( KErrUnknown );
    iViewAppUi = (CAknViewAppUi*)eikonEnv->EikAppUi();
    if ( !iViewAppUi ) User::Leave( KErrUnknown );

    // create resource loader
    iFs = &eikonEnv->FsSession();
    iResourceLoader = CWmResourceLoader::NewL( *eikonEnv );
    iEffectManager = CWmEffectManager::NewL( *eikonEnv );
    
    // wait object
    iWait = new (ELeave) CActiveSchedulerWait();
    
    // main view
    CWmMainContainerView* mainView =
            CWmMainContainerView::NewL( *this );
    CleanupStack::PushL( mainView );
	iViewAppUi->AddViewL( mainView );	
	CleanupStack::Pop( mainView );
	iWmMainView = mainView;
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
    iWmMainContainer->SetClosingDown( EFalse );
    
    // Don't forward numeric keys to phone
    ForwardNumericKeysToPhone( EFalse );
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewDeactivated
// ---------------------------------------------------------
//
void CWmPlugin::MainViewDeactivated()
    {
    // Forward numeric keys to phone
    ForwardNumericKeysToPhone( ETrue );
    
    iPreviousViewUid.iViewUid = KNullUid;
    iWmMainContainer = NULL;
    if ( iEffectManager && !iWait->IsStarted() )
        {
        iEffectManager->UiRendered();
        }

    TRAP_IGNORE( ExecuteCommandL(); );

    if ( iWait->IsStarted() ) { iWait->AsyncStop(); }
    }

// ---------------------------------------------------------
// CWmPlugin::ForwardNumericKeysToPhone
// ---------------------------------------------------------
//
void CWmPlugin::ForwardNumericKeysToPhone( TBool aEnabled )
    {
    TInt value = aEnabled ? 
        EPSAiForwardNumericKeysToPhone : 
        EPSAiDontForwardNumericKeysToPhone;
    
    RProperty::Set(
        KPSUidAiInformation,
        KActiveIdleForwardNumericKeysToPhone,
        value );
    }

// ---------------------------------------------------------
// CWmPlugin::SetPostponedCommandL
// ---------------------------------------------------------
//
void CWmPlugin::SetPostponedCommandL(
        TCommand aCommand, CHsContentInfo& aContentInfo )
    {
    iPostponedCommand = aCommand;
    delete iPostponedContent;
    iPostponedContent = NULL;
    iPostponedContent = aContentInfo.CloneL();
    }

// ---------------------------------------------------------
// CWmPlugin::ExecuteCommandL
// ---------------------------------------------------------
//
void CWmPlugin::ExecuteCommandL()
    {
    if ( iPostponedCommand == EAddToHomescreen )
        {
        TInt err = ContentController().AddWidgetL( *iPostponedContent );
        if ( err == KHsErrorViewFull ||
                err == KHsErrorDoesNotFit )
            {
            ResourceLoader().InfoPopupL(
                R_QTN_HS_ADD_WIDGET_NO_SPACE_NOTE, KNullDesC );
            }
        else if ( err == KHsErrorMaxInstanceCountExceeded )
            {
            ResourceLoader().InfoPopupL(
                R_QTN_HS_ADD_WIDGET_MAX_COUNT_NOTE, KNullDesC );
            }
        else if ( err == KErrNoMemory )
            {
            ResourceLoader().InfoPopupL(
                R_QTN_HS_HS_MEMORY_FULL, KNullDesC );
            }
        else if ( err != KErrNone )
            {
            ResourceLoader().ErrorPopup( err );
            }
        }
    iPostponedCommand = ENone;
    delete iPostponedContent;
    iPostponedContent = NULL;
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewDeactivated
// ---------------------------------------------------------
//
CAknViewAppUi& CWmPlugin::ViewAppUi()
    {
    return *iViewAppUi;
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewDeactivated
// ---------------------------------------------------------
//
CWmResourceLoader& CWmPlugin::ResourceLoader()
    {
    return *iResourceLoader;
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewDeactivated
// ---------------------------------------------------------
//
MHsContentController& CWmPlugin::ContentController()
    {
    return *iHsContentController;
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewDeactivated
// ---------------------------------------------------------
//
RFs& CWmPlugin::FileServer()
    {
    return *iFs;
    }

// ---------------------------------------------------------
// CWmPlugin::NotifyWidgetListChanged
// ---------------------------------------------------------
//
void CWmPlugin::NotifyWidgetListChanged()
    {
    if ( iWmMainContainer && !iWmMainContainer->ClosingDown() )
        {
        iWmMainContainer->HandleWidgetListChanged();
        }
    }

// End of file
