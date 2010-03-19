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
* Description: snsruseractivityservice_p.cpp
*
*/

#include "snsruseractivityservice.h"
#include "snsruseractivityservice_p.h"

#include <QTimer>
#include <QObject>

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    \param servicePublic public implementation.
*/
SnsrUserActivityServicePrivate::SnsrUserActivityServicePrivate(SnsrUserActivityService *servicePublic) :
    m_q(servicePublic), mInactivityPeriod(9)
{
    mActivityTimer = new QTimer();
    QObject::connect(mActivityTimer, SIGNAL(timeout()), m_q, SIGNAL(active()));

    mInactivityTimer = new QTimer();
    QObject::connect(mInactivityTimer, SIGNAL(timeout()), m_q, SIGNAL(notActive()));
}

/*!
    Destructor.
*/
SnsrUserActivityServicePrivate::~SnsrUserActivityServicePrivate()
{
    delete mActivityTimer;
    delete mInactivityTimer;
}

/*!
    Sets the inactivity period after which to emit signal of inactivity.
    \param seconds after which inactivity is detected.
*/
void SnsrUserActivityServicePrivate::setInactivityPeriod(int seconds)
{
    mInactivityPeriod = seconds;
    if (isWatching()) {
        mInactivityTimer->start(seconds * 1000);
    }
}

/*!
    Retrives the current inactivity period setting.
    \retval inactivity period set.
*/
int SnsrUserActivityServicePrivate::inactivityPeriod() const
{
    return mInactivityPeriod;
}

/*!
    Starts or stops activity manager user activity watching.
    \param shouldWatch determines if we shoul start watching or stop watching.
*/
void SnsrUserActivityServicePrivate::watch(bool shouldWatch)
{
    if (shouldWatch && !isWatching()) {
        mActivityTimer->start(5000);
        mInactivityTimer->start(mInactivityPeriod * 1000);
    } else if (!shouldWatch && isWatching()) {
        mActivityTimer->stop();
        mInactivityTimer->stop();
    }
}

/*!
    Checks if activity service is currently watching for user activity.
    \retval true if user acitivity service is active.
*/
bool SnsrUserActivityServicePrivate::isWatching() const
{
    return (mActivityTimer->isActive() || mInactivityTimer->isActive());
}


