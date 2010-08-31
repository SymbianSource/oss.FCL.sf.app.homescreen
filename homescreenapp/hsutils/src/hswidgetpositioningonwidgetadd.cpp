/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QLineF>
#include <QtGlobal>
#include <QPointF>
#include <math.h>

#include <HbInstance>


#include "hswidgetpositioningonwidgetadd.h"

const qreal offset = 40; //TODO: Implement this as configurable parameter


/*!
    \class HsWidgetPositioningOnWidgetAdd
    \ingroup group_hsutils
    \brief 
*/

/*!
    \class HsWidgetPositioningOnWidgetAdd
    \brief Defines widget positioning on widget add.

    Widget positioning on widget add sets positions for
    a set of home screen widgets added from application library.
*/

/*!
    Sets the positioning \a instance as the current one.
    Deletes the existing instance if present.
*/
void HsWidgetPositioningOnWidgetAdd::setInstance(
    HsWidgetPositioningOnWidgetAdd *instance)
{
    if (mInstance)
        delete mInstance;
    mInstance = instance;
}
 
/*!
    Returns the current positioning instance.
*/
HsWidgetPositioningOnWidgetAdd *HsWidgetPositioningOnWidgetAdd::instance()
{
    return mInstance;
}

/*!
    Stores the current positioning instance.
*/
HsWidgetPositioningOnWidgetAdd *HsWidgetPositioningOnWidgetAdd::mInstance = 0;

/*!
    \class HsAnchorPointInBottomRight
    \brief Diagonal widget positioning algorithm.
    
    Sets widget's lower right corner to follow content area's diagonal.
    Widgets are positioned to certain offset to each other.
*/
QList<QRectF> HsAnchorPointInBottomRight::convert(
    const QRectF &contentArea,
    const QList<QRectF> &existingRects,
    const QList<QRectF> &newRects,
    const QPointF &startPoint)
{
    Q_UNUSED(existingRects);

    QList<QRectF> toGeometries;

    //Offset for widgets' bottom right position to each other
    qreal k = contentArea.height()/contentArea.width(); //slope of the diagonal
    qreal offset_x = offset/(sqrt(k + 1));
    qreal offset_y = k*offset_x;
    QPointF offsetPoint(offset_x, offset_y);
    
    QPointF anchorPoint;
   
    if(startPoint.isNull()){

        QLineF diagonal(contentArea.topLeft(), contentArea.bottomRight());
        QLineF widgetRightSide(contentArea.center().x()+ newRects.at(0).width()/2,
                           contentArea.top(),
                           contentArea.center().x()+ newRects.at(0).width()/2,
                           contentArea.bottom());

        // right side line intersection with diagonal will be bottom right position
        // for the first rect
        if(QLineF::BoundedIntersection != 
            diagonal.intersect(widgetRightSide, &anchorPoint)) {
            return newRects; //Return original since undefined error.
                            //In this case widget's must be wider than the content area.
        }
    }else{
        anchorPoint = startPoint - offsetPoint;
    }

    QRectF widgetRect;
    for(int i=0;i<newRects.count();++i) {
        widgetRect = newRects.at(i);
        widgetRect.moveBottomRight(anchorPoint);
        //if widget rect doesn't fit, try to move it
        if(!contentArea.contains(widgetRect)) {
            /*! precondition is that
             widget's max height < content area height
             widget's max widht < content area width
            */
            widgetRect.moveBottomRight(contentArea.bottomRight());
            // anchorPoin is always previous bottom right
            anchorPoint = widgetRect.bottomRight();
        }
        toGeometries << widgetRect;
        anchorPoint -= offsetPoint;
        
    }
    return toGeometries;
}


/*!
    \class HsAnchorPointInCenter
    \brief Diagonal widget positioning algorithm.
    
    Sets widget's center point to follow content area's diagonal.
    Widgets are positioned to certain offset to each other.
*/
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
QList<QRectF> HsAnchorPointInCenter::convert(
    const QRectF &contentArea,
    const QList<QRectF> &existingRects,
    const QList<QRectF> &newRects,
    const QPointF &startPoint )
{
    Q_UNUSED(existingRects);
    Q_UNUSED(startPoint)

    QList<QRectF> toGeometries;

    //Offset for widgets' centers position to each other
    qreal k = contentArea.height()/contentArea.width(); //slope of the diagonal
    qreal offset_x = offset/(sqrt(k + 1));
    qreal offset_y = k*offset_x;
    QPointF offsetPoint(offset_x, offset_y);

    //First widget to the center of the content area
    QPointF anchorPoint = contentArea.center();
    foreach (QRectF g, newRects) {
        g.moveCenter(anchorPoint);
        toGeometries << g;
        anchorPoint -= offsetPoint;
        if(!contentArea.contains(anchorPoint)) {
            anchorPoint = contentArea.bottomRight();
        }
    }
    return toGeometries;
}

/*!
    \class HsWidgetOrganizer
    \brief Advanced widget positioning algorithm.
    
    Organizes widget's starting from upper left corner towards right,
    and then continues the on the next line.
*/
QList<QRectF> HsWidgetOrganizer::convert(
    const QRectF &contentArea,
    const QList<QRectF> &existingRects,
    const QList<QRectF> &newRects,
    const QPointF &startPoint)
{
    Q_UNUSED(startPoint)

    // TODO: maybe we can utilize start point in some use cases / optimizations?

    QList<QRectF> toGeometries;

    // TODO: anchor distance to configuration?
    // TODO: optimize anchor distance based on new content amount
    // TODO: snap value to same as anchor distance?
    mAnchorDistance = 5;
    QList<bool> temp;
    mAnchors = temp;

    // test flag
//    int test = 0;

    // initialize anchor network for widget positions
//    if (test == 0) {
        initAnchors(contentArea.size());
//    } else {
//        mAnchorDistance = 2;
//        initAnchors(QSizeF(6,6));
//    }

    // mark existing rects (widgets) reserved
    foreach (QRectF rect, existingRects) {
        // TODO: could mStartWidthAnchorPoint, mEndWidthAnchorPoint, mEndHeightAnchorPoint be somehow refactored better way?
        mStartWidthAnchorPoint.setX(lenghtInAnchorPoints(rect.x() - contentArea.x()));
        mEndWidthAnchorPoint.setX(lenghtInAnchorPoints(rect.x() + rect.width() - contentArea.x()));
        mStartWidthAnchorPoint.setY(lenghtInAnchorPoints(rect.y() - contentArea.y()));
        mEndHeightAnchorPoint.setY(lenghtInAnchorPoints(rect.y() + rect.height() - contentArea.y()));
        // mark reserved anchor points
        markReservedAnchors();
        mStartWidthAnchorPoint = QPointF(0,0);
        mEndWidthAnchorPoint = QPointF(0,0);
        mEndHeightAnchorPoint = QPointF(0,0);
    }

    QList<QRectF> notOrganizedRects;

    // get positions for all new rects (widgets)
    for ( int i = 0; i < newRects.count(); i++) {
        bool found = false;
//        if (test == 0) {
            // find first free anchor point for rect
            found = getAnchorPoint(newRects.at(i).size());
//        } else {
//            found = getAnchorPoint(QSizeF(2,2));
//        }

        if (found) {
            // save to geometry list
            toGeometries << QRectF(mStartWidthAnchorPoint.x() * mAnchorDistance + contentArea.x(),
                                   mStartWidthAnchorPoint.y() * mAnchorDistance + contentArea.y(),
                                   newRects.at(i).width(), newRects.at(i).height());
            // mark new widgets rect reserved
            markReservedAnchors();
            // TODO: these optimizations could be used for empty page
            //mStartWidthAnchorPoint.setX(mEndWidthAnchorPoint.x() + 1);
            //mStartWidthAnchorPoint.setY(mEndWidthAnchorPoint.y());
        } else {
            // collect widgets that do not fit
            notOrganizedRects << newRects.at(i);
        }
        // TODO: remove these to optimize for empty page
        mStartWidthAnchorPoint = QPointF(0,0);
        mEndWidthAnchorPoint = QPointF(0,0);
    }

    // use center algorithm with offset for the rest widget that did not fit to screen
    if (notOrganizedRects.count() > 0) {
        QList<QRectF> tmpExistingRects;
        tmpExistingRects += newRects;
        tmpExistingRects += existingRects;
        HsAnchorPointInCenter *centerAlgorithm = new HsAnchorPointInCenter();
        QList<QRectF> calculatedRects =
            centerAlgorithm->convert(contentArea, tmpExistingRects, notOrganizedRects, QPointF());
        toGeometries += calculatedRects;
        delete centerAlgorithm;
    }

    return toGeometries;
}


/*!    
    Initializes anchor points for context area
*/
bool HsWidgetOrganizer::initAnchors(const QSizeF &areaSize)
{
    // mandatory check ups
    // TODO: these mAnchorDistance checks to earlier phase
    if (areaSize == QSizeF(0,0) || areaSize.width() < mAnchorDistance ||
        areaSize.height() < mAnchorDistance || mAnchorDistance == 0 || mAnchorDistance == 1) {
        return false;
    }
    mAnchorColumns = 0;
    mAnchorRows = 0;

    // TODO: can we optimize anchor amount utilizing minimum widget size
    mAnchorColumns = lenghtInAnchorPoints(areaSize.width());
    mAnchorRows = lenghtInAnchorPoints(areaSize.height());

    // create anchor network
    for (int i = 0; i < (mAnchorRows * mAnchorColumns); i++) {
        mAnchors << false;
    }
    // zero start points
    mStartWidthAnchorPoint = QPointF(0,0);
    mEndWidthAnchorPoint = QPointF(0,0);

    return true;
}

/*!    
    Finds anchor points for content size
*/
bool HsWidgetOrganizer::getAnchorPoint(const QSizeF &contentSize)
{
    bool anchorFound = false;

    while (anchorFound == false) {
        // if no width found for content
        if (!searchWidthSpace(contentSize)) {
            // when content organized in height order remove this line for optimization
            mStartWidthAnchorPoint = QPointF(0,0);
            mEndWidthAnchorPoint = QPointF(0,0);
            return false;
        }
        // search height for content
        int height = lenghtInAnchorPoints(contentSize.height());
        anchorFound = searchHeightSpace(height);
    }
    return true;
}

/*!    
    Searches anchor point width for content size
*/
bool HsWidgetOrganizer::searchWidthSpace(const QSizeF &contentSize) 
{
    int availableWidth = 0;    
    int contentWidth = lenghtInAnchorPoints(contentSize.width());
    // TODO: use this optimizations for empty page
    //int contentHeight = lenghtInAnchorPoints(contentSize.height());
    bool newRow = true;

    for (int i = getIndexForCoordinate(mStartWidthAnchorPoint); i <= mAnchors.count(); i++) {
        // no width left on the page
        if ((newRow == false) && ((i % (mAnchorColumns)) == 0)) {
            availableWidth = 0;
            // jump to new row
            mStartWidthAnchorPoint.setX(0);
            // TODO: use this optimizations for empty page
            //mStartWidthAnchorPoint.setY(mStartWidthAnchorPoint.y() + contentHeight + 1);
            mStartWidthAnchorPoint.setY(mStartWidthAnchorPoint.y() + 1);
            i = getIndexForCoordinate(mStartWidthAnchorPoint) - 1;
            // if no height found
            if (i < 0) {
                return false;
            }
            newRow = true;
        } else {
            // if enough width found
            if (availableWidth == contentWidth) {
                mEndWidthAnchorPoint = getAnchorCoordinates(i);
                if (mEndWidthAnchorPoint == QPointF()) {
                    return false;
                }
                return true;
            }
            // if anchor reserved
            if (mAnchors[i] == true) {
                availableWidth = 0;
            } else {
                // update available width
                availableWidth = availableWidth + 1;
            }
            newRow = false;
        }   
    }
    return false;
}

/*!    
    Searches anchor point area for content size
*/
bool HsWidgetOrganizer::searchHeightSpace(int contentHeight)
{
    mEndHeightAnchorPoint = QPointF(0,0);
 
    for (int i = mStartWidthAnchorPoint.x(); i <= mEndWidthAnchorPoint.x(); i++) {
        for (int j = mStartWidthAnchorPoint.y(); j <= (mStartWidthAnchorPoint.y() + contentHeight); j++) {
            int index = getIndexForCoordinate(QPointF(i,j));
            // check that index is not out of bounds
            if (index == -1) {
                // update start width point one step
                mStartWidthAnchorPoint.setX(mStartWidthAnchorPoint.x() + 1); 
                return false;
            }
            // if anchor reserved
            if (mAnchors[index] == true) {
                // update start width point one step
                mStartWidthAnchorPoint.setX(mStartWidthAnchorPoint.x() + 1);
                return false;
            }
        }
    }
    mEndHeightAnchorPoint = QPointF(mEndWidthAnchorPoint.x(), mEndWidthAnchorPoint.y() + contentHeight);
    return true;
}

/*!    
    Marks reserved anchor points based on pre-defined starting and ending points
*/
bool HsWidgetOrganizer::markReservedAnchors()
{
    for (int i = mStartWidthAnchorPoint.x(); i <= mEndWidthAnchorPoint.x(); i++) {
        for (int j = mStartWidthAnchorPoint.y(); j <= mEndHeightAnchorPoint.y(); j++) {
            mAnchors[getIndexForCoordinate(QPointF(i,j))] = true;
        }
    }
    return true;
}

/*!    
    Returns pixel coordinate based on anchor coordinate
*/
QPointF HsWidgetOrganizer::getAnchorCoordinates(int index)
{
    if (index < mAnchors.count()) {
        int x = index % mAnchorColumns;
        int y = (index - x) / mAnchorColumns;
        return QPointF(x,y);
    } else {
        return QPointF();
    }
}

/*!    
    Returns anchor coordinate based on pixel coordinate
*/
int HsWidgetOrganizer::getIndexForCoordinate(QPointF position)
{
    int index = (position.y() * mAnchorColumns) + position.x();
    if (index < mAnchors.count()) {
        return index;
    } else {
        return -1;
    }
}

/*!    
    Calculates pixel length as anchor points
*/
int HsWidgetOrganizer::lenghtInAnchorPoints(QVariant length)
{
    // check remainder
    int remainder = length.toInt() % mAnchorDistance;
    return ((length.toInt() - remainder) / mAnchorDistance);
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

