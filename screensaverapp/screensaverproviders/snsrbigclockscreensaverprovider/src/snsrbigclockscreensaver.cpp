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

#include "snsrbigclockscreensaver.h"

#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#endif // Q_OS_SYMBIAN

#include <QDebug>
#include <QTime>
#include <QTimer>

#include <hbinstance.h>
#include <hbmainwindow.h>

#include "snsranalogclockcontainer.h"
#include "snsrdigitalclockcontainer.h"
#include "snsroledanalogclockcontainer.h"
#include "snsroleddigitalclockcontainer.h"

/*!
    \class SnsrBigClockScreensaver
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver with big digital clock.
 */

/*!
    Constructs a new SnsrBigClockScreensaver.
 */
SnsrBigClockScreensaver::SnsrBigClockScreensaver() :
    mMainWindow(0),
    mCurrentContainer(0)
{
    mMainWindow = HbInstance::instance()->allMainWindows().at(0);
    // for nice looking clock hand transformations
    mMainWindow->setRenderHint(QPainter::SmoothPixmapTransform);
}

/*!
    Destructs the class.
 */
SnsrBigClockScreensaver::~SnsrBigClockScreensaver()
{
    // mCurrentContainer - deleted by the parent
}

/*!
    @copydoc Screensaver::onInitialize()
*/
bool SnsrBigClockScreensaver::onInitialize()
{
    qDebug() << "SnsrBigClockScreensaver::onInitialize()";
    return true;
}

/*!
    @copydoc Screensaver::onForeground()
 */
bool SnsrBigClockScreensaver::onForeground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onForeground")
    qDebug() << "SnsrBigClockScreensaver::onForeground()";

    bool ret(false);

    if (mCurrentContainer) {
        disconnect(
            mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
            );
        disconnect(
            &mTimer, SIGNAL(timeout()),
            mCurrentContainer, SLOT(update())
            );
        emit viewChanged(0);
        delete mCurrentContainer;
        mCurrentContainer = 0;
    }

    if (clockFormat() == ClockFormatAnalog) {
        mCurrentContainer = new SnsrAnalogClockContainer();
    }
    else {
        mCurrentContainer = new SnsrDigitalClockContainer();
    }
    connect(
            mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
        );
    connect(&mTimer, SIGNAL(timeout()), mCurrentContainer, SLOT(update()));

    mCurrentContainer->changeLayout(mMainWindow->orientation());
    mTimer.start(1000);
    emit viewChanged(mCurrentContainer);

    ret = true;

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onForeground")
    return ret;
}

/*!
    @copydoc Screensaver::onPartialForeground()
 */
bool SnsrBigClockScreensaver::onPartialForeground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onPartialForeground")
    qDebug() << "SnsrBigClockScreensaver::onPartialForeground()";

    bool ret(false);

    if (mCurrentContainer) {
        disconnect(
            mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
            );
        disconnect(
            &mTimer, SIGNAL(timeout()),
            mCurrentContainer, SLOT(update())
            );
        emit viewChanged(0);
        delete mCurrentContainer;
        mCurrentContainer = 0;
    }

    if (clockFormat() == ClockFormatAnalog) {
        mCurrentContainer = new SnsrOledAnalogClockContainer();
    }
    else {
        mCurrentContainer = new SnsrOledDigitalClockContainer();
    }
    connect(
        mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
        mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
        );
    connect(&mTimer, SIGNAL(timeout()), mCurrentContainer, SLOT(update()));

    mCurrentContainer->changeLayout(mMainWindow->orientation());
    mTimer.start(1000);
    emit viewChanged(mCurrentContainer);

    ret = true;

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onPartialForeground")
    return ret;
}

/*!
    @copydoc Screensaver::onBackground()
 */
bool SnsrBigClockScreensaver::onBackground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onBackground")
    qDebug() << "SnsrBigClockScreensaver::onBackground()";

    disconnect(
        mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
        mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
        );
    disconnect(
        &mTimer, SIGNAL(timeout()),
        mCurrentContainer, SLOT(update())
        );
    mTimer.stop();
    emit viewChanged(0);
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onBackground")
    return true;
}

/*!
    @copydoc Screensaver::onPowerSave()
 */
bool SnsrBigClockScreensaver::onPowerSave()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onPowerSave")
    qDebug() << "SnsrBigClockScreensaver::onPowerSave()";
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onPowerSave")
    return false;
}

/*!
    @copydoc Screensaver::onClose()
 */
bool SnsrBigClockScreensaver::onClose()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onClose")
    qDebug() << "SnsrBigClockScreensaver::onClose()";

    bool ret(false);
    if (onBackground()) {
        delete mCurrentContainer;
        mCurrentContainer = 0;
        ret = true;
    }
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onClose")
    return ret;
}

/*!
    Determines the curent clock format settings.
    \retval ClockFormat.
 */
SnsrBigClockScreensaver::ClockFormat SnsrBigClockScreensaver::clockFormat()
{
#ifdef Q_OS_SYMBIAN
    if (TLocale().ClockFormat() == EClockAnalog) {
        return ClockFormatAnalog;
    } else {
        return ClockFormatDigital;
    }
#else
    // windows build - change the format every 30 seconds for testing purposes
    if (QTime::currentTime().second() < 30) {
        return ClockFormatAnalog;
    } else {
        return ClockFormatDigital;
    }
#endif // Q_OS_SYMBIAN
}
