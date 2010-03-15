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

#include <e32math.h>

#include "xngesture.h"
#include "xngesturedefs.h"

using namespace XnGestureHelper;

// ======== LOCAL FUNCTIONS ===================================================

/**
 * Point array for which only x axis is relevant
 */
class TXAxisPointArray : public TXnPointArray
    {
public:
    TXAxisPointArray( const RArray< TXnPointEntry >& aPoints )
        : TXnPointArray( aPoints )
        {
        }

    // from TXnPointArray
    TPoint operator[]( TInt aIndex ) const
        {
        return TPoint( Raw( aIndex ).iX, 0 );
        }
    };

/**
 * Point array for which only y axis is relevant
 */
class TYAxisPointArray : public TXnPointArray
    {
public:
    TYAxisPointArray( const RArray< TXnPointEntry >& aPoints )
        : TXnPointArray( aPoints )
        {
        }

    // from TXnPointArray
    TPoint operator[]( TInt aIndex ) const
        {
        return TPoint( 0, Raw( aIndex ).iY );
        }
    };

/** @return the current time */
TTime CurrentTime()
    {
    TTime time;
    time.UniversalTime();
    return time;
    }

// ----------------------------------------------------------------------------
// destructor
// ----------------------------------------------------------------------------
//
CXnGesture::~CXnGesture()
    {
    iPoints.Close();
    }

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
//
void CXnGesture::Reset()
    {
    // store previous gesture data before resetting the state
    if ( iPoints.Count() > 0 )
        {
        iPreviousGesture = TGestureRecord( Type(), iCompletionTime,
            iPoints[iPoints.Count() - 1].iPos );
        }
    else
        {
        iPreviousGesture = TGestureRecord();
        }

    iPoints.Reset();
    iHoldingState = ENotHolding;
    iState = ENotComplete;
    iHoldingPointIndex = 0;
    }

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
//
TBool CXnGesture::IsEmpty() const
    {
    return iPoints.Count() == 0;
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// ----------------------------------------------------------------------------
//
TInt CXnGesture::AddPoint( const TPoint& aPoint )
    {
    if ( !IsLatestPoint( aPoint ) )
        {
        return iPoints.Append( TXnPointEntry( aPoint, CurrentTime() ) );
        }
    return KErrNone;
    }

/**
 * @return ETrue if the point is within a specified distance of the other point
 */
inline TBool IsNear( const TPoint& aPointUnderTest, const TPoint& aPoint,
        TInt aMargin )
    {
    TRect rect(
        aPoint.iX - aMargin, aPoint.iY - aMargin,
        aPoint.iX + aMargin, aPoint.iY + aMargin );
    return rect.Contains( aPointUnderTest );
    }

// ----------------------------------------------------------------------------
// IsNearHoldingPoint
// ----------------------------------------------------------------------------
//
TBool CXnGesture::IsNearHoldingPoint( const TPoint& aPoint ) const
    {
    return IsNear( aPoint, iPoints[iHoldingPointIndex].iPos,
        KSamePointTolerance );
    }

// ----------------------------------------------------------------------------
// IsLatestPoint
// ----------------------------------------------------------------------------
//
TBool CXnGesture::IsLatestPoint( const TPoint& aPoint ) const
    {
    if ( iPoints.Count() > 0 )
        {
        return aPoint == CurrentPos();
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// StartHolding
// ----------------------------------------------------------------------------
//
void CXnGesture::StartHolding()
    {
    iHoldingState = EHoldStarting;

    // remove all points that were introduced after holding started
    for ( TInt i = iPoints.Count() - 1; i > iHoldingPointIndex; i-- )
        {
        iPoints.Remove( i );
        }
    }

// ----------------------------------------------------------------------------
// SetHoldingPoint
// ----------------------------------------------------------------------------
//
void CXnGesture::SetHoldingPoint()
    {
    iHoldingPointIndex = iPoints.Count() - 1;
    }

// ----------------------------------------------------------------------------
// ContinueHolding
// ----------------------------------------------------------------------------
//
void CXnGesture::ContinueHolding()
    {
    iHoldingState = EHolding;
    }

// ----------------------------------------------------------------------------
// SetReleased
// ----------------------------------------------------------------------------
//
void CXnGesture::SetReleased()
    {
    // IsMovementStopped expects SetComplete to be called before SetRelea
    __ASSERT_DEBUG( EComplete == iState, Panic( EGesturePanicIllegalLogic ) );
    iState = EReleased;
    }

/**
 * @return elapsed time between aStartTime and aEndTime
 */
inline TTimeIntervalMicroSeconds32 Elapsed(
    const TTime& aStartTime,
    const TTime& aEndTime )
    {
    return aEndTime.MicroSecondsFrom( aStartTime ).Int64();
    }

// ----------------------------------------------------------------------------
// SetComplete
// ----------------------------------------------------------------------------
//
void CXnGesture::SetComplete()
    {
    __ASSERT_DEBUG( iPoints.Count() > 0, Panic( EGesturePanicIllegalLogic ) );
    iState = EComplete;
    iCompletionTime = CurrentTime();
    }

// ----------------------------------------------------------------------------
// SetComplete
// ----------------------------------------------------------------------------
//
void CXnGesture::SetCancelled()
    {
    iState = ECancelled;
    }

// ----------------------------------------------------------------------------
// IsTap
// ----------------------------------------------------------------------------
//
TBool CXnGesture::IsTap() const
    {
    return CodeFromPoints( EAxisBoth ) == EGestureTap;
    }

/**
 * Translates a non-holding code into a holding code
 * @param aCode original gesture code
 * @return a gesture code with hold flag applied
 */
inline TXnGestureCode Hold( TXnGestureCode aCode )
    {
    if ( aCode != EGestureStart &&
         aCode != EGestureDrag &&
         aCode != EGestureReleased &&
         aCode != EGestureUnknown )
        {
        return static_cast< TXnGestureCode >( aCode | EFlagHold );
        }
    return aCode;
    }

// ----------------------------------------------------------------------------
// Code
// ----------------------------------------------------------------------------
//
TXnGestureCode CXnGesture::Code( TAxis aRelevantAxis ) const
    {
    switch ( iState )
        {
        case ENotComplete:
            // "start" event if only first point received
            // need to check that not holding, in case user pressed stylus
            // down, and activated holding without moving the stylus
            if ( iPoints.Count() == 1 && !IsHolding() )
                {
                return EGestureStart;
                }
            // "drag" event if holding not started or holding started earlier
            else if ( iHoldingState != EHoldStarting )
                {
                return EGestureDrag;
                }
            // holding was just started
            else
                {
                return Hold( CodeFromPoints( aRelevantAxis ) );
                }

        case EComplete:
            {
            TXnGestureCode code = CodeFromPoints( aRelevantAxis );

#ifdef _GESTURE_DOUBLE_TAP_SUPPORT
            if ( EGestureTap == code && IsTapDoubleTap() )
                {
                code = EGestureDoubleTap;
                }
#endif // _GESTURE_DOUBLE_TAP_SUPPORT

            return code;
            }

        case EReleased:
            return EGestureReleased;

        case ECancelled: // fallthrough
        default:
            return EGestureUnknown;
        }
    }

// ----------------------------------------------------------------------------
// IsHolding
// ----------------------------------------------------------------------------
//
TBool CXnGesture::IsHolding() const
    {
    return iHoldingState >= EHoldStarting;
    }

// ----------------------------------------------------------------------------
// StartPos
// ----------------------------------------------------------------------------
//
TPoint CXnGesture::StartPos() const
    {
    // at least one point will be in the array during callback (pointer down pos)
    return iPoints[0].iPos;
    }

// ----------------------------------------------------------------------------
// CurrentPos
// ----------------------------------------------------------------------------
//
TPoint CXnGesture::CurrentPos() const
    {
    // at least on point will be in the array during callback (pointer down pos)
    return iPoints[iPoints.Count() - 1].iPos;
    }

// ----------------------------------------------------------------------------
// IsMovementStopped
// ----------------------------------------------------------------------------
//
inline TBool CXnGesture::IsMovementStopped() const
    {
    // iCompletionTime is only only valid if client has called SetComplete
    if ( iState >= EComplete )
        {
        return Elapsed( NthLastEntry( 1 ).iTime, iCompletionTime )
            .Int() > KSpeedStopTime;
        }
    return EFalse;
    }

namespace
    {
    const TInt KFloatingPointAccuracy = 0.000001;

    /** @return percentage (0.0-1.0) how far aPos is from aEdge1 towards aEdge2 */
    inline TReal32 Proportion( TReal32 aPos, TReal32 aEdge1, TReal32 aEdge2 )
        {
        if ( Abs( aEdge2 - aEdge1 ) > KFloatingPointAccuracy )
            {
            return ( aPos - aEdge1 ) / ( aEdge2 - aEdge1 );
            }
        return 0; // avoid division by zero
        }

    /** Edges (pixels) at which speed should be -100% or 100% */
    NONSHARABLE_STRUCT( TEdges )
        {
        TReal32 iMin;
        TReal32 iMax;
        };

    /**
     * scale which allows different (coordinate -> percentage) mapping
     * between -100% to 0% and 0 and 100%
     */
    NONSHARABLE_STRUCT( TScale )
        {
        TScale( TInt aZero, const TEdges& aEdges )
                : iMin( aEdges.iMin ), iZero( aZero ), iMax( aEdges.iMax )
            {
            }

        /** @return aPos as a percentage between -100% and 100% in aScale */
        TReal32 Percent( TReal32 aPos ) const;

        /// coordinate where speed is -100%
        TReal32 iMin;
        /// coordinate where speed is 0%
        TReal32 iZero;
        /// coordinate where speed is 100%
        TReal32 iMax;
        };

    /** @convert aPos into a percentage between -100% and 100% in aScale */
    TReal32 TScale::Percent( TReal32 aPos ) const
        {
        TReal32 percent;
        if ( aPos < iZero )
            {
            // return negative percentages on the lower side of zero point
            percent = -1 * Proportion( aPos, iZero, iMin );
            }
        else
            {
            percent = Proportion( aPos, iZero, iMax );
            }
        // constrain between -100% and 100%
        return Min( Max( percent, -1.0F ), 1.0F );
        }

    /** Scale in x and y dimensions */
    NONSHARABLE_STRUCT( TScale2D )
        {
        TRealPoint Percent( const TPoint& aPos ) const
            {
            return TRealPoint(
                iX.Percent( aPos.iX ),
                iY.Percent( aPos.iY ) );
            }

        TScale iX;
        TScale iY;
        };

    enum TDirection { ESmaller, ELarger };

    /** @return the direction of pos compared to the previous pos */
    inline TDirection Direction( TInt aPos, TInt aPreviousPos )
        {
        return aPos < aPreviousPos ? ESmaller : ELarger;
        }

    /** Direction in x and y dimensions */
    NONSHARABLE_STRUCT( TDirection2D )
        {
        TDirection iX;
        TDirection iY;
        };

    /** Return the direction (up/down) of signal at aIndex */
    inline TDirection2D Direction(
        TInt aIndex,
        const RArray< TXnPointEntry >& aPoints )
        {
        const TPoint& pos = aPoints[aIndex].iPos;
        const TPoint& prevPos = aPoints[aIndex - 1].iPos;
        TDirection2D dir = {
            Direction( pos.iX, prevPos.iX ),
            Direction( pos.iY, prevPos.iY ) };
        return dir;
        }

    /**
     * @return a position in the aLow and aHigh, so that it aProportion of
     *         of length is above the pos
     */
    TReal32 ProportionalLength( TReal32 aLow, TReal32 aHigh, TReal32 aProportion )
        {
        return ( aHigh - aLow ) * aProportion / ( 1 + aProportion );
        }

    /**
     * @return aVariableEdge scaled to new position, when the other edge changes
     *         from aOldEdge to aNewEdge, so that aOrigin maintains the *same
     *         relative position* between aVariableEdge and the other edge
     */
    inline TReal32 ScaledEdge(
        TReal32 aOrigin,
        TReal32 aVariableEdge,
        TReal32 aOldEdge,
        TReal aNewEdge )
        {
        TReal32 proportion = Proportion( aOrigin, aVariableEdge, aOldEdge );
        return ( proportion * aNewEdge - aOrigin ) / ( proportion - 1 );
        }

    TScale Rescale(
        TReal32 aPos,
        TDirection aDir,
        TDirection aPrevDir,
        const TScale& aPrevScale,
        const TEdges& aEdges )
        {
        TScale scale( aPrevScale );
        if ( aPrevDir != aDir )
            {
            // the code duplication is accepted here, since it is difficult
            // to factor out while maintaining the understandability of this
            // anyway complex algorithm
            if ( aDir == ESmaller )
                {
                scale.iMin = aEdges.iMin;
                if ( aPrevScale.iZero < aPos )
                    {
                    TReal32 proportionAboveZero = Proportion(
                        aPos, aPrevScale.iZero, aPrevScale.iMax );
                    scale.iZero = aPos - ProportionalLength(
                        aEdges.iMin, aPos, proportionAboveZero );
                    }
                else
                    {
                    // adjust zero pos so that proportion between aPos, Min,
                    // and Zero pos stay the same (Min will move to 0,
                    // aPos stays the same)
                    scale.iZero = ScaledEdge( aPos, aPrevScale.iZero,
                        aPrevScale.iMin, aEdges.iMin );
                    }

                // adjust the upper edge to take into account the movement of
                // zero pos
                scale.iMax = ScaledEdge( aPos, aPrevScale.iMax,
                    aPrevScale.iZero, scale.iZero );
                }
            else // ELarger
                {
                scale.iMax = aEdges.iMax;
                if ( aPos < aPrevScale.iZero )
                    {
                    TReal32 proportionBelowZero = Proportion(
                        aPos, aPrevScale.iZero, aPrevScale.iMin );
                    scale.iZero = aPos + ProportionalLength(
                        aPos, aEdges.iMax, proportionBelowZero );
                    }
                else
                    {
                    // adjust zero pos so that proportion between aPos, Max, and
                    // Zero pos stay the same (Max will move edge, aPos stays
                    // the same)
                    scale.iZero = ScaledEdge( aPos, aPrevScale.iZero,
                        aPrevScale.iMax, aEdges.iMax );
                    }

                // adjust the lower edge to take into account the movement of
                // zero pos
                scale.iMin = ScaledEdge( aPos, aPrevScale.iMin,
                    aPrevScale.iZero, scale.iZero );
                }
            }
        return scale;
        }

    /** Edges in x and y dimensions */
    NONSHARABLE_STRUCT( TEdges2D )
        {
        TEdges iX;
        TEdges iY;
        };

    /**
     * @param aEdges edges of the area in which gesture points are accepted
     * @return the scale of latest point in the list of points
     */
    TScale2D Scale( const RArray< TXnPointEntry >& aPoints, const TEdges2D& aEdges )
        {
        TScale2D scale = { TScale( aPoints[0].iPos.iX, aEdges.iX ),
                           TScale( aPoints[0].iPos.iY, aEdges.iY ) };
        TInt count = aPoints.Count();
        if ( count > 1 )
            {
            // iterate the whole point list to arrive to the current scale
            TDirection2D dir( Direction( 1, aPoints ) );
            for ( TInt i = 1; i < count; i++ )
                {
                // get direction at i
                TDirection2D newDir( Direction( i, aPoints ) );
                // get new scale at i
                scale.iX = Rescale(
                    aPoints[i - 1].iPos.iX,
                    newDir.iX,
                    dir.iX,
                    scale.iX,
                    aEdges.iX );
                scale.iY = Rescale(
                    aPoints[i - 1].iPos.iY,
                    newDir.iY,
                    dir.iY,
                    scale.iY,
                    aEdges.iY );
                dir = newDir;
                }
            }
        return scale;
        }
    } // unnamed namespace

TRealPoint CXnGesture::SpeedPercent( const TRect& aEdges ) const
    {
    // x and y coordinates are easier to handle separately, extract from TRect:
    // ((iMinX, iMinY), (iMaxX, iMaxY)) -> ((iMinX, iMaxX), (iMinY, iMaxY))
    TEdges2D edges = {
        { aEdges.iTl.iX, aEdges.iBr.iX },
        { aEdges.iTl.iY, aEdges.iBr.iY } };
    // work out the current scale (coordinate -> percentage mapping) from
    // the history of points (i.e., points of current gesture). Then
    // calculate the percentage of the current position.
    return Scale( iPoints, edges ).Percent( CurrentPos() );
    }

// ----------------------------------------------------------------------------
// Speed
// ----------------------------------------------------------------------------
//
TRealPoint CXnGesture::Speed() const
    {
    const TReal32 KMicroSecondsInSecond = 1000000;

    // Speed is only evaluated at the end of the swipe
    // if user stops at the end of the swipe before lifting stylus,
    // speed is zero. If time is zero, return 0 speed (infinite does
    // not make sense either). Will need to consider also earlier points
    // and their times or start time, if this zero-speed behavior is a problem
    TRealPoint speed;
    TReal32 time = static_cast< TReal32 >( TimeFromPreviousPoint().Int() )
        / KMicroSecondsInSecond;
    if ( !IsMovementStopped() && time > 0 )
        {
        TPoint distance = CurrentPos() - PreviousPos();
        speed.iX = static_cast< TReal32 >( distance.iX ) / time;
        speed.iY = static_cast< TReal32 >( distance.iY ) / time;
        }
    return speed;
    }

// ----------------------------------------------------------------------------
// Distance
// ----------------------------------------------------------------------------
//
TPoint CXnGesture::Distance() const
    {
    return CurrentPos() - StartPos();
    }

// ----------------------------------------------------------------------------
// TimeFromPreviousPoint
// ----------------------------------------------------------------------------
//
inline TTimeIntervalMicroSeconds32 CXnGesture::TimeFromPreviousPoint() const
    {
    const TInt KLatestEntryOffset = 1;
    return Elapsed( PreviousEntry().iTime, NthLastEntry( KLatestEntryOffset ).iTime );
    }

// ----------------------------------------------------------------------------
// CodeFromPoints
// ----------------------------------------------------------------------------
//
TXnGestureCode CXnGesture::CodeFromPoints( TAxis aRelevantAxis ) const
    {
    // select the correct filter based on aRelevantAxis
    // these filter_ objects are array decorators that will eliminate either
    // x, y or neither coordinate of each point
    TXAxisPointArray filterY( iPoints );
    TYAxisPointArray filterX( iPoints );
    TXnPointArray filterNone( iPoints );
    TXnPointArray& filter =
        aRelevantAxis == EAxisHorizontal ? static_cast< TXnPointArray& >( filterY ) :
        aRelevantAxis == EAxisVertical   ? static_cast< TXnPointArray& >( filterX ) :
        /* otherwise EAxisBoth */          filterNone;

    // currently the gesture recogniser does not have any state, so it is fast
    // to instantiate. The call is not static however, to allow the recogniser
    // to be replaced by a more complicated implementation that has state.
    // then it may make sense to make the recogniser a member variable.
    return TXnGestureRecogniser().GestureCode( filter );
    }

// ----------------------------------------------------------------------------
// CodeFromPoints
// ----------------------------------------------------------------------------
//
TXnGestureCode CXnGesture::LastDirection( TAxis aRelevantAxis ) const
    {
    // select the correct filter based on aRelevantAxis
    // these filter_ objects are array decorators that will eliminate either
    // x, y or neither coordinate of each point
    TXAxisPointArray filterY( iPoints );
    TYAxisPointArray filterX( iPoints );
    TXnPointArray filterNone( iPoints );
    TXnPointArray& filter =
        aRelevantAxis == EAxisHorizontal ? static_cast< TXnPointArray& >( filterY ) :
        aRelevantAxis == EAxisVertical   ? static_cast< TXnPointArray& >( filterX ) :
        /* otherwise EAxisBoth */          filterNone;

    // currently the gesture recogniser does not have any state, so it is fast
    // to instantiate. The call is not static however, to allow the recogniser
    // to be replaced by a more complicated implementation that has state.
    // then it may make sense to make the recogniser a member variable.
    return TXnGestureRecogniser().LastDirection( filter );
    }

// ----------------------------------------------------------------------------
// return nth point from the end of the points array
// ----------------------------------------------------------------------------
//
inline const TXnPointEntry& CXnGesture::NthLastEntry( TInt aOffset ) const
    {
    return iPoints[Max( iPoints.Count() - aOffset, 0 )];
    }

// ----------------------------------------------------------------------------
// PreviousEntry
// ----------------------------------------------------------------------------
//
inline const TXnPointEntry& CXnGesture::PreviousEntry() const
    {
    return NthLastEntry( KPreviousPointOffset );
    }

// ----------------------------------------------------------------------------
// PreviousPos
// ----------------------------------------------------------------------------
//
inline TPoint CXnGesture::PreviousPos() const
    {
    return NthLastEntry( KPreviousPointOffset - 1 ).iPos;
    }

// ----------------------------------------------------------------------------
// SetComplete
// ----------------------------------------------------------------------------
//
TBool CXnGesture::IsTapDoubleTap() const
    {
    return iPreviousGesture.iType == TGestureRecord::ETypeTap &&
           Elapsed( iPreviousGesture.iCompletionTime, iCompletionTime ).Int() <=
               KMaxDoubleTapDuration &&
           IsNear( iPreviousGesture.iPos, iPoints[iPoints.Count() - 1].iPos,
               KSamePointTolerance );
    }

// ----------------------------------------------------------------------------
// Type
// ----------------------------------------------------------------------------
//
CXnGesture::TGestureRecord::TType CXnGesture::Type() const
    {
    if ( CodeFromPoints( EAxisBoth ) == EGestureTap && !IsHolding() )
        {
        if ( IsTapDoubleTap() )
            {
            return CXnGesture::TGestureRecord::ETypeDoubleTap;
            }
        else
            {
            return CXnGesture::TGestureRecord::ETypeTap;
            }
        }
    else
        {
        return CXnGesture::TGestureRecord::ETypeOther;
        }
    }

// ----------------------------------------------------------------------------
// TGestureRecord constructor
// ----------------------------------------------------------------------------
//
CXnGesture::TGestureRecord::TGestureRecord()
    {
    iType = ETypeOther;
    }

// ----------------------------------------------------------------------------
// TGestureRecord constructor
// ----------------------------------------------------------------------------
//
CXnGesture::TGestureRecord::TGestureRecord(
    CXnGesture::TGestureRecord::TType aType,
    TTime aCompletionTime,
    TPoint aPos )
        : iType( aType ), iCompletionTime( aCompletionTime ), iPos( aPos )
    {
    }
