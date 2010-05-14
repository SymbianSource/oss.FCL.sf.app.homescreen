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
#include <QSizeF>

#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestRuntimeServices)
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HbMainWindow;
class HsWallpaper;
class HsPage;
class HsWidgetHost;
class HsSceneData;

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
    int maximumPageCount() const;
    QSizeF maximumWidgetSizeInPixels() const;
    QSizeF minimumWidgetSizeInPixels() const;
    QSizeF maximumWidgetSizeInUnits() const;
    QSizeF minimumWidgetSizeInUnits() const;
    void setActiveWidget(HsWidgetHost *widget);
    HsWidgetHost *activeWidget() const;

    void setOnline(bool online = true);
    bool isOnline() const;

    static HsScene *instance();
    static HbMainWindow *mainWindow();
    static Qt::Orientation orientation();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    HsScene(QObject *parent = 0);
    void calculateWidgetSizeLimitations(HsSceneData &sceneData);
    Q_DISABLE_COPY(HsScene)

private:
    int mDatabaseId;    
    HsWallpaper *mWallpaper;
    QList<HsPage *> mPages;
    HsPage *mActivePage;
    HsWidgetHost *mActiveWidget;
    bool mIsOnline;
    int mMaximumPageCount;
    QSizeF mMaximumWidgetSizeInPixels;
    QSizeF mMinimumWidgetSizeInPixels;
    QSizeF mMaximumWidgetSizeInUnits;
    QSizeF mMinimumWidgetSizeInUnits;
    
    static HsScene *mInstance;

    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSSCENE_H
