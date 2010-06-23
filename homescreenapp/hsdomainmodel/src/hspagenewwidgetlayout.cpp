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

#include <QPainter>
#include <QRectF>
#include <QGraphicsLinearLayout>
#include <HbInstance>
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsAnchorLayout>

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
HsPageNewWidgetLayout::HsPageNewWidgetLayout(QGraphicsLayoutItem *parent) :
    QGraphicsLayout(parent)
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
   HsWidgetPositioningOnWidgetAdd *algorithm =
        HsWidgetPositioningOnWidgetAdd::instance();
    QRectF pageRect = HsScene::mainWindow()->layoutRect();
    pageRect.adjust( (qreal)0,(qreal)64,(qreal)0,(qreal)0);
    QList<QRectF> calculatedRects =
        algorithm->convert(pageRect, rects, QPointF());

    for ( int i=0; i<mNewWidgets.count(); i++) {
        mNewWidgets.at(i)->setGeometry(calculatedRects.at(i));
    }
}

/*!
    Adds item to layout.
*/
void HsPageNewWidgetLayout::addItem(HsWidgetHost *item)
{
    mNewWidgets.append(item);
}
