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

#include <QGraphicsProxyWidget>
#include <QPushButton>
#include "helloworldwidget.h"

/*!
    \ingroup group_helloworld_widget
    \class HelloWorldWidget
    \brief Example implementation for home screen widget.

    HelloWorldWidget derived from the HbWidget and implements 
    needed functions for the home screen widget. 
*/

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
HelloWorldWidget::HelloWorldWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mButton(0)
{
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    mButton = new QPushButton("Hello World");
    proxy->setWidget(mButton);        
}

/*!
    Destructor
*/
HelloWorldWidget::~HelloWorldWidget()
{

}

/*!
    Return bounding rect
*/
QRectF HelloWorldWidget::boundingRect() const
{
    return childrenBoundingRect();
}

/*!
    Return shape
*/
QPainterPath HelloWorldWidget::shape() const
{   
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
    Called when widget is shown in the home screen
*/
void HelloWorldWidget::onShow()
{
}

/*!
    Called when widget is hidden from the home screen
*/
void HelloWorldWidget::onHide()
{
}
