/*
 * ============================================================================
 *  Name        : hgteleportphysicsengine.h
 *  Part of     : Hg Teleport
 *  Description : Teleport physic engine
 *  Version     : %version:  3 %
 *
 *  Copyright © 2009 Nokia.  All rights reserved.
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

#ifndef HGTELEPORTPHYSICSENGINE_H_
#define HGTELEPORTPHYSICSENGINE_H_

#include <e32base.h>
#include <akntouchgesturefw.h>
// CLASS DECLARATION
class CAknPhysics;
class CHgTeleportPhysics;
/**
 * Teleport physic engine implementation
 */
class CHgTeleportPhysicsEngine: public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CHgTeleportPhysicsEngine();

    /**
     * Two-phased constructor.
     */
    static CHgTeleportPhysicsEngine* NewL(MAknPhysicsObserver& aPhysicObserver,
            CCoeControl& aViewControl);

    /**
     * Two-phased constructor.
     */
    static CHgTeleportPhysicsEngine* NewLC(MAknPhysicsObserver& aPhysicObserver,
            CCoeControl& aViewControl);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CHgTeleportPhysicsEngine();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(MAknPhysicsObserver& aPhysicObserver,
            CCoeControl& aViewControl);
    
    
public:
    /**
     * Implements drag gesture handling
     * @param aEvent - drag event
     */
    void HandleDragEvent(AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent);
    
    /**
     */
    TBool IsRunning() const;
    
    /**
     */
    void Stop();
    
    /**
     * Initialize animation between current position and destination point
     * @param aPoint - destination point 
     */
    void AnimateToTarget(const TPoint& aPoint);
    
    /**
     * 
     * @param aWorldSize Physics world size.
     * @param aViewSize Physics (visible) view size.
     * @param aLandscape ETrue if physics should work in landscape.     
     **/
    void ReInitPhysicsL(const TSize& aWorldSize, 
                        const TSize& aViewSize,
                        TBool aLandscape);
    
private:
    /**
     * Physics. 
     * Own.
     */
    CAknPhysics* iPhysics;

    /**
     * Drag start time
     */
    TTime iStartTime;
    
    /**
     * Teleport physics for item switch animation
     */
    CHgTeleportPhysics* iTeleportPhysics;

    };

#endif /* HGTELEPORTPHYSICSENGINE_H_*/
