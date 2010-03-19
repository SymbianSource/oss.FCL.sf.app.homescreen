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

#ifndef SNSRBIGCLOCKSCREENSAVERPROVIDER_H
#define SNSRBIGCLOCKSCREENSAVERPROVIDER_H

#include <iscreensaverprovider.h>
#include <screensaver.h>

class SnsrBigClockScreensaverProvider : public QObject, public IScreensaverProvider
{
    Q_OBJECT
    Q_INTERFACES(IScreensaverProvider)

public:

    SnsrBigClockScreensaverProvider();
    virtual ~SnsrBigClockScreensaverProvider();

    QList<ScreensaverToken> screensavers();
    Screensaver *createScreensaver(const ScreensaverToken& token);

private:

    ScreensaverToken mClockScreensaverToken;

};

#endif // SNSRBIGCLOCKSCREENSAVERPROVIDER_H
