/*
 * ============================================================================
 *  Name        : hgteleportphysics.h
 *  Part of     : Hg Teleport
 *  Description : Application class
 *  Version     : %version:  %
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

#ifndef HGTELEPORTPHYSICS_H
#define HGTELEPORTPHYSICS_H

#include <e32base.h>
#include <e32std.h>

class MAknPhysicsObserver;

/**
 * Simple physics for grid animation
 */
class CHgTeleportPhysics : public CActive
    {
public:
    /*
     * Destructor
     */
    ~CHgTeleportPhysics();

    /**
     * Two-phased constructor.
     */
    static CHgTeleportPhysics* NewL( MAknPhysicsObserver& aObserver );

    /**
     * Two-phased constructor.
     */
    static CHgTeleportPhysics* NewLC( MAknPhysicsObserver& aObserver );

public: // New functions
    
    /**
     * Starts animation
     * 
     * @param  aDrag       next drag point
     * @param  aStartTime  when animation should be started
     */
    void StartPhysics( const TPoint& aTarget );

    /**
     * Stops animation
     */
    void StopPhysics();
    
private:
    
    /**
     * C++ constructor
     */
    CHgTeleportPhysics( MAknPhysicsObserver& aObserver );

    /**
     * Second-phase constructor
     */
    void ConstructL();

private: // From CActive
    
    /**
     * Handles completion
     */
    void RunL();

    /**
     * Called when cancelled
     */
    void DoCancel();

    /**
     * Hanldes error
     */
    TInt RunError( TInt aError );
    
private: // New functions
    
    /**
     * Calculates animation steps
     */
    TInt CalculateAnimationSteps( const TPoint& aTarget );

private:

    enum TPhysicsStatus
        {
        EStopped,
        ERunning,
        EFinished
        };
    
    /**
     * Observer
     */
    MAknPhysicsObserver& iObserver;
    
    /**
     * Animation timer
     */
    RTimer iTimer;
    
    /**
     * Animation state
     */
    TPhysicsStatus iPhysicsState;
    
    /**
     * Controls animation refresh rate
     */
    TTimeIntervalMicroSeconds32 iAnimationTickTime;
    
    /**
     * Animation ticks counter
     */
    TInt iAnimationTicks;
    
    /**
     * Offset of how much view should be moved
     */
    RArray<TPoint> iAnimationSteps;

    };

#endif // HGTELEPORTPHYSICS_H
