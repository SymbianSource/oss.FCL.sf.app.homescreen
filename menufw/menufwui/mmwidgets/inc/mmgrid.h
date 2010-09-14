/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMmGrid declaration
*  Version     : %version: MM_32.1.26 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMGRID_H
#define C_MMGRID_H

#include <e32std.h>
#include <e32base.h>
#include <AknGrid.h>

class CMmGridModel;
class CMmListBoxItemDrawer;
class CMmMarqueeAdapter;
class CMmWidgetContainer;
class CMmTemplateLibrary;

/**
 *  Multimedia Menu Grid Widget
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmGrid ) : public CAknGrid
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aParent Parent control.
     * @param aFlags Additional Flags.
     * @param aTemplateLibrary Template library for drawer.
     */
    static CMmGrid* NewL( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Two-phased constructor.
     *
     * @param aParent Parent control.
     * @param aFlags Additional Flags.
     * @param aTemplateLibrary Template library for drawer.
     */
    static CMmGrid* NewLC(const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Destructor.
     */
    virtual ~CMmGrid();

    /**
     * Draws the grid is given rectangle.
     *
     * @since S60 v3.0
     * @param aRect Rect within which grid should be drawn.
     */
    void Draw( const TRect& aRect ) const;

    /**
     * Draws the grid view.
     *
     * @since S60 v5.0
     */
    void DrawView();

    /**
     * Creates the item drawer for grid.
     *
     * @since S60 v3.0
     */
    void CreateItemDrawerL();

    /**
     * Creates the item drawer for grid.
     *
     * @param aTemplateLibrary Template library for drawer.
     * @since S60 v3.0
     */
    void CreateItemDrawerL( CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Creates the view class instance for grid.
     *
     * @since S60 v3.0
     * @return Griv view.
     */
    CListBoxView* MakeViewClassInstanceL();

    /**
     * Handles scrolling event.
     *
     * @since S60 v3.0
     * @param aScrollBar Scrollbar being scrolled.
     * @param aEventType Type of scrollbar event.
     */
    void HandleScrollEventL( CEikScrollBar* aScrollBar,
            TEikScrollEvent aEventType );

    /**
     * Handles pointer event.
     *
     * @since S60 v3.0
     * @param aPointerEvent Type of pointer event.
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * Sets item drawer and view background context.
     *
     * @since S60 v3.0
     * @param aBgContext Background context to be set in view and item drawer.
     */
    void SetItemDrawerAndViewBgContext(
        CAknsBasicBackgroundControlContext* aBgContext );

    /**
     * Handles changes in scrollbar visibility.
     *
     * @since S60 v3.0
     */
    TBool HandleScrollbarVisibilityChangeL();

    /**
     * Checks if all items from model fit in given rectangle.
     *
     * @since S60 v3.0
     * @return Do items fit in view rectangle.
     */
    TBool AllItemsFitInViewRect();

    /**
     * Updates scrollbar changes.
     *
     * @since S60 v3.0
     */
    void UpdateScrollBarsL();

    /**
     * Sets the marquee adapter.
     *
     * @since S60 v3.0
     * @param aAdapter Marquee adapter.
     */
    void SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter );

    /**
     * Sets up the layout (orientation and items' sizes).
     *
     * @since S60 v3.0
     */
    void SetupLayout();

    /**
     * Gets the Grid model.
     *
     * @since S60 v3.0
     * @return Grid model.
     */
    CMmGridModel * MmModel();

    /**
     * This function from @c CAknGrid handles key events.
     *
     * It has been overridden to allow for correct handling of
     * left and right rocker keys when mirrored layout is used.
     *
     * @param aKeyEvent event to handle
     * @param aType type of the key event
     * @return response code ( @c EKeyWasConsumed, @c EKeyWasNotConsumed )
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    /**
     * This function from CAknGrid is overriden to make grid
     * behavior correct in the mirrored layout.
     * Please note that it is necessary because of the hackish
     * solution used to draw items in mirrored layout.
     */
    void HandleViewRectSizeChangeL();

    /**
     * Overridden function from CAknGrid.
     */
    void AdjustTopItemIndex() const;

    /**
     * Set the vertical item offset;
     * @param aOffset The offset to set to the widget.
     *
     * @since S60 v5.0
     */
    void SetVerticalItemOffset( TInt aOffset );

    /**
     * Gets the current widget vertical item offset.
     *
     * @since S60 v5.0
     * @return The current widget vertical item offset.
     */
    TInt VerticalItemOffset() const;

    /**
     * Simply sets the item height members in widget and view.
     */
    void SetItemHeight( TInt aItemHeight );

    /**
     * Updates scrollbar thumbs.
     */
    void UpdateScrollBarThumbs();

    /**
     * Counts the number of component controls which this component owns.
     */
    TInt CountComponentControls() const;

    /**
     * Disables/enables child component (scrollbar) drawing.
     */
    void SetDisableChildComponentDrawing( TBool aDisable );

private:
    /**
     * Default constructor.
     *
     * @since S60 v3.0
     */
    CMmGrid();

    /**
     * 2nd phase constructor.
     *
     * @since S60 v3.0
     * @param aParent Parent control.
     * @param aFlags Additional Flags.
     * @param aTemplateLibrary Template library for drawer.
     */
    void ConstructL( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Handles changes in resource.
     *
     * @since S60 v3.0
     * @param aIsLandscape Is orientation in landscape mode.
     * @param aCellSize The size of grid cell/item.
     * @param aViewLayout The layout of the grid (e.g. 3x4).
     */
    void DoHandleResourceChangeL( TBool aIsLandscape, TSize& aCellSize,
            TSize& aViewLayout );

    /**
     * Handles changes in resource.
     *
     * @since S60 v3.0
     * @param aCellSize The size of grid cell/item.
     * @param aViewLayout The layout of the grid (e.g. 3x4).
     */
    void DoHandleResourceChangeL( TSize& aCellSize,
            TSize& aViewLayout );

    /**
     * Sets up the layout (for use in non-leaving SetupLayout())
     *
     * @since S60 v3.0
     */
    void DoSetupLayoutL();

    /**
     * Handles pointer events when edit mode is enabled.
     *
     * This method is only called during edit mode. It selectively passes
     * only some of the pointer events to CAknGrid::HandlePointerEventL
     * in order to disable flicking and panning.
     *
     * @param aPointerEvent pointer event
     */
    void HandlePointerEventInEditModeL( const TPointerEvent& aPointerEvent );

    /**
     * Handles pointer events when edit mode is disabled.
     *
     * @param aPointerEvent pointer event
     */
    void HandlePointerEventInNormalModeL( const TPointerEvent& aPointerEvent );

    /**
     * Scrolls the view if pointer is close to the top/bottom edge.
     *
     * This method is used only when edit mode is active. In a way it
     * brings back the focus based scrolling behavior that was present
     * in CAknGrid before ODE scrolling was introduced.
     */
    void HandleScrollingInEditMode( const TPointerEvent& aPointerEvent );

    /**
     * Checks if pointer position is within the scroll-triggering area.
     *
     * @param aPointerEvent pointer event
     * @return true if pointer above the top scrolling threshold.
     */
    TBool IsPointerInTopScrollingThreshold(
            const TPointerEvent& aPointerEvent ) const;

    /**
     * Checks if pointer position is within the scroll-triggering area.
     *
     * @param aPointerEvent pointer event
     * @return true if pointer below the bottom scrolling threshold.
     */
    TBool IsPointerInBottomScrollingThreshold(
            const TPointerEvent& aPointerEvent ) const;

    /**
     * Scrolls the view if the pointer is near top/bottom edge of the screen.
     *
     * Assumes that current item index is the index of the item under the
     * pointer. The time value returned is based on the distance of the pointer
     * from the top/bottom edge of the grid view (this makes the scrolling speed
     * dependent of how close the pointer is to the edge).
     * If there is no need to continue scrolling because the beginning/end of
     * the list has already been reached, 0 is returned.
     *
     * @param aPointerEvent pointer event
     * @return time to wait before calling this method again (in microseconds)
     *         or 0 if already at the beginning/end of the list
     */
    TInt ScrollIfNeeded( const TPointerEvent& aPointerEvent );

    /**
     * Minimal scrolling (setting top item index and vertical offset) with
     * boundary checking and WITHOUT redrawing.
     *
     * @param aDistanceInPixels Distance to scroll.
     */
    void ScrollWithoutRedraw( TInt aDistanceInPixels );

    /**
     * Updates the dispapearing highlight.
     *
     * @since S60 v3.0
     * @param aPointerEvent Type of pointer event.
     */
    void HandleOverridenHighlightDrawing( const TPointerEvent& aPointerEvent,
        TInt aIndexBefore, TInt aIndexAfter );

    /**
     * Ensures that elements are drawn correctly when mirrored layout is
     * used and scrollbar is visible by shifting ViewRect to the right.
     * Also disabled background drawing for scrollbar as it would not
     * be drawn correctly in mirrored layout.
     */
    void FixViewForMirroredLayout();

    /**
     * Gets the offset margin of scrollbar related to view rectangle.
     *
     * @return The distance from view rect side to scrollbar.
     */
    TInt ScrollBarOffset();

    /**
     * Redraws the background under the vertical scrollbar in mirrored layout.
     *
     * The reason such method is needed is that view rectangle is shifted
     * to the right in mirrored layout and does not cover the scrollbar
     * area. In normal (non-mirrored) mode this method does nothing.
     */
    void RedrawScrollbarBackground() const;

    /**
     * Does actual handling of scroll events.
     *
     * @param aScrollBar Scrollbar being scrolled.
     * @param aEventType Type of scrollbar event.
     */
    void ProcessScrollEventL( CEikScrollBar* aScrollBar,
            TEikScrollEvent aEventType );

private:
    /**
     * Grid model.
     */
    CMmGridModel* iMmModel;

    /**
     * Item drawer.
     */
    CMmListBoxItemDrawer* iMmDrawer;

    /**
     * Marquee adapter.
     */
    CMmMarqueeAdapter* iMarqueeAdapter;

    /**
     * Scrollbar visibility flag.
     */
    TBool iScrollbarVisibilityChanged;

    /**
     * Item index which is current in grid.
     */
    TInt iCurrentItemIndex;

    /**
     * Currently top view item index.
     */
    TInt iCurrentTopItemIndex;

    /**
     * Layout of grid.
     */
    TSize iViewLayout;

    /**
     * A flag that indicates that highlight is visible because
     * the user is pressing the screen (continuously) and that
     * highlight should remain visible only as long as the screen
     * is being pressed.
     */
    TBool iHighlightVisibleUntilButton1Up;

    /**
     * Blocks scrollbar drawing. When this flag is set scrollbars components
     * are blocked in the CountComponentControls() method;
     */
    TBool iDisableChildComponentDrawing;

    /**
     * This member is only used in edit mode to store the position of the pointer
     * during EButton1Down event.
     */
    TPoint iButton1DownPos;

    };

#endif // C_MMGRID_H
