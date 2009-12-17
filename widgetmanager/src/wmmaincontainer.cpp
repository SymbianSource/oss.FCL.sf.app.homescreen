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
* Implementation of the container control for WidgetManager application.
*
*/

// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikappui.h>
#include <widgetmanagerview.rsg>
#include <coemain.h>
#include <eikenv.h>
#include <barsread.h>
#include <AknUtils.h>
#include <barsread.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <AknIconArray.h>
#include <aknstyluspopupmenu.h> 
#include <avkon.loc>
#include <aknlayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsSkinInstance.h>
#include <apgicnfl.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <aknenv.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <aknsfld.h>
#include <AknLayout.lag>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsBasicBackgroundControlContext.h>
#include <apgcli.h>
#include <widgetmanager.mbg>
#include <avkon.rsg>
#include <coecobs.h>
#include <coecntrl.h>
#include <SWInstApi.h> //installer
#include <WidgetRegistryClient.h> // widgetreqistry
#include <aknbutton.h> // CAknButton
#include <SchemeHandler.h> // for starting the OVI client
#include <featmgr.h>     // FeatureManager
#include <hlplch.h>      // HlpLauncher
#include <csxhelp/hmsc.hlp.hrh>
#include <aisystemuids.hrh>

#include "wmcommon.h"
#include "wmplugin.h"
#include "wmmaincontainer.h"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "widgetmanager.hrh"
#include "wmlistbox.h"
#include "wmpersistentwidgetorder.h"
#include "wmdetailsdlg.h"
#include "wmwidgetloaderao.h"

// CONSTANTS
const TInt KTextLimit = 40; // Text-limit for find-field
const TInt KMinWidgets = 1; // minimum number of widgets to show findpane
_LIT8( KWrtMime, "application/x-nokia-widget");

// ---------------------------------------------------------
// CWmMainContainer::CWmMainContainer()
// ---------------------------------------------------------
//
CWmMainContainer::CWmMainContainer( CWmPlugin& aWmPlugin )
    :iWmPlugin( aWmPlugin )
	{
    iWidgetsList = NULL;
    iFindbox = NULL;
    iFindPaneIsVisible = EFalse;
    iBgContext = NULL;
    iFocusMode = ENowhere;
    iClosingDown = EFalse;
	}

// ---------------------------------------------------------
// CWmMainContainer::~CWmMainContainer()
// ---------------------------------------------------------
//
CWmMainContainer::~CWmMainContainer()
	{
	RemoveCtrlsFromStack();
    Components().ResetAndDestroy();
    delete iWidgetLoader;
    iWidgetsList = NULL;
    iOviPortal = NULL;
    iFindbox = NULL;
	delete iBgContext;
	}

// ---------------------------------------------------------
// CWmMainContainer::NewL
// ---------------------------------------------------------
//
CWmMainContainer* CWmMainContainer::NewL( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver,
		CWmPlugin& aWmPlugin )
	{
	CWmMainContainer* self = CWmMainContainer::NewLC( 
			aRect, 
			aParent, 
			aCommandObserver,
			aWmPlugin );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CWmMainContainer::NewLC
// ---------------------------------------------------------
//
CWmMainContainer* CWmMainContainer::NewLC( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
    MEikCommandObserver* aCommandObserver,
    CWmPlugin& aWmPlugin )
	{
	CWmMainContainer* self = new ( ELeave ) CWmMainContainer( aWmPlugin );
	CleanupStack::PushL( self );
	self->ConstructL( aRect, aParent, aCommandObserver );
	return self;
	}

// ---------------------------------------------------------
// ScreenRect
// rectangle representing the screen
// ---------------------------------------------------------
//
TRect ScreenRect()
    {
    TRect screen;
    CCoeEnv* env = CCoeEnv::Static();
    if ( env )
        {
        CWsScreenDevice* screenDevice = env->ScreenDevice();
        if ( screenDevice )
            screen.SetRect( TPoint(0,0), screenDevice->SizeInPixels() );
        }
    return screen;
    }

// ---------------------------------------------------------
// CWmMainContainer::ConstructL
// ---------------------------------------------------------
//
void CWmMainContainer::ConstructL( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
        MEikCommandObserver* /*aCommandObserver*/ )
	{
    // Initialize control array
	InitComponentArrayL();
	Components().SetControlsOwnedExternally( ETrue );

	// create the UI
	if ( aParent == NULL )
	    {
		CreateWindowL();
		iParent = this;
	    }
	else
	    {
	    SetContainerWindowL( *aParent );
	    iParent = aParent;
	    }

	// background context
	iBgContext = CAknsBasicBackgroundControlContext::NewL( 
	        KAknsIIDQsnBgScreen, ScreenRect() , ETrue);

	// set up controls
	InitializeControlsL( aRect );

	// set size and activate
	SetRect( aRect );
	if ( !aParent )
        {
        ActivateL();
        }
	
	}

// ---------------------------------------------------------
// CWmMainContainer::SizeChanged
// ---------------------------------------------------------
//
void CWmMainContainer::SizeChanged()
	{
	CCoeControl::SizeChanged();
    iBgContext->SetRect( ScreenRect() );
	TRAP_IGNORE( LayoutControlsL() );
	}

// ---------------------------------------------------------
// CWmMainContainer::LayoutControlsL
// ---------------------------------------------------------
//
void CWmMainContainer::LayoutControlsL()
	{
	TRect rect = Rect();
	TBool landscape = ( rect.Width() > rect.Height() );

    // calculate layout (different in landscape and portrait)
	TRect oviButtonRect;
	TRect widgetsListRect;
	if ( landscape )
	    {
	    iLayout = ELandscape;
        const TInt KOffset = 2;
        TInt oviButtonWidth = rect.Width() / 6;
        widgetsListRect.SetRect(
                TPoint( 0, 0 ),
                TPoint( rect.Width() - oviButtonWidth - KOffset, rect.Height() ) );
        oviButtonRect.SetRect(
                TPoint( rect.Width() - oviButtonWidth, KOffset ),
                TPoint( rect.Width() - KOffset, rect.Height() - KOffset ) );
	    }
	else
	    {
	    iLayout = EPortrait;
	    const TInt KOffset = 2;
	    TInt oviButtonHeight = rect.Height() / 8;
	    oviButtonRect.SetRect(
	            TPoint( KOffset, KOffset ),
	            TPoint( rect.Width() - KOffset, oviButtonHeight ) );
	    widgetsListRect.SetRect(
	            TPoint( 0, oviButtonHeight + KOffset ),
	            TPoint( rect.Width(), rect.Height() ) );
	    }

    // Apply layout to child controls
	iOviPortal->SetRect( oviButtonRect );
	iOviPortal->SetHighlightRect( oviButtonRect );
    iWidgetsList->SetRect( widgetsListRect );
    if( iFindbox && iFindPaneIsVisible )
        {
        // resizes listbox/searchfield
        HandleFindSizeChanged();
        }

    // change OVI portal button style
    if ( landscape )
        {
        // vertical style button
        iOviPortal->State()->SetTextL( KNullDesC() );
        TInt iconDimension = oviButtonRect.Width() * 2 / 3;
        iOviPortal->SetIconSize( TSize( iconDimension, iconDimension ) );
        iOviPortal->SetTextVerticalAlignment( 
                CAknButton::ECenter );
        iOviPortal->SetTextAndIconAlignment(
                CAknButton::EIconOverText );
        }
    else
        {
        // horizontal style button
        HBufC* oviText = StringLoader::LoadLC( R_QTN_WM_GO_TO_OVI_STORE );
        iOviPortal->State()->SetTextL( *oviText );
        CleanupStack::PopAndDestroy( oviText );
        TInt iconDimension = oviButtonRect.Height() * 2 / 3;
        iOviPortal->SetIconSize( TSize( iconDimension, iconDimension ) );
        iOviPortal->SetTextHorizontalAlignment( 
                CGraphicsContext::ECenter );
        iOviPortal->SetTextAndIconAlignment(
                CAknButton::EIconBeforeText );
        }

    DrawDeferred();
	}

// ---------------------------------------------------------
// CWmMainContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::OfferKeyEventL( 
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
	{
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    if ( iFindbox )
        {
        // open search field with alpha digit numbers
        if ( aType == EEventKeyDown && !iFindPaneIsVisible && 
        aKeyEvent.iScanCode < EStdKeyF1 &&
            TChar(aKeyEvent.iScanCode).IsAlphaDigit() )
            {
            ActivateFindPaneL();
            
            if ( iFindPaneIsVisible )
                {
                return EKeyWasConsumed;
                }
            }
        
        if ( iFindPaneIsVisible )
            {
            // deactive the FindPane when Back has been pressed
            if ( aType == EEventKeyDown && 
                aKeyEvent.iScanCode == EStdKeyBackspace )
                {
                TBuf<KTextLimit> searchText;
                iFindbox->GetSearchText( searchText );
                if ( searchText == KNullDesC )
                    {
                    DeactivateFindPaneL();
                    return EKeyWasConsumed;
                    }
                }
            
            // Cancel-selected need to unfocus findbox 
            // to receive event in ProcessCommand
            if ( aType == EEventKeyDown 
            && aKeyEvent.iScanCode == EStdKeyDevice1 )
                {
                iFindbox->SetFocus( EFalse );
                UpdateFocusMode();
                return EKeyWasConsumed;
                }

            if ( aKeyEvent.iScanCode == EStdKeyNo ||
                aKeyEvent.iCode == EKeyNo )
                {
                DeactivateFindPaneL();
                return EKeyWasConsumed;
                }

            // find items with all event codes (that's the reason why there is EEventKey instead of aType)
            TBool needsRefresh( EFalse );
            keyResponse = AknFind::HandleFindOfferKeyEventL( 
                                                aKeyEvent, EEventKey, this, 
                                iWidgetsList, iFindbox, ETrue, needsRefresh );            
            if ( needsRefresh )
                {
                DrawNow();
                }
            }
        }

    // check special cases for movement between controls
    if ( keyResponse == EKeyWasNotConsumed && !iFindPaneIsVisible )
         {
         keyResponse = MoveFocusByKeys( aKeyEvent, aType );
         }

    // pass key event except backpace or delete key event to widgets list if focused
    if ( keyResponse == EKeyWasNotConsumed && iWidgetsList->IsFocused() )
        {
        if ( (aType == EEventKey) && (aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete))
            {
            if( CanDoUninstall() )
                {
                UninstallWidgetL();
                }
            else
                {
                CWmWidgetData* data = iWidgetsList->WidgetData();
                if( data != NULL )
                    iWmPlugin.ResourceLoader().InfoPopupL(
                         R_QTN_WM_UNINST_NOT_ALLOWED, data->Name() );
                }
            }
        else 
            {
			//passing to listbox handler
            keyResponse = iWidgetsList->OfferKeyEventL( 
                    aKeyEvent, aType );
            }
        }

    // pass key event to OVI portal if focused
    if ( keyResponse == EKeyWasNotConsumed && iOviPortal->IsFocused() )
        {
        keyResponse = iOviPortal->OfferKeyEventL( 
                            aKeyEvent, aType );
        }

    if ( keyResponse == EKeyWasConsumed )
        DrawDeferred();

	// Do not let UI framework forward the keys to child controls as
	// we have already done that.
    return EKeyWasConsumed;
	}


// ---------------------------------------------------------
// CWmMainContainer::MoveFocusByKeys
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::MoveFocusByKeys(
                        const TKeyEvent& aKeyEvent, 
                        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );

    if ( iWidgetsList->IsFocused() )
        {
        // ------------------------------------
        // focus is in the WIDGETS LIST
        // ------------------------------------
        if ( iLayout == EPortrait &&
                aKeyEvent.iScanCode == EStdKeyUpArrow &&
                iWidgetsList->CurrentItemIndex() == 0 )
            {
            // widget list top -> up -> ovi button (portrait)
            if ( aType == EEventKey )
                SetFocusToOviButton();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLayout == EPortrait &&
                aKeyEvent.iScanCode == EStdKeyDownArrow &&
                iWidgetsList->CurrentItemIndex() ==
                   iWidgetsList->Model()->NumberOfItems() - 1 )
            {
            // widget list bottom -> down -> ovi button (portrait)
            if ( aType == EEventKey )
                SetFocusToOviButton();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLayout == ELandscape &&
                aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // widget list -> right -> ovi button (landscape)
            if ( aType == EEventKey )
                SetFocusToOviButton();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLayout == ELandscapeMirrored &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            // widget list -> left -> ovi button (landscape mirrored)
            if ( aType == EEventKey )
                SetFocusToOviButton();
            keyResponse = EKeyWasConsumed;
            }
        }
    else if ( iOviPortal->IsFocused() )
        {
        // ------------------------------------
        // focus is in the OVI PORTAL BUTTON
        // ------------------------------------
        if ( iLayout == EPortrait &&
                aKeyEvent.iScanCode == EStdKeyDownArrow )
            {
            // ovi button -> down -> widget list top (portrait)
            if ( aType == EEventKey )
                SetFocusToWidgetList( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLayout == EPortrait &&
                aKeyEvent.iScanCode == EStdKeyUpArrow )
            {
            // obi button -> up -> widget list bottom (portrait)
            if ( aType == EEventKey )
                SetFocusToWidgetList( iWidgetsList->Model()->NumberOfItems()-1 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLayout == ELandscape &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            // ovi button -> left -> widget list (landscape)
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLayout == ELandscapeMirrored &&
                aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // ovi button -> right -> widget list (landscape mirrored)
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        }
    else
        {
        // ------------------------------------
        // focus is NOWHERE
        // ------------------------------------
        if ( aKeyEvent.iScanCode == EStdKeyUpArrow ||
                aKeyEvent.iScanCode == EStdKeyDownArrow ||
                aKeyEvent.iScanCode == EStdKeyLeftArrow ||
                aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // no focus -> key hit -> focus list
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        }

    return keyResponse;
    }

// ---------------------------------------------------------
// CWmMainContainer::SetFocusToOviButton
// ---------------------------------------------------------
//
void CWmMainContainer::SetFocusToOviButton()
    {
    iWidgetsList->SetFocus(EFalse);
    iOviPortal->SetFocus(ETrue);
    DrawDeferred();
    UpdateFocusMode();
    }

// ---------------------------------------------------------
// CWmMainContainer::SetFocusToWidgetList
// ---------------------------------------------------------
//
void CWmMainContainer::SetFocusToWidgetList( TInt aIndex )
    {
    iOviPortal->SetFocus(EFalse);
    iWidgetsList->SetFocus(ETrue);
    if ( aIndex >= 0 && aIndex < iWidgetsList->Model()->NumberOfItems() )
        {
        iWidgetsList->SetCurrentItemIndex( aIndex );
        }
    DrawDeferred();
    UpdateFocusMode();
    }

// ---------------------------------------------------------
// CWmMainContainer::UpdateFocusMode
// ---------------------------------------------------------
//
void CWmMainContainer::UpdateFocusMode()
    {
    if ( iOviPortal->IsFocused() )
        {
        // OVI BUTTON is focused 
        iFocusMode = EOvi;
        }
    else if( ( iFindPaneIsVisible ) && 
        ( iFindbox->IsFocused() || iWidgetsList->IsFocused() ) )
        {
        // FIND MODE
        iFocusMode = EFind;
        }
    else if ( iWidgetsList->IsFocused() )
        {
        // WIDGETS LIST is focused 
        iFocusMode = EList;
        }
    else
        {
        // NO focus
        iFocusMode = ENowhere;
        }
    }

// ---------------------------------------------------------
// CWmMainContainer::FindChildControlByPoint
// ---------------------------------------------------------
//
CCoeControl* CWmMainContainer::FindChildControlByPoint(
        const TPoint& aPoint )
    {
    CCoeControl* theControl = NULL;
    CCoeControlArray::TCursor cursor = Components().Begin();
    CCoeControl* c;
    while((c=cursor.Control<CCoeControl>()) != NULL )
        {
        if(c->IsVisible())
            {
            if ( c->Rect().Contains( aPoint ) ) theControl = c;
            }
        cursor.Next();
        }
    return theControl;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CWmMainContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( !iClosingDown  )
        {
		
		if (aPointerEvent.iType == TPointerEvent::EButton1Down)
			{
	        // Check if user clicked a child control
	        CCoeControl* control = FindChildControlByPoint( aPointerEvent.iPosition );
	        if ( control && !control->IsFocused() )
	            {
	            // Set focus to the control that was clicked
	            control->SetFocus( ETrue );
	            // remove focus from ALL other child controls.
	            CCoeControlArray::TCursor cursor = Components().Begin();
	            CCoeControl* c;
	            while ((c=cursor.Control<CCoeControl>()) != NULL )
	                {
	                if ( c != control && c->IsFocused() ) c->SetFocus( EFalse );
	                cursor.Next();
	                }
	            // update focus mode accordingly
	            UpdateFocusMode();
	            // repaint
	            DrawDeferred();
	            }
			}
        
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
    }

// ---------------------------------------------------------
// CWmMainContainer::InitializeControlsL
// ---------------------------------------------------------
//
void CWmMainContainer::InitializeControlsL( const TRect& /*aRect*/ )
	{
	// Create OVI STORE button
    iOviPortal = CAknButton::NewL();
    iWmPlugin.ResourceLoader().LoadButtonL( *iOviPortal,
            R_WM_PORTAL_BUTTON );
    iOviPortal->SetMopParent( this );
    iOviPortal->SetFocusing( ETrue );
    AddControlL( iOviPortal, EOviPortal );
    iOviPortal->SetContainerWindowL( *this );
	// ToDo : replace 3-10 IID with real one when available (wk46)
	iOviPortal->SetFrameAndCenterIds( 
						KAknsIIDQgnHomeWmButton,
						KAknsIIDQgnHomeWmButtonCenter,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault );
    iOviPortal->ActivateL();
    iOviPortal->SetObserver( this );

    // Create widget list box
    iWidgetsList = CWmListBox::NewL(
            iWmPlugin,
            TRect(),
            iParent );
    iWidgetsList->SetMopParent( this );
    AddControlL( iWidgetsList, EWidgetsList );
	iWidgetsList->ActivateL();
    iWidgetsList->SetListBoxObserver( this );
    
    // Create scroll bar.
    iWidgetsList->CreateScrollBarFrameL( ETrue );
    iWidgetsList->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iWidgetsList->UpdateScrollBarsL(); 
    
    // search field
    iFindbox = CAknSearchField::NewL( *this,
                        CAknSearchField::EAdaptiveSearch,
                        0, KTextLimit );
    AddControlL( iFindbox, EFindBox );

    UpdateFocusMode();
    StartLoadingWidgetsL();
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleWidgetListChanged
// ---------------------------------------------------------
//
void CWmMainContainer::HandleWidgetListChanged()
    {
    TRAP_IGNORE( StartLoadingWidgetsL(); );
    }

// ---------------------------------------------------------
// CWmMainContainer::StartLoadingWidgetsL
// ---------------------------------------------------------
//
void CWmMainContainer::StartLoadingWidgetsL()
    {
    if ( !iWidgetLoader )
        {
        // create the widget loader AO
        iWidgetLoader = CWmWidgetLoaderAo::NewL( iWmPlugin, *iWidgetsList );
        }
    iWidgetLoader->StartLoading();
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CWmMainContainer::HandleResourceChange( TInt aType )
	{
    CCoeControl::HandleResourceChange( aType );
	
    if ( KEikDynamicLayoutVariantSwitch == aType )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( 
                            AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        
        // notify widgetlist
        iWidgetsList->HandleLayoutChanged();
        }
	}


// ---------------------------------------------------------
// CWmMainContainer::MopSupplyObject
// ---------------------------------------------------------
//
TTypeUid::Ptr CWmMainContainer::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( 
                                            aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CWmMainContainer::Draw
// ---------------------------------------------------------
//
void CWmMainContainer::Draw( const TRect& /*aRect*/ ) const
	{
	CWindowGc& gc = SystemGc();	
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, Rect() );
	}

// ---------------------------------------------------------
// CWmMainContainer::AddControlL
// ---------------------------------------------------------
//
void CWmMainContainer::AddControlL( 
                                CCoeControl* aControl, 
                                TInt aControlId )
    {
    Components().AppendLC( aControl, aControlId ); // Ownership transfered 
    CleanupStack::Pop(aControl);    
    iWmPlugin.ViewAppUi().AddToStackL( aControl );
    aControl->MakeVisible( ETrue );
    }

// ---------------------------------------------------------
// CWmMainContainer::PortalSelected
// ---------------------------------------------------------
//
TBool CWmMainContainer::PortalSelected()
    {    
    return ( iFocusMode == EOvi );
    }

// ---------------------------------------------------------
// CWmMainContainer::WidgetSelected
// ---------------------------------------------------------
//
TBool CWmMainContainer::WidgetSelected() 
    {
    return ( iFocusMode == EList ) || ( iFocusMode == EFind );
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoAdd
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoAdd()
    {
    TBool retVal = EFalse;
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( WidgetSelected() && data )
        {
        if ( data->HsContentInfo().CanBeAdded() )
            retVal = ETrue;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoUninstall
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoUninstall() 
    {
    TBool retVal( EFalse );
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( WidgetSelected() && data )
        {
        if ( data->WidgetType() == CWmWidgetData::ECps &&
                data->PublisherUid() != KNullUid )
            {
            retVal = ETrue;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoLaunch
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoLaunch() 
    {
    TBool retVal(EFalse);
    if ( WidgetSelected() )
        {
        CWmWidgetData* data = iWidgetsList->WidgetData();        
        if ( data->WidgetType() == CWmWidgetData::ECps &&
            data->PublisherUid() != KNullUid )
            {
            retVal = ETrue;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoFind
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoFind()
    {
    TBool canDo( !iFindPaneIsVisible );
    if ( canDo )
        {
        canDo = ( iFocusMode == EList || iFocusMode == ENowhere );
        }
    return canDo;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoSort
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoSort()
    {
    return !iFindPaneIsVisible;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoDetails
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoDetails()
    {
    return WidgetSelected();
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoHelp
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoHelp()
    {
    return FeatureManager::FeatureSupported( KFeatureIdHelp );
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::AddWidgetToHomeScreenL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::AddWidgetToHomeScreenL()
    {
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( data )
        {
        if ( iFindbox && iFindPaneIsVisible )
            {
            DeactivateFindPaneL();
            }

        SetExitStatus( iWmPlugin.ContentController().AddWidgetL(
            data->HsContentInfo() ) );
        
        iWmPlugin.Deactivate();
        }

    }

// ---------------------------------------------------------------------------
// CWmMainContainer::SetExitStatus
// ---------------------------------------------------------------------------
//
void CWmMainContainer::SetExitStatus( TInt aExitStatus )
    {
    iExitStatus = aExitStatus;
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::ExitStatus
// ---------------------------------------------------------------------------
//
TInt CWmMainContainer::ExitStatus( )
    {
    return iExitStatus;
    }
	
// ---------------------------------------------------------------------------
// CWmMainContainer::LaunchWidgetL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::LaunchWidgetL()
    {
    if ( CanDoLaunch() )
        {
        CWmWidgetData* data = iWidgetsList->WidgetData();
        
        HBufC* param = KNullDesC().AllocLC(); // launch params
        
        RApaLsSession appArc;
        User::LeaveIfError( appArc.Connect() ); // connect to AppArc server
        CleanupClosePushL( appArc );
        TThreadId id;
        User::LeaveIfError( appArc.StartDocument(
            *param, data->PublisherUid(), id ) );
    
        CleanupStack::PopAndDestroy( &appArc );      
        CleanupStack::PopAndDestroy( param );
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::ActivateFindPaneL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::ActivateFindPaneL()
    {
    if ( iFindbox && !iFindPaneIsVisible &&
            iWidgetsList->Model()->NumberOfItems() > KMinWidgets )
        {
		// enable filtering
        CAknFilteredTextListBoxModel* m = 
                static_cast <CAknFilteredTextListBoxModel*> ( iWidgetsList->Model() );
        if ( m )
            {
            m->CreateFilterL( iWidgetsList, iFindbox );
            if ( m->Filter() )
                {
                m->Filter()->SetParentControl( this );
                }
            }

        iFindbox->ActivateL();
        iFindbox->ResetL();
        iFindbox->SetSearchTextL( KNullDesC );
        iFindbox->SetSkinEnabledL( ETrue );

        iFindPaneIsVisible = ETrue;
        iFindbox->MakeVisible( ETrue );
        
        // layout listbox and findbox
        LayoutControlsL();        
        
        iFindbox->SetFocus( ETrue );
        iWidgetsList->SetFindPaneIsVisible( ETrue );

        // set soft key set
        CEikButtonGroupContainer* cbaGroup =
            CEikButtonGroupContainer::Current();
        cbaGroup->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_CANCEL__SELECT );
        cbaGroup->DrawNow();

        UpdateFocusMode();
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::DeactivateFindPaneL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::DeactivateFindPaneL()
    {
    if( iFindbox && iFindPaneIsVisible )
	    {
        // notify search field we're shutting down
        TKeyEvent keyEvent = { EKeyNo, EStdKeyNo, 0, 0  };
        iFindbox->OfferKeyEventL( keyEvent, EEventKey );
        
        // reset findbox
        iFindbox->ResetL();

        // reset filter
        CAknFilteredTextListBoxModel* m = 
                static_cast <CAknFilteredTextListBoxModel*> ( iWidgetsList->Model() );
        if ( m && m->Filter() )
            {
            m->Filter()->ResetFilteringL();
            m->RemoveFilter();
            }

        iFindbox->MakeVisible( EFalse );
        iFindbox->SetFocus( EFalse );
        iFindPaneIsVisible = EFalse;
        iWidgetsList->SetFindPaneIsVisible( EFalse );
        
        LayoutControlsL();
        iWidgetsList->SetFocus( ETrue );

        // set soft key set
        CEikButtonGroupContainer* cbaGroup =
            CEikButtonGroupContainer::Current();
        cbaGroup->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT );
        cbaGroup->DrawNow();

        UpdateFocusMode();
        DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::SortListAlphabeticallyL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::SortListAlphabeticallyL()
    {
    if ( iWidgetsList )
        {
        iWidgetsList->SetSortOrderL( CWmListBox::EAlphabetical );
       
        //store changed list order
        CWmPersistentWidgetOrder* widgetOrder =
            CWmPersistentWidgetOrder::NewL( iWmPlugin.FileServer() );
        CleanupStack::PushL( widgetOrder );
        widgetOrder->StoreL( iWidgetsList->WidgetDataArray() );
        CleanupStack::PopAndDestroy( widgetOrder );

        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::UninstallWidgetL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::UninstallWidgetL()
    {
    if ( CanDoUninstall() )
        {
        CWmWidgetData* data = iWidgetsList->WidgetData();
        
        SwiUI::RSWInstLauncher installer;
        User::LeaveIfError( installer.Connect() );
        
        installer.Uninstall( data->PublisherUid(), KWrtMime );
        installer.Close();
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::OpenOviPortalL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::OpenOviPortalL()
    {
    _LIT( KParam, "{\"Parameters\": {\"cmd\": \"channel\", \"id\": \"tag:mosh.nokia.com,2009-04:atom/2.0/channels/10\" }}" );
    _LIT( KHtmlFile, "C:\\private\\102829a0\\com.nokia.ovi.juniper.WRTClient\\Juniper\\Main.html" );
    _LIT( KFileName, "securewidgetui.exe" );
    _LIT( KUrl, "4 https://store.ovi.com/applications/");
    
    RApaLsSession appArcSession;
    User::LeaveIfError( appArcSession.Connect() );
    CleanupClosePushL( appArcSession );
    
    HBufC* param = HBufC::NewLC( KParam().Length() );
    param->Des().Copy(KParam);
    
    TThreadId id;
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC(); 
    cmdLine->SetCommandL( EApaCommandOpen ); 
    cmdLine->SetExecutableNameL( KFileName );
    cmdLine->SetDocumentNameL( KHtmlFile );
    HBufC8* buf8 = HBufC8::NewLC( param->Des().Length() );
    buf8->Des().Copy( *param );
    cmdLine->SetOpaqueDataL( *buf8 );
    cmdLine->SetTailEndL( *buf8 );
    
    TInt err = appArcSession.StartApp( *cmdLine, id );
    if ( err == KErrNone )
        {
        _LIT( KSchemeTemplate, "ecwrtservices:command=launch&app=com.nokia.ovi.juniper.WRTClient&commandLine=");
        const TInt KSecureWidgetWebAppSchemeHandlerUid = 0x102829B2;
        
        // Create scheme url to launch the widget
        HBufC* schemeUrl = HBufC::NewLC( KSchemeTemplate().Length() + 
                param->Des().Length() );
        
        schemeUrl->Des().Copy( KSchemeTemplate );
        schemeUrl->Des().Append( *param );
        
        // Initialize scheme handler plugin
        TUid dtorId;
        CSchemeHandler* schemeHandler = reinterpret_cast<CSchemeHandler*>(
                    REComSession::CreateImplementationL( 
                            TUid::Uid( KSecureWidgetWebAppSchemeHandlerUid ), 
                            dtorId, schemeUrl ) );
        CleanupStack::PushL( schemeHandler );
        
        // Handle the scheme url
        schemeHandler->HandleUrlEmbeddedL();
        
        CleanupStack::PopAndDestroy( schemeHandler );
        REComSession::FinalClose();
        CleanupStack::PopAndDestroy( schemeUrl );     
        }
    else
        {
        // Ovi store not found start browser
        const TUid KOSSBrowserUidValue = { 0x10008D39 };
        HBufC* param = HBufC::NewLC( KUrl().Length() );
        param->Des().Copy( KUrl );
        TUid id( KOSSBrowserUidValue );
        
        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
        TApaTask task = taskList.FindApp(id);
        if( task.Exists() )
            {
            task.BringToForeground();
            HBufC8* param8 = HBufC8::NewLC(param->Length());
            param8->Des().Append(*param);
            task.SendMessage(TUid::Uid(0), *param8); // UID not used
            CleanupStack::PopAndDestroy(param8);
            }
        else
            {
            if( !appArcSession.Handle() )
                {
                User::LeaveIfError( appArcSession.Connect() );
                }
            TThreadId thread;
            User::LeaveIfError(appArcSession.StartDocument(*param, KOSSBrowserUidValue, thread));
            appArcSession.Close();   
            }
        CleanupStack::PopAndDestroy( param );
        }
    
    CleanupStack::PopAndDestroy( buf8 );
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy( param );
    CleanupStack::PopAndDestroy( &appArcSession );

    // unfocus OVI button
	iOviPortal->SetFocus( EFalse );
	iOviPortal->DrawDeferred();
    UpdateFocusMode();
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::SelectL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::SelectL()
    {
    if ( WidgetSelected() )
        AddWidgetToHomeScreenL();
    else if ( PortalSelected() )
        OpenOviPortalL();
    else
        SetFocusToWidgetList();
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::ShowHelpL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::ShowHelpL()
    {
    TUid fwUid = TUid::Uid( AI_UID3_AIFW_COMMON );
    // retrieve resolve host application help context
    CArrayFix<TCoeHelpContext>* helpContext =
        iEikonEnv->EikAppUi()->AppHelpContextL();

    // add the widget manager entry into the help context
    CleanupStack::PushL( helpContext );
    helpContext->InsertL( 0, TCoeHelpContext(
            fwUid, KWM_HLP_WIDGET_COLLECTION ) );
    CleanupStack::Pop( helpContext );

    // launch help
    HlpLauncher::LaunchHelpApplicationL(
          iEikonEnv->WsSession(),
          helpContext );
    helpContext = 0; // HlpLauncher takes ownership of helpContext
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::RemoveCtrlsFromStack
// ---------------------------------------------------------------------------
//
void CWmMainContainer::RemoveCtrlsFromStack()
    {    
    for ( TInt i=0; i < CountComponentControls(); i++ ) 
        {
        CCoeControl* ctl = ComponentControl( i ) ;
        if ( ctl )
            {
            iWmPlugin.ViewAppUi().RemoveFromStack( ctl );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::HandleListBoxEventL(
        CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( !iClosingDown && ( aEventType == EEventEnterKeyPressed ||
            aEventType == EEventItemSingleClicked ) )
        {
        AddWidgetToHomeScreenL();
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::HandleControlEventL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::HandleControlEventL(
        CCoeControl* aControl, TCoeEvent aEventType )
    {
    if ( !iClosingDown && aControl == iOviPortal && aEventType == EEventStateChanged )
        {
        OpenOviPortalL();
        }
    }

// ----------------------------------------------------
// CWmMainContainer::LaunchDetailsDialogL
// ----------------------------------------------------
//
void CWmMainContainer::LaunchDetailsDialogL()
    {
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( data )
        {
        const CFbsBitmap* logo = ( data->LogoImage() ) ? 
                    data->LogoImage() : iWidgetsList->DefaultLogo();
        const CFbsBitmap* mask = ( data->LogoImageMask() ) ? 
                    data->LogoImageMask() : iWidgetsList->DefaultMask();
        
        TPtrC description = ( data->Description().Length() > 0 ) ? 
            data->Description() : iWmPlugin.ResourceLoader().NoDescriptionText();
        CWmDetailsDlg* dlg = CWmDetailsDlg::NewL(
                data->Name(), description, 
                data->HsContentInfo().CanBeAdded(),
                logo, mask, iBgContext );

        if ( dlg && dlg->ExecuteLD() == ECbaAddToHs )
            {
            AddWidgetToHomeScreenL();
            }
        }
    }

// ----------------------------------------------------
// CWmMainContainer::SetClosingDown
// ----------------------------------------------------
//
void CWmMainContainer::SetClosingDown( TBool aClosingDown )
    {
    iClosingDown = aClosingDown;
    }

// ----------------------------------------------------
// CWmMainContainer::ClosingDown
// ----------------------------------------------------
//
TBool CWmMainContainer::ClosingDown()
    {
    return iClosingDown;
    }

// ----------------------------------------------------
// CWmMainContainer::HandleFindSizeChanged
// ----------------------------------------------------
//
void CWmMainContainer::HandleFindSizeChanged()
    {    
    TAknWindowLineLayout findPane = 
            AknLayoutScalable_Avkon::find_pane().LayoutLine();

    TAknWindowLineLayout listPane;

    TRect listRect = iWidgetsList->Rect();
    listPane.il = listRect.iTl.iX;
    listPane.it = listRect.iTl.iY;
    listPane.ib = 0;
    listPane.iH = TUint16( listRect.Height() - ( findPane.iH - 1 ) );
    listPane.iW = listRect.Width();
    listPane.ir = ELayoutEmpty;
    findPane.iW = listPane.iW;

    TRect rect = Rect();
    AknLayoutUtils::LayoutControl( iFindbox, rect, findPane );
    AknLayoutUtils::LayoutControl( iWidgetsList, rect, listPane );
    
    iWidgetsList->DrawNow();
    }


// End of File

