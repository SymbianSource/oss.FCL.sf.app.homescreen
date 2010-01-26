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
* Description:  Application class
 *
*/


#ifndef TSPHYSICS_H
#define TSPHYSICS_H

#include <e32base.h>
#include <e32std.h>

class MAknPhysicsObserver;

/**
 * Simple physics for grid animation
 */
class CTsPhysics : public CActive
    {
public:
    /*
     * Destructor
     */
    ~CTsPhysics();

    /**
     * Two-phased constructor.
     */
    static CTsPhysics* NewL( MAknPhysicsObserver& aObserver );

    /**
     * Two-phased constructor.
     */
    static CTsPhysics* NewLC( MAknPhysicsObserver& aObserver );

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
    CTsPhysics( MAknPhysicsObserver& aObserver );

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

#endif // TSPHYSICS_H
