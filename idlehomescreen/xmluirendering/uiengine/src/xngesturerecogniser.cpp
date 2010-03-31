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

#include <e32math.h>

#include "xngesturerecogniser.h"
#include "xngesturedefs.h"
#include "xnpointarray.h"

using namespace XnGestureHelper;

const TInt minPointCount = 5;

/**
 * Vector class (math)
 */
NONSHARABLE_CLASS( TVector )
    {
public:
    /**
     * Constructor
     * @param aFrom starting point of the vector
     * @param aTo ending point of the vector
     */
    TVector( const TPoint& aFrom, const TPoint& aTo )
            : iX( aTo.iX - aFrom.iX ),
              iY( aTo.iY - aFrom.iY )
        {
        }

    /** @return angle of the vector */
    TReal Angle() const
        {
        TReal angle = 0;
        TReal length = Length();
        if ( length != 0 )
            {
            Math::ACos( angle, iX / Length() );
            if ( iY < 0 )
                {
                angle = 2 * KPi - angle;
                }
            }
        return Degrees( angle );
        }

    /** @return length of the vector */
    TReal Length() const
        {
        TReal length = 0;
        Math::Sqrt( length, iX * iX + iY * iY );
        return length;
        }

private:
    /** @return radians in degrees */
    inline TReal Degrees( TReal aRadians ) const
        {
        return aRadians * 180 / KPi;
        }

public:
    /// x coordinate that represent the vector
    TReal iX;
    /// y coordinate that represent the vector
    TReal iY;
    };

/**
 * @return ETrue if points for a tap event
 */
inline TBool IsTap( const TXnPointArray& aPoints )
    {
    // with tap, the pointer is not allowed to leave the tap area and come back
    // therefore, gesture length is not an acceptable test for tap, as it tests
    // only the last point. therefore, check if *any* point is outside tap area.
    TInt i = aPoints.Count(); // latest point if most likely to be outside tap area
    while ( --i >= 0 )
        {
        // use the raw point (from which no axis has been filtered out)
        // because tap should consider both axes even when Code() ignores one axis
        if ( KSamePointTolerance < Abs( aPoints.Raw( i ).iX - aPoints.Raw( 0 ).iX ) ||
             KSamePointTolerance < Abs( aPoints.Raw( i ).iY - aPoints.Raw( 0 ).iY ) )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

/**
 * @return Length of the gesture in points
 */
inline TReal GestureLength( const TXnPointArray& aPoints )
    {
    return TVector( aPoints[0], aPoints[aPoints.Count() - 1] ).Length();
    }

/**
 * @return ETrue if aAngleUnderTest is almost aAngle
 *         Closeness of the angles is controlled by KAngleTolerance
 */
static TBool IsNear( TReal aAngleUnderTest, TReal aAngle )
    {
    return aAngle - KAngleTolerance <= aAngleUnderTest &&
           aAngleUnderTest <= aAngle + KAngleTolerance;
    }

/**
 * @return the angle as a direction flags of TGesture
 */
inline TXnGestureCode Direction( TReal aAngle )
    {
    TXnGestureCode direction = EGestureUnknown;

    if ( IsNear( aAngle, 90.0 ) )
        {
        direction = EGestureSwipeDown;
        }
    else if ( IsNear( aAngle, 180.0 ) )
        {
        direction = EGestureSwipeLeft;
        }
    else if ( IsNear( aAngle, 270.0 ) )
        {
        direction = EGestureSwipeUp;
        }
    else if ( 360.0 - KAngleTolerance <= aAngle || aAngle <= KAngleTolerance )
        {
        direction = EGestureSwipeRight;
        }
    else // for lint warning
        {
        // unknown angle
        }

    return direction;
    }

/** @return direction between points */
inline TXnGestureCode Direction( const TPoint& aFromPoint, const TPoint& aToPoint )
    {
    return Direction( TVector( aFromPoint, aToPoint ).Angle() );
    }

/** @return overall direction between points */
static TXnGestureCode GeneralDirection( const TXnPointArray& aPoints )
    {
    return Direction( aPoints[0], aPoints[aPoints.Count() - 1]);
    }

/**
* @return the last received point that is different that the latest point,
*         or first point if no point is different than latest
*/
inline TPoint PreviousPoint( const TXnPointArray& aPoints )
    {
    TPoint latestPoint = aPoints[aPoints.Count() - 1];
    TInt i = aPoints.Count() - 1;
    while ( --i >= 0 )
        {
        if ( latestPoint != aPoints[i] )
            {
            return aPoints[i];
            }
        }
    return aPoints[0];
    }

/** @return direction between last two points */
TXnGestureCode TXnGestureRecogniser::LastDirection( const TXnPointArray& aPoints ) const
    {
    TXnGestureCode ret = EGestureUnknown;
    
    if ( ( aPoints.Count() > 0 ) &&
         ( GestureLength( aPoints ) >= KMinSwipeLength ))
        {
        if ( aPoints.Count() > minPointCount )
            {
            // return direction between latest and previous points.
            // pick the previous point that is different than the last point
            // because while using an x or y filter array, more than one
            // sequential points may look like the same point because
            // the differing coordinate coordinate is filtered out. For example,
            // if dragging left and slightly up, many y coordinates will have the
            // same value, while only x differs.
            ret = Direction( aPoints[ aPoints.Count() - minPointCount ],
                              aPoints[ aPoints.Count() - 1 ] );
            }
        else
            {
            ret = GeneralDirection( aPoints );
            }
        }
    
    return ret;
    }

// ----------------------------------------------------------------------------
// Return gesture code of a gesture formed by a sequence of points
// ----------------------------------------------------------------------------
//
TXnGestureCode TXnGestureRecogniser::GestureCode( const TXnPointArray& aPoints ) const
    {
    __ASSERT_DEBUG( aPoints.Count() > 0, Panic( EGesturePanicIllegalLogic ) );

    if (aPoints.Count() <= 0)
        {
        return EGestureUnknown;
        }

    if ( GestureLength( aPoints ) >= KMinSwipeLength )
        {
        return GeneralDirection( aPoints );
        }

    // the pointer was moved but was either not moved far enough, or was
    // brought back to close to the starting point
    return EGestureUnknown;
    }
