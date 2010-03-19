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
* Description:  Main Screensaver application class, loads runtime.
*
*/

#include "snsrscreensaver.h"

#include <hsruntimefactory.h>
#include <hsruntime.h>

#include "snsrtest_global.h"

/*!
    \class SnsrScreensaver
    \ingroup group_snsrapplication
    \brief Main Screensaver application class.

    Loads and manages the runtime.
*/

/*!
    Constructs a new SnsrScreensaver with parent.
*/
SnsrScreensaver::SnsrScreensaver(QObject *parent):
    QObject(parent)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrScreensaver::SnsrScreensaver")
    
    HsRuntimeFactory factory(
        "snsrresources/plugins/runtimeproviders",
        "snsrresources/plugins/runtimeproviders");

    HsRuntimeToken token;
    token.mLibrary = "snsrdefaultruntimeprovider.dll";
    token.mUri = "screensaver.nokia.com/runtime/defaultruntime";

    mRuntime = factory.createRuntime(token);
    if (mRuntime) {
        mRuntime->setParent(this);

        connect(mRuntime, SIGNAL(started()), SLOT(onRuntimeStarted()));
        connect(mRuntime, SIGNAL(stopped()), SLOT(onRuntimeStopped()));
        connect(mRuntime, SIGNAL(faulted()), SLOT(onRuntimeFaulted()));
    }
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrScreensaver::SnsrScreensaver")
}

/*!
    Destructs the class.
*/
SnsrScreensaver::~SnsrScreensaver()
{
}

/*!
    \fn void SnsrScreensaver::exit()

    This signal is emitted when the runtime ends job.
 */

/*!
    Starts the runtime.
*/
void SnsrScreensaver::start()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrScreensaver::start")
    
    if (mRuntime) {
        mRuntime->start();
    }
    else {
        emit exit();
    }
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrScreensaver::start")
}

/*!
    Stops the runtime.
*/
void SnsrScreensaver::stop()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrScreensaver::stop")
    
    if (mRuntime) {
        mRuntime->stop();
    }
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrScreensaver::stop")
}

/*!
    Called after the runtime has started.
*/
void SnsrScreensaver::onRuntimeStarted()
{
}
 
/*!
    Called after the runtime has stopped.
*/
void SnsrScreensaver::onRuntimeStopped()
{
    emit exit();
}
 
/*!
     Called after the runtime has faulted.
*/
void SnsrScreensaver::onRuntimeFaulted()
{
    emit exit();
}
