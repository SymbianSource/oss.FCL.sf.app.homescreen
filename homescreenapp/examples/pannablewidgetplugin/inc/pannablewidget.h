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

#ifndef PANNABLEWIDGET_H
#define PANNABLEWIDGET_H

#include <HbWidget>

class PannableWidget : public HbWidget
{
    Q_OBJECT

public:
    PannableWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~PannableWidget();

    Q_INVOKABLE bool isPannable(QGraphicsSceneMouseEvent *event);
public slots:
    void onShow();
    void onHide();
private:
    QGraphicsItem *findChildItemByName(QGraphicsItem *item, const QString &name);
private:
    Q_DISABLE_COPY(PannableWidget)
};

#endif // PANNABLEWIDGET_H
