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
class XQAIWGetImageClient;

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
    bool saveImage(QImage &image, const QString &path, Qt::Orientation orientation);

private slots:
    void action_selectWallpaper();
    void action_disconnectImageFetcher();

    void onFetchComplete(QStringList imageStringList);
    void onFetchFailed(int error);

private:
    XQAIWGetImageClient *mImageFetcher;
    HbView *mSourceView;

    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif
