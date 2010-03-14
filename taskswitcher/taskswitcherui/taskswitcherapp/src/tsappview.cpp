/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Taskswitcher view class
 *
*/


#include <AknUtils.h>
#include <AknsUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <eikmenub.h>
#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>
#include <akntranseffect.h>
#include <aknPopupHeadingPane.h>
#include <StringLoader.h>
#include <taskswitcher.rsg>
#include "tsappview.h"
#include "tsappui.h"
#include "tsapplogging.h"
#include "tsfastswaparea.h"
#include "tseventcontroler.h"
#include "tsuid.hrh"



// -----------------------------------------------------------------------------
// CTsAppView::NewL
// -----------------------------------------------------------------------------
//
CTsAppView* CTsAppView::NewL( const TRect& aRect,
        CTsDeviceState& aDeviceState,
        RWindowGroup& aWg)
    {
    CTsAppView* self = CTsAppView::NewLC( aRect, aDeviceState, aWg );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsAppView::NewLC
// -----------------------------------------------------------------------------
//
CTsAppView* CTsAppView::NewLC( const TRect& aRect,
        CTsDeviceState& aDeviceState,
        RWindowGroup& aWg)
    {
    CTsAppView* self = new (ELeave) CTsAppView( aDeviceState );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aWg );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsAppView::CTsAppView
// -----------------------------------------------------------------------------
//
CTsAppView::CTsAppView(CTsDeviceState& aDeviceState)
        : iDeviceState( aDeviceState )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// InvalidateWindows
// -----------------------------------------------------------------------------
//
static void InvalidateWindows( CCoeControl* aControl )
    {
    if ( aControl )
        {
        if ( aControl->OwnsWindow() )
            {
            static_cast<RWindow*>( aControl->DrawableWindow() )->ClearRedrawStore();
            }

        for ( TInt i = aControl->CountComponentControls() - 1; i >= 0; --i )
            {
            InvalidateWindows( aControl->ComponentControl( i ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::~CTsAppView
// -----------------------------------------------------------------------------
//
CTsAppView::~CTsAppView()
    {
    delete iBgContext;
    delete iFastSwapArea;
    delete iAppsHeading;
    
    delete iEvtHandler;
    }

// -----------------------------------------------------------------------------
// CTsAppView::ConstructL
// -----------------------------------------------------------------------------
//
void CTsAppView::ConstructL( const TRect& aRect, RWindowGroup& aWg )
    {
    TSLOG_CONTEXT( CTsAppView::ConstructL, TSLOG_LOCAL );
    TSLOG_IN();

    // Create a window for this application view
    CreateWindowL(aWg);

    // Store rect
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TAknLayoutRect mainView;
    mainView.LayoutRect( aRect,
            AknLayoutScalable_Apps::main_tport_pane( variety ) );
    iViewRect = mainView.Rect();

    // Background control context is for a frame
    UpdatePopupRects();
    iBgContext = CAknsFrameBackgroundControlContext::NewL(
               KAknsIIDQsnFrPopup,
               iBgContextOuterRect,
               iBgContextInnerRect,
               ETrue );
    iBgContext->SetFrameRects(iBgContextOuterRect, iBgContextInnerRect);
    iBgContext->SetCenter( KAknsIIDQsnFrPopupCenter );

    TSLOG4( TSLOG_INFO, "view rect = %d %d %d %d",
        iViewRect.iTl.iX, iViewRect.iTl.iY,
        iViewRect.iBr.iX, iViewRect.iBr.iY );

    // Create components
    CreateControlsL();

    // Hide 'active applications' item in Options menu
    CEikMenuBar* menub = iEikonEnv->AppUiFactory()->MenuBar();
    if ( menub )
        {
        menub->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
        }

    // Register for notifications about orientation etc. changes
    iDeviceState.AddObserverL( *this, MTsDeviceStateObserver::EAny );
    SetRect( iViewRect );
    //SetExtentToWholeScreen();
    iViewRect = Rect();
    // try to enable window transparency
    if( CAknEnv::Static()->TransparencyEnabled() )
        {
        Window().SetRequiredDisplayMode( EColor16MA );
        if ( Window().SetTransparencyAlphaChannel() == KErrNone )
            {
            Window().SetBackgroundColor( ~0 );
            }
        }
    
    DrawableWindow()->EnableBackup(EWindowBackupFullScreen);
    SetComponentsToInheritVisibility(ETrue);
    MakeVisible(EFalse);
    // Ready to be drawn
    ActivateL();
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppView::DisableAppSwitchEffects
// -----------------------------------------------------------------------------
//
void CTsAppView::DisableAppSwitchEffects()
    {
    TInt flags = AknTransEffect::TParameter::ENoEffects;
    GfxTransEffect::BeginFullScreen(
        AknTransEffect::ENone,
        TRect(0,0,0,0),
        AknTransEffect::EParameterType,
        AknTransEffect::GfxTransParam( TUid::Uid(KTsAppUidValue), flags ) );
    }
        
// -----------------------------------------------------------------------------
// CTsAppView::GetPopupRects
// -----------------------------------------------------------------------------
//
void CTsAppView::UpdatePopupRects(  )
    {
    TSLOG_CONTEXT( UpdatePopupRects, TSLOG_LOCAL );
    TSLOG_IN();

    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    TAknLayoutRect bgTport;
    bgTport.LayoutRect( iViewRect,
        AknLayoutScalable_Apps::bg_popup_window_pane_cp01( variety ) );
    iBgContextOuterRect = bgTport.Rect();
    bgTport.LayoutRect( iBgContextOuterRect,
        AknLayoutScalable_Avkon::aid_inside_area_window_primary( variety ) );
    iBgContextInnerRect = bgTport.Rect();
    TSLOG4( TSLOG_INFO, "outer rect for popup = %d %d %d %d",
            iBgContextOuterRect.iTl.iX, iBgContextOuterRect.iTl.iY,
            iBgContextOuterRect.iBr.iX, iBgContextOuterRect.iBr.iY );
    TSLOG4( TSLOG_INFO, "inner rect for popup = %d %d %d %d",
            iBgContextInnerRect.iTl.iX, iBgContextInnerRect.iTl.iY,
            iBgContextInnerRect.iBr.iX, iBgContextInnerRect.iBr.iY );
    
    TSLOG_OUT();
    }
    
// -----------------------------------------------------------------------------
// CTsAppView::CreateControlsL
// -----------------------------------------------------------------------------
//
void CTsAppView::CreateControlsL()
    {
    RArray<TRect> rects;
    CleanupClosePushL( rects );
    rects.ReserveL( 2 ); // so appending in GetRects cannot fail
    GetRects( rects );

    iEvtHandler = CTsEventControler::NewL(*this, *this);
    iFastSwapArea = CTsFastSwapArea::NewL(rects[1], *this,
            iDeviceState, *iEvtHandler);
    iAppsHeading = new ( ELeave ) CAknPopupHeadingPane;

    iFastSwapArea->SetMopParent( this );
    iFastSwapArea->SetFocusing( ETrue );
    iFastSwapArea->Setup( *this );
    iFastSwapArea->SetDataChangeObserver( this );
    
    iAppsHeading->SetContainerWindowL( *this );
    iAppsHeading->ConstructL( KNullDesC() );
    iAppsHeading->SetLayout( CAknPopupHeadingPane::EListHeadingPane ); 

    //TODO: check why we need to set rect (set already in contructL)
    iFastSwapArea->SetRect( rects[1] ); // cannot be before iAppsHeading constructL
    iAppsHeading->SetRect( rects[0] );
    
    CleanupStack::PopAndDestroy( &rects );
    
    UpdateHeadingsL();
    }

// -----------------------------------------------------------------------------
// CTsAppView::GetRects
// -----------------------------------------------------------------------------
//
void CTsAppView::GetRects( RArray<TRect>& aRects )
    {
    TSLOG_CONTEXT( CTsAppView::GetRects, TSLOG_LOCAL );
    TSLOG_IN();
    
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    TAknLayoutRect appsLabel;
    TAknLayoutRect fastSwapAreaPane;

    appsLabel.LayoutRect( iViewRect,
            AknLayoutScalable_Apps::heading_pane_cp07( variety ) );
    TSLOG4( TSLOG_INFO, "apps heading rect = %d %d %d %d",
            appsLabel.Rect().iTl.iX, appsLabel.Rect().iTl.iY,
            appsLabel.Rect().iBr.iX, appsLabel.Rect().iBr.iY );

    
    fastSwapAreaPane.LayoutRect( iViewRect,
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    TSLOG4( TSLOG_INFO, "fsw rect = %d %d %d %d",
            fastSwapAreaPane.Rect().iTl.iX, fastSwapAreaPane.Rect().iTl.iY,
            fastSwapAreaPane.Rect().iBr.iX, fastSwapAreaPane.Rect().iBr.iY );

    aRects.Append( appsLabel.Rect() );
    aRects.Append( fastSwapAreaPane.Rect() );
#else
    TRect tempRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, tempRect);
    tempRect.iTl = TPoint(0,0);
    TRect fastSwapRect, headingRect;
    fastSwapRect.iTl = TPoint(KFswBorderSize, (tempRect.Height() - KFswItemHeight - KFswHeadingHeight) / 2 + KFswHeadingHeight);
    fastSwapRect.iBr = TPoint(tempRect.iBr.iX - KFswBorderSize, fastSwapRect.iTl.iY + KFswItemHeight);
    headingRect.iTl = TPoint(KFswBorderSize, fastSwapRect.iTl.iY - KFswHeadingHeight);
    headingRect.iBr = TPoint(fastSwapRect.iBr.iX, fastSwapRect.iTl.iY);
    aRects.Append( headingRect );
    aRects.Append( fastSwapRect );
#endif

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppView::SizeChanged
// -----------------------------------------------------------------------------
//
void CTsAppView::SizeChanged()
    {
    TSLOG_CONTEXT( CTsAppView::SizeChanged, TSLOG_LOCAL );
    TSLOG_IN();
    iViewRect = Rect();
    UpdatePopupRects();
    iBgContext->SetFrameRects(iBgContextOuterRect, iBgContextInnerRect);
    if ( iFastSwapArea && iAppsHeading  )
        {
        RArray<TRect> rects;
        // make sure that appending in GetRect cannot fail
        if ( rects.Reserve( 2 ) == KErrNone )
            {
            GetRects( rects );
            iAppsHeading->SetRect( rects[0] );
            iFastSwapArea->SetRect( rects[1] );
            
            DrawDeferred();
            }
        rects.Close();

        // Focus jumps back to fsw
        ChangeFocus( iFastSwapArea );
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppView::Draw
// -----------------------------------------------------------------------------
//
void CTsAppView::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::DrawFrame( skin,
           gc,
           iBgContextOuterRect,
           iBgContextInnerRect,
           KAknsIIDQsnFrPopup,
           KAknsIIDQsnFrPopupCenter );
    }

// -----------------------------------------------------------------------------
// CTsAppView::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CTsAppView::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CTsAppView::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CTsAppView::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    TSLOG_CONTEXT( OfferKeyEventL, TSLOG_LOCAL );
    TSLOG2_IN( "iCode = %d type = %d", aKeyEvent.iCode, aType );

    TKeyResponse result = EKeyWasNotConsumed;

    // Always use the portrait version, the layout in landscape is now
    // similar to portrait.
    result = OfferKeyEventPortraitStyleL( aKeyEvent, aType );

    // handle some other keys
    if ( result == EKeyWasNotConsumed && aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyYes: // send (green) key
                result = EKeyWasConsumed;
                break;
            }
        }

    // forward to focused control if not yet handled
    if ( result == EKeyWasNotConsumed && iFocused )
        {
        result = iFocused->OfferKeyEventL( aKeyEvent, aType );
        }

    TSLOG1_OUT( "response = %d", result );
    return result;
    }

// -----------------------------------------------------------------------------
// CTsAppView::OfferKeyEventPortraitStyleL
// -----------------------------------------------------------------------------
//
TKeyResponse CTsAppView::OfferKeyEventPortraitStyleL(
        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    TKeyResponse result = EKeyWasNotConsumed;
    return result;
    }


// -----------------------------------------------------------------------------
// CTsAppView::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CTsAppView::CountComponentControls() const
    {
    return 2;
    }

// -----------------------------------------------------------------------------
// CTsAppView::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CTsAppView::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFastSwapArea;
        case 1:
            return iAppsHeading;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::HandleSwitchToBackgroundEvent
// -----------------------------------------------------------------------------
//
void CTsAppView::HandleSwitchToBackgroundEvent()
    {
    iPopupFader.FadeBehindPopup( this, NULL, EFalse );
    
    iFastSwapArea->HandleSwitchToBackgroundEvent();

    }

// -----------------------------------------------------------------------------
// CTsAppView::HandleSwitchToForegroundEvent
// -----------------------------------------------------------------------------
//
void CTsAppView::HandleSwitchToForegroundEvent()
    {
    TSLOG_CONTEXT( CTsAppView::HandleSwitchToForegroundEvent, TSLOG_LOCAL );
    TSLOG_IN();
    
    Window().Invalidate(Rect());
    
    // Fade behind the pop-up
    iPopupFader.FadeBehindPopup( this, NULL, ETrue );

    DrawDeferred(); // otherwise some parts may not be drawn properly

    // Focus jumps back to fsw
    ChangeFocus( iFastSwapArea );

    // Forward event to interested controls
    iFastSwapArea->HandleSwitchToForegroundEvent();

    // Start animation
    CTsAppUi* appui =
        static_cast<CTsAppUi*>( iEikonEnv->AppUi() );
    if ( appui->EffectsEnabled() )
        {
        InvalidateWindows( this );
        appui->StartTransion(CTsAppUi::EForegroundTransition);
        }
    else
        {
        CAknTransitionUtils::MakeVisibleSubComponents( this,
            CAknTransitionUtils::EForceVisible );
        MakeVisible( ETrue );
        }
    
    DrawDeferred();
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppView::ChangeFocus
// -----------------------------------------------------------------------------
//
void CTsAppView::ChangeFocus( CCoeControl* aNewFocusedControl )
    {
    if ( iFocused != aNewFocusedControl )
        {
        if ( iFocused )
            {
            iFocused->SetFocus( EFalse, EDrawNow );
            }
        iFocused = aNewFocusedControl;
        if ( iFocused )
            {
            iFocused->SetFocus( ETrue, EDrawNow );
            }
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::HandleControlEventL
// EEventRequestFocus must be handled to have proper focus change also with
// pointer events.
// Observed controls are the ganes list and grid.
// -----------------------------------------------------------------------------
//
void CTsAppView::HandleControlEventL( CCoeControl* aControl,
        TCoeEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventRequestFocus:
            // must use the parent because aControl is the ganes control
            ChangeFocus( aControl->Parent() );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::HandleDeviceStateChanged
// Called when screen orientation, touch awareness, or the skin has been changed.
// -----------------------------------------------------------------------------
//
void CTsAppView::HandleDeviceStateChanged( TChangeType aChangeType )
    {
    TSLOG_CONTEXT( HandleDeviceStateChanged, TSLOG_LOCAL );
    TSLOG_IN();

    // Just set all the sizes, even when there is a skin change, because this will
    // guarantee proper redraw also with the ganes controls.

    if(aChangeType == EOrientation)
        {
        SetRect( static_cast<CAknAppUi*>(iEikonEnv->AppUi())->ApplicationRect() );
        }
    else
        {
        iViewRect = Rect();
        TSLOG4( TSLOG_INFO, "setting rect %d %d %d %d",
            iViewRect.iTl.iX, iViewRect.iTl.iY,
            iViewRect.iBr.iX, iViewRect.iBr.iY );
        SetRect( iViewRect );
        }
    
    InvalidateWindows(this);

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsAppView::CountFadedComponents
// -----------------------------------------------------------------------------
//
TInt CTsAppView::CountFadedComponents()
    {
    return 3;
    }

// -----------------------------------------------------------------------------
// CTsAppView::FadedComponent
// -----------------------------------------------------------------------------
//
CCoeControl* CTsAppView::FadedComponent( TInt aIndex )
    {
    // well, these are in fact the components that will _not_ be faded...
    // Must return the bottom-level controls here, e.g. the ganes controls, not the wrappers over them.
    switch ( aIndex )
        {
        case 0:
            return this;
        case 1:
            return iFastSwapArea->GetChildControl();
        case 2:
            return iAppsHeading;
//        case 3:
//            return iEikonEnv->AppUiFactory()->Cba();
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CTsAppView::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    if( TPointerEvent::EButton1Down == aPointerEvent.iType )
        {
		LaunchFeedback(ETouchFeedbackBasic, TTouchFeedbackType(
				ETouchFeedbackVibra | ETouchFeedbackAudio), aPointerEvent);
        } 
    iFastSwapArea->HandlePointerEventL(aPointerEvent);
    }

// -----------------------------------------------------------------------------
// CTsAppView::DataChanged
// -----------------------------------------------------------------------------
//
void CTsAppView::DataChanged( CCoeControl* /*aWhere*/, TInt /*aNewCount*/ )
    {
    TRAP_IGNORE( UpdateHeadingsL() );
    }

// -----------------------------------------------------------------------------
// CTsAppView::UpdateHeadingsL
// -----------------------------------------------------------------------------
//
void CTsAppView::UpdateHeadingsL()
    {
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
    HBufC* text = StringLoader::LoadLC(
        R_TASK_SWITCHER_HEADING_APPLICATIONS );
#else
    _LIT( KTitle, "Task switcher" );
    HBufC* text = KTitle().AllocLC();
#endif
    iAppsHeading->SetTextL( *text );
    iAppsHeading->DrawDeferred();
    CleanupStack::PopAndDestroy( text );

    }


// -----------------------------------------------------------------------------
// CTsAppView::HandleAppKey
// -----------------------------------------------------------------------------
//
void CTsAppView::HandleAppKey(TInt aType)
    {
    iFastSwapArea->HandleAppKey(aType);
    }


// -----------------------------------------------------------------------------
// CTsAppView::OrderBackgrRedraw
// -----------------------------------------------------------------------------
//
void CTsAppView::OrderFullWindowRedraw()
    {
    InvalidateWindows(this);
    DrawNow();
    }


// -----------------------------------------------------------------------------
// CTsAppView::MoveOffset
// -----------------------------------------------------------------------------
//
void CTsAppView::MoveOffset(const TPoint& aOffset)
    {
    iFastSwapArea->MoveOffset(aOffset);
    }

// -----------------------------------------------------------------------------
// CTsAppView::TapL
// -----------------------------------------------------------------------------
//
void CTsAppView::TapL(const TPoint& aPoint)
    {
    if(iFastSwapArea->Rect().Contains(aPoint))
        {
        iFastSwapArea->TapL(aPoint);
        DrawNow();
        }
    else if( !iAppsHeading->Rect().Contains(aPoint))
        {
        //move task switcher to background
        iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::LongTapL
// -----------------------------------------------------------------------------
//
void CTsAppView::LongTapL(const TPoint& aPoint)
    {
    if(iFastSwapArea->Rect().Contains(aPoint))
        {
        iFastSwapArea->LongTapL(aPoint);
        }
    else if( !iAppsHeading->Rect().Contains(aPoint))
        {
        //move task switcher to background
        iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
        }
    }

// -----------------------------------------------------------------------------
// CTsAppView::DragL()
// -----------------------------------------------------------------------------
//
void CTsAppView::DragL(const MAknTouchGestureFwDragEvent& aEvent)
    {
	if( aEvent.State() == EAknTouchGestureFwStop )
		{
		LaunchFeedback(ETouchFeedbackBasic, TTouchFeedbackType(
				ETouchFeedbackVibra | ETouchFeedbackAudio), TPointerEvent());
		}
    if( iFastSwapArea->Rect().Contains(aEvent.StartPosition()) ||
		iAppsHeading->Rect().Contains(aEvent.StartPosition()) )
        {
		iFastSwapArea->DragL(aEvent);
        }
    else 
    	{
		//move task switcher to background
		iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
    	}
    }

// -----------------------------------------------------------------------------
// CTsAppView::Stop
// -----------------------------------------------------------------------------
//
void CTsAppView::Stop()
    {
    iFastSwapArea->Stop();
    }

// -----------------------------------------------------------------------------
// CTsAppView::ViewPos
// -----------------------------------------------------------------------------
//
TPoint CTsAppView::ViewPos() const
    {
    return iFastSwapArea->ViewPos();
    }

// -----------------------------------------------------------------------------
// CTsAppView::LaunchFeedback
// -----------------------------------------------------------------------------
//
void CTsAppView::LaunchFeedback( TTouchLogicalFeedback aLogicalType,
        TTouchFeedbackType aFeedbackType,
        const TPointerEvent& aPointerEvent)
	{
	MTouchFeedback* feedback = MTouchFeedback::Instance();
	if(0 != feedback)
	    {
	    feedback->InstantFeedback(this, aLogicalType,
	    		aFeedbackType,
	    		aPointerEvent);
	    } 
	}




// End of file
