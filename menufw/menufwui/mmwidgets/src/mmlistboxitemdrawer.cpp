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


#include <eikfrlb.h>
#include "mmwidgetsconstants.h"
#include "hnconvutils.h"
#include <gdi.h>
#include <AknIconArray.h>
#include <AknsConstants.h>
#include <AknFontAccess.h>
#include <aknlistboxtfx.h>
#include <layoutmetadata.cdl.h>
#include <AknIconUtils.h>
#include <AknBidiTextUtils.h>
#include <AknDef.hrh>
#include <AknLayoutFont.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif

#include <AknsLayeredBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>
#include "bitmaptransforms.h"
#include "mmwidgetsconstants.h"
#include "mmlistbox.h"
#include "mmtemplatelibrary.h"
#include "mmlistboxitemdrawer.h"
#include "mmlistboxmodel.h"
#include "mmdraweranimator.h"
#include "mmmarqueeadapter.h"
#include "mmfloatingitem.h"
#include "mmgridview.h"
#include "mmcacheforitem.h"
#include "mmitemsdatacache.h"
#include "mmwidgetcontainer.h"
#include "hnsuitemodel.h"
#include "menudebug.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxItemDrawer::CMmListBoxItemDrawer(
        CMmListBoxModel* aMmListBoxModel,
	    const CFont* aFont,
	    CFormattedCellListBoxData* aFormattedCellData,
	    TMmWidgetType aWidgetType,
	    CMmTemplateLibrary* aTemplateLibrary )
	: CFormattedCellListBoxItemDrawer( aMmListBoxModel , aFont, aFormattedCellData),
	iFont(aFont), iLeftOverAreaUnderAnimatedItem(EFalse)
	{
	iWidgetType = aWidgetType;
	iTemplateLibrary = aTemplateLibrary;
	iMmModel = aMmListBoxModel;
	iRedrawBackground = ETrue;
	iZoomIconIndex = -1;
	iIconAnimationZoomRatio = 1;
	// this is needed to get iColors initialized on first use:
	iLastDrawnItemWasFloating = ETrue;
    SetFlags( CListItemDrawer::EDisableHighlight );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxItemDrawer::~CMmListBoxItemDrawer()
	{
	iFloatingItems.Close();
	delete iItemsDataCache;
	delete iAnimator;
	delete iSubcellText;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::EnableCachedDataUse( TBool aEnable )
    {
    iUseCache = aEnable;
    if ( aEnable )
        {
        RemoveFloatingItems();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::CachedDataUseIsEnabled() const
    {
    return iUseCache;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::TrimCacheSize( TInt aItemCount )
    {
    iItemsDataCache->Trim( aItemCount );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::InvalidateCache()
    {
    iItemsDataCache->Invalidate();
    // this is needed to get iColors initialized on first use:
    iLastDrawnItemWasFloating = ETrue;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxItemDrawer* CMmListBoxItemDrawer::NewLC(
        CMmListBoxModel* aMmListBoxModel,
	    const CFont* aFont,
	    CFormattedCellListBoxData* aFormattedCellData,
	    TMmWidgetType aWidgetType,
	    CMmTemplateLibrary* aTemplateLibrary )
	{
	CMmListBoxItemDrawer* self = new (ELeave)CMmListBoxItemDrawer(
			aMmListBoxModel, aFont, aFormattedCellData, aWidgetType,
			aTemplateLibrary );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmListBoxItemDrawer* CMmListBoxItemDrawer::NewL(
        CMmListBoxModel* aMmListBoxModel,
	    const CFont* aFont,
	    CFormattedCellListBoxData* aFormattedCellData,
	    TMmWidgetType aWidgetType,
	    CMmTemplateLibrary* aTemplateLibrary )
	{
	CMmListBoxItemDrawer* self = CMmListBoxItemDrawer::NewLC(
	    aMmListBoxModel, aFont, aFormattedCellData, aWidgetType,
        aTemplateLibrary );
	CleanupStack::Pop( self );
	return self;

	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetView(CEikListBox * aView)
    {
    iWidget = aView;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CEikListBox* CMmListBoxItemDrawer::Widget() const
    {
    return iWidget;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::ConstructL()
	{
	iAnimator = CMmDrawerAnimator::NewL( *this );
	iItemsDataCache = CMmItemsDataCache::NewL();
	iIsEditMode = EFalse;
	iHighlightShown = EFalse;
	iDrawMoveIndicators = ETrue;
	}

// ----xm-------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DrawEmptyItem( TInt aItemIndex,
        TPoint aItemRectPos,  TBool aViewIsDimmed ) const
    {
    TRect r( aItemRectPos, iItemCellSize );

    CFormattedCellListBoxItemDrawer::DrawEmptyItem( aItemIndex, aItemRectPos,
        aViewIsDimmed );

    const_cast<CMmListBoxItemDrawer*>(this)->DrawFloatingItems(r);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxItemDrawer::GetFloatingItemIndex(TMmFloatingItemType aType) const
    {
    TInt ret (KErrNotFound);
    TInt i(iFloatingItems.Count()-1);
    for(; i >= 0; i--)
        {
        if (iFloatingItems[i].GetFloatingItemType() == aType)
            {
            ret = i;
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TMmFloatingItem& CMmListBoxItemDrawer::GetFloatingItemL(TMmFloatingItemType aType)
    {
    TInt index = GetFloatingItemIndex(aType);
    User::LeaveIfError( index );
    return iFloatingItems[ index ];
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AnimateDragItemTransitionL()
    {
    iAnimator->AnimateDragItemTransitionL();
    iAnimator->Trigger();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AnimateItemSwapL( TInt aItemFrom, TInt aItemTo )
    {
    iAnimator->AnimateItemSwapL( aItemFrom, aItemTo );
    iAnimator->Trigger();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AnimateItemZoomInL( TInt aItemIndex )
    {
    iAnimator->AnimateItemZoomL( aItemIndex, ETrue );
    iAnimator->Trigger();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AnimateItemZoomOutL( TInt aItemIndex )
    {
    iAnimator->AnimateItemZoomL( aItemIndex, EFalse );
    iAnimator->Trigger();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DrawFloatingItems(TRect currentlyDrawnRect)
    {
    SetRedrawItemBackground( EFalse );
    for(TInt i(iFloatingItems.Count()-1); i >= 0 ; i--)
        {
        TMmFloatingItemType type = iFloatingItems[i].GetFloatingItemType();
        if ( iFloatingItems[i].IsFloatingItemValid() )
            {
            TInt drawnItemIndex = iFloatingItems[i].GetDrawnItemIndex();
            TSize size = iWidget->View()->ItemSize( drawnItemIndex );
            TRect rect( iFloatingItems[i].GetItemPosition(), iFloatingItems[i].GetItemPosition() + size);

            if (rect.Intersects(currentlyDrawnRect))
				{
				TInt tempZoomIconIndex = iZoomIconIndex;
				TInt tempZoomRatio = iIconAnimationZoomRatio;

				iZoomIconIndex = iFloatingItems[i].GetDrawnItemIndex();
				iIconAnimationZoomRatio = iFloatingItems[i].GetCurrentZoomRatio();

				if ( ItemHasFloatingType( drawnItemIndex, EDrag) ||
						ItemHasFloatingType( drawnItemIndex, EDragTransition) )
					{
					ClearFlags( CListItemDrawer::EPressedDownState );
					}

				DrawActualItem( drawnItemIndex, rect, ETrue, EFalse, EFalse, EFalse );
				iIconAnimationZoomRatio = tempZoomRatio;
				iZoomIconIndex = tempZoomIconIndex;
				}
            }
        else
        	{
            iFloatingItems.Remove(i);
        	}
        }
    SetRedrawItemBackground( ETrue );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxItemDrawer::GetValidFloatingItemCount(TMmFloatingItemType aType)
    {
    TInt count (0);

    for(TInt i(iFloatingItems.Count()-1); i >= 0; i--)
        {
        if (iFloatingItems[i].GetFloatingItemType() == aType
                && iFloatingItems[i].IsFloatingItemValid() )
            count++;
        }
    return count;
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DrawItem(TInt aItemIndex, TPoint aItemRectPos,
        TBool aItemIsSelected, TBool aItemIsCurrent, TBool aViewIsEmphasized,
        TBool aViewIsDimmed) const
        {
        if ( !Widget()->View()->RedrawDisabled() )
            {
            TBool highlightVisible =
                !( Flags() & CListItemDrawer::ESingleClickDisabledHighlight );
            aItemIsCurrent = aItemIsCurrent && highlightVisible;
            DoDrawItem( aItemIndex, aItemRectPos, aItemIsSelected,
                    aItemIsCurrent, aViewIsEmphasized, aViewIsDimmed);
            }
        }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DoDrawItem(TInt aItemIndex, TPoint aItemRectPos,
        TBool aItemIsSelected, TBool aItemIsCurrent, TBool aViewIsEmphasized,
        TBool aViewIsDimmed) const
    {
    TSize itemCellSize = TSize( GetItemSize( aItemIndex, aItemIsCurrent ));
    TRect actualItemRect(aItemRectPos, itemCellSize);

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
	if ( transApi )
		{
		transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
		transApi->StopDrawing();
		transApi->BeginRedraw( MAknListBoxTfxInternal::EListItem,
				actualItemRect, aItemIndex );
		}
#endif

    const_cast<CMmListBoxItemDrawer*>(this)->iLeftOverAreaUnderAnimatedItem = EFalse;
    for(TInt i(iFloatingItems.Count()-1); i >= 0; i--)
        {
        if ( iFloatingItems[i].GetDrawnItemIndex() == aItemIndex
        		&& iFloatingItems[i].IsFloatingItemValid() )
            {
            const_cast<CMmListBoxItemDrawer*>(this)->iLeftOverAreaUnderAnimatedItem = ETrue;
            break;
            }
        }

    DrawActualItem(aItemIndex, actualItemRect, aItemIsCurrent, aViewIsEmphasized,
    		aViewIsDimmed, aItemIsSelected);
    const_cast<CMmListBoxItemDrawer*>(this)->iLeftOverAreaUnderAnimatedItem = EFalse;

    const_cast<CMmListBoxItemDrawer*>(this)->DrawFloatingItems(actualItemRect);

    if (!AknLayoutUtils::PenEnabled() && IsEditMode())
        {
        const_cast<CMmListBoxItemDrawer*>(this)->DrawActualIndicatorItem(
        		aItemIndex, actualItemRect );
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	if ( transApi )
		{
	    transApi->EndRedraw( MAknListBoxTfxInternal::EListItem, aItemIndex );
		}
#endif



    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DrawItemText(TInt aItemIndex,
		const TRect &aItemTextRect, TBool aItemIsCurrent,
		TBool aViewIsEmphasized, TBool aItemIsSelected ) const
    {
    TRAP_IGNORE( DoDrawItemTextL( aItemIndex, aItemTextRect, aItemIsCurrent,
    		aViewIsEmphasized, aItemIsSelected ) );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DoDrawItemTextL( TInt aItemIndex, const TRect
		&aItemTextRect, TBool aItemIsCurrent, TBool aViewIsEmphasized,
		TBool /* aItemIsSelected */) const
    {
    CMmCacheForItem* cache = iItemsDataCache->GetItemCacheL( aItemIndex );
    cache->InvalidateIfCacheMayNotBeUsed(
            aItemIsCurrent, iLastSubcellsSetupCode );

    if ( IsRedrawItemBackgroundEnabled() )
        {
        DrawBackgroundAndSeparatorLines( aItemTextRect );
        }

    if ( !iUseCache || !cache->IsValid() )
        {
        SetupSubCellsL( aItemIsCurrent, aItemIndex );
        }
    __ASSERT_DEBUG( cache->IsValid(), User::Invariant() );

    FormattedCellData()->SetIconArray( cache->GetIconListL() );

	TBool isFloating = !iUseCache && IsFloating( aItemIndex );
	if ( !!isFloating != !!iLastDrawnItemWasFloating ) // Ex-OR
	    {
	    iLastDrawnItemWasFloating = isFloating;
	    iColors = SetupColors( isFloating );
	    }

	CFormattedCellListBoxData* data = static_cast<CFormattedCellListBoxData*>(iData);
	data->EnableMarqueeL( EFalse );

    TBool highlightShown = ETrue;
	if (FormattedCellData()->RespectFocus() && !aViewIsEmphasized)
		{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
		MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( iGc );
		if ( transApi )
			 {
			 transApi->Remove( MAknListBoxTfxInternal::EListHighlight );
			 }
#endif
		highlightShown = EFalse;
		}

    data->Draw( Properties(aItemIndex), *iGc, &( cache->GetItemText() ), aItemTextRect,
    		GetHighlightVisibility( aItemIndex, aItemIsCurrent, highlightShown ), iColors );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( iGc );
	if ( transApi )
		{
		transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
		}
#endif

	if ( iMarqueeAdapter && aItemIsCurrent )
		{
		DEBUG(("CMmListBoxItemDrawer::DoDrawItemTextL - DrawMarquee"));
		iMarqueeAdapter->DrawMarqueeL( *iGc );
		}

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	if ( transApi )
		{
		transApi->StopDrawing();
		}
#endif

	ColumnData()->SetIconArray( NULL );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetRedrawItemBackground( TBool aRedraw )
    {
    iRedrawBackground = aRedraw;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::IsRedrawItemBackgroundEnabled() const
    {
    return iRedrawBackground;
    }

void CMmListBoxItemDrawer::DrawBackgroundAndSeparatorLines( const TRect& aItemTextRect ) const
    {
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    CCoeControl* control = FormattedCellData()->Control();

    if ( IsRedrawItemBackgroundEnabled() )
        {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
  	    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( iGc );
        if ( transApi )
            {
            transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
            }
#endif
        TBool bgDrawn( EFalse );
        if ( control )
            {
	        if ( CAknEnv::Static()->TransparencyEnabled() )
                {
                bgDrawn = AknsDrawUtils::Background(
                    skin, iBgContext, control, *iGc, aItemTextRect,
                    KAknsDrawParamNoClearUnderImage );
                }
            else
                {
                bgDrawn = AknsDrawUtils::Background(
                    skin, iBgContext, control, *iGc, aItemTextRect,
                    KAknsDrawParamNoClearUnderImage |
                    KAknsDrawParamBottomLevelRGBOnly );
                }
            }
        if ( !bgDrawn )
            {
            iGc->Clear( aItemTextRect );
            }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        if ( transApi )
            {
            transApi->StopDrawing();
            }
#endif
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupSubNoCellL( TInt aIndex,
        TInt aItemIndex ) const
    {
    TTemplateChild child;
    child.iFontId = EAknLogicalFontSecondaryFont;
    child.iIsImage = EFalse;
    const_cast<CMmListBoxItemDrawer*>(this)->
        ReplaceSubCellText( KNullDesC() );
    SetupSubCellL( child, aIndex, aItemIndex );
    iLastSubcellsSetupCode.AddSubcellInfo( TMmSubcellsSetupCode::ENoSubcell );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupSubIcondDragHighlightCellL(
        TTemplateChild aSubCellTemplate, TInt aIndex, TInt aItemIndex ) const
    {
    aSubCellTemplate.iRectAccordingToParent.iBr
		= TPoint(ItemCellSize().iWidth * iIconAnimationZoomRatio,
				ItemCellSize().iHeight * iIconAnimationZoomRatio);

    aSubCellTemplate.iRectAccordingToParent.iTl = TPoint(0, 0);
    SetupSubCellL( aSubCellTemplate, aIndex , aItemIndex);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupSubCellL( TTemplateChild aSubCellTemplate,
        TInt aIndex, TInt aItemIndex  ) const
	{
    CFormattedCellListBoxData* data = static_cast<CFormattedCellListBoxData*>(iData);
    TInt width = aSubCellTemplate.iRectAccordingToParent.iBr.iX - aSubCellTemplate.iRectAccordingToParent.iTl.iX;
    TInt height = aSubCellTemplate.iRectAccordingToParent.iBr.iY - aSubCellTemplate.iRectAccordingToParent.iTl.iY;
    data->SetTransparentSubCellL(aIndex, ETrue);
    data->SetSubCellSizeL( aIndex, TSize(width,height));
    data->SetSubCellIconSize( aIndex, TSize(width,height) );
	data->SetSubCellPositionL( aIndex, aSubCellTemplate.iRectAccordingToParent.iTl);
	data->SetGraphicsSubCellL( aIndex, aSubCellTemplate.iIsImage );
	data->SetSubCellAlignmentL( aIndex, aSubCellTemplate.iTextAlign );
	const CFont* font = AknLayoutUtils::FontFromId(aSubCellTemplate.iFontId);
	data->SetSubCellFontL (aIndex, font);
	data->SetSubCellBaselinePosL( aIndex,
			CAknLayoutFont::AsCAknLayoutFontOrNull( font )->TextPaneTopToBaseline()
			+ aSubCellTemplate.iRectAccordingToParent.iTl.iY );

//	If some text is clipped then marquee will do the drawing right after the subcell is drawn by ListBoxData.
//	Setting the subcell size to zero prevents ListBoxData from drawing the text.
	if (!aSubCellTemplate.iIsImage
			&& !iIsMarqueeBeingDrawn
			&& ( aItemIndex == iWidget->View()->CurrentItemIndex() )
			&& !IsEditMode()
			&& iWidgetType == EListbox
			&& iSubcellText
            && iMarqueeAdapter->IsMarqueeEnabled() )

		{
		if (iMarqueeAdapter && IsTextClippedL( aSubCellTemplate, *iSubcellText )
			&& STATIC_CAST(CMmWidgetContainer*,Widget()->Parent())->IsHighlightVisible() )
		    {
		    data->SetSubCellSizeL( aIndex, TSize(0,0));
    		const_cast<CMmListBoxItemDrawer*>(this)->AddSubcellMarqueeElementL(
    				aSubCellTemplate, aIndex, aItemIndex);
		    }
		else if (iMarqueeAdapter)
            {
            iMarqueeAdapter->StopMarqueeDrawing(aIndex);
            }
		}

	if ( iMarqueeAdapter && iIsMarqueeBeingDrawn
    		&& iMarqueeAdapter->SubcellMarqueeElementExists( aIndex ) )
    	{
    	if (aItemIndex != iWidget->View()->CurrentItemIndex() )
            {
            iMarqueeAdapter->StopMarqueeDrawing(aIndex);
            }
    	else
    	    {
    	    data->SetSubCellSizeL(aIndex, TSize(0, 0));
    	    }
        }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CFormattedCellListBoxData::TColors CMmListBoxItemDrawer::SetupColors(  TBool aDragged ) const
	{
	CFormattedCellListBoxData::TColors colors;
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();

	if ( !aDragged )
	    {
        AknsUtils::GetCachedColor( skin, colors.iText, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG9 );
        AknsUtils::GetCachedColor( skin, colors.iBack , KAknsIIDQsnTextColors,
                EAknsCIQsnOtherColorsCG6 );
        AknsUtils::GetCachedColor( skin, colors.iHighlightedText,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG11 );
        AknsUtils::GetCachedColor( skin, colors.iHighlightedBack,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
	    }
	else
	    {
        AknsUtils::GetCachedColor( skin, colors.iText, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG11 );
        AknsUtils::GetCachedColor( skin, colors.iBack , KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6 );
        AknsUtils::GetCachedColor( skin, colors.iHighlightedText,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG11 );
        AknsUtils::GetCachedColor( skin, colors.iHighlightedBack,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
	    }

	return colors;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxItemDrawer::GetItemHeight( TInt aItemIndex, TBool aItemIsCurrent ) const
    {
    TSize ret(TInt(0),TInt(0));
    ret = GetItemSize(aItemIndex, aItemIsCurrent);
    return ret.iHeight;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxItemDrawer::GetFloatingItemCount()
    {
	for( TInt i=0; i< iFloatingItems.Count(); i++)
		{
		TMmFloatingItem& current = GetFloatingItemAtIndex(i);
		if (current.GetDrawnItemIndex() == KErrNotFound)
			{
			RemoveFloatingItem(i);
			}
		}

    return iFloatingItems.Count();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TMmFloatingItem& CMmListBoxItemDrawer::GetFloatingItemAtIndex( TInt aIndex )
    {
    return iFloatingItems[ aIndex ];
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::RemoveFloatingItem( TInt aPosition )
    {
    if (aPosition != KErrNotFound)
    	{
    	iFloatingItems.Remove( aPosition );
    	}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::RemoveFloatingItems()
    {
    iFloatingItems.Reset();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AddFloatingItemL( TMmFloatingItem& aFloatingItem,
        TInt aPosition )
    {
    EnableCachedDataUse( EFalse );
    if (aPosition != KErrNotFound)
        {
        iFloatingItems.InsertL( aFloatingItem, aPosition );
        }
    else
        {
        iFloatingItems.AppendL( aFloatingItem );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TSize CMmListBoxItemDrawer::GetItemSize( TInt aItemIndex, TBool aItemIsCurrent ) const
    {
    if ( aItemIndex < 0 )
        {
        return TSize( 1, 1 );
        }

	TSize size;

	CMmCacheForItem* cache = NULL;
	TRAPD( cacheError, cache = iItemsDataCache->GetItemCacheL( aItemIndex ) );
	if ( cacheError != KErrNone )
	    {
	    cache = NULL;
	    }

	if ( cache )
	    {
	    cache->InvalidateIfCacheMayNotBeUsed(
	            aItemIsCurrent, iLastSubcellsSetupCode );
	    }

    if ( !iUseCache || !cache || !cache->IsValid() )
        {
        const TDesC8& mm_template = iMmModel->GetAttributeAsText (aItemIndex,
                KMmTemplate8);
        TRect viewRect = iWidget->View()->ViewRect();
        TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();

        if ( iTemplateLibrary->GetSize( size, iWidgetType, mm_template,
                landscapeOrientation, aItemIsCurrent, viewRect )
                != KErrNone )
            {
            size = TSize( 1, 1 );
            }
        if ( cache )
            {
            cache->SetSize( size );
            }
        }
    else
        {
        size = cache->GetSize();
        }
    return size;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TRect CMmListBoxItemDrawer::GetIndicatorRect() const
    {
    TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
    const TDesC8& mm_template = iMmModel->GetAttributeAsText(
            iWidget->View()->CurrentItemIndex(), KMmTemplate8 );
    TRect indicatorRect( iTemplateLibrary->GetMoveIndicatorRect(
            iWidgetType, mm_template, landscapeOrientation, ETrue ) );
    return indicatorRect;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetDraggableL( TBool aDraggable )
    {

	iDraggable = aDraggable;

    if (!iDraggable)
        {
        for(int i=0; i< iFloatingItems.Count(); i++)
            {
            if (iFloatingItems[i].GetFloatingItemType() == EZoomTransition
                    && iFloatingItems[i].GetZoomingStatus() > 0)
                {
                AnimateItemZoomOutL( iFloatingItems[i].GetDrawnItemIndex() );
                }
            else if (iFloatingItems[i].GetFloatingItemType() == EDrag
                    || iFloatingItems[i].IsManualDelete())
                {
                iFloatingItems[i].InvalidateFloatingItem();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetEditModeL( TBool aEditMode )
    {
    if ( aEditMode )
        {
        EnableCachedDataUse( EFalse );
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal *trans = CAknListLoader::TfxApiInternal( iGc );
    if( trans )
        {
        trans->Remove( MAknListBoxTfxInternal::EListEverything );
        trans->Draw( iViewRect );
        }
#endif
    iIsEditMode = aEditMode;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::IsEditMode() const
	{
	return iIsEditMode;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmDrawerAnimator* CMmListBoxItemDrawer::GetAnimator()
    {
    return iAnimator;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetDraggedPointL( TPoint aPoint )
	{
	TInt dragFloatingItem = GetFloatingItemIndex(EDrag);
	if (dragFloatingItem != KErrNotFound )
		{
		TMmFloatingItem & item = GetFloatingItemAtIndex( dragFloatingItem );
		TMmFloatingItem floatingItem( item.GetDrawnItemIndex(),
				aPoint,	EDrag, MmEffects::KNoAnimationFramesCount, NULL );
		floatingItem.SetManualDelete( ETrue );

		TMmFloatingItem postDragRefresh( item.GetDrawnItemIndex(),
						item.GetItemPosition(),	EPostDragRefreshItem,
						MmEffects::KNoAnimationFramesCount, iWidget->View() );

		iFloatingItems.Remove(dragFloatingItem);

		if (postDragRefresh.GetItemPosition() != floatingItem.GetItemPosition())
			{
			iFloatingItems.Append( postDragRefresh );
			}
		iFloatingItems.Insert( floatingItem, 0 );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetDraggedIndexL(TInt aDraggedItemIndex,
		TPoint aPoint)
    {
    TInt dragFloatingItem = KErrNotFound;
    do
    	{
	    dragFloatingItem = GetFloatingItemIndex(EDrag);
	    if (dragFloatingItem != KErrNotFound)
	    	{
	    	TMmFloatingItem & item = GetFloatingItemAtIndex( dragFloatingItem );

	    	TMmFloatingItem postDragRefresh( item.GetDrawnItemIndex(),
				item.GetItemPosition(),	EPostDragRefreshItem,
				MmEffects::KNoAnimationFramesCount, iWidget->View() );

	    	if (postDragRefresh.GetItemPosition() != aPoint)
	    		{
	    		iFloatingItems.Append( postDragRefresh );
	    		}
	    	}

		RemoveFloatingItem( dragFloatingItem );

    	}
	while ( dragFloatingItem != KErrNotFound );

    if ( aDraggedItemIndex != KErrNotFound )
    	{
		TMmFloatingItem floatingItem( aDraggedItemIndex, aPoint, EDrag,
				MmEffects::KNoAnimationFramesCount, iWidget->View() );
		floatingItem.SetManualDelete( ETrue );
		AddFloatingItemL(floatingItem, 0);

	    ClearFlags( CListItemDrawer::EPressedDownState );
    	}

    iAnimator->Trigger();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::IsDraggable() const
    {
    return iDraggable && iWidget->View();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetBgContext(
		CAknsBasicBackgroundControlContext * aBgContext )
	{
	iBgContext = aBgContext;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetScrollbarVisibilityL( TBool aIsScrollbarVisible )
	{
	iTemplateLibrary->SetScrollbarVisibilityL( aIsScrollbarVisible );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TRect CMmListBoxItemDrawer::AdjustItemRect( TInt aItemIndex ) const
	{
	TSize size = iWidget->View()->ItemSize (iWidget->View()->CurrentItemIndex () );
	TRect rect(iWidget->View()->ItemPos (iWidget->View()->CurrentItemIndex () ),
			iWidget->View()->ItemPos (iWidget->View()->CurrentItemIndex () ) + size);
	if ( !AknLayoutUtils::PenEnabled () && iIsIndicatorItem)
		{
		TBool landscapeOrientation =
				Layout_Meta_Data::IsLandscapeOrientation ();
		const TDesC8& mm_template = iMmModel->GetAttributeAsText (
				0, KMmTemplate8);
		TRect relativeToParentRect = TRect (TPoint (0, 0), TPoint (0, 0));
		relativeToParentRect = iTemplateLibrary->GetMoveIndicatorRect(
						iWidgetType, mm_template, landscapeOrientation,
						aItemIndex == iWidget->View()->CurrentItemIndex () );
		rect.Move (relativeToParentRect.iTl);
		rect.iBr.iX = rect.iBr.iX + relativeToParentRect.Width ();
		rect.iBr.iY = rect.iBr.iY + relativeToParentRect.Height ();
		}
	return rect;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DrawActualIndicatorItem( TInt aItemIndex, TRect /*actualItemRect*/ )
	{
	if ( iDrawMoveIndicators )
		{
		iIsIndicatorItem = ETrue;
		SetRedrawItemBackground( EFalse );

		DrawActualItem( aItemIndex, AdjustItemRect( aItemIndex ) , EFalse, EFalse, EFalse, EFalse);

		SetRedrawItemBackground( ETrue );
		iIsIndicatorItem = EFalse;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::ReplaceSubCellText( const TDesC& aText )
	{
	delete iSubcellText;
	iSubcellText = NULL;
	if( aText.Compare( KNullDesC() ) )
		{
		iSubcellText = aText.Alloc();
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AddSubcellMarqueeElementL( TTemplateChild aSubCellTemplate, TInt aIndex, TInt aItemIndex )
	{
	TRgb textColor;
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsUtils::GetCachedColor( skin, textColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 );
	iMarqueeAdapter->AddMarqueeElementL( aSubCellTemplate.iRectAccordingToParent, *iSubcellText,
			aSubCellTemplate.iFontId, textColor, aSubCellTemplate.iTextAlign, aIndex,
			aSubCellTemplate.iRectAccordingToParent.iTl.iY,
			aItemIndex);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter )
	{
	iMarqueeAdapter = aAdapter;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetMarqueeDrawing( TBool aIsMarqueeBeingDrawn )
	{
	iIsMarqueeBeingDrawn = aIsMarqueeBeingDrawn;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::IsTextClippedL( TTemplateChild aTemplateChild,
		const TDesC& aText ) const
	{
    TBuf< MmMarqueeConstants::KClippingBufLength > clipbuf
		= aText.Left( MmMarqueeConstants::KTextTrimmingThreshold);
	TInt maxClipWidth = aTemplateChild.iRectAccordingToParent.Width();
	const CFont* font = AknLayoutUtils::FontFromId(aTemplateChild.iFontId);
    return AknBidiTextUtils::ConvertToVisualAndClipL( clipbuf, *font,
    		aTemplateChild.iRectAccordingToParent.Width(), maxClipWidth );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetNumberOfColsInView(TInt aNumberOfColumns)
	{
	iNumberOfColsInWidget = aNumberOfColumns;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupIconSubcellL(
		RArray<TTemplateChild>& aTemplateChildArray, TInt aChildIndex, TInt aItemIndex,
		RBuf& aItemText, TInt& aSubcellIncrement ) const
		{
		CGulIcon* icon = NULL;
		TTemplateChild child = aTemplateChildArray[aChildIndex];
		if ( !IsEditMode() &&
		        child.iImageVisualId == EImageVisualIdEditMode )
		    {
		    child.iIsImage = EFalse;
		    }

		if ( IsDraggable() && aItemIndex == iZoomIconIndex )
			{
			const TReal KNormalZoomRatio = 1.0;
			TReal zoomDelta = ( iIconAnimationZoomRatio - KNormalZoomRatio ) / 2.0;
			TSize size = child.iRectAccordingToParent.Size();
			TSize sizeDelta( size.iWidth * zoomDelta, size.iHeight * zoomDelta );
			child.iRectAccordingToParent.Grow( sizeDelta );
			}

		TSize targetSize = child.iRectAccordingToParent.Size();

		CHnIconHolder* iconHolder = iMmModel->GetAttributeAsRefCountedGraphics(
                aItemIndex, child.iData, &targetSize );
        icon = iconHolder ? iconHolder->GetGulIcon() : NULL;
        if ( icon )
            {
            CFbsBitmap* bitmap = icon->Bitmap();
            ASSERT( bitmap );
            //resize the item if it is a move indicator
            if( iIsIndicatorItem )
                {
                AknIconUtils::SetSize( bitmap, child.iRectAccordingToParent.Size(),
                		EAspectRatioNotPreserved );
                }
            else
                {
                TSize bmpSize = bitmap->SizeInPixels();
                TBool setSizeRequired = bitmap->DisplayMode() == ENone;
                if ( targetSize.iWidth && targetSize.iHeight &&
                        ( setSizeRequired || !BitmapFitsIntoTarget( bmpSize, targetSize ) ) )
                    {
                    CFbsBitmap* mask = icon->Mask();
                    if ( mask )
                        {
                        __ASSERT_DEBUG( bmpSize == mask->SizeInPixels(), User::Invariant() );
                        AknIconUtils::SetSize( mask, targetSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
                        }
                    AknIconUtils::SetSize( bitmap, targetSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
                    }
                }

            TInt iconIndex = iItemsDataCache->GetItemCacheL( aItemIndex )->AppendIconL( iconHolder );

            HBufC8* number = HnConvUtils::NumToStr8LC( iconIndex );
            const TInt newLength = aItemText.Length() + number->Length();
            if( aItemText.MaxLength() < newLength )
                {
                aItemText.ReAllocL( newLength );
                }
            CleanupStack::PopAndDestroy( number );
            aItemText.AppendNum( iconIndex );

    	    SetupSubCellL( child, aSubcellIncrement, aItemIndex );

        	if ( aChildIndex < aTemplateChildArray.Count() - 1 )
        	    {
	            const TInt newLength = aItemText.Length() + KTab().Length();
                if( aItemText.MaxLength() < newLength )
                    {
                    aItemText.ReAllocL( newLength );
                    }
        	    aItemText.Append( KTab );
        	    }

        	aSubcellIncrement++;
        	iLastSubcellsSetupCode.AddSubcellInfo( TMmSubcellsSetupCode::EGraphicsSubcell );
        	}
        else
            {
            // Mark the fact that subcell was not set to ensure that TMmSubcellsSetupCode
            // works properly.
            iLastSubcellsSetupCode.AddSubcellInfo( TMmSubcellsSetupCode::ESkippedSubcell );
            }
		}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::BitmapFitsIntoTarget(
        TSize aBmpSize, TSize aTargetSize ) const
    {
    TBool widthLessOrEqual = aBmpSize.iWidth <= aTargetSize.iWidth;
    TBool heightLessOrEqual = aBmpSize.iHeight <= aTargetSize.iHeight;
    TBool widthAlmostEqual = Abs( aBmpSize.iWidth - aTargetSize.iWidth ) < 2;
    TBool heightAlmostEqual = Abs( aBmpSize.iHeight - aTargetSize.iHeight ) < 2;
    return ( widthLessOrEqual && heightAlmostEqual ) ||
           ( widthAlmostEqual && heightLessOrEqual );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupTextSubcellL(
		RArray<TTemplateChild>& aTemplateChildArray, TInt aChildIndex,
		TInt aItemIndex, RBuf& aItemText, TInt& aSubcellIncrement ) const
		{
		TTemplateChild child = aTemplateChildArray[aChildIndex];
		RBuf itemChildText;
		CleanupClosePushL( itemChildText );
		const TDesC8& mmTitleDes8 = iMmModel->GetAttributeAsText( aItemIndex,
				child.iData  );
		itemChildText.Assign( HnConvUtils::Str8ToStr( mmTitleDes8 ) );

		AppendText( aItemText, itemChildText );

		const_cast<CMmListBoxItemDrawer*>(this)->ReplaceSubCellText(
				itemChildText );
		CleanupStack::PopAndDestroy( &itemChildText );

	    SetupSubCellL( child, aSubcellIncrement, aItemIndex );

    	if ( aChildIndex < aTemplateChildArray.Count() - 1 )
    	    {
    	    AppendText( aItemText, KTab );
    	    }

    	aSubcellIncrement++;
    	iLastSubcellsSetupCode.AddSubcellInfo( TMmSubcellsSetupCode::ETextSubcell );
		}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::AppendText( RBuf& aBuffer, const TDesC& aTextToAppend ) const
	{
		TInt newLength = aBuffer.Length() + aTextToAppend.Length();
		TInt error = KErrNone;

   		if( aBuffer.MaxLength() < newLength )
   		    {
   		    error = aBuffer.ReAlloc( newLength );
   		    }
   		if ( error == KErrNone )
   			{
   			aBuffer.Append( aTextToAppend );
   			}
	}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupBackdropSubcellL(
		RArray<TTemplateChild>& aTemplateChildArray, TInt aItemIndex,
		RBuf& aItemText, TInt& aSubcellIncrement ) const
	{
    CGulIcon* icon = NULL;
    for ( TInt i = 0; i < aTemplateChildArray.Count() /*&&
            aItemIndex != iWidget->View()->CurrentItemIndex()*/; ++i )
        {
        TTemplateChild child = aTemplateChildArray[i];
        if( child.iImageVisualId == EImageVisualIdEditMode  &&
                child.iIsImage )
            {
            TSize itemSize = GetItemSize( aItemIndex,
            		aItemIndex == iWidget->View()->CurrentItemIndex() );
            CHnIconHolder* iconHolder = iMmModel->GetAttributeAsRefCountedGraphics( aItemIndex, child.iData, &itemSize );
            icon = iconHolder ? iconHolder->GetGulIcon() : NULL;
            if ( icon )
                {
                CFbsBitmap* bitmap = icon->Bitmap();
				child.iRectAccordingToParent = TRect( TPoint( 0, 0 ),
						TPoint( itemSize.iWidth, itemSize.iHeight ) );
				AknIconUtils::SetSize( bitmap, itemSize,
						EAspectRatioNotPreserved );
				TInt iconIndex = iItemsDataCache->GetItemCacheL( aItemIndex )->AppendIconL( iconHolder );

                HBufC8* number = HnConvUtils::NumToStr8LC( iconIndex );
                TInt newLength = aItemText.Length() + number->Length();
                if( aItemText.MaxLength() < newLength )
                    {
                    aItemText.ReAllocL( newLength );
                    }
                CleanupStack::PopAndDestroy( number );
                aItemText.AppendNum( iconIndex );

                newLength = aItemText.Length() + KTab().Length();
                if( aItemText.MaxLength() < newLength )
                    {
                    aItemText.ReAllocL( newLength );
                    }
                aItemText.Append( KTab );
                SetupSubCellL( child, aSubcellIncrement, aItemIndex );
                aSubcellIncrement++;
                iLastSubcellsSetupCode.AddSubcellInfo( TMmSubcellsSetupCode::EBackdropSubcell );
                }
            }
        }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetupSubCellsL( TBool aItemIsCurrent,
        TInt aItemIndex ) const
    {
    CMmCacheForItem* cache = iItemsDataCache->GetItemCacheL( aItemIndex );
    cache->MarkAsInvalid();

    const TDesC8& mmTemplate = iMmModel->GetAttributeAsText( aItemIndex, KMmTemplate8 );
    if ( !mmTemplate.Compare( KNullDesC8 ) || !mmTemplate.Compare( KEmpty8 ) )
        {
        User::Leave( KErrNotFound );
        }
    cache->SetTemplateL( mmTemplate );

    cache->ClearIconArray();
    cache->SetCurrent( aItemIsCurrent );

    TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();

    RBuf& itemText = cache->GetItemTextForModifications();
    itemText.Close();
    itemText.CreateL( MmTemplateContants::KItemSubCellsText );

    RArray<TTemplateChild> templateChildArray;
    CleanupClosePushL( templateChildArray );
    if ( !iIsIndicatorItem )
		{
		iTemplateLibrary->GetChildrenL( iWidgetType, templateChildArray, mmTemplate,
				landscapeOrientation, aItemIsCurrent,	IsEditMode() );
		}
    else if ( !AknLayoutUtils::PenEnabled() )
		{
		iTemplateLibrary->GetMoveIndicatorChildrenL( iWidgetType, templateChildArray,
	    		mmTemplate, landscapeOrientation, aItemIsCurrent );
		}

    iLastSubcellsSetupCode.Clear();

    //Backdrop icon as first element to draw
    TInt subcellIncrement( 0 );
    if ( GetBackdropVisibility( aItemIndex, aItemIsCurrent ) )
        {
        SetupBackdropSubcellL( templateChildArray, aItemIndex, itemText, subcellIncrement );
        iItemHasBackdrop = ETrue;
        }
    else
    	{
    	iItemHasBackdrop = EFalse;
    	}

	for ( TInt i( 0 ) ; i < templateChildArray.Count() && !iLeftOverAreaUnderAnimatedItem; i++ )
		{
		TTemplateChild child = templateChildArray[i];
        if ( child.iImageVisualId == EImageVisualIdEditMode && child.iIsImage )
            {
            continue;
            }
        else if( !child.iIsImage )
    		{
    		SetupTextSubcellL( templateChildArray, i, aItemIndex, itemText, subcellIncrement );
    		}
    	else
    		{
    		SetupIconSubcellL( templateChildArray, i, aItemIndex, itemText, subcellIncrement );
    		}
		}
	TInt subcellsJustSet = subcellIncrement;
	iCurrentNumberOfSubcellsSet = Max( iCurrentNumberOfSubcellsSet, subcellsJustSet );
	__ASSERT_DEBUG( iCurrentNumberOfSubcellsSet <= MmTemplateContants::KSubCellsCount,
	        User::Invariant() );

	for ( TInt i = subcellIncrement; i < iCurrentNumberOfSubcellsSet; i++ )
	    {
	    SetupSubNoCellL( i, aItemIndex );
	    }

	iLastSubcellsSetupCode.AddTemplateInfo(
	        iItemsDataCache->GetTemplateIdentifierL( mmTemplate ) );
	iLastSubcellsSetupCode.AddIsCurrentInfo( aItemIsCurrent );

	CleanupStack::PopAndDestroy( &templateChildArray );

	cache->SetSubcellsSetupCode( iLastSubcellsSetupCode );
	// the line below is here only to make the cached information complete
	GetItemSize( aItemIndex, aItemIsCurrent );
	cache->MarkAsValidL();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMmTemplateLibrary* CMmListBoxItemDrawer::TemplateLibrary()
    {
    return iTemplateLibrary;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::SetHighlightShown( TBool aDrawn )
	{
	iHighlightShown = aDrawn;
	iDrawMoveIndicators = aDrawn;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::IsFloating( TInt aItemIndex ) const
    {
    TBool isFloating( EFalse );

    TInt index = GetFloatingItemIndex( EDrag );
    if (index == KErrNotFound)
    	{
    	index = GetFloatingItemIndex( EDragTransition );
    	}

    if ( KErrNotFound != index )
        {
        TMmFloatingItem& current = const_cast<CMmListBoxItemDrawer*>(this)->GetFloatingItemAtIndex( index );
        TInt drawnIndex = current.GetDrawnItemIndex();
        if ( drawnIndex == aItemIndex )
            {
            isFloating = ETrue;
            }
        }
    return isFloating;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxItemDrawer::DrawActualItem(TInt aItemIndex,
		const TRect& aActualItemRect, TBool aItemIsCurrent,
		TBool aViewIsEmphasized, TBool /*aViewIsDimmed*/,
		TBool aItemIsSelected) const
		{
		if (Properties(aItemIndex).IsSelectionHidden())
			{
			aItemIsSelected = EFalse;
			}
		DrawItemText(aItemIndex,aActualItemRect,aItemIsCurrent,
				aViewIsEmphasized,aItemIsSelected);
		}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::GetHighlightVisibility( TInt aItemIndex,
		TBool aItemIsCurrent, TBool aAllowHighlightForNonDraggedItem ) const
	{
	TBool highlightVisibility(EFalse);
	if (!iItemHasBackdrop && !iLeftOverAreaUnderAnimatedItem ) //never draw highlight when item has backdrop or when left over area under animated item
		{
		TBool currentlyDraggedItem =
			ItemHasFloatingType( aItemIndex, EDrag ) ||
			ItemHasFloatingType( aItemIndex, EDragTransition );

		if ( ( STATIC_CAST(CMmWidgetContainer*,Widget()->Parent())->IsHighlightVisible()
				&& aItemIsCurrent && aAllowHighlightForNonDraggedItem )
				|| currentlyDraggedItem )
			{
			highlightVisibility = ETrue;
			}
		}
	return highlightVisibility;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::GetBackdropVisibility( TInt aItemIndex,
		TBool aItemIsCurrent ) const
	{
	TBool currentlyDraggedItem =
		ItemHasFloatingType( aItemIndex, EDrag ) ||
		ItemHasFloatingType( aItemIndex, EDragTransition );

    return 	IsEditMode() /* draw the backdrop only in edit mode */
    		&& !currentlyDraggedItem /* backdrop is disabled for dragged items */
    		&& !iIsIndicatorItem /* in non-touch backdrop is not drawn, just "move indicators" */
    		&& !iLeftOverAreaUnderAnimatedItem /* is the currently drawn item the area left over behind dragged item*/
    		&& !( STATIC_CAST( CMmWidgetContainer*,Widget()->Parent() )->IsHighlightVisible()
    				&& aItemIsCurrent );/*if the timer is active then we want to draw highlight (not backdrop) on the current index*/
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmListBoxItemDrawer::ItemHasFloatingType( TInt aItemIndex,
		TMmFloatingItemType aFloatingType) const
	{
	TBool hasFloatingType( EFalse );
	for ( TInt i = iFloatingItems.Count() - 1 ; i >= 0; i-- )
        {
		const TMmFloatingItem& item = iFloatingItems[i];
        if ( item.GetFloatingItemType() == aFloatingType
        		&& item.GetDrawnItemIndex() == aItemIndex )
            {
            hasFloatingType = ETrue;
            break;
            }
        }
	return hasFloatingType;
	}
// End of file
