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

#ifndef HELLOWORLDWIDGET_H
#define HELLOWORLDWIDGET_H

#include <HbWidget>

class QPushButton;

class HelloWorldWidget : public HbWidget
{
    Q_OBJECT

public:
    HelloWorldWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HelloWorldWidget();

public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    
public slots:
    void onShow();
    void onHide();

private:
    Q_DISABLE_COPY(HelloWorldWidget)
    QPushButton *mButton;
};

#endif // HELLOWORLDWIDGET_H
