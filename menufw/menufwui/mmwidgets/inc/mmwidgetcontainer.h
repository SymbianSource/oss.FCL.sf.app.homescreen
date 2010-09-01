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
*  Version     : %version: MM_48.1.43 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef MMMWIDGETCONTAINER_H
#define MMMWIDGETCONTAINER_H

// INCLUDES
#include <AknDef.hrh>
#include <e32std.h>
#include <e32base.h>
#include <eiklbx.h>
#include <coecntrl.h>
#include <AknsDrawUtils.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <aknlongtapdetector.h>
#include <liwgenericparam.h>

#include "hnsuiteobserver.h"
#include "mmvisibilityobserver.h"
#include "hnglobals.h"

// CLASS DECLARATION
class CHnSuiteModel;
class CMmListBoxModel;
class MEikListBoxObserver;
class MMmKeyEventObserver;
class MMmDragAndDropObserver;
class MMmWidgetObserver;
class CMatrixMenuAppUi;
class CMmHighlightTimer;
class CMmMarqueeAdapter;
class CMmListBoxItemDrawer;
class CMmTemplateLibrary;
class CMmPostEvaluationProcessor;
class MMmLongTapObserver;
/**
 * Square of the distance the cursor has to be dragged from the current
 * position to really move the dragged item.
 */
const TInt KDragDelta( 640 );

/**
 * Structure used to store cache for widget position.
 *
 * @since S60 v5.0
 * @ingroup group_mmwidgets
 */
class TMmWidgetPosition
  {
public:
    /**
     * Default constructor.
     * Initializes member variables with sensible values.
     */
    TMmWidgetPosition();

public: // data
    /**
     * The vertical item offset cache.
     */
    TInt iVerticalItemOffset;

    /**
     * The top item index cache.
     */
    TInt iTopItemIndex;

    /**
     * Tells if the cached values are valid.
     */
    TBool iValid;

    /**
     * The mode (portrait/landscape) for which the cache is valid.
     */
    TBool iLandscape;

    /**
     * Id from model (not index) of the currently highlighted item.
     * It should only be set if the currently highlighted item is
     * visible (fully or partially), otherwise it must remain
     * initialized to KErrNotFound.
     */
    TInt iHighlightedItemId;
    };

/**
 *  Interface for widget containers.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */

NONSHARABLE_CLASS( CMmWidgetContainer ): public CCoeControl,
                                         public MMmVisibilityObserver,
                                         public MEikListBoxObserver,
                                         public MAknLongTapDetectorCallBack
  {
public:

    /**
     * Creates a new grid container.
     *
     * @since S60 v3.0
     * @param aRect Parent rectangle.
     * @param aObjectProvider Object provider.
     * @return GridContainer.
     */
     IMPORT_C static CMmWidgetContainer* NewGridContainerL( const TRect& aRect,
            MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Creates a new listbox container.
     *
     * @since S60 v3.0
     * @param aRect Parent rectangle.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary Template library for drawer.
     * @return ListboxContainer.
     */
     IMPORT_C static CMmWidgetContainer* NewListBoxContainerL( const TRect& aRect,
            MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary );

public: // Highlight related methods

    /**
     * Sets the default highlight.
     *
     * @since S60 v3.0
     * @param aRedraw Is highlight to redraw.
    */
    virtual void SetDefaultHighlightL( TBool aRedraw = ETrue ) = 0;

    /**
     * Sets the highlight.
     *
     * @since S60 v3.0
     * @param aItemIndex Index to set the highlight at.
     * @param aRedraw Is highlight to redraw.
     */
    IMPORT_C virtual void SetManualHighlightL(TInt aItemIndex,  TBool aRedraw = ETrue );

    /**
     * Gets the highlight from the widget.
     *
     * @since S60 v3.0
     * @return Current Highlight in the widget.
     */
    IMPORT_C virtual TInt GetHighlight();

    /**
     * Gets the highlight from the widget.
     *
     * @since S60 v3.0
     * @return Previous Highlight in the widget.
     */
    IMPORT_C virtual TInt GetPreviousHighlight();

    /**
     * Set highlight visibility.
     *
     * @since S60 v3.0
     * @param aVisible Visibility status.
     */
    IMPORT_C virtual void SetHighlightVisibilityL( TBool aVisible );

    /**
     * Set highlight visibility.
     *
     * @since S60 v3.0
     * @return Visibility status.
     */
    IMPORT_C virtual TBool IsHighlightVisible();

    /**
     * Handle item addition.
     *
     * @since S60 v3.0
     */
    IMPORT_C virtual void HandleItemAdditionL();


    /**
     * Handle item removal.
     *
     * @since S60 v3.0
     */
    IMPORT_C virtual void HandleItemRemovalL();

    /**
     * Handle item removal.
     *
     * @since S60 v3.0
     */
    IMPORT_C CHnSuiteModel* GetSuiteModelL();

    /**
     * Retrieve type of widget.
     *
     * @since S60 v3.0
     * @return Type of widget.
     */
    IMPORT_C virtual THnSuiteWidgetType WidgetType();

    /**
     * Removes all LIW objects owned by this object.
     *
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
    */
    virtual void RemoveLiwObjects();

    /**
     * Sets the long tap observer.
     *
     * @param aObserver Observer to receive long tap events.
     */
    IMPORT_C virtual void SetLongTapObserver( MMmLongTapObserver* aObserver );

    /**
     * Informs the container that long tap event is finished (e.g. because
     * a command from the context menu has been issued).
     *
     * @param aStopTimer ETrue when the highlight timer should be stopped.
     */
    IMPORT_C virtual void EndLongTapL( TBool aStopTimer = ETrue );

public:

    /**
     * Sets suite model.
     *
     * @since S60 v3.0
     * @param aModel Suite model.
     */
    virtual void SetSuiteModelL( CHnSuiteModel* aModel );

    /**
     * Gets Multimedia Menu model.
     *
     * @since S60 v3.0
     * @return Model.
     */
    virtual CMmListBoxModel* GetMmModel() = 0;

    /**
     * Sets widget observer.
     *
     * @since S60 v3.0
     * @param aObserver Widget observer.
     */
    IMPORT_C virtual void SetObserver( MMmWidgetObserver*  aObserver);

    /**
     * Gets item rectangle according to item index.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index.
     * @return Item rectangle.
     */
    IMPORT_C TRect GetItemRectL( TInt aItemIndex );

    /**
     * Sets empty text visible when model has no items.
     *
     * @since S60 v3.0
     * @param aText Text to be shown when view is empty.
     */
    virtual void SetEmptyTextL(const TDesC& aText) = 0 ;

    /**
     * Sets flag.
     *
     * @since S60 v3.0
     * @param Flag Flag.
     */
    virtual void SetFlag(TInt Flag);

    /**
     * Gets widget.
     *
     * @since S60 v3.0
     * @return Widget.
     */
    virtual CEikListBox* Widget();

    /**
     * Sets background context for item drawer and view.
     *
     * @since S60 v3.0
     * @param aBgContext Background context.
     */
    virtual void SetItemDrawerAndViewBgContext(
      CAknsBasicBackgroundControlContext * aBgContext ) =0;

    /**
     * Sets edit mode so UI is aware.
     *
     * @since S60 v3.0
     * @param aIsEditMode Is edit mode.
     */
  virtual void SetEditModeL( TBool aIsEditMode );

    /**
     * Tells if UI is aware of edit mode.
     *
     * @since S60 v5.0
     * @return Edit mode status.
     */
  virtual TBool IsEditMode() const;

    /**
     * Sets up widget layout (needed for grid).
     *
     * @since S60 v3.0
     */
  virtual void SetupWidgetLayoutL() =0;

    /**
     * Constructor.
     */
  CMmWidgetContainer();

    /**
     * Destructor.
     */
    ~CMmWidgetContainer();

    /**
     * Enables/disables animation during long tap.
     * Tactile feedback is also enabled and disabled with the animation.
     * Changes made with this method will be effective on the next long
     * tap, i.e. it is not possible to stop an already started animation
     * by using this method.
     *
     * @param aEnable ETrue - enable, EFalse - disable long tap animation
     *                and long tap tactile feedback
     */
    IMPORT_C void EnableLongTapAnimation( TBool aEnable );

    /**
     * from CCoeControl.
     *
     * @since S60 v3.0
     */
    void SizeChanged();

    /**
     * Draws the widget.
     *
     * @since S60 v3.0
     * @param aRect Rectangle within the widget shuld be drawn.
     */
    void Draw(const TRect& aRect) const;

    /**
      * Draws the widget view.
      *
      * @since S60 v5.0
      */
    virtual void DrawView() = 0;

    /**
     * Returns type UID pointer that can be used in MopSupplyObject.
     *
     * @since S60 v3.0
     * @param aId Type UID, should be the same that was given as aId.
     * parameter of MopSupplyObject.
     * @return Type UID pointer.
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    /**
     * From CCoeControl.
     *
     * @since S60 v3.0
     * @param aPointerEvent Pointer event.
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * From CCoeControl.
     *
     * @since S60 v5.0
     * @param aPointerEvent Pointer event.
     */
    void HandlePointerEventsInEditModeL(const TPointerEvent& aPointerEvent,
        TBool aAbortAnimations );

    /**
     * Set draggable.
     *
     * @since S60 v3.0
     * @param aDraggable Is draggable.
     */
    void SetDraggableL( TBool aDraggable );

    /**
     * Gets draggable status.
     * Draggable status determines if the item has
     * the capability to be dragged at the momont.
     * So before threshold is crossed the container
     * is not draggable in the sense of this method
     * because the dragged item is not yet drawn.
     * It becomes draggable when it start to be drawn
     *
     * @since S60 v3.0
     * @return Is draggable.
     */
    IMPORT_C TBool IsDraggable();

    /**
     * From CCoeControl.
     *
     * @since S60 v3.0
     * @return count component controls.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     *
     * @since S60 v3.0
     * @param aIndex index of control.
     * @return component control.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl.
     *
     * @since S60 v3.0
     * @param aKeyEvent key event.
     * @param aType event type.
     * @return response to key event.
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

    /**
     * Zooms item icon.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index.
     */
    IMPORT_C void ItemIconZoomL( TInt aItemIndex );

    /**
     * Handle button up event.
     *
     * @since S60 v3.0
     * @param aPointerEvent Pointer event.
     */
    void HandleButtonDownL(const TPointerEvent& aPointerEvent );

    /**
     * Handle button down event.
     *
     * @since S60 v3.0
     * @param aPointerEvent Pointer event.
     */
    void HandleButtonUpL(const TPointerEvent& aPointerEvent );

    /**
     * Overridden to invalidate item drawer cache on skin change and
     * dynamic variant switch.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );

    /**
     * Handle dragging of item.
     *
     * @since S60 v3.0
     * @param aPointerEvent Pointer event.
     * @param aAbortAnimation Should animation be aborted.
     */
    void HandleDragL(const TPointerEvent& aPointerEvent, TBool aAbortAnimation );

    /**
     * Sets marquee adapter.
     *
     * @since S60 v3.0
     * @param aAdapter A marquee adapter.
     */
    void SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter );

    /**
     * Sets up item drawer.
     *
     * @since S60 v3.0
     */
    void SetupDrawer();

    /**
     * Cancels gragging of item.
     *
     * @since S60 v3.0
     * @param aAnimate Should dragged item transition be animated.
     */
    IMPORT_C void CancelDragL( TBool aAnimate );

    /**
     * Animates item shifting.
     *
     * @since S60 v3.0
     * @param aHighlight Highlighted item index.
     */
    void AnimateShiftL(TInt aHighlight);

    /**
     * Gest number of items.
     *
     * @since S60 v3.0
     * @return Number of items.
     */
    IMPORT_C TInt NumberOfItems();

    /**
     * This should be called at the end of edit mode in non-touch
     *
     * @since S60 v5.0
     */
    IMPORT_C void StopMovingL();

    /**
     * Set the vertical item offset;
     * @param aOffset The offset to set to the widget.
     *
     * @since S60 v5.0
     */
    virtual void SetVerticalItemOffset( TInt aOffset ) = 0;

    /**
     * Gets the current vertical item offset for the widget in the container.
     * @since S60 v5.0
     *
     * @return The current vertical item offset.
     */
    virtual TInt VerticalItemOffset() const = 0;

    /**
     * Saves the currents position of the widget. The vertical item offset
     * and the top item index are cached.
     * @since S60 v5.0
     */
    IMPORT_C void CacheWidgetPosition();

    /**
     * Resets the widget position cache to top of view.
     * Both vartical item offset and top index are set to zero
     * in cache and widget.
     * @since S60 v5.0
     */
    IMPORT_C void ResetWidgetPosition();

    /**
     * Restores the cached widget position values in the widget.
     * @since S60 v5.0
     */
    IMPORT_C void RestoreWidgetPosition();

    /**
     * Scrolls to the specified item index so that the item is seen entirely on screen.
     * @since S60 v5.0
     *
     * @param aIndex The widget item index to scroll to.
     * @return True if any scrolling was done.
     */
    IMPORT_C TBool ScrollToItemL( TInt aIndex );

    /**
     * Called when the number of items in widget model changed.
     * @since S60 v5.0
     *
     * @param aChange Type of change
     */
    IMPORT_C void NumberOfItemsChangedL( TItemsChangeType aChange );

    /**
     * Aligns the bottom of view to the last model item bottom edge so that no extra
     * unused pixels are visible at the bottom of screen.
     * @since S60 v5.0
     *
     * @return True if the view was scrolled.
     */
    TBool AlignBottomOfViewL( );

    /**
     * Calculate numer of pixels required to scroll when aligning bottom of view.
     * @since S60 v5.0
     */
    TInt CalcBottomPixelsToScroll();

    /**
     * Sets visibility of a widget.
     *
     * @param aVisible ETrue if widget should be visible, EFalse - otherwise
     */
    void MakeVisible(TBool aVisible);

    /**
     * Tells if item is visible (fully or partially).
     * @param aItemIndex Item index.
     * @return ETrue if visible, EFalse otherwise.
     */
    virtual TBool ItemIsVisible( TInt aItemIndex ) const;

     /**
     * Tells if item is entirely visible.
     * @param aIndex The index of the item.
     */
    IMPORT_C TBool ItemIsFullyVisible(TInt aIndex);

    /**
    * Prepares the container to be set to the garbage collector.
    */
    IMPORT_C void PrepareForGarbage();

    /**
     * Checks, whether the currently dragged item is a draggable item. i.e. parent folder
     * is not a draggable item.
     * @returns True, if dragged item is parent folder
     * or no item is dragged (iDraggedIndex = -1), false otherwise.
     */
    TBool IsNoItemDragged();

    /**
     * Sets the iHasFocus member variable, which is used to determine
     * if marquee animation can be enabled.
     */
    IMPORT_C void SetHasFocusL( TBool aHasFocus );

    /**
     * Sets the iIsFaded member variable, which is used to determine
     * if marquee animation can be enabled.
     */
    IMPORT_C void SetIsFaded( TBool aIsFaded );

    /**
     * Determines if long tap is in progress.
     *
     * @return ETrue if long tap is in progress.
     */
    TBool LongTapInProgress() const;

    /**
     * Determines if folder can be moved to another one.
     */
    IMPORT_C TBool AllowMove() const;

    /**
     * Sets allow move param.
     * @param aAllowMove. ETrue if move item is allowed.
     */
    void SetAllowMove( TBool aAllowMove );

    /**
     * Sets parameters for move event.
     * @param aRecipientId Item id to be moved.
     * @param aEventParameters Event parameters.
     */
    IMPORT_C void SetTriggerMoveItemL( const TInt aRecipientId,
                CLiwGenericParamList* aEventParameters );

    /**
     * Calls move event.
     */
    void TriggerMoveItemL();

    /**
     * Sets if extention dialog is opened.
     */
    IMPORT_C void SetExDialogOpened( TBool aOpened );

    /**
     * Widget position cache.
     */
    TMmWidgetPosition WidgetPositionCache() const;

public: // from MMmVisibilityObserver

    /**
     *
     * @since S60 v3.0
     */
    IMPORT_C void HandleBackgroundGainedL();

    /**
     *
     * @since S60 v3.0
     */
    IMPORT_C void HandleForegroundGainedL();
public: // from MEikListBoxObserver

    /**
     * Handles list box events.
     *
     * @since S60 v5.0
     */
    virtual void HandleListBoxEventL(CEikListBox* aListBox,
            TListBoxEvent aEventType);

public:
    /**
     * From MAknLongTapDetectorCallBack. Handles long tap events.
     *
     * @since S60 v5.0
     * @param aPenEventLocation Point coordinates relative to container.
     * @param aPenEventScreenLocation Point coordinates relative to screen.
     */
    virtual void HandleLongTapEventL( const TPoint& aPenEventLocation,
                                      const TPoint& aPenEventScreenLocation );
protected:

    /**
     * Handles additional contruction tasks.
     *
     * @since S60 v3.0
     */
  void ConstructL();

    /**
     * Sets highlight locally.
     * Does not modify the avkon behaviur.
     *
     * @since S60 v3.0
     * @param aItemIndex Index of the item.
     */
  void SetHighlightL(TInt aItemIndex);

  /**
     * Checks whether given point collides with specific item's re-order area.
     *
     * This function gets called during drag-and-drop operations to
     * help determine whether item re-ordering is needed.
     *
     * @param aItemIndex index of the item to check
     * @param aPoint point coordinates
     * @return ETrue if point is located within item re-order area,
     *         EFalse otherwise
     */
    virtual TBool PointInItemReorderAreaL( TInt aItemIndex, TPoint aPoint );

    /**
     * Hides the options menu if it is being displayed.
     * This has (probably) the same effect as clicking Cancel (RSK) -
     * the menu just disappears.
     */
    void HideOptionsMenuIfDisplayed();

protected:

    /**
     * Validates the widget current item index so that it is consistent with
     * the current model.
     */
    void ValidateWidgetCurrentItemIndex();

    /**
     * Updates current view's scrollbar thumbs.
     */
    virtual void UpdateViewScrollBarThumbs();

    /**
     * Starts or stops marquee animation based on the current state
     * of the container.
     */
    void StartOrStopMarquee();

private:

   /**
   * Called when the number of items in widget model changed.
   * @since S60 v5.0
   *
   * @param aChange Type of change
   */
  void HandleNumberOfItemsChangedL( TItemsChangeType aChange );

    /**
     * Manages zooming of folder.
     *
     * @since S60 v3.0
     * @param aDraggedItemOverIcons Is dragged over icons flag.
     */
  void ManageFolderZoomingL( TBool aDraggedItemOverIcons );

    /**
     * Cancels gragging of item if relevant( Edit Mode is activated )
     *
     * @since S60 v3.0
     */
    void CancelDragL();

    /**
     * Sets the widget highlight back to place if e.g. item was dragged over an item
     * where it could not be dropped into.
     *
     * @since S60 v3.0
     */
    void SetHighlightAfterDrag();

    /**
     * Handles key event.
     *
     * @since S60 v3.0
     * @param aKeyEvent Key event.
     * @param aType Event code.
     */
    TKeyResponse HandleKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

    /**
     * Handles rocker (select) press.
     */
    void HandleRockerPressL();

    /**
     * Gets distance between current and previous position.
     * @param aPos Current position.
     * @param aTapPoint Point of the tap.
     * @return Distance between two points in pixels.
     */
    TInt DeltaSquare( const TPoint aTapPoint,
        const TPoint aPos );

    /**
     * Checks, whether given item is a folder or root folder.
     * @param aItemIndex Item index to check.
     * @returns True, if given item is a folder or root folder, false otherwise.
     */
    TBool IsFolderL( TInt aItemIndex );

    /**
     * Checks, whether given item has IsDeleteLockedL flag set.
     * @param aItemIndex Item index to check.
     * @returns IsDeleteLockedL flag
     */
    TBool IsDeleteLocked( TInt aItemIndex );

  /**
   * Gets column count in current view.
   * @returns Column count in current view.
   */
  virtual TInt ColumnsInCurrentView();

    /**
     * Gets row count in current view.
     * @returns Row count in current view.
     */
    virtual TInt RowsInCurrentView();

  /**
   * Scrolls the view move mode non-touch so that move indicators are visible.
   */
  void ScrollViewIfNeededL();

    /**
     * Scrolls the view in pixels.
     * @param aPixels THe number of pixels to scroll. If negative, the view is scrolled down.
     */
  void ScrollInPixelsL( TInt aPixels );

    /**
     * Sets up the scrolling effect movement type.
     * @param aDown True if scrolling is downwards.
     */
  void SetupScrollingEffectsL( TBool aDown );

protected:


    /**
     * Own.
     */
    CEikListBox* iWidget;

    /**
     * Key event observer.
     */
    MMmKeyEventObserver* iKeyEventObserver;

    /**
     * Drag and drop observer.
     */
    MMmDragAndDropObserver* iDragAndDropObserver;
     /**
      * List box observer.
      */
    MEikListBoxObserver* iListBoxObserver;

    /**
     * Marquee adapter.
     * Own.
     */
    CMmMarqueeAdapter* iMarqueeAdapter;

    /**
     * Item drawer.
     * Not own.
     */
    CMmListBoxItemDrawer* iDrawer;

protected:

    /**
     * Current Highlight.
     */
    TInt iCurrentHighlight;

    /**
     * Processed display elelments for better performance.
     */
    CMmPostEvaluationProcessor* iPostProcessor;

    /**
     * Set when long tap is in progress (stylus popup displayed over container)
     */
    TBool iLongTapInProgress;

private:

    /**
     * Has drag occurred.
     */
    TBool iDragOccured;

    /**
     * Background context.
     * Own.
     */
  CAknsBasicBackgroundControlContext* iBgContext;

    /**
     * Last drag point.
     */
  TPoint iLastDragPoint;

    /**
     * First tap point.
     */
  TPoint iTapPoint;

    /**
     * First tap point.
     */
  TPoint iItemRelativeTapPoint;

  /**
     * Last drag highlight.
     */
  TInt iLastDragHighlight;

    /**
     * Dragged item index.
     */
    TInt iDraggedIndex;

    /**
     * Edit mode status.
     */
  TBool iIsEditMode;

  /**
     * Destination of item index.
     */
  TInt iItemIndexDestination;

  /**
     * Previous Highlight.
     */
  TInt iPreviousHighlight;

  /**
   * The current rect of the widget control.
   */
    TRect iWidgetRect;

    /**
     * Longpress allowed flag. We only accept long press (EEventKey+iRepeats)
     * when there was no highlight visible before EEventKeyDown, otherwise
     * we react only to EEventKeyDown
     */
    TBool iAllowLongPress;

  /**
   * Cache for widget position.
   */
    TMmWidgetPosition iWidgetPositionCache;

    /**
     * ETrue if Matrix menu window is faded.
     */
    TBool iIsFaded;

    /**
     * ETrue if Matrix menu has focus (i.e. it is not obscured by any popup
     * note).
     */
    TBool iHasFocus;

    /**
     * ETrue if in foreground.
     */
    TBool iInForeground;

    /**
     * Own.
     * Detects long tap events.
     */
    CAknLongTapDetector* iLongTapDetector;

    /**
     * Observer to notify about long tap events.
     */
    MMmLongTapObserver* iLongTapObserver;

    /**
     * Stores previously set highlight visibility.
     * ETrue - hightlight visible, EFalse - highlight disabled.
     * Please note that in most cases it is better to read
     * ESingleClickDisabledHighlight flag of itemdrawer than rely
     * on this member variable to determine if highlight is visible
     * (@c IsHighlightVisible).
     */
    TBool iPreviousHighlightVisibility;

    /**
     * Defines if move item is allowed.
     */
    TBool iAllowMove;

    /**
     * Defines item id to be moved.
     */
    TInt iRecipientId;

    /**
     * Event parameters for move item.
     */
    CLiwGenericParamList *iEventParameters;

    /*
     * Two following flags were added for fix ou1cimx1#344006 error.
     * iDialogOpened - it's used to mark if any dialog has been opened.
     * iHighlightVisibleBeforeLongTap - it's used to remember if highlight
     *          had been visible when dialog has been opened.
     */
    /**
     * ETrue if extension dialog is opend.
     */
    TBool iDialogOpened;

    /**
     * ETrue if highlight was visible before longTap event.
     */
    TBool iHighlightVisibleBeforeLongTap;

  };

#endif // MMMWIDGETCONTAINER_H
