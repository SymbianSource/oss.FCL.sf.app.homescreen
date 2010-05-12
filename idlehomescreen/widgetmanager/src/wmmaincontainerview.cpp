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

#include "wmplugin.h"
#include "widgetmanager.hrh"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "wmmaincontainer.h"
#include "wmspbgcleaner.h"

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
            case EWmMainContainerViewOpenPortalMenuItemCommand:
                HandleOpenMenuItemSelectedL();
                break;
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
            case EAknSoftkeySelect:
		        if ( iWmMainContainer )
		            iWmMainContainer->SelectL();
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
    StatusPane()->SwitchLayoutL( 
            R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );    
    // disable transparancy 
    StatusPane()->EnableTransparent( EFalse );
    // apply changes 
    StatusPane()->ApplyCurrentSettingsL();

    // create container
    if ( iWmSpBgCleaner == NULL )
        {
        iWmSpBgCleaner = CWmSpBgCleaner::NewL();
        iWmSpBgCleaner->SetMopParent( this );
        AppUi()->AddToStackL( *this, iWmSpBgCleaner );
        }

    // title in status pane
    SetTitleL();

    StatusPane()->DrawNow();

    // update cba
    CEikButtonGroupContainer* bgc( Cba() );
    CEikCba* cba = static_cast< CEikCba* >( bgc->ButtonGroup() );
    if ( cba ) 
        {
        TInt cbaResourceId = ( AknLayoutUtils::MSKEnabled() ?
                                R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT : 
                                R_AVKON_SOFTKEYS_OPTIONS_BACK );

        cba->SetCommandSetL( cbaResourceId );
        bgc->SetBoundingRect( TRect() );
        cba->DrawNow();
        }

    // create container
    if ( iWmMainContainer == NULL )
        {
        iWmMainContainer = CreateContainerL();
        iWmMainContainer->SetMopParent( this );
        AppUi()->AddToStackL( *this, iWmMainContainer );
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
            title->DrawDeferred();
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
        if ( !iWmMainContainer->PortalSelected() )
            aMenuPane->DeleteMenuItem( EWmMainContainerViewOpenPortalMenuItemCommand );
        if ( !iWmMainContainer->WidgetSelected() )
            aMenuPane->DeleteMenuItem( EWmMainContainerViewAddMenuItemCommand );
        }
    }

// ---------------------------------------------------------
// CWmMainContainerView::HandleOpenMenuItemSelectedL
// ---------------------------------------------------------
//
TBool CWmMainContainerView::HandleOpenMenuItemSelectedL()
    {
    if ( iWmMainContainer && iWmMainContainer->PortalSelected() )
        {       
        iWmMainContainer->OpenPortalL();
        }
    return ETrue;
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
// CWmMainContainerView::HandleForegroundEventL
// ---------------------------------------------------------
//
void CWmMainContainerView::HandleForegroundEventL( TBool aForeground )
    {
    CAknView::HandleForegroundEventL( aForeground );
    if ( iWmMainContainer )
        {
        iWmMainContainer->ProcessForegroundEvent( aForeground );
        }
    }

// End of file
