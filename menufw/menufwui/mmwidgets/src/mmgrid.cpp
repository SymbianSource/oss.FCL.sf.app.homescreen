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
*  Version     : %version: MM_101 % << Don't touch! Updated by Synergy at check-out.
*
*/


#include <AknsDrawUtils.h>
#include <layoutmetadata.cdl.h>
#include "mmgrid.h"
#include "mmgridmodel.h"
#include "mmlistboxitemdrawer.h"
#include "mmgridview.h"
#include "mmtemplatelibrary.h"
#include "mmlistboxmodel.h"
#include "mmmarqueeadapter.h"
#include "mmwidgetsconstants.h"
#include "mmwidgetcontainer.h"
#include "hnsuitemodel.h"
#include "mmdraweranimator.h"

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGrid::CMmGrid()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGrid::~CMmGrid()
    {
    delete iRedrawTimer;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGrid* CMmGrid::NewLC( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary)
    {
    CMmGrid* self = new (ELeave)CMmGrid();
    CleanupStack::PushL(self);
    self->ConstructL( aParent, aFlags, aTemplateLibrary );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGrid* CMmGrid::NewL( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary )
    {
    CMmGrid* self = CMmGrid::NewLC( aParent, aFlags, aTemplateLibrary );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// If a parent to the supplied control has its Gc set, this function will find
// it and return it. (Copied from EIKLBX.CPP, needed by CMmGrid::Draw)
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
void CMmGrid::Draw(const TRect& aRect) const
    {
    if ( View()->RedrawDisabled () || !IsVisible () )
        {
        return;
        }
    iMmDrawer->SetNumberOfColsInView(
            static_cast<CMmGridView*>(iView)->NumberOfColsInView() );

    // If a parent has a custom gc, draw listbox using that gc
    CWindowGc* replacedGc = ReplaceGcWithCustomGc( this );

    CWindowGc* gc = iItemDrawer->Gc();
    TGulBorder::TColors borderColors;
    iBorder.Draw(*gc, Rect(), borderColors);

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(
            static_cast<CMmGridView*>(iView)->Gc() );
    TBool effects = transApi && !transApi->EffectsDisabled();
    if ( effects )
        {
        // ViewRect might have been moved to the right to prevent grid items
        // from overlapping the scrollbar in mirrored layout.
        // However, we still have to draw scrollbar background, thus the
        // rectangle object passed to MAknListBoxTfxInternal::BeginRedraw must
        // be placed at (0, 0) so that it covers the area where scrollbar is
        // drawn.
        TRect r( View()->ViewRect().Size() );
        transApi->BeginRedraw( MAknListBoxTfxInternal::EListView, r );
        transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    ClearMargins();
    RedrawScrollbarBackground();

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( effects )
        {
        transApi->StopDrawing();
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    iView->Draw(&aRect);

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( effects )
        {
        transApi->EndViewRedraw( aRect );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    if ( replacedGc )
        {
        // Stop using the custom gc
        iItemDrawer->SetGc( replacedGc );
        }
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::DrawView()
    {
    iDisableChildComponentDrawing = ETrue;
    DrawNow(Rect());
    iDisableChildComponentDrawing = EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::CreateItemDrawerL()
    {
    iItemDrawer = iMmDrawer;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::CreateItemDrawerL( CMmTemplateLibrary* aTemplateLibrary )
    {
    CFormattedCellListBoxData* data=CFormattedCellGridData::NewL();
    CleanupStack::PushL(data);

    iMmDrawer = CMmListBoxItemDrawer::NewL(
        MmModel()->MmListBoxModel(), iEikonEnv->NormalFont(), data, EGrid,
        aTemplateLibrary );
    iItemDrawer = iMmDrawer;
    data->SetControl( this );
    CleanupStack::Pop( data );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandleScrollEventL( CEikScrollBar* aScrollBar,
        TEikScrollEvent aEventType )
    {
    if ( aEventType == EEikScrollThumbDragVert && !iScrollbarThumbIsBeingDragged )
        {
        iScrollbarThumbIsBeingDragged = ETrue;
        static_cast<CMmListBoxItemDrawer*>(
                View()->ItemDrawer() )->EnableCachedDataUse( ETrue );
        iRedrawTimer->Start( KScrollingRedrawInterval, KScrollingRedrawInterval,
                TCallBack( &CMmGrid::RedrawTimerCallback, static_cast<TAny*>( this ) ) );
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

// -----------------------------------------------------------------------------
// Clearing ELeftDownInViewRect flag before invoking the base class
// HandlePointerEventL method effectively prevents that method from doing most
// of the things it would normally do in response to EButton1Down event.
// This flag is explicitly cleared to achieve two things:
// 1. Prevent kinetic scrolling (flick) in edit mode.
// 2. Prevent highlight removal when popup menu is displayed.
// -----------------------------------------------------------------------------
//
void CMmGrid::HandlePointerEventInEditModeL( const TPointerEvent& aPointerEvent )
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
        CAknGrid::HandlePointerEventL( aPointerEvent );
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
        if ( CurrentItemIndex() != itemUnderPointerIndex )
            {
            CMmWidgetContainer* parent = static_cast<CMmWidgetContainer*>( Parent() );
    		if ( parent->IsNoItemDragged() )
    			{
    			if ( ItemDrawer()->Flags() & CListItemDrawer::EPressedDownState )
    				{
    				ItemDrawer()->ClearFlags( CListItemDrawer::EPressedDownState );
    				iView->DrawItem( CurrentItemIndex() );
    				}
    			}
    		else
    			{
    			iView->SetCurrentItemIndex( itemUnderPointerIndex );
				iView->DrawItem(itemUnderPointerIndex);
    			}
            }
        }

    HandleScrollingInEditMode( aPointerEvent );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandlePointerEventInNormalModeL( const TPointerEvent& aPointerEvent )
    {
    CMmWidgetContainer* parent = static_cast<CMmWidgetContainer*>( Parent() );
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
		{
        const TInt KIgnoreRectSize = 40;
        TRect ignoreDragRect(aPointerEvent.iPosition,
                TSize(KIgnoreRectSize, KIgnoreRectSize));
        ignoreDragRect.Move( - KIgnoreRectSize / 2, - KIgnoreRectSize / 2 );
        Window().RequestPointerRepeatEvent( 50000, ignoreDragRect );
		}
    else if ( aPointerEvent.iType == TPointerEvent::EButtonRepeat )
        {
        HandleButtonRepeatEventInNormalModeL( aPointerEvent );
        }
	CAknGrid::HandlePointerEventL(aPointerEvent);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandleButtonRepeatEventInNormalModeL(
        const TPointerEvent& aPointerEvent )
    {
    TInt itemUnderPointerIndex = KErrNotFound;
    TBool itemFound = View()->XYPosToItemIndex(
            aPointerEvent.iPosition, itemUnderPointerIndex );
    if ( !itemFound )
        {
        return;
        }

    View()->ItemDrawer()->SetFlags( CListItemDrawer::EPressedDownState );
    CMmWidgetContainer* parent = static_cast<CMmWidgetContainer*>( Parent() );
    TBool highlightWasVisible = parent->IsHighlightVisible();

    if( itemUnderPointerIndex == View()->CurrentItemIndex() )
        {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(
                iMmDrawer->Gc() );
         if ( transApi )
             {
             transApi->Remove( MAknListBoxTfxInternal::EListHighlight );
             View()->DrawItem( itemUnderPointerIndex );
             TRect itemUnderPointerIndexRect (
                     View()->ItemPos(itemUnderPointerIndex),
                     View()->ItemSize(itemUnderPointerIndex));
             transApi->Draw( itemUnderPointerIndexRect );
             }
         else
        	 {
        	 View()->DrawItem( itemUnderPointerIndex );
        	 }
#else
         View()->DrawItem( itemUnderPointerIndex );
#endif
        }
     else
        {
        TInt previouslyHighlightedItemIndex = View()->CurrentItemIndex();
        parent->SetManualHighlightL( itemUnderPointerIndex , EFalse );
        if ( highlightWasVisible )
            {
            View()->DrawItem( previouslyHighlightedItemIndex );
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandleScrollingInEditMode( const TPointerEvent& aPointerEvent )
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
TBool CMmGrid::IsPointerInTopScrollingThreshold(
            const TPointerEvent& aPointerEvent ) const
    {
    TInt topScrollingTreshold = Rect().iTl.iY
        + ( MmGrid::KFocusScrollingThreshold
            * TReal( View()->ItemSize().iHeight ) );

    return ( aPointerEvent.iPosition.iY < topScrollingTreshold );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmGrid::IsPointerInBottomScrollingThreshold(
            const TPointerEvent& aPointerEvent ) const
    {
    TInt bottomScrollingTreshold = Rect().iBr.iY
        - ( MmGrid::KFocusScrollingThreshold
            * TReal( View()->ItemSize().iHeight ) );

    return ( aPointerEvent.iPosition.iY > bottomScrollingTreshold );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::ScrollWithoutRedraw( TInt distanceInPixels )
    {
    CAknGridView* view = static_cast<CAknGridView*>( iView );
    const TInt rowHeight = ItemHeight();
    const TInt numOfCols = view->NumberOfColsInView();
    const TInt itemCount = iModel->NumberOfItems();
    TInt totalNumberOfRows = itemCount / numOfCols;
    if ( itemCount % numOfCols )
        {
        ++totalNumberOfRows;
        }
    const TInt topItemRowIndex = TopItemIndex() / numOfCols;
    
    // desired view position relative to the first item in grid (always positive)
    TInt desiredViewPosition = rowHeight * topItemRowIndex - VerticalItemOffset();
    
    desiredViewPosition += distanceInPixels;
    
    const TInt viewPositionMin = 0;
    const TInt viewPositionMax = 
		Max( 0, ( totalNumberOfRows * rowHeight ) - view->ViewRect().Height() );
    
    desiredViewPosition = Min( desiredViewPosition, viewPositionMax );
    desiredViewPosition = Max( desiredViewPosition, viewPositionMin );
   
    ASSERT( desiredViewPosition >= 0 );
    
    TInt newTopItemIndex = ( desiredViewPosition / rowHeight ) * numOfCols;
    TInt newVerticalOffset = -( desiredViewPosition % rowHeight ); 
    SetTopItemIndex( newTopItemIndex );
    SetVerticalItemOffset( newVerticalOffset );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmGrid::ScrollIfNeeded( const TPointerEvent& aPointerEvent )
    {
    CAknGridView* view = static_cast<CAknGridView*>( View() );
    TInt nextScrollDelay = 0;

    TBool readyForScrolling = iMmDrawer->GetAnimator()->IsReadyForNewAnimation()
		&& iMmDrawer->GetFloatingItemCount() != 0;

	if ( IsPointerInTopScrollingThreshold( aPointerEvent ) )
		{
		// scroll up by one row
		TInt newCurrentItemIndex = CurrentItemIndex() - view->NumberOfColsInView();
        if ( newCurrentItemIndex < 0 )
            {
            newCurrentItemIndex = CurrentItemIndex();
            }

       nextScrollDelay = MmEffects::KEditModeScrollingDelayFactor *
		   Max( 1, aPointerEvent.iPosition.iY - Rect().iTl.iY );

		if ( readyForScrolling )
			{
			ScrollWithoutRedraw( -iItemHeight );
			View()->SetCurrentItemIndex( newCurrentItemIndex );
			}
		}
	else if ( IsPointerInBottomScrollingThreshold( aPointerEvent) )
		{
		TInt newCurrentItemIndex = CurrentItemIndex() + view->NumberOfColsInView();
		if ( newCurrentItemIndex > iModel->NumberOfItems() - 1 )
			{
			newCurrentItemIndex = CurrentItemIndex();
			}
	
		nextScrollDelay = MmEffects::KEditModeScrollingDelayFactor *
			Max( 1, Rect().iBr.iY - aPointerEvent.iPosition.iY );

		if ( readyForScrolling )
			{
			ScrollWithoutRedraw( iItemHeight );
			View()->SetCurrentItemIndex( newCurrentItemIndex );
			}
		}

    return nextScrollDelay;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandlePointerEventL(const TPointerEvent& aPointerEvent)
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
        HandlePointerEventInNormalModeL( aPointerEvent );
        }

    if ( iMmDrawer->GetFloatingItemCount() > 0 &&
            iMmDrawer->GetAnimator()->IsActive() )
        {
        iMmDrawer->GetAnimator()->CancelNextRedrawL();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandleOverridenHighlightDrawing( const TPointerEvent& aPointerEvent,
        TInt aIndexBefore, TInt aIndexAfter )
    {
    // In grid our behaviour is a little different than defalt avkon.
    // It is required to clean the highlight after the finger has been raised,
    // and simulate the tap effect, when an item that has an invisible
    // highlight is tapped again.
    TBool effects (EFalse);
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(
                iMmDrawer->Gc() );
        effects = transApi && !transApi->EffectsDisabled();
#endif
    if (effects)
        {
        CMmWidgetContainer* parent = static_cast< CMmWidgetContainer* > ( Parent() );
        if ( aIndexBefore == aIndexAfter
                && !parent->IsEditMode()
                && aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            DrawItem( aIndexAfter );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
            MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(
                    iMmDrawer->Gc() );
             if ( transApi )
                 {
                 transApi->SetMoveType( MAknListBoxTfxInternal::EListTap);
                 transApi->Draw( Rect() );
                 }
#endif
            }
        else if ( aIndexBefore == aIndexAfter
                && aPointerEvent.iType == TPointerEvent::EButton1Up
                && !parent->IsHighlightVisible())
            {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
            MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(
                    iMmDrawer->Gc() );
             if ( transApi )
                 {
                 transApi->Draw( Rect() );
                 }
#endif
            }
        }

    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::FixViewForMirroredLayout()
    {
    TInt scrollbarWidth = ScrollBarOffset() +
            ScrollBarFrame()->VerticalScrollBar()->Rect().Width();
    TRect r( View()->ViewRect() );
    TInt currentShift = r.iTl.iX;
    TBool scrollbarVisible = ScrollBarFrame()->VerticalScrollBar()->IsVisible();
    TBool layoutMirrored = AknLayoutUtils::LayoutMirrored();

    if ( layoutMirrored )
        {
        // Extra width, which results from the fact that it is not always
        // possible to divide screen width by the number of columns without
        // a non-zero remainder. This makes the distance of grid items from
        // scrollbar in mirrored layout exactly the same as in normal layout.
        scrollbarWidth += ( r.Width() - scrollbarWidth ) %
            ( (CAknGridView*) View() )->NumberOfColsInView();
        }

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
void CMmGrid::ConstructL( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary )
    {
    iDisableChildComponentDrawing = EFalse;
    iModel = iMmModel = CMmGridModel::NewL();
    CreateItemDrawerL(aTemplateLibrary);
    CAknGrid::ConstructL( aParent, aFlags );
    iMmDrawer->SetView( this );

    //set initial value so avkon does not panic
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        iViewLayout = TSize( MmGrid::K4By3LayoutX, MmGrid::K4By3LayoutY );
        }
    else
        {
        iViewLayout = TSize( MmGrid::K3By4LayoutX, MmGrid::K3By4LayoutY );
        }

    DoSetupLayoutL();
    iRedrawTimer = CPeriodic::NewL( EPriorityRealTime );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CListBoxView* CMmGrid::MakeViewClassInstanceL()
    {
    return CMmGridView::NewL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGridModel* CMmGrid::MmModel()
    {
    return iMmModel;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TKeyResponse CMmGrid::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TKeyEvent eventCopy( aKeyEvent );
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        if ( eventCopy.iCode == EKeyLeftArrow )
            {
            eventCopy.iCode = EKeyRightArrow;
            }
        else if ( eventCopy.iCode == EKeyRightArrow )
            {
            eventCopy.iCode = EKeyLeftArrow;
            }
        }
    return CAknGrid::OfferKeyEventL( eventCopy, aType );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandleViewRectSizeChangeL()
    {
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        CAknGrid::HandleViewRectSizeChangeL();
        }
    else
        {
        // for mirrored layout this function should do
        // exactly the same things as CAknGrid::HandleViewRectSizeChangeL
        // would do for normal layout
        iView->CalcBottomItemIndex();
        if ( CurrentItemIndex() >= 0 )
            {
            AdjustTopItemIndex();
            iView->CalcDataWidth();
            UpdateScrollBarsL();
            iView->CalcBottomItemIndex();
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::AdjustTopItemIndex() const
    {
    CAknGridView* view = static_cast<CAknGridView*>( iView );
    const TInt numOfCols = view->NumberOfColsInView();
    const TInt numOfRows = view->NumberOfRowsInView();
    const TInt itemCount = iModel->NumberOfItems();

    ASSERT( numOfCols > 0 );

    TInt lastRow = 0;
    if ( itemCount > 0 )
        {
        lastRow = ( itemCount - 1 ) / numOfCols;
        }
    TInt maxPossibleTopRow = Max( 0, lastRow - numOfRows + 1 );

    TInt topRow = TopItemIndex() / numOfCols;

    if ( !( TopItemIndex() % numOfCols == 0 &&
         topRow <= maxPossibleTopRow ) )
        {
        topRow = Min( topRow, maxPossibleTopRow );
        SetTopItemIndex( topRow * numOfCols );
        }

    // prevent problems with view being scrolled up beyond limits
    if ( topRow == maxPossibleTopRow && VerticalItemOffset() < 0 )
        {
        TRAP_IGNORE(
                ( ( CMmWidgetContainer* ) Parent() )->AlignBottomOfViewL() );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::DoHandleResourceChangeL( TSize& aCellSize,
        TSize& aViewLayout )
    {
    TBool cond = Layout_Meta_Data::IsLandscapeOrientation();
    TRAPD( err, DoHandleResourceChangeL( cond, aCellSize, aViewLayout ) );
    if ( err == KErrNotFound )
        {
        aViewLayout = iViewLayout;
        aCellSize = TSize (1,1);
        }
    else if ( err == KErrNoMemory )
        {
        User::Leave( KErrNoMemory );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::DoHandleResourceChangeL( TBool aIsLandscape, TSize& aCellSize,
        TSize& aViewLayout )
    {
    CHnSuiteModel* model = iMmModel->MmListBoxModel()->GetSuiteModel();
    const TDesC8& templ = (model) ? model->GetTemplate() : KNullDesC8();
    aCellSize = iMmDrawer->TemplateLibrary()->GetSize(
        EGrid, templ, aIsLandscape, EFalse, iView->ViewRect() );
    aViewLayout = iMmDrawer->TemplateLibrary()->GetLayoutSizeL(
        EGrid, templ, aIsLandscape );
    iViewLayout = aViewLayout;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::SetItemDrawerAndViewBgContext(
        CAknsBasicBackgroundControlContext * aBgContext )
    {
    iMmDrawer->SetBgContext (aBgContext);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmGrid::HandleScrollbarVisibilityChangeL()
    {
    TBool ret = EFalse;
    if ( AllItemsFitInViewRect() )
        {
        if ( ScrollBarFrame()->VerticalScrollBar()->IsVisible()
             || iMmDrawer->TemplateLibrary()->GetScrollbarVisibility() )
            {
            ScrollBarFrame()->VerticalScrollBar()->MakeVisible( EFalse );
            iMmDrawer->SetScrollbarVisibilityL( EFalse );
            SetTopItemIndex( 0 );
            iCurrentTopItemIndex = 0;
            ret = ETrue;
            }
        }
    else if ( !ScrollBarFrame()->VerticalScrollBar()->IsVisible()
            || !iMmDrawer->TemplateLibrary()->GetScrollbarVisibility() )
        {
        ScrollBarFrame()->VerticalScrollBar()->MakeVisible( ETrue );
        iMmDrawer->SetScrollbarVisibilityL( ETrue );
        ret = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

TBool CMmGrid::AllItemsFitInViewRect()
    {
    if ( iMmModel->NumberOfItems() <= iViewLayout.iWidth * iViewLayout.iHeight
            || iViewLayout == TSize(0,0) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::UpdateScrollBarsL()
    {
    TBool visibilityChanged = HandleScrollbarVisibilityChangeL();
    if (ScrollBarFrame()->VerticalScrollBar()->IsVisible())
        {
        CAknGrid::UpdateScrollBarsL();
        }
    iCurrentTopItemIndex = TopItemIndex();
    iMmDrawer->TemplateLibrary()->SetScrollbarWidthL(
            ScrollBarFrame()->VerticalScrollBar()->Rect().Width() + ScrollBarOffset() );
    FixViewForMirroredLayout();
    if ( visibilityChanged )
        {
        CMmGridView* view = static_cast<CMmGridView*>( iView );
        view->UpdateItemHeightAndWidth();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter )
    {
    iMarqueeAdapter = aAdapter;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::SetupLayout()
    {
    TRAP_IGNORE( DoSetupLayoutL() );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::DoSetupLayoutL()
    {
    TSize cellSize;
    TSize viewLayout;
    DoHandleResourceChangeL( cellSize, viewLayout );
//  The mirrored layout arabic item lineout ( right to left )
//  is handled by CMmGridView::ItemPos( TInt aItemIndex )
//  and CMmGridView::XYPosToItemIndex( TPoint aPosition, TInt& aItemIndex )
    SetLayoutL( EFalse, ETrue /*!AknLayoutUtils::LayoutMirrored()*/, ETrue, viewLayout.iWidth,
        viewLayout.iHeight, cellSize, 0, 0 );
    iMmDrawer->SetNumberOfColsInView(
            static_cast<CMmGridView*>(iView)->NumberOfColsInView() );
    // setting the top item index when changing orientation
    TInt cols( static_cast<CMmGridView*>(iView)->NumberOfColsInView() );
    SetTopItemIndex( ( (TInt) ( iCurrentTopItemIndex / cols ) ) * cols );
    UpdateScrollBarsL();
    AknsUtils::RegisterControlPosition( iMmDrawer->FormattedCellData()->Control() );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmGrid::ScrollBarOffset()
    {
    TInt offset = 0;
    if ( ScrollBarFrame()->ScrollBarExists( CEikScrollBar::EVertical ) )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            offset = ScrollBarFrame()->VerticalScrollBar()->Position().iX;
            }
        else
            {
            offset = View()->ViewRect().Width()
                    - ( ScrollBarFrame()->VerticalScrollBar()->Position().iX
                        + ScrollBarFrame()->VerticalScrollBar()->Rect().Width() );
            }
        }
    return offset;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::RedrawScrollbarBackground() const
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
void CMmGrid::ProcessScrollEventL( CEikScrollBar* aScrollBar, 
            TEikScrollEvent aEventType )
    {
    CAknGrid::HandleScrollEventL( aScrollBar, aEventType );
    iCurrentTopItemIndex = TopItemIndex();

    // setting default highligh in order not to overwrite the top item index
    // set before in the SetLayout method
    CMmWidgetContainer* parent = static_cast< CMmWidgetContainer* > ( Parent() );
    if (!parent->IsHighlightVisible())
         {
         parent->SetDefaultHighlightL( EFalse );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
         MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(
                 static_cast<CMmGridView*>(iView)->Gc() );
         TBool effects = transApi && !transApi->EffectsDisabled();
         if ( effects && aEventType == EEikScrollPageUp ||
                 aEventType == EEikScrollPageDown )
             {
             DrawNow();
             }
#endif

         }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::HandleRedrawTimerEventL()
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
TInt CMmGrid::RedrawTimerCallback( TAny* aPtr )
    {
    CMmGrid* self = static_cast<CMmGrid*>( aPtr );
    TRAP_IGNORE( self->HandleRedrawTimerEventL() );
    // Do not bother returning a meaningful error code, CPeriodic will ignore it
    // anyway.
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::SetVerticalItemOffset(TInt aOffset)
	{
	static_cast<CMmGridView*> (View())->SetItemOffsetInPixels(aOffset);
	UpdateScrollBarThumbs();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmGrid::VerticalItemOffset() const
	{
	return static_cast<CMmGridView*> (View())->VerticalItemOffset();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::SetItemHeight( TInt aItemHeight )
	{
	iItemHeight = aItemHeight;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::UpdateScrollBarThumbs()
	{
	CAknGrid::UpdateScrollBarThumbs();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmGrid::CountComponentControls() const
    {
    TInt componentControls(0);
    if ( !iDisableChildComponentDrawing )
    	{
        componentControls = CAknGrid::CountComponentControls();
    	}
    return componentControls;
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGrid::SetDisableChildComponentDrawing( TBool aDisable )
    {
    iDisableChildComponentDrawing = aDisable;
    }
//End of file
