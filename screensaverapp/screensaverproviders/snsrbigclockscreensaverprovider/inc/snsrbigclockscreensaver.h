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
* Description: Big clock Screensaver.
*
*/

#ifndef SNSRBIGCLOCKSCREENSAVER_H
#define SNSRBIGCLOCKSCREENSAVER_H

#include <QTimer>

#include <screensaver.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverProvider)

class HbMainWindow;
class SnsrBigClockContainer;

class SnsrBigClockScreensaver : public Screensaver
{
    Q_OBJECT

public:

    enum ClockFormat
    {
        ClockFormatUndefined,
        ClockFormatAnalog,
        ClockFormatDigital
    };

    SnsrBigClockScreensaver();
    ~SnsrBigClockScreensaver();

private:

    ClockFormat clockFormat();

    virtual bool onInitialize();
    virtual bool onForeground();
    virtual bool onPartialForeground();
    virtual bool onBackground();
    virtual bool onPowerSave();
    virtual bool onClose();

private:

    QTimer mTimer;
    HbMainWindow *mMainWindow;
    SnsrBigClockContainer *mCurrentContainer;
    
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverProvider)

};

#endif // SNSRBIGCLOCKSCREENSAVER_H
