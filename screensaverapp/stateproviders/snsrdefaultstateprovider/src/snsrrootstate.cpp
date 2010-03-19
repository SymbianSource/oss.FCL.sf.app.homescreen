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
* Description:  Root Screensaver state.
*
*/

#include "snsrrootstate.h"

#include <QDebug>
#include <QEventTransition>

#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbview.h>

#include <screensaverfactory.h>

#include "snsrtest_global.h"
#include "snsruseractivityservice.h"

/*!
    \class SnsrRootState
    \ingroup group_snsrdefaultstateprovider
    \brief Root Screensaver state.
 */

/*!
    Constructs a new SnsrRootState with parent.
*/
SnsrRootState::SnsrRootState(QState *parent) : 
    QState(parent), mScreensaver(0), mCurrentView(0), mMainWindow(0)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrRootState::SnsrRootState")
    
    mMainWindow = HbInstance::instance()->allMainWindows().at(0);
    // TODO: consider if this should be Screensaver's responsibility
    mMainWindow->hideItems(Hb::AllItems);
    // load screensaver plugin
    mUserActivity = new SnsrUserActivityService();
    mUserActivity->setInactivityPeriod(5); // 5 seconds inactivity period

    ScreensaverFactory factory(QString("snsrresources/plugins/screensaverproviders"));
    ScreensaverToken bigClockToken;
    bigClockToken.mLibrary = QString("snsrbigclockscreensaverprovider.dll");
    bigClockToken.mUri = QString("screensaver.nokia.com/screensaver/bigclock");
    mScreensaver = factory.createScreensaver(bigClockToken);
    if (mScreensaver) {
        mScreensaver->setParent(this);
    } else {
        // TODO: error state
    }
    connect(mScreensaver, SIGNAL(viewChanged(QGraphicsWidget*)), SLOT(changeView(QGraphicsWidget*)));
    connect(mScreensaver, SIGNAL(faulted()), SLOT(screensaverFaulted()));

    // foreground state
    QState *foregroundState = new QState(this);
    connect(foregroundState, SIGNAL(entered()), mScreensaver, SLOT(foreground()));

    // background state
    QState *backgroundState = new QState(this);
    connect(backgroundState, SIGNAL(entered()), mScreensaver, SLOT(background()));

    // partial foreground state
    QState *partialForegroundState = new QState(this);
    connect(partialForegroundState, SIGNAL(entered()), mScreensaver, SLOT(partialForeground()));

    //setup user inactivity behaviour
    partialForegroundState->addTransition(mUserActivity, SIGNAL(active()), foregroundState);
    foregroundState->addTransition(mUserActivity, SIGNAL(notActive()), partialForegroundState);

    // power save state

    // set up transitions
    QEventTransition *toPartialForegroundTransition =
        new QEventTransition(qApp, QEvent::ApplicationActivate);
    toPartialForegroundTransition->setTargetState(partialForegroundState);
    backgroundState->addTransition(toPartialForegroundTransition);

    QEventTransition *toBackgroundTransition = 
        new QEventTransition(qApp, QEvent::ApplicationDeactivate);
    toBackgroundTransition->setTargetState(backgroundState);
    partialForegroundState->addTransition(toBackgroundTransition);

    QEventTransition *toBackgroundTransition2 =
        new QEventTransition(qApp, QEvent::ApplicationDeactivate);
    toBackgroundTransition2->setTargetState(backgroundState);
    foregroundState->addTransition(toBackgroundTransition2);

    setInitialState(partialForegroundState);
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrRootState::SnsrRootState")
}

/*!
    Destructs the class.
*/
SnsrRootState::~SnsrRootState()
{
    // mScreensaver deleted by parent
}

/*!
    This function is called when the state is entered.
*/
void SnsrRootState::onEntry(QEvent *event)
{
    QState::onEntry(event);
    qDebug() << objectName() << "- onEntry()";

    mScreensaver->initialize();
}

/*!
    This function is called when the state is exited.
*/
void SnsrRootState::onExit(QEvent *event)
{
    if (mCurrentView) {
        mMainWindow->removeView(mCurrentView);
    }

    mScreensaver->close();

    qDebug() << objectName() << "- onExit()";
    QState::onExit(event);
}

/*!
    Changes view in main window on widget.
    \param widget Widget with new view.
*/
void SnsrRootState::changeView(QGraphicsWidget *widget)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrRootState::changeView")
    
    qDebug("SnsrRootState::changeView() - widget: 0x%X", (int)widget);

    if (mCurrentView) {
        mCurrentView->hide();
        mMainWindow->removeView(mCurrentView);
    }
    mCurrentView = widget;
    if (mCurrentView) {
        mCurrentView->show();
        mMainWindow->addView(mCurrentView);
        mMainWindow->currentView()->setContentFullScreen(true);
    }
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrRootState::changeView")
}

/*!
    Invoked when screensaver faulted.
*/
void SnsrRootState::screensaverFaulted()
{
    qWarning() << "SnsrRootState::screensaverFaulted()";
    // TODO: some error handling
}
