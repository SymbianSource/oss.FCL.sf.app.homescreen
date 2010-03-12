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
#include <eikappui.h>
#include <eikapp.h>
#include <e32property.h>
#include <e32base.h>
#include <activeidle2domainpskeys.h>
#include <widgetmanagerview.rsg>

#include "wmcommon.h"
#include "widgetmanager.hrh"
#include "wmmaincontainer.h"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "wmplugin.h"
#include "wmeffectmanager.h"
#include "wmwidgetdata.h"
#include "wminstaller.h"


// ---------------------------------------------------------
// CWmPlugin::NewL
// ---------------------------------------------------------
//
CWmPlugin* CWmPlugin::NewL()
    {
    CWmPlugin* self = new( ELeave ) CWmPlugin();
    CleanupStack::PushL( self );
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
    if ( iWmInstaller && 
       iWmInstaller->IsActive() )
        {
        iWmInstaller->Cancel();
        }

    // delete members
    delete iResourceLoader;
    delete iEffectManager;
    delete iPostponedContent;
    delete iWmInstaller;
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
    iWmInstaller = CWmInstaller::NewL();

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
    CWmMainContainerView* view = static_cast<CWmMainContainerView*>(
            iViewAppUi->View( TUid::Uid(EWmMainContainerViewId) ) );
    if ( !IsActive() && view && iHsContentController )
        {
        TRAP_IGNORE( 
            iEffectManager->BeginFullscreenEffectL( 
                KAppStartEffectStyle );
            iViewAppUi->ActivateLocalViewL( 
                TUid::Uid( EWmMainContainerViewId ) );
            );
        }
    }

// ---------------------------------------------------------
// CWmPlugin::DeActivate
// ---------------------------------------------------------
//
void CWmPlugin::DeActivate()
    {
    iPostponedCommand = ENone;
    iPreviousViewUid.iViewUid = KNullUid;    
    CWmMainContainerView* view = static_cast<CWmMainContainerView*>(
        iViewAppUi->View( TUid::Uid(EWmMainContainerViewId) ) );
    if ( view ) { view->DoDeactivate(); }
    }

// ---------------------------------------------------------
// CWmPlugin::Views
// ---------------------------------------------------------
//
void CWmPlugin::Views( RPointerArray<CAknView>& aViews )
    {
    // return view to be destroyed.
    CAknView* view = iViewAppUi->View( 
            TUid::Uid(EWmMainContainerViewId) );
    if ( view )
        {        
        aViews.Append( view );
        }
    }

// ---------------------------------------------------------
// CWmPlugin::IsActive
// ---------------------------------------------------------
//
TBool CWmPlugin::IsActive()
    {
    TVwsViewId activeViewId(KNullUid,KNullUid);
    if ( iViewAppUi->GetActiveViewId( activeViewId ) == KErrNone &&
        activeViewId.iViewUid == TUid::Uid( EWmMainContainerViewId ) )
        {
        if ( iPreviousViewUid.iViewUid == KNullUid )
            {            
            iPreviousViewUid.iAppUid = iViewAppUi->Application()->AppDllUid();
            iPreviousViewUid.iViewUid = TUid::Uid( 1 );
            }        
        return ETrue;
        }

    return( iPreviousViewUid.iViewUid != KNullUid );
    }

// ---------------------------------------------------------
// CWmPlugin::CloseView
// ---------------------------------------------------------
//
void CWmPlugin::CloseView()
    {
    if ( IsActive() )
        {
        iWmMainContainer->SetClosingDown( ETrue );
        TRAP_IGNORE( 
            iEffectManager->BeginFullscreenEffectL(
                KAppExitEffectStyle );
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
    // verify if we have correct viewid to activate.
    if ( iPreviousViewUid.iViewUid == KNullUid )
        {
        // use default if we got wrong viewid as previous view
        iPreviousViewUid.iAppUid = iViewAppUi->Application()->AppDllUid();
        iPreviousViewUid.iViewUid = TUid::Uid( 1 );
        }

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
    if ( iEffectManager )
        {
        iEffectManager->UiRendered();
        }

    TRAP_IGNORE( ExecuteCommandL(); );
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
        if ( err != KErrNone )
            ShowErrorNoteL( err );
        }
    iPostponedCommand = ENone;
    delete iPostponedContent;
    iPostponedContent = NULL;
    }

// ---------------------------------------------------------
// CWmPlugin::ShowErrorNoteL
// ---------------------------------------------------------
//
void CWmPlugin::ShowErrorNoteL( TInt aError )
    {
    switch ( aError )
        {
        case KHsErrorViewFull:
        case KHsErrorDoesNotFit:
            // Not enough space to add new widget to active page. 
            // Remove some content first.
            ResourceLoader().InfoPopupL(
                R_QTN_HS_ADD_WIDGET_NO_SPACE_NOTE, KNullDesC );
            break;
        case KErrNoMemory:
            // Not enough memory to add more content. 
            // Please remove some Homescreen content and try again.
            ResourceLoader().InfoPopupL( R_QTN_HS_HS_MEMORY_FULL, KNullDesC ); 
            break;
        default:
            ResourceLoader().ErrorPopup( aError );
            break;
        }
    }

// ---------------------------------------------------------
// CWmPlugin::ViewAppUi
// ---------------------------------------------------------
//
CAknViewAppUi& CWmPlugin::ViewAppUi()
    {
    return *iViewAppUi;
    }

// ---------------------------------------------------------
// CWmPlugin::ResourceLoader
// ---------------------------------------------------------
//
CWmResourceLoader& CWmPlugin::ResourceLoader()
    {
    return *iResourceLoader;
    }

// ---------------------------------------------------------
// CWmPlugin::ContentController
// ---------------------------------------------------------
//
MHsContentController& CWmPlugin::ContentController()
    {
    return *iHsContentController;
    }

// ---------------------------------------------------------
// CWmPlugin::FileServer
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

// ---------------------------------------------------------
// CWmPlugin::WmInstaller
// ---------------------------------------------------------
//
CWmInstaller& CWmPlugin::WmInstaller()
    {
    return *iWmInstaller;
    }

// End of file

