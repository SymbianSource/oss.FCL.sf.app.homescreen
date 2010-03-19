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
* Description:  SelectBackground gui state.
*
*/

#ifndef HSSELECTBACKGROUNDSTATESTATE_H
#define HSSELECTBACKGROUNDSTATESTATE_H

#include <QState>
#include <QStringList>
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HbView;
class HsContentService;
class XQAIWGetImageClient;
class QImage;

class HsSelectBackgroundState : public QState
{
    Q_OBJECT

public:
    HsSelectBackgroundState(QState *parent = 0);
    ~HsSelectBackgroundState();

signals:
    void event_waitInput();

private:
    bool saveImage(QImage &image, const QString &path, Qt::Orientation orientation);

private slots:
    void selectPageBackgroundAction();
    void disconnectImageFetcherAction();
    void onBackgroundImageFetched(QStringList);
    void onBackgroundImageFetchFailed(int);

private:
    XQAIWGetImageClient *mXQAIWGetImageClient;
    HbView *mSourceView;

    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif
