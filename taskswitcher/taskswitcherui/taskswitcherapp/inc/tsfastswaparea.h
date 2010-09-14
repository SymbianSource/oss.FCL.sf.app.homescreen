/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSFASTSWAPAREA_H
#define TSFASTSWAPAREA_H

#include <e32base.h>
#include <eikmobs.h>
#include "tsdevicestate.h"
#include "tsfswclientobserver.h"
#include "tsfastswapgrid.h"
#include "tseventcontrolerobserver.h"


// FORWARD DECLARATIONS
class CTsFswClient;
class CTsFswEntry;
class CAknStylusPopUpMenu;
class CAknQueryDialog;
class CTsEventControler;
class MTsDataChangeObserver;
class CAknLongTapAnimation;
class CTsFastSwapAreaExtension;
class CAknPointerEventSuppressor;

/**
 * Taskswitcher Fast Swap area UI.
 */
class CTsFastSwapArea: public CCoeControl,
                               public MEikMenuObserver,
                               public MTsFswObserver,
                               public MEikListBoxObserver,
                               public MTsFastSwapGridObserver,
                               private MTsEventControlerObserver,
                               public MTsDeviceStateObserver,
                               public MTsFastSwapTimerObserver
    {
public:
    /**
     * Two-phase constructor.
     * 
     * @param aRect Area where fast swap info & list should draw itself.
     * @param aParent Parent CoeControl. 
     */
    static CTsFastSwapArea* NewL( const TRect& aRect,
            CCoeControl& aParent, CTsDeviceState& aDeviceState,
            CTsEventControler& aEventHandler);

    /**
     * @copydoc NewL
     */
    static CTsFastSwapArea* NewLC( const TRect& aRect,
            CCoeControl& aParent, CTsDeviceState& aDeviceState,
            CTsEventControler& aEventHandler);

    /**
     * Destructor.
     */
    ~CTsFastSwapArea();

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
     * Brings the app with the give Uid into foreground
     */
    void SwitchToApp( const TUid& aUid );
    
    /**
     * Sends close msg to given app.
     * @param   aIndex      index
     * @param   aSuppressRendering  content is not refreshed if ETrue
     */
    void TryCloseAppL( TInt aIndex, TBool aSuppressRendering = EFalse );
    
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
    void SetDataChangeObserver( MTsDataChangeObserver* aDcObserver );

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
    CTsFastSwapArea(CCoeControl& aParent,
            CTsDeviceState& aDeviceState,
            CTsEventControler& aEventHandler);
    
    // From MEikListBoxObserver
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    
public:
    
    enum TsFastSwapAreaGridData {
        EMaxItemsOnScreen = 1,
        EGridItemWidth,
        EGridItemGap
    };
    
    /**
     * Implements drag gesture handling
     * @see MTsEventControlerObserver
     */
    void MoveOffset(const TPoint&, TBool);

    /**
     * Implements tap gesture handling
     * @see MTsEventControlerObserver
     */
    void TapL(const TPoint&);

    /**
     * Implements long tap gesture handling
     * @see MTsEventControlerObserver
     */
    void LongTapL(const TPoint&);

    /**
     * Implements drag gesture handling
     * @see MTsEventControlerObserver
     */
    void DragL(const AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent);
    
    /**
     * Implements animation stop handling
     * @see MTsEventControlerObserver
     */
    void Stop();
    
    /**
     * Implements view position getter. Returns point in the middle of the
     * grid view window.
     * 
     * @see MTsEventControlerObserver
     */
    TPoint ViewPos()const;
    
    /**
     * Checks if the app with the given window group id is closing
     */
    TBool IsAppClosing( TInt aWgId );
    
    /**
     * Checks if app with the given window group id is present on taskswitcher
     * list.
     */
    TBool WgOnTaskList( TInt aWgId );
    
    /**
     * Returns full grid view size.
     */
    TSize GridWorldSize();
    
    /**
     * Retrieves number of items in grid
     */
    TInt GridItemCount();
    
    /**
     * Returnes value of the requested grid data
     */
    TInt GetGridItemData( TsFastSwapAreaGridData aDataType );
    
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
    // From MTsFswObserver     
    void HandleFswContentChanged();
    
    // From MEikMenuObserver
    void ProcessCommandL( TInt aCommandId );
	void SetEmphasis( CCoeControl*, TBool ) { }
	
	// From MTsFastSwapGridObserver
	void HandleCloseEventL(TInt aItemIdx);
	
	// From MTsDeviceStateObserver
	void HandleDeviceStateChanged( TChangeType aChangeType );

    // From MTsFastSwapTimerObserver
    void TimerCompletedL( CTsFastSwapTimer* aSource );
	
// new functions    

    /**
     * Switches to another application.
     */
    void SwitchToApp( TInt aWgId, const TUid& aUid );

    /**
     * Update and prepare data for grid.
     */    
    void GetContentForGrid();
    
    /**
     * Obtain actual data from server.
     */ 
    void GetContentL();
    
    /**
     * Prepare the content from iArray to be shown on the screen.
     */
    void RenderContentL( );
    
    /**
     * Creates the ganes grid control.
     */
    void ReCreateGridL();
    
    /**
     * Setup grid layout
     */
    void LayoutGridL();
    
    /**
     * Setup grid layout for view
     * 
     * @param  aItemCount  number of items in grid
     */
    void LayoutGridViewL( TInt aItemCount );

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
     * Sends the data-changed notification.
     */   
    void NotifyChange();
    
    /**
     * Selects next item in grid
     */
    void SelectNextItem();
    
    /**
     * Centralise most centered item so it was most exposed on the view.
     * Centered element is being selected by grid.
     * Animation to the centered element is made after aRedrawDelay time.
     * 
     * @param aRedrawDelay - animation delay.
     */
    void CenterItem( TInt aRedrawDelay);
    
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
     * Show highlight when disabled and 
     * consume EEventKeyUp that follows after event that swiched on the highlight.
     */
    TKeyResponse ShowHighlightOnKeyEvent(const TKeyEvent& aKeyEvent, TEventCode aType);
    
    /**
     * Cancels long tap animation.
     */
    void CancelLongTapAnimation( TBool aDisablePopup = ETrue );
    
    /**
     * Checks if long tap animation should be shown in
     * a given point.
     * 
     * @param  aHitPoint  point where tapped event is registered
     * @return  ETrue if long animation is allowed for that position
     */
    TBool LongTapAnimForPos( const TPoint& aHitPoint );
    
private: // Data
    
    // parent control
    CCoeControl& iParent;
    
    // utility helper class
    CTsFastSwapAreaExtension* iFastSwapExt;
    
    // device state
    // not own 
    CTsDeviceState& iDeviceState;

    // grid, own
    CTsFastSwapGrid* iGrid;
        
    // fsw client, own
    CTsFswClient* iFSClient;
    // fsw content, ptrs owned
    RPointerArray<CTsFswEntry> iArray;
    
    // index of selected (highlighted) item in the grid
    // when the fsw was last focused
    TInt iSavedSelectedIndex;
    
    // fsw popup (activate, close, etc.), own
    CAknStylusPopUpMenu* iPopup;
    // the aIndex argument of ShowPopupL is stored here
    TInt iAppIndexForPopup;
    
    // Observer ptr, may be NULL, not owned
    MTsDataChangeObserver* iDcObserver;
    
    //Input events handler
    CTsEventControler& iEvtHandler;
    
    TInt iMaxItemsOnScreen;
    
    // Tap event
    TPointerEvent iTapEvent;
    CTsFastSwapTimer* iHighlightTimer;
    TPoint iActivateOnPointerRelease;
    TBool iHandlePointerCandidate;
    
    // View offset position, used by animation physics
    TInt iLogicalViewPosOffset;
    
    // Grid scrolling
    TInt iGridItemWidth;
    TInt iGridItemGap;
    
    CTsFastSwapTimer* iRedrawTimer; // owned
    CTsFastSwapTimer* iUpdateGridTimer;//owned
    CTsFastSwapTimer* iOrientationSignalTimer; // owned
    TInt iPrevScreenOrientation;
    TBool iIgnoreLayoutSwitch;
    
    // Key event handling
    TBool iConsumeEvent;
    TBool iKeyEvent;
    CAknPointerEventSuppressor* iEventSupressor;
    TBool iSupressDrag;
    
    // App closing handling
    RArray<TInt> iIsClosing;
    TInt iWidgetClosingCount;
    TInt iPrevAppCount;
    
    // Long tap animation
    CAknLongTapAnimation* iLongTapAnimation;
    CTsFastSwapTimer* iLongTapAnimationTimer;
    TBool iLongTapAnimationRunning;
    
    // Screenshot handling
    RArray<TInt> iPrevScreenshots;
    };

#endif // TSFASTSWAPAREA_H
