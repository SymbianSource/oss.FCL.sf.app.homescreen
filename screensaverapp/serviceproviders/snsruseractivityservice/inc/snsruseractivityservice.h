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
* Description: snsruseractivityservice.h
*
*/

#ifndef SNSRSNSRUSERACTIVITYSERVICE_H
#define SNSRSNSRUSERACTIVITYSERVICE_H

#include "snsruseractivityserviceinterface.h"
#include "snsruseractivityservice_global.h"

#include "snsrtest_global.h"

class SnsrUserActivityServicePrivate;

class SNSRUSERACTIVITYSERVICE_EXPORT SnsrUserActivityService : public SnsrUserActivityServiceInterface
{
    Q_OBJECT

public:

    explicit SnsrUserActivityService(/*QObject *parent = 0*/);
    ~SnsrUserActivityService();

    virtual void setInactivityPeriod(int seconds);
    virtual int inactivityPeriod() const;

private:

    //virtual void connectNotify(const char *signal);
    //virtual void disconnectNotify(const char *signal);

    //bool receiversConnected();

    Q_DISABLE_COPY(SnsrUserActivityService)

private:

    SnsrUserActivityServicePrivate *m_d;
    friend class SnsrUserActivityServicePrivate;
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrUserActivityService)

};

#endif // SNSRSNSRUSERACTIVITYSERVICE_H
