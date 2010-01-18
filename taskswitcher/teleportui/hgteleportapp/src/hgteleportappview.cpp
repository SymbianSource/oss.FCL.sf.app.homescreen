 /*
 * ============================================================================
 *  Name        : hgteleportappview.cpp
 *  Part of     : Hg Teleport
 *  Description : Teleport view class
 *  Version     : %version: sa1spcx1#54 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
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
#include <hgteleport.rsg>
#include <touchfeedback.h>

#include "hgteleportappview.h"
#include "hgteleportappui.h"
#include "hgteleportapplogging.h"
#include "hgteleportfastswaparea.h"
#include "hgteleporteventcontroler.h"
#include "hgteleportuid.hrh"

// -----------------------------------------------------------------------------
// CHgTeleportAppView::NewL
// -----------------------------------------------------------------------------
//
CHgTeleportAppView* CHgTeleportAppView::NewL( const TRect& aRect,
        CHgTeleportDeviceState& aDeviceState )
    {
    CHgTeleportAppView* self = CHgTeleportAppView::NewLC( aRect, aDeviceState );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::NewLC
// -----------------------------------------------------------------------------
//
CHgTeleportAppView* CHgTeleportAppView::NewLC( const TRect& aRect,
        CHgTeleportDeviceState& aDeviceState )
    {
    CHgTeleportAppView* self = new (ELeave) CHgTeleportAppView( aDeviceState );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::CHgTeleportAppView
// -----------------------------------------------------------------------------
//
CHgTeleportAppView::CHgTeleportAppView(CHgTeleportDeviceState& aDeviceState)
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
// CHgTeleportAppView::~CHgTeleportAppView
// -----------------------------------------------------------------------------
//
CHgTeleportAppView::~CHgTeleportAppView()
    {
    GfxTransEffect::SetTransitionObserver( 0 );
    if ( GfxTransEffect::IsRegistered( this ) )
        {
        MakeVisible( EFalse ); 
        CAknTransitionUtils::MakeVisibleSubComponents( this,
            CAknTransitionUtils::EForceInvisible );
        GfxTransEffect::Deregister( this );
        }
    delete iBgContext;
    delete iFastSwapArea;
    delete iAppsHeading;
    
    delete iEvtHandler;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::ConstructL( const TRect& aRect )
    {
    HGLOG_CONTEXT( CHgTeleportAppView::ConstructL, HGLOG_LOCAL );
    HGLOG_IN();

    // Create a window for this application view
    CreateWindowL();

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

    HGLOG4( HGLOG_INFO, "view rect = %d %d %d %d",
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
    iDeviceState.AddObserverL( *this, MHgDeviceStateObserver::EAny );
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
    Window().SetOrdinalPosition( 0, ECoeWinPriorityNormal );
    SetComponentsToInheritVisibility(ETrue);
    MakeVisible(ETrue);
    // Ready to be drawn
    ActivateL();
    
    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::DisableAppSwitchEffects
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::DisableAppSwitchEffects()
    {
    TInt flags = AknTransEffect::TParameter::ENoEffects;
    GfxTransEffect::BeginFullScreen(
        AknTransEffect::ENone,
        TRect(0,0,0,0),
        AknTransEffect::EParameterType,
        AknTransEffect::GfxTransParam( TUid::Uid(KHgTeleportAppUidValue), flags ) );
    }
        
// -----------------------------------------------------------------------------
// CHgTeleportAppView::GetPopupRects
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::UpdatePopupRects(  )
    {
    HGLOG_CONTEXT( UpdatePopupRects, HGLOG_LOCAL );
    HGLOG_IN();

    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    TAknLayoutRect bgTport;
    bgTport.LayoutRect( iViewRect,
        AknLayoutScalable_Apps::bg_popup_window_pane_cp01( variety ) );
    iBgContextOuterRect = bgTport.Rect();
    bgTport.LayoutRect( iBgContextOuterRect,
        AknLayoutScalable_Avkon::aid_inside_area_window_primary( variety ) );
    iBgContextInnerRect = bgTport.Rect();
    HGLOG4( HGLOG_INFO, "outer rect for popup = %d %d %d %d",
            iBgContextOuterRect.iTl.iX, iBgContextOuterRect.iTl.iY,
            iBgContextOuterRect.iBr.iX, iBgContextOuterRect.iBr.iY );
    HGLOG4( HGLOG_INFO, "inner rect for popup = %d %d %d %d",
            iBgContextInnerRect.iTl.iX, iBgContextInnerRect.iTl.iY,
            iBgContextInnerRect.iBr.iX, iBgContextInnerRect.iBr.iY );
   
#ifdef TASKSWITCHER_USE_CUSTOM_LAYOUT
    if ( iFastSwapArea )
        {
        iBgContextOuterRect = iFastSwapArea->Rect();
        iBgContextInnerRect = iFastSwapArea->Rect();
        }
#endif
    
    HGLOG_OUT();
    }
    
// -----------------------------------------------------------------------------
// CHgTeleportAppView::CreateControlsL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::CreateControlsL()
    {
    RArray<TRect> rects;
    CleanupClosePushL( rects );
    rects.ReserveL( 2 ); // so appending in GetRects cannot fail
    GetRects( rects );

    iEvtHandler = CHgTeleportEventControler::NewL(*this, *this);
    iFastSwapArea = CHgTeleportFastSwapArea::NewL(rects[1], *this,
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
// CHgTeleportAppView::GetRects
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::GetRects( RArray<TRect>& aRects )
    {
    HGLOG_CONTEXT( CHgTeleportAppView::GetRects, HGLOG_LOCAL );
    HGLOG_IN();
    
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    TAknLayoutRect appsLabel;
    TAknLayoutRect fastSwapAreaPane;

    appsLabel.LayoutRect( iViewRect,
            AknLayoutScalable_Apps::heading_pane_cp07( variety ) );
    HGLOG4( HGLOG_INFO, "apps heading rect = %d %d %d %d",
            appsLabel.Rect().iTl.iX, appsLabel.Rect().iTl.iY,
            appsLabel.Rect().iBr.iX, appsLabel.Rect().iBr.iY );

    
    fastSwapAreaPane.LayoutRect( iViewRect,
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    HGLOG4( HGLOG_INFO, "fsw rect = %d %d %d %d",
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

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::SizeChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::SizeChanged()
    {
    HGLOG_CONTEXT( CHgTeleportAppView::SizeChanged, HGLOG_LOCAL );
    HGLOG_IN();
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

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::Draw
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::Draw( const TRect& aRect ) const
    {
    if ( aRect == Rect() )
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
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CHgTeleportAppView::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CHgTeleportAppView::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    HGLOG_CONTEXT( OfferKeyEventL, HGLOG_LOCAL );
    HGLOG2_IN( "iCode = %d type = %d", aKeyEvent.iCode, aType );

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

    HGLOG1_OUT( "response = %d", result );
    return result;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::OfferKeyEventPortraitStyleL
// -----------------------------------------------------------------------------
//
TKeyResponse CHgTeleportAppView::OfferKeyEventPortraitStyleL(
        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    TKeyResponse result = EKeyWasNotConsumed;
    return result;
    }


// -----------------------------------------------------------------------------
// CHgTeleportAppView::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CHgTeleportAppView::CountComponentControls() const
    {
    return 2;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CHgTeleportAppView::ComponentControl( TInt aIndex ) const
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
// CHgTeleportAppView::HandleSwitchToBackgroundEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::HandleSwitchToBackgroundEvent()
    {
    // Stop animation and unfade
    GfxTransEffect::Abort();
    iPopupFader.FadeBehindPopup( this, NULL, EFalse );
    
    GfxTransEffect::Begin( this, 5);
    // Forward event to interested controls
    iFastSwapArea->HandleSwitchToBackgroundEvent();

    // Hide
    MakeVisible( EFalse );
    //GfxTransEffect::NotifyExternalState( ENotifyGlobalAbort );
    CAknTransitionUtils::MakeVisibleSubComponents( this,
        CAknTransitionUtils::EForceInvisible );
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::HandleSwitchToForegroundEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::HandleSwitchToForegroundEvent()
    {
    Window().Invalidate(Rect());
    
    // Fade behind the pop-up
    iPopupFader.FadeBehindPopup( this, NULL, ETrue );

    DrawDeferred(); // otherwise some parts may not be drawn properly

    // Focus jumps back to fsw
    ChangeFocus( iFastSwapArea );

    // Forward event to interested controls
    iFastSwapArea->HandleSwitchToForegroundEvent();

    // Start animation
    CHgTeleportAppUi* appui =
        static_cast<CHgTeleportAppUi*>( iEikonEnv->AppUi() );
    if ( appui->EffectsEnabled() )
        {
        InvalidateWindows( this );
        appui->StartTransion(AknTransEffect::EApplicationStart);
        }
    else
        {
        CAknTransitionUtils::MakeVisibleSubComponents( this,
            CAknTransitionUtils::EForceVisible );
        MakeVisible( ETrue );
        }
    
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::ChangeFocus
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::ChangeFocus( CCoeControl* aNewFocusedControl )
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
// CHgTeleportAppView::HandleControlEventL
// EEventRequestFocus must be handled to have proper focus change also with
// pointer events.
// Observed controls are the ganes list and grid.
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::HandleControlEventL( CCoeControl* aControl,
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
// CHgTeleportAppView::HandleDeviceStateChanged
// Called when screen orientation, touch awareness, or the skin has been changed.
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::HandleDeviceStateChanged( TChangeType /*aChangeType*/ )
    {
    HGLOG_CONTEXT( HandleDeviceStateChanged, HGLOG_LOCAL );
    HGLOG_IN();

    // Just set all the sizes, even when there is a skin change, because this will
    // guarantee proper redraw also with the ganes controls.

    iViewRect = Rect();
    HGLOG4( HGLOG_INFO, "setting rect %d %d %d %d",
        iViewRect.iTl.iX, iViewRect.iTl.iY,
        iViewRect.iBr.iX, iViewRect.iBr.iY );
    SetRect( iViewRect );

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::CountFadedComponents
// -----------------------------------------------------------------------------
//
TInt CHgTeleportAppView::CountFadedComponents()
    {
    return 3;
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::FadedComponent
// -----------------------------------------------------------------------------
//
CCoeControl* CHgTeleportAppView::FadedComponent( TInt aIndex )
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
// CHgTeleportAppView::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if(0 != feedback &&
       (TPointerEvent::EButton1Down == aPointerEvent.iType || 
       TPointerEvent::EButton1Up == aPointerEvent.iType))
        {
        feedback->InstantFeedback(this,
                                  ETouchFeedbackBasic, 
                                  ETouchFeedbackVibra, 
                                  aPointerEvent);
        } 
    iFastSwapArea->HandlePointerEventL(aPointerEvent);
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::DataChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::DataChanged( CCoeControl* /*aWhere*/, TInt /*aNewCount*/ )
    {
    TRAP_IGNORE( UpdateHeadingsL() );
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::UpdateHeadingsL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::UpdateHeadingsL()
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
// CHgTeleportAppView::HandleAppKey
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::HandleAppKey(TInt aType)
    {
    iFastSwapArea->HandleAppKey(aType);
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::MoveOffset
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::MoveOffset(const TPoint& aOffset)
    {
    iFastSwapArea->MoveOffset(aOffset);
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::TapL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::TapL(const TPoint& aPoint)
    {
    if(iFastSwapArea->Rect().Contains(aPoint))
        {
        iFastSwapArea->TapL(aPoint);
        }
    else if( !iAppsHeading->Rect().Contains(aPoint))
        {
        //move task switcher to background
        iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::LongTapL
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::LongTapL(const TPoint& aPoint)
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
// CHgTeleportAppView::Drag()
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::Drag(const MAknTouchGestureFwDragEvent& aEvent)
    {
    iFastSwapArea->Drag(aEvent);
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::Stop
// -----------------------------------------------------------------------------
//
void CHgTeleportAppView::Stop()
    {
    iFastSwapArea->Stop();
    }

// -----------------------------------------------------------------------------
// CHgTeleportAppView::ViewPos
// -----------------------------------------------------------------------------
//
TPoint CHgTeleportAppView::ViewPos() const
    {
    return iFastSwapArea->ViewPos();
    }

// End of file
