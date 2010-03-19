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
* Description:  Container for oled digital clock.
*
*/

#ifndef SNSROLEDDIGITALCLOCKCONTAINER_H
#define SNSROLEDDIGITALCLOCKCONTAINER_H

#include "snsrbigclockcontainer.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverProvider)

class HbLabel;

class SnsrOledDigitalClockContainer : public SnsrBigClockContainer
{
    Q_OBJECT

public:

    SnsrOledDigitalClockContainer();
    ~SnsrOledDigitalClockContainer();

public slots:

    virtual void update();
    virtual void changeLayout(Qt::Orientation orientation);

private:

    HbLabel *mTimeLabel;
    HbLabel *mAmPmLabel;
    HbLabel *mDateLabel;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverProvider)
};

#endif // SNSROLEDDIGITALCLOCKCONTAINER_H
