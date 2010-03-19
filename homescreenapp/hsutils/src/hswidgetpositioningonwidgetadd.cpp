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
#include "hswidgetpositioningonwidgetadd.h"
#include <math.h>
#include <QLineF>

const qreal offset = 40; //TODO: Implement this as configurable parameter

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
    const QList<QRectF> &widgets)
{
    QList<QRectF> toGeometries;

    //Offset for widgets' bottom right position to each other
    qreal k = contentArea.height()/contentArea.width(); //slope of the diagonal
    qreal offset_x = offset/(sqrt(k + 1));
    qreal offset_y = k*offset_x;
    QPointF offsetPoint(offset_x, offset_y);
    
    QLineF diagonal(contentArea.topLeft(), contentArea.bottomRight());
    QLineF widgetRight(contentArea.center().x()+ widgets.at(0).width()/2,
                       contentArea.top(),
                       contentArea.center().x()+ widgets.at(0).width()/2,
                       contentArea.bottom());

    QPointF anchorPoint;
    if(QLineF::BoundedIntersection != diagonal.intersect(widgetRight, &anchorPoint)) {
        return widgets; //Return original since undefined error.
                        //In this case widget's must be wider than the content area.
    }

    //First widget to the center of the content area
    foreach (QRectF g, widgets) {
        g.moveBottomRight(anchorPoint);
        toGeometries << g;
        anchorPoint -= offsetPoint;
        if(!contentArea.contains(anchorPoint)) {
            anchorPoint = contentArea.bottomRight();
        }
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
    const QList<QRectF> &widgets)
{
    QList<QRectF> toGeometries;

    //Offset for widgets' centers position to each other
    qreal k = contentArea.height()/contentArea.width(); //slope of the diagonal
    qreal offset_x = offset/(sqrt(k + 1));
    qreal offset_y = k*offset_x;
    QPointF offsetPoint(offset_x, offset_y);

    //First widget to the center of the content area
    QPointF anchorPoint = contentArea.center();
    foreach (QRectF g, widgets) {
        g.moveCenter(anchorPoint);
        toGeometries << g;
        anchorPoint -= offsetPoint;
        if(!contentArea.contains(anchorPoint)) {
            anchorPoint = contentArea.bottomRight();
        }
    }
    return toGeometries;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

