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
* Description:  Gesture and physics helper declaration
 *
*/


#ifndef TSEVENTCONTROLER_H
#define TSEVENTCONTROLER_H

#include "tseventcontrolerobserver.h"
#include <akntouchgesturefw.h>
#include <aknphysicsobserveriface.h>

class CTsPhysicsEngine;
using namespace AknTouchGestureFw;
/**
 * CTsEventControler
 * Input events handler. Class analyse pointer events and translate it to gesture. Procerss animations physics
 */
NONSHARABLE_CLASS(CTsEventControler): 
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
    static CTsEventControler* NewLC(MTsEventControlerObserver& aObs, 
                                            CCoeControl& aEventSrc);
    
    /**
     * Two phase constructor.
     * @param aObs - reference to control observer
     * @param aEventSrc - reference to control which provide pointer events
     * @return address of controler instance
     */
    static CTsEventControler* NewL(MTsEventControlerObserver& aObs, 
                                           CCoeControl& aEventSrc);
    
    /**
     * Destructor
     */
    ~CTsEventControler();

private:
    /**
     * First phase construction
     */
    CTsEventControler(MTsEventControlerObserver& aObserver);
    
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
    MTsEventControlerObserver& iObserver;
    
    /**
     * Gesture recognition helper. Own
     */
    CAknTouchGestureFw* iGestureHelper;
    
    /**
     * Physics helper. Own
     */
    CTsPhysicsEngine* iPhysicsHelper;
    };

#endif // TSEVENTCONTROLER_H
