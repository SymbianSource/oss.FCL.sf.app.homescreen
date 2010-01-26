/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <avkon.mbg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknsFrameBackgroundControlContext.h>
#include <touchfeedback.h>

#include "tsfastswapgrid.h"

 /* ================================================================================
  * CTsFastSwapGrid
  * ================================================================================
  */

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::CTsFastSwapGrid
// -----------------------------------------------------------------------------
//
CTsFastSwapGrid::CTsFastSwapGrid()
: CAknGrid(),
  iCloseIconHitIdx( KErrNotFound ),
  iBehaviour( ETouchOnly ),
  iHighlightVisible( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::~CTsFastSwapGrid
// -----------------------------------------------------------------------------
//
CTsFastSwapGrid::~CTsFastSwapGrid()
    {
    iCloseItems.Close();
    delete iBgContext;
    iFullyVisibleItems.Close();
    iPartialVisibleItems.Close();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::ConstructL( const CCoeControl* aParent )
    {
    iParent = aParent;
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
// CTsFastSwapGrid::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    TBool eventHandled( EFalse );
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up ||
         aPointerEvent.iType == TPointerEvent::EButton1Down )
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
            CTsGridItemDrawer* itemDrawer =
                static_cast<CTsGridItemDrawer*>( ItemDrawer() );
            TRect closeIconRect = itemDrawer->GetCloseButtonRect( itemRect );
            if ( closeIconRect.Contains( aPointerEvent.iParentPosition ) )
                {
                // Close icon hit
                if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                    {
                    // pointer down - finish processing but do not forward to grid
                    eventHandled = ETrue;
                    }
                else
                    {
                    // Pointer up
                    TInt hitDataIdx(hitItem);
                    if ( AknLayoutUtils::LayoutMirrored() )
                        {
                        // Calculate logical item index
                        hitDataIdx = Model()->ItemTextArray()->MdcaCount() - 1 - hitItem;
                        }
                    iCloseIconHitIdx = hitDataIdx;
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
                        iFastSwapGridObserver->HandleCloseEventL( hitDataIdx );
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
        }
    
    if ( !eventHandled )
        {
        CTsGridItemDrawer* itemDrawer =
            static_cast<CTsGridItemDrawer*>( ItemDrawer() );
        if( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            itemDrawer->SetRedrawBackground( ETrue );
            }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            itemDrawer->SetRedrawBackground( EFalse );
            }
        CAknGrid::HandlePointerEventL( aPointerEvent );
        Redraw();
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::HandleDeviceStateChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HandleDeviceStateChanged( TChangeType aChangeType )
    {
    if ( aChangeType == ESkin )
        {
        LoadCloseIcon();
        }
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CTsFastSwapGrid::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HandleResourceChange( TInt aType )
    {
    if ( aType != KEikDynamicLayoutVariantSwitch )
        {
        CAknGrid::HandleResourceChange( aType );
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SizeChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SizeChanged()
    {
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SetFastSwapGridObserver
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetFastSwapGridObserver( MTsFastSwapGridObserver* aObserver )
    {
    iFastSwapGridObserver = aObserver;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::CreateItemDrawerL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::CreateItemDrawerL()
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
    CTsGridItemDrawer* itemDrawer =
        new ( ELeave ) CTsGridItemDrawer( this, data );
    CleanupStack::PushL( itemDrawer );
    itemDrawer->SetEdgeOffset( leftOffset, rightOffset );
    iItemDrawer = itemDrawer;
    CleanupStack::Pop( itemDrawer );
    CleanupStack::Pop( data );
    LoadCloseIcon();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SetCloseItemsL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetCloseItemsL( RArray<TInt>& aItemIndex )
    {
    iCloseItems.Close();
    for ( TInt i = 0; i < aItemIndex.Count(); i++ )
        {
        iCloseItems.AppendL( aItemIndex[i] );
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::CanCloseItem
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapGrid::CanCloseItem( TInt aItemIndex )
    {
    return iCloseItems.Find(aItemIndex) != KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::ItemCloseHit
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapGrid::IsItemCloseHit( TInt aItemIndex )
    {
    return iCloseIconHitIdx == aItemIndex;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::ResetCloseHit
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::ResetCloseHit()
    {
    iCloseIconHitIdx = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SetBehaviour
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetGridBehaviour( TFastSwapGridBehaviour aBehaviour )
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
// CTsFastSwapGrid::GridBehaviour
// -----------------------------------------------------------------------------
//
CTsFastSwapGrid::TFastSwapGridBehaviour CTsFastSwapGrid::GridBehaviour()
    {
    return iBehaviour;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::ShowHighlight
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::ShowHighlight()
    {
    if ( !iHighlightVisible )
        {
        // Draw highlight
        iHighlightVisible = ETrue;
        Redraw();
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::HideHighlight
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HideHighlight()
    {
    if ( iHighlightVisible )
        {
        iHighlightVisible = EFalse;
        Redraw();
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::IsHighlightVisible
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapGrid::IsHighlightVisible()
    {
    return iHighlightVisible;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SetVisibleViewRect
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetVisibleViewRect( const TRect aRect )
    {
    iVisibleViewRect = aRect;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::VisibleViewRect
// -----------------------------------------------------------------------------
//
TRect CTsFastSwapGrid::VisibleViewRect()
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
// CTsFastSwapGrid::LoadCloseIconL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::LoadCloseIcon()
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

    TAknLayoutRect gridAppPane;
    TAknLayoutRect gridItem;
    TAknLayoutRect gridCloseButton;
    TAknLayoutRect gridCloseIcon;
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TRect source = iParent ? iParent->Rect() : Rect();
    gridAppPane.LayoutRect( source,
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    gridItem.LayoutRect( gridAppPane.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane( variety, 0, 0 ) );
    gridCloseButton.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::bg_button_pane_cp16( variety, 0, 0 ));
    gridCloseIcon.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g3( variety, 0, 0 ));
    AknIconUtils::SetSize( icon, gridCloseIcon.Rect().Size(), EAspectRatioPreserved );
    AknIconUtils::SetSize( mask, gridCloseIcon.Rect().Size(), EAspectRatioPreserved );
    CTsGridItemDrawer* itemDrawer =
        static_cast<CTsGridItemDrawer*>(iItemDrawer);
    itemDrawer->SetCloseIcon( icon, mask );
    TRect relGridCloseButton = TRect( TPoint( gridCloseButton.Rect().iTl.iX - gridItem.Rect().iTl.iX,
                                              gridCloseButton.Rect().iTl.iY - gridItem.Rect().iTl.iY),
                                      gridCloseButton.Rect().Size() );
    TRect relGridCloseIconRect = TRect( TPoint( gridCloseIcon.Rect().iTl.iX - gridItem.Rect().iTl.iX,
                                                gridCloseIcon.Rect().iTl.iY - gridItem.Rect().iTl.iY),
                                        gridCloseIcon.Rect().Size() );
    itemDrawer->SetCloseIconRect( relGridCloseButton, relGridCloseIconRect );
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::Redraw
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::Redraw()
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

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::LaunchTactileFeedback
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::LaunchTactileFeedback()
    {
    if( !iTactileFeedbackSupport )
        {
        return;
        }
    
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        feedback->InstantFeedback(ETouchFeedbackSensitive);
        }
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SetTactileFeedbackSupport
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetTactileFeedbackSupport(TBool aSupport)
    {
    iTactileFeedbackSupport = aSupport;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::RemoveFromVisibleItems
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapGrid::RemoveFromVisibleItems(TInt aItem) const
    {
    TBool retVal(EFalse);
    TInt idx(0);
    idx = iFullyVisibleItems.Find(aItem); 
    if(  idx >= 0)
        {
        iFullyVisibleItems.Remove(idx);
        retVal = ETrue;
        }
    idx = iPartialVisibleItems.Find( aItem );
    if( idx >= 0)
        {
        iPartialVisibleItems.Remove(idx);
        retVal = ETrue;
        }
    return retVal;    
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::AddToFullyVisibleItems
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapGrid::AddToFullyVisibleItems( TInt aItem ) const
    {
    TBool retVal(EFalse);
    TInt idx(0);

    idx = iPartialVisibleItems.Find(aItem); 
    if(  idx >= 0)
        {
        iPartialVisibleItems.Remove(idx);
        }
    idx = iFullyVisibleItems.Find( aItem );
    if( idx == KErrNotFound )
        {
        iFullyVisibleItems.Append(aItem);
        retVal = ETrue; 
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::MoveToPartialVisibleItems
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapGrid::MoveToPartialVisibleItems( TInt aItem ) const
    {
    TInt idx(0);
    idx = iFullyVisibleItems.Find(aItem); 
    if(  idx >= 0)
        {
        iFullyVisibleItems.Remove(idx);
        }
    idx = iPartialVisibleItems.Find( aItem );
    if( idx == KErrNotFound )
        {
        iPartialVisibleItems.Append(aItem);
        }
    return EFalse;
    }

/* ================================================================================
 * CTsGridItemDrawer
 * ================================================================================
 */

// -----------------------------------------------------------------------------
// CTsGridItemDrawer::CTsGridItemDrawer
// -----------------------------------------------------------------------------
//
CTsGridItemDrawer::CTsGridItemDrawer(
        CTsFastSwapGrid* aGrid,
        CFormattedCellListBoxData* aData )
: CFormattedCellListBoxItemDrawer( aGrid->Model(),
        NULL,
        aData ),
  iGrid( aGrid )
    {
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, iScreenRect);
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::~CTsGridItemDrawer
// -----------------------------------------------------------------------------
//
CTsGridItemDrawer::~CTsGridItemDrawer()
    {
    delete iCloseIcon;
    delete iCloseIconMask;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetCloseIcon
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetCloseIcon( CFbsBitmap* aBmp, CFbsBitmap* aMask )
    {
    delete iCloseIcon;
    iCloseIcon = aBmp;
    delete iCloseIconMask;
    iCloseIconMask = aMask;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetCloseIconRect
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetCloseIconRect( const TRect& aButtonRect, const TRect& aIconRect )
    {
    iCloseButtonRect = aButtonRect;
    iCloseIconRect = aIconRect;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::GetCloseButtonRect
// -----------------------------------------------------------------------------
//
TRect CTsGridItemDrawer::GetCloseButtonRect( const TRect& aItemRect ) const
    {
    TRect retVal;
    retVal = TRect( TPoint(aItemRect.iTl.iX + iCloseButtonRect.iTl.iX,
                           aItemRect.iTl.iY + iCloseButtonRect.iTl.iY),
                    iCloseButtonRect.Size() );
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetEdgeOffset
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetEdgeOffset( TInt aLeftOffset, TInt aRightOffset )
    {
    iLeftOffset = aLeftOffset;
    iRightOffset = aRightOffset;
    }


// -----------------------------------------------------------------------------
// CTsTeleportGridItemDrawer::SetRedrawBackground
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetRedrawBackground( TBool aEnable )
    {
    iRedrawBackground = aEnable;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
//
 void CTsGridItemDrawer::DrawActualItem( TInt aItemIndex, const TRect& aActualItemRect,
                                                 TBool aItemIsCurrent, TBool aViewIsEmphasized,
                                                 TBool aViewIsDimmed, TBool aItemIsSelected ) const
    {
    TBool feedbackNeed(EFalse);//feedback when item disappear or fully appear
    
    if (IsItemRectVisible(aActualItemRect))
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
                
		if(IsRectContained( aActualItemRect, drawRect ))
            {
            feedbackNeed = iGrid->AddToFullyVisibleItems( aItemIndex );
            }
        else
            {
            feedbackNeed = iGrid->MoveToPartialVisibleItems( aItemIndex );
            }
			
        // Check for item highlight
        TBool itemIsCurrent = !iGrid->IsHighlightVisible() ? EFalse : aItemIsCurrent;
        
        // Draw background if necessary
        if ( iRedrawBackground )
            {
            CCoeControl* targetCtrlCtx = iGrid->Parent();
            if(targetCtrlCtx)
                {
                MAknsSkinInstance* skin = AknsUtils::SkinInstance();
                MAknsControlContext* cc = AknsDrawUtils::ControlContext( targetCtrlCtx );
                AknsDrawUtils::Background( skin,
                       cc,
                       targetCtrlCtx,
                       *iGc,
                       drawRect );
                }
            }
        
        // Draw item
        CFormattedCellListBoxItemDrawer::DrawActualItem(aItemIndex, aActualItemRect,
                itemIsCurrent, aViewIsEmphasized, aViewIsDimmed, aItemIsSelected );
        
        if ( iGrid->CanCloseItem( aItemIndex ) && iCloseIcon && iCloseIconMask )
            {
            TRect closeIconRect = GetCloseButtonRect( aActualItemRect );
            // Draw frame
            TRect outerRect = closeIconRect;
            TRect innerRect = CalculateInnerButtonRect( outerRect );
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
            iGc->DrawBitmapMasked( innerRect, iCloseIcon, sourceRect, iCloseIconMask, ETrue );
            }
        }
    else
        {
        feedbackNeed = iGrid->RemoveFromVisibleItems(aItemIndex);
        }
    if(feedbackNeed)
        {
        iGrid->LaunchTactileFeedback();
        }
    }

// -----------------------------------------------------------------------------
// CTsGridItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
//
TBool CTsGridItemDrawer::IsItemRectVisible( const TRect& aItemRect ) const
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

// -----------------------------------------------------------------------------
// CTsGridItemDrawer::IsRectContained
// -----------------------------------------------------------------------------
//
TBool CTsGridItemDrawer::IsRectContained(
        const TRect& aRectContained, const TRect& aRectContainig) const
    {
    TBool retVal(EFalse);
    TRect intersectionRect(aRectContainig);
    intersectionRect.Intersection( aRectContained );
    if( intersectionRect == aRectContained)
        {
        retVal = ETrue;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTsGridItemDrawer::CalculateInnerButtonRect
// -----------------------------------------------------------------------------
//
TRect CTsGridItemDrawer::CalculateInnerButtonRect( const TRect& aOuterRect ) const
    {
    TRect retVal(aOuterRect);
    retVal.iTl.iX += iCloseIconRect.iTl.iX - iCloseButtonRect.iTl.iX;
    retVal.iTl.iY += iCloseIconRect.iTl.iY - iCloseButtonRect.iTl.iY;
    retVal.iBr.iX -= iCloseButtonRect.iBr.iX - iCloseIconRect.iBr.iX;
    retVal.iBr.iY -= iCloseButtonRect.iBr.iY - iCloseIconRect.iBr.iY;
    return retVal;
    }
 
 
 
 /* ================================================================================
  * CTsGridHighlightTimer
  * ================================================================================
  */

// -----------------------------------------------------------------------------
// CTsGridHighlightTimer::CTsGridHighlightTimer
// -----------------------------------------------------------------------------
//
CTsFastSwapTimer::CTsFastSwapTimer( MTsFastSwapTimerObserver& aObserver )
: CTimer( EPriorityStandard ),
  iObserver( &aObserver )
    {
    }


// -----------------------------------------------------------------------------
// CTsGridHighlightTimer::CTsGridHighlightTimer
// -----------------------------------------------------------------------------
//
CTsFastSwapTimer::~CTsFastSwapTimer()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CTsGridHighlightTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFastSwapTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CTsGridHighlightTimer::CTsGridHighlightTimer
// -----------------------------------------------------------------------------
//
void CTsFastSwapTimer::RunL()
    {
    iObserver->TimerCompletedL(this);
    }
 
 // End of file
