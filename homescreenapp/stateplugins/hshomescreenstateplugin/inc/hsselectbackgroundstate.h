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

#ifndef HSSELECTBACKGROUNDSTATESTATE_H
#define HSSELECTBACKGROUNDSTATESTATE_H

#include <QState>
#include <QStringList>
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class QImage;
class HbView;
#ifdef Q_OS_SYMBIAN
class HsImageFetcherClient;
#else
class XQAIWGetImageClient;
#endif

class HsSelectBackgroundState : public QState
{
    Q_OBJECT

public:
    HsSelectBackgroundState(QState *parent = 0);
    ~HsSelectBackgroundState();

signals:
    void event_waitInput();

private:
    Q_DISABLE_COPY(HsSelectBackgroundState)

private slots:
    void action_selectWallpaper();
    void action_disconnectImageFetcher();
   
    void onFetchComplete(QStringList imageStringList);
    void onFetchFailed(int error);

    // for S60 API
#ifdef Q_OS_SYMBIAN
    void fetchCompleted(const QString&);
    void fetchFailed(int, const QString&);    
#endif

private:
    
#ifdef Q_OS_SYMBIAN
    HsImageFetcherClient *mImageFetcher;
#else
    XQAIWGetImageClient *mImageFetcher;
#endif
    
    HbView *mSourceView;

    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif
