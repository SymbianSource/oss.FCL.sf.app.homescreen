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
* Implementation of the main container view for WidgetManager application.
*
*/

// INCLUDES
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <akncontext.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <widgetmanagerview.rsg>
#include <hscontentcontroller.h>
#include <AknUtils.h>
#include <avkon.rsg>
#include <layoutmetadata.cdl.h>
#include <widgetmanager.mbg>
#include "wmplugin.h"
#include "widgetmanager.hrh"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "wmmaincontainer.h"
#include "wmspbgcleaner.h"
#include "wmconfiguration.h"
#include "wmimageconverter.h"
#include "wmstore.h"
       
// ---------------------------------------------------------
// CWmMainContainerView::CWmMainContainerView()
// ---------------------------------------------------------
//
CWmMainContainerView::CWmMainContainerView(
        CWmPlugin& aWmPlugin ):
    iWmPlugin( aWmPlugin )
	{
	iWmMainContainer = NULL;
	iWmSpBgCleaner = NULL;
	}

// ---------------------------------------------------------
// CWmMainContainerView::~CWmMainContainerView()
// ---------------------------------------------------------
//
CWmMainContainerView::~CWmMainContainerView()
	{    
    if ( iWmMainContainer != NULL )
        {
        AppUi()->RemoveFromStack( iWmMainContainer );

        delete iWmMainContainer;
        iWmMainContainer = NULL;
        }
    
    if ( iWmSpBgCleaner != NULL )
        {
        AppUi()->RemoveFromStack( iWmSpBgCleaner );
        delete iWmSpBgCleaner;
        iWmSpBgCleaner = NULL;
        }
	}

// ---------------------------------------------------------
// CWmMainContainerView::NewL
// ---------------------------------------------------------
//
CWmMainContainerView* CWmMainContainerView::NewL(
        CWmPlugin& aWmPlugin )
	{
	CWmMainContainerView* self = CWmMainContainerView::NewLC(
	        aWmPlugin );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CWmMainContainerView::NewLC
// ---------------------------------------------------------
//
CWmMainContainerView* CWmMainContainerView::NewLC(
        CWmPlugin& aWmPlugin )
	{
	CWmMainContainerView* self = 
        new ( ELeave ) CWmMainContainerView( aWmPlugin );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------
// CWmMainContainerView::ConstructL
// ---------------------------------------------------------
//
void CWmMainContainerView::ConstructL()
	{
	BaseConstructL( R_WM_MAIN_CONTAINER_VIEW );
	}

// ---------------------------------------------------------
// CWmMainContainerView::Id
// ---------------------------------------------------------
//
TUid CWmMainContainerView::Id() const
	{
	return TUid::Uid( EWmMainContainerViewId );
	}

// ---------------------------------------------------------
// CWmMainContainerView::HandleCommandL
// ---------------------------------------------------------
//
void CWmMainContainerView::HandleCommandL( TInt aCommand )
	{
    if ( !iWmMainContainer->ClosingDown() )
        {
        switch ( aCommand )
            {
            case EAknSoftkeySelect:
            case EWmMainContainerViewAddMenuItemCommand:
                HandleAddMenuItemSelectedL();
                break;
            case EWmMainContainerViewLaunchMenuItemCommand:
                HandleLaunchMenuItemSelectedL();
                break;
            case EWmMainContainerViewSearchMenuItemCommand: // flow through
            case EAknSoftkeySearch:
                HandleSearchMenuItemSelectedL();
                break;
            case EWmMainContainerViewSortAlphaMenuItemCommand:
                HandleSortAlphaMenuItemSelectedL();
                break;
            case EWmMainContainerViewUninstallMenuItemCommand:
                HandleUninstallMenuItemSelectedL();
                break;
            case EWmMainContainerViewHelpMenuItemCommand:
                HandleHelpMenuItemSelectedL();
                break;
            case EWmMainContainerViewBackMenuItemCommand: // flow through
            case EAknSoftkeyBack:
                {
                if ( iWmMainContainer && 
                  !iWmMainContainer->IsLoadingWidgets() )
                    {
                    iWmPlugin.CloseView();
                    }
                break;
                }
            case EWmMainContainerViewWiddetDetailsMenuItemCommand:
                HandleDetailsMenuItemSelectedL();
                break;
            case EWmStoreCommand:
                {
		        if ( iWmMainContainer )
		            iWmMainContainer->HandleStoreCommandL();
                }
		        break;
            case EAknSoftkeyCancel: // flow through
            case EEikCmdCanceled:
                {
                HandleDeactivateFindPaneL();
                break;
                }
            default:
                break;
            }
        }
	}

// ---------------------------------------------------------
// CWmMainContainerView::DoActivateL
// ---------------------------------------------------------
//
void CWmMainContainerView::DoActivateL( 
		const TVwsViewId& aPrevViewId,
		TUid /*aCustomMessageId*/,
		const TDesC8& /*aCustomMessage*/ )
	{
    // setup status pane layout
    TInt layoutResourceId = ( (iWmPlugin.Configuration().StoreCount() ) ? 
            R_AVKON_STATUS_PANE_LAYOUT_IDLE_FLAT :
            R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
    StatusPane()->SwitchLayoutL( layoutResourceId );
    
    // ToDo: Support for msk in all layout is ongoing, when available in 
    // sdk use next line and get rid of above.
    
    //StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
    
    // apply changes 
    StatusPane()->ApplyCurrentSettingsL();
    // disable transparancy
    StatusPane()->EnableTransparent( EFalse );
    // create container
    if ( iWmSpBgCleaner == NULL )
        {
        iWmSpBgCleaner = CWmSpBgCleaner::NewL();
        iWmSpBgCleaner->SetMopParent( this );
        AppUi()->AddToStackL( *this, iWmSpBgCleaner );
        }

    // title in status pane
    SetTitleL();

    iWmSpBgCleaner->DrawNow();
    StatusPane()->DrawNow();

    // update cba
    if ( !AknLayoutUtils::MSKEnabled() ||
        iWmPlugin.Configuration().StoreCount() == 0 )
        {
        CEikButtonGroupContainer* bgc( Cba() );
        CEikCba* cba = static_cast< CEikCba* >( bgc->ButtonGroup() );
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
        bgc->SetBoundingRect( TRect() );
        cba->DrawNow();
        }
    else if ( AknLayoutUtils::MSKEnabled() )
        {
        UpdateMSKIconL();
        }

    // create container
    if ( iWmMainContainer == NULL )
        {
        iWmMainContainer = CreateContainerL();
        iWmMainContainer->SetMopParent( this );
        AppUi()->AddToStackL( *this, iWmMainContainer );
        iWmMainContainer->DrawNow();
        }
    
    iWmPlugin.MainViewActivated( aPrevViewId, iWmMainContainer );
	}

// ---------------------------------------------------------
// CWmMainContainerView::DoDeactivate
// ---------------------------------------------------------
//
void CWmMainContainerView::DoDeactivate()
	{
	if ( iWmMainContainer != NULL )
		{
        AppUi()->RemoveFromStack( iWmMainContainer );
		delete iWmMainContainer;
		iWmMainContainer = NULL;
		iWmPlugin.MainViewDeactivated();
		}

    if ( iWmSpBgCleaner != NULL )
        {
        AppUi()->RemoveFromStack( iWmSpBgCleaner );
        delete iWmSpBgCleaner;
        iWmSpBgCleaner = NULL;
        }
	}

// ---------------------------------------------------------
// CWmMainContainerView::SetTitleL
// ---------------------------------------------------------
//
void CWmMainContainerView::SetTitleL()
	{	
	// setup the title pane
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
		StatusPane()->PaneCapabilities( titlePaneUid );
	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* title = static_cast< CAknTitlePane* >( 
			StatusPane()->ControlL( titlePaneUid ) );
		if ( title )
		    {
		    TResourceReader reader;
            iEikonEnv->CreateResourceReaderLC( 
                    reader, R_WM_MAIN_CONTAINER_TITLE_RESOURCE );
            title->SetFromResourceL( reader );
            CleanupStack::PopAndDestroy(); // reader internal state
		    }
		}
	}

// ---------------------------------------------------------
// CWmMainContainerView::CreateContainerL
// ---------------------------------------------------------
//
CWmMainContainer* CWmMainContainerView::CreateContainerL()
	{
	return CWmMainContainer::NewL( ClientRect(), iWmPlugin );
	}

// ---------------------------------------------------------
// CWmMainContainerView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CWmMainContainerView::DynInitMenuPaneL( 
        TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_WM_MAIN_CONTAINER_MENU_PANE &&
         iWmMainContainer )
        {
        aMenuPane->SetItemDimmed( EWmMainContainerViewAddMenuItemCommand,
                !iWmMainContainer->CanDoAdd() );
        aMenuPane->SetItemDimmed( EWmMainContainerViewWiddetDetailsMenuItemCommand,
                !iWmMainContainer->CanDoDetails() );
        aMenuPane->SetItemDimmed( EWmMainContainerViewLaunchMenuItemCommand,
                !iWmMainContainer->CanDoLaunch() );
        aMenuPane->SetItemDimmed( EWmMainContainerViewSearchMenuItemCommand,
                !iWmMainContainer->CanDoFind() );
        aMenuPane->SetItemDimmed( EWmMainContainerViewSortAlphaMenuItemCommand,
                !iWmMainContainer->CanDoSort() );
        aMenuPane->SetItemDimmed( EWmMainContainerViewHelpMenuItemCommand,
                !iWmMainContainer->CanDoHelp() );
		aMenuPane->SetItemDimmed( EWmMainContainerViewUninstallMenuItemCommand,
                !iWmMainContainer->CanDoUninstall() );
        if ( !iWmMainContainer->WidgetSelected() )
            aMenuPane->DeleteMenuItem( EWmMainContainerViewAddMenuItemCommand );
        }
    }

// ---------------------------------------------------------
// CWmMainContainerView::HandleDetailsMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleDetailsMenuItemSelectedL()
    {
    if ( iWmMainContainer && iWmMainContainer->WidgetSelected() )
        {       
        iWmMainContainer->LaunchDetailsDialogL();
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CWmMainContainerView::HandleAddMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleAddMenuItemSelectedL()
	{
	if ( iWmMainContainer && iWmMainContainer->WidgetSelected() )
	    {	    
	    iWmMainContainer->AddWidgetToHomeScreenL();
	    }
	return ETrue;
	}

// ---------------------------------------------------------
// CWmMainContainerView::HandleLaunchMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleLaunchMenuItemSelectedL()
	{
    if ( iWmMainContainer && iWmMainContainer->WidgetSelected() )
        {
        iWmMainContainer->LaunchWidgetL();
        }
	return ETrue;
	}

// ---------------------------------------------------------
// CWmMainContainerView::HandleSearchMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleSearchMenuItemSelectedL()
	{
    if ( iWmMainContainer )
        {
        iWmMainContainer->ActivateFindPaneL( ETrue );
        }
	return ETrue;
	}

// ---------------------------------------------------------
// CWmMainContainerView::HandleSortAlphaMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleSortAlphaMenuItemSelectedL()
    {
    if ( iWmMainContainer )
        {
        iWmMainContainer->SortListAlphabeticallyL();
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CWmMainContainerView::HandleUninstallMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleUninstallMenuItemSelectedL()
    {
    if ( iWmMainContainer )
        {
        iWmMainContainer->UninstallWidgetL();
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CWmMainContainerView::HandleHelpMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleHelpMenuItemSelectedL()
    {
    iWmMainContainer->ShowHelpL();
    return ETrue;
    }

// ---------------------------------------------------------
// CWmMainContainerView::HandleDeactivateFindPaneL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleDeactivateFindPaneL()
    {
    if ( iWmMainContainer )
        {
        iWmMainContainer->DeactivateFindPaneL();
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CWmMainContainerView::UpdateMSKIconL
// ---------------------------------------------------------
//
void CWmMainContainerView::UpdateMSKIconL()
    {
    CEikButtonGroupContainer* bgc( Cba() );
    CEikCba* cba = static_cast< CEikCba* >( bgc->ButtonGroup() );
    TFileName fileName( iWmPlugin.ResourceLoader().IconFilePath() );
    TAknsItemID skinId = KAknsIIDQgnMenuOviStore;
    TInt bitmapId( EMbmWidgetmanagerQgn_menu_ovistore );
    TInt maskId( EMbmWidgetmanagerQgn_menu_ovistore_mask );

    if ( iWmPlugin.Configuration().StoreCount() > 1 )
        {
        skinId = KAknsIIDQgnPropLmShopping;        
        bitmapId = EMbmWidgetmanagerQgn_menu_shopping;
        maskId = EMbmWidgetmanagerQgn_menu_shopping_mask;
        }
    else
        {
        const RPointerArray<CWmConfItem>& storeConfArray = 
                iWmPlugin.Configuration().StoreConfArray();
        CWmConfItem* confItem( storeConfArray[0] );
        
        if ( confItem && 
            confItem->Id() != CWmStore::EOvi && 
            confItem->Icon().Length() )
            {            
            CWmImageConverter* imageConverter = CWmImageConverter::NewL();
            CleanupStack::PushL( imageConverter );
            
            // parse icon str      
            // TODO: now only mif&skin are supported. Later when API is there
            // we can add support for rest icon types
            TBool res = imageConverter->ParseIconString(
                    confItem->Icon(), skinId, bitmapId, maskId, fileName );
            
            CleanupStack::PopAndDestroy( imageConverter );
            
            if ( !res )
                {
                // use default shopping icon
                skinId = KAknsIIDQgnPropLmShopping;        
                bitmapId = EMbmWidgetmanagerQgn_menu_shopping;
                maskId = EMbmWidgetmanagerQgn_menu_shopping_mask;
                fileName.Copy( iWmPlugin.ResourceLoader().IconFilePath() );
                }
            }
        }
    
    cba->UpdateMSKIconL(
                    skinId,
                    fileName,
                    bitmapId,
                    maskId,
                    ETrue );
    bgc->SetBoundingRect( TRect() );                   
    cba->DrawNow();
    }

// End of file
