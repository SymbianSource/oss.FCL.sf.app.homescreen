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
 *  Version     : %version: MM_50 % << Don't touch! Updated by Synergy at check-out.
 *
*/



#include "mmlistboxview.h"
#include "mmlistbox.h"
#include "mmlistboxitemdrawer.h"
#include "mmlistboxmodel.h"
#include "hnsuitemodel.h"
#include "hnitemsorder.h"
#include <eikfrlb.h>
#include <AknUtils.h>
#include <eikfrlbd.h>
#include <eikspmod.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsDrawUtils.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistboxtfx.h>
#include <aknlistboxtfxinternal.h>
#include <akntransitionutils.h>
#include <aknlistloadertfx.h>
#endif

CMmListBoxView::CMmListBoxView ()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxView::~CMmListBoxView ()
	{
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxView* CMmListBoxView::NewLC ()
	{
	CMmListBoxView* self = new (ELeave)CMmListBoxView();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxView* CMmListBoxView::NewL ()
	{
	CMmListBoxView* self=CMmListBoxView::NewLC ();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::ConstructL ()
	{
	iScrollbarIsVisible = ETrue;
	iPreviouslyDrawnCurrentItemIndex = KErrNotFound;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::UpdateAverageItemHeight ()
	{
	TInt count( iModel->NumberOfItems() );
	if ( !count )
	    {
	    SetItemHeight( 2 );
	    }
	else
	    {
	    TInt totalHeight = GetTotalHeight( 0, count - 1 );
	    TInt averageItemHeight = totalHeight / count;
	    if ( totalHeight % count )
	        {
	        ++averageItemHeight;
	        // this ensures that it is always possible to
	        // scroll to the very bottom of the view by
	        // using scrollbar.
	        }
	    SetItemHeight( averageItemHeight );
	    }
	}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxView::GetTotalHeight (const TInt aStartIndex, TInt aEndIndex) const
	{
	TInt totalHeight = 0;

	if ( aEndIndex >= 0)
		{
		TInt itemCount = iModel->NumberOfItems ();
		aEndIndex = (aEndIndex >= itemCount ) ? itemCount-1 : aEndIndex;
		CMmListBoxItemDrawer* drawer= STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer);
		for (TInt i(aStartIndex); i <= aEndIndex; i++)
			{
			totalHeight += drawer->GetItemHeight (i, CurrentItemIndex () == i);
			}
		}

	return totalHeight;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxView::GetLastIndexInHeight (const TInt aStartIndex, TInt aHeight) const
	{
	TInt i = aStartIndex;
	TInt totalHeight = 0;

	TInt itemCount(iModel->NumberOfItems () );
	CMmListBoxItemDrawer* drawer= STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer);

	for (; (i > -1) && (i < itemCount); i++)
		{
		totalHeight += drawer->GetItemHeight (i, CurrentItemIndex () == i);
		if ( totalHeight > aHeight)
			break;
		}

	TInt ret(i - aStartIndex);
	
	if ( !AknLayoutUtils::PenEnabled() && totalHeight > aHeight )
		{
		ret--; // exclude partial item
		}

	return ret;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxView::NumberOfItemsThatFitInRect(const TRect& aRect) const
	{
	return GetNumberOfItemsThatFitInRect( aRect, EFalse );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxView::GetNumberOfItemsThatFitInRect (const TRect& aRect,
		TBool aIncludePartialItem ) const
	{
	TInt i = iTopItemIndex;
	TInt totalHeight = iVerticalOffset;

	TInt itemCount( iModel->NumberOfItems () );
	CMmListBoxItemDrawer* drawer= STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer);
	while (totalHeight < aRect.Height ())
		{
		totalHeight += ((i > -1) && (i < itemCount )) ? drawer->GetItemHeight (
				i, CurrentItemIndex () == i) : iItemHeight;
		++i;
		}

	TInt ret(i - iTopItemIndex);

	if ( !AknLayoutUtils::PenEnabled() && !aIncludePartialItem && totalHeight > aRect.Height() )
		{
		ret--; // exclude partial item
		}

	return ret;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::CalcBottomItemIndex ()
	{

	TInt numberOfVisibleItems = NumberOfItemsThatFitInRect( iViewRect );
	iBottomItemIndex = Min( iTopItemIndex + numberOfVisibleItems - 1,
	        iModel->NumberOfItems() );

	// The next piece of code removes filtering from find box when
	// new list items are added.
	if ( Flags () & CListBoxView::EItemCountModified)
		{
		CAknFilteredTextListBoxModel *model= STATIC_CAST(CAknFilteredTextListBoxModel*,iModel);
		CAknListBoxFilterItems *filter = model ? model->Filter () : 0;
		if ( filter)
			{
			TRAP_IGNORE(filter->ResetFilteringL());
			}
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::Draw (const TRect* aClipRect) const
	{
    TBool drawingInitiated(EFalse);
    if ( CAknEnv::Static()->TransparencyEnabled() &&
    		iWin && iWin->GetDrawRect() == TRect::EUninitialized )
    	{
    	TRect a(ViewRect());
    	if (!aClipRect || *aClipRect == TRect(0,0,0,0) )
    		{
    		aClipRect = &a;
    		}
    	drawingInitiated=ETrue;
		iWin->Invalidate( *aClipRect );
		iWin->BeginRedraw( *aClipRect );
    	}

	DoDraw(aClipRect);

	CMmListBoxItemDrawer* itemDrawer =
			static_cast<CMmListBoxItemDrawer*>(iItemDrawer );
    if (aClipRect)
        {
        TRect rect(*aClipRect);
        rect.iTl.iY = ItemPos( BottomItemIndex() ).iY + ItemSize( BottomItemIndex() ).iHeight;

//      iGc->SetClippingRect( rect );
//		removed to prevent non-redraw drawing. Was present to prevent out of view drawing when effects are on.
//      could be removed because effects were disabled at some point in edit mode to enhance performance.
        itemDrawer->DrawFloatingItems( rect );
//      iGc->CancelClippingRect();
        }

    if ( CAknEnv::Static()->TransparencyEnabled() &&
    		iWin && drawingInitiated )
    	{
    	drawingInitiated = EFalse;
    	iWin->EndRedraw( );
    	}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::DoDraw(const TRect* aClipRect) const
	{
	CMmListBoxView* view= CONST_CAST( CMmListBoxView*, this );
	view->UpdateAverageItemHeight ();

	CMmListBoxModel* model = static_cast< CMmListBoxModel* > ( iModel );
	if ( model && model->GetSuiteModel()
			&& !model->GetSuiteModel()->GetItemsOrder()->IsSuiteReadyToShow() )
		{
		return;
		}

	if ( RedrawDisabled () || !IsVisible () )
		{
		return;
		}

	TInt i = iTopItemIndex;
	CMmListBoxItemDrawer* itemDrawer =
			static_cast<CMmListBoxItemDrawer*>(iItemDrawer );
	MAknsSkinInstance *skin = AknsUtils::SkinInstance ();
	CCoeControl* control = itemDrawer->FormattedCellData()->Control ();
	MAknsControlContext *cc = AknsDrawUtils::ControlContext (control);

	if ( !cc)
		{
		cc = itemDrawer->FormattedCellData()->SkinBackgroundContext ();
		}
	
	itemDrawer->SetTopItemIndex (iTopItemIndex);

	if ( iModel->NumberOfItems () > 0)
		{
		TBool drawingInitiated = ETrue;
		if ( CAknEnv::Static()->TransparencyEnabled () )
			{
			if ( iWin && iWin->GetDrawRect () == TRect::EUninitialized)
				{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
								MAknListBoxTfxInternal* transApi =
									CAknListLoader::TfxApiInternal( iGc );
								drawingInitiated = transApi && !transApi->EffectsDisabled();
#else
				drawingInitiated = EFalse;
#endif
				}

			if ( !drawingInitiated)
				{
				iWin->Invalidate ( *aClipRect);
				iWin->BeginRedraw ( *aClipRect);
				}
			}

		TInt lastPotentialItemIndex = Min (iModel->NumberOfItems (),
				iTopItemIndex + GetNumberOfItemsThatFitInRect( ViewRect (), ETrue ) );
		while (i < lastPotentialItemIndex)
			{
            DrawItem(i++);
            }

		RedrawBackground();

		if ( CAknEnv::Static()->TransparencyEnabled () && !drawingInitiated)
			{
			iWin->EndRedraw ();
			}
		}

	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::DrawItem (TInt aItemIndex) const
	{
	CMmListBoxItemDrawer* itemDrawer= STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer );
	TBool currentChanged( CurrentItemIndex() != iPreviouslyDrawnCurrentItemIndex );
	TBool redrawConsumed(EFalse);
	if ( currentChanged )
		{
		CMmListBoxView* view= CONST_CAST( CMmListBoxView*, this );
		redrawConsumed =
			static_cast<CMmListBox*> (itemDrawer->Widget())->RedrawIfNecessary(
								iPreviouslyDrawnCurrentItemIndex,
								CurrentItemIndex());
		view->SetPreviouslyDrawnCurrentItemIndex( CurrentItemIndex() );
		}
	
	if ( !redrawConsumed )
		{
		DrawSingleItem ( aItemIndex );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TSize CMmListBoxView::ItemSize (TInt aItemIndex) const
	{
	CMmListBoxView* view= CONST_CAST( CMmListBoxView*, this );
	view->UpdateAverageItemHeight ();

    CMmListBoxItemDrawer* itemDrawer= STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer );
    TSize size(CFormattedCellListBoxView::ItemSize(aItemIndex).iWidth, itemDrawer->GetItemHeight (aItemIndex,
            CurrentItemIndex () == aItemIndex) );

    if ( size.iHeight < 2)
        size.iHeight = 2;

    return size;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPoint CMmListBoxView::ItemPos (TInt aItemIndex) const
	{
	TInt vRealPos = CFormattedCellListBoxView::ItemPos(TopItemIndex()).iY;
	TInt totalHeight = 0;
	if ( aItemIndex > iTopItemIndex )
	    {
	    totalHeight = GetTotalHeight( iTopItemIndex, aItemIndex - 1 );
	    }
	else if ( aItemIndex < iTopItemIndex )
	    {
	    totalHeight = -GetTotalHeight( aItemIndex, iTopItemIndex - 1 );
	    }

	return TPoint (-iHScrollOffset + iViewRect.iTl.iX, iViewRect.iTl.iY
			+ totalHeight + vRealPos);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxView::XYPosToItemIndex (TPoint aPosition, TInt& aItemIndex) const
	{
	// returns ETrue and sets aItemIndex to the index of the item whose bounding box contains aPosition
	// returns EFalse if no such item exists
	TBool itemFound = EFalse;
	if ( iViewRect.Contains (aPosition))
		{
		TInt vRealPos = CFormattedCellListBoxView::ItemPos(TopItemIndex()).iY;
		// aPosition is inside the display area
		TInt numberOfVisibleItems = GetLastIndexInHeight (iTopItemIndex,
				aPosition.iY - iViewRect.iTl.iY - vRealPos );
		TInt itemAtSpecifiedPos = iTopItemIndex + numberOfVisibleItems;
		aItemIndex = itemAtSpecifiedPos;
		itemFound = ( GetTotalHeight( iTopItemIndex, iBottomItemIndex )
            >= aPosition.iY ) && ( iModel->NumberOfItems() > itemAtSpecifiedPos );
//        if ( itemFound )
//            {
//            // aPosition is inside the display area
//            TInt numberOfVisibleItems = GetLastIndexInHeight (iTopItemIndex,
//                    aPosition.iY - iViewRect.iTl.iY);
//            TInt itemAtSpecifiedPos = iTopItemIndex + numberOfVisibleItems;
//            aItemIndex = itemAtSpecifiedPos;
//            }
		}
	return itemFound;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::SetItemHeight (TInt aItemHeight)
	{
	//	we need to update the iItemHeight member in widget also (there are two different item height value holders - in widget and here in widget view)
	iItemHeight = aItemHeight;
	
	CMmListBoxItemDrawer* itemDrawer =
			STATIC_CAST( CMmListBoxItemDrawer*, ItemDrawer() );
	static_cast<CMmListBox*>(itemDrawer->Widget())->SetItemHeight( aItemHeight );
		
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxView::CalcNewTopItemIndexSoItemIsVisible (TInt aItemIndex) const
	{
	CMmListBoxItemDrawer* itemDrawer =
            static_cast<CMmListBoxItemDrawer*>( iItemDrawer );

	TInt newTopItemIndex = iTopItemIndex;

	TInt itemHeight = itemDrawer->
		GetItemHeight( aItemIndex, aItemIndex == CurrentItemIndex() );
	
	// ItemIsPartiallyVisible uses fixed iItemHeight, but we have to support
	// variable item height in lists, unfortunately ItemIsPartiallyVisible
	// is not virtual
    TPoint itemPosition( ItemPos( aItemIndex ) );
    TBool itemPartiallyVisible =
        ( itemPosition.iY < iViewRect.iTl.iY &&
          itemPosition.iY + itemHeight >= iViewRect.iTl.iY ) ||
        ( itemPosition.iY <= iViewRect.iBr.iY &&
          itemPosition.iY + itemHeight > iViewRect.iBr.iY );
		
	TBool itemIsFullyVisible = ItemIsVisible( aItemIndex ) &&
            !itemPartiallyVisible;

	TBool itemIsAboveVisibleArea = !itemIsFullyVisible &&
            ItemPos( aItemIndex ).iY < ViewRect().iTl.iY;
		
	TBool itemIsBeneathVisibleArea = !itemIsFullyVisible &&
            !itemIsAboveVisibleArea && ItemPos( aItemIndex ).iY + itemDrawer->
            GetItemHeight( aItemIndex, aItemIndex == CurrentItemIndex() ) >
            ViewRect().iBr.iY;

	if ( itemIsAboveVisibleArea )
	    {
	    newTopItemIndex = aItemIndex;
	    const_cast<CMmListBoxView*>( this )->SetItemOffsetInPixels( 0 );
	    }

	if ( itemIsBeneathVisibleArea )
        {
        const TInt viewHeight = ViewRect().Height(); 
        newTopItemIndex = aItemIndex;
        for ( ;; )
            {
            TInt totalHeight = GetTotalHeight( newTopItemIndex, aItemIndex );
            if ( totalHeight >= viewHeight || newTopItemIndex == 0 )
                {
                const_cast<CMmListBoxView*>( this )->SetItemOffsetInPixels(
                        viewHeight - totalHeight );
                break;
                }
            --newTopItemIndex;
            }
        }

	return newTopItemIndex;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::RedrawBackground (TRect aUsedPortionOfViewRect,
		TRect aSmallerViewRect) const
	{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(iGc);
    if (transApi)
        {
        transApi->StartDrawing(MAknListBoxTfxInternal::EListView);
        }
#endif

	CMmListBoxItemDrawer* itemDrawer = STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer );
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    CCoeControl* control = itemDrawer->FormattedCellData()->Control();
    MAknsControlContext *cc = AknsDrawUtils::ControlContext(control);

	if (control)
        {
        AknsDrawUtils::BackgroundBetweenRects(skin, cc, control, *iGc,
                aSmallerViewRect, aUsedPortionOfViewRect);
        }
    else
        {
        iGc->SetBrushColor(BackColor());
        DrawUtils::ClearBetweenRects(*iGc, aSmallerViewRect,
                aUsedPortionOfViewRect);
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if (transApi)
        {
        transApi->StopDrawing();
        }
#endif
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::RedrawBackground () const
    {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    CMmListBoxItemDrawer* drawer =
            static_cast<CMmListBoxItemDrawer*>( iItemDrawer );
    CMmTemplateLibrary* templateLib = drawer->TemplateLibrary();

    TInt usedPortionHeight = GetTotalHeight(iTopItemIndex, iBottomItemIndex );
    TInt usedPortionWidth = iViewRect.Width();
    if (templateLib->GetScrollbarVisibility())
        {
        usedPortionWidth -= templateLib->ScrollbarWidth();
        }

    TRect usedPortionOfViewRect(iViewRect.iTl, TSize(usedPortionWidth,
            usedPortionHeight));
    usedPortionOfViewRect.Move(0,
            CFormattedCellListBoxView::ItemPos(iTopItemIndex).iY);
    
    RedrawBackground(usedPortionOfViewRect, iViewRect);
#endif
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::SetPreviouslyDrawnCurrentItemIndex( TBool aIndex )
	{
	iPreviouslyDrawnCurrentItemIndex = aIndex;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::DrawSingleItem (TInt aItemIndex) const
	{
	CMmListBoxItemDrawer* itemDrawer =
            STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer );
	TBool highlightVisible = !( itemDrawer->Flags()
	        & CListItemDrawer::ESingleClickDisabledHighlight );
	TSize size = itemDrawer->GetItemSize( aItemIndex, highlightVisible &&
	        CurrentItemIndex() == aItemIndex );
	itemDrawer->SetItemCellSize( size );
	
	// CMmListBoxView* view= CONST_CAST( CMmListBoxView*, this );
	// view->SetItemHeight( size.iHeight );
	// The above line (currently commented-out) was originaly needed to correct
	// some drawing-related error which used to occur when moving highlight with
	// rocker keys. It seems that this is no longer needed. If anything should
	// change, please note that now the SetItemHeight method does much more than
	// it used to, so simply uncommenting this line would be a bad idea (consider
	// setting the iItemHeight member variable directly). 
	
	CFormattedCellListBoxView::DrawItem (aItemIndex);

	//To eliminate the effect of undrawn fragment of background, when the last
	//is drawn, background is refreshed
	if ( aItemIndex == ( iModel->NumberOfItems()-1 ) && ItemIsVisible( iModel->NumberOfItems()-1 ) )
		{
		CMmListBoxItemDrawer* itemDrawer= STATIC_CAST( CMmListBoxItemDrawer*, iItemDrawer );
		if ( !itemDrawer->IsEditMode() )
			{
			RedrawBackground();
			}
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxView::VerticalItemOffset() const
	{
	return iVerticalOffset;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxView::ScrollToMakeItemVisible(TInt aItemIndex)
    {
    TBool scrollConsumed(EFalse);
    if ( !iScrollToItemDisabled )
        {
        scrollConsumed = CFormattedCellListBoxView::ScrollToMakeItemVisible(
                aItemIndex);
        }
    return scrollConsumed;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxView::DisableScrollToItem( TBool aDisable )
    {
    iScrollToItemDisabled = aDisable;
    }

// End of file
