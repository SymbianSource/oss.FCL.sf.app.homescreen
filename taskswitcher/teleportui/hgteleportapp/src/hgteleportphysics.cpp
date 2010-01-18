/*
 * ============================================================================
 *  Name        : hgteleportphysics.cpp
 *  Part of     : Hg Teleport
 *  Description : Application class
 *  Version     : %version:  4 %
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

#include <aknphysicsobserveriface.h>

#include "hgteleportphysics.h"

const TInt KSingleItemChangeAnimTime = 1000000; // 1 second
const TInt KAnimationFrameCount = 15; // 15 frames per second

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::CHgTeleportPhysics
// -----------------------------------------------------------------------------
//
CHgTeleportPhysics::CHgTeleportPhysics( MAknPhysicsObserver& aObserver ) :
    CActive( EPriorityStandard ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::NewLC
// -----------------------------------------------------------------------------
//
CHgTeleportPhysics* CHgTeleportPhysics::NewLC( MAknPhysicsObserver& aObserver )
    {
    CHgTeleportPhysics* self = new ( ELeave ) CHgTeleportPhysics( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::NewL
// -----------------------------------------------------------------------------
//
CHgTeleportPhysics* CHgTeleportPhysics::NewL( MAknPhysicsObserver& aObserver )
    {
    CHgTeleportPhysics* self = CHgTeleportPhysics::NewLC( aObserver );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysics::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    // Animation parameters
    const TInt KInitValue = 0;
    iAnimationTickTime = KSingleItemChangeAnimTime / KSingleItemChangeAnimTime;
    iPhysicsState = EStopped;
    iAnimationTicks = KInitValue;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::~CHgTeleportPhysics
// -----------------------------------------------------------------------------
//
CHgTeleportPhysics::~CHgTeleportPhysics()
    {
    Cancel();
    iTimer.Close();
    iAnimationSteps.Close();
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::DoCancel
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysics::DoCancel()
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::StartPhysics
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysics::StartPhysics( const TPoint& aTarget )
    {
    Cancel();
    // Setup animation
    TPoint currentPoint = iObserver.ViewPosition();
    if ( currentPoint.iX != aTarget.iX )
        {
        iAnimationTicks = 0;
        if ( CalculateAnimationSteps( aTarget ) == KErrNone )
            {
            // Request
            iTimer.After( iStatus, 0 );
            SetActive();
            }
        else
            {
            // If calculation failes (no memory) or there is
            // only a small step, set view to target
            iObserver.ViewPositionChanged( aTarget, ETrue, 0);
            iObserver.PhysicEmulationEnded();
            }
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::StopPhysics
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysics::StopPhysics()
    {
    Cancel();
    if ( iPhysicsState == ERunning || iPhysicsState == EFinished )
        {
        iObserver.PhysicEmulationEnded();
        }
    iPhysicsState = EStopped;
    iAnimationTicks = 0;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::RunL
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysics::RunL()
    {
    if ( iPhysicsState == EStopped )
        {
        // Start animation
        iPhysicsState = ERunning;
        iTimer.After( iStatus, iAnimationTickTime );
        SetActive();
        }
    else if ( iPhysicsState == ERunning )
        {
        // Ongoing animation
        if ( iAnimationTicks >= 0 && iAnimationTicks < KAnimationFrameCount )
            {
            iObserver.ViewPositionChanged( iObserver.ViewPosition()+iAnimationSteps[iAnimationTicks], ETrue, 0 );
            }
        iAnimationTicks++;
        if ( iAnimationTicks >= KAnimationFrameCount )
            {
            iPhysicsState = EFinished;
            }
        iTimer.After( iStatus, iAnimationTickTime );
        SetActive();
        }
    else if ( iPhysicsState == EFinished )
        {
        iAnimationTicks = 0;
        iPhysicsState = EStopped;
        iObserver.PhysicEmulationEnded();
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::RunError
// -----------------------------------------------------------------------------
//
TInt CHgTeleportPhysics::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysics::CalculateAnimationSteps
// -----------------------------------------------------------------------------
//
TInt CHgTeleportPhysics::CalculateAnimationSteps( const TPoint& aTarget )
    {
    TInt retVal( KErrNone );
    iAnimationSteps.Reset();
    retVal = iAnimationSteps.Reserve( KAnimationFrameCount );
    if ( retVal == KErrNone )
        {
        TInt yValue = aTarget.iY;
        TPoint currentPos = iObserver.ViewPosition();
        TInt moveLen = aTarget.iX - currentPos.iX;
        if ( moveLen > KAnimationFrameCount || moveLen < -KAnimationFrameCount )
            {
            TInt singleStep = moveLen / KAnimationFrameCount;
            for ( TInt i = 0; i < KAnimationFrameCount - 1; i++ )
                {
                iAnimationSteps.Append( TPoint( singleStep, yValue ) );
                }
            TInt lastStep = moveLen - ( ( KAnimationFrameCount - 1 ) * singleStep );
            iAnimationSteps.Append( TPoint( lastStep, yValue ) );
            }
        else
            {
            retVal = KErrArgument;
            }
        }
    return retVal;
    }
