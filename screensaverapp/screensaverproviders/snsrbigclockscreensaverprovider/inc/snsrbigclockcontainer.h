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
* Description:  Base class container.
*
*/

#ifndef SNSRBIGCLOCKCONTAINER_H
#define SNSRBIGCLOCKCONTAINER_H

#include <hbwidget.h>

#include "snsrtest_global.h"
#include "snsrdocumentloader.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverProvider)

class QGraphicsLinearLayout;

class SnsrBigClockContainer : public HbWidget
{
    Q_OBJECT

public:

    SnsrBigClockContainer();
    ~SnsrBigClockContainer();

public slots:

    virtual void update() = 0;
    virtual void changeLayout(Qt::Orientation orientation) = 0;

protected:

    virtual void changeEvent(QEvent * event);

private:

    void setBackgroundColor(); 
    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = 0
            );

protected:

    QGraphicsWidget *mMainContainer;
    QGraphicsLinearLayout *mBackgroundContainerLayout;

    SnsrDocumentLoader mDocumentLoader;
    QObjectList mDocumentObjects;

    int mCurrentOrientation;

private:

    QColor mBackgroundColor;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverProvider)
};

#endif // SNSRBIGCLOCKCONTAINER_H
