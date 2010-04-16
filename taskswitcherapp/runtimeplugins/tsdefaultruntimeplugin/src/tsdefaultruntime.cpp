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
* Description: Default TaskSwitcher runtime.
*
*/

#include <QAbstractItemModel>
#include <QState>
#include <QFinalState>
#include <qservicemanager.h>

#include "tsdefaultruntime.h"

QTM_USE_NAMESPACE

/*!
    \class TsDefaultRuntime
    \ingroup group_tsdefaultruntimeplugin
    \brief Default implementation of the taskswitcher runtime.
*/

/*!
    Constructs a new TsDefaultRuntime with parent.
*/
TsDefaultRuntime::TsDefaultRuntime(QObject *parent) : QStateMachine(parent)
{
    // load services
    // FIXME: workaround for bug in QtSF - can't destroy QServiceManager
    QServiceManager *serviceManager = new QServiceManager;  
    QObject *itemProvider = createCriticalInterface(serviceManager, "com.nokia.taskswitcher.itemprovider");
    QObject *activation = createCriticalInterface(serviceManager, "com.nokia.taskswitcher.activation");
    QObject *deactivation = createCriticalInterface(serviceManager, "com.nokia.taskswitcher.deactivation");
    QObject *presentation = createCriticalInterface(serviceManager, "com.nokia.taskswitcher.presentation");

    // create states
    QState *taskSwitcherStates = new QState(this);
    QState *backgroundState = new QState(taskSwitcherStates);
    QState *activeState = new QState(taskSwitcherStates);
    QFinalState *finalState = new QFinalState(this);
    setInitialState(taskSwitcherStates);
    taskSwitcherStates->setInitialState(backgroundState);

    // Forward signals emited by statemachine.
    backgroundState->addTransition(activation, SIGNAL(activated()), activeState);
    activeState->addTransition(deactivation, SIGNAL(deactivated()), backgroundState);
    taskSwitcherStates->addTransition(this, SIGNAL(event_exit()), finalState);

    // exchange model between itemprovider and presentation service
    QAbstractItemModel *model;
    QMetaObject::invokeMethod(itemProvider, "model", Q_RETURN_ARG(QAbstractItemModel*, model));
    QMetaObject::invokeMethod(presentation, "setModel", Q_ARG(QAbstractItemModel*, model));

    // connect all services
    connect(presentation, SIGNAL(openApplication(QModelIndex)), itemProvider, SLOT(openApplication(QModelIndex)));
    connect(presentation, SIGNAL(closeApplication(QModelIndex)), itemProvider, SLOT(closeApplication(QModelIndex)));
    connect(presentation, SIGNAL(closeAllApplications()), itemProvider, SLOT(closeAllApplications()));

    connect(presentation, SIGNAL(hideTaskSwitcher()), deactivation, SIGNAL(deactivated()));

    connect(activeState, SIGNAL(exited()), presentation, SLOT(hide()));
    connect(activeState, SIGNAL(entered()), presentation, SLOT(show()));

    connect(activeState, SIGNAL(exited()), itemProvider, SLOT(clearClosedApplicationList()));
}

/*!
    Creates critical interface with /a name using passed /a serviceManager.
*/
QObject *TsDefaultRuntime::createCriticalInterface(QServiceManager *serviceManager, const QString &name)
{
    QObject *interface = serviceManager->loadInterface(name);
    Q_ASSERT_X(interface, "TsDefaultRuntime::createCriticalInterface", qPrintable(QString("Cannot initialize critical %1 interafce").arg(name)));
    interface->setParent(this);
    return interface;
}
