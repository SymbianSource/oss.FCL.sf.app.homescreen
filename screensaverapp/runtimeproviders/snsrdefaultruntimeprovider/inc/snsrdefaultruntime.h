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
* Description: Default Screensaver runtime.
*
*/

#ifndef SNSRDEFAULTRUNTIME_H
#define SNSRDEFAULTRUNTIME_H

#include <hsruntime.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrDefaultRuntimeProvider)

class QStateMachine;
class HbMainWindow;

class SnsrDefaultRuntime : public HsRuntime
{

    Q_OBJECT

public:

    SnsrDefaultRuntime(QObject *parent = 0);
    ~SnsrDefaultRuntime();

public slots:

    void start();
    void stop();

private:

    Q_DISABLE_COPY(SnsrDefaultRuntime)

    void initializeUserInterface();
    void initializeStateMachine();

    void createStates();

private:

    QStateMachine *mStateMachine;
    HbMainWindow *mWindow;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrDefaultRuntimeProvider)

};

#endif // SNSRDEFAULTRUNTIME_H
