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
#include "tsapplogging.h"

 /* ================================================================================
  * CTsFastSwapGrid
  * ================================================================================
  */

const TInt KCloseIconRedrawTime = 300000; // 0.3 second

const TInt KStrokeThickness = 1;

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::CTsFastSwapGrid
// -----------------------------------------------------------------------------
//
CTsFastSwapGrid::CTsFastSwapGrid()
: CAknGrid(),
  iCloseIconHitIdx( KErrNotFound ),
  iBehaviour( ETouchOnly ),
  iHighlightVisible( EFalse ),
  iAknEventHandlingEnabled(ETrue)
    {
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::~CTsFastSwapGrid
// -----------------------------------------------------------------------------
//
CTsFastSwapGrid::~CTsFastSwapGrid()
    {
    iCloseItems.Close();
    delete iCloseIconRedrawTimer;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::ConstructL( const CCoeControl* aParent )
    {
    TSLOG_CONTEXT( CTsFastSwapGrid::ConstructL, TSLOG_LOCAL );
    TSLOG_IN();
    
    iParent = aParent;
    CAknGrid::ConstructL( aParent, EAknListBoxSelectionGrid );
    SetPrimaryScrollingType(CAknGridView::EScrollFollowsItemsAndLoops);
    SetSecondaryScrollingType(CAknGridView::EScrollFollowsItemsAndLoops);
    iCloseIconRedrawTimer = new (ELeave) CTsFastSwapTimer( *this );
    iCloseIconRedrawTimer->ConstructL();
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    TSLOG_CONTEXT( CTsFastSwapGrid::HandlePointerEventL, TSLOG_LOCAL );
    TSLOG_IN();
    
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
        if ( hitItem != KErrNotFound && CanCloseItem( hitItem ) )
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
                TInt hitDataIdx(hitItem);
                if ( AknLayoutUtils::LayoutMirrored() )
                    {
                    // Calculate logical item index
                    hitDataIdx = Model()->ItemTextArray()->MdcaCount() - 1 - hitItem;
                    }
                iCloseIconHitIdx = hitDataIdx;
                eventHandled = ETrue;
                
                if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                    {
                    // Update current item and redraw grid
                    SetCurrentItemIndex( hitItem );
                    DrawNow();
                    
                    iCloseIconRedrawTimer->Cancel();
                    iCloseIconRedrawTimer->After(KCloseIconRedrawTime);
                    }
                else
                    {
                    // Pointer up
                    if ( iFastSwapGridObserver )
                        {
                        MTouchFeedback* feedback = MTouchFeedback::Instance();
                        feedback->InstantFeedback(this,
                                                  ETouchFeedbackBasicButton, 
                                                  ETouchFeedbackVibra, 
                                                  aPointerEvent);
                        }
                    if ( GridBehaviour() == EHybrid )
                        {
                        ShowHighlight();
                        }
                    else
                        {
                        Redraw();
                        }
                    iFastSwapGridObserver->HandleCloseEventL( hitDataIdx );
                    ResetCloseHit();
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
        if ( iAknEventHandlingEnabled )
            {
            CAknGrid::HandlePointerEventL( aPointerEvent );
            }
        Redraw();
        }
    
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::HandleDeviceStateChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HandleDeviceStateChanged( TChangeType aChangeType )
    {
    TSLOG_CONTEXT( CTsFastSwapGrid::HandleDeviceStateChanged, TSLOG_LOCAL );
    TSLOG_IN();
    
    if ( aChangeType == ESkin )
        {
        LoadCloseIconAndStrokeParams();
        }
    
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::HandleResourceChange( TInt aType )
    {
    TSLOG_CONTEXT( CTsFastSwapGrid::HandleResourceChange, TSLOG_LOCAL );
    TSLOG_IN();
    
    if ( aType != KEikDynamicLayoutVariantSwitch &&
         Model()->ItemTextArray()->MdcaCount() )
        {
        CAknGrid::HandleResourceChange( aType );
        }
    
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SizeChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SizeChanged()
    {
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::Draw
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::Draw( const TRect& aRect ) const
    {
    CAknGrid::Draw(aRect);
    if ( !Model()->ItemTextArray()->MdcaCount() )
        {
        GridView()->DrawEmptyList();
        }
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
    TSLOG_CONTEXT( CTsFastSwapGrid::CreateItemDrawerL, TSLOG_LOCAL );
    TSLOG_IN();
    
    CFormattedCellGridData* data = CFormattedCellGridData::NewL();
    CleanupStack::PushL( data );
    CTsGridItemDrawer* itemDrawer =
        new ( ELeave ) CTsGridItemDrawer( this, data );
    iItemDrawer = itemDrawer;
    CleanupStack::Pop( data );
    
    TSLOG_OUT();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::UpdateItemDrawerLayoutDataL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::UpdateItemDrawerLayoutDataL()
    {
    CTsGridItemDrawer* itemDrawer =
        static_cast<CTsGridItemDrawer*>( ItemDrawer() );
    
    TPixelsAndRotation screenSize;
    iEikonEnv->ScreenDevice()->GetDefaultScreenSizeAndRotation(screenSize);
    TRect availableRect = TRect( TPoint(0,0), screenSize.iPixelSize );
    itemDrawer->SetScreenRect(availableRect);
    
    TAknLayoutRect fastSwapAreaPane;
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    fastSwapAreaPane.LayoutRect( availableRect,
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    const TInt leftOffset = iParent->Rect().iTl.iX;
    const TInt rightOffset = availableRect.Width() - iParent->Rect().iBr.iX;
    SetVisibleViewRect(fastSwapAreaPane.Rect());
    itemDrawer->SetEdgeOffset( leftOffset, rightOffset );
    
    LoadCloseIconAndStrokeParams();
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::EnableAknEventHandling
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::EnableAknEventHandling( TBool aEnable )
    {
    iAknEventHandlingEnabled = aEnable;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapGrid::TimerCompletedL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::TimerCompletedL( CTsFastSwapTimer* aSource )
    {
    if ( aSource == iCloseIconRedrawTimer )
        {
        ResetCloseHit();
        }
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
// CTsFastSwapGrid::SetStrokeColor
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetStrokeColors( TRgb aColor,
                                       TRgb aHighlightedColor )
    {
    static_cast<CTsGridItemDrawer*>(iItemDrawer)->SetStrokeColors(aColor, aHighlightedColor);
    }


// -----------------------------------------------------------------------------
// CTsFastSwapGrid::SetStrokeItemsL
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::SetStrokeItemsL( RArray<TInt>& aItemIndex )
    {
    static_cast<CTsGridItemDrawer*>(iItemDrawer)->SetStrokeItemsL(aItemIndex);
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
// CTsFastSwapGrid::LoadCloseIconAndStrokeParams
// -----------------------------------------------------------------------------
//
void CTsFastSwapGrid::LoadCloseIconAndStrokeParams()
    {
    // Load and set close icon
    CFbsBitmap* icon = NULL;
    CFbsBitmap* iconMask = NULL;
    CFbsBitmap* iconPressed = NULL;
    CFbsBitmap* iconPressedMask = NULL;
    
    TRAP_IGNORE(AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiTsButtonClose,
                icon,
                iconMask,
                KAvkonBitmapFile,
                EMbmAvkonQgn_indi_button_preview_close,
                EMbmAvkonQgn_indi_button_preview_close_mask
                ));
    
    TRAP_IGNORE(AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiTsButtonClosePressed,
                iconPressed,
                iconPressedMask,
                KAvkonBitmapFile,
                EMbmAvkonQgn_indi_button_preview_close,
                EMbmAvkonQgn_indi_button_preview_close_mask
                ));

    TAknLayoutRect gridItem;
    TAknLayoutRect gridImage;
    TAknLayoutRect gridCloseButton;
    TAknLayoutRect gridCloseIcon;
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    TRect source = Rect();
    gridItem.LayoutRect( source,
            AknLayoutScalable_Apps::cell_tport_appsw_pane( variety, 0, 0 ) );
    gridImage.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g1( variety ) ); 
    gridCloseButton.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::bg_button_pane_cp16( variety, 0, 0 ));
    gridCloseIcon.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g3( variety, 0, 0 ));
    
    // Set icon size
    AknIconUtils::SetSize( icon, gridCloseIcon.Rect().Size(), EAspectRatioPreserved );
    AknIconUtils::SetSize( iconPressed, gridCloseIcon.Rect().Size(), EAspectRatioPreserved );
    CTsGridItemDrawer* itemDrawer =
        static_cast<CTsGridItemDrawer*>(iItemDrawer);
    
    // Setup close parameters
    itemDrawer->SetCloseIcon( icon, iconMask, iconPressed, iconPressedMask );
    TRect relGridCloseButton = TRect( TPoint( gridCloseButton.Rect().iTl.iX - gridItem.Rect().iTl.iX,
                                              gridCloseButton.Rect().iTl.iY - gridItem.Rect().iTl.iY),
                                      gridCloseButton.Rect().Size() );
    TRect relGridCloseIconRect = TRect( TPoint( gridCloseIcon.Rect().iTl.iX - gridItem.Rect().iTl.iX,
                                                gridCloseIcon.Rect().iTl.iY - gridItem.Rect().iTl.iY),
                                        gridCloseIcon.Rect().Size() );
    itemDrawer->SetCloseIconRect( relGridCloseButton, relGridCloseIconRect );
    
    // Setup stroke parameters
    TPoint strokeOffset;
    strokeOffset.iX = gridImage.Rect().iTl.iX - gridItem.Rect().iTl.iX - KStrokeThickness;
    strokeOffset.iY = gridImage.Rect().iTl.iY - gridItem.Rect().iTl.iY - KStrokeThickness;
    TSize strokeSize = gridImage.Rect().Size();
    strokeSize.iHeight += KStrokeThickness * 2;
    strokeSize.iWidth += KStrokeThickness * 2;
    itemDrawer->SetStrokeOffset( strokeOffset, strokeSize );
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
  iGrid( aGrid ),
  iStrokeColor( KRgbBlack ),
  iHighlightStrokeColor( KRgbBlack )
    {
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::~CTsGridItemDrawer
// -----------------------------------------------------------------------------
//
CTsGridItemDrawer::~CTsGridItemDrawer()
    {
    delete iCloseIcon;
    delete iCloseIconMask;
    delete iCloseIconPressed;
    delete iCloseIconPressedMask;
    iStrokeItems.Close();
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetCloseIcon
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetCloseIcon( CFbsBitmap* aBmp, CFbsBitmap* aBmpMask,
                                      CFbsBitmap* aBmpPressed, CFbsBitmap* aBmpPressedMask  )
    {
    delete iCloseIcon;
    iCloseIcon = aBmp;
    delete iCloseIconMask;
    iCloseIconMask = aBmpMask;
    delete iCloseIconPressed;
    iCloseIconPressed = aBmpPressed;
    delete iCloseIconPressedMask;
    iCloseIconPressedMask = aBmpPressedMask;
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
// CTsGridItemDrawer::SetRedrawBackground
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetRedrawBackground( TBool aEnable )
    {
    iRedrawBackground = aEnable;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetStrokeColor
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetStrokeColors( TRgb aColor,
                                         TRgb aHighlightedColor )
    {
    iStrokeColor = aColor;
    iHighlightStrokeColor = aHighlightedColor;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetStrokeItemsL
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetStrokeItemsL( RArray<TInt>& aItemIndex )
    {
    iStrokeItems.Close();
    for ( TInt i = 0; i < aItemIndex.Count(); i++ )
        {
        iStrokeItems.AppendL( aItemIndex[i] );
        }
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetStrokeOffset
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetStrokeOffset( TPoint aStrokeOffset, TSize aStrokeSize )
    {
    iStrokeRect = TRect( aStrokeOffset, aStrokeSize );
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::SetScreenRect
// -----------------------------------------------------------------------------
//
void CTsGridItemDrawer::SetScreenRect( TRect aRect )
    {
    iScreenRect = aRect;
    }


// -----------------------------------------------------------------------------
// CTsGridItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
//
 void CTsGridItemDrawer::DrawActualItem( TInt aItemIndex, const TRect& aActualItemRect,
                                                 TBool aItemIsCurrent, TBool aViewIsEmphasized,
                                                 TBool aViewIsDimmed, TBool aItemIsSelected ) const
    {
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
        
        // Draw stroke
        if ( iStrokeItems.Find( aItemIndex ) != KErrNotFound )
            {
            TRect strokeRect = iStrokeRect;
            strokeRect.Move( aActualItemRect.iTl );
            if ( itemIsCurrent )
                {
                iGc->SetPenColor( iHighlightStrokeColor );
                }
            else
                {
                iGc->SetPenColor( iStrokeColor );
                }
            // Thumbnail stroke
            iGc->DrawRect( strokeRect );
            }
        
        // Draw close button
        if ( iGrid->CanCloseItem( aItemIndex ) && iCloseIcon && iCloseIconPressed )
            {
            TRect closeIconRect = GetCloseButtonRect( aActualItemRect );
            // Draw frame
            TRect outerRect = closeIconRect;
            TRect innerRect = CalculateInnerButtonRect( outerRect );
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            if ( iGrid->IsItemCloseHit( aItemIndex ) )
                {
                TRect sourceRect( TPoint(0,0), iCloseIconPressed->SizeInPixels() );
                iGc->DrawBitmapMasked( innerRect,
                        iCloseIconPressed,
                        sourceRect,
                        iCloseIconPressedMask,
                        ETrue );
                }
            else
                {
                TRect sourceRect( TPoint(0,0), iCloseIcon->SizeInPixels() );
                iGc->DrawBitmapMasked( innerRect,
                        iCloseIcon,
                        sourceRect,
                        iCloseIconMask,
                        ETrue );
                }
            }
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
    if ( viewRect.Intersects( aItemRect ) )
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
  
 // End of file
