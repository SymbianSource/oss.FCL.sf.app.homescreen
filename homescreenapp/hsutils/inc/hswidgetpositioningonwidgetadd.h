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

#ifndef HSWIDGETPOSITIONINGONWIDGETADD_H
#define HSWIDGETPOSITIONINGONWIDGETADD_H

#include <QList>
#include <QRectF>
#include <QPointF>
#include "hsutils_global.h"

class HsWidgetHost;

class HSUTILS_EXPORT HsWidgetPositioningOnWidgetAdd
{
public:
    virtual ~HsWidgetPositioningOnWidgetAdd() {}
    
    virtual QList<QRectF> convert(const QRectF &contentArea,
                                  const QList<QRectF> &existingRects,
                                  const QList<QRectF> &newRects,
                                  const QPointF &startPoint) = 0;
   
    static void setInstance(HsWidgetPositioningOnWidgetAdd *instance);
    static HsWidgetPositioningOnWidgetAdd *instance();

private:
    static HsWidgetPositioningOnWidgetAdd *mInstance;
};

class HSUTILS_EXPORT HsAnchorPointInBottomRight : public HsWidgetPositioningOnWidgetAdd
{
public:
    QList<QRectF> convert(const QRectF &contentArea,
                          const QList<QRectF> &existingRects,
                          const QList<QRectF> &newRects,
                          const QPointF &startPoint);
 
};

class HSUTILS_EXPORT HsAnchorPointInCenter : public HsWidgetPositioningOnWidgetAdd
{
public:
    QList<QRectF> convert(const QRectF &contentArea,
                          const QList<QRectF> &existingRects,
                          const QList<QRectF> &newRects,
                          const QPointF &startPoint);
 };

class HSUTILS_EXPORT HsWidgetOrganizer : public HsWidgetPositioningOnWidgetAdd
{
public:
    QList<QRectF> convert(const QRectF &contentArea,
                          const QList<QRectF> &existingRects,
                          const QList<QRectF> &newRects,
                          const QPointF &startPoint);

private:
    bool initAnchors(const QSizeF &areaSize);
    bool getAnchorPoint(const QSizeF &contentSize);
    bool searchWidthSpace(const QSizeF &contentSize);
    bool searchHeightSpace(int contentHeight);
    bool markReservedAnchors();
    QPointF getAnchorCoordinates(int index);
    int getIndexForCoordinate(QPointF position);
    int lenghtInAnchorPoints(QVariant length);

private:
    int mAnchorColumns;
    int mAnchorRows;
    int mAnchorDistance;
    QPointF mStartWidthAnchorPoint;
    QPointF mEndWidthAnchorPoint;
    QPointF mEndHeightAnchorPoint;
    // TODO: is there better way to store anchor points, perhaps with pointers?
    QList<bool> mAnchors;

};


#endif // HSWIDGETPOSITIONINGONWIDGETADD_H
