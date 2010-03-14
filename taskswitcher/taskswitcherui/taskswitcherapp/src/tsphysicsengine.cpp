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
* Description:  Taskswitcher physics
 *
*/

#include <aknphysics.h>

#include "tsphysicsengine.h"
#include "tsphysics.h"

// -----------------------------------------------------------------------------
// CTsPhysicsHandler::CTsPhysicsHandler
// -----------------------------------------------------------------------------
//
CTsPhysicsEngine::CTsPhysicsEngine()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::~CTsPhysicsEngine
// -----------------------------------------------------------------------------
//
CTsPhysicsEngine::~CTsPhysicsEngine()
    {
    delete iTaskswitcherPhysics;
    delete iPhysics;
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::NewLC
// -----------------------------------------------------------------------------
//
CTsPhysicsEngine* CTsPhysicsEngine::NewLC(
        MAknPhysicsObserver& aPhysicObserver, CCoeControl& aViewControl)
    {
    CTsPhysicsEngine* self = new (ELeave) CTsPhysicsEngine();
    CleanupStack::PushL(self);
    self->ConstructL(aPhysicObserver, aViewControl);
    return self;
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::NewL
// -----------------------------------------------------------------------------
//
CTsPhysicsEngine* CTsPhysicsEngine::NewL(
        MAknPhysicsObserver& aPhysicObserver, CCoeControl& aViewControl)
    {
    CTsPhysicsEngine* self = CTsPhysicsEngine::NewLC(
            aPhysicObserver, aViewControl);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::ConstructL
// -----------------------------------------------------------------------------
//
void CTsPhysicsEngine::ConstructL(MAknPhysicsObserver& aPhysicObserver,
        CCoeControl& aViewControl)
    {
    iPhysics = CAknPhysics::NewL(aPhysicObserver, &aViewControl);//TODO:
    iTaskswitcherPhysics = CTsPhysics::NewL(aPhysicObserver);
    }


// -----------------------------------------------------------------------------
// CTsPhysicsEngine::IsRunning
// -----------------------------------------------------------------------------
//
TBool CTsPhysicsEngine::IsRunning() const
    {
    return iPhysics->OngoingPhysicsAction() != CAknPhysics::EAknPhysicsActionNone;
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::HandleDragEvent
// -----------------------------------------------------------------------------
//
void CTsPhysicsEngine::HandleDragEvent(
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
        iStartTime.HomeTime();
        }
    else //AknTouchGestureFw::EAknTouchGestureFwStop
        {
        TPoint drag(aEvent.PreviousPosition() - aEvent.CurrentPosition());
        iPhysics->StartPhysics(drag, iStartTime);
        }
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::Stop
// -----------------------------------------------------------------------------
//
void CTsPhysicsEngine::Stop()
    {
    iPhysics->StopPhysics();
    iTaskswitcherPhysics->StopPhysics();
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::AnimateToTargetL
// -----------------------------------------------------------------------------
//
void CTsPhysicsEngine::AnimateToTarget(const TPoint& aPoint)
    {
    iTaskswitcherPhysics->StartPhysics(aPoint);
    }

// -----------------------------------------------------------------------------
// CTsPhysicsEngine::ReInitPhysicsL
// -----------------------------------------------------------------------------
//
void CTsPhysicsEngine::ReInitPhysicsL(const TSize& aWorldSize,
        const TSize& aViewSize, TBool aLandscape)
    {
    iPhysics->InitPhysicsL(aWorldSize, aViewSize, aLandscape);
    }

//End file
