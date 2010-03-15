/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture class
*
*/


#ifndef _XNGESTURE_H_
#define _XNGESTURE_H_

// System includes
#include <e32base.h>

// User includes
#include "xnpointarray.h"
#include "xngesturerecogniser.h"

// Namespace declaration

/**
 * XnGestureHelper namespace
 * Used for the whole gesture family - Gesture recognizer, gesture helper, 
 * Point array
 */
namespace XnGestureHelper
    {
    
    /**
     * Struct for a point in floating-point represanation 
     */
    NONSHARABLE_STRUCT( TRealPoint )
        {
        inline TRealPoint();
        inline TRealPoint( const TRealPoint& aPoint );
        inline TRealPoint( TReal aX, TReal aY );
        inline TBool operator==( const TRealPoint& aPoint ) const;

        TReal32 iX;
        TReal32 iY;
        };
    
    // Class declaration

    /**  
     *  TGesture represents the gesture that the user has made.
     *  Implements MGestureEvent interface to allow clients to 
     *  inspect the gesture
     *  
     *  @ingroup group_xnlayoutengine 
     */
    NONSHARABLE_CLASS( CXnGesture ) : public CBase
        {
    public:
        
        /** 
         * X and Y axes, or both 
         */
        enum TAxis
            {
            EAxisBoth, 
            EAxisHorizontal, 
            EAxisVertical
            };

    public:
        
        /** destructor */
        ~CXnGesture();

        /** 
         * Sets the gesture as empty. resets timers to receive points
         * immediately
         */
        void Reset();

        /** 
         * Check the gesture if it is empty
         * 
         * @return ETrue if the gesture has no points 
         */
        TBool IsEmpty() const;

        /**
         * Add a point to the sequence of points that forms the gesture
         * Call Reset() just before adding the first point
         * 
         * @param aPoint the point to add
         * @return error code
         */
        TInt AddPoint( const TPoint& aPoint );

        /** 
         * Checks if the point it is close to holding point
         * 
         * @param aPoint Point to be examined
         * @return ETrue if the point is very near the holding point
         */
        TBool IsNearHoldingPoint( const TPoint& aPoint ) const;

        /** 
         * Set the latest point as the holding point
         */
        void SetHoldingPoint();

        /** 
         * Checks if the examined point is at the last point's position
         * 
         * @param aPoint Point to be examined
         * @return ETrue if aPoint is the same as point added last 
         */
        TBool IsLatestPoint( const TPoint& aPoint ) const;

        /** 
         * After call, Code(...) will return appropriate holding gesture code.
         */
        void StartHolding();

        /** 
         * After call, Code(...) will return a "hold released" gesture code 
         * when the gesture is completed. Meanwhile, code will be drag.
         */
        void ContinueHolding();

        /** 
         * Set as stylus released 
         */
        void SetReleased();

        /** 
         * Set the gesture as complete. Gesture is completed at pointer up 
         */
        void SetComplete();

        /** 
         * After call, Code(...) will return a "cancelled" gesture code 
         */
        void SetCancelled();

        /** 
         * Checks if the gesture is a tap
         * 
         * @return Whether the current gesture is a tap 
         *         (and hence not a swipe). Does not consider holding or 
         *         other state information (such as whether the gesture was 
         *         just started or is dragging). Results are based purely on 
         *         the current stream of points.
         */
        TBool IsTap() const;

        /**
         * From MGestureEvent
         * Checks and return current gesture status at desired axis
         * 
         * @param aRelevantAxis Axis to be examined
         * @return Gesture status
         */
        TXnGestureCode Code( TAxis aRelevantAxis ) const;

        /**
         * From MGestureEvent
         * Checks and returns holding status
         * 
         * @return Return holding
         */
        TBool IsHolding() const;

        /**
         * From MGestureEvent
         * Returns first position form iPoints
         * 
         * @return Returns first point
         */
        TPoint StartPos() const;

        /**
         * From MGestureEvent
         * Returns current (last) position from iPoints
         * 
         * @return Returns last point
         */
        TPoint CurrentPos() const;

        /**
         * From MGestureEvent
         * Returns speed of X, Y movement 
         * 
         * @return Speed in X, Y axis
         */
        TRealPoint Speed() const;

        /**
         * From MGestureEvent
         * Scales the speed
         * 
         * @return Scaled speed
         */
        TRealPoint SpeedPercent( const TRect& aEdges ) const;

        /**
         * From MGestureEvent
         * Cheks and returns distance between start and current position
         * 
         * @return Distance between two points
         */
        TPoint Distance() const;
        
        /**
         * Checks and returns gesture
         * 
         * @param aRelevantAxis See @ref MGestureEvent::Code
         * @return gesture code by analysing the sequence of points
         */
        TXnGestureCode CodeFromPoints( TAxis aRelevantAxis ) const;
        
        TXnGestureCode LastDirection( TAxis aRelevantAxis ) const;

    private:
        
        /** Information of an earlier gesture */
        struct TGestureRecord
            {
            enum TType
                {
                ETypeOther, 
                ETypeTap, 
                ETypeDoubleTap
                };

            TGestureRecord();
            TGestureRecord( TType aType, TTime aCompletionTime, TPoint aPos );

            /** type of the gesture */
            TType iType;
            /** completion time fo the gesture */
            TTime iCompletionTime;
            /** point of completion */
            TPoint iPos;
            };

        /** 
         * Returns elapsed time
         *
         * @return elapsed time between the latest and previous points 
         */
        inline TTimeIntervalMicroSeconds32 TimeFromPreviousPoint() const;

        /** 
         * Checks whether the movement has stopped
         *
         * @return ETrue if user has stopped moving the stylus before lifting 
         *         it 
         */
        inline TBool IsMovementStopped() const;

        /** 
         * Offset of the last entry
         *  
         * @return aOffset'th last entry in the list of points 
         */
        inline const TXnPointEntry& NthLastEntry( TInt aOffset ) const;

        /** 
         * Calles NthLastEntry for previous point
         * 
         * @return previous point (may not be exactly the point before last)
         */
        inline const TXnPointEntry& PreviousEntry() const;

        /** 
         * Returns the point before the last one
         * 
         * @return position of point received just before the latest point 
         */
        inline TPoint PreviousPos() const;

        /** 
         * Return if the tap was a double tap
         * 
         * @return ETrue if tap should be a double tap 
         */
        TBool IsTapDoubleTap() const;

        /** 
         * Returns current gesture type
         * 
         * @return the type of the current gesture 
         */
        TGestureRecord::TType Type() const;

    private:
        // Enumerators for holding and completion state
        
        /**
         * the different states that indicate whether user has made a holding 
         * gesture
         */
        enum THoldingState
            {
            /** before holding activated (i.e., holding not activated) */
            ENotHolding = 0,
            /** holding activated just now (lasts just one callback round) */
            EHoldStarting,
            /** holding activated earlier */
            EHolding
            };

        /**
         * Completion states
         */                 
        enum TCompletionState
            {
            // Note: implementation assumes this order of enums
            ENotComplete = 0, 
            ECancelled, 
            EComplete, 
            EReleased
            };
    
    private:
        // Members
        
        /** 
         * sequence of points that make up the gesture, owned. 
         */
        RArray< TXnPointEntry > iPoints;
        
        /** 
         * index in iPoints of the point in which user started holding 
         */
        TInt iHoldingPointIndex;     
        
        /**
         * state that indicates is user has held the pointer down in one 
         * position for long time to indicate a holding gesture 
         */
        THoldingState iHoldingState;
        
        /** 
         * gesture only becomes complete when user lifts the pointer 
         */ 
        TCompletionState iState;
        
        /** 
         * time when the stylus was lifted. 
         */
        TTime iCompletionTime;
        
        /** 
         * previous gesture information for double tap
         */
        TGestureRecord iPreviousGesture;
        };
    } // namespace XnGestureHelper

// Inline functions
#include "xngesture.inl"

#endif // _XNGESTURE_H_
