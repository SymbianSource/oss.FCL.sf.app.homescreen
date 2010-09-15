/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View switcher.
*
*/

#ifndef _XNVIEWSWITCHER_H
#define _XNVIEWSWITCHER_H

//  System includes
#include <e32base.h>
#include <akntouchgesturefw.h>

// User includes
#include "xnviewmanager.h"

class CXnViewControlAdapter;

using namespace AknTouchGestureFw;

// Class declaration

/**
 * This class handles view swithing when "finger follow" feature is used. 
 * The windows of current, next and previous views are moved along with the finger. 
 * Final view switch animation has been handled here as well.
 * 
*  @ingroup group_xnlayoutengine
*  @lib xn3layoutengine.lib
*  @since S60 5.2
*/   
NONSHARABLE_CLASS( CXnViewSwitcher ) : public CBase
   {

    /**
     * Important information of a view to be dragged.
     */
    struct TViewInformation
        {
        CXnControlAdapter*  iAdapter;
        CXnViewData*        iViewData;
        TBool               iReadyToDraw;
        TBool               iBgDrawn;
        };
    
    /**
     * States for the view swithcing using "finger follow".
     */
    enum TViewSwitchState
        {
        KViewSwitchIdle,
        KViewSwitchBegin,
        KViewSwitchDragOngoing,
        KViewSwitchScroll
        };
    
public:
    // Constructors and destructor
    
    /**
     * 2 phase construction.
     */
    static CXnViewSwitcher* NewL();

    /**
     * Destructor.
     */
    ~CXnViewSwitcher();

    /**
     * Is finger follow feature activated
     */
    TBool FingerFollowSupported() const;

    /**
     * Try to handle and consume pointer event
     */
    TBool ProcessPointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * Check if flick or some gesture occured during drag
     */            
    void TouchGesture( TAknTouchGestureFwType& aTouchGesture );

    /**
     * Updates the client rect
     */            
    void SizeChanged( TRect aRect );

    /**
     * Stop view switch
     */            
    void StopViewSwitchL();
    
private: // constructors
    
    /**
     * C++ default constructor
     */    
    CXnViewSwitcher();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /**
     * View scroll timer callback
     */    
    static TInt TimerCallback( TAny *aPtr );

    /**
     * Gets value of finger follow support from AI repository
     */    
    TBool IsFingerFollowSupportedL() const;

    /**
     * Adjusts current view position. Used in final animation.
     */    
    void Scroll();

    /**
     * Moves windows according to the current view position.
     */    
    void DoScroll();
    
    /**
     * Start view dragging by setting up view control adapters
     */    
    TBool SetupViewsL();

    /**
     * Layouts and renders the given view out of sight. 
     */    
    void PrepareViewL( CXnViewSwitcher::TViewInformation& aView );
    
    /**
     * Removes focused node and sends the up event in order to prevent activate triggers and long tapping.
     */
    void StartViewDraggingL();

    /**
     * Stop view dragging and start view scroll
     */    
    void StartViewScrolling();

    /**
     * View scrolling ended. 
     */    
    void ViewSwitchEndedL();

    /**
     * Get view data
     */    
    CXnViewData* ViewData( CXnViewData& aCurrentViewData, TInt aView );

    /**
     * Clear view information of current, next and previous views.
     */    
    void ClearViews();
    
    /**
     * Sets whether next, current or previous view is to be activeted. 
     * Updated also background if needed.
     */    
    void UpdateViewToBeActivated( TInt aOffset );

    /**
     * Changes background image and runs NGA effect. 
     */    
    void ChangeBackground( CXnViewData& aCurrent, CXnViewData& aNext );

private: // data
    
    /** 
     * AppUi 
     */
    CXnAppUiAdapter& iAppUi;

    /** 
     * Client rect set by UI Engine 
     */
    TRect iRect;
    
    /** 
     * Whether the feature is supported or not. ETrue when finger follow is supported 
     */
    TBool iFingerFollowSupported;

    /**
     * Timer for animating view switch.
     * Own.  
     */    
    CPeriodic* iTimer;

    /** 
     * Important information of the current view. 
     */
    TViewInformation iCurrentView;

    /** 
     * Important information of the view coming from the right side. 
     */
    TViewInformation iNextView;    

    /** 
     * Important information of the view coming from the left side. 
     */
    TViewInformation iPreviousView;

    /**
     * Flick gesture type. 
     */
    //TAknTouchGestureFwType iTouchGesture;
    
    /**
     * The state of view switch. 
     */
    TViewSwitchState iViewSwitchState;
    
    /**
     * Scroll speed in pixels. 
     */
    TInt iScrollSpeed;

    /**
     * Total scroll distance from the beginning in mode KViewSwitchDragOngoing.
     * Remaining distance to be scrolled in mode KViewSwitchScroll. 
     */
    TInt iScrollDistance;

    /**
     * Measures the scroll distance in current direction. I.e. if direction changes, 
     * this is set to 0. 
     */
    TInt iScrollDirectionDistance;

    /**
     * Position of the current view in the beginning. 
     */
    TPoint iViewStartPosition;

    /**
     * Position in the beginning. 
     */
    TPoint iStartPosition;

    /**
     * Current view position from the beginning. 
     */
    TPoint iViewPosition;

    /**
     * View to be activated. 
     */
    TInt iActivateView;
    
    /**
     * Tick amount to be used when calculating drag speed. 
     */    
    TInt iTickAmount;

    /**
     * Scroll direction. 
     */    
    TInt iDirection;
    
   };
    
#endif      // _XNVIEWSWITCHER_H
            
// End of File
