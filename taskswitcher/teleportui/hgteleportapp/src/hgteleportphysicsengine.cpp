/*
 * ============================================================================
 *  Name        : hgteleportphysicsengine.cpp
 *  Part of     : Hg Teleport
 *  Description : Teleport physics
 *  Version     : %version:  5 %
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
#include <aknphysics.h>

#include "hgteleportphysicsengine.h"
#include "hgteleportphysics.h"

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsHandler::CHgTeleportPhysicsHandler
// -----------------------------------------------------------------------------
//
CHgTeleportPhysicsEngine::CHgTeleportPhysicsEngine()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::~CHgTeleportPhysicsEngine
// -----------------------------------------------------------------------------
//
CHgTeleportPhysicsEngine::~CHgTeleportPhysicsEngine()
    {
    delete iTeleportPhysics;
    delete iPhysics;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::NewLC
// -----------------------------------------------------------------------------
//
CHgTeleportPhysicsEngine* CHgTeleportPhysicsEngine::NewLC(
        MAknPhysicsObserver& aPhysicObserver, CCoeControl& aViewControl)
    {
    CHgTeleportPhysicsEngine* self = new (ELeave) CHgTeleportPhysicsEngine();
    CleanupStack::PushL(self);
    self->ConstructL(aPhysicObserver, aViewControl);
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::NewL
// -----------------------------------------------------------------------------
//
CHgTeleportPhysicsEngine* CHgTeleportPhysicsEngine::NewL(
        MAknPhysicsObserver& aPhysicObserver, CCoeControl& aViewControl)
    {
    CHgTeleportPhysicsEngine* self = CHgTeleportPhysicsEngine::NewLC(
            aPhysicObserver, aViewControl);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysicsEngine::ConstructL(MAknPhysicsObserver& aPhysicObserver,
        CCoeControl& aViewControl)
    {
    iPhysics = CAknPhysics::NewL(aPhysicObserver, &aViewControl);//TODO:
    iTeleportPhysics = CHgTeleportPhysics::NewL(aPhysicObserver);
    }


// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::IsRunning
// -----------------------------------------------------------------------------
//
TBool CHgTeleportPhysicsEngine::IsRunning() const
    {
    return iPhysics->OngoingPhysicsAction() != CAknPhysics::EAknPhysicsActionNone;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::HandleDragEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysicsEngine::HandleDragEvent(
        AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent)
    {
    if (AknTouchGestureFw::EAknTouchGestureFwStart == aEvent.State())
        {
        iPhysics->StopPhysics();
        iStartTime.HomeTime();
        }
    else if (AknTouchGestureFw::EAknTouchGestureFwOn == aEvent.State())
        {
        TPoint deltaPoint(aEvent.PreviousPosition() - aEvent.CurrentPosition());
        iPhysics->RegisterPanningPosition(deltaPoint);
        }
    else //AknTouchGestureFw::EAknTouchGestureFwStop
        {
        TPoint drag(aEvent.PreviousPosition() - aEvent.CurrentPosition());
        iPhysics->StartPhysics(drag, iStartTime);
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::Stop
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysicsEngine::Stop()
    {
    iPhysics->StopPhysics();
    iTeleportPhysics->StopPhysics();
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::AnimateToTargetL
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysicsEngine::AnimateToTarget(const TPoint& aPoint)
    {
    iTeleportPhysics->StartPhysics(aPoint);
    }

// -----------------------------------------------------------------------------
// CHgTeleportPhysicsEngine::ReInitPhysicsL
// -----------------------------------------------------------------------------
//
void CHgTeleportPhysicsEngine::ReInitPhysicsL(const TSize& aWorldSize,
        const TSize& aViewSize, TBool aLandscape)
    {
    iPhysics->InitPhysicsL(aWorldSize, aViewSize, aLandscape);
    }

//End file
