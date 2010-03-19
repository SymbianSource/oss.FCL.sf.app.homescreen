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

#ifndef TSDEFAULTRUNTIME_H
#define TSDEFAULTRUNTIME_H

#include <QStateMachine>
#include <qservicemanager.h>

QTM_USE_NAMESPACE

class TsDefaultRuntime : public QStateMachine
{
    Q_OBJECT

public:
    TsDefaultRuntime(QObject *parent = 0);

signals:
    void event_exit();

private:
    Q_DISABLE_COPY(TsDefaultRuntime)
    QObject *createCriticalInterface(QServiceManager *serviceManager, const QString &name);

};

#endif // TSDEFAULTRUNTIME_H
