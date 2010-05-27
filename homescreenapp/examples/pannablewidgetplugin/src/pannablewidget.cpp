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
* Description:  Example of home screen widget
*
*/
#include <qtDebug>

#include <QGraphicsLinearLayout>
#include <HbPushButton>
#include <HbSlider>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include "pannablewidget.h"



/*!
    \ingroup group_pannablewidget
    \class PannableWidget
    \brief Example implementation for pannable home screen widget.

    Pannable widget demostrates usage of horizontal pan gesture instead of Home Screen using it for
    page change.
    
    To enable horizontal pan gesture widget need to implement isPannable() function. Widget should return true if pannable item's
    pan region contains event position.  
*/


const int ItemName = 0xfffe;

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
PannableWidget::PannableWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags)
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    setLayout(layout);
    
    HbPushButton *button = new HbPushButton("Jenson Button");
    button->setPreferredSize(QSizeF(200.0,20.0));
    layout->addItem(button);

    HbSlider *slider = new HbSlider(Qt::Horizontal);
    slider->setData(ItemName,QLatin1String("slider"));
    slider->setRange(0,10);
    slider->setValue(5);
    layout->addItem(slider);

    
    QGraphicsRectItem *background = new QGraphicsRectItem();
    background->setBrush(QBrush(QColor(0x9acd32)));
    setBackgroundItem(background);
    
    resize(200.0, 100.0);
    background->setRect(rect());

}

/*!
    Destructor
*/
PannableWidget::~PannableWidget()
{
}
/*!
    Called by Home Screen when pan event is recognized. Given \a event
    can be used to check whether pannable item contains event position.
*/
bool PannableWidget::isPannable(QGraphicsSceneMouseEvent *event)
{
    bool ret(false);
    // Take position in scene coordinates
    QPointF position = event->buttonDownScenePos(Qt::LeftButton);
    // find pannable item
    QGraphicsItem* item = findChildItemByName(this,"slider");
    // check whether it is event is inside it
    if (item && item->sceneBoundingRect().contains(position)) {
        ret = true;
    }
    return ret;
}
/*!
    Find \a item childs by given \a name. Uses recursion.
*/
QGraphicsItem *PannableWidget::findChildItemByName(QGraphicsItem *item, const QString &name)
{
    if (!item) return NULL;

    QList<QGraphicsItem *> children = item->childItems();
    foreach (QGraphicsItem *child, children) {
        if(name == child->data(ItemName).toString()) {
            return child;
        }
    }
    // do recursive search
    foreach (QGraphicsItem *child, children) {
        QGraphicsItem* itemFromChild = findChildItemByName(child,name);
        if (itemFromChild) {
            return itemFromChild;
        }
    }
    return NULL;
}


/*!
    Called when widget is shown in the home screen
*/
void PannableWidget::onShow()
{
}

/*!
    Called when widget is hidden from the home screen
*/
void PannableWidget::onHide()
{
}
