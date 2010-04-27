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


#include <eikfrlbd.h>

#include "mmgridview.h"
#include "mmgrid.h"
#include "mmlistboxitemdrawer.h"
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGridView::CMmGridView() :
    iLastCurMove( ECursorFirstItem ), iOldIndex( KErrNotFound )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGridView::~CMmGridView()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGridView* CMmGridView::NewLC()
    {
    CMmGridView* self = new (ELeave)CMmGridView();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmGridView* CMmGridView::NewL()
    {
    CMmGridView* self = CMmGridView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPoint CMmGridView::ItemPos( TInt aItemIndex ) const
    {
    if ( aItemIndex < 0 )
        {
        // let avkon handle the insane cases
        return CAknGridView::ItemPos( aItemIndex );
        }

    if ( AknLayoutUtils::LayoutMirrored() )
        {
        const TInt colNum = NumberOfColsInView();
        TInt itemCol = aItemIndex % colNum;
        TInt mirroredItemCol = colNum - itemCol - 1;
        aItemIndex = aItemIndex - itemCol + mirroredItemCol;
        }

    // return CAknGridView::ItemPos( aItemIndex );
    return CorrectItemPos( aItemIndex );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPoint CMmGridView::CorrectItemPos( TInt aItemIndex ) const
    {
    // it the assertion below fails, review this implementation to make sure that
    // primary vertical case is handled correctly
    ASSERT( !IsPrimaryVertical() );

    ASSERT( aItemIndex >= 0 );
    const TInt colNum = NumberOfColsInView();
    TInt itemRow = aItemIndex / colNum;
    TInt itemCol = aItemIndex % colNum;

    TInt topItemRow = TopItemIndex() / colNum;
//    __ASSERT_DEBUG( TopItemIndex() % colNum == 0, User::Invariant() );

    // it is safe to assume that size between items is (0, 0) because we
    // explicitly set such value in CMmGrid::DoSetupLayoutL
    const TSize sizeBetweenItems( 0, 0 );

    TPoint itemPos(
        iViewRect.iTl.iX + itemCol *
            ( ColumnWidth() + sizeBetweenItems.iWidth ),
        iViewRect.iTl.iY + (itemRow - topItemRow) *
            ( iItemHeight + sizeBetweenItems.iHeight ) + iVerticalOffset );

    return itemPos;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmGridView::XYPosToItemIndex( TPoint aPosition, TInt& aItemIndex ) const
    {
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        aPosition.iX = iViewRect.Width() - ( aPosition.iX - iViewRect.iTl.iX );
        }
    return CAknGridView::XYPosToItemIndex( aPosition, aItemIndex );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CWindowGc* CMmGridView::Gc()
    {
    return iGc;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::MoveCursorL( TCursorMovement aCursorMovement,
        TSelectionMode aSelectionMode )
    {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    iLastCurMove = aCursorMovement;
    iOldIndex = iCurrentItemIndex;
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    CAknGridView::MoveCursorL( aCursorMovement, aSelectionMode );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    iLastCurMove = ECursorFirstItem;
    iOldIndex = KErrNotFound;
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::UpdateSelectionL( TSelectionMode aSelectionMode )
    {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        MAknListBoxTfxInternal* api = CAknListLoader::TfxApiInternal( Gc() );
        TInt row( 0 );
        TInt col( 0 );
        TInt newRow( 0 );
        TInt newCol( 0 );
        LogicalPosFromListBoxIndex( iOldIndex, row, col );
        LogicalPosFromListBoxIndex( iCurrentItemIndex, newRow, newCol );
        if ( api )
            {
            if ( iLastCurMove == CAknGridView::ECursorNextColumn )
                {
                if ( newCol < col || newRow != row )
                    api->SetMoveType( MAknListBoxTfxInternal::EListNoMovement );
                }
            else if ( iLastCurMove == CAknGridView::ECursorPreviousColumn )
                {
                if ( newCol > col || newRow != row )
                    api->SetMoveType( MAknListBoxTfxInternal::EListNoMovement );
                }
            }
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    CAknGridView::UpdateSelectionL( aSelectionMode );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::Draw(const TRect* aClipRect) const
    {
    const_cast<CMmGridView*>( this )->UpdateItemHeightAndWidth();
    DoDraw( aClipRect );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::DoDraw(const TRect* aClipRect) const
    {
    CMmListBoxItemDrawer* itemDrawer = static_cast< CMmListBoxItemDrawer*> ( ItemDrawer() );
    TBool drawingInitiated(EFalse);
    if ( CAknEnv::Static()->TransparencyEnabled() &&
    		iWin && iWin->GetDrawRect() == TRect::EUninitialized )
    	{
    	TRect a;
    	if (!aClipRect || *aClipRect == TRect(0,0,0,0) )
    		{
    		a = ViewRect();
    		aClipRect = &a;
    		}

    	drawingInitiated=ETrue;
		iWin->Invalidate( *aClipRect );
		iWin->BeginRedraw( *aClipRect );
    	}

    if ( !itemDrawer->IsEditMode() )
        {
        itemDrawer->DrawBackground(ViewRect());
        itemDrawer->SetRedrawItemBackground( EFalse );
        CAknGridView::Draw( aClipRect );
        itemDrawer->SetRedrawItemBackground( ETrue );
        }
    else
        {
        CAknGridView::Draw( aClipRect );
        }

	if ( aClipRect )
	    {
    	TRect rect(*aClipRect);
    	TInt lastItemInView = (iModel->NumberOfItems() <= BottomItemIndex() )
    	    ? iModel->NumberOfItems()-1 : BottomItemIndex();
    	rect.iTl.iY = ItemPos( lastItemInView ).iY + ItemSize( lastItemInView ).iHeight;
//      iGc->SetClippingRect( rect );
//		removed to prevent non-redraw drawing. Was present to prevent out of view drawing when effects are on.
//      could be removed because effects were disabled at some point in edit mode to enhance performance.
    	itemDrawer->DrawFloatingItems( rect );
//      iGc->CancelClippingRect();
	    }

//    if (aClipRect)
//    	{
//    	const_cast< CMmGridView* >(this)->Gc()->DrawRect(*aClipRect);
//    	}

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
TInt CMmGridView::VerticalItemOffset() const
	{
	return iVerticalOffset;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::SetItemHeight(TInt aItemHeight)
	{
	//	we need to update the iItemHeight member in grid also (there are two different item height value holders - in grid and here in grid view)
	CMmListBoxItemDrawer* itemDrawer =
	        STATIC_CAST( CMmListBoxItemDrawer*, ItemDrawer() );
	static_cast<CMmGrid*>(itemDrawer->Widget())->SetItemHeight( aItemHeight );

	CAknGridView::SetItemHeight(aItemHeight);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridView::UpdateItemHeightAndWidth()
    {
    if ( iModel && iModel->NumberOfItems() )
        {
        CMmListBoxItemDrawer* itemDrawer =
                STATIC_CAST( CMmListBoxItemDrawer*, ItemDrawer() );
        TSize size = itemDrawer->GetItemSize( 0, EFalse );
        if ( itemDrawer->ItemCellSize() != size  )
            {
            const_cast<CMmGridView*>( this )->SetItemHeight( size.iHeight );
            const_cast<CMmGridView*>( this )->SetColumnWidth( size.iWidth );
            }
        }
    }

//End of file
