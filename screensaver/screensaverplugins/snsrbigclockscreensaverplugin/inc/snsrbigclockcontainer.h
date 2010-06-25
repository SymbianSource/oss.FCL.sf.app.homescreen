/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class container.
*
*/

#ifndef SNSRBIGCLOCKCONTAINER_H
#define SNSRBIGCLOCKCONTAINER_H

#include <hbwidget.h>

#include "snsrtest_global.h"
#include "snsrdocumentloader.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverPlugin)

class QGraphicsLinearLayout;
class HbIndicatorInterface;
class SnsrIndicatorModel;
class SnsrIndicatorWidget;

class SnsrBigClockContainer : public HbWidget
{
    Q_OBJECT

public:

    SnsrBigClockContainer();
    ~SnsrBigClockContainer();

public slots:

    virtual void update() = 0;
    virtual void changeLayout(Qt::Orientation orientation) = 0;
    
public:
    
    virtual int updateIntervalInMilliseconds() = 0;
    void initIndicators(SnsrIndicatorModel &model);

protected:

    virtual void changeEvent(QEvent * event);
    QPointF randomPosition(const QRectF &rect);
    QPointF nextRandomPosition(const QPointF &curPos, QPointF &destPos, const QRectF &containerRect);

private:

    void resetIndicatorConnections();
    void setBackgroundColor(); 
    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = 0
            );

signals:

    void unlockRequested();

protected:

    SnsrDocumentLoader mDocumentLoader;
    QObjectList mDocumentObjects;

    QGraphicsLinearLayout *mBackgroundContainerLayout;
    QGraphicsWidget *mMainView;
    QGraphicsWidget *mMainContainer;
    
    SnsrIndicatorModel *mIndicatorModel; // not owned
    SnsrIndicatorWidget *mIndicatorWidget;

    int mCurrentOrientation;

private:

    QColor mBackgroundColor;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverPlugin)
};

#endif // SNSRBIGCLOCKCONTAINER_H
