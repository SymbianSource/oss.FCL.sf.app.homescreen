/*
 * ============================================================================
 *  Name        : hgteleportfastswaparea.h
 *  Part of     : Hg Teleport
 *  Description : Teleport Fast Swap area UI
 *  Version     : %version: sa1spcx1#45 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
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

//macro TASKSWITCHER_USE_CUSTOM_LAYOUT
#if !defined ( TASKSWITCHER_USE_CUSTOM_LAYOUT )
    //#define TASKSWITCHER_USE_CUSTOM_LAYOUT 1
#endif


#ifndef HGTELEPORTFASTSWAPAREA_H
#define HGTELEPORTFASTSWAPAREA_H

#include <e32base.h>
#include <eikmobs.h>
#include "hgteleportdevicestate.h"
#include "hgfswclientobserver.h"
#include "hgteleportfastswapgrid.h"
#include "hgteleporteventcontrolerobserver.h"


// FORWARD DECLARATIONS
class CHgFswClient;
class CHgFswEntry;
class CAknStylusPopUpMenu;
class CAknQueryDialog;
class CHgTeleportEventControler;
class MHgTeleportDataChangeObserver;

#ifdef TASKSWITCHER_USE_CUSTOM_LAYOUT
const TInt KFswItemHeight = 250;
const TInt KFswItemWidth = 240;
const TInt KFswImageSize = 200;
const TInt KFswTextHeight = 20;
const TInt KFswBorderSize = 10;
const TInt KFswHeadingHeight = 50;
#endif

/**
 * Teleport Fast Swap area UI.
 */
class CHgTeleportFastSwapArea: public CCoeControl,
                               public MEikMenuObserver,
                               public MHgFswObserver,
                               public MEikListBoxObserver,
                               public MHgTeleportFastSwapGridObserver,
                               private MHgTeleportEventControlerObserver,
                               public MHgDeviceStateObserver,
                               public MHgTeleportFastSwapTimerObserver
    {
public:
    /**
     * Two-phase constructor.
     * 
     * @param aRect Area where fast swap info & list should draw itself.
     * @param aParent Parent CoeControl. 
     */
    static CHgTeleportFastSwapArea* NewL( const TRect& aRect,
            CCoeControl& aParent, CHgTeleportDeviceState& aDeviceState,
            CHgTeleportEventControler& aEventHandler);

    /**
     * @copydoc NewL
     */
    static CHgTeleportFastSwapArea* NewLC( const TRect& aRect,
            CCoeControl& aParent, CHgTeleportDeviceState& aDeviceState,
            CHgTeleportEventControler& aEventHandler);

    /**
     * Destructor.
     */
    ~CHgTeleportFastSwapArea();

    /**
     * Called from appview when coming to foreground.
     */
    void HandleSwitchToForegroundEvent();
    
    /**
     * Called from appview when going to background.
     */
    void HandleSwitchToBackgroundEvent();

    /**
     * Called after construction to set up additional properties,
     * like the control observer.
     */
    void Setup( MCoeControlObserver& aControlObserver );

    /**
     * Saves the index of the currently selected (highlighted) item.
     */
    void SaveSelectedIndex();
    
    /**
     * Restores the selection (highlight) position.
     */
    void RestoreSelectedIndex();

    // Functions to perform activate, close, close others operations.
    // They are public so same functionality can be implemented in the
    // Options menu (appui) for example.
    
    /**
     * Index of highlighted item or KErrNotFound.
     */
    TInt SelectedIndex() const;
    
    /**
     * Brings the app corresponding to the item at given index into foreground.
     */
    void SwitchToApp( TInt aIndex );
    
    /**
     * Sends close msg to given app.
     * @param   aIndex      index
     * @param   aSuppressRendering  content is not refreshed if ETrue
     */
    void TryCloseAppL( TInt aIndex, TBool aSuppressRendering = EFalse );
    
    /**
     * Sends close msg to given app if user confirms the query.
     */
    void TryCloseAppWithQueryL( TInt aIndex );
    
    /**
     * Tries to Close all applications that cn be closed
     */
    void TryCloseAllL();
    
    /**
     * True if it is allowed to ask the given app to close.
     * @param   aIndex  index
     */
    TBool CanClose( TInt aIndex ) const;
    
    /**
     * @param  aSelectedItem  item for which pop up is shown
     * 
     * @return ETrue, when number of closable apps is >= 2 or there is at least
     *                one closable item and non closable item is selected.
     * @return EFalse, when number of closable apps is < 2.
     */
    TBool CanCloseAll( TInt aSelectedItem ) const;

    /**
     * Returns the number of items in the list.
     */
    TInt Count() const;

    /**
     * Returns the underlying Ganes control pointer.
     */
    CCoeControl* GetChildControl() { return iGrid; }

    /**
     * Sets the data-changed observer.
     */
    void SetDataChangeObserver( MHgTeleportDataChangeObserver* aDcObserver );

    /*
     * Handling short/long app key.
     */
    void HandleAppKey(TInt aType);

private:

    /**
     * @copydoc NewL
     */
    void ConstructL( const TRect& aRect );

    /**
     * C++ constructor.
     */
    CHgTeleportFastSwapArea(CCoeControl& aParent,
            CHgTeleportDeviceState& aDeviceState,
            CHgTeleportEventControler& aEventHandler);
    
    // From MEikListBoxObserver
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    
public:
    /**
     * Implements drag gesture handling
     * @see MHgTeleportEventControlerObserver
     */
    void MoveOffset(const TPoint&);

    /**
     * Implements tap gesture handling
     * @see MHgTeleportEventControlerObserver
     */
    void TapL(const TPoint&);

    /**
     * Implements long tap gesture handling
     * @see MHgTeleportEventControlerObserver
     */
    void LongTapL(const TPoint&);

    /**
     * Implements drag gesture handling
     * @see MHgTeleportEventControlerObserver
     */
    void Drag(const AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent);
    
    /**
     * Implements animation stop handling
     * @see MHgTeleportEventControlerObserver
     */
    void Stop();
    
    /**
     * Implements view position getter. Returns point in the middle of the
     * grid view window.
     * 
     * @see MHgTeleportEventControlerObserver
     */
    TPoint ViewPos()const;
    
public:    
    // from CCoeControl    
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void FocusChanged( TDrawNow aDrawNow );
    void SizeChanged();
    void Draw( const TRect& aRect ) const;
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
private:
    // From MHgFswObserver     
    void HandleFswContentChanged();
    
    // From MEikMenuObserver
    void ProcessCommandL( TInt aCommandId );
	void SetEmphasis( CCoeControl*, TBool ) { }
	
	// From MHgTeleportFastSwapGridObserver
	void HandleCloseEventL(TInt aItemIdx);
	
	// From MHgDeviceStateObserver
	void HandleDeviceStateChanged( TChangeType aChangeType );

    // From MHgTeleportFastSwapTimerObserver
    void TimerCompletedL( CHgTeleportFastSwapTimer* aSource );
	
// new functions    

    /**
     * Makes a copy of the given bitmap.
     * Also scaled to the given size, but maintains aspect ratio,
     * so the size of the returned bitmap may be less then aSize.
     */
    CFbsBitmap* CopyBitmapL( TInt aFbsHandle, TSize aSize );

    /**
     * Called from HandleFswContentChanged.
     */    
    void HandleFswContentChangedL();
    
    /**
     * Shows the content from iArray on the screen.
     */
    void RenderContentL();
    
    /**
     * Creates the ganes grid control.
     */
    void ReCreateGridL();
    
    /**
     * Returns rectangles for fast swap area controls
     */
    void GetFastSwapAreaRects( RArray<TAknLayoutRect>& aRects );

    /**
     * Creates a stylus popup instance (iPopup) if not yet done.
     */
    void PreCreatePopupL();

    /**
     * Shows the stylus popup menu if not yet visible.
     * @param   aIndex  index of item in the grid
     * @param   aPoint  position for the popup
     */
    TBool ShowPopupL( TInt aIndex, const TPoint& aPoint );
    
    /**
     * Shows the app close confirmation query.
     * @param   aIndex  index of item in the grid
     * @return ETrue if accepted EFalse if canceled
     */
    TBool ConfirmCloseL( TInt aIndex );

    /**
     * Sends the data-changed notification.
     */   
    void NotifyChange();

    /**
     * Chage application order.
     * move Homescreen to first left position.
     */   
    void SwapApplicationOrder( RPointerArray<CHgFswEntry>& aArray );
    
    /**
     * Retrieves and returns size for image graphics.
     */
    TSize PreferredImageSize();
    
    /**
     * Retrieves number of items in grid
     */
    TInt GridItemCount();
    
    /**
     * Calculates the correct size if aspect ratio needs to be preserved.
     */
    TSize CalculateSizePreserveRatio(const TSize& aTargetAreaSize,
            const TSize& aSourceSize);
    
    /**
     * Selects next item in grid
     */
    void SelectNextItem();
    
    /**
     * Highlight current grid'd item and redraw
     */
    void ShowHighlight();
    
    /**
     * Centralise most centered item so it was most exposed on the view.
     * Centered element is being selected by grid.
     * Animation to the centered element is made after aRedrawDelay time.
     * 
     * @param aRedrawDelay - animation delay.
     */
    void CenterItem( TInt aRedrawDelay);
    
    /**
     * Returns full grid view size.
     */
    TSize GridWorldSize();
    
    /**
     * Updates grid, called for manual grid scroll update
     * 
     * @param  aForceRedraw    if set to ETrue - redraw will be forced
     * @param  aAnimate        set to ETrue to enable animation
     */
    void UpdateGrid( TBool aForceRedraw = EFalse,
                     TBool aAnimate = ETrue );
    
    /**
     * Returns grid view size
     */
    TSize ViewSize();
    
    /**
     * Returns the position of the given item inside grid world rect.
     * Returns point in the middle of the view rectangle.
     * 
     * @param  aItemIdx  index of the item for which calculation will be done
     */
    TPoint ItemViewPosition( TInt aItemIdx );
    
    /**
     * Returns index of the shown item, based on the logical
     * position of the grids view. Item that is closest to the
     * middle of screen is returned.
     * 
     * @param  aViewPos  grids view position. This must be point in
     *                   the middle of the view rectangle
     * @return  index of the item
     */
    TInt ViewToVisibleItem( const TPoint aViewPos );
    
    /**
     * Launches increasing pop-up feedback.
     */
    void LaunchPopupFeedback();

private: // Data
    
    // parent control
    CCoeControl& iParent;
    
    // device state
    // not own 
    CHgTeleportDeviceState& iDeviceState;

    // grid, own
    CHgTeleportFastSwapGrid* iGrid;
        
    // fsw client, own
    CHgFswClient* iFSClient;
    // fsw content, ptrs owned
    RPointerArray<CHgFswEntry> iArray;
    
    // index of selected (highlighted) item in the grid
    // when the fsw was last focused
    TInt iSavedSelectedIndex;
    
    // fsw popup (activate, close, etc.), own
    CAknStylusPopUpMenu* iPopup;
    // the aIndex argument of ShowPopupL is stored here
    TInt iAppIndexForPopup;

    // query shown when pressing Clear key in fsw, own
    CAknQueryDialog* iConfirmCloseQuery;

    // Timestamp of the last application close operation
    TTime iTimeOfLastClose;
    
    // Observer ptr, may be NULL, not owned
    MHgTeleportDataChangeObserver* iDcObserver;
    
    //Input events handler
    CHgTeleportEventControler& iEvtHandler;
    
    TInt iMaxItemsOnScreen;
    TInt iPreviousNoOfItems;
    
    // Tap event
    TPointerEvent iTapEvent;
    CHgTeleportFastSwapTimer* iHighlightTimer;
    TBool iLongTap;
    
    // Grid scrolling
    TInt iGridItemWidth;
    
    CHgTeleportFastSwapTimer* iRedrawTimer; // owned
    
    CHgTeleportFastSwapTimer* iUpdateGridTimer;//owned
    
    // Key event handling
    TBool iConsumeEvent;
    TBool iKeyEvent;
    };

#endif // HGTELEPORTFASTSWAPAREA_H
