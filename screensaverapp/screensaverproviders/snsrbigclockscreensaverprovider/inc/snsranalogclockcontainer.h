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
* Description:  Container for analog clock.
*
*/

#ifndef SNSRANALOGCLOCKCONTAINER_H
#define SNSRANALOGCLOCKCONTAINER_H

#include "snsrbigclockcontainer.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverProvider)

class HbLabel;
class SnsrAnalogClockWidget;

class SnsrAnalogClockContainer : public SnsrBigClockContainer
{
    Q_OBJECT

public:

    SnsrAnalogClockContainer();
    ~SnsrAnalogClockContainer();

public slots:

    virtual void update();
    virtual void changeLayout(Qt::Orientation orientation);

private:
    
    HbLabel *mDateLabel;
    SnsrAnalogClockWidget *mAnalogClockWidget;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverProvider)
};

#endif // SNSRANALOGCLOCKCONTAINER_H
