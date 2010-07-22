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
#include "snsrindicatormodel.h"

/*!
    \class SnsrBigClockScreensaver
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Screensaver with big digital clock.
 */

const int gTimeInterval(100);

/*!
    Constructs a new SnsrBigClockScreensaver.
 */
SnsrBigClockScreensaver::SnsrBigClockScreensaver() :
    mMainWindow(0),
    mCurrentContainer(0),
    mIndicatorModel(0)
{
    mMainWindow = HbInstance::instance()->allMainWindows().at(0);
    // for nice looking clock hand transformations
    mMainWindow->setRenderHint(QPainter::SmoothPixmapTransform);
    
    // This model holds indicator status information and must exist as
    // long as screensaver does.
    mIndicatorModel = new SnsrIndicatorModel(this);
}

/*!
    Destructs the class.
 */
SnsrBigClockScreensaver::~SnsrBigClockScreensaver()
{
    // mCurrentContainer, mIndicatorModel - deleted by the parent
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

    removeCurrentContainer();

    SnsrBigClockContainer* newContainer( 0 );
    if (clockFormat() == ClockFormatAnalog) {
        newContainer = new SnsrAnalogClockContainer();
    }
    else {
        newContainer = new SnsrDigitalClockContainer();
    }

    setCurrentContainer( newContainer );

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onForeground")
    return true;
}

/*!
    @copydoc Screensaver::onPartialForeground()
 */
bool SnsrBigClockScreensaver::onPartialForeground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onPartialForeground")
    qDebug() << "SnsrBigClockScreensaver::onPartialForeground()";

    removeCurrentContainer();
    
    SnsrBigClockContainer* newContainer( 0 );
    if (clockFormat() == ClockFormatAnalog) {
        newContainer = new SnsrOledAnalogClockContainer();
    }
    else {
        newContainer = new SnsrOledDigitalClockContainer();
    }

    setCurrentContainer( newContainer );

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onPartialForeground")
    return true;
}

/*!
    @copydoc Screensaver::onBackground()
 */
bool SnsrBigClockScreensaver::onBackground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onBackground")
    qDebug() << "SnsrBigClockScreensaver::onBackground()";

    removeCurrentContainer();

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
    @copydoc Screensaver::onHandleActiveIndicators
 */
void SnsrBigClockScreensaver::onHandleActiveIndicators(
        const QList<HbIndicatorInterface*> &activeIndicators)
{
    mIndicatorModel->handleActiveIndicators(activeIndicators);
}

/*!
    @copydoc Screensaver::onHandleActivatedIndicator
 */
void SnsrBigClockScreensaver::onHandleActivatedIndicator(
        HbIndicatorInterface *activatedIndicator)
{
    mIndicatorModel->handleActivatedIndicator(activatedIndicator);
}

/*!
    @copydoc Screensaver::onHandleDeactivatedIndicator
 */
void SnsrBigClockScreensaver::onHandleDeactivatedIndicator(
        HbIndicatorInterface *deactivatedIndicator)
{
    mIndicatorModel->handleDeactivatedIndicator(deactivatedIndicator);
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

void SnsrBigClockScreensaver::removeCurrentContainer()
{
    if ( mCurrentContainer ) {
        disconnect(
            mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
            );
        disconnect(
            &mTimer, SIGNAL(timeout()),
            mCurrentContainer, SLOT(update())
            );
        disconnect( 
            mCurrentContainer, SIGNAL(unlockRequested()), 
            this, SIGNAL(unlockRequested()) );
        mTimer.stop();
        emit viewChanged(0);
        
        delete mCurrentContainer;
        mCurrentContainer = 0;
    }
}

void SnsrBigClockScreensaver::setCurrentContainer( SnsrBigClockContainer* newContainer )
{
    mCurrentContainer = newContainer;
    connect(
            mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            mCurrentContainer, SLOT(changeLayout(Qt::Orientation))
        );
    connect( &mTimer, SIGNAL(timeout()), mCurrentContainer, SLOT(update()) );
    connect( mCurrentContainer, SIGNAL(unlockRequested()), SIGNAL(unlockRequested()) );

    mCurrentContainer->initIndicators(*mIndicatorModel);
    
    mCurrentContainer->changeLayout(mMainWindow->orientation());
    mTimer.start( mCurrentContainer->updateIntervalInMilliseconds() );
    emit viewChanged(mCurrentContainer);
}

