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
* Description:  CMmListBox
*  Version     : %version: MM_22.1.16 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMLISTBOX_H
#define C_MMLISTBOX_H

#include <e32std.h>
#include <e32base.h>
#include <aknlists.h>
#include <e32cmn.h> 

class CMmListBoxModel;
class CMmListBoxItemDrawer;
class CMmMarqueeAdapter;
class CMmWidgetContainer;
class CMmTemplateLibrary;

/**
 *  CMmListBox
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmListBox ) : public CEikFormattedCellListBoxTypedef
	{
public:
    /**
     * Two-phased constructor.
     * 
     * @since S60 v3.0
     * @param aParent Parent control.
     * @param aFlags Flags.
     * @param aTemplateLibrary Template library for drawer. 
     * @return List box.
     */
    static CMmListBox* NewL( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary );
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 v3.0
     * @param aParent Parent control.
     * @param aFlags Flags.
     * @param aTemplateLibrary Template library for drawer. 
     * @return List box.
     */
    static CMmListBox* NewLC( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary );
	
    /**
	 * Destructor.
	 * 
	 * @since S60 v3.0
	 */
	virtual ~CMmListBox();
   
	/**
     * Gets the listbox model.
     *
     * @since S60 v3.0
     * @return Listbox model.
     */
    CMmListBoxModel * MmModel(); 
    
    /**
     * Sets the listbox model.
     *
     * @since S60 v3.0
     * @param aMmModel The model to be set.
     */
    void SetMmModel( CMmListBoxModel* aMmModel );

   /**
    * Creates the view class instance.
    *
    * @since S60 v3.0
    * @return Listbox view.
    */
   CListBoxView* MakeViewClassInstanceL();
   
   /** 
    * Creates the item drawer for listbox.
    * 
    * @param aTemplateLibrary Template library for drawer. 
    * @since S60 v3.0
    */
   void CreateItemDrawerL( CMmTemplateLibrary* aTemplateLibrary );
   
   /** 
    * Sets the flag.
    * 
    * @since S60 v3.0
    * @param aFlag Avkon list flag
    */
   void SetListFlag( TInt aFlag );
   
   /** 
    * Clears the flag.
    * 
    * @since S60 v3.0
    * @param aFlag Avkon list flag
    */
   void ClearListFlag( TInt aFlag );
      

public: // from base class CCoeControl.

   /**
    * From CCoeControl.     
    * Handles pointer events.
    * 
    * @since S60 v3.0
    * @param aPointerEvent Pointer event.
    */
   void HandlePointerEventL( const TPointerEvent& aPointerEvent ); 
   
   /**
    * From base class.
    *
    * @since S60 v3.0
    * @param aKeyEvent Key event.
    * @param aType Event code.
    */
   TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
           TEventCode aType );
   
   /**
    * Adjusts the given rectangle.
    *
    * @since S60 v3.0
    * @param aRect Rectangle to be adjusted.
    * @return Error code.
    */
   TInt AdjustRectHeightToWholeNumberOfItems( TRect& aRect ) const;
   
   /**
    * Sets item drawer and view background context.
    *
    * @since S60 v3.0
    * @param aBgContext Background context.
    */
   void SetItemDrawerAndViewBgContext( 
           CAknsBasicBackgroundControlContext * aBgContext );
 
   /**
    * Updates scrollbar.
    *
    * @since S60 v3.0
    */
   void UpdateScrollBarsL();
   
   /**
    * Special version of Update scrollbar that never calls DrawNow()
    */
   void UpdateScrollBarsNoRedrawL();
   
   /**
    * Checks if all items fit in view rectangle.
    *
    * @since S60 v3.0
    * @return Do all items fit in view rectangle.
    */
   TBool AllItemsFitInViewRect();
   
   /**
    * Handles scrollbar visibility change.
    *
    * @since S60 v3.0
    * @return Is redraw needed.
    */
   TBool HandleScrollbarVisibilityChangeL();
   
   /**
    * Sets marquee adapter.
    *
    * @since S60 v3.0
    * @param aAdapter Marquee adapter.
    */
   void SetMarqueeAdapter( CMmMarqueeAdapter* aAdapter );
   
   /**
    * Notifies that marquee is being drawn.
    *
    * @since S60 v3.0
    * @param aIsMarqueeBeingDrawn Is marquee being drawn.
    */
   void SetMarqueeDrawing( TBool aIsMarqueeBeingDrawn );
   
   /**
    * Handles item removal. redraws view, updates scrollbar.
    *
    * @since S60 v3.0
    */
   void HandleItemRemovalL();
   
   /**
    * Redraws items in view if it is necessary.
    *
    * @since S60 v3.0
    * @param aPreviousCurrent Previously current item index.
    * @param aCurrent Current item index.
    * 
    * @return true if anything was drawn.
    */
	TBool RedrawIfNecessary( TInt aPreviousCurrent, TInt aCurrent );
	
	/**
	 * Overridden from base class to allow for drawing scrollbar
	 * background in mirrored layout.
	 * 
	 * @param aRect clipping rect
	 */
	void Draw(const TRect& aRect) const;
	
	/**
     * Draws the listbox view.
     *
     * @since S60 v5.0
     */
	void DrawView();
    
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
    
    /**
     * Handles scrolling event.
     *
     * @since S60 v3.0
     * @param aScrollBar Scrollbar being scrolled.
     * @param aEventType Type of scrollbar event.
     */
    void HandleScrollEventL( CEikScrollBar* aScrollBar, 
            TEikScrollEvent aEventType );
    
private:
    /**
     * Default constructor.
     * 
     * @since S60 v3.0
     */
	CMmListBox();
	
    /**
     * 2nd phase constructor.
     * 
     * @since S60 v3.0
     * @param aParent Parent control.
     * @param aFlags Flags.
     * @param aTemplateLibrary Template library for drawer. 
     */
	void ConstructL( const CCoeControl* aParent, TInt aFlags,
        CMmTemplateLibrary* aTemplateLibrary );
    
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
     * Ensures that elements are drawn correctly when mirrored layout is
     * used and scrollbar is visible by shifting ViewRect to the right.
     */
    void FixViewForMirroredLayout();
    
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
    
    /**
     * Handles periodic events from @c iRedrawTimer.
     * Such events are generated at equal time intervals while
     * the view is being scrolled using the scrollbar.
     * This function typically calls @c ProcessScrollEventL,
     * which actually scrolls the view and causes a redraw. 
     */
    void HandleRedrawTimerEvent();
    
private:
    /**
     * Callback function for @c iRedrawTimer.
     * It simply calls @c HandleRedrawTimerEvent and returns 0.
     * 
     * @param aPtr A pointer to CMmListBox object.
     * @return 0 (always).
     */
    static TInt RedrawTimerCallback( TAny* aPtr );
    
private: // Data
    /**
     * Item drawer.
     */
    CMmListBoxItemDrawer* iMmDrawer;
    
    /**
     * Multimedia Menu model.
     */
	CMmListBoxModel* iMmModel;

    /**
     * marquee adapter.
     */
	CMmMarqueeAdapter* iMarqueeAdapter;

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
    
    /**
     * ETrue if the view is being scrolled with the scrollbar.
     */
    TBool iScrollbarThumbIsBeingDragged;
    
    /**
     * Stores the number of scrollbar events that were ignored.
     * It is only used while scrolling the view using scrollbar,
     * in such situation the scroll events that this object receives
     * are ignored, and actual scrolling is done only when
     * iRedrawTimer completes.
     */
    TInt iSkippedScrollbarEventsCount;
    
    /**
     * A timer that initiates redraws at certain time intervals.
     * It is used to refresh the view while scrolling with
     * the scrollbar.
     */
    CPeriodic* iRedrawTimer;
	};
	
#endif // MMLISTBOX_H
