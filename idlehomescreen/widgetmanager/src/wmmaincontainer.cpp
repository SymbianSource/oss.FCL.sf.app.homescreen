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
#include <apgicnfl.h>
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
#include <SWInstApi.h> //installer
#include <widgetregistryclient.h> // widgetreqistry
#include <schemehandler.h> // for starting the OVI client
#include <featmgr.h>     // FeatureManager
#include <hlplch.h>      // HlpLauncher
#include <csxhelp/hmsc.hlp.hrh>
#include <aisystemuids.hrh>
#include <centralrepository.h>

#include "wmcrkeys.h"
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

// CONSTANTS
const TInt KTextLimit = 40; // Text-limit for find-field
const TInt KMinWidgets = 1; // minimum number of widgets to show findpane
_LIT8( KWrtMime, "application/x-nokia-widget");
_LIT( KBrowserPrefix, "4 ");

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
    delete iOviStoreUrl;
    delete iOviStoreClientBundleId;
    delete iOviStoreClientParam;
        
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

	// Read needed values from cenrep
	TRAP_IGNORE( FetchRepositoryDataL(); );
	
	// set up controls
	InitializeControlsL( aRect );

	// set size and activate
	SetRect( aRect );
    ActivateL();
	
	}

// ---------------------------------------------------------
// CWmMainContainer::FetchRepositoryDataL
// ---------------------------------------------------------
//
void CWmMainContainer::FetchRepositoryDataL()
    {
    iOviStoreUrl = NULL;
    iOviStoreClientBundleId = NULL;
    iOviStoreClientParam = NULL;
    
    CRepository *repository = CRepository::NewLC( 
            TUid::Uid( KCrWidgetManagerm ) );
    TInt err = KErrNone;
    
    //read localized data from cenrep
    TLanguage sysLang = User::Language();
    for( TUint32 i = KLangId0; i <= KLangId9; i+=4 )
        {
        TInt crLang = 0;
        err = repository->Get( i, crLang );

        if ( sysLang == crLang && err == KErrNone )
            {
            // system langauge matches langauge in cenrep
            iOviStoreUrl = HBufC::NewL( 
                    NCentralRepositoryConstants::KMaxUnicodeStringLength );
            TPtr ptr( iOviStoreUrl->Des() );
            
            // get localized ovi store url
            err = repository->Get( i + KOviStoreBrowserUrlOffset, ptr );
            if ( err != KErrNone )
                {
                delete iOviStoreUrl;
                iOviStoreUrl = NULL;
                }
            }
        }
    
    //read other data from cenrep
    iOviStoreClientBundleId = HBufC::NewL( 
            NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr ptr( iOviStoreClientBundleId->Des() );
    err = repository->Get( KOviStoreBunbleId, ptr );
    if ( err != KErrNone )
        {
        delete iOviStoreClientBundleId;
        iOviStoreClientBundleId = NULL;
        }
    
    iOviStoreClientParam = HBufC::NewL( 
            NCentralRepositoryConstants::KMaxUnicodeStringLength );
    ptr.Set( iOviStoreClientParam->Des() );
    err = repository->Get( KOviStoreClientParam, ptr );
    if ( err != KErrNone )
        {
        delete iOviStoreClientParam;
        iOviStoreClientParam = NULL;
        }
        
    CleanupStack::PopAndDestroy( repository );
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
    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();

    TAknWindowLineLayout listPane = 
                AknLayoutScalable_Apps::listscroll_wgtman_pane(landscape ? 1 : 0).LayoutLine();

    TAknWindowLineLayout btnPane = 
                AknLayoutScalable_Apps::wgtman_btn_pane(landscape ? 1 : 0).LayoutLine();

	TRect rect = Rect();	
    
	if ( landscape )
	    {
        if ( Layout_Meta_Data::IsMirrored() )
            {
            iLayout = ELandscapeMirrored;
            }
        else
            {
            iLayout = ELandscape;
            }
	    }
	else
	    {
        iLayout = EPortrait;
	    }
	
    // layout Ovi button
    AknLayoutUtils::LayoutControl( iOviPortal, rect, btnPane );
    
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
	            // set focus to selected list box item.           
                if ( control == iWidgetsList )
                    {
                    TInt itemIndex = iWidgetsList->CurrentListBoxItemIndex();
                    TBool itemPointed = iWidgetsList->View()->XYPosToItemIndex(
                            aPointerEvent.iParentPosition,
                            itemIndex );
                    if ( itemIndex >= 1 && itemPointed )
                        {
                        iWidgetsList->SetCurrentItemIndex( itemIndex - 1 );
                        }
                    }

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
    HBufC* oviText = StringLoader::LoadLC( R_QTN_WM_GO_TO_OVI_STORE );    
    iOviPortal = CWmPortalButton::NewL( this, *oviText );
    CleanupStack::PopAndDestroy( oviText );
    
    iOviPortal->SetMopParent( this );    
    AddControlL( iOviPortal, EOviPortal );
    
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
    iWmPlugin.ViewAppUi().AddToStackL( aControl );
    aControl->MakeVisible( ETrue );
    }

// ---------------------------------------------------------
// CWmMainContainer::WmPlugin
// ---------------------------------------------------------
//
CWmPlugin& CWmMainContainer::WmPlugin()
    {    
    return iWmPlugin;
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
    if ( !iClosingDown && data )
        {
        if ( iFindbox && iFindPaneIsVisible )
            {
            DeactivateFindPaneL();
            }

        // set add to homescreen to be executed later
        iWmPlugin.SetPostponedCommandL(
                CWmPlugin::EAddToHomescreen, data->HsContentInfo() );

        iWmPlugin.Deactivate();
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
        LayoutControls();        
        
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
    if ( !iClosingDown )
        {
        RApaLsSession session;
        User::LeaveIfError( session.Connect() );
        CleanupClosePushL( session );
        TApaAppInfo appInfo;
        TUid launchUid;

        // Get ovi store uid    
        RWidgetRegistryClientSession widgetSession;
        User::LeaveIfError( widgetSession.Connect() );    
        CleanupClosePushL( widgetSession );
        
        launchUid.iUid = widgetSession.GetWidgetUidL( *iOviStoreClientBundleId );    
        CleanupStack::PopAndDestroy( &widgetSession );

        TInt err = session.GetAppInfo( appInfo, launchUid );
        if ( err == KErrNone )
            {
            CApaCommandLine* commandLine = CApaCommandLine::NewLC();
            commandLine->SetExecutableNameL(appInfo.iFullName); 
            HBufC8* buf8 = HBufC8::NewLC( iOviStoreClientParam->Des().Length() );
            buf8->Des().Copy( *iOviStoreClientParam );
                    
            //cmdLine->SetOpaqueDataL( *buf8 );
            commandLine->SetTailEndL( *buf8 );
            err = session.StartApp( *commandLine );
            CleanupStack::PopAndDestroy( buf8 );
            CleanupStack::PopAndDestroy( commandLine );
            }
        else
            {
            if( iOviStoreUrl )
                {
                // Ovi store not found start browser
                const TUid KOSSBrowserUidValue = { 0x10008D39 };
                HBufC* param = HBufC::NewLC( iOviStoreUrl->Length() + 
                        KBrowserPrefix().Length() );
                
                param->Des().Copy( KBrowserPrefix );
                param->Des().Append( *iOviStoreUrl );
                
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
                    if( !session.Handle() )
                        {
                        User::LeaveIfError( session.Connect() );
                        }
                    TThreadId thread;
                    User::LeaveIfError(session.StartDocument(*param, KOSSBrowserUidValue, thread));
                      
                    }
                CleanupStack::PopAndDestroy( param );
                }
            }
        CleanupStack::PopAndDestroy( &session );
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
    
    // set findpane width equal to listwidth
    findPane.iW = listPane.iW;

    TRect rect = Rect();
    AknLayoutUtils::LayoutControl( iFindbox, rect, findPane );
    AknLayoutUtils::LayoutControl( iWidgetsList, rect, listPane );
    
    iWidgetsList->DrawNow();
    }


// End of File

