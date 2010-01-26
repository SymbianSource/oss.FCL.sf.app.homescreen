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
*  Version     : %version: MM_71.1.17.1.49 % << Don't touch! Updated by Synergy at check-out.
*
*/


#include <e32keys.h>
#include <e32math.h>
#include <AknsDrawUtils.h>
#include <layoutmetadata.cdl.h>
#include <aknlongtapdetector.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#include <aknlistboxtfx.h>
#endif

#include "mmwidgetcontainer.h"
#include "mmlistboxmodel.h"
#include "mmmarqueeadapter.h"
#include "mmwidgetobserver.h"
#include "mmvisibilityobserver.h"
#include "mmlistboxcontainer.h"
#include "mmgridcontainer.h"
#include "mmlistboxitemdrawer.h"
#include "mmmarqueeadapter.h"
#include "mmfloatingitem.h"
#include "hnsuitemodel.h"
#include "hnitemsorder.h"
#include "menudebug.h"
#include "mmdraweranimator.h"
#include "hnglobals.h"
#include "mmpostevaluationprocessor.h"
#include "mmgrid.h"
#include "mmlongtapobserver.h"

class CMmTemplateLibrary;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TMmWidgetPosition::TMmWidgetPosition()
    : iVerticalItemOffset( 0 )
    , iTopItemIndex( KErrNotFound )
    , iValid( EFalse )
    , iLandscape( EFalse )
    , iHighlightedItemId( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CMmWidgetContainer* CMmWidgetContainer::NewGridContainerL( const TRect& aRect,
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aLibrary )
    {
    return CMmGridContainer::NewL( aRect, aObjectProvider, aLibrary );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CMmWidgetContainer* CMmWidgetContainer::NewListBoxContainerL( const TRect& aRect,
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary )
    {
    return CMmListBoxContainer::NewL( aRect, aObjectProvider, aTemplateLibrary );
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::ConstructL()
	{
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
    	KAknsIIDQsnBgAreaMainAppsGrid, Rect(), EFalse);
    iLongTapDetector = CAknLongTapDetector::NewL( this );
    iLongTapDetector->EnableLongTapAnimation(ETrue);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmWidgetContainer::CMmWidgetContainer()
    : iIsFaded( EFalse )
    , iHasFocus( ETrue )
    , iInForeground( ETrue )
    {
    iMarqueeAdapter = NULL;
    iBgContext = NULL;
    iWidgetPositionCache.iValid = EFalse;
    iLongTapInProgress = EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmWidgetContainer::~CMmWidgetContainer()
    {
    AknsUtils::DeregisterControlPosition( this );
    AknsUtils::DeregisterControlPosition( this->iWidget );
    delete iBgContext;
    delete iPostProcessor;
    delete iLongTapDetector;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::EnableLongTapAnimation( TBool aEnable )
    {
    if ( iLongTapDetector )
        {
        iLongTapDetector->EnableLongTapAnimation( aEnable );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SizeChanged()
    {
    if ( iWidget && iWidgetRect != Rect() )
        {
    	CMmTemplateLibrary * templateLibrary =
    	static_cast<CMmListBoxItemDrawer*> (
    			Widget()->View()->ItemDrawer() )->TemplateLibrary();
        if ( Rect() != templateLibrary->GetParentRect(
        		Layout_Meta_Data::IsLandscapeOrientation() ) )
        	{
        	templateLibrary->CleanAndClearCache();
        	}
        iWidget->View()->SetViewRect( Rect() );
        iWidget->SetRect( Rect() );
        iWidgetRect = Rect();
        }

    if ( iBgContext )
        {
        TRect rect = iCoeEnv->ScreenDevice()->SizeInPixels();
        iBgContext->SetRect( rect );
        SetItemDrawerAndViewBgContext( iBgContext );
        }

    AknsUtils::RegisterControlPosition( this );
    AknsUtils::RegisterControlPosition( this->iWidget );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::Draw( const TRect& /*aRect*/ ) const
    {
    // do nothing
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CMmWidgetContainer::MopSupplyObject(TTypeUid aId)
    {
    if (aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        return MAknsControlContext::SupplyMopObject(aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject(aId);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleButtonDownL(const TPointerEvent& aPointerEvent )
    {
    iTapPoint = aPointerEvent.iPosition;
    iLastDragPoint = aPointerEvent.iPosition;
    iLastDragHighlight = GetHighlight();
    if( !iWidget->View()->XYPosToItemIndex(
    		aPointerEvent.iPosition, iDraggedIndex ) )
        {
        iDraggedIndex = KErrNotFound;
        }
    iItemRelativeTapPoint = aPointerEvent.iPosition - iWidget->View()->ItemPos(
    		iDraggedIndex );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleButtonUpL(const TPointerEvent& /*aPointerEvent*/ )
    {
    iTapPoint = TPoint( 0, 0 );
    CancelDragL( EFalse );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch ||
            aType == KAknsMessageSkinChange )
        {
        static_cast<CMmListBoxItemDrawer*>( iDrawer )->InvalidateCache();
        CMmTemplateLibrary * templateLibrary =
        static_cast<CMmListBoxItemDrawer*>( iDrawer )->TemplateLibrary();
        templateLibrary->CleanAndClearCache();
        }

    CCoeControl::HandleResourceChange( aType );

	if ( aType == KEikDynamicLayoutVariantSwitch && !IsHighlightVisible() )
		{
		SetDefaultHighlightL( EFalse );
		}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::ItemIconZoomL( TInt /*aItemIndex */)
    {
    TInt index = GetHighlight();
    if (index != iDraggedIndex)
        {
        iDrawer->AnimateItemZoomInL( index );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CMmWidgetContainer::GetSuiteModelL()
	{
	return GetMmModel()->GetSuiteModel();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleDragL(const TPointerEvent& aPointerEvent,
        TBool aAbortAnimation )
    {
    DEBUG(("\t_Mm_:CMmWidgetContainer::HandleDragL: iLastDragHighlight = %d ",
    		iLastDragHighlight ));

    TInt highlight = GetHighlight();
    DEBUG(("\t_Mm_:CMmWidgetContainer::HandleDragL: highlight = %d ", highlight ));

    TPoint dragDelta ( iLastDragPoint - aPointerEvent.iPosition);
    TInt dragSpeed = dragDelta.iX * dragDelta.iX + dragDelta.iY * dragDelta.iY;
    TBool tooFast = (dragSpeed > MmEffects::KDragSpeedIgnoreThreshold);

    if (iLastDragHighlight != highlight )
        {
       	iDragAndDropObserver->HandleDragOverL( GetHighlight() );
        }

    if (!tooFast)
    	{
		TSize itemSize = iWidget->View()->ItemSize( highlight );
		TPoint itemPos = iWidget->View()->ItemPos( highlight );

		if ( PointInItemReorderAreaL( highlight, aPointerEvent.iPosition ) )
		   {
		   CHnSuiteModel* model = GetMmModel()->GetSuiteModel();
		   CHnItemModel* onItemModel = model->GetItemModel( model->IdByIndex( highlight ));
		   if (iDrawer->GetAnimator()->IsReadyForNewAnimation() && !aAbortAnimation
				   && onItemModel->GetItemType() != EItemTypeParentFolder
				   && iDraggedIndex != highlight)
				   {
				   GetMmModel()->ReorderModelL( iDraggedIndex, highlight );
				   AnimateShiftL( highlight );
				   iDrawer->SetDraggedIndexL( highlight,
						   aPointerEvent.iPosition - iItemRelativeTapPoint );
				   iDraggedIndex = highlight;
				   iDragAndDropObserver->HandleDraggedIndexUpdatedL( highlight );
				   }
		   }
    	}

    if ( DeltaSquare( iTapPoint, aPointerEvent.iPosition ) > KDragDelta &&
				!iDrawer->IsDraggable() )
		{
		iDrawer->SetDraggedIndexL( iDraggedIndex, iTapPoint - iItemRelativeTapPoint );
		iDrawer->SetDraggableL( ETrue );
		}
	else if ( iDrawer->IsDraggable() )
		{
		iDrawer->SetDraggedPointL( aPointerEvent.iPosition - iItemRelativeTapPoint );
		}

    DEBUG(("\t_Mm_:CMmWidgetContainer::HandleDragL: iLastDragHighlight = %d ", iLastDragHighlight ));
    DEBUG(("\t_Mm_:CMmWidgetContainer::HandleDragL: highlight = %d ", highlight ));
    iLastDragPoint = aPointerEvent.iPosition;
    iLastDragHighlight = highlight;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::AnimateShiftL(TInt aHighlight)
	{
    TInt lowerVal =  (iDraggedIndex < aHighlight)? iDraggedIndex : aHighlight;
    TInt higherVal = (iDraggedIndex < aHighlight)? aHighlight : iDraggedIndex;
    TInt switchOperation = (iDraggedIndex < aHighlight) ? EFalse : ETrue;

       for(TInt iter(lowerVal); iter < higherVal; iter++ )
           {
           TInt from = iter + 1;
           TInt to = iter;
           if (switchOperation)
               {
               from = iter;
               to = iter + 1;
               }
           iDrawer->AnimateItemSwapL( from, to );
        }
     }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::CancelDragL( TBool aAnimate )
    {
    if (aAnimate)
        {
        iDrawer->AnimateDragItemTransitionL();
        }

    SetHighlightAfterDrag();

    iDraggedIndex = -1;
    iDrawer->SetDraggedIndexL( iDraggedIndex, TPoint(0,0) );
    SetDraggableL( EFalse );

    TInt dragFloatingItem;
    do
        {
        dragFloatingItem = iDrawer->GetFloatingItemIndex( EDrag );
        iDrawer->RemoveFloatingItem( dragFloatingItem );
        }
    while ( dragFloatingItem != KErrNotFound );

    iDrawer->GetAnimator()->CancelNextRedrawL();

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

TBool CMmWidgetContainer::IsFolderL(TInt aItemIndex)
	{
	TBool result = EFalse;

	CHnSuiteModel* model = GetMmModel()->GetSuiteModel();
	if (model)
		{
		CHnItemModel* onItemModel = model->GetItemModel( model->IdByIndex( aItemIndex ));
	    if (onItemModel)
	    	{
	    	result = onItemModel->GetItemType() == EItemTypeParentFolder ||
	      		onItemModel->GetItemType() == EItemTypeFolder;
	    	}
		}

	return result;

	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

TBool CMmWidgetContainer::IsNoItemDragged()
	{
	TBool noItemDragged = EFalse;
	noItemDragged = ( KErrNotFound == iDraggedIndex ) ? ETrue : noItemDragged;
	CHnSuiteModel* model = GetMmModel()->GetSuiteModel();
	if ( !noItemDragged && model)
		{
		CHnItemModel* onItemModel = model->GetItemModel( model->IdByIndex( iDraggedIndex ));
	    if (onItemModel)
	    	{
	    	noItemDragged = ( onItemModel->GetItemType() == EItemTypeParentFolder );
	    	}
		}
	return noItemDragged;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::SetHasFocusL( TBool aHasFocus )
    {
    if ( !!aHasFocus != !!iHasFocus ) // logical Ex-OR
        {
        iHasFocus = aHasFocus;
        StartOrStopMarquee();
        }
    if ( !aHasFocus )
    	{
    	SetHighlightVisibilityL( EFalse );
    	}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::SetIsFaded( TBool aIsFaded )
    {
    if ( !!aIsFaded != !!iIsFaded ) // logical Ex-OR
        {
        iIsFaded = aIsFaded;
        StartOrStopMarquee();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmWidgetContainer::LongTapInProgress() const
    {
    return iLongTapInProgress;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmWidgetContainer::IsDeleteLockedL( TInt aItemIndex )
	{
	TBool result = ETrue;

	CHnSuiteModel* model = GetMmModel()->GetSuiteModel();
	if (model)
		{
		CHnItemModel* onItemModel = model->GetItemModel(model->IdByIndex(
				aItemIndex));
		if (onItemModel)
			{
			result = onItemModel->IsDeleteLocked();
			}
		}

	return result;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::ManageFolderZoomingL( TBool aDraggedItemOverIcons )
	{
	TBool isAnimated;

	isAnimated = EFalse;
	for(int i=0; i< iDrawer->GetFloatingItemCount(); i++)
		{
	    TMmFloatingItem& current = iDrawer->GetFloatingItemAtIndex(i);
	    if (/*current.GetFloatingItemType() != EZoomTransition &&*/
	    		current.GetFloatingItemType() != EPostHighlightChangeRefreshItem &&
 				current.GetDrawnItemIndex() == GetHighlight() )
	    	{
	        isAnimated = ETrue;
	        break;
	        }
	    }

	if (!isAnimated)
		{
		if ( GetHighlight() != iDraggedIndex &&
			 IsFolderL( GetHighlight() ) &&
			 !IsDeleteLockedL( GetHighlight() ) &&
			 iDrawer->IsDraggable() )
			{
			iDrawer->AnimateItemZoomInL( GetHighlight() );
			}
		}

	isAnimated = EFalse;
	for(int i=0; i< iDrawer->GetFloatingItemCount(); i++)
		{
	    TMmFloatingItem& current = iDrawer->GetFloatingItemAtIndex(i);
	    if (current.GetFloatingItemType() != EZoomTransition &&
	    		current.GetFloatingItemType() != EPostHighlightChangeRefreshItem &&
				current.GetDrawnItemIndex() == iLastDragHighlight )
	    	{
	        isAnimated = ETrue;
	        break;
	        }
	    }

	if (!isAnimated)
		{
		if ( ( !aDraggedItemOverIcons && IsFolderL( iLastDragHighlight ) ) ||
				iLastDragHighlight != Widget()->CurrentItemIndex() )
			{
			iDrawer->AnimateItemZoomOutL( iLastDragHighlight );
			}
		}
	}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandlePointerEventsInEditModeL(
		const TPointerEvent& aPointerEvent, TBool aAbortAnimations )
	{

	TInt pointedItem = KErrNotFound;
	TBool draggedItemOverIcons = iWidget->View()->XYPosToItemIndex(
			aPointerEvent.iPosition, pointedItem );

	if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
//TODO: PROTO OF 2PHASE HIGHLIGHT EDITMODE BEHAVIOUR
//        if (!IsFolderL(pointedItem))
//        	{
//        	iDrawer->ClearFlags( CListItemDrawer::EPressedDownState );
//        	}
        HandleButtonDownL(aPointerEvent);
        iDragAndDropObserver->HandleDragStartL( GetHighlight() );
        }
    else if ( (aPointerEvent.iType == TPointerEvent::EDrag ||
            aPointerEvent.iType == TPointerEvent::EButtonRepeat) &&
            iDraggedIndex != KErrNotFound)
        {
//TODO: PROTO OF 2PHASE HIGHLIGHT EDITMODE BEHAVIOUR
//        if (!IsFolderL(pointedItem))
//        	{
//        	iDrawer->SetFlags( CListItemDrawer::EPressedDownState );
//        	}

        TPointerEvent pointerEvent = aPointerEvent;
        if( WidgetType() == EListWidget )
        	{
        	pointerEvent.iPosition.iX =
                    iWidget->View()->ItemPos( pointedItem ).iX +
                    iItemRelativeTapPoint.iX;
        	}

        if (GetHighlight() !=  Widget()->CurrentItemIndex() )
        	{
        	SetHighlightL( Widget()->CurrentItemIndex() );
        	}

        ManageFolderZoomingL( draggedItemOverIcons );

        HandleDragL(pointerEvent, aAbortAnimations);

        }
    else if (aPointerEvent.iType == TPointerEvent::EButton1Up && iDrawer->IsDraggable())
        {
        if ( !draggedItemOverIcons )
        	{
        	SetManualHighlightL( iDraggedIndex );
        	}

        iDragAndDropObserver->HandleDragStopL( GetHighlight() );
        HandleButtonUpL(aPointerEvent);
        }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent )
    {
    if ( iMarqueeAdapter && aPointerEvent.iType == TPointerEvent::EButton1Down )
    	{
    	iMarqueeAdapter->StopMarqueeDrawing();
    	}

    TInt index = KErrNotFound;
    TBool itemExists = iWidget->View()->XYPosToItemIndex( aPointerEvent.iPosition, index );
    
    if ( iLongTapDetector )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            iLongTapDetector->EnableLongTapAnimation( itemExists &&
                    GetMmModel()->GetNumberOfSpecificMenuItemsL( index ) > 0 );
            }
        TPointerEvent longTapPointerEvent = aPointerEvent;
        if ( aPointerEvent.iType == TPointerEvent::EButtonRepeat )
            {
            longTapPointerEvent.iType = TPointerEvent::EDrag;
            }
        iLongTapDetector->PointerEventL( longTapPointerEvent );
        }

    CCoeControl::HandlePointerEventL(aPointerEvent);

    if ( aPointerEvent.iType == TPointerEvent::EButton1Down
            && itemExists  )
        {
        SetHighlightL( index );
        }
    else if ( aPointerEvent.iType == TPointerEvent::EDrag
              // the line below is needed to enable edit mode in the list widget
    		  // because behaviour in the grid and list is different
    		  || (aPointerEvent.iType == TPointerEvent::EButtonRepeat && WidgetType() == EGridWidget ))
    	{
    	if ( itemExists )
    		{
        	if ( GetHighlight() != index )
        	    {
        	    SetHighlightVisibilityL( EFalse );
        	    if ( IsEditMode() )
        	    	{
        		    SetHighlightL( index );
        	    	}
        	    }
    		}
    	else
    		{
    		SetHighlightVisibilityL( EFalse );
    		}
    	}

	TInt lastTopItemIndex = Widget()->TopItemIndex();

    TBool abortAnimation = lastTopItemIndex != Widget()->TopItemIndex();

    if (abortAnimation)
        {
        iDrawer->GetAnimator()->CancelAnimationsL();
        }

    if ( IsEditMode() && iDragAndDropObserver )
    	{
		HandlePointerEventsInEditModeL(aPointerEvent, abortAnimation);
    	}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CMmWidgetContainer::GetItemRectL( TInt aItemIndex )
	{
	User::LeaveIfNull( iWidget );
	TPoint pos = iWidget->View()->ItemPos( aItemIndex );
	TSize size = iDrawer->GetItemSize( aItemIndex, EFalse );
	return TRect( pos.iX, pos.iY, pos.iX + size.iWidth, pos.iY + size.iHeight );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetDraggableL( TBool aDraggable )
    {

    iDrawer = STATIC_CAST(CMmListBoxItemDrawer*, iWidget->View()->ItemDrawer());
    iDrawer->SetDraggableL( aDraggable );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmWidgetContainer::IsEditMode() const
	{
	return iIsEditMode;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetEditModeL( TBool aIsEditMode )
    {
    iIsEditMode = aIsEditMode;
    StartOrStopMarquee();

    ASSERT(iDrawer);
    iDrawer->SetEditModeL( aIsEditMode );
    if ( !AknLayoutUtils::PenEnabled() && aIsEditMode )
    	{
        iDraggedIndex = iWidget->CurrentItemIndex();
        ScrollViewIfNeededL(); //only edit mode non-touch
    	}

    // scrollbar does not work when scrolling is disabled
    // iWidget->DisableScrolling( aIsEditMode );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::SetHighlightVisibilityL( TBool aEnable )
    {
    // activate the model
    CHnSuiteModel* suiteModel = GetMmModel()->GetSuiteModel();

    if ( suiteModel )
        {
        suiteModel->SetActiveL( aEnable );
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(
	    		iDrawer->Gc() );
#endif
    if ( !aEnable )
        {
        iWidget->View()->ItemDrawer()->SetFlags(
            CListItemDrawer::ESingleClickDisabledHighlight );
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	    if ( transApi )
	        {
           	transApi->Remove( MAknListBoxTfxInternal::EListHighlight );
  	        }
#endif
        }
    else
        {
        iWidget->View()->ItemDrawer()->ClearFlags(
            CListItemDrawer::ESingleClickDisabledHighlight );
        }
    
    if ( IsVisible() )
        {
        TInt highlight = GetHighlight();
        CListBoxView* view = Widget()->View();
        Widget()->DrawNow( TRect( view->ItemPos( highlight ),
                view->ItemSize( highlight ) ) );
        }
    }

// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CMmWidgetContainer::IsHighlightVisible()
    {
    return !( iWidget->View()->ItemDrawer()->Flags() & 
    		CListItemDrawer::ESingleClickDisabledHighlight );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMmWidgetContainer::IsDraggable()
	{
    iDrawer = STATIC_CAST(CMmListBoxItemDrawer*, iWidget->View()->ItemDrawer());
    return iDrawer->IsDraggable();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmWidgetContainer::CountComponentControls() const
    {
    return 1;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CCoeControl* CMmWidgetContainer::ComponentControl(TInt aIndex) const
    {
    switch( aIndex )
        {
        case 0:
            return iWidget;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmWidgetContainer::NumberOfItems()
    {
    return GetMmModel()->NumberOfItems();
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::SetObserver( MMmWidgetObserver*  aObserver )
    {
    //iWidget->SetListBoxObserver( aObserver );
    iListBoxObserver = aObserver;
    iKeyEventObserver = aObserver;
	iDragAndDropObserver = aObserver;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmWidgetContainer::GetPreviousHighlight()
    {
    return iPreviousHighlight;
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmWidgetContainer::GetHighlight( )
    {
    return iCurrentHighlight;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetManualHighlightL( TInt aItemIndex,
		TBool aRedraw )
	{
	SetHighlightL( aItemIndex );

	if ( aItemIndex >= 0 && aItemIndex < GetMmModel()->NumberOfItems() )
		{
		if ( aItemIndex != iWidget->CurrentItemIndex() )
			{
			iWidget->View()->SetCurrentItemIndex( aItemIndex );
			}

		if ( aRedraw )
			{
			iWidget->SetCurrentItemIndex( aItemIndex );
			iWidget->DrawNow();
			}
		}
	}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetHighlightL( TInt aItemIndex )
    {
    ASSERT( aItemIndex >= KErrNotFound );

    if (aItemIndex == KErrNotFound)
        {
        iCurrentHighlight =	 iPreviousHighlight = KErrNotFound;
        SetDefaultHighlightL( EFalse );
        }
    else if (aItemIndex >= 0 && aItemIndex < GetMmModel()->NumberOfItems())
        {
        iPreviousHighlight = iCurrentHighlight;
        iCurrentHighlight = aItemIndex;

        if ( iPreviousHighlight != iCurrentHighlight )
            {
            HideOptionsMenuIfDisplayed();
            }

        CHnSuiteModel* suiteModel = GetMmModel()->GetSuiteModel();

        if (suiteModel)
            {
            suiteModel->SetSuiteHighlightL( iCurrentHighlight );
            }
        }


	if ( IsEditMode() )
    	{
    	TMmFloatingItem postDragCurrent( GetHighlight(),
    			Widget()->View()->ItemPos( GetHighlight() ),
    			EPostHighlightChangeRefreshItem,
				MmEffects::KNoAnimationFramesCount, Widget()->View() );

    	iDrawer->AddFloatingItemL( postDragCurrent );

    	if ( GetPreviousHighlight() != GetHighlight() )
    		{
    		TMmFloatingItem postDragPrevious( GetPreviousHighlight(),
    				Widget()->View()->ItemPos( GetPreviousHighlight() ),
    				EPostHighlightChangeRefreshItem,
    				MmEffects::KNoAnimationFramesCount, Widget()->View() );

    		iDrawer->AddFloatingItemL( postDragPrevious );
    		}
    	}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmWidgetContainer::PointInItemReorderAreaL(
            TInt aItemIndex, TPoint aPoint )
    {
    TRect itemRect = GetItemRectL( aItemIndex );
    itemRect.Shrink( MmEffects::KShiftRatio * itemRect.Width(),
            MmEffects::KShiftRatio * itemRect.Height() );
    return itemRect.Contains( aPoint );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HideOptionsMenuIfDisplayed()
    {
    CEikMenuBar* menuBar = CEikonEnv::Static()->AppUiFactory()->MenuBar();
    if ( menuBar && menuBar->IsDisplayed() )
        {
        menuBar->StopDisplayingMenuBar();
        }
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetFlag(TInt /*Flag*/)
    {
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CEikListBox* CMmWidgetContainer::Widget()
    {
    return iWidget;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TKeyResponse CMmWidgetContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                                   TEventCode aType)
    {
    TKeyResponse resp = EKeyWasNotConsumed;

    TBool highlightVisibleWhenEventReceived = IsHighlightVisible();

    if ( IsEditMode() && IsDraggable() )
    	return resp;

    resp = HandleKeyEventL( aKeyEvent, aType );
    if ( iMarqueeAdapter && ( aType == EEventKeyDown ) )
    	{
    	iMarqueeAdapter->StopMarqueeDrawing();
    	}

    if (!AknLayoutUtils::PenEnabled() && iIsEditMode )
        {
		if ((GetHighlight() + ColumnsInCurrentView() > NumberOfItems() - 1)
						&& aKeyEvent.iScanCode == EStdKeyDownArrow)
			// the second condition is needed to block moving item down when there is no item below the moved item.
			{
			return resp;
			}

        if (aType == EEventKey )
            {
            TInt prevIndex = GetHighlight();
            static_cast<CMmListBoxItemDrawer*>(iDrawer)->SetHighlightShown( EFalse );
            resp = iWidget->OfferKeyEventL( aKeyEvent, aType );
            static_cast<CMmListBoxItemDrawer*>(iDrawer)->SetHighlightShown( ETrue );
            SetHighlightL( iWidget->CurrentItemIndex() );
            ScrollViewIfNeededL(); //only edit mode non-touch

            if ( prevIndex != GetHighlight()
            		&& KErrNotFound != prevIndex  )
            	{
            	iDraggedIndex = prevIndex;
                iDragAndDropObserver->HandleDragStartL( iDraggedIndex );
                GetMmModel()->ReorderModelL( iDraggedIndex, GetHighlight() );
                iDrawer->AnimateItemSwapL( iDraggedIndex , GetHighlight() );
                AnimateShiftL( GetHighlight() );
            	}
            return resp;
            }
        }

    if ( resp == EKeyWasNotConsumed )
        {
        resp = iWidget->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( iKeyEventObserver )
    	{
    	resp = iKeyEventObserver->HandleKeyPressedL( aKeyEvent, aType );
    	}

    if ( ( aKeyEvent.iScanCode == EStdKeyDevice3 ||
    		aKeyEvent.iScanCode == EStdKeyEnter ||
    		aKeyEvent.iScanCode == EStdKeyNkpEnter ) && aType == EEventKeyDown )
    	{
    	if ( highlightVisibleWhenEventReceived )
    		{
    		iAllowLongPress = EFalse;
    		}
    	else
    		{
    		iAllowLongPress = ETrue;

			SetHighlightVisibilityL( ETrue );
			SetDefaultHighlightL( ETrue );
			HandleForegroundGainedL();
			}
    	}
    return resp;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmWidgetContainer::ColumnsInCurrentView()
	{
	//should be overridden by deriving classes
	return NumberOfItems();
	}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmWidgetContainer::RowsInCurrentView()
    {
    //should be overridden by deriving classes
    CListBoxView *view = ((CListBoxView*) Widget()->View());
    return view->NumberOfItemsThatFitInRect( Rect());//Widget()->View()->ViewRect());
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleRockerPressL()
	{
    TKeyEvent ke;
    ke.iScanCode = EStdKeyNull;
    ke.iCode = EStdKeyNull;
    ke.iModifiers = 0;
    ke.iRepeats = 0;
    if( iKeyEventObserver )
        {
        iKeyEventObserver->HandleKeyPressedL( ke, EEventUser );
        }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TKeyResponse CMmWidgetContainer::HandleKeyEventL( const TKeyEvent &aKeyEvent,
        TEventCode aType )
    {
    TKeyResponse resp = EKeyWasNotConsumed;
    TBool arrowKeyPressed = aKeyEvent.iScanCode == EStdKeyRightArrow ||
        aKeyEvent.iScanCode == EStdKeyLeftArrow ||
        aKeyEvent.iScanCode == EStdKeyUpArrow ||
        aKeyEvent.iScanCode == EStdKeyDownArrow;
    TBool arrowHasHandling = ( aKeyEvent.iScanCode == EStdKeyRightArrow
									&& WidgetType() != EListWidget )
							|| ( aKeyEvent.iScanCode == EStdKeyLeftArrow
									&& WidgetType() != EListWidget )
							|| aKeyEvent.iScanCode == EStdKeyUpArrow
							|| aKeyEvent.iScanCode == EStdKeyDownArrow;

    // handle arrow: draw highlight when arrow used
    if ( arrowHasHandling )
        {
        if ( !IsHighlightVisible() )
            {
            if ( aType == EEventKey  || aType == EEventKeyUp )
                {
                SetHighlightVisibilityL( ETrue );
                SetDefaultHighlightL( ETrue );
                }
           	// override avkon's default highlight setting
         	resp = EKeyWasConsumed;
            }
        else
            {
            // this block is used to set the highlight only once for each event
            if ( aKeyEvent.iRepeats > 0 )
                {
                // if repeated key events we move focus on EEventKey
                if ( aType == EEventKey )
                    {
                    SetHighlightL( iWidget->CurrentItemIndex() );
                    }
                }
            else
                {
                // if single event we move focus on EEventKeyUp
                if ( aType == EEventKeyUp )
                    {
                    SetHighlightL( iWidget->CurrentItemIndex() );
                    }
                }
            }
    	}
    else if( arrowKeyPressed )
        {
        // ignore left and right arrows in list
        resp = EKeyWasConsumed;
        }
    // rocker select (short press)
    else if ( ( aKeyEvent.iScanCode == EStdKeyDevice3 ) && ( aType == EEventKeyDown  ) )
    	{
		HandleRockerPressL();
		resp = EKeyWasConsumed;
    	}
    else if ( iAllowLongPress && aType == EEventKey && aKeyEvent.iRepeats > 0 &&
    		( aKeyEvent.iScanCode == EStdKeyDevice3
				|| aKeyEvent.iScanCode == EStdKeyEnter
				|| aKeyEvent.iScanCode == EStdKeyNkpEnter ) )
    	{
    	HandleRockerPressL();
    	resp = EKeyWasConsumed;
    	iAllowLongPress = EFalse;
    	}

    return resp;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetSuiteModelL(CHnSuiteModel* aModel)
    {
    CHnSuiteModel* prevModel = GetMmModel()->GetSuiteModel();

    TInt numberOfItemsBefore (0);
    numberOfItemsBefore = GetMmModel()->NumberOfItems();

    GetMmModel()->SetSuiteModelL( aModel );

    // This needs to be in place (disabling redraw)
    // to udpate widget internal state, however to wait for
    // drawing until all highlight set matters are solved.
    iWidget->MakeVisible(EFalse);
    if ( GetMmModel()->NumberOfItems() >= numberOfItemsBefore )
        {
        HandleItemAdditionL();
        }
    else
        {
        HandleItemRemovalL();
        }
    if ( prevModel && aModel )
        {
        SetupWidgetLayoutL();
        }
    iWidget->MakeVisible(ETrue);

    Widget()->View()->ItemDrawer()->ClearFlags(
    		CListItemDrawer::EPressedDownState );
	iPostProcessor->StartAt( iWidget->BottomItemIndex() + 1 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::SetupDrawer()
	{
	iDrawer = STATIC_CAST(CMmListBoxItemDrawer*, iWidget->View()->ItemDrawer());
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C THnSuiteWidgetType CMmWidgetContainer::WidgetType()
	{
	return EUnspecified;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::RemoveLiwObjects()
    {
    // there is only one place where LIW objects can be (co)owned in mmwidgets
    // and that place is item drawer's cache
    iDrawer->TrimCacheSize( 0 );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::HandleBackgroundGainedL()
    {
    iInForeground = EFalse;
    StartOrStopMarquee();

    CancelDragL();

    // Fix for EMWK-7NYKRJ
    // Matrix Menu: Scrollbar highligted when App Key is pressed while it is held
    if ( AknLayoutUtils::PenEnabled() )
        {
        CEikScrollBar* scrollBar =
                iWidget->ScrollBarFrame()->VerticalScrollBar();
        if ( scrollBar && scrollBar->IsVisible() )
            {
            TPointerEvent fakeButton1UpEvent;
            fakeButton1UpEvent.iType = TPointerEvent::EButton1Up;
            fakeButton1UpEvent.iModifiers = 0;
            scrollBar->HandlePointerEventL( fakeButton1UpEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::HandleForegroundGainedL()
    {
    iInForeground = ETrue;
    StartOrStopMarquee();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmWidgetContainer::DeltaSquare( const TPoint aTapPoint,
		const TPoint aPos )
	{
	TInt delta( KErrNotFound );
	TInt height = aTapPoint.iY - aPos.iY;
	TInt width = aTapPoint.iX - aPos.iX;
    if( WidgetType() == EListWidget )
    	{
    	delta = height * height + height * height;
    	}
    else
    	{
    	delta = height * height + width * width;
    	}
	return delta;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::CancelDragL()
    {
    if( iIsEditMode && iDrawer )
        {
        if( iDrawer->GetFloatingItemIndex( EDrag ) != KErrNotFound )
            {
            CancelDragL( EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::HandleItemAdditionL()
	{
	// only the the descended method should be invoked.
	// this is needed to avoid codescanner warning
	User::Leave( KErrNotSupported );
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::HandleItemRemovalL()
	{
	// only the the descended method should be invoked.
	// this is needed to avoid codescanner warning
	User::Leave( KErrNotSupported );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::StopMovingL()
	{
	if ( !AknLayoutUtils::PenEnabled() && iDragAndDropObserver )
		iDragAndDropObserver->HandleDragStopL( GetHighlight() );
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::ScrollViewIfNeededL()
	{
	if ((iWidget->BottomItemIndex() / ColumnsInCurrentView()
			-  iWidget->TopItemIndex() / ColumnsInCurrentView() ) <= 1 )
		{
		return;
		}

	TBool needToScrollUp =
		GetHighlight() - iWidget->TopItemIndex() < ColumnsInCurrentView()
		&& iWidget->TopItemIndex() != 0;

	TBool needToScrollDown =
		iWidget->BottomItemIndex() - GetHighlight() < ColumnsInCurrentView()
		&& iWidget->BottomItemIndex() / ColumnsInCurrentView()
			!= ( NumberOfItems() - 1 )  / ColumnsInCurrentView();

	if ( WidgetType() == EGridWidget )
		{
		// TODO: temporary - invisible partial items in MCL grid :/
		needToScrollDown =
			iWidget->BottomItemIndex() - ColumnsInCurrentView() - GetHighlight() < ColumnsInCurrentView()
		    && ( iWidget->BottomItemIndex() / ColumnsInCurrentView() ) - 1
				!= ( NumberOfItems() - 1 ) / ColumnsInCurrentView()
			&& iWidget->BottomItemIndex() - iWidget->TopItemIndex()
				> ColumnsInCurrentView() * RowsInCurrentView();
		}

	if ( needToScrollUp )
		{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
			MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( iDrawer->Gc() );
			if ( transApi )
				{
				transApi->SetMoveType( MAknListBoxTfxInternal::EListScrollUp );
				}
#endif
		iDrawer->RemoveFloatingItems();
		iWidget->View()->VScrollTo(
				iWidget->TopItemIndex() - ColumnsInCurrentView() );
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
			if ( transApi )
				{
				transApi->Draw( Rect() );
				}
#endif
		UpdateViewScrollBarThumbs();
		}
	else if ( needToScrollDown )
		{

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
			MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( iDrawer->Gc() );
			if ( transApi )
				{
				transApi->SetMoveType( MAknListBoxTfxInternal::EListScrollDown );
				}
#endif
		iDrawer->RemoveFloatingItems();
		iWidget->View()->VScrollTo(
				iWidget->TopItemIndex() + ColumnsInCurrentView() );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
			if ( transApi )
				{
				transApi->Draw( Rect() );
				}
#endif

		UpdateViewScrollBarThumbs();
		}
	}

//----------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::UpdateViewScrollBarThumbs()
	{

	}

//----------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::StartOrStopMarquee()
    {
    if ( iMarqueeAdapter )
        {
        TBool marqueeShouldBeEnabled = iHasFocus && iInForeground
                && !iIsFaded && !IsEditMode();

         // logical Ex-OR
        if ( !!marqueeShouldBeEnabled != !!iMarqueeAdapter->IsMarqueeEnabled() )
            {
            if ( !marqueeShouldBeEnabled )
                {
                iMarqueeAdapter->StopMarqueeDrawing();
                }

            iMarqueeAdapter->EnableMarquee( marqueeShouldBeEnabled );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::SetHighlightAfterDrag()
	{
    if ( AknLayoutUtils::PenEnabled() &&
    		iDraggedIndex != KErrNotFound &&
			iDraggedIndex != Widget()->CurrentItemIndex()
			&& iDraggedIndex < NumberOfItems() )
    	{
    	TBool isFolder(EFalse);
    	CHnSuiteModel* model = GetMmModel()->GetSuiteModel();
		CHnItemModel* onItemModel = model->GetItemModel(
				model->IdByIndex( Widget()->CurrentItemIndex() ));
	    if (onItemModel)
	    	{
	    	isFolder = onItemModel->GetItemType() == EItemTypeParentFolder ||
				onItemModel->GetItemType() == EItemTypeFolder;
	    	}
	    if (!isFolder)
	    	{
	    	Widget()->View()->SetCurrentItemIndex( iDraggedIndex );
	    	}
    	}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::ValidateWidgetCurrentItemIndex()
	{
	if (Widget()->CurrentItemIndex() >= GetMmModel()->NumberOfItems()
			|| Widget()->CurrentItemIndex() == KErrNotFound )
		{
		iCurrentHighlight = GetMmModel()->NumberOfItems();
		if( iCurrentHighlight > 0 )
			{
			Widget()->View()->SetCurrentItemIndex( --iCurrentHighlight );
			}
		else
			{
			iCurrentHighlight = KErrNotFound ;
			}
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::CacheWidgetPosition()
	{
	iWidgetPositionCache.iVerticalItemOffset = VerticalItemOffset();
	iWidgetPositionCache.iTopItemIndex = Widget()->TopItemIndex();
	iWidgetPositionCache.iLandscape
			= Layout_Meta_Data::IsLandscapeOrientation();
	iWidgetPositionCache.iHighlightedItemId = KErrNotFound;

	TInt highlightedItemIndex = Widget()->CurrentItemIndex();
	CHnSuiteModel* suiteModel = GetMmModel()->GetSuiteModel();
	if ( suiteModel && highlightedItemIndex != KErrNotFound
	        && suiteModel->GetSuiteHighlight() == highlightedItemIndex
	        && ItemIsVisible( highlightedItemIndex ) )
	    {
	    iWidgetPositionCache.iHighlightedItemId =
            suiteModel->IdByIndex( highlightedItemIndex );
	    }

	iWidgetPositionCache.iValid = ETrue;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::RestoreWidgetPosition()
	{
	if (iWidgetPositionCache.iValid)
		{
	   if ( iWidgetPositionCache.iLandscape
				== Layout_Meta_Data::IsLandscapeOrientation() )
			{
            // If view position was cached during boundary effect, correct the
            // cached position.
            if ( iWidgetPositionCache.iTopItemIndex == 0 &&
                    iWidgetPositionCache.iVerticalItemOffset > 0 )
                {
                iWidgetPositionCache.iVerticalItemOffset = 0;
                // This corrects the position cached during the upper boundary
                // effect. AlignBottomOfViewL will take care of lower boundary
                // effect.
                }

			Widget()->View()->SetTopItemIndex(iWidgetPositionCache.iTopItemIndex);
			SetVerticalItemOffset(iWidgetPositionCache.iVerticalItemOffset);
			TRAP_IGNORE( AlignBottomOfViewL() );

			// Important: If an item that was previously highlighted and visible is
			// still highlighted and yet somehow is not visible after the position
			// has been restored, fix the problem by scrolling the view until that
			// item is visible again:
			CHnSuiteModel* suiteModel = GetMmModel()->GetSuiteModel();
			if ( suiteModel && IsHighlightVisible() )
			    {
			    TInt highlightedItemIndex = suiteModel->GetSuiteHighlight();
			    TInt highlightedItemId = highlightedItemIndex != KErrNotFound ?
			        suiteModel->IdByIndex( highlightedItemIndex ) : KErrNotFound;
			    if ( highlightedItemId != KErrNotFound
			            && highlightedItemId == iWidgetPositionCache.iHighlightedItemId
			            && !ItemIsVisible( highlightedItemIndex ) )
			        {
			        TRAP_IGNORE( ScrollToItemL( highlightedItemIndex ) );
			        }
			    }
			}
		else
			{
			TRAP_IGNORE( ScrollToItemL(Widget()->CurrentItemIndex()) );
			}

		iWidgetPositionCache.iValid = EFalse;
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::MakeVisible(TBool aVisible)
	{
	if ( !aVisible )
		{
		RestoreWidgetPosition();
		CacheWidgetPosition();
		iDrawer->RemoveFloatingItems();
		}
	else if ( aVisible )
		{
		RestoreWidgetPosition();
		}
	CCoeControl::MakeVisible(aVisible);
	iWidget->MakeVisible(aVisible);
	if ( !aVisible )
        {
        SetHighlightVisibilityL( EFalse );
        }
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::ResetWidgetPosition()
	{
	iWidgetPositionCache.iVerticalItemOffset = 0;
	iWidgetPositionCache.iTopItemIndex = 0;
	iWidgetPositionCache.iValid = EFalse;
	iWidgetPositionCache.iHighlightedItemId = KErrNotFound;
	Widget()->SetTopItemIndex(0);
	SetVerticalItemOffset(0);
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::NumberOfItemsChangedL( TItemsChangeType aChange )
	{
	if ( AknLayoutUtils::PenEnabled() )
		{
		CacheWidgetPosition();
		Widget()->View()->SetDisableRedraw( ETrue );

		HandleNumberOfItemsChangedL( aChange );

		Widget()->View()->SetDisableRedraw( EFalse );
		RestoreWidgetPosition();
		}
	else
		{
		// there is no need to cache and restore widget position in non-touch;
		// moreover, it can cause some problems with scrolling when adding
		// new folder
		HandleNumberOfItemsChangedL( aChange );
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleNumberOfItemsChangedL( TItemsChangeType aChange )
	{
	SetHighlightVisibilityL( EFalse );
	if ( aChange == EItemsAdded )
		{
		HandleItemAdditionL();
		}
	else if ( aChange == EItemsRemoved )
		{
		HandleItemRemovalL();
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmWidgetContainer::AlignBottomOfViewL()
	{
	TInt scrollConsumed( EFalse );
	TInt pixelsToScroll( 0 );

	if ( NumberOfItems() > 0 )
		{
		pixelsToScroll = CalcBottomPixelsToScroll();
		}

	if ( pixelsToScroll != 0 )
		{
		ScrollInPixelsL( pixelsToScroll );
		scrollConsumed = ETrue;
		}

	return scrollConsumed;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmWidgetContainer::CalcBottomPixelsToScroll()
    {
    const TInt firstItemIndex( 0 );
    TInt lastItemIndex = NumberOfItems() - 1;
    TInt viewHeight = Widget()->View()->ViewRect().Height();
    TInt lastItemBottomY = Widget()->View()->ItemPos(lastItemIndex).iY
                + Widget()->ItemHeight();
    TInt pixelsToScroll( 0 );

    if ( Widget()->ScrollBarFrame()->VerticalScrollBar()->IsVisible() )
        {
        pixelsToScroll = Min( 0, lastItemBottomY - viewHeight );
        }
    else
        {
        pixelsToScroll = Widget()->View()->ItemPos( firstItemIndex ).iY;
        }

    return pixelsToScroll;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmWidgetContainer::ItemIsVisible( TInt aItemIndex ) const
    {
    return iWidget->View()->ItemIsVisible( aItemIndex );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMmWidgetContainer::ItemIsFullyVisible(TInt aIndex)
	{
	return ItemIsVisible( aIndex )
			&& !Widget()->View()->ItemIsPartiallyVisible(aIndex);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::ScrollInPixelsL(TInt aPixels)
	{
	SetupScrollingEffectsL( aPixels > 0 );

	if ( AknLayoutUtils::PenEnabled() )
		{
		Widget()->HandlePhysicsScrollEventL(aPixels);
		}
	else
		{
		// non-touch avkon doesn't seem to support scrolling by given
		// amount of pixels
		TInt delta = aPixels / Widget()->View()->ItemHeight();

		Widget()->View()->VScrollTo( Widget()->TopItemIndex() +
				delta * ColumnsInCurrentView() );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMmWidgetContainer::ScrollToItemL(TInt aIndex)
	{
	TInt scrollConsumed(EFalse);
	if (aIndex >= 0 && aIndex <= NumberOfItems())
		{
		scrollConsumed = AlignBottomOfViewL();
		if ( !scrollConsumed && Widget()->View()->ItemIsPartiallyVisible(aIndex))
			{
//			the case when the item is partially visible at top or
//			bottom of screen. Th e view is scrolled the offset to
//			make the item entirely visible.
			TInt offsetBottom = Widget()->View()->ItemPos(aIndex).iY
					+ Widget()->ItemHeight()
					- Widget()->View()->ViewRect().Height();
			TInt offsetTop = Widget()->View()->ItemPos(aIndex).iY;
			TBool takeTop = Abs(offsetTop) < Abs(offsetBottom);
			TInt offset = (takeTop) ? offsetTop : offsetBottom;
			if (offset != 0)
				{
				ScrollInPixelsL( offset );
				scrollConsumed = ETrue;
				}

			}
		else if (!Widget()->View()->ItemIsVisible(aIndex))
			{
//			the case when the item is not visible on screen
			SetupScrollingEffectsL( aIndex > Widget()->BottomItemIndex() );
			Widget()->ScrollToMakeItemVisible(aIndex);
			AlignBottomOfViewL();
			scrollConsumed = ETrue;
			}
		}
	return scrollConsumed;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::SetupScrollingEffectsL(TBool aDown)
	{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(
			iDrawer->Gc());
	if (transApi)
		{
		MAknListBoxTfxInternal::TMovementType movementType;
		movementType = (aDown == (TInt)ETrue ) ? MAknListBoxTfxInternal::EListScrollDown
				: MAknListBoxTfxInternal::EListScrollUp;
		transApi->SetMoveType(movementType);
		}
#endif
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::PrepareForGarbage()
	{
//	This is called when the suite model is destoyed and the container is set to be destroyed
//	by the garbage collector. There should be no redraws done to the widget in this period
//	because this will cause the screen to be redrawn only containing the balnk background.
//	Before garbage cleanup an asynchronous redraw event may intend to redraw the widget.
//	SetDisableRedraw() prevents redrawing. Animator is also prepared for garbage so there is
//	no instance which can enable redraw of the widget.
	Widget()->View()->SetDisableRedraw( ETrue );
	iDrawer->RemoveFloatingItems();
//	Prapares the animator for garbage, which means it blocks any animator drawing.
	iDrawer->GetAnimator()->PrepareForGarbage();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleLongTapEventL( const TPoint& aPenEventLocation,
                                      const TPoint& aPenEventScreenLocation )
	{
	TInt index( KErrNotFound );
	if ( iWidget->View()->XYPosToItemIndex( aPenEventLocation, index ) )
		{
		SetHighlightVisibilityL( ETrue );
	    iLongTapInProgress = ETrue;
		if ( iLongTapObserver )
			{
			iLongTapObserver->HandleLongTapEventL( aPenEventScreenLocation );
			}
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::SetLongTapObserver( MMmLongTapObserver* aObserver )
	{
	iLongTapObserver = aObserver;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmWidgetContainer::EndLongTapL( TBool aStopTimer )
	{
	if ( iLongTapInProgress )
		{
		iLongTapInProgress = EFalse;
		if( aStopTimer )
			{
			SetHighlightVisibilityL( EFalse );
			}
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmWidgetContainer::HandleListBoxEventL( CEikListBox* aListBox,
        TListBoxEvent aEventType )
    {
//    handle same behaviour in edit mode and normal mode
    switch ( aEventType )
		{
		case MEikListBoxObserver::EEventPenDownOnItem:
			{
			iDragOccured = EFalse;
			if ( !iLongTapInProgress )
				{
				SetHighlightVisibilityL( ETrue );
				}
			break;
			}
		case MEikListBoxObserver::EEventItemSingleClicked:
			{
			if ( !iDragOccured && !iLongTapInProgress )
				{
				SetHighlightL( Widget()->CurrentItemIndex() );
				SetHighlightVisibilityL( EFalse );
				}
			iDragOccured = EFalse;
			break;
			}
		case MEikListBoxObserver::EEventItemDraggingActioned:
			{
			SetHighlightVisibilityL( EFalse );
			iDragOccured = ETrue;
			break;
			}
		case MEikListBoxObserver::EEventPanningStarted:
		case MEikListBoxObserver::EEventPanningStopped:
		case MEikListBoxObserver::EEventFlickStarted:
		case MEikListBoxObserver::EEventFlickStopped:
			{
			SetHighlightVisibilityL( EFalse );
			break;
			}
		}

//    handle different behaviour in edit mode and normal mode
	if ( !IsEditMode() )
		{
		switch ( aEventType )
			{
			case MEikListBoxObserver::EEventFlickStarted:
				{
				static_cast<CMmListBoxItemDrawer*>(
						Widget()->View()->ItemDrawer() )->
						EnableCachedDataUse( ETrue );
				break;
				}
			case MEikListBoxObserver::EEventFlickStopped:
				{
				static_cast<CMmListBoxItemDrawer*>(
						Widget()->View()->ItemDrawer() )->
						EnableCachedDataUse( EFalse );
				DrawView();
				break;
				}
			}
		}
	else
		{
		switch ( aEventType )
			{
			case MEikListBoxObserver::EEventFlickStopped:
				{
				// this fixes some problems with messed edit mode
				// caused by kinetic scrolling
				iDrawer->GetAnimator()->SetNextRedrawToWholeScreen();
				break;
				}
			}
		}

    if ( iListBoxObserver && !iLongTapInProgress )
        {
        iListBoxObserver->HandleListBoxEventL( aListBox, aEventType );
        }

    }

//End of file
