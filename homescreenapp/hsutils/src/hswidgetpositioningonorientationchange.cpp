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

#include <QtGlobal>
#include "hswidgetpositioningonorientationchange.h"

/*!
    \class HsWidgetPositioningOnOrientationChange
    \brief Defines widget positioning on orientation change.

    Widget positioning on orientation change sets positions for
    a set of home screen widgets from landscape to portrait
    orientation and vice versa.
*/

/*!
    Sets the positioning \a instance as the current one.
    Deletes the existing instance if present.
*/
void HsWidgetPositioningOnOrientationChange::setInstance(
    HsWidgetPositioningOnOrientationChange *instance)
{
    if (mInstance)
        delete mInstance;
    mInstance = instance;
}

/*!
    Returns the current positioning instance.
*/
HsWidgetPositioningOnOrientationChange *HsWidgetPositioningOnOrientationChange::instance()
{
    return mInstance;
}

/*!
    Stores the current positioning instance.
*/
HsWidgetPositioningOnOrientationChange *HsWidgetPositioningOnOrientationChange::mInstance = 0;

/*!
    \class HsSimpleWidgetPositioningOnOrientationChange
    \brief Bounds center points of input geometries to
           the given output rectangle.
*/

/*!
    \class HsSimpleWidgetPositioningOnOrientationChange
    \brief Simpe widget positioning algorithm.

    Bounds center points of \a fromGeometries to the
    \a toRect. \a fromRect is not used by the algorithm.
    \verbatim
    ------
    | A  |
    |    |
    |    |
    |    |
    ------
    \endverbatim
    and after conversion widgets from A are moved to A':
    \verbatim
    |----------|
    |A'        |
    |----------|
    \endverbatim
    and vice versa.
*/
QList<QRectF> HsSimpleWidgetPositioningOnOrientationChange::convert(
    const QRectF &fromRect,
    const QList<QRectF> &fromGeometries,
    const QRectF &toRect)
{
    Q_UNUSED(fromRect)
    QList<QRectF> toGeometries;
    foreach (QRectF g, fromGeometries) {
        QPointF c(g.center());
        qreal x = qBound(toRect.left(), c.x(), toRect.right());
        qreal y = qBound(toRect.top(), c.y(), toRect.bottom());
        g.moveCenter(QPointF(x, y));
        toGeometries << g;
    }
    return toGeometries;
}

/*!
    \class HsAdvancedWidgetPositioningOnOrientationChange
    \brief More advanced widget positioning algorithm.

    Calculates new center points of
    \a fromGeometries when moving from \a fromRect to \a toRect.
    Screen is divided to two pieces:
    \verbatim
    ------
    | A  |
    |    |
    ------
    | B  |
    |    |
    ------
    \endverbatim
    and after conversion widgets from A are moved to A' and widgets from B to B':
    \verbatim
    |-----|-----|
    |A'   | B'  |
    |-----|-----|
    \endverbatim
    and vice versa.
*/
QList<QRectF> HsAdvancedWidgetPositioningOnOrientationChange::convert(
    const QRectF &fromRect,
    const QList<QRectF> &fromGeometries,
    const QRectF &toRect)
{
    QList<QRectF> toGeometries;
	// check if moving from landscape to portrait
    if ( fromRect.width() > fromRect.height() ) {
        foreach (QRectF g, fromGeometries) {
            QRectF leftHalf = QRectF(fromRect.x(), fromRect.y(), fromRect.width()/2, fromRect.height());
            QPointF c(g.center());
            qreal x = 0;
            qreal y = 0;
            if ( leftHalf.contains(c) ) {
                x = qBound(toRect.left(), c.x(), toRect.right());
                y = qBound(toRect.top(), c.y(), toRect.bottom());
            } else {
                QRectF lowerHalf = QRectF(toRect.x(), toRect.y()+toRect.height()/2, toRect.width(), toRect.height()/2);
                x = qBound(toRect.left(), c.x()-leftHalf.width(), toRect.right());
                y = qBound(lowerHalf.top(), c.y()+leftHalf.height(), lowerHalf.bottom());
            }
            g.moveCenter(QPointF(x, y));
            toGeometries << g;
        }
    }
    else {  // moving from portrait to landscape
        foreach (QRectF g, fromGeometries) {
            QRectF upperHalf = QRectF(fromRect.x(), fromRect.y(), fromRect.width(), fromRect.height()/2);
            QPointF c(g.center());
            qreal x = 0;
            qreal y = 0;
            if ( upperHalf.contains(c) ) {
                x = qBound(toRect.left(), c.x(), toRect.right());
                y = qBound(toRect.top(), c.y(), toRect.bottom());
            } else {
                QRectF rightHalf = QRectF(toRect.x()+toRect.width()/2, toRect.y(), toRect.width()/2, toRect.height());
                x = qBound(rightHalf.left(), c.x()+rightHalf.width(), rightHalf.right());
                y = qBound(toRect.top(), c.y()-upperHalf.height(), toRect.bottom());
            }
            g.moveCenter(QPointF(x, y));
            toGeometries << g;
        }
    }

    return toGeometries;
}

/*!
    \class HsRelativePositionWidgetPositioningOnOrientationChange
    \brief More advanced widget positioning algorithm.

    Calculates new center points of
    \a fromGeometries when moving from \a fromRect to \a toRect.
    It calculates widget's relative position on the screen and then calculates new position on the target rect:
    \verbatim
      w
    -------
    | A   |
    |(x,y)| h
    |     |
    |     |
    -------
    \endverbatim
    \verbatim
        w'
    |----------|
    |A'(x',y') | h'
    |----------|
    \endverbatim
    A(x,y) -> A'(x',y') where x'=x*w'/w and y'=y*h'/h and w=width of original rect and h=height of original rect and
    w'=width of target rect and h=height of target rect.
*/
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
QList<QRectF> HsRelativePositionWidgetPositioningOnOrientationChange::convert(
    const QRectF &fromRect,
    const QList<QRectF> &fromGeometries,
    const QRectF &toRect)
{
    qreal xTransformation = toRect.width()/fromRect.width();
    qreal yTransformation= toRect.height()/fromRect.height();
    QList<QRectF> toGeometries;
    foreach (QRectF g, fromGeometries) {
        QPointF c(g.center());
        qreal x = qBound(toRect.left(), c.x()*xTransformation, toRect.right());
        qreal y = qBound(toRect.top(), c.y()*yTransformation, toRect.bottom());
        g.moveCenter(QPointF(x, y));
        toGeometries << g;
    }
    return toGeometries;
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


/*!
    \class HsKeepCornerPositionWidgetPositioningOnOrientationChange
    \brief More advanced widget positioning algorithm.

    Calculates new center points of
    \a fromGeometries when moving from \a fromRect to \a toRect.
    It checks if left or right edge is closer to widget's center and keeps that distance same on the target rect.
    And same with upper and lower edges.
    \verbatim
      w
    -------
    | A   |
    |(x,y)| h
    |     |
    |     |
    -------
    \endverbatim
    \verbatim
        w'
    |----------|
    |A'(x',y') | h'
    |----------|
    \endverbatim
*/
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
QList<QRectF> HsKeepCornerPositionWidgetPositioningOnOrientationChange::convert(
    const QRectF &fromRect,
    const QList<QRectF> &fromGeometries,
    const QRectF &toRect)
{
    QList<QRectF> toGeometries;
    foreach (QRectF g, fromGeometries) {
        QPointF c(g.center());
        qreal x = 0;
        qreal y = 0;
        if ( c.x()-fromRect.x() < fromRect.right() - c.x() ) {
            x = qBound(toRect.left(), c.x(), toRect.right());
        }
        else {
            x = qBound(toRect.left(), toRect.right() - fromRect.right() + c.x(), toRect.right());
        }
        if ( c.y()-fromRect.y() < fromRect.bottom() - c.y() ) {
            y = qBound(toRect.top(), c.y(), toRect.bottom());
        }
        else {
            y = qBound(toRect.top(), toRect.bottom() - fromRect.bottom() + c.y(), toRect.bottom());
        }
        g.moveCenter(QPointF(x, y));
        toGeometries << g;
    }
    return toGeometries;
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
