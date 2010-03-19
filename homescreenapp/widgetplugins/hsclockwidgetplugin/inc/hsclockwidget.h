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
* Description:  Clock widget
*
*/

#ifndef HSCLOCKWIDGET_H
#define HSCLOCKWIDGET_H

#include <QGraphicsSvgItem>
#include <QTime>
#include <QWidget>
#include <QGraphicsWidget>
#include <QImage>
#include <hbwidget.h>
#include <hstest_global.h>
#include "hsanalogclockstyleoption.h"

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HbWidget;
class QGraphicsLinearLayout;

class HsClockWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(QString clockType READ clockType WRITE setClockType)

public:

    HsClockWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HsClockWidget();

    QString clockType() const;
    void setClockType(const QString &type);

public slots:

    void onInitialize();
    void onShow();
    void onHide();
    void onUninitialize();

    void updateTime();
    void onTypeChanged(QString type);
    
signals:
    void setPreferences(const QStringList &names);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event) }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *aEvent);

private:
    void toggleClockType();
    HbWidget *loadClockWidget();

private:

    QTimer *mTimer;
    QString mClockType;
    HbWidget *mWidget;
    QGraphicsLinearLayout *mLayout;
    bool mWidgetShown;

    Q_DISABLE_COPY(HsClockWidget)

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)

};




#endif
