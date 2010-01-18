/*
 * ============================================================================
 *  Name        : hgteleportfastswapgrid.cpp
 *  Part of     : Hg Teleport
 *  Description : Teleport Fast Swap area UI
 *  Version     : %version:  19 %
 *
 *  Copyright © 2009 Nokia.  All rights reserved.
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

#include <avkon.mbg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknsFrameBackgroundControlContext.h>
#include <touchfeedback.h>

#include "hgteleportfastswapgrid.h"

// TODO: Layout update
const TInt KCloseIconSize = 30;


 /* ================================================================================
  * CHgTeleportFastSwapGrid
  * ================================================================================
  */

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::CHgTeleportFastSwapGrid
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapGrid::CHgTeleportFastSwapGrid()
: CAknGrid(),
  iCloseIconHitIdx( KErrNotFound ),
  iBehaviour( ETouchOnly ),
  iHighlightVisible( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::~CHgTeleportFastSwapGrid
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapGrid::~CHgTeleportFastSwapGrid()
    {
    iCloseItems.Close();
    delete iBgContext;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::ConstructL( const CCoeControl* aParent )
    {
    CAknGrid::ConstructL( aParent, EAknListBoxSelectionGrid );
    SetPrimaryScrollingType(CAknGridView::EScrollFollowsItemsAndLoops);
    SetSecondaryScrollingType(CAknGridView::EScrollFollowsItemsAndLoops);
    iBgContext = CAknsFrameBackgroundControlContext::NewL(
               KAknsIIDQsnFrPopup,
               TRect(),
               TRect(),
               ETrue );
    iBgContext->SetCenter( KAknsIIDQsnFrPopupCenter );
    iVisibleViewRect = TRect( 0, 0, 0, 0 );
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    TBool eventHandled( EFalse );
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up &&
         iCloseIconHitIdx == KErrNotFound )
        {
        // Check if close icon has been hit
        // Find hit item
        TInt hitItem( KErrNotFound );
        for ( TInt i = 0; i < Model()->NumberOfItems(); i++ )
            {
            if ( GridView()->XYPosToItemIndex( aPointerEvent.iParentPosition, i ) )
                {
                hitItem = i;
                break;
                }
            }
        if ( hitItem != KErrNotFound )
            {
            // Item found, check if close icon has been hit
            TPoint itemPos = GridView()->ItemPos( hitItem );
            TSize itemSize = GridView()->ItemSize( hitItem );
            TRect itemRect( itemPos, itemSize );
            CHgTeleportGridItemDrawer* itemDrawer =
                static_cast<CHgTeleportGridItemDrawer*>( ItemDrawer() );
            TRect closeIconRect = itemDrawer->GetCloseIconRect( itemRect );
            if ( closeIconRect.Contains( aPointerEvent.iParentPosition ) )
                {
                // Close icon hit
                iCloseIconHitIdx = hitItem;
                eventHandled = ETrue;
                // Hide highlight to mark close icon
                HideHighlight();
                // Update current item and redraw grid
                SetCurrentItemIndex( hitItem );
                DrawNow();
                if ( iFastSwapGridObserver )
                    {
                    MTouchFeedback* feedback = MTouchFeedback::Instance();
                    feedback->InstantFeedback(this,
                                              ETouchFeedbackBasicButton, 
                                              ETouchFeedbackVibra, 
                                              aPointerEvent);
                    iFastSwapGridObserver->HandleCloseEventL( hitItem );
                    }
                ResetCloseHit();
                if ( GridBehaviour() == EHybrid )
                    {
                    ShowHighlight();
                    }
                else
                    {
                    Redraw();
                    }
                }
            }
        }
    
    if ( !eventHandled )
        {
        CAknGrid::HandlePointerEventL( aPointerEvent );
        Redraw();
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::HandleDeviceStateChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::HandleDeviceStateChanged( TChangeType aChangeType )
    {
    if ( aChangeType == ESkin )
        {
        LoadCloseIcon();
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CHgTeleportFastSwapGrid::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::HandleResourceChange( TInt aType )
    {
    if ( aType != KEikDynamicLayoutVariantSwitch )
        {
        CAknGrid::HandleResourceChange( aType );
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::SizeChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::SizeChanged()
    {
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::SetFastSwapGridObserver
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::SetFastSwapGridObserver( MHgTeleportFastSwapGridObserver* aObserver )
    {
    iFastSwapGridObserver = aObserver;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::CreateItemDrawerL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::CreateItemDrawerL()
    {
    TRect availableRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, availableRect);
    TAknLayoutRect fastSwapAreaPane;
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    fastSwapAreaPane.LayoutRect( availableRect,
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    const TInt leftOffset = fastSwapAreaPane.Rect().iTl.iX;
    const TInt rightOffset = availableRect.Width() - fastSwapAreaPane.Rect().iBr.iX;
    
    CFormattedCellGridData* data = CFormattedCellGridData::NewL();
    CleanupStack::PushL( data );
    CHgTeleportGridItemDrawer* itemDrawer =
        new ( ELeave ) CHgTeleportGridItemDrawer( this, data );
    CleanupStack::PushL( itemDrawer );
    itemDrawer->SetEdgeOffset( leftOffset, rightOffset );
    iItemDrawer = itemDrawer;
    CleanupStack::Pop( itemDrawer );
    CleanupStack::Pop( data );
    LoadCloseIcon();
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::SetCloseItemsL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::SetCloseItemsL( RArray<TInt>& aItemIndex )
    {
    iCloseItems.Close();
    for ( TInt i = 0; i < aItemIndex.Count(); i++ )
        {
        iCloseItems.AppendL( aItemIndex[i] );
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::CanCloseItem
// -----------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapGrid::CanCloseItem( TInt aItemIndex )
    {
    return iCloseItems.Find(aItemIndex) != KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::ItemCloseHit
// -----------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapGrid::IsItemCloseHit( TInt aItemIndex )
    {
    return iCloseIconHitIdx == aItemIndex;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::ResetCloseHit
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::ResetCloseHit()
    {
    iCloseIconHitIdx = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::SetBehaviour
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::SetGridBehaviour( TFastSwapGridBehaviour aBehaviour )
    {
    iBehaviour = aBehaviour;
    switch ( iBehaviour )
        {
        case ETouchOnly:
            {
            HideHighlight();
            }
            break;
        default:
            {
            ShowHighlight();
            }
            break;
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::GridBehaviour
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapGrid::TFastSwapGridBehaviour CHgTeleportFastSwapGrid::GridBehaviour()
    {
    return iBehaviour;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::ShowHighlight
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::ShowHighlight()
    {
    if ( !iHighlightVisible )
        {
        // Draw highlight
        iHighlightVisible = ETrue;
        Redraw();
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::HideHighlight
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::HideHighlight()
    {
    if ( iHighlightVisible )
        {
        iHighlightVisible = EFalse;
        Redraw();
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::IsHighlightVisible
// -----------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapGrid::IsHighlightVisible()
    {
    return iHighlightVisible;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::SetVisibleViewRect
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::SetVisibleViewRect( const TRect aRect )
    {
    iVisibleViewRect = aRect;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::VisibleViewRect
// -----------------------------------------------------------------------------
//
TRect CHgTeleportFastSwapGrid::VisibleViewRect()
    {
    TRect retVal;
    TRect emptyRect = TRect( 0, 0, 0, 0 );
    if ( iVisibleViewRect == emptyRect )
        {
        retVal = Rect();
        }
    else
        {
        retVal = iVisibleViewRect;
        }
    return retVal;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::LoadCloseIconL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::LoadCloseIcon()
    {
    // Load and set close icon
    CFbsBitmap* icon = NULL;
    CFbsBitmap* mask = NULL;

    TRAP_IGNORE(AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiItutListCollapse,
                icon,
                mask,
                KAvkonBitmapFile,
                EMbmAvkonQgn_indi_button_preview_close,
                EMbmAvkonQgn_indi_button_preview_close_mask
                );
                CleanupStack::Pop( 2 ); // codescanner::cleanup
                );

    // TODO: Layout update
    AknIconUtils::SetSize( icon, TSize( KCloseIconSize, KCloseIconSize ), EAspectRatioPreserved );
    AknIconUtils::SetSize( mask, TSize( KCloseIconSize, KCloseIconSize ), EAspectRatioPreserved );

    static_cast<CHgTeleportGridItemDrawer*>(iItemDrawer)->SetCloseIcon( icon, mask );
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapGrid::Redraw
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapGrid::Redraw()
    {
    DrawDeferred();
    CCoeControl* ctrlToRedraw = Parent();
    if ( ctrlToRedraw )
        {
        ctrlToRedraw->DrawDeferred();
        ctrlToRedraw = ctrlToRedraw->Parent();
        if ( ctrlToRedraw )
            {
            ctrlToRedraw->DrawNow();
            }
        }
    }


/* ================================================================================
 * CHgTeleportGridItemDrawer
 * ================================================================================
 */

// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::CHgTeleportGridItemDrawer
// -----------------------------------------------------------------------------
//
CHgTeleportGridItemDrawer::CHgTeleportGridItemDrawer(
        CHgTeleportFastSwapGrid* aGrid,
        CFormattedCellListBoxData* aData )
: CFormattedCellListBoxItemDrawer( aGrid->Model(),
        NULL,
        aData ),
  iGrid( aGrid )
    {
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, iScreenRect);
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::~CHgTeleportGridItemDrawer
// -----------------------------------------------------------------------------
//
CHgTeleportGridItemDrawer::~CHgTeleportGridItemDrawer()
    {
    delete iCloseIcon;
    delete iCloseIconMask;
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::SetCloseIcon
// -----------------------------------------------------------------------------
//
void CHgTeleportGridItemDrawer::SetCloseIcon( CFbsBitmap* aBmp, CFbsBitmap* aMask )
    {
    delete iCloseIcon;
    iCloseIcon = aBmp;
    delete iCloseIconMask;
    iCloseIconMask = aMask;
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::GetCloseIconRect
// -----------------------------------------------------------------------------
//
TRect CHgTeleportGridItemDrawer::GetCloseIconRect( const TRect& aItemRect ) const
    {
    // TODO: layout update
    const TInt KRightMargin = 7;
    const TInt KTopMargin = 34;
    TRect retVal( TPoint(aItemRect.iBr.iX - KCloseIconSize - KRightMargin,
                         aItemRect.iTl.iY + KTopMargin),
                         TSize( KCloseIconSize, KCloseIconSize ) );
    return retVal;
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::SetEdgeOffset
// -----------------------------------------------------------------------------
//
void CHgTeleportGridItemDrawer::SetEdgeOffset( TInt aLeftOffset, TInt aRightOffset )
    {
    iLeftOffset = aLeftOffset;
    iRightOffset = aRightOffset;
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
//
 void CHgTeleportGridItemDrawer::DrawActualItem( TInt aItemIndex, const TRect& aActualItemRect,
                                                 TBool aItemIsCurrent, TBool aViewIsEmphasized,
                                                 TBool aViewIsDimmed, TBool aItemIsSelected ) const
    {
    if ( IsItemRectVisible( aActualItemRect ) )
        {
        // Calculate offset of the visible rectangle
        TRect drawRect = aActualItemRect;
        if ( drawRect.iTl.iX < iLeftOffset )
            {
            drawRect.iTl.iX = iLeftOffset;
            }
        if ( iScreenRect.Width() - drawRect.iBr.iX < iRightOffset )
            {
            drawRect.iBr.iX = iScreenRect.Width() - iRightOffset;
            }
        iGc->SetClippingRect(drawRect);
        
        // Check for item highlight
        TBool itemIsCurrent = !iGrid->IsHighlightVisible() ? EFalse : aItemIsCurrent;
        
        // Draw item
        CFormattedCellListBoxItemDrawer::DrawActualItem(aItemIndex, aActualItemRect,
                itemIsCurrent, aViewIsEmphasized, aViewIsDimmed, aItemIsSelected );
        
        if ( iGrid->CanCloseItem( aItemIndex ) && iCloseIcon && iCloseIconMask )
            {
            TRect closeIconRect = GetCloseIconRect( aActualItemRect );
            // Draw frame
            // TODO: layout update
            const TInt KFrameGrow = 5;
            TRect innerRect = closeIconRect;
            TRect outerRect = innerRect;
            outerRect.Grow(KFrameGrow, KFrameGrow);
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            if ( iGrid->IsItemCloseHit( aItemIndex ) )
                {
                AknsDrawUtils::DrawFrame(skin, *iGc, outerRect, innerRect,
                        KAknsIIDQgnFrSctrlButtonPressed, KAknsIIDQgnFrSctrlButtonCenterPressed);
                }
            else
                {
                AknsDrawUtils::DrawFrame(skin, *iGc, outerRect, innerRect,
                        KAknsIIDQgnFrSctrlButton, KAknsIIDQgnFrSctrlButtonCenter);
                }
            // Draw close icon
            TRect sourceRect( TPoint(0,0), iCloseIcon->SizeInPixels() );
            iGc->DrawBitmapMasked( closeIconRect, iCloseIcon, sourceRect, iCloseIconMask, ETrue );
            }
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
//
TBool CHgTeleportGridItemDrawer::IsItemRectVisible( const TRect& aItemRect ) const
    {
    TBool retVal( EFalse );
    TRect viewRect = iGrid->VisibleViewRect();
    if ( // left edge of item rectangle on screen
         ( aItemRect.iTl.iX >= viewRect.iTl.iX && aItemRect.iTl.iX <= viewRect.iBr.iX ) ||
         // right edge of item rectangle on screen
         ( aItemRect.iBr.iX >= viewRect.iTl.iX && aItemRect.iBr.iX <= viewRect.iBr.iX )
        )
        {
        retVal = ETrue;
        }
    return retVal;
    }
 
 
 
 /* ================================================================================
  * CHgTeleportGridHighlightTimer
  * ================================================================================
  */

// -----------------------------------------------------------------------------
// CHgTeleportGridHighlightTimer::CHgTeleportGridHighlightTimer
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapTimer::CHgTeleportFastSwapTimer( MHgTeleportFastSwapTimerObserver& aObserver )
: CTimer( EPriorityStandard ),
  iObserver( &aObserver )
    {
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridHighlightTimer::CHgTeleportGridHighlightTimer
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapTimer::~CHgTeleportFastSwapTimer()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridHighlightTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CHgTeleportGridHighlightTimer::CHgTeleportGridHighlightTimer
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapTimer::RunL()
    {
    iObserver->TimerCompletedL(this);
    }
 
 // End of file
