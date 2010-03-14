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

#include "tseventcontroler.h"
#include "tsphysicsengine.h"

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CTsEventControler* CTsEventControler::NewLC(
    MTsEventControlerObserver& aObserver,
    CCoeControl& aEventSrc)
    {
    CTsEventControler* self = 
        new(ELeave)CTsEventControler(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aEventSrc);
    return self;
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CTsEventControler* CTsEventControler::NewL(
    MTsEventControlerObserver& aObserver,
    CCoeControl& aEventSrc)
    {
    CTsEventControler* self = 
        CTsEventControler::NewLC(aObserver, aEventSrc);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// ~CTsPointerHandler
// -----------------------------------------------------------------------------
//
CTsEventControler::~CTsEventControler()
    {
    delete iGestureHelper;
    delete iPhysicsHelper;
    }

// -----------------------------------------------------------------------------
// CTsPointerHandler
// -----------------------------------------------------------------------------
//
CTsEventControler::CTsEventControler(
    MTsEventControlerObserver& aObserver)
    :
    CBase(),
    iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CTsEventControler::ConstructL(CCoeControl& aEventSrc)
    {
    iGestureHelper = 
        AknTouchGestureFw::CAknTouchGestureFw::NewL(*this, aEventSrc);
    iGestureHelper->SetGestureInterestL(EAknTouchGestureFwAll);
    iPhysicsHelper = CTsPhysicsEngine::NewL(*this, aEventSrc);
    }

// -----------------------------------------------------------------------------
// HandleTouchGestureL
// -----------------------------------------------------------------------------
//
void CTsEventControler::HandleTouchGestureL(
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
void CTsEventControler::HandleTapEventL(
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
void CTsEventControler::HandleDragEventL(
    MAknTouchGestureFwDragEvent& aEvent)
    {
    iObserver.DragL(aEvent);
    iPhysicsHelper->HandleDragEvent(aEvent);
    }

// -----------------------------------------------------------------------------
// PhysicEmulationEnded
// -----------------------------------------------------------------------------
//
void CTsEventControler::ViewPositionChanged(const TPoint& aNewPosition,
    TBool /*aDrawNow*/,
    TUint /*aFlags*/)
    {
    iObserver.MoveOffset(aNewPosition);
    }

// -----------------------------------------------------------------------------
// PhysicEmulationEnded
// -----------------------------------------------------------------------------
//
void CTsEventControler::PhysicEmulationEnded()
    {
    iObserver.Stop();
    }

// -----------------------------------------------------------------------------
// ViewPosition
// -----------------------------------------------------------------------------
//
TPoint CTsEventControler::ViewPosition() const
    {
    return iObserver.ViewPos();
    }

// -----------------------------------------------------------------------------
// Animate
// -----------------------------------------------------------------------------
//
void CTsEventControler::Animate(const TPoint& aPoint)
    {
    iPhysicsHelper->AnimateToTarget(aPoint);
    }

// -----------------------------------------------------------------------------
// ReInitPhysicL
// -----------------------------------------------------------------------------
//
void CTsEventControler::ReInitPhysicsL(const TSize& aWorldSize,
        const TSize& aViewSize, TBool aLandscape)
    {
    iPhysicsHelper->ReInitPhysicsL( aWorldSize, aViewSize, aLandscape);
    }

// -----------------------------------------------------------------------------
// StopAnimation
// -----------------------------------------------------------------------------
//
void CTsEventControler::StopAnimation()
    {
    iPhysicsHelper->Stop();
    }

// end of file
