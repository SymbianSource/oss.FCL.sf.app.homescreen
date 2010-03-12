/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Taskswitcher physic engine
 *
*/


#ifndef TSPHYSICSENGINE_H_
#define TSPHYSICSENGINE_H_

#include <e32base.h>
#include <akntouchgesturefw.h>
// CLASS DECLARATION
class CAknPhysics;
class CTsPhysics;
/**
 * Taskswitcher physic engine implementation
 */
class CTsPhysicsEngine: public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CTsPhysicsEngine();

    /**
     * Two-phased constructor.
     */
    static CTsPhysicsEngine* NewL(MAknPhysicsObserver& aPhysicObserver,
            CCoeControl& aViewControl);

    /**
     * Two-phased constructor.
     */
    static CTsPhysicsEngine* NewLC(MAknPhysicsObserver& aPhysicObserver,
            CCoeControl& aViewControl);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CTsPhysicsEngine();

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
     * Checks if the physics is running
     */
    TBool IsRunning() const;
    
    /**
     * Checks if the physics is running drag
     */
    TBool IsDragging() const;
    
    /**
     * Stops physics
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
     * Taskswitcher physics for item switch animation
     */
    CTsPhysics* iTaskswitcherPhysics;

    };

#endif /* TSPHYSICSENGINE_H_*/
