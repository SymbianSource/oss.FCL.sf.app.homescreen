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

#include <QGraphicsLinearLayout>
#include <HbLabel>
#include <HbPushButton> 
#include <QDir>
#include <QPainter>
#include <HbIconItem>
#include "localisedhellowidget.h"

/*!
    \ingroup group_helloworld_widget
    \class LocalisedHelloWidget
    \brief Example implementation for home screen widget.

    LocalisedHelloWidget derived from the HbWidget and implements 
    needed functions for the home screen widget. 
*/

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
LocalisedHelloWidget::LocalisedHelloWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),mIcon(0)
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    setPreferredSize(200,100);
    setLayout(layout);
    mButton = new HbPushButton();
    layout->addItem(mButton);
   
    connect(mButton,SIGNAL(pressed()),SLOT(onButtonPressed()));

    
   
}

/*!
    Destructor
*/
LocalisedHelloWidget::~LocalisedHelloWidget()
{

}

/*!
    Return bounding rect
*/
QRectF LocalisedHelloWidget::boundingRect() const
{
    return childrenBoundingRect();
}

/*!
    Return shape
*/
QPainterPath LocalisedHelloWidget::shape() const
{   
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}
QString LocalisedHelloWidget::rootPath()const
{
    return mRootPath;
}

void LocalisedHelloWidget::setRootPath(const QString &rootPath)
{
    mRootPath = rootPath;
}

void LocalisedHelloWidget::onInitialize()
{
    // localisation can be used now
    mText = hbTrId("localisedhellowidgetplugin_txt_helloview_info_hello");
    mButton->setText(hbTrId("localisedhellowidgetplugin_txt_helloview_title_hello_world"));
    // roothPath is set thus we can use it
    QString iconPath = QDir::toNativeSeparators(mRootPath + "/localisedhellowidgetplugin.png"); 
    mIcon = new HbIconItem(iconPath);
    static_cast<QGraphicsLinearLayout*>(layout())->addItem(mIcon);
   
  
}

/*!
    Called when widget is shown in the home screen
*/
void LocalisedHelloWidget::onShow()
{
}

/*!
    Called when widget is hidden from the home screen
*/
void LocalisedHelloWidget::onHide()
{
}

void LocalisedHelloWidget::onButtonPressed()
{
    QString tmp = mButton->text();
    mButton->setText(mText);
    mText = tmp;
}

