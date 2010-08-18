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

#include "hsdomainmodeldatastructures.h"
#include "hspagenewwidgetlayout.h"
#include "hsscene.h"
#include "hspage.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsconfiguration.h"
#include "hsgui.h"


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
    mSize = HsGui::instance()->layoutRect().size();
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
    return mNewWidgets.at(i)->visual();
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

#ifdef HSWIDGETORGANIZER_ALGORITHM
    // sort new widgets in order
    if (mNewWidgets.count() > 1) {
        // TODO: read from configuration? or just use height for portrait and width for landscape (currently only height is used)
        sortOrder order(height);
        mNewWidgets = sortWidgets(order);
    }
#endif

    // get rects for new widgets
    QList<QRectF> newRects;
    foreach (HsWidgetHost *newWidget, mNewWidgets) {
        newRects << QRectF(QPointF(), newWidget->visual()->preferredSize());
    }

    /* if there is touch point defined (widget added from context menu)
       then there is only one widget in the list
       -> set widget center point to this touch point
    */
    if (mTouchPoint != QPointF() && mNewWidgets.count() == 1) {
        QRectF widgetRect = newRects.at(0);
        widgetRect.moveCenter(mTouchPoint);
        widgetRect.moveTopLeft(HsScene::instance()->activePage()->adjustedWidgetPosition(widgetRect));
        mNewWidgets.at(0)->visual()->setGeometry(widgetRect);
        /* we have to save widget presentation data here after drawing
           to get correct position for later use
        */
        mNewWidgets.at(0)->savePresentation();
    }
    // otherwise calculate widget positions with algorithm
    else {
        // get page rect
        QRectF pageRect = HsScene::instance()->activePage()->contentGeometry();

        // scan existing widgets rects
        QList<HsWidgetHost*> existingWidgets;
        existingWidgets = HsScene::instance()->activePage()->widgets();
        QList<QRectF> existingRects;
        foreach (HsWidgetHost *widget, existingWidgets) {
            if (!mNewWidgets.contains(widget)) {
                existingRects << QRectF(widget->visual()->pos(), widget->visual()->preferredSize());
            }
        }
         
        // calculate new widget positions with "stuck 'em all"-algorithm
        HsWidgetPositioningOnWidgetAdd *algorithm =
            HsWidgetPositioningOnWidgetAdd::instance();
        QList<QRectF> calculatedRects =
            algorithm->convert(pageRect, existingRects, newRects, QPointF());

        for ( int i=0; i<mNewWidgets.count(); i++) {
            mNewWidgets.at(i)->visual()->setGeometry(calculatedRects.at(i));
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

#ifdef HSWIDGETORGANIZER_ALGORITHM
// TODO: sorting should be done in algorithm class, make widget<->rect mapping here and move sortWidgets function to algorithm side
/*!
    Sorts widgets in height/width order
*/
QList<HsWidgetHost*> HsPageNewWidgetLayout::sortWidgets(sortOrder order)
{
    QList<HsWidgetHost*> tmpWidgets;

    for ( int i = 0; i < mNewWidgets.count(); i++) {
        int index = 0;
        // add first widget to sorted list
        if (i == 0) {
            tmpWidgets << mNewWidgets.at(i);
        } else {
            // go through existing widgets in the sorted list
            for ( int j = 0; j < tmpWidgets.count(); j++) {
                // sort widgets in height order
                if (order == height) {
                    /* if widgets heigth is smaller on already
                       existing ones in the list -> increment index
                    */
                    if (mNewWidgets.at(i)->visual()->preferredHeight() <= tmpWidgets.at(j)->visual()->preferredHeight()) {
                        index++;
                    }
                // sort widgets in width order
                } else {
                    /* if widgets width is smaller on already
                       existing ones in the sorted list -> increment index
                    */
                    if (mNewWidgets.at(i)->visual()->preferredWidth() <= tmpWidgets.at(j)->visual()->preferredWidth()) {
                        index++;
                    }
                }
            }
            // add widget to its correct index in sorted list
            tmpWidgets.insert(index, mNewWidgets.at(i));
        }
    }
    return tmpWidgets;
}
#endif // HSWIDGETORGANIZER_ALGORITHM

