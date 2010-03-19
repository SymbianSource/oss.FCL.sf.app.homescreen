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

#ifndef SNSRROOTSTATE_H
#define SNSRROOTSTATE_H

#include <screensaver.h>

#include <QState>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrDefaultStateProvider)

class QGraphicsWidget;
class HbMainWindow;
class SnsrUserActivityServiceInterface;

class SnsrRootState : public QState
{
    Q_OBJECT

public:

    SnsrRootState(QState *parent = 0);
    virtual ~SnsrRootState();

protected:

    void onEntry(QEvent *event);
    void onExit(QEvent *event);

private slots:

    void changeView(QGraphicsWidget *widget);
    void screensaverFaulted();

private:

    Screensaver *mScreensaver;
    QGraphicsWidget *mCurrentView;
    HbMainWindow *mMainWindow;
    SnsrUserActivityServiceInterface *mUserActivity;
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrDefaultStateProvider)

};

#endif // SNSRROOTSTATE_H
