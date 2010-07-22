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
* Description:  Swipe widget.
*
*/

#ifndef SNSRSWIPEWIDGET_H
#define SNSRSWIPEWIDGET_H

#include <QGesture>

#include <hbwidget.h>
#include <hblabel.h>
#include <hbeffect.h>
#include "snsrtest_global.h"

// TODO: SCREENSAVER_TEST_CLASS
SCREENSAVER_TEST_CLASS(T_SnsrSwipeWidgets)

class HbAction;
class HbPushButton;
class HbLabel;

class SnsrSwipeWidget : public HbWidget
    {
    Q_OBJECT

public:
    
    explicit SnsrSwipeWidget(QGraphicsItem *parent = 0);
    virtual ~SnsrSwipeWidget();

public slots:

    void start();

protected:

    void reset();

private slots:

    void animationComplete(const HbEffect::EffectStatus &status);
    void createPrimitives();

protected: // from base classes

    virtual void gestureEvent(QGestureEvent *event);

signals:
    void swipeDownDetected();

private: // data

    HbLabel *mSlideLabel;
    HbLabel *mSlideIcon;

    Q_DISABLE_COPY(SnsrSwipeWidget)

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrSwipeWidgets)
    
};

#endif // SNSRSWIPEWIDGET_H

// EOF
