/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <aknlists.h>
#include <eikspmod.h>
#include <eikclb.h>
#include <eikfrlb.h>
#include <eikslb.h>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistboxtfxinternal.h>
#include <akntransitionutils.h>
#include <aknlistloadertfx.h>
#endif

#include "mmlistboxmodel.h"
#include "mmlistbox.h"
#include "mmlistboxview.h"
#include "mmlistboxitemdrawer.h"
#include "mmmarqueeadapter.h"
#include "mmwidgetcontainer.h"
#include "mmwidgetsconstants.h"
#include "mmdraweranimator.h"
#include "mmtemplatelibrary.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBox::CMmListBox() : AKNDOUBLELISTBOXNAME(R_LIST_PANE_LINES_AB_COLUMN)
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBox::~CMmListBox()
	{
	delete iRedrawTimer;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBox* CMmListBox::NewLC( const CCoeControl* aParent, TInt aFlags,
    CMmTemplateLibrary* aTemplateLibrary )
	{
	CMmListBox* self = new (ELeave)CMmListBox();
	CleanupStack::PushL(self);
	self->ConstructL( aParent, aFlags, aTemplateLibrary );
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetListFlag( TInt aFlag )
	{
	iListBoxFlags = iListBoxFlags | aFlag;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::ClearListFlag( TInt aFlag )
	{
	iListBoxFlags = iListBoxFlags & !aFlag;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBox* CMmListBox::NewL( const CCoeControl* aParent, TInt aFlags,
    CMmTemplateLibrary* aTemplateLibrary )
	{
	CMmListBox* self = CMmListBox::NewLC( aParent, aFlags, aTemplateLibrary );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::ConstructL( const CCoeControl* aParent, TInt aFlags,
    CMmTemplateLibrary* aTemplateLibrary )
	{
	iDisableChildComponentDrawing = EFalse;
	iModel = iMmModel = CMmListBoxModel::NewL();
    CreateItemDrawerL( aTemplateLibrary );

    EnableExtendedDrawingL();

    iItemDrawer->SetDrawMark(EFalse);
    CEikListBox::ConstructL(aParent,aFlags);
    iMmDrawer->SetView( this );
    iRedrawTimer = CPeriodic::NewL( EPriorityRealTime );
	}

// -----------------------------------------------------------------------------
// Clearing ELeftDownInViewRect flag before invoking the base class
// HandlePointerEventL method effectively prevents that method from doing most
// of the things it would normally do in response to EButton1Down event.
// This flag is explicitly cleared to achieve two things:
// 1. Prevent kinetic scrolling (flick) in edit mode.
// 2. Prevent highlight removal when popup menu is displayed.
// -----------------------------------------------------------------------------
//
void CMmListBox::HandlePointerEventInEditModeL(
        const TPointerEvent& aPointerEvent )
    {
    CMmWidgetContainer* parent = static_cast<CMmWidgetContainer*>( Parent() );
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iButton1DownPos = aPointerEvent.iPosition;
        }
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        TPoint dragDelta = iButton1DownPos - aPointerEvent.iPosition;
        if ( Abs( dragDelta.iY ) > KDragTreshold || parent->LongTapInProgress() )
            {
            iListBoxFlags &= ~ELeftDownInViewRect;
            }
        }
    
    TInt itemUnderPointerIndex = KErrNotFound;
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up ||
            aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        TBool highlightWasVisible = parent->IsHighlightVisible();
        CEikFormattedCellListBoxTypedef::HandlePointerEventL( aPointerEvent );
        // Tricky: Do not allow the base class implementation of HandlePointerEventL 
        //         to remove the highlight on EButton1Up event when context menu
        //         is displayed for an item
        if ( aPointerEvent.iType == TPointerEvent::EButton1Up &&
                highlightWasVisible && parent->LongTapInProgress()
                && !parent->IsHighlightVisible() )
            {
            ItemDrawer()->ClearFlags( CListItemDrawer::ESingleClickDisabledHighlight );
            }
        }
    else if ( View()->XYPosToItemIndex(
            aPointerEvent.iPosition, itemUnderPointerIndex ) )
        {
        TInt currentItemIndex = CurrentItemIndex();
        if ( currentItemIndex != itemUnderPointerIndex )
            {
            View()->SetCurrentItemIndex( itemUnderPointerIndex );
            // remove hightlight from the previously highlighted item
            }
        }
    HandleScrollingInEditMode( aPointerEvent );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::HandleScrollingInEditMode( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EDrag
            || aPointerEvent.iType == TPointerEvent::EButtonRepeat )
        {
        TInt nextScrollDelay = ScrollIfNeeded( aPointerEvent );
        if ( nextScrollDelay )
            {
            TRect ignoreDragRect(
                TPoint(aPointerEvent.iParentPosition.iX - MmEffects::KDragIgnoreRectValue,
                    aPointerEvent.iParentPosition.iY - MmEffects::KDragIgnoreRectValue),
                TPoint(aPointerEvent.iParentPosition.iX + MmEffects::KDragIgnoreRectValue,
                    aPointerEvent.iParentPosition.iY + MmEffects::KDragIgnoreRectValue));

            Window().CancelPointerRepeatEventRequest();
            Window().RequestPointerRepeatEvent( nextScrollDelay, ignoreDragRect );
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBox::IsPointerInTopScrollingThreshold(
            const TPointerEvent& aPointerEvent ) const
    {
    TInt topScrollingTreshold = Rect().iTl.iY
        + ( MmListBox::KFocusScrollingThreshold
            * TReal( View()->ItemSize().iHeight ) );

    return ( aPointerEvent.iPosition.iY < topScrollingTreshold );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBox::IsPointerInBottomScrollingThreshold(
            const TPointerEvent& aPointerEvent ) const
    {
    TInt bottomScrollingTreshold = Rect().iBr.iY
        - ( MmListBox::KFocusScrollingThreshold
            * TReal( View()->ItemSize().iHeight ) );

    return ( aPointerEvent.iPosition.iY > bottomScrollingTreshold );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBox::ScrollIfNeeded( const TPointerEvent& aPointerEvent )
    {
    TInt nextScrollDelay = 0;
    
	TBool readyForScrolling =
			iMmDrawer->GetAnimator()->IsReadyForNewAnimation()
					&& iMmDrawer->GetFloatingItemCount() != 0;
	
	if ( IsPointerInTopScrollingThreshold( aPointerEvent ) )
		{
		// scroll up by one row
		TInt newCurrentItemIndex = CurrentItemIndex() - 1;

		if ( newCurrentItemIndex >= 0 )
			{
			nextScrollDelay = MmEffects::KEditModeScrollingDelayFactor *
				Max( 1, aPointerEvent.iPosition.iY - Rect().iTl.iY );
			if (readyForScrolling)
				{
				View()->VScrollTo( View()->CalcNewTopItemIndexSoItemIsVisible(
                        newCurrentItemIndex ) );
                View()->SetCurrentItemIndex( newCurrentItemIndex );
                UpdateScrollBarThumbs();
				}
			}
		}
	else if ( IsPointerInBottomScrollingThreshold( aPointerEvent) )
		{
		// scroll down by one row
		TInt lastItemIndex = iModel->NumberOfItems() - 1;
		TInt newCurrentItemIndex = CurrentItemIndex() + 1;
		
		
		if ( newCurrentItemIndex <= lastItemIndex )
			{
			nextScrollDelay = MmEffects::KEditModeScrollingDelayFactor *
				Max( 1, Rect().iBr.iY - aPointerEvent.iPosition.iY );

			if (readyForScrolling)
				{
				View()->VScrollTo( View()->CalcNewTopItemIndexSoItemIsVisible(
						newCurrentItemIndex ) );
				View()->SetCurrentItemIndex( newCurrentItemIndex );
                UpdateScrollBarThumbs();
				}
			}
		}

    return nextScrollDelay;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
//    if ( aPointerEvent.iType == TPointerEvent::EButton1Down &&
//            iMmDrawer->CachedDataUseIsEnabled() )
//        {
//        // Touching the screen stops flick but avkon does not send us
//        // MEikListBoxObserver::EEventFlickStopped event in such case.
//        // Thus this little hack:
//        iMmDrawer->EnableCachedDataUse( EFalse );
//        DrawView();
//        }

    CMmWidgetContainer* parent = static_cast<CMmWidgetContainer*>( Parent() );

    if ( parent->IsEditMode() )
        {
        HandlePointerEventInEditModeL( aPointerEvent );
        }
    else
    	{
        CEikFormattedCellListBoxTypedef::HandlePointerEventL( aPointerEvent );
    	}

    if ( iMmDrawer->GetAnimator()->IsActive() )
    	{
    	iMmDrawer->GetAnimator()->CancelNextRedrawL();
    	}

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::FixViewForMirroredLayout()
    {
    TInt scrollbarWidth =
            ScrollBarFrame()->VerticalScrollBar()->Rect().Width();
    TRect r( View()->ViewRect() );
    TInt currentShift = r.iTl.iX;
    TBool scrollbarVisible = ScrollBarFrame()->VerticalScrollBar()->IsVisible();
    TBool layoutMirrored = AknLayoutUtils::LayoutMirrored();

    if ( layoutMirrored && scrollbarVisible && currentShift != scrollbarWidth )
        {
        // shift view rect to the right
        r.Move( scrollbarWidth - currentShift, 0 );
        View()->SetViewRect( r );
        }
    else if ( ( !layoutMirrored || !scrollbarVisible ) && currentShift != 0 )
        {
        // restore view rect to its normal position
        r.Move( -currentShift, 0 );
        View()->SetViewRect( r );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::RedrawScrollbarBackground() const
    {
    TRect viewRect( View()->ViewRect() );
    if ( viewRect.iTl.iX > 0 )
        {
        TRect scrollbarRect( TPoint( 0, 0 ),
                TSize( viewRect.iTl.iX, viewRect.Height() ) );

        CWindowGc* gc = iItemDrawer->Gc();
        CMmListBoxItemDrawer* itemDrawer =
                static_cast<CMmListBoxItemDrawer*>( iItemDrawer );
        MAknsSkinInstance *skin = AknsUtils::SkinInstance();
        CCoeControl* control = itemDrawer->FormattedCellData()->Control();
        MAknsControlContext *cc = AknsDrawUtils::ControlContext( control );
        if( gc )
            {
            if ( control )
                {
                AknsDrawUtils::Background( skin, cc, control, *gc, scrollbarRect );
                }
            else
                {
                gc->SetBrushColor( BackColor() );
                gc->Clear( scrollbarRect );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::ProcessScrollEventL( CEikScrollBar* aScrollBar, 
            TEikScrollEvent aEventType )
    {
    CEikFormattedCellListBoxTypedef::HandleScrollEventL(
            aScrollBar, aEventType );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::HandleRedrawTimerEventL()
    {
    if ( iSkippedScrollbarEventsCount )
        {
        ProcessScrollEventL( ScrollBarFrame()->VerticalScrollBar(),
                EEikScrollThumbDragVert );
        }
    iSkippedScrollbarEventsCount = 0;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBox::RedrawTimerCallback( TAny* aPtr )
    {
    CMmListBox* self = static_cast<CMmListBox*>( aPtr );
    TRAP_IGNORE( self->HandleRedrawTimerEventL() );
    // Do not bother returning a meaningful error code, CPeriodic will ignore it
    // anyway.
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TKeyResponse CMmListBox::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,TEventCode aType )
    {
    TInt itemIndex = CurrentItemIndex();
    TInt previousItemIndex = CurrentItemIndex();
    TKeyResponse ret = CEikFormattedCellListBoxTypedef::OfferKeyEventL(
        aKeyEvent, aType );
    TInt currentItemIndex = CurrentItemIndex();

    TInt itemY = View()->ItemPos( currentItemIndex ).iY
               + View()->ItemSize( currentItemIndex ).iHeight;

    if ( currentItemIndex == BottomItemIndex()
    		&& currentItemIndex != previousItemIndex
    		&& itemY > View()->ViewRect().iBr.iY )
    	{
    	if( aType == EEventKey )
    		{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    		MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(
    		    View()->ItemDrawer()->Gc() );
    		TBool effects = transApi && !transApi->EffectsDisabled();
    		if ( effects )
    			{
    			transApi->SetMoveType( MAknListBoxTfxInternal::EListScrollDown );
    			}
#endif
			iView->VScrollTo(
				iView->CalcNewTopItemIndexSoItemIsVisible( currentItemIndex ) );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
			if ( effects )
				{
				transApi->Draw( Rect() );
				}
#endif
			}
    	SetCurrentItemIndex( currentItemIndex );
    	}

    RedrawIfNecessary( itemIndex, CurrentItemIndex());
    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBox::RedrawIfNecessary( TInt aPreviousCurrent, TInt aCurrent )
    {
    TBool redrawConsumed( EFalse );
    if( aCurrent == KErrNotFound )
        {
        return redrawConsumed;
        }

    CMmListBoxView * view = static_cast<CMmListBoxView *>(iView);
    TInt differenceIndex = Abs( aPreviousCurrent - aCurrent );
    if ( aPreviousCurrent == KErrNotFound  || differenceIndex == 0 )
        {
        //TODO: It should be checked if this is really necessary
        view->RedrawBackground();
        return redrawConsumed;
        }

    TInt sizePrevBefore = iMmDrawer->GetItemHeight(aPreviousCurrent, ETrue);
    TInt sizeCurBefore = iMmDrawer->GetItemHeight(aCurrent, EFalse);
    TInt sizePrevAfter = iMmDrawer->GetItemHeight(aPreviousCurrent, EFalse);
    TInt sizeCurAfter = iMmDrawer->GetItemHeight(aCurrent, ETrue);

    if ( ( sizePrevBefore == sizePrevAfter ) && (sizeCurBefore == sizeCurAfter) )
        {
        return redrawConsumed;
        }
     else
        {
        TInt sizeAllBefore = sizePrevBefore + sizePrevAfter;
        TInt sizeAllAfter = sizeCurBefore + sizeCurAfter;

        TInt lastPotentialItemIndex = Min( iModel->NumberOfItems(),
                iView->TopItemIndex() + iView->NumberOfItemsThatFitInRect(
                        iView->ViewRect() ) );
        TInt redrawIndex = lastPotentialItemIndex;

        if ( differenceIndex == 1 )
            {
            if( sizeAllBefore != sizeAllAfter )
            	{
                redrawIndex = Min( aPreviousCurrent, aCurrent );
            	}
            else
            	{
            	return redrawConsumed;
            	}
            }
        else if ( differenceIndex > 1 && sizeAllBefore == sizeAllAfter  )
            {
            redrawIndex = Min( aPreviousCurrent, aCurrent );
            lastPotentialItemIndex = Max( aPreviousCurrent, aCurrent );
            }
        else if ( differenceIndex > 1 )
            {
            redrawIndex = Min( aPreviousCurrent, aCurrent );
            }

        if (redrawIndex < iView->TopItemIndex())
            redrawIndex = iView->TopItemIndex();

        if (lastPotentialItemIndex > iView->BottomItemIndex())
            lastPotentialItemIndex = iView->BottomItemIndex();

        if ( aPreviousCurrent < TopItemIndex() )
        	{
        	lastPotentialItemIndex = BottomItemIndex() ;
        	}
        else if ( BottomItemIndex() < aPreviousCurrent )
        	{
        	lastPotentialItemIndex = BottomItemIndex() + 1;
        	}
        
        while ( redrawIndex < lastPotentialItemIndex +1 )
            {
            view->DrawSingleItem( redrawIndex++ );
            redrawConsumed = ETrue;
            }

        view->RedrawBackground();
        }
    return redrawConsumed;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::CreateItemDrawerL( CMmTemplateLibrary* aTemplateLibrary )
    {
    CFormattedCellListBoxData* cellData=CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer = iMmDrawer = CMmListBoxItemDrawer::NewL( MmModel(),
            iEikonEnv->NormalFont(), cellData, EListbox, aTemplateLibrary );
    CleanupStack::Pop( cellData );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxModel* CMmListBox::MmModel()
	{
	return iMmModel;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetMmModel( CMmListBoxModel* aMmModel )
	{
	if ( iMmModel != aMmModel )
		{
		delete iMmModel;
		iMmModel = aMmModel;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CListBoxView* CMmListBox::MakeViewClassInstanceL()
    {
    return CMmListBoxView::NewL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBox::AdjustRectHeightToWholeNumberOfItems(TRect& /*aRect*/) const
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetItemDrawerAndViewBgContext (CAknsBasicBackgroundControlContext * aBgContext)
	{
	iMmDrawer->SetBgContext (aBgContext);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBox::HandleScrollbarVisibilityChangeL()
	{
	TBool ret = EFalse;
	if ( AllItemsFitInViewRect() )
		{
		if ( ScrollBarFrame()->VerticalScrollBar()->IsVisible()
            || iMmDrawer->TemplateLibrary()->GetScrollbarVisibility() )
			{
			ScrollBarFrame()->VerticalScrollBar()->MakeVisible( EFalse );
			iMmDrawer->SetScrollbarVisibilityL( EFalse );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
			MAknListBoxTfxInternal *trans = CAknListLoader::TfxApiInternal( ItemDrawer()->Gc() );
				if ( trans )
					{
					trans->Remove( MAknListBoxTfxInternal::EListEverything );
					}
#endif

			ret = ETrue; //redraw is needed
			}
		}
	else if ( !ScrollBarFrame()->VerticalScrollBar()->IsVisible()
	          || !iMmDrawer->TemplateLibrary()->GetScrollbarVisibility() )
		{
		ScrollBarFrame()->VerticalScrollBar()->MakeVisible( ETrue );
		iMmDrawer->SetScrollbarVisibilityL( ETrue );
		ret = ETrue; //redraw is needed
		}
	return ret;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBox::AllItemsFitInViewRect()
	{
	CMmListBoxView* view = static_cast< CMmListBoxView* >(iView);
	TInt totalHeight = view->GetTotalHeight( view->TopItemIndex(), view->BottomItemIndex());
	if ( view->TopItemIndex() == 0 && iMmModel->NumberOfItems() <= view->BottomItemIndex() + 1
	        && totalHeight <= iView->ViewRect().Height() )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
 * Helper class whose only purpose is to ensure that
 * ScrollToItem method will be always re-enabled. 
 */
struct TScrollToItemEnabler
    {
    CMmListBoxView* iV;
    void Close() { iV->DisableScrollToItem( EFalse ); }
    };

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::UpdateScrollBarsL()
	{
	TBool redrawNeeded = HandleScrollbarVisibilityChangeL();
	if (ScrollBarFrame()->VerticalScrollBar()->IsVisible())
		{
		CMmListBoxView* view = static_cast<CMmListBoxView*>( View() );
		view->DisableScrollToItem( ETrue );
		TScrollToItemEnabler reverter = { view };
		CleanupClosePushL( reverter );
		CEikFormattedCellListBoxTypedef::UpdateScrollBarsL();
		CleanupStack::PopAndDestroy( &reverter );
		}
	iMmDrawer->TemplateLibrary()->SetScrollbarWidthL(
        ScrollBarFrame()->VerticalScrollBar()->Rect().Width() );
	FixViewForMirroredLayout();
	if ( redrawNeeded )
		{
		DrawNow();
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::UpdateScrollBarsNoRedrawL()
	{
	HandleScrollbarVisibilityChangeL();
	if ( ScrollBarFrame()->VerticalScrollBar()->IsVisible() )
		{
		CMmListBoxView* view = static_cast<CMmListBoxView*>( View() );
		view->DisableScrollToItem( ETrue );
		TScrollToItemEnabler reverter = { view };
		CleanupClosePushL( reverter );
		CEikFormattedCellListBoxTypedef::UpdateScrollBarsL();
		CleanupStack::PopAndDestroy( &reverter );
		iMmDrawer->TemplateLibrary()->SetScrollbarWidthL(
		        ScrollBarFrame()->VerticalScrollBar()->Rect().Width() );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter )
	{
	iMarqueeAdapter = aAdapter;
	iMarqueeAdapter->SetControl( const_cast< CMmListBox *>(this) );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetMarqueeDrawing( TBool aIsMarqueeBeingDrawn )
	{
	iMmDrawer->SetMarqueeDrawing( aIsMarqueeBeingDrawn );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::HandleItemRemovalL()
	{
	CEikFormattedCellListBoxTypedef::HandleItemRemovalL();
    DrawNow();
    //avkon does not redraw the items for listbox when item is
    //removed. This needs to be forced here.
    UpdateScrollBarsL();
	}

// -----------------------------------------------------------------------------
// If a parent to the supplied control has its Gc set, this function will find
// it and return it. (Copied from EIKLBX.CPP, needed by CMmListBox::Draw)
// -----------------------------------------------------------------------------
//
LOCAL_C CWindowGc* ReplaceGcWithCustomGc( const CEikListBox* aListBox )
    {
    const CCoeControl* parent = aListBox;
    CWindowGc* customGc;
    while(parent)
        {
        customGc = parent->GetGc();
        if ( customGc )
            {
            CListItemDrawer* itemDrawer = aListBox->View()->ItemDrawer();
            CWindowGc* originalGc = itemDrawer->Gc();
            if ( customGc == originalGc )
            {
                return NULL;
                }
            else
                {
                itemDrawer->SetGc( customGc );
                return originalGc;
                }
            }
        parent = parent->Parent();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::Draw(const TRect& aRect) const
    {
//    if (!iResized)
//        {
//        return;
//        }

    CWindowGc* gc = this->iItemDrawer->Gc();

    // If a parent has a custom gc, draw listbox using that gc
    CWindowGc* replacedGc = ReplaceGcWithCustomGc(
                                    static_cast<const CEikListBox*>( this ) );

    if (this->iModel->NumberOfItems() == 0)
        {
        this->iView->DrawEmptyList(this->Rect());

        if ( replacedGc )
            {
            // Stop using the custom gc
            this->iItemDrawer->SetGc( replacedGc );
            }
        return;
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( gc );
    
    if ( transApi )
        {
        transApi->SetListType( MAknListBoxTfxInternal::EListBoxTypeMainPane );
        // ViewRect might have been moved to the right to prevent grid items
        // from overlapping the scrollbar in mirrored layout.
        // However, we still have to draw scrollbar background, thus the
        // rectangle object passed to MAknListBoxTfxInternal::BeginRedraw must
        // be placed at (0, 0) so that it covers the area where scrollbar is
        // drawn.
        TRect r( View()->ViewRect().Size() );
        transApi->BeginRedraw( MAknListBoxTfxInternal::EListView, r );
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
    if (!this->iView->RedrawDisabled())
        {
        MAknsControlContext *cc = AknsDrawUtils::ControlContext( this );
        if (!cc) cc = ItemDrawer()->FormattedCellData()->SkinBackgroundContext();

        if (gc)
            {
            TRect clientRect;
            this->RestoreClientRectFromViewRect(clientRect);
#ifdef RD_UI_TRANSITION_EFFECTS_LIST           
            if ( transApi )
                {
                transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
                }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
            gc->SetBrushColor(this->BackColor());
            AknsDrawUtils::BackgroundBetweenRects( AknsUtils::SkinInstance(), cc, this, *gc, clientRect, this->iView->ViewRect() );
            RedrawScrollbarBackground();
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
            if ( transApi )
                {
                transApi->StopDrawing();
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
            }
        }
    if ( this->iModel->NumberOfItems() )
        {
        // finally, draw the actual list
        this->iView->Draw(&aRect);
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        CMmListBoxView* view = static_cast<CMmListBoxView*>( View() );
        view->DisableScrollToItem( ETrue );
        transApi->EndViewRedraw( aRect );
        view->DisableScrollToItem( EFalse );
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
    if ( replacedGc )
        {
        // Stop using the custom gc
        this->iItemDrawer->SetGc( replacedGc );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBox::DrawView()
    {
    iDisableChildComponentDrawing = ETrue;
    DrawNow(Rect());
    iDisableChildComponentDrawing = EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetVerticalItemOffset( TInt aOffset )
	{
	static_cast<CMmListBoxView*>( View() )->SetItemOffsetInPixels( aOffset );
	UpdateScrollBarThumbs();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBox::VerticalItemOffset() const
	{
	return static_cast<CMmListBoxView*>( View() )->VerticalItemOffset();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetItemHeight( TInt aItemHeight )
	{
	if ( aItemHeight != iItemHeight )
	    {
	    iItemHeight = aItemHeight;
	    TRAP_IGNORE( UpdateScrollBarsNoRedrawL() );
	    }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::UpdateScrollBarThumbs()
	{
	CEikFormattedCellListBox::UpdateScrollBarThumbs();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBox::CountComponentControls() const
    {
    TInt componentControls(0);
    if ( !iDisableChildComponentDrawing )
		{
        componentControls = CEikFormattedCellListBoxTypedef::CountComponentControls();
    	}
    return componentControls;
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::SetDisableChildComponentDrawing( TBool aDisable )
    {
    iDisableChildComponentDrawing = aDisable;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBox::HandleScrollEventL( CEikScrollBar* aScrollBar, 
            TEikScrollEvent aEventType )
    {
    if ( aEventType == EEikScrollThumbDragVert && !iScrollbarThumbIsBeingDragged )
        {
        iScrollbarThumbIsBeingDragged = ETrue;
        static_cast<CMmListBoxItemDrawer*>(
                View()->ItemDrawer() )->EnableCachedDataUse( ETrue );
        iRedrawTimer->Start( KScrollingRedrawInterval, KScrollingRedrawInterval,
                TCallBack( &CMmListBox::RedrawTimerCallback, static_cast<TAny*>( this ) ) );
        }
    else if ( aEventType == EEikScrollThumbReleaseVert )
        {
        iScrollbarThumbIsBeingDragged = EFalse;
        static_cast<CMmListBoxItemDrawer*>(
                View()->ItemDrawer() )->EnableCachedDataUse( EFalse );
        // The view will be redrawn with cache disabled when ProcessScrollEventL
        // calls the base class's HandleScrollEventL method -- no need to
        // explicitly redraw the view.
        iRedrawTimer->Cancel();
        }

    if ( !iScrollbarThumbIsBeingDragged )
        {
        ProcessScrollEventL( aScrollBar, aEventType );
        }
    else
        {
        __ASSERT_DEBUG( aEventType == EEikScrollThumbDragVert, User::Invariant() );
        ++iSkippedScrollbarEventsCount;
        }
    }

// End of file
