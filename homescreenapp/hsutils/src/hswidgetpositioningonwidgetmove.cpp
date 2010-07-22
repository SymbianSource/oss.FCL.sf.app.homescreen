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
* Description:
*
*/

#include <QVector2D>
#include "hswidgetpositioningonwidgetmove.h"
#include "hsapp_defs.h"


/*!
    The distance by which the line is extended on each side of reference points
*/
#define SNAP_LINE_EXTEND_VALUE 10.0

/*!
    Sets the snap method instance. The existing instance
    will be deleted.
*/
void HsWidgetPositioningOnWidgetMove::setInstance(HsWidgetPositioningOnWidgetMove *instance)
{
    if (mInstance)
        delete mInstance;
    mInstance = instance;
}

/*!
    Returns the snap method instance.
*/
HsWidgetPositioningOnWidgetMove *HsWidgetPositioningOnWidgetMove::instance()
{
    return mInstance;
}

/*!
    Points to the snap method instance.
*/
HsWidgetPositioningOnWidgetMove *HsWidgetPositioningOnWidgetMove::mInstance = 0;

/*!
    Constructor.
*/
HsSnapToLines::HsSnapToLines() :
    mActiveRectWidth(0.0), mActiveRectHeight(0.0),
    mHorizontalSnapPosition(0.0), mVerticalSnapPosition(0.0),
    mHorizontalSnapFound(false),
    mVerticalSnapFound(false),
    mRectLieAbove(false), mLeftInRange(false), mRightInRange(false),
    mMinVerticalEdgesDistance(0.0), mVerticalDistance(0.0),
    mVerticalDistanceFromSelectedRect(0.0), mContainerVerticalEdgeDistance(0.0),
    mRectLieLeft(false), mTopInRange(false), mBottomInRange(false),
    mMinHorizontalEdgesDistance(0.0), mHorizontalDistance(0.0),
    mHorizontalDistanceFromSelectedRect(0.0), mContainerHorizontalEdgeDistance(0.0),
    mSnapEnabled(false), mSnapForce(0.0), mSnapGap(0.0), mSnapBorderGap(0.0),
    mRectVerticalEdgeLiesInLineWithVerticalLine(false), mRectLieAboveVerticalLine(false), mRectLieBelowVerticalLine(false),
    mRectHorizontalEdgeLiesInLineWithHorizontalLine(false), mRectLieLeftOfHorizontalLine(false), mRectLiesRightOfHorizontalLine(false)
{

}

/*!
    Configures the snap-to-lines algorithm.
*/
void HsSnapToLines::setConfiguration(const QVariantHash &configuration)
{
    bool canConvert = false;
    mSnapEnabled = configuration[SNAPENABLED].toBool();
    //The following values should be in qreal, so the status received in canConvert is ignored
    mSnapForce = configuration[SNAPFORCE].toDouble(&canConvert);
    mSnapGap = configuration[SNAPGAP].toDouble(&canConvert);
    mSnapBorderGap = configuration[SNAPBORDERGAP].toDouble(&canConvert);
}

/*!
    Set the dimensions of container rect, active rect and position of Inactive Rects
    on the page.
*/
void HsSnapToLines::setPagePresentation(const QRectF &containerRect, 
                                        const QList<QRectF> &inactiveRects,
                                        const QRectF &activeRect)
{
    mContainerRect.setLeft(containerRect.left() + mSnapBorderGap);
    mContainerRect.setTop(containerRect.top() + mSnapBorderGap);
    mContainerRect.setRight(containerRect.right() - mSnapBorderGap);
    mContainerRect.setBottom(containerRect.bottom() - mSnapBorderGap);

    mActiveRectWidth = activeRect.width();
    mActiveRectHeight = activeRect.height();

    createSnappableRectangles(inactiveRects);
}

/*!
    Create the list of rects and flag if their sides are snappable from top or bottom or left or right,
    depending on other rects overlapping with the rect.
*/
void HsSnapToLines::createSnappableRectangles(const QList<QRectF> &inactiveRects)
{
    mInactiveSnapRects.clear();

    int i;
    for (i = 0; i<inactiveRects.count(); ++i) {
        QRectF rect = inactiveRects[i];
        HsSnapRectangle snapRectangle(rect);

        int j;
        for (j = 0; j<inactiveRects.count(); ++j) {
            QRectF rectToCompare = inactiveRects[j];
            if (rect != rectToCompare) {
                //Check if the rectangles being compared intersect each other
                if (rectToCompare.intersects(rect)) {
                    //As the widgets intersect, check which corner is contained,
                    //The corner that is contained is not snappable, when the moving widget is in the same position
                    if (rectToCompare.contains(rect.topLeft())) {
                        snapRectangle.isLeftSnapableForAbove = false;
                        snapRectangle.isTopSnapableForLeft = false;
                    }

                    if (rectToCompare.contains(rect.topRight())) {
                        snapRectangle.isRightSnapableForAbove = false;
                        snapRectangle.isTopSnapableForRight = false;
                    }

                    if (rectToCompare.contains(rect.bottomRight())) {
                        snapRectangle.isRightSnapableForBelow = false;
                        snapRectangle.isBottomSnapableForRight = false;
                    }

                    if (rectToCompare.contains(rect.bottomLeft())) {
                        snapRectangle.isLeftSnapableForBelow = false;
                        snapRectangle.isBottomSnapableForLeft = false;
                    }
                }
            }
        }
        if (snapRectangle.isLeftSnapableForAbove || snapRectangle.isLeftSnapableForBelow ||
            snapRectangle.isRightSnapableForAbove || snapRectangle.isRightSnapableForBelow ||
            snapRectangle.isTopSnapableForLeft || snapRectangle.isTopSnapableForRight ||
            snapRectangle.isBottomSnapableForLeft || snapRectangle.isBottomSnapableForRight) {
                mInactiveSnapRects.append(snapRectangle);
        }
    }
}

/*!
    Run the snap algorithm to with the position of moving rect, to get a snap result.
*/
HsWidgetPositioningOnWidgetMove::Result HsSnapToLines::run(const QRectF &movingRect)
{
    HsWidgetPositioningOnWidgetMove::Result result;

    if (mSnapEnabled) {
        mMovingRect = movingRect;
        mHorizontalSnapPosition = 0.0;
        mVerticalSnapPosition = 0.0;

        mVerticalLine = QLineF();
        mMinVerticalEdgesDistance = mSnapForce;
        mVerticalDistanceFromSelectedRect = 0.0;
        mContainerVerticalEdgeDistance = 0.0;
        mHorizontalSnapFound = false;

        mHorizontalLine = QLineF();
        mMinHorizontalEdgesDistance = mSnapForce;
        mHorizontalDistanceFromSelectedRect = 0.0;
        mContainerHorizontalEdgeDistance = 0.0;
        mVerticalSnapFound = false;

        checkForCenterSnapping();
        for (int i = 0; i < mInactiveSnapRects.count(); ++i) {
            mInactiveSnapRectToCompare = mInactiveSnapRects[i];
            mInactiveRectToCompare = mInactiveSnapRectToCompare.rectangle;
            if (!movingRect.intersects(mInactiveRectToCompare)) { //Only compare if Inactive Rect and moving rect do not overlap.
                //TODO: Move the above check to another function...
                // X - Direction Snapping
                compareLeftSideOfMovingRectForSnapping();
                compareRightSideOfMovingRectForSnapping();
                // Y - Direction Snapping
                compareTopOfMovingRectForSnapping();
                compareBottomOfMovingRectForSnapping();
            }
        }

        if (mHorizontalSnapFound) {
            result.hasHorizontalSnap = true;
            result.horizontalSnapPosition = mHorizontalSnapPosition;
            extendVerticalLine();
            result.verticalSnapLine = mVerticalLine;
        }

        if (mVerticalSnapFound) {
            result.hasVerticalSnap = true;
            result.verticalSnapPosition = mVerticalSnapPosition;
            extendHorizontalLine();
            result.horizontalSnapLine = mHorizontalLine;
        }
    }

    return result;
}

/*!
    Check if the center of moving rect is in the snap force in the middle of continer rect.
*/
void HsSnapToLines::checkForCenterSnapping()
{
    QPointF centerOfContainerRect = mContainerRect.center();

    QRectF verticalSnapField = QRectF(QPointF(centerOfContainerRect.x() - (mSnapForce/2), mContainerRect.top()),
         QPointF(centerOfContainerRect.x() + (mSnapForce/2), mContainerRect.bottom()));

    //Check that the widget lies in the container rect, if the snapping position is proposed... is not required,
    //unless some widget is bigger than the page width
    QPointF centerOfMovingRect = mMovingRect.center();
    if (verticalSnapField.contains(centerOfMovingRect)) {
        mHorizontalSnapFound = true;
        mHorizontalSnapPosition = centerOfContainerRect.x() - mActiveRectWidth/2;
        mMinVerticalEdgesDistance = qAbs(centerOfContainerRect.x() - centerOfMovingRect.x());
        //save the points for the Vertical line
        mVerticalLine.setP1(QPointF(centerOfContainerRect.x(), mMovingRect.top()));
        mVerticalLine.setP2(QPointF(centerOfContainerRect.x(), mMovingRect.bottom()));
    }
}

/*!
    Check if the inactive rect being compared with moving rect lies above or below of moving rect.
*/
void HsSnapToLines::checkInactiveRectLieAboveOrBelowOfMovingRect()
{
    //Check if the inactive rect lies below or above of the moving rect
    mRectLieAbove = false;
    mVerticalDistance = 0.0;
    //Inactive Rect lies above or at the save Y position as the Moving Rect
    if (mInactiveRectToCompare.top() <= mMovingRect.top()) {
        mRectLieAbove = true;
        if (mInactiveRectToCompare.bottom() <= mMovingRect.top()) {
            mVerticalDistance = mMovingRect.top() - mInactiveRectToCompare.bottom();
        }
        else {
            mVerticalDistance = mMovingRect.top() - mInactiveRectToCompare.top();
        }
    }
    //Inactive Rect lies below the Moving Rect
    else {
        mRectLieAbove = false;
        if (mMovingRect.bottom() <= mInactiveRectToCompare.top()) {
            mVerticalDistance = mInactiveRectToCompare.top() - mMovingRect.bottom();
        }
        else {
            mVerticalDistance = mInactiveRectToCompare.bottom() - mMovingRect.bottom();
        }
    }
}


/*!
    Check if the left edge of moving rect is snappable to the incative rect's left or right edge.
    The inactive rect's edge is only chosen if it is a better fit for horizontal snapping.
*/
void HsSnapToLines::compareLeftSideOfMovingRectForSnapping()
{
    checkInactiveRectLieAboveOrBelowOfMovingRect();

    //calculate the distance of the moving rect's left edge to the inactive rect's left and right edges
    qreal leftToLeftOfInactiveRect = qAbs(mInactiveRectToCompare.left() - mMovingRect.left());
    qreal leftToRightOfInactiveRect = qAbs(mInactiveRectToCompare.right() - mMovingRect.left());
    mLeftInRange = false;
    mRightInRange = false;

    if (leftToLeftOfInactiveRect <= mMinVerticalEdgesDistance) {
        if (mRectLieAbove && mInactiveSnapRectToCompare.isLeftSnapableForBelow
         || !mRectLieAbove && mInactiveSnapRectToCompare.isLeftSnapableForAbove) { 
            mLeftInRange = true;
        }
    }
    if (leftToRightOfInactiveRect <= mMinVerticalEdgesDistance) {
        if (mRectLieAbove && mInactiveSnapRectToCompare.isRightSnapableForBelow
         || !mRectLieAbove && mInactiveSnapRectToCompare.isRightSnapableForAbove) {
            mRightInRange = true;
        }
    }

    //calculate the distance of inactive rect's left edge and container rect's left edge
    qreal differenceContainerLeftEdgeToInactiveRectLeftEdge = mInactiveRectToCompare.left() - mContainerRect.left();
    //calculate the distance of inactive rect's right edge and container rect's right edge
    qreal differenceContainerRightEdgeToInactiveRectRightEdge = mContainerRect.right() - mInactiveRectToCompare.right();

    qreal minDistancePosition = 0.0;
    qreal distanceVerticalEdges = 0.0;
    qreal xSnapGapAdjustment = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mLeftInRange && !mRightInRange)
        || !mLeftInRange && mRightInRange) {
        if (mLeftInRange) {
            minDistancePosition = mInactiveRectToCompare.left();
            distanceVerticalEdges = leftToLeftOfInactiveRect;
            xSnapGapAdjustment = 0.0;
        }
        else {
            minDistancePosition = mInactiveRectToCompare.right();
            distanceVerticalEdges = leftToRightOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mLeftInRange && mRightInRange) {
        //if left edge of moving rect to the left of the inactive rect is closer than the left edge of moving rect to the right of the inactive rect
        if (leftToLeftOfInactiveRect < leftToRightOfInactiveRect) {
            minDistancePosition = mInactiveRectToCompare.left();
            distanceVerticalEdges = leftToLeftOfInactiveRect;
            xSnapGapAdjustment = 0.0;
            mRightInRange = false;
        }
        //if the left edge of inactive rect to left of moving rect is at the same distance as the right edge of inactive rect to the right of moving rect
        else if (leftToLeftOfInactiveRect == leftToRightOfInactiveRect) {
            //if inactive rect lies towards the left or middle of container rect, then the left edge is priortized as the selected edge for outside snapping 
            if (differenceContainerLeftEdgeToInactiveRectLeftEdge <= differenceContainerRightEdgeToInactiveRectRightEdge) { 
                minDistancePosition = mInactiveRectToCompare.left();
                distanceVerticalEdges = leftToLeftOfInactiveRect;
                xSnapGapAdjustment = 0.0;
                mRightInRange = false;
            }
            //else right of the inactive rect lies more close to the right of the container rect, and hence prioritize it for snapping.
            else {
                minDistancePosition = mInactiveRectToCompare.right();
                distanceVerticalEdges = leftToRightOfInactiveRect;
                xSnapGapAdjustment = mSnapGap;
                mLeftInRange = false;
            }
        }
        //else right edge of inactive rect to the left of the moving rect is closer than the left edge of inactive rect to the left of the moving rect
        else{
            minDistancePosition = mInactiveRectToCompare.right();
            distanceVerticalEdges = leftToRightOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
            mLeftInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect for X - snapping
    bool horizontalSnappingBetterFit = false;
    if (mLeftInRange || mRightInRange) {
        if (distanceVerticalEdges < mMinVerticalEdgesDistance) {
            horizontalSnappingBetterFit = true;
        }
        else if (distanceVerticalEdges == mMinVerticalEdgesDistance) { //the distance in the vertical edges is same as from the selected rectangle
            //check the position of rect with respect to Vertical line
            checkInactiveRectPositionToVerticalLine();
            //if horizontal snap position was previously found the rect's edges are in line with Vertical line
            if (mHorizontalSnapFound && mRectVerticalEdgeLiesInLineWithVerticalLine) {
                if (mRectLieAboveVerticalLine || mRectLieBelowVerticalLine) {
                    extendVerticalLineToIncludeInactiveRect();
                }
            }
            //here the case is that moving rect lies exactly in middle of two same sides of two inactive widgets.
            else {
                //Prioritize first on the fact if the inactive rect is closer to the moving rect in Y - direction.
                if (mVerticalDistance < mVerticalDistanceFromSelectedRect) {
                    horizontalSnappingBetterFit = true;
                }
                else if (mVerticalDistance == mVerticalDistanceFromSelectedRect) {
                    //Prioritize next if this Inactive rect is closer to the left edge of the container rect, then the previously selected rect
                    if (differenceContainerLeftEdgeToInactiveRectLeftEdge < mContainerVerticalEdgeDistance) {
                        horizontalSnappingBetterFit = true;
                    }
                    //Prioritize next if the Inactive widget's left edge lies near to left edge of the container rect
                    else if (differenceContainerLeftEdgeToInactiveRectLeftEdge < differenceContainerRightEdgeToInactiveRectRightEdge) {
                        horizontalSnappingBetterFit = true;
                    }
                    else {
                        //This else will happen if this rectangle being compared is exactly the same as the selected rectangle for snapping.
                        //In that case it does not matter which is the selected rectangle. Hence we leave the already selected rectangle as the better fit.
                    }
                }
            }
        }
    }

    if (horizontalSnappingBetterFit) {
        qreal proposedRightOfActiveRect = minDistancePosition + xSnapGapAdjustment + mActiveRectWidth;
        if (qBound(mContainerRect.left(), proposedRightOfActiveRect, mContainerRect.right())
            == proposedRightOfActiveRect) {
            mHorizontalSnapFound = true;
            mHorizontalSnapPosition = minDistancePosition + xSnapGapAdjustment;
            mMinVerticalEdgesDistance = distanceVerticalEdges;
            mVerticalDistanceFromSelectedRect = mVerticalDistance;
            //Save the new distance of the Chosen Rectangle's left edge from Container's left edge
            mContainerVerticalEdgeDistance = differenceContainerLeftEdgeToInactiveRectLeftEdge;

            if (mRectLieAbove) {
                mVerticalLine.setP1(QPointF(minDistancePosition, mInactiveRectToCompare.top()));
                mVerticalLine.setP2(QPointF(minDistancePosition, mMovingRect.bottom()));
            }
            else {
                mVerticalLine.setP1(QPointF(minDistancePosition, mInactiveRectToCompare.bottom()));
                mVerticalLine.setP2(QPointF(minDistancePosition, mMovingRect.top()));
            }
        }
    }
}

/*!
    Check if the right edge of moving rect is snappable to the incative rect's left or right edge.
    The inactive rect's edge is only chosen if it is a better fit for horizontal snapping.
*/
void HsSnapToLines::compareRightSideOfMovingRectForSnapping()
{
    checkInactiveRectLieAboveOrBelowOfMovingRect();

    //calculate the distance of the moving rect's right edge to the inactive rect's left and right edges
    qreal rightToLeftOfInactiveRect = qAbs(mInactiveRectToCompare.left() - mMovingRect.right());
    qreal rightToRightOfInactiveRect = qAbs(mInactiveRectToCompare.right() - mMovingRect.right());
    mLeftInRange = false;
    mRightInRange = false;

    if (rightToLeftOfInactiveRect <= mMinVerticalEdgesDistance) {
        if (mRectLieAbove && mInactiveSnapRectToCompare.isLeftSnapableForBelow
         || !mRectLieAbove && mInactiveSnapRectToCompare.isLeftSnapableForAbove) {
            mLeftInRange = true;
        }
    }
    if (rightToRightOfInactiveRect <= mMinVerticalEdgesDistance) {
        if (mRectLieAbove && mInactiveSnapRectToCompare.isRightSnapableForBelow
         || !mRectLieAbove && mInactiveSnapRectToCompare.isRightSnapableForAbove) {
            mRightInRange = true;
        }
    }

    //calculate the distance of inactive rect's left edge and container rect's left edge
    qreal differenceContainerLeftEdgeToInactiveRectLeftEdge = mInactiveRectToCompare.left() - mContainerRect.left();
    //calculate the distance of inactive rect's right edge and container rect's right edge
    qreal differenceContainerRightEdgeToInactiveRectRightEdge = mContainerRect.right() - mInactiveRectToCompare.right();
    qreal minDistancePosition = 0.0;
    qreal distanceVerticalEdges = 0.0;
    qreal xSnapGapAdjustment = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mLeftInRange && !mRightInRange)
        || !mLeftInRange && mRightInRange) {
        if (mLeftInRange) {
            minDistancePosition = mInactiveRectToCompare.left();
            distanceVerticalEdges = rightToLeftOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
        }
        else {
            minDistancePosition = mInactiveRectToCompare.right();
            distanceVerticalEdges = rightToRightOfInactiveRect;
            xSnapGapAdjustment = 0.0;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mLeftInRange && mRightInRange) {
        //if right edge of moving rect to the right of the inactive rect is closer than the right edge of moving rect to the left of inactive rect
        if (rightToRightOfInactiveRect < rightToLeftOfInactiveRect) {
            minDistancePosition = mInactiveRectToCompare.right();
            distanceVerticalEdges = rightToRightOfInactiveRect;
            xSnapGapAdjustment = 0.0;
            mLeftInRange = false;
        }
        //if the right edge of moving rect to right of inactive rect is at the same distance as the right edge of moving rect to the left of inactive rect
        else if (rightToRightOfInactiveRect == rightToLeftOfInactiveRect) {
            //if inactive rect lies towards the right of container rect, then the right edge is priortized as the selected edge for outside snapping
            if (differenceContainerRightEdgeToInactiveRectRightEdge < differenceContainerLeftEdgeToInactiveRectLeftEdge ) { 
                minDistancePosition = mInactiveRectToCompare.right();
                distanceVerticalEdges = rightToRightOfInactiveRect;
                xSnapGapAdjustment = 0.0;
                mLeftInRange = false;
            }
            //else left of the inactive rect lies more close to the left or middle of the container rect, and hence prioritize it
            else {
                minDistancePosition = mInactiveRectToCompare.left();
                distanceVerticalEdges = rightToLeftOfInactiveRect;
                xSnapGapAdjustment = mSnapGap;
                mRightInRange = false;
            }
        }
        //else right edge of moving rect to the left of the inactive rect is closer than the right edge of moving rect to the right of the incoming rect
        else{
            minDistancePosition = mInactiveRectToCompare.left();
            distanceVerticalEdges = rightToLeftOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
            mRightInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect 
    bool horizontalSnappingBetterFit = false;
    if (mLeftInRange || mRightInRange) {
        if (distanceVerticalEdges < mMinVerticalEdgesDistance) {
            horizontalSnappingBetterFit = true;
        }
        else if (distanceVerticalEdges == mMinVerticalEdgesDistance) { //the distance in the vertical edge is same as from the selected rectangle
            //check the position of rect with respect to Vertical line
            checkInactiveRectPositionToVerticalLine();
            //if horizontal snap position was previously found and the rect's edge is in line with Vertical line
            if (mHorizontalSnapFound && mRectVerticalEdgeLiesInLineWithVerticalLine) {
                if (mRectLieAboveVerticalLine || mRectLieBelowVerticalLine) {
                    extendVerticalLineToIncludeInactiveRect();
                }
            }
            //here the case is that moving rect lies exactly in middle of two same sides of two inactive widgets.
            else {
                //Prioritize first on the fact if the inactive rect is closer to the moving rect in Y - direction.
                if (mVerticalDistance < mVerticalDistanceFromSelectedRect) {
                    horizontalSnappingBetterFit = true;
                }
                else if (mVerticalDistance == mVerticalDistanceFromSelectedRect) {
                    //Prioritize next if this Inactive rect is closer to the right edge of the container rect, then the previously selected rect
                    if (differenceContainerRightEdgeToInactiveRectRightEdge < mContainerVerticalEdgeDistance) {
                        horizontalSnappingBetterFit = true;
                    }
                    //Prioritize next if the Inactive rect's right edge lies near to right edge of the container rect
                    else if (differenceContainerRightEdgeToInactiveRectRightEdge < differenceContainerLeftEdgeToInactiveRectLeftEdge) {
                        horizontalSnappingBetterFit = true;
                    }
                    else {
                        //This else will happen if this rectangle being compared is exactly the same as the selected rectangle for snapping, but in opposite Y direction.
                        //In that case it does not matter which is the selected rectangle. Hece we leave the already selected rectangle as the better fit.
                    }
                }
            }
        }
    }

    if (horizontalSnappingBetterFit) {
        qreal proposedLeftOfActiveRect = minDistancePosition - mActiveRectWidth - xSnapGapAdjustment;
        if (qBound(mContainerRect.left(), proposedLeftOfActiveRect, mContainerRect.right())
            == proposedLeftOfActiveRect) {
            mHorizontalSnapFound = true;
            mHorizontalSnapPosition = proposedLeftOfActiveRect;
            mMinVerticalEdgesDistance = distanceVerticalEdges;
            mVerticalDistanceFromSelectedRect = mVerticalDistance;
            //Save the new distance of the Chosen Rectangle's right edge from Container's right edge
            mContainerVerticalEdgeDistance = differenceContainerRightEdgeToInactiveRectRightEdge;

            if (mRectLieAbove) {
                //save the points for the Vertical line
                mVerticalLine.setP1(QPointF(minDistancePosition, mInactiveRectToCompare.top()));
                mVerticalLine.setP2(QPointF(minDistancePosition, mMovingRect.bottom()));
            }
            else {
                //save the points for the Vertical line
                mVerticalLine.setP1(QPointF(minDistancePosition, mInactiveRectToCompare.bottom()));
                mVerticalLine.setP2(QPointF(minDistancePosition, mMovingRect.top()));
            }
        }
    }
}

/*!
    Check if the inactive rect being compared with moving rect lies on left or right of moving rect.
*/
void HsSnapToLines::checkInactiveRectLieLeftOrRightOfMovingRect()
{
    mRectLieLeft = false;
    mHorizontalDistance = 0.0;
    //Inactive Rect lies left of the Moving Rect
    if (mInactiveRectToCompare.left() < mMovingRect.left()) {
        mRectLieLeft = true;
        if (mInactiveRectToCompare.right() <= mMovingRect.left()) {
            mHorizontalDistance = mMovingRect.left() - mInactiveRectToCompare.right();
        }
        else {
            mHorizontalDistance = mMovingRect.left() - mInactiveRectToCompare.left();
        }
    }
    //Inactive Rect lies right of the Moving Rect
    else {
        mRectLieLeft = false;
        if (mMovingRect.right() <= mInactiveRectToCompare.left()) {
            mHorizontalDistance = mInactiveRectToCompare.left() - mMovingRect.right();
        }
        else {
            mHorizontalDistance = mInactiveRectToCompare.right() - mMovingRect.right();
        }
    }
}

/*!
    Check if the top edge of moving rect is snappable to the incative rect's top or bottom edge.
    The inactive rect's edge is only chosen if it is a better fit for vertical snapping.
*/
void HsSnapToLines::compareTopOfMovingRectForSnapping()
{
    //Check if the inactive rect lies to the left or right of the moving rect
    checkInactiveRectLieLeftOrRightOfMovingRect();

    //calculate the distance of the moving rect's top edge to the inactive rect's top and bottom edges
    qreal topToTopOfInactiveRect = qAbs(mInactiveRectToCompare.top() - mMovingRect.top());
    qreal topToBottomOfInactiveRect = qAbs(mInactiveRectToCompare.bottom() - mMovingRect.top());
    mTopInRange = false;
    mBottomInRange = false;

    if (topToTopOfInactiveRect <= mMinHorizontalEdgesDistance) {
        if (mRectLieLeft && mInactiveSnapRectToCompare.isTopSnapableForRight
         || !mRectLieLeft && mInactiveSnapRectToCompare.isTopSnapableForLeft) {
            mTopInRange = true;
        }
    }
    if (topToBottomOfInactiveRect <= mMinHorizontalEdgesDistance) {
        if (mRectLieLeft && mInactiveSnapRectToCompare.isBottomSnapableForRight
         || !mRectLieLeft && mInactiveSnapRectToCompare.isBottomSnapableForLeft) {
            mBottomInRange = true;
        }
    }

    //calculate the distance of inactive rect's top edge and container rect's top edge
    qreal differenceContainerTopEdgeToInactiveRectTopEdge = mInactiveRectToCompare.top() - mContainerRect.top();
    //calculate the distance of inactive rect's bottom edge and container rect's bottom edge
    qreal differenceContainerBottomEdgeToInactiveRectBottomEdge = mContainerRect.bottom() - mInactiveRectToCompare.bottom();
    qreal minDistancePosition = 0.0;
    qreal distanceHorizontalEdges = 0.0;
    qreal ySnapGapAdjustment = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mTopInRange && !mBottomInRange)
        || !mTopInRange && mBottomInRange) {
        if (mTopInRange) {
            minDistancePosition = mInactiveRectToCompare.top();
            distanceHorizontalEdges = topToTopOfInactiveRect;
            ySnapGapAdjustment = 0.0;
        }
        else {
            minDistancePosition = mInactiveRectToCompare.bottom();
            distanceHorizontalEdges = topToBottomOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mTopInRange && mBottomInRange) {
        //if top edge of moving rect to the top of the inactive rect is closer than the bottom edge of moving rect to the bottom of the inactive rect
        if (topToTopOfInactiveRect < topToBottomOfInactiveRect) {
            minDistancePosition = mInactiveRectToCompare.top();
            distanceHorizontalEdges = topToTopOfInactiveRect;
            ySnapGapAdjustment = 0.0;
            mBottomInRange = false;
        }
        //if the top edge of moving rect to top of inactive rect is at the same distance as the top edge of moving rect to the bottom of inactive rect
        else if (topToTopOfInactiveRect == topToBottomOfInactiveRect) {
            //if inactive rect lies towards the top or middle of container rect, then the top edge is priortized as the selected edge for outside snapping
            if (differenceContainerTopEdgeToInactiveRectTopEdge <= differenceContainerBottomEdgeToInactiveRectBottomEdge) { 
                minDistancePosition = mInactiveRectToCompare.top();
                distanceHorizontalEdges = topToTopOfInactiveRect;
                ySnapGapAdjustment = 0.0;
                mBottomInRange = false;
            }
            //else bottom of the inactive rect lies more close to the bottom of the container rect, and hence prioritize it for snapping.
            else {
                minDistancePosition = mInactiveRectToCompare.bottom();
                distanceHorizontalEdges = topToBottomOfInactiveRect;
                ySnapGapAdjustment = mSnapGap;
                mTopInRange = false;
            }
        }
        //else top edge of moving rect to the bottom of the inactive rect is closer than the top edge of moving rect to the top of the inactive rect
        else{
            minDistancePosition = mInactiveRectToCompare.bottom();
            distanceHorizontalEdges = topToBottomOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
            mTopInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect 
    bool verticalSnappingBetterFit = false;
    if (mTopInRange || mBottomInRange) {
        if (distanceHorizontalEdges < mMinHorizontalEdgesDistance) {
            verticalSnappingBetterFit = true;
        }
        else if (distanceHorizontalEdges == mMinHorizontalEdgesDistance) { //the distance in the horizontal edge is same as from the selected rectangle
            //check the position of rect with respect to horizontal line
            checkInactiveRectPositionToHorizontalLine();
            //if vertical snap position was already found and this rect's horizontal edges lies in line with Horizontal snap line
            if (mVerticalSnapFound && mRectHorizontalEdgeLiesInLineWithHorizontalLine) {
                if (mRectLieLeftOfHorizontalLine || mRectLiesRightOfHorizontalLine) {
                    extendHorizontalLineToIncludeInactiveRect();
                }
            }
            else {
                //Prioritize first on the fact if the inactive rect is closer to the moving rect in X - direction.
                if (mHorizontalDistance < mHorizontalDistanceFromSelectedRect) {
                    verticalSnappingBetterFit = true;
                }
                else if (mHorizontalDistance == mHorizontalDistanceFromSelectedRect) {
                    //Prioritize next if this Inactive rect is closer to the top edge of the container rect, then the previously selected rect
                    if (differenceContainerTopEdgeToInactiveRectTopEdge < mContainerHorizontalEdgeDistance) {
                        verticalSnappingBetterFit = true;
                    }
                    //Prioritize next if the Inactive widget's top edge lies near to top edge of the container rect
                    else if (differenceContainerTopEdgeToInactiveRectTopEdge < differenceContainerBottomEdgeToInactiveRectBottomEdge) {
                        verticalSnappingBetterFit = true;
                    }
                    else {
                        //This else will happen if this rectangle being compared is exactly the same as the selected rectangle for snapping, or in opposite X direction.
                        //In that case it does not matter which is the selected rectangle. Hence we leave the already selected rectangle as the better fit.
                    }
                }
            }
        }
    }

    if (verticalSnappingBetterFit) {
        qreal proposedBottomOfActiveRect = minDistancePosition + mActiveRectHeight + ySnapGapAdjustment;
        if (qBound(mContainerRect.top(), proposedBottomOfActiveRect, mContainerRect.bottom())
            == proposedBottomOfActiveRect) {
            mVerticalSnapFound = true;
            mVerticalSnapPosition = minDistancePosition + ySnapGapAdjustment;
            mMinHorizontalEdgesDistance = distanceHorizontalEdges;
            mHorizontalDistanceFromSelectedRect = mHorizontalDistance;
            //Save the new distance of the Chosen Rectangle's top edge from Container's top edge
            mContainerHorizontalEdgeDistance = differenceContainerTopEdgeToInactiveRectTopEdge;

            if (mRectLieLeft) {
                //save the points for the Horizontal line
                mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.left(), minDistancePosition));
                mHorizontalLine.setP2(QPointF(mMovingRect.right(), minDistancePosition));
            }
            else {
                //save the points for the Horizontal line
                mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.right(), minDistancePosition));
                mHorizontalLine.setP2(QPointF(mMovingRect.left(), minDistancePosition));
            }
        }
    }
}

/*!
    Check if the bottom edge of moving rect is snappable to the incative rect's top or bottom edge.
    The inactive rect's edge is only chosen if it is a better fit for vertical snapping.
*/
void HsSnapToLines::compareBottomOfMovingRectForSnapping()
{
    //Check if the inactive rect lies to the left or right of the moving rect
    checkInactiveRectLieLeftOrRightOfMovingRect();

    //calculate the distance of the moving rect's bottom edge to the inactive rect's top and bottom edges
    qreal bottomToTopOfInactiveRect = qAbs(mInactiveRectToCompare.top() - mMovingRect.bottom());
    qreal bottomToBottomOfInactiveRect = qAbs(mInactiveRectToCompare.bottom() - mMovingRect.bottom());
    mTopInRange = false;
    mBottomInRange = false;

    if (bottomToTopOfInactiveRect <= mMinHorizontalEdgesDistance) {
        if (mRectLieLeft && mInactiveSnapRectToCompare.isTopSnapableForRight
         || !mRectLieLeft && mInactiveSnapRectToCompare.isTopSnapableForLeft) {
            mTopInRange = true;
        }
    }
    if (bottomToBottomOfInactiveRect <= mMinHorizontalEdgesDistance) {
        if (mRectLieLeft && mInactiveSnapRectToCompare.isBottomSnapableForRight
         || !mRectLieLeft && mInactiveSnapRectToCompare.isBottomSnapableForLeft) {
            mBottomInRange = true;
        }
    }

    //calculate the distance of inactive rect's top edge and container rect's top edge
    qreal differenceContainerTopEdgeToInactiveRectTopEdge = mInactiveRectToCompare.top() - mContainerRect.top();
    //calculate the distance of inactive rect's bottom edge and container rect's bottom edge
    qreal differenceContainerBottomEdgeToInactiveRectBottomEdge = mContainerRect.bottom() - mInactiveRectToCompare.bottom();
    qreal minDistancePosition = 0.0;
    qreal distanceHorizontalEdges = 0.0;
    qreal ySnapGapAdjustment = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mTopInRange && !mBottomInRange)
        || !mTopInRange && mBottomInRange) {
        if (mTopInRange) {
            minDistancePosition = mInactiveRectToCompare.top();
            distanceHorizontalEdges = bottomToTopOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
        }
        else {
            minDistancePosition = mInactiveRectToCompare.bottom();
            distanceHorizontalEdges = bottomToBottomOfInactiveRect;
            ySnapGapAdjustment = 0.0;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mTopInRange && mBottomInRange) {
        //if bottom edge of moving rect to the bottom of inactive rect is closer than the bottom edge of moving rect to the top of the inactive rect
        if (bottomToBottomOfInactiveRect < bottomToTopOfInactiveRect ) {
            minDistancePosition = mInactiveRectToCompare.bottom();
            distanceHorizontalEdges = bottomToBottomOfInactiveRect;
            ySnapGapAdjustment = 0.0;
            mTopInRange = false;
        }
        //if bottom edge of moving rect to the bottom of inactive rect is at the same distance as the bottom edge of moving rect to the top of inactive rect
        else if (bottomToBottomOfInactiveRect == bottomToTopOfInactiveRect) {
            //if inactive rect lies towards the bottom of container rect, then the bottom edge is priortized as the selected edge for snapping
            //This is done for outside snapping
            if (differenceContainerBottomEdgeToInactiveRectBottomEdge < differenceContainerTopEdgeToInactiveRectTopEdge) {
                minDistancePosition = mInactiveRectToCompare.bottom();
                distanceHorizontalEdges = bottomToBottomOfInactiveRect;
                ySnapGapAdjustment = 0.0;
                mTopInRange = false;
            }
            //else top of the inactive rect lies more close to the top of the container rect or at the same distance, and hence prioritize it
            else {
                minDistancePosition = mInactiveRectToCompare.top();
                distanceHorizontalEdges = bottomToTopOfInactiveRect;
                ySnapGapAdjustment = mSnapGap;
                mBottomInRange = false;
            }
        }
        //else bottom edge of moving rect to the top of inactive rect is closer than the bottom edge of moving rect to the bottom of the inactive rect
        else{
            minDistancePosition = mInactiveRectToCompare.top();
            distanceHorizontalEdges = bottomToTopOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
            mBottomInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect 
    bool verticalSnappingBetterFit = false;
    if (mTopInRange || mBottomInRange) {
        if (distanceHorizontalEdges < mMinHorizontalEdgesDistance) {
            verticalSnappingBetterFit = true;
        }
        else if (distanceHorizontalEdges == mMinHorizontalEdgesDistance) { //the distance in the horizontal edge is same as from the selected rectangle
            //check the position of rect with respect to horizontal line
            checkInactiveRectPositionToHorizontalLine();
            //if vertical snap was already found and the horizontal line of rect is in line with horizontal snap line
            if (mVerticalSnapFound && mRectHorizontalEdgeLiesInLineWithHorizontalLine) {
                if (mRectLieLeftOfHorizontalLine || mRectLiesRightOfHorizontalLine) {
                    extendHorizontalLineToIncludeInactiveRect();
                }
            }
            else {
                //Prioritize first on the fact if the inactive rect is closer to the moving rect in X - direction.
                if (mHorizontalDistance < mHorizontalDistanceFromSelectedRect) {
                    verticalSnappingBetterFit = true;
                }
                else if (mHorizontalDistance == mHorizontalDistanceFromSelectedRect) {
                    //Prioritize next if this Inactive rect is closer to the bottom edge of the container rect, then the previously selected rect
                    if (differenceContainerBottomEdgeToInactiveRectBottomEdge < mContainerHorizontalEdgeDistance) {
                        verticalSnappingBetterFit = true;
                    }
                    //Prioritize next if the Inactive widget's bottom edge lies near to bottom edge of the container rect
                    else if (differenceContainerBottomEdgeToInactiveRectBottomEdge < differenceContainerTopEdgeToInactiveRectTopEdge) {
                        verticalSnappingBetterFit = true;
                    }
                    else {
                        //This else will happen if this rectangle being compared is exactly the same as the selected rectangle for snapping, or in opposite X direction.
                        //In that case it does not matter which is the selected rectangle. Hence we leave the already selected rectangle as the better fit.
                    }
                }
            }
        }
    }

    if (verticalSnappingBetterFit) {
        qreal proposedTopOfActiveRect = minDistancePosition - mActiveRectHeight - ySnapGapAdjustment;
        if (qBound(mContainerRect.top(), proposedTopOfActiveRect, mContainerRect.bottom())
            == proposedTopOfActiveRect) {
            mVerticalSnapFound = true;
            mVerticalSnapPosition = proposedTopOfActiveRect;
            mMinHorizontalEdgesDistance = distanceHorizontalEdges;
            mHorizontalDistanceFromSelectedRect = mHorizontalDistance;
            //Save the new distance of the Selected Rectangle's bottom edge from Container's bottom edge
            mContainerHorizontalEdgeDistance = differenceContainerBottomEdgeToInactiveRectBottomEdge;

            if (mRectLieLeft) {
                //save the points for the Horizontal line
                mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.left(), minDistancePosition));
                mHorizontalLine.setP2(QPointF(mMovingRect.right(), minDistancePosition));
            }
            else {
                //save the points for the Horizontal line
                mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.right(), minDistancePosition));
                mHorizontalLine.setP2(QPointF(mMovingRect.left(), minDistancePosition));
            }
        }
    }
}

/*!
    Extend the Vertical line on both side of reference(snapping) rectancles.
*/
void HsSnapToLines::extendVerticalLine()
{
    if (mVerticalLine.y1() <= mVerticalLine.y2()) {
        mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mVerticalLine.y1() - SNAP_LINE_EXTEND_VALUE));
        mVerticalLine.setP2(QPointF(mVerticalLine.x2(), mVerticalLine.y2() + SNAP_LINE_EXTEND_VALUE));
    }
    else {
        mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mVerticalLine.y1() + SNAP_LINE_EXTEND_VALUE));
        mVerticalLine.setP2(QPointF(mVerticalLine.x2(), mVerticalLine.y2() - SNAP_LINE_EXTEND_VALUE));
    }
}

/*!
    Extend the Horizontal line on both side of reference(snapping) rectancles.
*/
void HsSnapToLines::extendHorizontalLine()
{
    if (mHorizontalLine.x1() <= mHorizontalLine.x2()) {
        mHorizontalLine.setP1(QPointF(mHorizontalLine.x1() - SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y1()));
        mHorizontalLine.setP2(QPointF(mHorizontalLine.x2() + SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y2()));
    }
    else {
        mHorizontalLine.setP1(QPointF(mHorizontalLine.x1() + SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y1()));
        mHorizontalLine.setP2(QPointF(mHorizontalLine.x2() - SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y2()));
    }
}

/*!
    Check if the Vertical edge of the Rectangle lies out of the Vertical line.
    Also check if the rectangle's edge lies out of the line.
*/
void HsSnapToLines::checkInactiveRectPositionToVerticalLine()
{
    mRectVerticalEdgeLiesInLineWithVerticalLine = false;
    mRectLieAboveVerticalLine = false;
    mRectLieBelowVerticalLine = false;

    //if rectangle vertical edge lies inline with Vertical line.
    if ((mLeftInRange && mInactiveRectToCompare.left() == mVerticalLine.x1())
        || (mRightInRange && mInactiveRectToCompare.right() == mVerticalLine.x1())) {
        mRectVerticalEdgeLiesInLineWithVerticalLine = true;
        //if the rectangle lies below the vertical line
        if (mInactiveRectToCompare.bottom() > mVerticalLine.y1() && mInactiveRectToCompare.bottom() > mVerticalLine.y2()) {
            mRectLieBelowVerticalLine = true;
        }
        //if the rectangle lies above the vertical line
        if (mInactiveRectToCompare.top() < mVerticalLine.y1() && mInactiveRectToCompare.top() < mVerticalLine.y2()) {
            mRectLieAboveVerticalLine = true;
        }
    }
}

/*!
    Increase the Vertical line to include the inactive rect whose vertical edge is inline with vertical line
*/
void HsSnapToLines::extendVerticalLineToIncludeInactiveRect()
{
    if (mRectLieAboveVerticalLine) {
        if (mVerticalLine.y1() < mVerticalLine.y2()) {
            mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.top()));
        }
        else {
            mVerticalLine.setP2(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.top()));
        }
    }
    if (mRectLieBelowVerticalLine) {
        if (mVerticalLine.y1() < mVerticalLine.y2()) {
            mVerticalLine.setP2(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.bottom()));
        }
        else {
            mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.bottom()));
        }
    }
}

/*!
    Check if the Horizontal edge of the Rectangle lies inline with the Horizontal line.
    Also check if the rectangle's edge lies out of the line.
*/
void HsSnapToLines::checkInactiveRectPositionToHorizontalLine()
{
    mRectHorizontalEdgeLiesInLineWithHorizontalLine = false;
    mRectLieLeftOfHorizontalLine = false;
    mRectLiesRightOfHorizontalLine = false;

    //if rectangle horizontal edge lies inline with Horizontal line.
    if ((mTopInRange && mInactiveRectToCompare.top() == mHorizontalLine.y1())
        || (mBottomInRange && mInactiveRectToCompare.bottom() == mHorizontalLine.y1())) {
        mRectHorizontalEdgeLiesInLineWithHorizontalLine = true;
        //if the rectangle lies left of the horizontal line
        if (mInactiveRectToCompare.left() < mHorizontalLine.x1() && mInactiveRectToCompare.left() < mHorizontalLine.x2()) {
            mRectLieLeftOfHorizontalLine = true;
        }
        //if the rectangle lies right of the horizontal line
        if (mInactiveRectToCompare.right() > mHorizontalLine.x1() && mInactiveRectToCompare.right() > mHorizontalLine.x2()) {
            mRectLiesRightOfHorizontalLine = true;
        }
    }
}

/*!
    Increase the Horizontal line to include the inactive rect whose horizontal edge is inline with horizontal line
*/
void HsSnapToLines::extendHorizontalLineToIncludeInactiveRect()
{
    if (mRectLieLeftOfHorizontalLine) {
        if (mHorizontalLine.x1() < mHorizontalLine.x2()) {
            mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.left(), mHorizontalLine.y1()));
        }
        else {
            mHorizontalLine.setP2(QPointF(mInactiveRectToCompare.left(), mHorizontalLine.y1()));
        }
    }
    if (mRectLiesRightOfHorizontalLine) {
        if (mHorizontalLine.x1() < mHorizontalLine.x2()) {
            mHorizontalLine.setP2(QPointF(mInactiveRectToCompare.right(), mHorizontalLine.y1()));
        }
        else {
            mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.right(), mHorizontalLine.y1()));
        }
    }
}
