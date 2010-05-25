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
* Description:  CMmListBoxItemDrawer
*  Version     : %version: MM_38.1.18 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMLISTBOXITEMDRAWER_H
#define C_MMLISTBOXITEMDRAWER_H

#include <e32std.h>
#include <e32base.h>
#include <AknsBasicBackgroundControlContext.h>
#include <eikfrlbd.h>
#include <AknDef.hrh>
#include "mmwidgetsconstants.h"
#include "mmtemplatelibrary.h"
#include "hniconholder.h"
#include "mmsubcellssetupcode.h"

class CMmListBoxModel;
class TMmFloatingItem;
class CMmMarqueeAdapter;
class CMmDrawerAnimator;
class CMmPostEvaluationProcessor;
class CMmItemsDataCache;

/**
 * Used to draw items in widgets. The item drawer is used both by grid
 * and listbox. Data is used to draw subcells. The subcell templates are fetched
 * from the template library and set to data to be drawn.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmListBoxItemDrawer ):
    public CFormattedCellListBoxItemDrawer
    {

    friend class CMmPostEvaluationProcessor;

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.0
     * @param aMmListBoxModel The listbox model.
     * @param aFont Font.
     * @param aFormattedCellData Cell data.
     * @param aWidgetType Widget type.
     */
  static CMmListBoxItemDrawer* NewL(
          CMmListBoxModel* aMmListBoxModel,
        const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData,
        TMmWidgetType aWidgetType,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.0
     * @param aMmListBoxModel The listbox model.
     * @param aFont Font.
     * @param aFormattedCellData Cell data.
     * @param aWidgetType Widget type.
     */
  static CMmListBoxItemDrawer* NewLC(
          CMmListBoxModel* aMmListBoxModel,
        const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData,
        TMmWidgetType aWidgetType,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Destructor.
     *
     * @since S60 v3.0
     */
    ~CMmListBoxItemDrawer();

    /**
     * Enables/disables cached data use.
     *
     * When aEnable is true:
     * Enables the use of cached data when drawing items.
     * This significantly improves performance.
     * Do not ever try to use this feature in edit mode.
     *
     * When aEnable is false:
     * Restores the normal mode where data needed for drawing
     * items is retrieved from the model and the cached data
     * is updated with the information retrieved from the model
     *
     * @param aEnable Self-explanatory.
     */
    void EnableCachedDataUse( TBool aEnable );

    /**
     * Returns ETrue if item drawer is using cached data.
     * To start/stop using cached data, use the @c EnableCachedDataUse
     * method.
     *
     * @return ETrue if cached data is used, EFalse otherwise.
     */
    TBool CachedDataUseIsEnabled() const;

    /**
     * This method should be called whenever items(s) are removed
     * in order to remove corresponding entries in the local
     * items data cache.
     *
     * @param aItemCount current item count
     */
    void TrimCacheSize( TInt aItemCount );

    /**
     * Invalidates the cached data for all items.
     */
    void InvalidateCache();

    /**
     * Get the height of the given item.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index for which height is calculated.
     * @param aItemIsCurrent Is item the current widget item.
     * @return Item height.
     */
    TInt GetItemHeight( TInt aItemIndex, TBool aItemIsCurrent ) const;

    /**
     * Gets size of given item.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index.
     * @param aItemIsCurrent Is item the current widget item.
     * @return Size of item.
     */
    TSize GetItemSize( TInt aItemIndex, TBool aItemIsCurrent ) const;

    /**
     * Gets rect of indicator.
     *
     * @since S60 v3.0
     * @return Rect of indicator.
     */
    TRect GetIndicatorRect() const;

    /**
     * Setx draggable state
     *
     * @since S60 v3.0
     * @param aDraggable Is draggable.
     */
    void SetDraggableL( TBool aDraggable );

    /**
     * Sets item which is dragged.
     *
     * @since S60 v3.0
     * @param aDraggedItemIndex Dragged item index.
     * @param aPoint Dragged point.
     */
    void SetDraggedIndexL( TInt aDraggedItemIndex, TPoint aPoint );

    /**
     * Sets item which is dragged.
     *
     * @since S60 v3.0
     * @param aCenterPoint Center point of dragged item.
     */
    void SetDraggedPointL( TPoint aCenterPoint );

    /**
     * Reveales if is possible to drag.
     *
     * @since S60 5.0
     * @return Is draggable.
     */
    TBool IsDraggable() const;

    /**
     * Sets associated widget.
     *
     * @since S60 5.0
     * @param aView Widget to associated with this drawer.
     */
    void SetView( CEikListBox* aView );

    /**
     * Get associated widget.
     *
     * @since S60 5.0
     * @return Widget associated with this drawer.
     */
    CEikListBox* Widget() const;

    /**
     * Gets template library.
     *
     * @since S60 5.0
     * @return Return iTemplateLibrary.
     */
    CMmTemplateLibrary* TemplateLibrary();

    /**
     * Sets the background context.
     *
     * @since S60 5.0
     * @param aBgContext Background context.
     */
  void SetBgContext(
      CAknsBasicBackgroundControlContext* aBgContext );

    /**
     * Sets condition if background should be redrawn.
     *
     * @since S60 v3.0
     * @param aRedraw Should background be redrawn.
     */
    void SetRedrawItemBackground( TBool aRedraw );

    /**
     * Sets condition if separator line should be drawn.
     *
     * @since S60 v3.0
     * @param aRedraw Should separator line be drawn.
     */
    void SetDrawSeparatorLines( TBool aDraw );

    /**
     * Checks if item background redrawing is enabled.
     *
     * @since S60 v3.0
     * @return Is background redrawing enabled.
     */
    TBool IsRedrawItemBackgroundEnabled() const;

    /**
     * Draws empty item ( no subcells ).
     *
     * @since S60 v3.0
     * @param aItemIndex Item index.
     * @param aItemRectPos Item rectangle position.
     * @param aViewIsDimmed Is view dimmed.
     */
    void DrawEmptyItem( TInt aItemIndex,
            TPoint aItemRectPos,  TBool aViewIsDimmed ) const;

    /**
     * Sets edit mode condition.
     *
     * @since S60 v3.0
     * @param aIsEditMode Is edit mode.
     */
    void SetEditModeL( TBool aIsEditMode );

    /**
     * Checks if edit mode is running.
     *
     * @since S60 v3.0
     * @return Is edit mode running.
     */
    TBool IsEditMode() const;

    /**
     * Animates dragged item transition.
     *
     * @since S60 v3.0
     */
    void AnimateDragItemTransitionL( );

    /**
     * Animates swapping of items.
     *
     * @since S60 v3.0
     * @param aItemFrom Start item position (index).
     * @param aItemTo End item position (index).
     */
    void AnimateItemSwapL( TInt aItemFrom, TInt aItemTo );

    /**
     * Animate item zoom in.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index of item to be zoomed in.
     */
    void AnimateItemZoomInL( TInt aItemIndex );

    /**
     * Animate item zoom out.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index of zoomed item.
     */
    void AnimateItemZoomOutL( TInt aItemIndex );

    /**
     * Checks if item drawer is ready to run new animation.
     *
     * @since S60 v3.0
     * @return Is drawer ready to run animation.
     */
    CMmDrawerAnimator* GetAnimator();

    /**
     * Draws floating items.
     *
     * @since S60 v3.0
     * @param aCurrentlyDrawnRect Currently drawn item rectangle.
     */
    void DrawFloatingItems( TRect aCurrentlyDrawnRect );

    /**
     * Gets floating item index.
     *
     * @since S60 v3.0
     * @param aType Floating item type.
     * @return Floating item index.
     */
    TInt GetFloatingItemIndex( TMmFloatingItemType aType ) const;

    /**
     * Gets floating item.
     *
     * @since S60 v3.0
     * @param aType Floating item type.
     * @return Floating item.
     */
    TMmFloatingItem& GetFloatingItemL( TMmFloatingItemType aType );

    /**
     * Gets number of floating items.
     *
     * @since S60 5.0
     * @param aType Floating item type.
     * @return Number of floating items.
     */
    TInt GetValidFloatingItemCount( TMmFloatingItemType aType );

    /**
     * Gets floating items count.
     *
     * @since S60 5.0
     * @return Number of the floating items in the array.
     */
    TInt GetFloatingItemCount();

    /**
     * Adds floating item to the floating item array.
     *
     * @since S60 5.0
     * @param aFloatingItem Floating item to be added.
     * @param aPosition Position to add at.
     */
    void AddFloatingItemL( TMmFloatingItem& aFloatingItem,
            TInt aPosition = KErrNotFound );

    /**
     * Removes floating item.
     *
     * @since S60 5.0
     * @param aPosition Floating item index to be removed.
     */
    void RemoveFloatingItem( TInt aPosition );

    /**
     * Removes all floating items.
     *
     * @since S60 5.0
     */
    void RemoveFloatingItems();

    /**
     * Gets floating item at particular index in the
     * floating item array.
     *
     * @since S60 5.0
     * @param aIndex Index of the floating item in the array.
     * @return Floating item.
     */
    TMmFloatingItem& GetFloatingItemAtIndex( TInt aIndex );

    /**
     * Used to set scrollbar visibility flag.
     *
     * @since S60 v3.0
     * @param aIsScrollbarVisible Is scrollbar visible.
     */
    void SetScrollbarVisibilityL( TBool aIsScrollbarVisible );

    /**
     * Replaces subcell text.
     *
     * @since S60 v3.0
     * @param aText Text which will replace previous text.
     */
    void ReplaceSubCellText( const TDesC& aText );

    /**
     * Adds subcell marquee element.
     *
     * @since S60 v3.0
     * @param aSubCellTemplate Subcell template data.
     * @param aIndex Subcell index.
     * @param aItemIndex Item index.
     */
    void AddSubcellMarqueeElementL( TTemplateChild aSubCellTemplate,
            TInt aIndex, TInt aItemIndex );

    /**
     * Sets the marquee adapter.
     *
     * @since S60 v3.0
     * @param aAdapter Marquee adapter.
     */
    void SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter );

    /**
     * Sets the flag which tells that marquee is currently being drawn.
     *
     * @since S60 v3.0
     * @param aIsMarqueeBeingDrawn Is marquee being drawn.
     */
    void SetMarqueeDrawing( TBool aIsMarqueeBeingDrawn );

    /**
     * Checks if text is clipped.
     *
     * @since S60 v3.0
     * @param aTemplateChild Subcell template data.
     * @param aText Text to be clipped.
     * @return Is text clipped.
     */
    TBool IsTextClippedL( TTemplateChild aTemplateChild,
            const TDesC& aText ) const;

    /**
     * Sets the flag which tells that a scrollbar event is running.
     *
     * @since S60 v3.0
     * @param aIsRunning Is scrollbar event running.
     */
  void SetScrollbarEventRunning( TBool aIsRunning );

    /**
     * Sets number of columns in view.
     *
     * @since S60 v3.0
     * @param aNumberOfColumns Number of columns in view.
     */
  void SetNumberOfColsInView( TInt aNumberOfColumns );

  /**
   * Used in non-touch edit mode to block drawing of move indicators and highlight
   * to clean screen for animation.
   *
   * @since S60 v3.0
   * @param aDrawn Should highlight and move indicators be drawn.
   */
  void SetHighlightShown( TBool aDrawn );

    /**
     * Draws background.
     *
     * @since S60 v3.0
     * @param aItemTextRect Item rectangle.
     */
    void DrawBackground( const TRect& aItemTextRect ) const;

protected:
    /**
     * From CListItemDrawer. Draws an item.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index to be drawn.
     * @param aItemRectPos Item rectangle position.
     * @param aItemIsSelected Is item selected.
     * @param aItemIsCurrent Is item current.
     * @param aViewIsEmphasized Is view emphasized.
     * @param aViewIsDimmed Is view dimmed.
     */
    void DrawItem( TInt aItemIndex, TPoint aItemRectPos,
                   TBool aItemIsSelected, TBool aItemIsCurrent,
                   TBool aViewIsEmphasized, TBool aViewIsDimmed ) const;

    /**
     * Draws item according to format text.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index to be drawn.
     * @param aItemTextRect Item rectangle.
     * @param aItemIsCurrent Is item current.
     * @param aViewIsEmphasized Is view emphasized.
     * @param aItemIsSelected Is item selected.
     */
    void DrawItemText( TInt aItemIndex, const TRect& aItemTextRect,
        TBool aItemIsCurrent, TBool aViewIsEmphasized,
        TBool aItemIsSelected ) const;
    /**
     * Draws the actual item contents for the specified item in the specified
     * rectangle.
     *
     * @param aItemIndex Index of the item to draw.
     * @param aActualItemRect Area to draw into.
     * @param aItemIsCurrent @c ETrue if the item is current.
     * @param aViewIsEmphasized @c ETrue if the view is emphasised.
     * @param aViewIsDimmed Ignored
     * @param aItemIsSelected @c ETrue if the item is selected.
     */
    void DrawActualItem(TInt aItemIndex, const TRect& aActualItemRect,
      TBool aItemIsCurrent, TBool aViewIsEmphasized, TBool aViewIsDimmed,
      TBool aItemIsSelected) const;

    /**
     * Sets up item currently drawn item subcells.
     * Also, fills-in/updates the cache information for the item at index
     * aItemIndex.
     *
     * @since S60 v3.0
     * @param aItemIsCurrent Is item the current item in widget.
     * @param aItemIndex Item index.
     */
    void SetupSubCellsL( TBool aItemIsCurrent, TInt aItemIndex ) const;

    /**
     * Sets up subcell to be drawn by listbox data.
     *
     * @since S60 v3.0
     * @param aSubCellTemplate Subcell template data.
     * @param aIndex Subcell index.
     * @param aItemIndex Item index.
     */
    void SetupSubCellL( TTemplateChild aSubCellTemplate, TInt aIndex,
            TInt aItemIndex ) const;

    /**
     * Sets up "null" subcell to be avoided when drawing.
     *
     * @since S60 v3.0
     * @param aIndex Subcell index.
     * @param aItemIndex Item index.
     */
    void SetupSubNoCellL( TInt aIndex, TInt aItemIndex ) const;

    /**
     * Sets up icon for dragged subcell.
     *
     * @since S60 v3.0
     * @param aSubCellTemplate Subcell template data.
     * @param aIndex Subcell index.
     * @param aItemIndex Index of dragged item.
     */
    void SetupSubIcondDragHighlightCellL( TTemplateChild aSubCellTemplate,
            TInt aIndex, TInt aItemIndex ) const;

    /**
     * Adjusts rectangle of item.
     *
     * @since S60 v3.0
     * @param aItemIndex Item's index which adjust rectangle.
     * @return Index of item which rectangle should be adjusted.
     */
    TRect AdjustItemRect( TInt aItemIndex ) const;

    /**
     * Draws the move indicator over the indicated item.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index with move indicator.
     * @param aActualItemRect The actual rectangle of indicator item.
     */
    void DrawActualIndicatorItem( TInt aItemIndex, TRect aActualItemRect );

    /**
     * Sets up colors for subcell data.
     *
     * @since S60 v3.0
     * @return Colors for subcell drawing.
     */
    CFormattedCellListBoxData::TColors SetupColors( TBool aDragged = EFalse ) const;

private:

    /**
     * Default constructor.
     *
     * @since S60 v3.0
     * @param  aMmListBoxModel List box model.
     * @param  aFont Font needed by base constructor.
     * @param  aFormattedCellData Used to format subcell data in item drawer.
     * @param  aWidgetType Type of widgte (list/grid).
     * @param  aTemplateLibrary TemplateLibrary contains items sizes.
     */
    CMmListBoxItemDrawer( CMmListBoxModel* aMmListBoxModel,
            const CFont* aFont,
            CFormattedCellListBoxData* aFormattedCellData,
            TMmWidgetType aWidgetType, CMmTemplateLibrary* aTemplateLibrary );

    /**
     * 2nd phase constructor.
     *
     * @since S60 v3.0
     */
    void ConstructL();

    /**
     *Draws the item text according to the format defined by a text string.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index to be drawn.
     * @param aItemTextRect Item rectangle.
     * @param aItemIsCurrent Is item current.
     * @param aViewIsEmphasized Is view emphasized.
     * @param aItemIsSelected Is item selected.
     */
    void DoDrawItemTextL( TInt aItemIndex, const TRect& aItemTextRect,
        TBool aItemIsCurrent, TBool aViewIsEmphasized,
        TBool aItemIsSelected ) const;

    /**
     * Draws an item.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index to be drawn.
     * @param aItemRectPos Position of item rectangle.
     * @param aItemIsSelected Is item selected.
     * @param aItemIsCurrent Is item current.
     * @param aViewIsEmphasized Is view emphasized.
     * @param aViewIsDimmed Is view dimmed.
     */
    void DoDrawItem(TInt aItemIndex, TPoint aItemRectPos,
        TBool aItemIsSelected, TBool aItemIsCurrent, TBool aViewIsEmphasized,
        TBool aViewIsDimmed) const;

    /**
     * Sets up backdrop image subcell.
     *
     * @since S60 v3.0
     * @param aTemplateChildArray Array of subcell children where the backdrop subcell template is appended.
     * @param aItemIndex Index of currently drawn item.
     * @param aItemText Text used to format item subcell content.
     * @param aSubcellIncrement Is incremented if backdrop subcell template is appended.
     */
    void SetupBackdropSubcellL(
        RArray<TTemplateChild>& aTemplateChildArray, TInt aItemIndex,
            RBuf& aItemText, TInt& aSubcellIncrement ) const;

    /**
     * Sets up icon image subcell.
     *
     * @since S60 v3.0
     * @param aTemplateChildArray Array of subcell children where the backdrop subcell template is appended.
     * @param aChildIndex Index of a template child.
     * @param aItemIndex Index of currently drawn item.
     * @param aItemText Text used to format item subcell content.
     * @param aSubcellIncrement Is incremented if backdrop subcell template is appended.
     */
    void SetupIconSubcellL(
        RArray<TTemplateChild>& aTemplateChildArray, TInt aChildIndex, TInt aItemIndex,
        RBuf& aItemText, TInt& aSubcellIncrement ) const;

    /**
     * Determines whether a bitmap of given size needs scaling to be displayed
     * in a rectangular area of some given target size.
     *
     * This method assumes that the bitmap will always be scaled with aspect
     * ratio preserved. A bitmap does not need scaling if its size meets either
     * of the two conditions:
     * 1. bitmap width is nearly the same* as target area width and bitmap height
     *    is not greater than target height
     * 2. bitmap width is not greater that target width and bitmap height is
     *    nearly the same* as target height.
     * * nearly the same means that it is exactly the same or differs by at most
     * 1 pixel.
     * Please note that a bitmap which is too small (i.e. neither its width or its
     * height is close to the target width/height) will be regarded as one that
     * needs scaling.
     *
     * @param aBmpSize size of the bitmap
     * @param aTargetSize size of the area where bitmap will be displayed
     * @return ETrue if bitmap does not need scaling
     */
    TBool BitmapFitsIntoTarget( TSize aBmpSize, TSize aTargetSize ) const;

    /**
     * Sets up icon text subcell.
     *
     * @since S60 v3.0
     * @param aTemplateChildArray Array of subcell children where the backdrop subcell template is appended.
     * @param aItemIndex Index of currently drawn item.
     * @param aItemText Text used to format item subcell content.
     * @param aSubcellIncrement Is incremented if backdrop subcell template is appended.
     * @param aChildIndex Index of a template child.
     */
    void SetupTextSubcellL(
        RArray<TTemplateChild>& aTemplateChildArray, TInt aChildIndex, TInt aItemIndex,
        RBuf& aItemText, TInt& aSubcellIncrement ) const;

    /**
     * Appends text if there is enough free memory.
     *
     * @since S60 v3.0
     * @param aBuffer Buffer to which the text will be added
     * @param aTextToAppend Text to add
     */
    void AppendText( RBuf& aBuffer, const TDesC& aTextToAppend ) const;

    /**
     * Returns true if an item having an index equal to aItemIndex is a
     * floating and dragged item.
     *
     * @param aItemIndex An index of a currently drawn item.
     * @return ETrue if an item is floating and dragged.
     */
    TBool IsFloating( TInt aItemIndex ) const;

    /**
     * Returns highlight visibility.
     *
     * @param aItemIndex An index of a currently drawn item.
     * @param aItemIsCurrent Is aItemIndex the current item index.
     * @param aAllowHighlightForNonDraggedItem Should highlight be shown
     * 	(set in case when effects are used).
     * @return ETrue if highlight is drawn for the actually drawn item.
     */
    TBool GetHighlightVisibility(  TInt aItemIndex,
        TBool aItemIsCurrent, TBool aAllowHighlightForNonDraggedItem  ) const;

    /**
     * Returns backdrop visibility.
     *
     * @param aItemIndex An index of a currently drawn item.
     * @param aItemIsCurrent Is aItemIndex the current item index.
     * @return ETrue if backdrop is visible for the currently drawn item.
     */
    TBool GetBackdropVisibility( TInt aItemIndex, TBool aItemIsCurrent ) const;

    /**
     * Checks if item has specified floating type.
     *
     * @param aItemIndex Item index.
     * @param aFloatingType Floating type to check on the item.
     * @return ETrue if the item is of the given floating type.
     */
    TBool ItemHasFloatingType( TInt aItemIndex, TMmFloatingItemType aFloatingType) const;

public:
    /**
     * Animator used by drawer to animate item transitions.
     * Own
     */
    CMmDrawerAnimator* iAnimator;

private: // Data
    /**
     * Listbox model.
     * Not own.
     */
    CMmListBoxModel* iMmModel;

    /**
     * Font.
     */
    const CFont * iFont;

    /**
     * Drawer owner widget type (list/grid).
     */
    TMmWidgetType iWidgetType;

    /**
     * Is draggable flag.
     */
    TBool iDraggable;

    /**
     * Widget.
     */
    CEikListBox* iWidget;

    /**
     * Background context.
     * Not own.
     */
    CAknsBasicBackgroundControlContext * iBgContext;

    /**
     * Redraw background flag.
     */
    TBool iRedrawBackground;

    /**
     * Draw separators flag.
     */
    TBool iDrawSeparatorLines;

    /**
     * Ratio of zooming animation.
     */
    TReal iIconAnimationZoomRatio;

    /**
     * Zommed item index.
     */
    TInt iZoomIconIndex;

    /**
     * Is edit mode flag.
     */
    TBool iIsEditMode;

    /**
     * Array of floating items.
     */
    RArray<TMmFloatingItem> iFloatingItems;

    /**
     * Just draw backdrop flag.
     */
    TBool iLeftOverAreaUnderAnimatedItem;

    /**
     * Is drawn item an indicator item.
     */
    TBool iIsIndicatorItem;

    /**
     * Marquee adapter.
     */
    CMmMarqueeAdapter* iMarqueeAdapter;

    /**
     * Subcell item text.
     */
    HBufC* iSubcellText;

    /**
     * Is marquee being drawn flag;
     */
    TBool iIsMarqueeBeingDrawn;

    /**
     * Number of columns in widget.
     */
    TInt iNumberOfColsInWidget;

    /**
     * Original mask of the dragged item.
     * Not own.
     */
    CFbsBitmap* iOriginalMask;

    /**
     * Gul Icon of the item dragged.
     * Not own.
     */
    CGulIcon* iGulDragged;

    /**
     * Temporary mask of the dragged item.
     * Not own.
     */
    CFbsBitmap* iDragMask;

    /**
     * Indicates id original gul had external bitmaps.
     */
    TBool iExternalBitamp;

    /**
     * Indicates if highlight should be shown.
     */
    TBool iHighlightShown;

    /**
     * Indicates if move indicators should be drawn.
     */
    TBool iDrawMoveIndicators;

    /**
     * Template library. Stores template attributes.
     * Not own.
     */
    CMmTemplateLibrary* iTemplateLibrary;

    /**
     * Storage for cached items data. The data is updated during normal operation
     * and used when iUseCache is ETrue.
     * Own.
     */
    CMmItemsDataCache* iItemsDataCache;

    /**
     * This flag tells whether to rely on cached data (when possible) or
     * to obtain data from the model normally. It should be set to ETrue
     * when drawing performance is critical (e.g. during flick).
     */
    TBool iUseCache;

    /**
     * Stores information on the last used subcells configuration.
     * Such information is needed to determine whether it is necessary to
     * setup subcells before drawing a particular item or is it possible
     * to skip this step because the subcells are already set properly.
     */
    mutable TMmSubcellsSetupCode iLastSubcellsSetupCode;

    /**
     * Set to ETrue if item has backdrop.
     */
    mutable TBool iItemHasBackdrop;

    /**
     * Number of subcells in iData.
     */
    mutable TInt iCurrentNumberOfSubcellsSet;

    /**
     * A structure that stores color information needed to draw an item.
     */
    mutable CFormattedCellListBoxData::TColors iColors;

    /**
     * ETrue if the item that was last drawn was a floating item.
     */
    mutable TBool iLastDrawnItemWasFloating;

    /**
     * ETrue if the drawing item is floating item of swap transition type.
     */
    mutable TBool iIsSwapFloating;

  };

#endif // MMLISTBOXITEMDRAWER_H
