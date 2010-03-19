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
* Description: Big clock Screensaver provider.
*
*/

#include "snsrbigclockscreensaverprovider.h"

#include "snsrbigclockscreensaver.h"

/*!
    \class SnsrBigClockScreensaverProvider
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver big clock screensaver provider collects ready-made screensaver clock.
 */

/*!
    Constructs a new SnsrBigClockScreensaverProvider.
 */
SnsrBigClockScreensaverProvider::SnsrBigClockScreensaverProvider()
{
    mClockScreensaverToken.mLibrary = 
        QString("snsrbigclockscreensaverprovider.dll");
    mClockScreensaverToken.mUri = 
        QString("screensaver.nokia.com/screensaver/bigclock");
}

/*!
    Destructs the class.
 */
SnsrBigClockScreensaverProvider::~SnsrBigClockScreensaverProvider()
{
}

/*!
    Returns list of loadable screensavers' tokens.
    \return ScreensaverToken info about loadable screensavers.
 */
QList<ScreensaverToken> SnsrBigClockScreensaverProvider::screensavers()
{
    return QList<ScreensaverToken>() << mClockScreensaverToken;
}

/*!
    Creates a screensaver based to given token.
    \param token Reference to ScreensaverToken.
 */
Screensaver *SnsrBigClockScreensaverProvider::
                createScreensaver(const ScreensaverToken &token)
{
    if (token.mLibrary == mClockScreensaverToken.mLibrary &&
        token.mUri == mClockScreensaverToken.mUri) {
        return new SnsrBigClockScreensaver();
    }
    return 0;
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(snsrbigclockscreensaverprovider, SnsrBigClockScreensaverProvider)
#endif //COVERAGE_MEASUREMENT
