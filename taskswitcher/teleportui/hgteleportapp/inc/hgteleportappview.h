/*
 * ============================================================================
 *  Name        : hgteleportappview.h
 *  Part of     : Hg Teleport
 *  Description : appui
 *  Version     : %version: sa1spcx1#32 %
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

#ifndef HGTELEPORTAPPVIEW_H
#define HGTELEPORTAPPVIEW_H

#include <e32base.h> 
#include <coecntrl.h>
#include <AknPopupFader.h>
#include "hgteleportdevicestate.h"
#include "hgteleportdatachangeobserver.h"
#include "hgteleporteventcontrolerobserver.h"

// FORWARD DECLARATIONS
class CAknsFrameBackgroundControlContext;
class CHgTeleportFastSwapArea;
class CHgTeleportDialog;
class CAknPopupHeadingPane;
class CHgTeleportEventControler;

/**
 * Teleport applicatio view class.
 */
class CHgTeleportAppView : public CCoeControl, public MCoeControlObserver,
        public MHgDeviceStateObserver,
        public MAknFadedComponent,
        public MHgTeleportDataChangeObserver,
        public MHgTeleportEventControlerObserver
    {
public:

    /**
     * Two-phase constructor.
     * 
     * @param aRect Area where view should draw itself.
     */
    static CHgTeleportAppView* NewL(const TRect& aRect,
            CHgTeleportDeviceState& aDeviceState);

    /**
     * @copydoc NewL
     */
    static CHgTeleportAppView* NewLC(const TRect& aRect,
            CHgTeleportDeviceState& aDeviceState);

    /**
     * Destructor.
     */
    ~CHgTeleportAppView();

    /**
     * Called from appui if the app came to foreground.
     */
    void HandleSwitchToForegroundEvent();
    
    /**
     * Called from appui if the app went to background.
     */
    void HandleSwitchToBackgroundEvent();

    /**
     * From CCoeControl      Method handling the pointer events while the fsw is used.
     *
     * @param aPointerEvent  Information about the event.
     */
    void HandlePointerEventL( const TPointerEvent &aPointerEvent );

public:
    // from CCoeControl
    void Draw( const TRect& aRect ) const;
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void SizeChanged();
    
    //Own
    /*
     * Forward app key handling to fasswaparea control.
     */
    void HandleAppKey(TInt aType);

protected:
    // from MCoeControlObserver
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    // from MHgDeviceStateObserver
    void HandleDeviceStateChanged( TChangeType aChangeType );

protected: // from MAknFadedComponent
    TInt CountFadedComponents();
    CCoeControl* FadedComponent( TInt aIndex );

protected: // from MHgCenrepChangeObserver
    void CenrepChanged( TUint32 aKey, TInt aNewValue );

protected: // from MHgTeleportDataChangeObserver
    void DataChanged( CCoeControl* aWhere, TInt aNewCount );

private:

    /**
     * @copydoc NewL
     */
    void ConstructL( const TRect& aRect );

    /**
     * C++ constructor.
     */
    CHgTeleportAppView( CHgTeleportDeviceState& aDeviceState );

    /**
     * Updates the internal state for currently focused control.
     * @param   aNewFocusedControl  ptr to new focused control, ownership not taken
     */    
    void ChangeFocus( CCoeControl* aNewFocusedControl );
    
    /**
     * Creates components.
     */
    void CreateControlsL();
    
    /**
     * Appends component rects to the given array.
     * @param   aRects  array
     */
    void GetRects( RArray<TRect>& aRects );
    
    /**
     * Adds the ptr to the list of dialog pointers.
     * @param   aDlg    dialog ptr, ownership not taken
     */
    void AddDlgSelfPtrL( CHgTeleportDialog* aDlg );
    
    /**
     * Handles key event according to the portrait-style (vertical) layout.
     */
    TKeyResponse OfferKeyEventPortraitStyleL( const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Calculates and returns the following:
     *   - the rectangle for main_tport_pane
     *   - the outer rectangle for the frame
     *   - the inner rectangle for the frame
     */
    void UpdatePopupRects(  );

    /**
     * Updates the text in the applications and suggestions heading panes.
     */
    void UpdateHeadingsL();
    
    /**
     * Disables transition animations for teleport app.
     * (the automatic effects that are applied when coming/leaving foreground)
     */
    void DisableAppSwitchEffects();
    
    
public://From MHgTeleportEventControlerObserver
    /**
     * Declare drag gesture handling.
     * @param aOffset - drag distance
     */
    virtual void MoveOffset(const TPoint& aOffset);
    
    /**
     * Declare tap gesture handling.
     * @param aPoint - tap position
     */
    virtual void TapL(const TPoint& aPoint);
    
    /**
     * Declare long tap gesture handling.
     * @param aPoint - tap position
     */
    virtual void LongTapL(const TPoint& aPoint);
    
    /**
     * 
     */
    virtual void Drag(
        const AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent);
    
    /**
     * Declare animation stop handling
     */
    virtual void Stop();
    
    /**
     * Declare getter for view position
     */
    virtual TPoint ViewPos() const;
        
private: // Data

    // Ref to container for commonly used instances
    CHgTeleportDeviceState& iDeviceState;

    // Background context, owned
    CAknsFrameBackgroundControlContext* iBgContext;
    
    // Fast Swap area, owned
    CHgTeleportFastSwapArea* iFastSwapArea;

    /**
     * Heading pane, own
     */
    CAknPopupHeadingPane* iAppsHeading;

    /**
     * Currently focused control, not own
     */
    CCoeControl* iFocused;
    
    /**
     * Rectangle of the control.
     */
    TRect iViewRect;
    
    /**
     * Popup fader instance.
     */
    TAknPopupFader iPopupFader;
    
    /**
     * Pointer for background image, owned
     */
    TRect iBgContextOuterRect;
    TRect iBgContextInnerRect;
    CHgTeleportEventControler* iEvtHandler; //own
    };

#endif // HGTELEPORTAPPVIEW_H
