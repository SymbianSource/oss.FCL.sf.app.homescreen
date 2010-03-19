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

#ifndef HSSCENE_H
#define HSSCENE_H

#include <QObject>
#include <QScopedPointer>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestRuntimeServices)
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HbMainWindow;
class HsWallpaper;
class HsPage;
class HsWidgetHost;

class HsScenePrivate;
class HSDOMAINMODEL_EXPORT HsScene : public QObject
{
    Q_OBJECT

public:
    ~HsScene();

    int databaseId() const;

    bool load();

    HsWallpaper *wallpaper() const;

    QList<HsPage *> pages() const;

    bool addPage(HsPage *page);
    bool removePage(HsPage *page);

    bool setActivePage(HsPage *page);
    bool setActivePageIndex(int index);
    HsPage *activePage() const;
    int activePageIndex() const;

    void setActiveWidget(HsWidgetHost *widget);
    HsWidgetHost *activeWidget() const;

    void setOnline(bool online = true);
    bool isOnline() const;

    static HsScene *instance();
    static HbMainWindow *mainWindow();
    static Qt::Orientation orientation();

private:
    HsScene(QObject *parent = 0);
    Q_DISABLE_COPY(HsScene)

private:
    QScopedPointer<HsScenePrivate> mD;
    static QScopedPointer<HsScene> mInstance;

    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSSCENE_H
