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
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout.lag>
#include <AknsSkinInstance.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <aknenv.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <aknsfld.h>
#include <AknsBasicBackgroundControlContext.h>
#include <apgcli.h>
#include <widgetmanager.mbg>
#include <avkon.rsg>
#include <coecobs.h>
#include <coecntrl.h>
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
#include "wmportalbutton.h"
#include "wmwidgetloaderao.h"
#include "wmconfiguration.h"
#include "wminstaller.h"

// CONSTANTS
const TInt KTextLimit = 40; // Text-limit for find-field
const TInt KMinWidgets = 1; // minimum number of widgets to show findpane

// ---------------------------------------------------------
// CWmMainContainer::CWmMainContainer()
// ---------------------------------------------------------
//
CWmMainContainer::CWmMainContainer( CWmPlugin& aWmPlugin ) :
    iWmPlugin( aWmPlugin )
	{
    iWidgetsList = NULL;
    iFindbox = NULL;
    iFindPaneIsVisible = EFalse;
    iBgContext = NULL;
    iFocusMode = ENowhere;
    iClosingDown = ETrue;
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
    iPortalButtonOne = NULL;
    iPortalButtonTwo = NULL;
    iFindbox = NULL;
	delete iBgContext;
	delete iConfiguration;
	}

// ---------------------------------------------------------
// CWmMainContainer::NewL
// ---------------------------------------------------------
//
CWmMainContainer* CWmMainContainer::NewL( 
    const TRect& aRect,
    CWmPlugin& aWmPlugin )
	{
	CWmMainContainer* self = CWmMainContainer::NewLC( 
        aRect, 
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
    CWmPlugin& aWmPlugin )
	{
	CWmMainContainer* self = new (ELeave) CWmMainContainer( aWmPlugin );
	CleanupStack::PushL( self );
	self->ConstructL( aRect );
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
		const TRect& aRect )
	{  
    // Initialize control array
	InitComponentArrayL();
	Components().SetControlsOwnedExternally( ETrue );

    // create the UI
    CreateWindowL();

	// background context
	iBgContext = CAknsBasicBackgroundControlContext::NewL( 
	        KAknsIIDQsnBgScreen, ScreenRect() , ETrue);

    // load configuration
    iConfiguration = CWmConfiguration::NewL( iWmPlugin.ResourceLoader() );
    
	// set up controls
	InitializeControlsL( aRect );

	// set size and activate
	SetRect( aRect );
    ActivateL();
	
	}

// ---------------------------------------------------------
// CWmMainContainer::SizeChanged
// ---------------------------------------------------------
//
void CWmMainContainer::SizeChanged()
	{
	CCoeControl::SizeChanged();
    iBgContext->SetRect( ScreenRect() );
	LayoutControls();
	}

// ---------------------------------------------------------
// CWmMainContainer::LayoutControls
// ---------------------------------------------------------
//
void CWmMainContainer::LayoutControls()
	{
    TRect rect( Rect() );

    // determine layout type
    iLandscape = Layout_Meta_Data::IsLandscapeOrientation();
    iMirrored = Layout_Meta_Data::IsMirrored();
    
    // layout iPortalButtons
	if ( iConfiguration->PortalButtonCount() == 1 )
	    {
	    // one button
	    TAknWindowLineLayout btnPane = AknLayoutScalable_Apps
	       ::wgtman_btn_pane( iLandscape ? 1 : 0 ).LayoutLine();
	    AknLayoutUtils::LayoutControl( iPortalButtonOne, rect, btnPane );
	    }
	else
	    {
	    // two buttons
        TInt variety = (iLandscape ? 3 : 2);
        TAknWindowLineLayout oviBtnLayout = AknLayoutScalable_Apps
                ::wgtman_btn_pane( variety ).LayoutLine();
        TAknWindowLineLayout operatorBtnLayout = AknLayoutScalable_Apps
                ::wgtman_btn_pane_cp_01( variety ).LayoutLine();
        AknLayoutUtils::LayoutControl( iPortalButtonOne, rect, oviBtnLayout );
        AknLayoutUtils::LayoutControl( iPortalButtonTwo, rect, operatorBtnLayout );
	    }
    
	// layout iWidgetsList
    TAknWindowLineLayout listPane = AknLayoutScalable_Apps
        ::listscroll_wgtman_pane( iLandscape ? 1 : 0 ).LayoutLine();
    if( iFindbox && iFindPaneIsVisible )
        {
		TAknLayoutRect layoutRect;
        layoutRect.LayoutRect( rect, listPane );
		iWidgetsList->SetRect( layoutRect.Rect() );
        HandleFindSizeChanged();
        }
    else
        {
        AknLayoutUtils::LayoutControl( iWidgetsList, rect, listPane );
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

    // This is a bug fix for ou1cimx1#217716 & ou1cimx1#217667.
    // For some weird reason homescreen is genarating one extra EEventKey 
    // when using Nokia SU-8W bluetooth keyboard & backspace key. This if is to 
    // ignore that event. Extra event allways has iModifiers set to 
    // EModifierAutorepeatable.
    if ( aType == EEventKey && 
             aKeyEvent.iScanCode == EStdKeyBackspace && 
             aKeyEvent.iModifiers == EModifierAutorepeatable )
        {
        return EKeyWasConsumed;
        }
    
    // Handle search keyevent
    keyResponse = HandleSearchKeyEventL( aKeyEvent, aType );
    
    // Move focus between controls
    if ( keyResponse == EKeyWasNotConsumed )
        {
        keyResponse = MoveFocusByKeys( aKeyEvent, aType );
        }
    
    // Handle list keyevent
    if ( keyResponse == EKeyWasNotConsumed )
        {
        keyResponse = HandleListKeyEventL( aKeyEvent, aType );
        }
    
    // Handle buttons keyevent
    if ( keyResponse == EKeyWasNotConsumed )
        {
        keyResponse = HandleButtonKeyEventL( aKeyEvent, aType );
        }
    
    // Update ui if needed 
    if ( keyResponse == EKeyWasConsumed )
        {
        DrawDeferred();
        }
    
    // Do not let UI framework forward the keys to child controls as
    // we have already done that.
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleSearchKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::HandleSearchKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    // open search field with alpha digit numbers
    if ( aType == EEventKeyDown && !iFindPaneIsVisible && 
            aKeyEvent.iScanCode < EStdKeyF1 &&
        TChar( aKeyEvent.iScanCode ).IsAlphaDigit() )
        {
        ActivateFindPaneL( EFalse );
        
        if ( iFindPaneIsVisible )
            {
            return EKeyWasConsumed;
            }
        }
    
    if ( iFindPaneIsVisible && aType == EEventKey )
        {
        if ( aKeyEvent.iScanCode == EStdKeyNo || aKeyEvent.iCode == EKeyNo )
            {
            DeactivateFindPaneL();
            return EKeyWasConsumed;
            }
        
        TBool needsRefresh( EFalse );
       
        if ( iFindbox->TextLength() == 0 
                && aKeyEvent.iScanCode == EStdKeyBackspace )
            {
            // if lenght is 0 and backspace is pressed AknFind will deactivate
            // searchbox so we don't want to pass this event to AknFind
            keyResponse = EKeyWasConsumed;
            }
        else
            {
            keyResponse = AknFind::HandleFindOfferKeyEventL( 
                    aKeyEvent, aType, this, 
                    iWidgetsList, iFindbox, ETrue, needsRefresh );
            }
        if ( needsRefresh )
            {
            DrawNow();
            }
        }

    return keyResponse;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleListKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::HandleListKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    // pass key event except backpace or delete key event to widgets list if focused
    if ( iWidgetsList->IsFocused() )
        {
        if ( ( aType == EEventKey ) && ( aKeyEvent.iCode == EKeyBackspace 
                || aKeyEvent.iCode == EKeyDelete ) )
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
            keyResponse = EKeyWasConsumed;
            }
        else 
            {
            //passing to listbox handler
            keyResponse = iWidgetsList->OfferKeyEventL( 
                    aKeyEvent, aType );
            }
        }
    
    return keyResponse;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleButtonKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::HandleButtonKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    // pass key event to portal button if focused
    if ( iPortalButtonOne->IsFocused() )
        {
        keyResponse = iPortalButtonOne->OfferKeyEventL(
                            aKeyEvent, aType );
        }

    // pass key event to the other portal button if exists and focused
    if ( iPortalButtonTwo && iPortalButtonTwo->IsFocused() )
        {
        keyResponse = iPortalButtonTwo->OfferKeyEventL(
                            aKeyEvent, aType );
        }
    
    return keyResponse;
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
        if ( !iLandscape &&
                aKeyEvent.iScanCode == EStdKeyUpArrow &&
                iWidgetsList->CurrentItemIndex() == 0 )
            {
            // widget list top -> up -> ovi button (portrait)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape &&
                aKeyEvent.iScanCode == EStdKeyDownArrow &&
                iWidgetsList->CurrentItemIndex() ==
                   iWidgetsList->Model()->NumberOfItems() - 1 )
            {
            // widget list bottom -> down -> ovi button (portrait)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape && !iMirrored &&
                aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // widget list -> right -> ovi button (landscape normal)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape && iMirrored &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            // widget list -> left -> ovi button (landscape mirrored)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        }
    else if ( iPortalButtonOne->IsFocused() )
        {
        // ------------------------------------
        // focus is in the FIRST PORTAL BUTTON
        // ------------------------------------
        if ( !iLandscape &&
                aKeyEvent.iScanCode == EStdKeyDownArrow )
            {
            // left portal -> down -> widget list top (portrait)
            if ( aType == EEventKey )
                SetFocusToWidgetList( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape &&
                aKeyEvent.iScanCode == EStdKeyUpArrow )
            {
            // left portal -> up -> widget list bottom (portrait)
            if ( aType == EEventKey )
                SetFocusToWidgetList( iWidgetsList->Model()->NumberOfItems()-1 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape && !iMirrored &&
                aKeyEvent.iScanCode == EStdKeyRightArrow &&
                iConfiguration->PortalButtonCount() > 1 )
            {
            // left portal -> right -> right portal (portrait normal)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 1 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape && iMirrored &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow &&
                iConfiguration->PortalButtonCount() > 1 )
            {
            // right portal -> left -> left portal (portrait mirrored)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 1 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape && !iMirrored &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            // upper portal -> left -> widget list (landscape normal)
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape && iMirrored &&
                aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // upper portal -> right -> widget list (landscape mirrored)
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape &&
                aKeyEvent.iScanCode == EStdKeyDownArrow &&
                iConfiguration->PortalButtonCount() > 1 )
            {
            // upper portal -> down -> lower portal (landscape)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 1 );
            keyResponse = EKeyWasConsumed;
            }
        }
    else if ( iPortalButtonTwo && iPortalButtonTwo->IsFocused() )
        {
        // ------------------------------------
        // focus is in the SECOND PORTAL BUTTON
        // ------------------------------------
        if ( !iLandscape &&
                aKeyEvent.iScanCode == EStdKeyDownArrow )
            {
            // right portal -> down -> widget list top (portrait)
            if ( aType == EEventKey )
                SetFocusToWidgetList( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape &&
                aKeyEvent.iScanCode == EStdKeyUpArrow )
            {
            // right portal -> up -> widget list bottom (portrait)
            if ( aType == EEventKey )
                SetFocusToWidgetList( iWidgetsList->Model()->NumberOfItems()-1 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape && !iMirrored &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow &&
                iConfiguration->PortalButtonCount() > 1 )
            {
            // right portal -> left -> left portal (portrait normal)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( !iLandscape && iMirrored &&
                aKeyEvent.iScanCode == EStdKeyRightArrow &&
                iConfiguration->PortalButtonCount() > 1 )
            {
            // left portal -> right -> right portal (portrait mirrored)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape && !iMirrored &&
                aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            // lower portal -> left -> widget list (landscape normal)
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape && iMirrored &&
                aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // lower portal -> right -> widget list (landscape mirrored)
            if ( aType == EEventKey )
                SetFocusToWidgetList();
            keyResponse = EKeyWasConsumed;
            }
        else if ( iLandscape &&
                aKeyEvent.iScanCode == EStdKeyUpArrow )
            {
            // lower portal -> up -> upper portal (landscape)
            if ( aType == EEventKey )
                SetFocusToPortalButton( 0 );
            keyResponse = EKeyWasConsumed;
            }
        }
    else
        {
        // ------------------------------------
        // focus is NOWHERE
        // ------------------------------------
        if ( aKeyEvent.iScanCode == EStdKeyUpArrow ||
            aKeyEvent.iScanCode == EStdKeyDownArrow )
            {
            // no focus -> key hit -> focus list
            if ( aType == EEventKey )
                {
                SetFocusToWidgetList();
                keyResponse = EKeyWasNotConsumed;
                }
            }
        }

    return keyResponse;
    }

// ---------------------------------------------------------
// CWmMainContainer::SetFocusToPortalButton
// ---------------------------------------------------------
//
void CWmMainContainer::SetFocusToPortalButton( TInt aIndex )
    {
    if ( aIndex != 0 && iPortalButtonTwo )
        {
        iWidgetsList->SetFocus(EFalse);
        iPortalButtonOne->SetFocus(EFalse);
        iPortalButtonTwo->SetFocus(ETrue);
        }
    else
        {
        iWidgetsList->SetFocus(EFalse);
        if ( iPortalButtonTwo )
            iPortalButtonTwo->SetFocus(EFalse);
        iPortalButtonOne->SetFocus(ETrue);
        }
    DrawDeferred();
    UpdateFocusMode();
    }

// ---------------------------------------------------------
// CWmMainContainer::SetFocusToWidgetList
// ---------------------------------------------------------
//
void CWmMainContainer::SetFocusToWidgetList( TInt aIndex )
    {
    iPortalButtonOne->SetFocus(EFalse);
    if ( iPortalButtonTwo )
        iPortalButtonTwo->SetFocus(EFalse);
    if ( aIndex >= 0 && aIndex < iWidgetsList->Model()->NumberOfItems() )
        {
        iWidgetsList->SetCurrentItemIndex( aIndex );
        }
    iWidgetsList->SetFocus(ETrue);
    DrawDeferred();
    UpdateFocusMode();
    }

// ---------------------------------------------------------
// CWmMainContainer::UpdateFocusMode
// ---------------------------------------------------------
//
void CWmMainContainer::UpdateFocusMode()
    {
    if ( iPortalButtonOne->IsFocused() )
        {
        // PORTAL BUTTON is focused
        iFocusMode = EPortal;
        }
    else if ( iPortalButtonTwo && iPortalButtonTwo->IsFocused() )
        {
        // SECOND PORTAL BUTTON is focused
        iFocusMode = EPortal;
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
	            // remove focus from ALL other child controls.
	            CCoeControlArray::TCursor cursor = Components().Begin();
	            CCoeControl* c;
	            while ((c=cursor.Control<CCoeControl>()) != NULL )
	                {
	                if ( c != control && c->IsFocused() ) c->SetFocus( EFalse );
	                cursor.Next();
	                }
	            
	            // Set focus to the control that was clicked
	            control->SetFocus( ETrue );
	                            
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
	// Create portal buttons
    iPortalButtonOne = CWmPortalButton::NewL( this, 0 );
    iPortalButtonOne->SetMopParent( this );    
    AddControlL( iPortalButtonOne, EPortalOne );
    if ( iConfiguration->PortalButtonCount() > 1 )
        {
        iPortalButtonTwo = CWmPortalButton::NewL( this, 1 );
        iPortalButtonTwo->SetMopParent( this );    
        AddControlL( iPortalButtonTwo, EPortalTwo );
        }
    
    // Create widget list box
    iWidgetsList = CWmListBox::NewL(
            iWmPlugin,
            TRect(),
            this );
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
    else if ( KAknsMessageSkinChange == aType )
        {
        // notify widgetlist , colored add icon need to be updated 
        iWidgetsList->HandleSkinChanged();
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
    CAknView* view = iWmPlugin.ViewAppUi().View( 
            TUid::Uid(EWmMainContainerViewId) );
    if ( view )
		{
        iWmPlugin.ViewAppUi().AddToStackL( *view, aControl );
		}
	else
		{
        iWmPlugin.ViewAppUi().AddToStackL( aControl );
		}
    aControl->MakeVisible( ETrue );
    }

// ---------------------------------------------------------
// CWmMainContainer::PortalSelected
// ---------------------------------------------------------
//
TBool CWmMainContainer::PortalSelected()
    {    
    return ( iFocusMode == EPortal );
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
    if ( WidgetSelected() && data && !data->IsUninstalling() )
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
    if ( WidgetSelected() && data && !data->IsUninstalling() )
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
        if ( data && !data->IsUninstalling() )
            {
            if ( data->WidgetType() == CWmWidgetData::ECps &&
                data->PublisherUid() != KNullUid )
                {
                retVal = ETrue;
                }
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
    return !iFindPaneIsVisible;
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
    return ( WidgetSelected() && iWidgetsList->WidgetData() 
            && !iWidgetsList->WidgetData()->IsUninstalling() );
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
    if ( !iClosingDown && data && !data->IsUninstalling() )
        {
        if ( iFindbox && iFindPaneIsVisible )
            {
            DeactivateFindPaneL();
            }

        // set add to homescreen to be executed later
        iWmPlugin.SetPostponedCommandL(
            CWmPlugin::EAddToHomescreen,
            data->HsContentInfo() );

        // check if we can add any widgets to hs. 
        TBool hsContentFull = ETrue;
        for ( TInt i=0; i<iWidgetsList->WidgetDataCount(); i++ )
            {
            CHsContentInfo& info = iWidgetsList->WidgetData(i).HsContentInfo();
            if ( info.CanBeAdded() ) 
                {
                hsContentFull = EFalse;
                break;
                }
            }
        
        // deactivate wm if there's not enough space to add widget to hs.
        if ( !data->HsContentInfo().CanBeAdded() && !hsContentFull )
            {
            iWmPlugin.ExecuteCommandL();
            }
        else
            {
            iWmPlugin.CloseView();
            }
        }
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
void CWmMainContainer::ActivateFindPaneL( TBool aActivateAdabtive )
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
        LayoutControls();        
        
        if ( aActivateAdabtive )
            {
            iFindbox->ShowAdaptiveSearchGrid();
            }
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
        
        LayoutControls();

        // set soft key set
        CEikButtonGroupContainer* cbaGroup =
            CEikButtonGroupContainer::Current();
        TInt cbaResourceId = ( AknLayoutUtils::MSKEnabled() ?
                                    R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT : 
                                    R_AVKON_SOFTKEYS_OPTIONS_BACK );

        cbaGroup->SetCommandSetL( cbaResourceId );
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
        iWidgetsList->DoSortToVisibleArray( CWmListBox::EAlphabetical );
        iWidgetsList->DoSortToOrderData( CWmListBox::EAlphabetical );
       
        //store changed list order
        CWmPersistentWidgetOrder* widgetOrder =
            CWmPersistentWidgetOrder::NewL( iWmPlugin.FileServer() );
        CleanupStack::PushL( widgetOrder );
        widgetOrder->StoreL( iWidgetsList->OrderDataArray() );
		
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
        if ( data )
            {
            if ( iWmPlugin.ResourceLoader().QueryPopupL( 
                    R_QTN_WM_UNINSTALL_WIDGET_QUERY, data->Name() ) )
                {
                
                TRAPD( err, iWmPlugin.WmInstaller().UninstallL( data ) );
                if ( err != KErrNone )
                    {
                    iWmPlugin.ResourceLoader().InfoPopupL( 
                            R_QTN_WM_UNINST_PROCESS_BUSY, KNullDesC );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::OpenPortalL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::OpenPortalL()
    {
    if ( !iClosingDown )
        {
        // execute whichever of the portal buttons happens to be active
        if ( iPortalButtonOne->IsFocused() )
            iPortalButtonOne->ExecuteL();
        else if ( iPortalButtonTwo && iPortalButtonTwo->IsFocused() )
            iPortalButtonTwo->ExecuteL();
        else if ( !iPortalButtonTwo )
            iPortalButtonOne->ExecuteL();
        }
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
        OpenPortalL();
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
    if ( ( aEventType == EEventEnterKeyPressed ||
           aEventType == EEventItemSingleClicked ) 
           && !iClosingDown )    
        {
        AddWidgetToHomeScreenL();
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
        
        CWmDetailsDlg* dlg = CWmDetailsDlg::NewL(
                data->Name(), data->Description(), 
                data->HsContentInfo().CanBeAdded(),
                logo, mask );

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
// CWmMainContainer::Configuration
// ----------------------------------------------------
//
CWmConfiguration& CWmMainContainer::Configuration()
    {
    return *iConfiguration;
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
    
    // set findpane width equal to listwidth
    findPane.iW = listPane.iW;

    TRect rect = Rect();
    AknLayoutUtils::LayoutControl( iFindbox, rect, findPane );
    AknLayoutUtils::LayoutControl( iWidgetsList, rect, listPane );
    
    iWidgetsList->DrawNow();
    }

// ----------------------------------------------------
// CWmMainContainer::ProcessForegroundEvent
// ----------------------------------------------------
//
void CWmMainContainer::ProcessForegroundEvent( TBool aForeground )
    {
    if ( aForeground )
        {
        // set init state when wm comes to foreground.
		// remove focus from all controls when activating view.
        CCoeControl* control = NULL;
        CCoeControlArray::TCursor cursor = Components().Begin();
        while( ( control = cursor.Control<CCoeControl>() ) != NULL )
            {
            if( control->IsVisible() && control->IsFocused() )
                {
                control->SetFocus( EFalse, EDrawNow );
                }
            cursor.Next();
            }
        UpdateFocusMode();
        }
    }

// End of File

