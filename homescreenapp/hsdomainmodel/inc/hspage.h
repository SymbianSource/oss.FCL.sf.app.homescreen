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

#ifndef HSPAGE_H
#define HSPAGE_H

#include <HbWidget>
#include <HbIconItem>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

class HsWidgetHost;
class HsPageData;
class HsWallpaper;

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsPage : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(int pageIndex READ pageIndex)

public:
    HsPage(QGraphicsItem *parent = 0);
    ~HsPage();

    int databaseId() const;
    void setDatabaseId(int id);

    bool load();

    HsWallpaper *wallpaper() const;

    bool addExistingWidget(HsWidgetHost *widgetHost);
    bool removeWidget(HsWidgetHost *widgeHost);

    QList<HsWidgetHost *> newWidgets();
    bool addNewWidget(HsWidgetHost *widgetHost);
    void layoutNewWidgets();
    void resetNewWidgets();
    bool deleteFromDatabase();

    QList<HsWidgetHost *> widgets() const;

    bool isRemovable() const;
    void setRemovable(bool removable);

    bool isDefaultPage() const;
    bool isActivePage() const;
    
    static HsPage *createInstance(const HsPageData &pageData);

public slots:
    void showWidgets();
    void hideWidgets();
    void setOnline(bool online = true);

    void updateZValues();

    int pageIndex();

private:
    Q_DISABLE_COPY(HsPage)
    void connectWidget(HsWidgetHost *widget);
    void disconnectWidget(HsWidgetHost *widget);

private slots:
    void onWidgetFinished();
    void onWidgetFaulted();
    void onWidgetResized();
    void onWidgetAvailable();
    void onWidgetUnavailable();

    void onOrientationChanged(Qt::Orientation orientation);

private:
    int mDatabaseId;
    HsWallpaper *mWallpaper;
    bool mRemovable;
    QList<HsWidgetHost*> mWidgets;
    QList<HsWidgetHost*> mNewWidgets;
    QList<HsWidgetHost*> mUnavailableWidgets;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif
