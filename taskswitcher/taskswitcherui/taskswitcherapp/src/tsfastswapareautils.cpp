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
* Description:  Taskswitcher Fast Swap area UI
 *
*/

#include <w32std.h>
#include <eikenv.h>
#include <layoutmetadata.cdl.h>
#include <touchfeedback.h>
#include <akntransitionutils.h>
#include <akntranseffect.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <bitmaptransforms.h>


#include "tsfastswapareautils.h"
#include "tsfastswaparea.h"
#include "tsappui.h"
#include "tsfswentry.h"


const TInt KLayoutItemCount = 4;

 /* ================================================================================
  * CTsFastSwapTimer
  * ================================================================================
  */

// -----------------------------------------------------------------------------
// CTsFastSwapTimer::CTsFastSwapTimer
// -----------------------------------------------------------------------------
//
CTsFastSwapTimer::CTsFastSwapTimer( MTsFastSwapTimerObserver& aObserver )
: CTimer( EPriorityStandard ),
  iObserver( &aObserver )
    {
    }


// -----------------------------------------------------------------------------
// CTsFastSwapTimer::CTsGridHighlightTimer
// -----------------------------------------------------------------------------
//
CTsFastSwapTimer::~CTsFastSwapTimer()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFastSwapTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CTsFastSwapTimer::CTsGridHighlightTimer
// -----------------------------------------------------------------------------
//
void CTsFastSwapTimer::RunL()
    {
    iObserver->TimerCompletedL(this);
    }
 


/* ================================================================================
 * CTsBitmapScaleTask
 * ================================================================================
 */

// -----------------------------------------------------------------------------
// CTsBitmapScaleTask::
// -----------------------------------------------------------------------------
//
CTsBitmapScaleTask::CTsBitmapScaleTask( CTsFastSwapAreaExtension& aObserver )
: CActive(EPriorityStandard),
  iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CTsBitmapScaleTask::
// -----------------------------------------------------------------------------
//
CTsBitmapScaleTask::~CTsBitmapScaleTask()
    {
    Cancel();
    delete iScaler;
    delete iSourceBmp;
    }


// -----------------------------------------------------------------------------
// CTsBitmapScaleTask::
// -----------------------------------------------------------------------------
//
void CTsBitmapScaleTask::StartLD( CFbsBitmap* aSourceBitmap,
                                  CFbsBitmap* aTargetBitmap )
    {
    if ( !iScaler )
        {
        iSourceBmp = aSourceBitmap;
        iScaler = CBitmapScaler::NewL();
        iScaler->Scale( &iStatus, *aSourceBitmap, *aTargetBitmap );
        SetActive();
        }
    }


// -----------------------------------------------------------------------------
// CTsBitmapScaleTask::
// -----------------------------------------------------------------------------
//
void CTsBitmapScaleTask::RunL()
    {
    iObserver.ScaleFinished( this );
    delete this;
    }


// -----------------------------------------------------------------------------
// CTsBitmapScaleTask::
// -----------------------------------------------------------------------------
//
void CTsBitmapScaleTask::DoCancel()
    {
    iScaler->Cancel();
    }


/* ================================================================================
 * CTsFastSwapAreaExtension
 * ================================================================================
 */

// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::NewL
// -----------------------------------------------------------------------------
//
CTsFastSwapAreaExtension* CTsFastSwapAreaExtension::NewL( CTsFastSwapArea& aFastSwapArea,
                                                  CEikonEnv& aEnv )
    {
    CTsFastSwapAreaExtension* self = CTsFastSwapAreaExtension::NewLC(
            aFastSwapArea, aEnv );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::NewLC
// -----------------------------------------------------------------------------
//
CTsFastSwapAreaExtension* CTsFastSwapAreaExtension::NewLC( CTsFastSwapArea& aFastSwapArea,
                                                   CEikonEnv& aEnv )
    {
    CTsFastSwapAreaExtension* self = new (ELeave) CTsFastSwapAreaExtension(
            aFastSwapArea, aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::~CTsFastSwapAreaExtension
// -----------------------------------------------------------------------------
//
CTsFastSwapAreaExtension::~CTsFastSwapAreaExtension()
    {
    iScaleTasks.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFastSwapAreaExtension::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::CTsFastSwapAreaExtension
// -----------------------------------------------------------------------------
//
CTsFastSwapAreaExtension::CTsFastSwapAreaExtension( CTsFastSwapArea& aFastSwapArea,
                                            CEikonEnv& aEnv )
: iFastSwapArea( aFastSwapArea ),
  iEnv( aEnv )
    {
    }

// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::GetCurrentScreenOrientation
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapAreaExtension::GetCurrentScreenOrientation()
    {
    TPixelsAndRotation availableRect;
    iEnv.ScreenDevice()->GetDefaultScreenSizeAndRotation(availableRect);
    return availableRect.iPixelSize.iWidth > availableRect.iPixelSize.iHeight;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::GetVariety
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapAreaExtension::GetVariety( TInt& aVariety )
    {
    aVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TBool foreground = static_cast<CTsAppUi*>(iEnv.AppUi())->IsForeground();
    if ( foreground )
        {
        TInt screenOrientation = GetCurrentScreenOrientation();
        if ( aVariety != screenOrientation )
            {
            aVariety = screenOrientation;
            return ETrue;
            }
        }
    return EFalse;
    }


// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::GetFastSwapAreaRects
// --------------------------------------------------------------------------
//
void CTsFastSwapAreaExtension::GetFastSwapAreaRects( RArray<TAknLayoutRect>& aRects )
    {
    TAknLayoutRect gridAppPane;
    TAknLayoutRect gridItem;
    TAknLayoutRect gridImage;
    TInt variety;
    TBool disable = GetVariety(variety);
    if ( disable )
        {
        TRAP_IGNORE(static_cast<CTsAppUi*>(iEnv.AppUi())->RequestPopUpL());
        }
    variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0; // double check to avoid layout panic
    gridAppPane.LayoutRect( iFastSwapArea.Rect(), 
            AknLayoutScalable_Apps::tport_appsw_pane_g1( variety ) );
    aRects.Append(gridAppPane);
    
    gridItem.LayoutRect( gridAppPane.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane( variety, 0, 0 ) );
    aRects.Append(gridItem);
    
    gridImage.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g1( variety ) ); 
    aRects.Append(gridImage);
    
    if ( disable )
        {
        TRAP_IGNORE(static_cast<CTsAppUi*>(iEnv.AppUi())->DisablePopUpL());
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::ItemPosition
// -----------------------------------------------------------------------------
//
TPoint CTsFastSwapAreaExtension::ItemViewPosition( TInt aItemIdx )
    {
    TPoint retVal = iFastSwapArea.Rect().iTl;
    TInt maxItemsOnScreen = iFastSwapArea.GetGridItemData( CTsFastSwapArea::EMaxItemsOnScreen );
    TInt gridItemWidth = iFastSwapArea.GetGridItemData( CTsFastSwapArea::EGridItemWidth );
    TInt gridItemGap = iFastSwapArea.GetGridItemData( CTsFastSwapArea::EGridItemGap );
    
    if ( aItemIdx < 0 )
        {
        // No items
        retVal.iX = 0;
        }
    else if ( aItemIdx == 0 )
        {
        // First item
        if( AknLayoutUtils::LayoutMirrored() )
            {
            if ( iFastSwapArea.GridItemCount() > maxItemsOnScreen )
                {
                retVal.iX = iFastSwapArea.GridWorldSize().iWidth - iFastSwapArea.Rect().Width();
                }
            else
                {
                retVal.iX = 0;
                }
            }
        else // normal layout
            {
            retVal.iX = 0;
            }
        }
    else if ( aItemIdx == iFastSwapArea.GridItemCount() - 1 )
        {
        // Last item selected
        if( AknLayoutUtils::LayoutMirrored() )
            {
            retVal.iX = 0;
            }
        else // normal layout
            {
            if ( iFastSwapArea.GridItemCount() > maxItemsOnScreen )
                {
                retVal.iX = iFastSwapArea.GridWorldSize().iWidth - iFastSwapArea.Rect().Width();
                }
            else
                {
                retVal.iX = 0;
                }
            }
        }
    else
        {
        // Middle item
        TInt screenMiddleItemOffset = ( iFastSwapArea.Rect().Width() - gridItemWidth ) / 2;
        if( AknLayoutUtils::LayoutMirrored() )
            {
            retVal.iX = gridItemWidth * ( iFastSwapArea.GridItemCount() - 1 - aItemIdx ) - screenMiddleItemOffset;
            retVal.iX += ( iFastSwapArea.GridItemCount() - 1 - aItemIdx ) * gridItemGap;
            }
        else // normal layout
            {
            retVal.iX = gridItemWidth * aItemIdx - screenMiddleItemOffset;
            retVal.iX += gridItemGap * aItemIdx;
            }
        if ( retVal.iX < 0 )
            {
            retVal.iX = 0;
            }
        // aligin to grid end edge in case
        // (item's position + draw area) extends over grid area and
        // (all items cover more than grid area or penultimate item)
        else if ( retVal.iX + iFastSwapArea.Rect().Width() > iFastSwapArea.GridWorldSize().iWidth &&
              (iFastSwapArea.GridWorldSize().iWidth > gridItemWidth * iFastSwapArea.GridItemCount() ||
               (aItemIdx > 1 &&   aItemIdx == iFastSwapArea.GridItemCount() - 2 )) )
            {
            retVal.iX = iFastSwapArea.GridWorldSize().iWidth - iFastSwapArea.Rect().Width();
            }
        }
    
    // Return middle of the view rectangle
    retVal.iX += iFastSwapArea.Rect().Width() / 2;
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::ViewToVisibleItem
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapAreaExtension::ViewToVisibleItem( const TPoint aViewPos )
    {
    TInt retVal(0);
    TPoint absViewPos = aViewPos;
    absViewPos.iX -= iFastSwapArea.Rect().Width() / 2;
    if ( absViewPos.iX < 0 )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // View crossed left border of grid world rect, last item selected
            retVal = iFastSwapArea.GridItemCount() - 1;
            }
        else // normal layout
            {
            // View crossed left border of grid world rect, first item selected
            retVal = 0;
            }
        }
    else if ( absViewPos.iX + iFastSwapArea.Rect().Width() > iFastSwapArea.GridWorldSize().iWidth )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // View crossed right border of grid world rect, first item selected
            retVal = 0;
            }
        else // normal layout
            {
            // View crossed right border of grid world rect, last item selected
            retVal = iFastSwapArea.GridItemCount() - 1;
            }
        }
    else
        {
        TInt retItemPosX(0);
        TInt offsetCheck = iFastSwapArea.GridWorldSize().iWidth;
        // View inside of grid world rect
        for ( TInt i = 0 ; i < iFastSwapArea.GridItemCount(); i++ )
            {
            TInt itemPosX = ItemViewPosition( i ).iX;
            TInt offset = aViewPos.iX - itemPosX;
            if ( Abs( offset ) <= offsetCheck )
                {
                offsetCheck = Abs( offset );
                retVal = i;
                retItemPosX = itemPosX;
                }
            else
                {
                break;
                }
            }
        // Check if item is fully visible. If not
        // return next one if possible
        TInt gridItemWidth = iFastSwapArea.GetGridItemData( CTsFastSwapArea::EGridItemWidth );
        if ( retItemPosX - gridItemWidth / 2 < absViewPos.iX &&
             retVal + 1 < iFastSwapArea.GridItemCount() )
            {
            retVal++;
            }
        }
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::LaunchPopupFeedback
// -----------------------------------------------------------------------------
//
void CTsFastSwapAreaExtension::LaunchPopupFeedback()
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            TTouchLogicalFeedback fbLogicalType = ETouchFeedbackPopUp;
            if ( CAknTransitionUtils::TransitionsEnabled(
                 AknTransEffect::EComponentTransitionsOff ) )
                {
                fbLogicalType = ETouchFeedbackIncreasingPopUp;
                }
            feedback->InstantFeedback( &iFastSwapArea,
                                       fbLogicalType,
                                       ETouchFeedbackVibra,
                                       TPointerEvent() );
            }
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapAreaExtension::PreferredImageSize
// -----------------------------------------------------------------------------
//
TSize CTsFastSwapAreaExtension::PreferredImageSize()
    {
    TAknLayoutRect gridImage;
    TRAP_IGNORE(
        RArray<TAknLayoutRect> rects;
        CleanupClosePushL(rects);
        rects.ReserveL(KLayoutItemCount);
        GetFastSwapAreaRects(rects);
        gridImage = rects[2];
        CleanupStack::PopAndDestroy(&rects);
        );
    return gridImage.Rect().Size();
    }

// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::CalculateSizePreserveRatio
// --------------------------------------------------------------------------
//
TSize CTsFastSwapAreaExtension::CalculateSizePreserveRatio(
        const TSize& aTargetAreaSize,
        const TSize& aSourceSize )
    {
    const TReal 
      resizedAspectRatio(aTargetAreaSize.iWidth/(TReal)aTargetAreaSize.iHeight);
    const TReal 
      orginalAspectRatio(aSourceSize.iWidth/(TReal)aSourceSize.iHeight);
    //this condition avoid empty margins ( bigger output ). to realy fit area change it
    const TReal scaleFactor = 
        ( orginalAspectRatio > resizedAspectRatio ) ? 
        (aTargetAreaSize.iHeight /(TReal)aSourceSize.iHeight) ://scale by height
        (aTargetAreaSize.iWidth /(TReal)aSourceSize.iWidth) ;//scale by width
    return TSize(aSourceSize.iWidth * scaleFactor, aSourceSize.iHeight * scaleFactor);
    }


// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::CopyBitmapL
// Copy and scale.
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFastSwapAreaExtension::CopyBitmapL( TInt aFbsHandle, TSize aSize, TBool aIsScreenshot  )
    {
    CFbsBitmap* ret = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( ret );

    CFbsBitmap* bmp = DuplicateBitmapLC( aFbsHandle );
    
    // First scale using normal method, to show "draft" screenshots.
    // Do not always use aSize, preserving the aspect ratio is quite
    // important when showing app icons instead of screenshots
    TSize sz = CalculateSizePreserveRatio( aSize, bmp->SizeInPixels() );
    User::LeaveIfError( ret->Create( sz, bmp->DisplayMode() ) );
    AknIconUtils::ScaleBitmapL( sz, ret, bmp );
    
    // Second, if it is screenshot, perform quality improvement scaling
    if ( aIsScreenshot )
        {
        ScaleBitmapL( bmp, ret );
        CleanupStack::Pop( bmp );
        }
    else
        {
        CleanupStack::PopAndDestroy( bmp );
        }
    
    CleanupStack::Pop( ret );
    return ret;
    }

// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::DuplicateBitmapL
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFastSwapAreaExtension::DuplicateBitmapLC( TInt aFbsHandle  )
    {
    CFbsBitmap* bmp = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( bmp );
    User::LeaveIfError( bmp->Duplicate( aFbsHandle ) );
    return bmp;
    }

// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::ScaleBitmapL
// --------------------------------------------------------------------------
//
void CTsFastSwapAreaExtension::ScaleBitmapL( CFbsBitmap* aSourceBitmap,
        CFbsBitmap* aTargetBitmap )
    {
    CTsBitmapScaleTask* rotaTask = new (ELeave) CTsBitmapScaleTask( *this );
    CleanupStack::PushL( rotaTask );
    iScaleTasks.AppendL( rotaTask );
    rotaTask->StartLD( aSourceBitmap, aTargetBitmap ); 
    // ownership of bmp transferred
    CleanupStack::Pop( rotaTask );
    }

// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::CancelScaleTasks
// --------------------------------------------------------------------------
//
void CTsFastSwapAreaExtension::CancelScaleTasks()
    {
    iScaleTasks.ResetAndDestroy();
    }


// --------------------------------------------------------------------------
// CTsFastSwapAreaExtension::ScaleFinished
// --------------------------------------------------------------------------
//
void CTsFastSwapAreaExtension::ScaleFinished( CTsBitmapScaleTask* iFinishedTask )
    {
    TInt task = iScaleTasks.Find( iFinishedTask );
    if ( task != KErrNotFound )
        {
        iScaleTasks.Remove( task );
        }
    iFastSwapArea.DrawDeferred();
    }

 // End of file
