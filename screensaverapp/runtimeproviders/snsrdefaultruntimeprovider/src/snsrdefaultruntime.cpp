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

#include "snsrdefaultruntime.h"

#include <QStateMachine>
#include <QState>

#include <hbmainwindow.h>

#include <hsistateprovider.h>
#include <hsstatefactory.h>

/*!
    \class SnsrDefaultRuntime
    \ingroup group_snsrdefaultruntimeprovider
    \brief Default implementation of the screensaver runtime.
*/

const char factoryManifestDir [] = "snsrresources/plugins/stateproviders";
const char factoryPluginDir [] = "snsrresources/plugins/stateproviders";
const char stateLibrary[] = "snsrdefaultstateprovider.dll";

const char rootStateUri [] = "screensaver.nokia.com/state/root";

/*!
    Constructs a new SnsrDefaultRuntime with parent.
*/
SnsrDefaultRuntime::SnsrDefaultRuntime(QObject *parent) :
    HsRuntime(parent),
    mStateMachine(0),
    mWindow(0)
{
    initializeUserInterface();
    initializeStateMachine();
}

/*!
    Destructs the class.
*/
SnsrDefaultRuntime::~SnsrDefaultRuntime()
{
    delete mWindow;
}

/*!
    Starts the runtime.
*/
void SnsrDefaultRuntime::start()
{
    mStateMachine->start();
}

/*!
    Stops the runtime.
*/
void SnsrDefaultRuntime::stop()
{
    mStateMachine->stop();
}

/*!
    Function initializes objects required to UI creation.
*/
void SnsrDefaultRuntime::initializeUserInterface()
{
    mWindow = new HbMainWindow();
    mWindow->show();
}

/*!
    Function initialize state machine.
*/
void SnsrDefaultRuntime::initializeStateMachine()
{
    // Ownership transfered to application.
    // State machine instance will be removed after runtime destruction.
    mStateMachine = new QStateMachine(this);

    // Forward signals emited by statemachine.
    connect(mStateMachine, SIGNAL(started()), SIGNAL(started()));
    connect(mStateMachine, SIGNAL(stopped()), SIGNAL(stopped()));

    createStates();
}

/*!
    Function creates and initializes UI states. 
*/
void SnsrDefaultRuntime::createStates()
{
    HsStateToken token;
    HsStateFactory factory(factoryManifestDir, factoryPluginDir);

    token.mLibrary = stateLibrary;
    token.mUri = rootStateUri;
    QState *snsrRootState = factory.createState(token);
    snsrRootState->setParent(mStateMachine);
    snsrRootState->setObjectName(token.mUri);

    mStateMachine->setInitialState(snsrRootState);
}
