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

#include <activitymanager.h>

const int lDefaultInactivityPeriod(5);

// ======== LOCAL FUNCTIONS ========

/*!
    Called as callback to activity event.
    \param pointer to activity callback object.
    \retval error code.    
*/
TInt activityCallback(TAny *ptr)
{
    static_cast<SnsrUserActivityServicePrivate *>(ptr)->emitActive();
    // activity manager doesn't care about the return value,
    // we return a value indicating 'true' to be elegant (see CIdle)
    return 1;
}

/*!
    Called as callback to inactivity event.
    \param pointer to inactivity callback object.
    \retval error code.    
*/
TInt inactivityCallback(TAny *ptr)
{
    static_cast<SnsrUserActivityServicePrivate *>(ptr)->emitNotActive();
    // activity manager doesn't care about the return value,
    // we return a value indicating 'true' to be elegant (see CIdle)
    return 1;
}

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    \param servicePublic public implementation.
*/
SnsrUserActivityServicePrivate::SnsrUserActivityServicePrivate(SnsrUserActivityService *servicePublic) :
    m_q(servicePublic), mActivityManager(0), mInactivityPeriod(lDefaultInactivityPeriod)
{
    mActivityManager = CUserActivityManager::NewL(CActive::EPriorityStandard);
}

/*!
    Destructor.
*/
SnsrUserActivityServicePrivate::~SnsrUserActivityServicePrivate()
{
    delete mActivityManager;
}

/*!
    Sets the inactivity period after which to emit signal of inactivity.
    \param seconds after which inactivity is detected.
*/
void SnsrUserActivityServicePrivate::setInactivityPeriod(int seconds)
{
    mInactivityPeriod = seconds;
    // activity manager panics if timeout set before start
    if (isWatching())
    {
        mActivityManager->SetInactivityTimeout(TTimeIntervalSeconds(mInactivityPeriod));   
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
        mActivityManager->Start(
                TTimeIntervalSeconds(mInactivityPeriod),
                TCallBack(inactivityCallback, this),
                TCallBack(activityCallback, this));
    } else if (!shouldWatch && isWatching()) {
        mActivityManager->Cancel();
    }
}

/*!
    Checks if activity service is currently watching for user activity.
    \retval true if user acitivity service is active.
*/
bool SnsrUserActivityServicePrivate::isWatching() const
{
    return mActivityManager && mActivityManager->IsActive();
}

/*!
    Emits signal that user is active.
*/
void SnsrUserActivityServicePrivate::emitActive() const
{
    emit m_q->active();
}

/*!
    Emits signal that user is not active.
*/
void SnsrUserActivityServicePrivate::emitNotActive() const
{
    emit m_q->notActive();
}
