/*
 * ============================================================================
 *  Name        : hgteleporteventcontroler.cpp
 *  Part of     : Hg Teleport
 *  Description : Gesture and physics helper declaration
 *  Version     : %version: 5 %
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
#include "hgteleporteventcontroler.h"
#include "hgteleportphysicsengine.h"

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CHgTeleportEventControler* CHgTeleportEventControler::NewLC(
    MHgTeleportEventControlerObserver& aObserver,
    CCoeControl& aEventSrc)
    {
    CHgTeleportEventControler* self = 
        new(ELeave)CHgTeleportEventControler(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aEventSrc);
    return self;
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CHgTeleportEventControler* CHgTeleportEventControler::NewL(
    MHgTeleportEventControlerObserver& aObserver,
    CCoeControl& aEventSrc)
    {
    CHgTeleportEventControler* self = 
        CHgTeleportEventControler::NewLC(aObserver, aEventSrc);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// ~CHgTeleportPointerHandler
// -----------------------------------------------------------------------------
//
CHgTeleportEventControler::~CHgTeleportEventControler()
    {
    delete iGestureHelper;
    delete iPhysicsHelper;
    }

// -----------------------------------------------------------------------------
// CHgTeleportPointerHandler
// -----------------------------------------------------------------------------
//
CHgTeleportEventControler::CHgTeleportEventControler(
    MHgTeleportEventControlerObserver& aObserver)
    :
    CBase(),
    iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::ConstructL(CCoeControl& aEventSrc)
    {
    iGestureHelper = 
        AknTouchGestureFw::CAknTouchGestureFw::NewL(*this, aEventSrc);
    iGestureHelper->SetGestureInterestL(EAknTouchGestureFwAll);
    iPhysicsHelper = CHgTeleportPhysicsEngine::NewL(*this, aEventSrc);
    }

// -----------------------------------------------------------------------------
// HandleTouchGestureL
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::HandleTouchGestureL(
    AknTouchGestureFw::MAknTouchGestureFwEvent& aEvent)
    {
    if (AknTouchGestureFwEventDrag(aEvent))
        {
        HandleDragEventL(*AknTouchGestureFwEventDrag(aEvent));
        }
    else if (AknTouchGestureFwEventTap(aEvent))
        {
        HandleTapEventL(*AknTouchGestureFwEventTap(aEvent));
        }
    //ignore flick and pinch events
    }

// -----------------------------------------------------------------------------
// HandleTapEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::HandleTapEventL(
    MAknTouchGestureFwTapEvent& aEvent)
    {
    if(EAknTouchGestureFwLongTap == aEvent.Type())
        {
        iObserver.LongTapL(aEvent.Position());
        }
    else if(EAknTouchGestureFwTap == aEvent.Type())
        {
        if( iPhysicsHelper->IsRunning())
            {
            iPhysicsHelper->Stop();
            }
        else
            {
            iObserver.TapL(aEvent.Position());
            }
        }
    }

// -----------------------------------------------------------------------------
// HandleDragEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::HandleDragEventL(
    MAknTouchGestureFwDragEvent& aEvent)
    {
    iObserver.Drag(aEvent);
    iPhysicsHelper->HandleDragEvent(aEvent);
    }

// -----------------------------------------------------------------------------
// PhysicEmulationEnded
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::ViewPositionChanged(const TPoint& aNewPosition,
    TBool /*aDrawNow*/,
    TUint /*aFlags*/)
    {
    iObserver.MoveOffset(aNewPosition);
    }

// -----------------------------------------------------------------------------
// PhysicEmulationEnded
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::PhysicEmulationEnded()
    {
    iObserver.Stop();
    }

// -----------------------------------------------------------------------------
// ViewPosition
// -----------------------------------------------------------------------------
//
TPoint CHgTeleportEventControler::ViewPosition() const
    {
    return iObserver.ViewPos();
    }

// -----------------------------------------------------------------------------
// Animate
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::Animate(const TPoint& aPoint)
    {
    iPhysicsHelper->AnimateToTarget(aPoint);
    }

// -----------------------------------------------------------------------------
// ReInitPhysicL
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::ReInitPhysicsL(const TSize& aWorldSize,
        const TSize& aViewSize, TBool aLandscape)
    {
    iPhysicsHelper->ReInitPhysicsL( aWorldSize, aViewSize, aLandscape);
    }

// -----------------------------------------------------------------------------
// StopAnimation
// -----------------------------------------------------------------------------
//
void CHgTeleportEventControler::StopAnimation()
    {
    iPhysicsHelper->Stop();
    }

// end of file
