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
* Description:  Gesture helper implementation
*
*/

// System includes
#include <e32base.h>
#include <w32std.h>

// User includes
#include "xngesturehelper.h"
#include "xngesture.h"
#include "xngesturedefs.h"
#include "xnnode.h"

using namespace XnGestureHelper;

namespace XnGestureHelper
    {
    NONSHARABLE_CLASS( CHoldingTimer ) : public CTimer
        {
    public:
        /** Two-phase constructor */
        static CHoldingTimer* NewL( CXnGestureHelper& aHelper )
            {
            CHoldingTimer* self = new ( ELeave ) CHoldingTimer( aHelper );
            CleanupStack::PushL( self );
            self->ConstructL();
            // "hold event" sending is enabled by default
            self->iIsEnabled = ETrue;
            CActiveScheduler::Add( self );
            CleanupStack::Pop( self );
            return self;
            }

        /** Destructor */
        ~CHoldingTimer()
            {
            Cancel();
            }

        /** Set whether sending holding events is currently enabled */
        void SetEnabled( TBool aEnabled )
            {
            iIsEnabled = aEnabled;
            // cancel in case hold timer is already running
            Cancel();
            }

        /** @return whether sending holding events is currently enabled */
        TBool IsEnabled() const
            {
            return iIsEnabled;
            }

        /** Start the timer. Calls CXnGestureHelper::StartHoldingL upon
          * completion */
        void Start()
            {
            // if sending hold events is disabled, do not ever start the hold
            // timer, and hence hold events will never be triggered
            if ( iIsEnabled )
                {
                Cancel();
                After( KHoldDuration );
                }
            }

    private:
        /** Constructor */
        CHoldingTimer( CXnGestureHelper& aHelper )
            : // give higher priority to new pointer events with - 1
              CTimer( EPriorityUserInput - 1 ),
              iHelper( aHelper )
            {
            }

        void RunL() // From CActive
            {
            iHelper.StartHoldingL();
            }

    private:
        /// helper object that will be called back when timer is triggered
        CXnGestureHelper& iHelper;
        /// whether sending holding events is currently enabled
        TBool iIsEnabled;
        };
    } // namespace GestureHelper

/**
* @return position from event. Use this instead of using aEvent direction to
*         avoid accidentally using TPointerEvent::iPosition
*/
inline TPoint Position( const TPointerEvent& aEvent )
    {
    // use parent position, since the capturer is using full screen area,
    // and because the (Alfred) drag events are not local to visual even when
    // coming from the client
    return aEvent.iParentPosition;
    }

// ----------------------------------------------------------------------------
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CXnGestureHelper* CXnGestureHelper::NewL( CXnNode& aNode )
    {
    CXnGestureHelper* self = new ( ELeave ) CXnGestureHelper( aNode );
    CleanupStack::PushL( self );
    self->iHoldingTimer = CHoldingTimer::NewL( *self );
    self->iGesture = new ( ELeave ) CXnGesture();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CXnGestureHelper::CXnGestureHelper( CXnNode& aNode )
    : iOwner( aNode )
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CXnGestureHelper::~CXnGestureHelper()
    {
    delete iHoldingTimer;
    delete iGesture;
    }

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
void CXnGestureHelper::SetHoldingEnabled( TBool aEnabled )
    {
    iHoldingTimer->SetEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
TBool CXnGestureHelper::IsHoldingEnabled() const
    {
    return iHoldingTimer->IsEnabled();
    }

// ----------------------------------------------------------------------------
// Reset state
// ----------------------------------------------------------------------------
//
void CXnGestureHelper::Reset()
    {
    iHoldingTimer->Cancel();
    iGesture->Reset();
    }

/**
* Helper function that calls Reset on the pointer to CXnGestureHelper
*/
static void ResetHelper( TAny* aHelper )
    {
    static_cast< CXnGestureHelper* >( aHelper )->Reset();
    }

// ----------------------------------------------------------------------------
// Sets gesture destination
// ----------------------------------------------------------------------------
//
void CXnGestureHelper::SetDestination( CXnNode* aDestination )
    {
    iDestination = aDestination;
    }

// ----------------------------------------------------------------------------
// Gets gesture destination
// ----------------------------------------------------------------------------
//    
CXnNode* CXnGestureHelper::Destination() const
    {
    return iDestination;
    }

// ----------------------------------------------------------------------------
// Gets gesture owner
// ----------------------------------------------------------------------------
//    
CXnNode* CXnGestureHelper::Owner() const
    {
    return &iOwner;
    }
    
// ----------------------------------------------------------------------------
// Handle a pointer event
// ----------------------------------------------------------------------------
//
TXnGestureCode CXnGestureHelper::HandlePointerEventL( 
    const TPointerEvent& aEvent )
    {
    TXnGestureCode ret( EGestureUnknown );
    
    switch ( aEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            // If no up event was received during previous gesture, cancel
            // previous event and reset state
            if ( !IsIdle() )
                {
                iGesture->SetCancelled();
                // ambiguous what is the right thing when "cancel" event leaves
                // and "start" does not. Leaving for cancel *after* "start" could
                // be unexpected to client, as client would have handled start
                // event successfully. Assume that leaving upon cancellation
                // can be ignored.
                Reset();
                }
            // adding the first point implicitly makes the state "not idle"
            AddPointL( aEvent );
            // If AddPointL leaves, IsIdle will return EFalse for other events
            // types, hence further pointer events will be ignored.
            // Therefore, holding will NOT be started if AddPointL leaves,
            // since the callback would trigger a gesture callback, and that
            // would access an empty points array.
            iHoldingTimer->Start();
            iDirection = EGestureUnknown;
            break;

        case TPointerEvent::EDrag:
            // ignore the event in case not in "recording" state. this may
            // happen if holding was triggered, or client sends up event after
            // down event was received in a different *client* state, and
            // client did not forward the down event to here.
            // Also, while stylus down, the same event is received repeatedly
            // even if stylus does not move. Filter out by checking if point
            // is the same as the latest point
            iDirection = iGesture->LastDirection( CXnGesture::EAxisHorizontal );
            
            if ( !IsIdle() && !iGesture->IsLatestPoint( Position( aEvent ) ) )
                {
                AddPointL( aEvent );
                if ( !( iGesture->IsHolding() ||
                        iGesture->IsNearHoldingPoint( Position( aEvent ) ) ) )
                    {
                    // restart hold timer, since pointer has moved
                    iHoldingTimer->Start();
                    // Remember the point in which holding was started
                    iGesture->SetHoldingPoint();
                    }
                }
            break;

        case TPointerEvent::EButton1Up:
            // ignore up event if no down event received
            if ( !IsIdle() )
                {
                // reset in case the down event is not received for a reason
                // in client, and instead drag or up events are received.
                // reset via cleanup stack to ensure Reset is run even if
                // observer leaves
                CleanupStack::PushL( TCleanupItem( &ResetHelper, this ) );
                iGesture->SetComplete();
                // if adding of the point fails, notify client with a
                // cancelled event. It would be wrong to send another
                // gesture code when the up point is not known
                if ( AddPoint( aEvent ) != KErrNone )
                    {
                    iGesture->SetCancelled();
                    }
                else
                    {
                    // send gesture code if holding has not been started
                    if ( !iGesture->IsHolding() )
                        {
                        // if client leaves, the state is automatically reset.
                        // In this case the client will not get the released event
                        ret = iDirection;
                        }
                    // send an event that stylus was lifted
                    iGesture->SetReleased();
                    }
                // reset state
                CleanupStack::PopAndDestroy( this );
                }
            break;

        default:
            break;
        }
    
    return ret;
    }

// ----------------------------------------------------------------------------
// Is the helper idle?
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline TBool CXnGestureHelper::IsIdle() const
    {
    return iGesture->IsEmpty();
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline void CXnGestureHelper::AddPointL( const TPointerEvent& aEvent )
    {
    User::LeaveIfError( AddPoint( aEvent ) );
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline TInt CXnGestureHelper::AddPoint( const TPointerEvent& aEvent )
    {
    return iGesture->AddPoint( Position ( aEvent ) );
    }

/**
* Helper function that calls ContinueHolding on the pointer to TGesture
*/
static void ContinueHolding( TAny* aGesture )
    {
    static_cast< CXnGesture* >( aGesture )->ContinueHolding();
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// ----------------------------------------------------------------------------
//
void CXnGestureHelper::StartHoldingL()
    {
    // hold & tap event is specifically filtered out. Use case: in list fast
    // scrolling activation (e.g. enhanced coverflow), tap & hold should not
    // start fast scroll. In addition, after long tap on start position,
    // drag and drag & hold swiping should emit normal swipe and swipe&hold
    // events. Therefore, tap & hold is not supported.
    if ( !iGesture->IsTap() )
        {
        // holding has just started, and gesture code should be provided to client.
        // set gesture state so that it produces a gesture code (other than drag)
        iGesture->StartHolding();

        // create an item in the cleanup stack that will set the gesture state
        // to holding-was-started-earlier state. NotifyL may leave, but the
        // holding-was-started-earlier state must still be successfully set,
        // otherwise, the holding gesture code will be sent twice
        CleanupStack::PushL( TCleanupItem( &ContinueHolding, iGesture ) );

        // set holding state to "post holding"
        CleanupStack::PopAndDestroy( iGesture );
        }
    }

// ----------------------------------------------------------------------------
// Check if swipe is valid
// ----------------------------------------------------------------------------
//
TXnGestureCode CXnGestureHelper::ValidSwipe() const
    {                        
    return iGesture->Code( CXnGesture::EAxisBoth );
    }
