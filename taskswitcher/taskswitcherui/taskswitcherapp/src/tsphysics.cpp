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


#include <aknphysicsobserveriface.h>

#include "tsphysics.h"

const TInt KSingleItemChangeAnimTime = 1000000; // 1 second
const TInt KAnimationFrameCount = 15; // 15 frames per second

// -----------------------------------------------------------------------------
// CTsPhysics::CTsPhysics
// -----------------------------------------------------------------------------
//
CTsPhysics::CTsPhysics( MAknPhysicsObserver& aObserver ) :
    CActive( EPriorityStandard ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CTsPhysics::NewLC
// -----------------------------------------------------------------------------
//
CTsPhysics* CTsPhysics::NewLC( MAknPhysicsObserver& aObserver )
    {
    CTsPhysics* self = new ( ELeave ) CTsPhysics( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTsPhysics::NewL
// -----------------------------------------------------------------------------
//
CTsPhysics* CTsPhysics::NewL( MAknPhysicsObserver& aObserver )
    {
    CTsPhysics* self = CTsPhysics::NewLC( aObserver );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CTsPhysics::ConstructL
// -----------------------------------------------------------------------------
//
void CTsPhysics::ConstructL()
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
// CTsPhysics::~CTsPhysics
// -----------------------------------------------------------------------------
//
CTsPhysics::~CTsPhysics()
    {
    Cancel();
    iTimer.Close();
    iAnimationSteps.Close();
    }

// -----------------------------------------------------------------------------
// CTsPhysics::DoCancel
// -----------------------------------------------------------------------------
//
void CTsPhysics::DoCancel()
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CTsPhysics::StartPhysics
// -----------------------------------------------------------------------------
//
void CTsPhysics::StartPhysics( const TPoint& aTarget )
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
// CTsPhysics::StopPhysics
// -----------------------------------------------------------------------------
//
void CTsPhysics::StopPhysics()
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
// CTsPhysics::RunL
// -----------------------------------------------------------------------------
//
void CTsPhysics::RunL()
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
// CTsPhysics::RunError
// -----------------------------------------------------------------------------
//
TInt CTsPhysics::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTsPhysics::CalculateAnimationSteps
// -----------------------------------------------------------------------------
//
TInt CTsPhysics::CalculateAnimationSteps( const TPoint& aTarget )
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
