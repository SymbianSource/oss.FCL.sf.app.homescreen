/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for digital clock.
*
*/

#ifndef SNSRDIGITALCLOCKCONTAINER_H
#define SNSRDIGITALCLOCKCONTAINER_H

#include "snsrbigclockcontainer.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverPlugin)

class SnsrLabel;
class SnsrSwipeWidget;
class HbPushButton;

class SnsrDigitalClockContainer : public SnsrBigClockContainer
{
    Q_OBJECT

public:

    SnsrDigitalClockContainer();
    ~SnsrDigitalClockContainer();

public slots:

    virtual void update();

public: // from base classes

    virtual int updateIntervalInMilliseconds();

protected: // from base classes

    virtual void loadWidgets();
    void loadWidgets(const char* docmlName);

private: // data

    SnsrLabel *mTimeLabel;
    SnsrLabel *mAmPmLabel;
    SnsrLabel *mDateLabel;
    HbPushButton * mUnlockButton;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverPlugin)
};

#endif // SNSRDIGITALCLOCKCONTAINER_H
