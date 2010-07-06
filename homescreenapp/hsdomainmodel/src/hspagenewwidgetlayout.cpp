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

#include <HbInstance>

#include "hsdomainmodeldatastructures.h"
#include "hspagenewwidgetlayout.h"
#include "hsscene.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsconfiguration.h"


/*!
    \class HsPageNewWidgetLayout
    \ingroup group_hsdomainmodel
    \brief Represents a page in the framework.
    HsPage is a parent for a group of widgets. HsPage can have a wallpaper.
*/

/*!
    Constructor.

    \a parent Owner.
*/
HsPageNewWidgetLayout::HsPageNewWidgetLayout(const QPointF &touchPoint,
                                             QGraphicsLayoutItem *parent)
    : QGraphicsLayout(parent),    
    mTouchPoint(touchPoint)
{
    mSize = HsScene::mainWindow()->layoutRect().size();
}

/*!
    Destructor.
*/
HsPageNewWidgetLayout::~HsPageNewWidgetLayout()
{
}

/*!
    Returns children count.
*/
int HsPageNewWidgetLayout::count() const
{
    return mNewWidgets.count();
}

/*!
    Returns item index of \a i.
*/
QGraphicsLayoutItem *HsPageNewWidgetLayout::itemAt(int i) const
{
    return mNewWidgets.at(i);
}

/*!
    Removes item \a index.
*/
void HsPageNewWidgetLayout::removeAt(int index)
{
    mNewWidgets.removeAt(index);
}

/*!
    Size hint.
*/
QSizeF HsPageNewWidgetLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
     switch (which) {
     case Qt::MinimumSize:
     case Qt::PreferredSize:
         return mSize;
     case Qt::MaximumSize:
         return mSize;
     default:
         break;
     }
     return constraint;
}

/*!
    Recalculates children positions.
*/
void HsPageNewWidgetLayout::setGeometry(const QRectF &rect)
{
    QGraphicsLayout::setGeometry(rect);
   
    QList<QRectF> rects;
    foreach (HsWidgetHost *newWidget, mNewWidgets) {
        rects << QRectF(QPointF(), newWidget->preferredSize());
    }

    /* if there is touch point defined (widget added from context menu)
       then there is only one widget in the list
       -> set widget center point to this touch point
    */
    if (mTouchPoint != QPointF() && mNewWidgets.count() == 1) {
        QRectF pageRect = HsScene::mainWindow()->layoutRect();
        qreal widgetX = qBound(qreal(0), mTouchPoint.x() - rects.at(0).width() / 2, pageRect.width() - rects.at(0).width());
        qreal widgetY = qBound(qreal(64), mTouchPoint.y() - rects.at(0).height() / 2, pageRect.height() - rects.at(0).height());
        mNewWidgets.at(0)->setGeometry(widgetX,
                                       widgetY,
                                       rects.at(0).width(), 
                                       rects.at(0).height());
        /* we have to save widget presentation data here after drawing
           to get correct position for later use
        */
        mNewWidgets.at(0)->savePresentation();
    }
    // otherwise calculate position with algorithm
    else {
        HsWidgetPositioningOnWidgetAdd *algorithm =
            HsWidgetPositioningOnWidgetAdd::instance();
        QRectF pageRect = HsScene::mainWindow()->layoutRect();
        pageRect.adjust( (qreal)0,(qreal)64,(qreal)0,(qreal)0);
        QList<QRectF> calculatedRects =
            algorithm->convert(pageRect, rects, QPointF());

        for ( int i=0; i<mNewWidgets.count(); i++) {
            mNewWidgets.at(i)->setGeometry(calculatedRects.at(i));
            mNewWidgets.at(i)->savePresentation();
        }
    }
}

/*!
    Adds item to layout.
*/
void HsPageNewWidgetLayout::addItem(HsWidgetHost *item)
{
    mNewWidgets.append(item);
}
