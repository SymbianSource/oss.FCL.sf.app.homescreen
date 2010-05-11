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
* Description:  appui
 *
*/


#ifndef TSAPPVIEW_H
#define TSAPPVIEW_H

#include <e32base.h> 
#include <coecntrl.h>
#include <AknPopupFader.h>
#include <touchfeedback.h>
#include "tsdevicestate.h"
#include "tsdatachangeobserver.h"
#include "tseventcontrolerobserver.h"

// FORWARD DECLARATIONS
class CAknsFrameBackgroundControlContext;
class CTsFastSwapArea;
class CAknPopupHeadingPane;
class CTsEventControler;

/**
 * Taskswitcher applicatio view class.
 */
class CTsAppView : public CCoeControl, public MCoeControlObserver,
        public MTsDeviceStateObserver,
        public MAknFadedComponent,
        public MTsDataChangeObserver,
        public MTsEventControlerObserver
    {
public:

    /**
     * Two-phase constructor.
     * 
     * @param aRect Area where view should draw itself.
     */
    static CTsAppView* NewL(const TRect& aRect,
            CTsDeviceState& aDeviceState,
            RWindowGroup& aWg);

    /**
     * @copydoc NewL
     */
    static CTsAppView* NewLC(const TRect& aRect,
            CTsDeviceState& aDeviceState,
            RWindowGroup& aWg);

    /**
     * Destructor.
     */
    ~CTsAppView();

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
    
    /**
     * Orders window invalidation to perform full
     * background redraw.
     */
    void OrderFullWindowRedraw();
    
    /**
     * Orders window invalidation to perform full
     * background redraw.
     */
    void EnableDragEvents( TBool aEnable );
    
    /**
     * Checks if app with the given wg id is closing
     * 
     * @param  aWgId  id of window group which is checked
     *                if it is closing
     * @return  ETrue if application is being closed, EFalse otherwise
     */
    TBool AppCloseInProgress( TInt aWgId );
    
    /**
     * Checks if app with the given window group id is present on taskswitcher
     * list.
     * 
     * @param  aWgId  window group of the application to be checked
     * @return  ETrue if there is application with the given id is on the list
     */
    TBool WgOnTaskList( TInt aWgId );

protected:
    // from MCoeControlObserver
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    // from MTsDeviceStateObserver
    void HandleDeviceStateChanged( TChangeType aChangeType );

protected: // from MAknFadedComponent
    TInt CountFadedComponents();
    CCoeControl* FadedComponent( TInt aIndex );

protected: // from MTsCenrepChangeObserver
    void CenrepChanged( TUint32 aKey, TInt aNewValue );

protected: // from MTsDataChangeObserver
    void DataChanged( CCoeControl* aWhere, TInt aNewCount );

private:

    /**
     * @copydoc NewL
     */
    void ConstructL( const TRect& aRect, RWindowGroup& aWg );

    /**
     * C++ constructor.
     */
    CTsAppView( CTsDeviceState& aDeviceState );

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
    void UpdateHeadingsL( TInt aNewCount );
    
    /**
     * Disables transition animations for taskswitcher app.
     * (the automatic effects that are applied when coming/leaving foreground)
     */
    void DisableAppSwitchEffects();
    
    /**
     * Launches feedback through MTouchFeedback::Instance()
     * 
     * @param aType         - The logical feedback type to play.
     * @param aFeedbackType - Feedback types to be played as a bitmask 
     *                        combination of enumeration items from
     *                        TTouchFeedbackType
     * @param aPointerEvent - Pointer event, which triggered this feedback.
     */
    void LaunchFeedback( TTouchLogicalFeedback aType,
		TTouchFeedbackType aFeedbackType,
		const TPointerEvent& aPointerEvent);
    
    /**
     * Area, where drag events are being handled.
     */
    TRect DragArea();
        
public://From MTsEventControlerObserver
    /**
     * Declare drag gesture handling.
     * @param aOffset - drag distance
     */
    virtual void MoveOffset(const TPoint& aOffset, TBool aDrawNow);
    
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
    virtual void DragL(
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
    CTsDeviceState& iDeviceState;

    // Background context, owned
    CAknsFrameBackgroundControlContext* iBgContext;
    
    // Fast Swap area, owned
    CTsFastSwapArea* iFastSwapArea;

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
    CTsEventControler* iEvtHandler; //own
    
    /**
     * Flag for marking exit on pointer events
     */
    TBool iExitOnPointerUp;
    };

#endif // TSAPPVIEW_H
