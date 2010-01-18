/*
 * ============================================================================
 *  Name        : hgteleporteventcontroler.h
 *  Part of     : Hg Teleport
 *  Description : Gesture and physics helper declaration
 *  Version     : %version:  3 %
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

#ifndef HGTELEPORTEVENTCONTROLER_H
#define HGTELEPORTEVENTCONTROLER_H

#include "hgteleporteventcontrolerobserver.h"
#include <akntouchgesturefw.h>
#include <aknphysicsobserveriface.h>

class CHgTeleportPhysicsEngine;
using namespace AknTouchGestureFw;
/**
 * CHgTeleportEventControler
 * Input events handler. Class analyse pointer events and translate it to gesture. Procerss animations physics
 */
NONSHARABLE_CLASS(CHgTeleportEventControler): 
    public CBase,
    public MAknTouchGestureFwObserver,
    public MAknPhysicsObserver
    {
public:
    /**
     * Two phase constructor. Instance is pushed on cleanup stack.
     * @param aObs - reference to controler observer
     * @param aEventSrc - reference to control which provide pointer events
     * @return address of controler instance
     */
    static CHgTeleportEventControler* NewLC(MHgTeleportEventControlerObserver& aObs, 
                                            CCoeControl& aEventSrc);
    
    /**
     * Two phase constructor.
     * @param aObs - reference to control observer
     * @param aEventSrc - reference to control which provide pointer events
     * @return address of controler instance
     */
    static CHgTeleportEventControler* NewL(MHgTeleportEventControlerObserver& aObs, 
                                           CCoeControl& aEventSrc);
    
    /**
     * Destructor
     */
    ~CHgTeleportEventControler();

private:
    /**
     * First phase construction
     */
    CHgTeleportEventControler(MHgTeleportEventControlerObserver& aObserver);
    
    /**
     * Second phase construction
     * @param aEventSrc - control that provides pointer ivents to handler
     */
    void ConstructL(CCoeControl& aEventSrc);
    
    /**
     * Implements gesture handling
     * @param aEvent - gesture event
     * @see MAknTouchGestureFwObserver
     */
    void HandleTouchGestureL(MAknTouchGestureFwEvent& aEvent);
    
    /**
     * Implements tap gesture handling ( short tap, double tap, long tap )
     * @param aEvent - tap event
     */
    void HandleTapEventL(MAknTouchGestureFwTapEvent& aEvent);
    
    /**
     * Implements drag gesture handling
     * @param aEvent - drag event
     */
    void HandleDragEventL(MAknTouchGestureFwDragEvent& aEvent);
    
    /**
     * Implements flick gesture handling
     * @param aEvent - drag event
     */
    void HandleFlickEventL(MAknTouchGestureFwFlickEvent& aEvent);
    
    //From MAknPhysicsObserver
public:
    /**
     * Physics emulation has moved the view.
     * 
     * When this method is called client should update its visual and logical
     * states to reflect the changes caused by view move. Note that if the
     * parameter @c aDrawNow is @c EFalse client is not allowed to redraw the
     * display but should only update its logical state.
     * Physics engine will ensure that the last frame is always drawn.
     *
     * @since S60 v5.0
     *
     * @param  aNewPosition  The new position of the view.
     * @param  aDrawNow      @c ETrue if client should redraw the screen,
     *                       otherwise @c EFalse.
     * @param  aFlags        Additional flags, not used currently.
     */
    virtual void ViewPositionChanged( const TPoint& aNewPosition,
                              TBool aDrawNow,
                              TUint aFlags );
        
    /**
     * Physics emulation has stopped moving the view
     *
     * @since S60 v5.0
     */
    virtual void PhysicEmulationEnded();

    /**
     * Returns the observer's view position.
     *
     * @since S60 v5.0
     *
     * @return Physics observer's view position.
     */
    virtual TPoint ViewPosition() const;
    
public:
    
    /**
     * Initialize animation between current position and destination point
     * @param aPoint - destination point 
     */
    void Animate(const TPoint& aPoint);

    /**
     * 
     * @param aWorldSize Physics world size.
     * @param aViewSize Physics (visible) view size.
     * @param aLandscape ETrue if physics should work in landscape.     
     **/
    void ReInitPhysicsL(const TSize& aWorldSize, 
                        const TSize& aViewSize,
                        TBool aLandscape);
    
    /**
     * Stops currently ongoing animation
     */
    void StopAnimation();
    
private:
    /**
     * Controler observer. Not own
     */
    MHgTeleportEventControlerObserver& iObserver;
    
    /**
     * Gesture recognition helper. Own
     */
    CAknTouchGestureFw* iGestureHelper;
    
    /**
     * Physics helper. Own
     */
    CHgTeleportPhysicsEngine* iPhysicsHelper;
    };

#endif // HGTELEPORTEVENTCONTROLER_H
