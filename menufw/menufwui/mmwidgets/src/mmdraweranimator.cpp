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
 *  Version     : %version: MM_43 % << Don't touch! Updated by Synergy at check-out.
 *
 */

#include <AknUtils.h>

#include "mmdraweranimator.h"
#include "mmlistboxitemdrawer.h"
#include "mmfloatingitem.h"
#include "mmwidgetcontainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmDrawerAnimator::CMmDrawerAnimator( CMmListBoxItemDrawer& aDrawer ) :
    CActive( AknLayoutUtils::PenEnabled()
        ? EPriorityAbsoluteVeryLow
        : EPriorityAbsoluteRealTime8 ), iDrawer( aDrawer ),
    iLastNotedHighlight( KErrNotFound ), iLastNotedTopItem( KErrNotFound ),
    iLastNotedVerticalOffset( 0 ),
    iUsualAnimationFramesCount( AknLayoutUtils::PenEnabled()
                    ? MmEffects::KUsualAnimationFramesCount
                    : MmEffects::KUsualAnimationFramesCountNonTouch ),
    iPreparedForGarbage( EFalse )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmDrawerAnimator* CMmDrawerAnimator::NewLC( CMmListBoxItemDrawer& aDrawer )
    {
    CMmDrawerAnimator* self = new ( ELeave ) CMmDrawerAnimator( aDrawer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmDrawerAnimator* CMmDrawerAnimator::NewL( CMmListBoxItemDrawer& aDrawer )
    {
    CMmDrawerAnimator* self = CMmDrawerAnimator::NewLC( aDrawer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() ); // Initialize timer
    CActiveScheduler::Add( this ); // Add to scheduler
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmDrawerAnimator::~CMmDrawerAnimator()
    {
    Cancel(); // Cancel any request, if outstanding
    iTimer.Close(); // Destroy the RTimer object
    // Delete instance variables if any
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::DoCancel()
    {
    iTimer.Cancel();
    if( iDrawer.GetFloatingItemCount() == 0 )
        {
        TRAP_IGNORE(TriggerMoveItemL());
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::RunL()
    {
    if( iPreparedForGarbage )
        {
        return;
        }

    if( iDrawer.GetFloatingItemCount() > 0 )
        {
        TRect refreshRect;
        for( int i = 0; i < iDrawer.GetFloatingItemCount(); i++ )
            {
            TMmFloatingItem& current = iDrawer.GetFloatingItemAtIndex( i );
            TSize itemSize = iDrawer.GetItemSize(current.GetDrawnItemIndex(), EFalse);

            TRect beforeRect( current.GetItemPosition(), itemSize );
            current.MakeStep();
            TRect afterRect( current.GetItemPosition(), itemSize );

            if( ( current.GetFloatingItemType() == EDrag
                        && iDrawer.GetFloatingItemIndex( EDragStart ) == KErrNotFound )
                    || current.GetFloatingItemType() == EDragStart )
                {
                TInt dragTrail = iDrawer.GetFloatingItemIndex( EPostDragRefreshItem );
                if( dragTrail != KErrNotFound )
                    {
                    refreshRect = ( refreshRect == TRect() ) ? beforeRect : refreshRect;
                    refreshRect.BoundingRect( afterRect );
                    static_cast<CMmWidgetContainer*> ( iDrawer.Widget()->Parent() )->
                            SetAllowMove( EFalse );
                    }
                }
            else if( ( current.GetFloatingItemType() == EDrag
                            && iDrawer.GetFloatingItemIndex( EDragStart ) == KErrNotFound )
                        || current.GetFloatingItemType() != EDrag )
                {
                refreshRect = (refreshRect == TRect() ) ? beforeRect : refreshRect;
                refreshRect.BoundingRect( beforeRect );
                refreshRect.BoundingRect( afterRect );
                }
            }
        const TInt highlightedItemIndex = iDrawer.Widget()->View()->CurrentItemIndex();
        if( iLastNotedHighlight != highlightedItemIndex )
            {
            TRect highlightedItemRect(
                    iDrawer.Widget()->View()->ItemPos( highlightedItemIndex ),
                    iDrawer.Widget()->View()->ItemSize( highlightedItemIndex ) );
            refreshRect.BoundingRect( highlightedItemRect );
            iLastNotedHighlight = highlightedItemIndex;
            }


        TInt currentVerticalOffset = static_cast<CMmWidgetContainer*>(
              iDrawer.Widget()->Parent() )->VerticalItemOffset();

        if( iLastNotedTopItem != iDrawer.Widget()->TopItemIndex()
                || iLastNotedVerticalOffset != currentVerticalOffset )
            {
            iDrawer.Widget()->DrawNow();
            iLastNotedTopItem = iDrawer.Widget()->TopItemIndex();
            iLastNotedVerticalOffset = currentVerticalOffset;
            }
        else
            {
            if( refreshRect != TRect() )
                {
                TRect prev( iPreviousRefreshRect );
                iPreviousRefreshRect = refreshRect;
                refreshRect.BoundingRect( prev );

                // expand rect if non-touch
                refreshRect = TRect( AdjustRefreshRectToNonTouch( refreshRect ) );

                // never draw on the outside of widget's view rectangle
                refreshRect.Intersection( iDrawer.Widget()->View()->ViewRect() );

                iDrawer.Widget()->View()->Draw( &refreshRect );
                }
            }

        iLastRedrawTime.HomeTime();

        }

    if( iDrawer.GetFloatingItemCount() > 0 )
        {
        Trigger();
        }
    else
        {
        TriggerMoveItemL();
        Cancel();

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        if( iTransTfx && iTransTfxInternal->EffectsDisabled() )
            {
            iTransTfx->EnableEffects( ETrue );

            if( iTransTfx )
                {
                iTransTfxInternal->Remove( MAknListBoxTfxInternal::EListEverything );
                iDrawer.Widget()->DrawNow();
                iTransTfxInternal->Draw( iDrawer.Widget()->Rect() );
                }
            }
#endif
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TRect CMmDrawerAnimator::AdjustRefreshRectToNonTouch( const TRect& aRefreshRect )
    {
    TRect rect( aRefreshRect );
    if( !AknLayoutUtils::PenEnabled() )
        {
        TRect indicatorRect( iDrawer.GetIndicatorRect() );
        TSize itemSize( iDrawer.GetItemSize(
                iDrawer.Widget()->CurrentItemIndex(), ETrue ) );

        TInt horizontalMargin( ( indicatorRect.Width() - itemSize.iWidth ) / 2 );
        TInt verticalMargin( ( indicatorRect.Height() - itemSize.iHeight ) / 2 );
        rect.Grow(horizontalMargin, verticalMargin);
        }
    return rect;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmDrawerAnimator::Trigger()
    {
    if( iPreparedForGarbage )
        {
        return KErrCancel;
        }

    if( !IsActive() && iDrawer.GetFloatingItemCount() > 0 )
        {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        iTransTfx = CAknListLoader::TfxApi( iDrawer.Gc() );
        iTransTfxInternal = CAknListLoader::TfxApiInternal( iDrawer.Gc() );

        if( iTransTfx && !iTransTfxInternal->EffectsDisabled() )
            {
            iTransTfx->EnableEffects( EFalse );

            if( iTransTfxInternal )
                {
                iTransTfxInternal->Remove( MAknListBoxTfxInternal::EListEverything );
                iTransTfxInternal->Draw( iDrawer.Widget()->Rect() );
                }
            }
#endif

        iTimer.After( iStatus, TTimeIntervalMicroSeconds32(
                MmEffects::KAnimationFrameDelay ) );
        SetActive();
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmDrawerAnimator::RunError( TInt aError )
    {
    return aError;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::AnimateDragItemTransitionL()
    {
    if( KErrNotFound != iDrawer.GetFloatingItemIndex( EDrag )
            || KErrNotFound != iDrawer.GetFloatingItemIndex( EDragStart ) )
        {
        TMmFloatingItemType floatingType =
                iDrawer.GetFloatingItemIndex( EDrag ) != KErrNotFound
                    ? EDrag : EDragStart;
        TMmFloatingItem floatingItem(
                iDrawer.GetFloatingItemL( floatingType ).GetDrawnItemIndex(),
                iDrawer.GetFloatingItemL( floatingType ).GetItemPosition(),
                EDragTransition, floatingType == EDrag
                    ? iUsualAnimationFramesCount : MmEffects::KNoAnimationFramesCount,
                iDrawer.Widget()->View() );
        TPoint pointEnd = iDrawer.Widget()->View()->ItemPos( floatingItem.GetDrawnItemIndex() );
        floatingItem.SetPositionStep( pointEnd - floatingItem.GetItemPosition() );
        iDrawer.AddFloatingItemL( floatingItem, 0 );

        TMmFloatingItem postDragRefresh(
                iDrawer.GetFloatingItemL( floatingType ).GetDrawnItemIndex(),
                pointEnd, EPostDragRefreshItem,
                MmEffects::KNoAnimationFramesCount, iDrawer.Widget()->View() );
        iDrawer.AddFloatingItemL( postDragRefresh );

        iDrawer.RemoveFloatingItem( iDrawer.GetFloatingItemIndex( EDrag ) );
        iDrawer.RemoveFloatingItem( iDrawer.GetFloatingItemIndex( EDragStart ) );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmDrawerAnimator::IsReadyForNewAnimation()
    {
    if( iPreparedForGarbage )
        {
        return EFalse;
        }

    for( int i = 0; i < iDrawer.GetFloatingItemCount(); i++ )
        {
        TMmFloatingItemType type =
                iDrawer.GetFloatingItemAtIndex( i ).GetFloatingItemType();
        if( type != EDrag && type != EZoomTransition )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::CancelAnimationsL()
    {
    for( int i = 0; i < iDrawer.GetFloatingItemCount(); i++ )
        {
        TMmFloatingItem& current = iDrawer.GetFloatingItemAtIndex( i );
        if( current.GetFloatingItemType() != EDrag )
            {
            current.InvalidateFloatingItem();
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::AnimateItemSwapL( TInt aItemFrom, TInt aItemTo )
    {
    TMmFloatingItem floatingItem( aItemTo,
            iDrawer.Widget()->View()->ItemPos( aItemFrom ),
            ESwapTransition, iUsualAnimationFramesCount,
            iDrawer.Widget()->View() );

    for( int i = 0; i < iDrawer.GetFloatingItemCount(); i++ )
        {
        TMmFloatingItem& current = iDrawer.GetFloatingItemAtIndex( i );

        if( current.GetFloatingItemType() == EZoomTransition
                && current.GetDrawnItemIndex() == aItemFrom )
            {
            current.InvalidateFloatingItem();
            floatingItem.SetSizeStep( current.GetCurrentZoomRatio(), 1.0f );
            break;
            }
        }

    floatingItem.SetPositionStep( iDrawer.Widget()->View()->ItemPos(aItemTo)
            - iDrawer.Widget()->View()->ItemPos(aItemFrom) );
    iDrawer.AddFloatingItemL( floatingItem );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::AnimateItemZoomL( TInt aItemIndex, TBool aZoomIn )
    {
    TMmFloatingItem floatingItem( aItemIndex,
            iDrawer.Widget()->View()->ItemPos( aItemIndex ),
            EZoomTransition, iUsualAnimationFramesCount,
            iDrawer.Widget()->View() );
    floatingItem.SetManualDelete( aZoomIn );
    floatingItem.SetSizeStep(
            (aZoomIn) ? KNormalStateZoomRatio : KZoomStateZoomRatio,
            (aZoomIn) ? KZoomStateZoomRatio : KNormalStateZoomRatio );

    TInt animationFound = EFalse;
    for( int i = 0; i < iDrawer.GetFloatingItemCount(); i++ )
        {
        TMmFloatingItem& current = iDrawer.GetFloatingItemAtIndex( i );
        if( current.GetFloatingItemType() == EZoomTransition
                && current.GetDrawnItemIndex() == aItemIndex )
            {
            current.InvalidateFloatingItem();
            floatingItem.SetSizeStep( current.GetCurrentZoomRatio(),
                    (aZoomIn) ? KZoomStateZoomRatio : KNormalStateZoomRatio);
            animationFound = ETrue;
            static_cast<CMmWidgetContainer*> ( iDrawer.Widget()->Parent() )->
                    SetAllowMove( EFalse );
            break;
            }
        }

    if( ( aZoomIn != EFalse || animationFound != EFalse ) )
        {
        // This covers the situation, when a zoom out animation is requested
        // whilst no zoom in occured earlier. In this case the request is
        // neglected.
        iDrawer.AddFloatingItemL( floatingItem );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::SetNextRedrawToWholeScreen()
    {
    iPreviousRefreshRect = iDrawer.Widget()->View()->ViewRect();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::PrepareForGarbage()
    {
    //	Cancel any outstanding requests
    Cancel();
    iPreparedForGarbage = ETrue;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::TriggerMoveItemL()
    {
    static_cast<CMmWidgetContainer*> ( iDrawer.Widget()->Parent() )->
            TriggerMoveItemL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmDrawerAnimator::AnimateDragItemStartL( TInt aDraggedIndex, TPoint aPoint )
    {
    TMmFloatingItem floatingItem(
            aDraggedIndex,
            iDrawer.Widget()->View()->ItemPos( aDraggedIndex ),
            EDragStart,
            MmEffects::KUsualAnimationFramesCountNonTouch,
            iDrawer.Widget()->View() );
    floatingItem.SetPositionStep( aPoint - floatingItem.GetItemPosition() );
    iDrawer.AddFloatingItemL( floatingItem, 0 );
    }
//End of file
