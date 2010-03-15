/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Algorithm to recognise gesture from a stream of points
*
*/


#ifndef _XNGESTURERECOGNISER_H_
#define _XNGESTURERECOGNISER_H_

// System includes
#include <e32std.h>


/**
 * XnGestureHelper namespace
 * Used for the whole gesture family - Gesture recognizer, gesture helper, 
 * Point array
 */
namespace XnGestureHelper
    {
    
    // Forward declarations
    class TXnPointArray;
    
    /**
     * flag that indicates gesture code is a holding code
     * clients should use MGestureEvent::IsHolding
     */
    const TInt EFlagHold = 0x10000000;
    
    /**
     * Gesture codes and states
     */
    enum TXnGestureCode
        {
        // states
        /** gesture just started (user pressed stylus down)*/ 
        EGestureStart = 0x00000001, 
        /** user moved stylus (may be holding or not holding) */
        EGestureDrag = 0x00000002, 
        /** user lifted stylus while user was holding */
        EGestureReleased = 0x00000003, 
        
        // gestures  
        /** gesture was not recognised */
        EGestureUnknown = 0x00000005, 
        /** these codes are sent when user lifts stylus 
         * (if holding not started)
         */
        EGestureTap = 0x00000006,
        /** first tap emits EGestureTap */
        EGestureDoubleTap = 0x00000007, 
        /** swipe left */
        EGestureSwipeLeft = 0x00000008,
        /** swipe right */
        EGestureSwipeRight = 0x00000009,
        /** swipe up */
        EGestureSwipeUp = 0x0000000A,
        /** swipe down */
        EGestureSwipeDown = 0x0000000B,
        EGestureCanceled = 0x0000000C,
        /** 
         * these codes are sent when user initiates holding by keeping stylus 
         * in same place for a longer duration
         */
        EGestureHoldLeft = EGestureSwipeLeft | EFlagHold,
        EGestureHoldRight = EGestureSwipeRight | EFlagHold,
        EGestureHoldUp = EGestureSwipeUp | EFlagHold,
        EGestureHoldDown = EGestureSwipeDown | EFlagHold
        };
    
    //Class declaration
    
    /**
     *  Set of algorithms to recognise gesture from a stream of points
     *  Note: Not a static class or a function pointer, just to make it 
     *  sligthly easier to replace it with a heavier implementation
     *
     *  @ingroup group_xnlayoutengine
     */
    NONSHARABLE_CLASS( TXnGestureRecogniser )
        {
    public:
        
        /** 
         * Translates points into a gesture code
         * @param aPoints Points that form the gestures. Client is not 
         *        required to pass in repeated points (sequential 
         *        points that are almost in the same place)
         * @param aIsHolding ETrue if gesture was ended at pointer being held 
         *        down at same position
         *        EFalse if gesture ended at pointer being released 
         * @return recognised gesture id or EUnknownGesture
         */
        TXnGestureCode GestureCode( const TXnPointArray& aPoints ) const;
        
        TXnGestureCode LastDirection( const TXnPointArray& aPoints ) const;
        };
    } // namespace XnGestureHelper

#endif // _XNGESTURERECOGNISER_H_
