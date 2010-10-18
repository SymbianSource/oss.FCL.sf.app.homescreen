/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef HSSYSTEMEVENTS_H
#define HSSYSTEMEVENTS_H

#include <QObject>

#include "hstest_global.h"
#include "hsdomainmodel_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsSystemEvents : public QObject
{
    Q_OBJECT

public:
    HsSystemEvents(QObject *parent = 0);
    ~HsSystemEvents();

    static HsSystemEvents *instance();
    static void setInstance(HsSystemEvents *instance);

signals:
    void homeKeyClicked();

private:
    Q_DISABLE_COPY(HsSystemEvents)

private: 
    static HsSystemEvents *mInstance;

    friend class HsStateMachine;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSSYSTEMEVENTS_H
