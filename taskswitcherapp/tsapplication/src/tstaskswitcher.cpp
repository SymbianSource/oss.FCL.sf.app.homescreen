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
* Description:
*
*/

#include "tstaskswitcher.h"

#include <QStateMachine>
#include <QEventLoop>

#include <qservicemanager.h>

QTM_USE_NAMESPACE

TsTaskSwitcher::TsTaskSwitcher(QObject* aParent) : QObject(aParent)
{
    // FIXME: we're leaking service manager to prevent crash caused by QServiceManager deletion
    QServiceManager *serviceManager = new QServiceManager;
            
    // FIXME: temporary workaround for bug in QtSF database - this process have to add services
    QList<QServiceInterfaceDescriptor> tsServiceInterfaces = serviceManager->findInterfaces("TaskSwitcher");
    QStringList requiredInterfaces;
    requiredInterfaces << "com.nokia.taskswitcher.itemprovider";
    requiredInterfaces << "com.nokia.taskswitcher.activation";
    requiredInterfaces << "com.nokia.taskswitcher.deactivation";
    requiredInterfaces << "com.nokia.taskswitcher.presentation";
    requiredInterfaces << "com.nokia.taskswitcher.runtime.defaultruntime";

    foreach (const QServiceInterfaceDescriptor &interface, tsServiceInterfaces) {
        QString interfaceName = interface.interfaceName();
        if (requiredInterfaces.contains(interfaceName)) {
            requiredInterfaces.removeAll(interfaceName);
        }
    }
    
    if (!requiredInterfaces.isEmpty()) {
        // clean old entries
        serviceManager->removeService("TaskSwitcher");
        
        bool servicesAdded = serviceManager->addService(":/tsserviceplugin.xml");
        Q_ASSERT_X(servicesAdded, "Adding Ts Services", qPrintable(QString("addService returned false, error %1").arg(serviceManager->error())));

        bool runtimeAdded = serviceManager->addService(":/tsdefaultruntimeplugin.xml");
        Q_ASSERT_X(runtimeAdded, "Adding Ts Runtime", qPrintable(QString("addService returned false, error %1").arg(serviceManager->error())));
    }
    // FIXME
    
    mRuntime = serviceManager->loadLocalTypedInterface<QStateMachine>("com.nokia.taskswitcher.runtime.defaultruntime");
    if (mRuntime) {
        mRuntime->setParent(this);
        connect(mRuntime, SIGNAL(stopped()), this, SIGNAL(exit()));
    } else {
        qCritical("Failed to create runtime");
    }
}

void TsTaskSwitcher::start()
{
    if (mRuntime) {
        mRuntime->start();
    } else {
        emit exit();
    }
}

void TsTaskSwitcher::stop()
{
    if (mRuntime) {
        if (mRuntime->isRunning()) {
            QEventLoop eventLoop;
            connect(mRuntime, SIGNAL(finished()), &eventLoop, SLOT(quit()));
            QMetaObject::invokeMethod(mRuntime, "event_exit", Qt::QueuedConnection);
            eventLoop.exec(); 
        }        
    }
}
