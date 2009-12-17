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
* Description:  Gesture helper constant definitions
*
*/


#ifndef _XNGESTUREDEFS_H_
#define _XNGESTUREDEFS_H_

// System includes
#include <e32std.h>

/** 
 * This flag controls whether helpers emits a double tap or a tap gesture 
 * for a second tap 
 */
#define _GESTURE_DOUBLE_TAP_SUPPORT

/**
 * XnGestureHelper namespace
 * Used for the whole gesture family - Gesture recognizer, gesture helper, 
 * Point array
 */
namespace XnGestureHelper
    {
    /** time to hold the stylus in the same position to activate 
     * holding, in microseconds 
     */
    const TInt KHoldDuration = 500000; 

    /** 
     * time to tap again for the second tap to be a double tap, in 
     * microseconds 
     */
    const TInt KMaxDoubleTapDuration = 400000; 

    /** 
     * time in which speed becomes zero if user stops stylus movement before
     * lifting the pointer, in microseconds
     */ 
    const TInt KSpeedStopTime = 100000; 

    /** 
     * tolerance in degrees to either side of an angle, for direction to be 
     * the same as reference angle
     */
    const TReal KAngleTolerance = 25;
    
    /**  
     * maximum pointer movement from pointer down position for a gesture be 
     * a tap or hold gesture
     */
    const TInt KSamePointTolerance = 3;
    
    /** 
     * minimum length of a valid swipe in pixels. should be larger than 
     * KTapTolerance
     */
    const TReal KMinSwipeLength = 5;
    
    /** Nth pointer event before the latest point */
    const TInt KPreviousPointOffset = 3;
    
    
    /** 
     * Panic codes 
     */
    enum TGesturePanic
        {
        EGesturePanicIllegalLogic
        };
    
    /** 
     * Panics the thread 
     * 
     * @param aReason Panic reason
     */
    inline void Panic( TGesturePanic aReason )
        {
        _LIT( KComponentName, "XnGestureHelper" );
        User::Panic( KComponentName, aReason );
        }   
    } // namespace XnGestureHelper

#endif // _XNGESTUREDEFS_H_
