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
#include "wmlistbox.h"

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
    iPostponedCommand = ENone;
    
    // store static view app ui
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    if ( !eikonEnv ) User::Leave( KErrUnknown );
    iViewAppUi = (CAknViewAppUi*)eikonEnv->EikAppUi();
    if ( !iViewAppUi ) User::Leave( KErrUnknown );

    // create resource loader
    iFs = &eikonEnv->FsSession();
    iResourceLoader = CWmResourceLoader::NewL( *eikonEnv );
    iEffectManager = CWmEffectManager::NewL( *eikonEnv );
    iWmInstaller = CWmInstaller::NewL( *this );

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
        // stop displaying menubar before starting fullscreen effects
        CEikMenuBar* menuBar = CEikonEnv::Static()->AppUiFactory()->MenuBar();
        if ( menuBar && menuBar->IsDisplayed() )
            {
            menuBar->StopDisplayingMenuBar();
            }

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
        if ( iPreviousViewUid.iViewUid == KNullUid && 
            iViewAppUi->GetDefaultViewId( iPreviousViewUid ) != KErrNone ) 
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
        TRAPD( err, 
            iEffectManager->BeginFullscreenEffectL(
                KAppExitEffectStyle );
            iViewAppUi->ActivateLocalViewL(
                iPreviousViewUid.iViewUid ); 
            );
        if ( KErrNone != err )
            {
            iWmMainContainer->SetClosingDown( EFalse );
            }
        }
    }

// ---------------------------------------------------------
// CWmPlugin::MainViewActivated
// ---------------------------------------------------------
//
void CWmPlugin::MainViewActivated( 
                    const TVwsViewId& /*aViewId*/,
                    CWmMainContainer* aWmMainContainer )
    {
    // previous view for Wm is always default view.
    if ( iViewAppUi->GetDefaultViewId( iPreviousViewUid ) != KErrNone )
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

    TRAPD( err, ExecuteCommandL(); );
    if ( KErrNone != err )
        {
        delete iPostponedContent;
        iPostponedContent = NULL;
        iPostponedCommand = ENone;
        }
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
        case KErrDiskFull:
            // do not show error note here to avoid multiple error notes
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

// ---------------------------------------------------------
// CWmPlugin::GetUnistalledWidget
// ---------------------------------------------------------
//
CWmWidgetData* CWmPlugin::GetUninstalledWidgetByUid( TUid aUid )
    {
    CWmWidgetData* retVal = NULL;
    if ( iWmMainContainer )
        {
        const RWidgetDataValues& widgetArray =
            iWmMainContainer->WmListBox().WidgetDataArray();
        for ( TInt i = 0; i < widgetArray.Count(); i++ )
            {
            CWmWidgetData* widgetData = widgetArray[i];
            if ( widgetData->IsUninstalling() &&
                widgetData->PublisherUid() == aUid )
                {
                retVal = widgetArray[i];
                break;
                }
            }
        }
    return retVal;
    }

// End of file

