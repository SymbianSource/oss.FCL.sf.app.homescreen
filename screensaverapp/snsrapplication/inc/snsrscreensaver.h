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
* Description:  Main Screensaver application class, loads runtime.
*
*/

#ifndef SNSRSCREENSAVER_H
#define SNSRSCREENSAVER_H

#include <QObject>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrScreensaver)

class HsRuntime;

class SnsrScreensaver : public QObject
{
    Q_OBJECT

public:

    SnsrScreensaver(QObject *parent = 0);
    virtual ~SnsrScreensaver();

signals:

    void exit();

public slots:

    void start();
    void stop();

private:

    Q_DISABLE_COPY (SnsrScreensaver)

private slots:

    void onRuntimeStarted();
    void onRuntimeStopped();
    void onRuntimeFaulted();

private:

    HsRuntime *mRuntime;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrScreensaver)

};

#endif // SNSRSCREENSAVER_H
