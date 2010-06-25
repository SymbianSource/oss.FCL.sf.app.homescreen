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
* Description:  Default implementation of the home screen runtime.
*
*/

#ifndef HSDEFAULTRUNTIME_H
#define HSDEFAULTRUNTIME_H

#include <QStateMachine>
#include <qmobilityglobal.h>

#ifdef Q_OS_SYMBIAN
#include <xqkeycapture.h>
#endif

#ifndef HSDEFAULTRUNTIMEPLUGIN_UNITTEST
	#define TEST_CLASS_FWD
	#define TEST_FRIEND
#else
	#define TEST_CLASS_FWD class DefaultRuntimePluginTest;
    #define TEST_FRIEND friend class DefaultRuntimePluginTest;
#endif

class HsContentService;
QTM_BEGIN_NAMESPACE
class QValueSpacePublisher;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

TEST_CLASS_FWD

class HsDefaultRuntime : public QStateMachine
{
    Q_OBJECT

public:
    HsDefaultRuntime(QObject *parent = 0);
    ~HsDefaultRuntime();

signals:
    void event_exit();
    void event_toIdle();
protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(HsDefaultRuntime)

    void registerAnimations();
    void createStatePublisher();
    void createContentServiceParts();
    void createStates();
    void assignServices();
	void updatePSKeys();

private slots:
	void onIdleStateEntered();
	void onIdleStateExited();
	void activityRequested(const QString &name);
	

private:
    HsContentService *mContentService;	
	
	bool mHomeScreenActive;
	bool mIdleStateActive;

	QValueSpacePublisher *mPublisher;
	
#ifdef Q_OS_SYMBIAN
    XqKeyCapture keyCapture;
#endif
    
    TEST_FRIEND
};

#endif
