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
* Description: snsruseractivityservice_p.h
*
*/

#ifndef SNSRUSERACTIVITYSERVICEPRIVATE_H
#define SNSRUSERACTIVITYSERVICEPRIVATE_H

#include "snsrtest_global.h"

class QTimer;

class SnsrUserActivityService;

class SnsrUserActivityServicePrivate
{

public:

    explicit SnsrUserActivityServicePrivate(SnsrUserActivityService *servicePublic);
    ~SnsrUserActivityServicePrivate();

    void setInactivityPeriod(int seconds);
    int inactivityPeriod() const;

    void watch(bool shouldWatch);

private:

    bool isWatching() const;

private:

    SnsrUserActivityService *m_q;

    QTimer *mActivityTimer;
    QTimer *mInactivityTimer;

    int mInactivityPeriod;
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrUserActivityService)
};

#endif // SNSRUSERACTIVITYSERVICEPRIVATE_H
