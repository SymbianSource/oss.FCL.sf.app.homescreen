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
* Description:  Implementation for SQLite content store.
*
*/

#ifndef HSDATABASE_H
#define HSDATABASE_H

#include <QScopedPointer>
#include <QtSql>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestRuntimeServices)
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HsSceneData;
class HsPageData;
class HsWidgetData;
class HsWidgetPresentationData;

class HSDOMAINMODEL_EXPORT HsDatabase : public QObject
{
    Q_OBJECT

public:
    static void setDatabaseName(const QString& dbName);
    static HsDatabase *instance();
    ~HsDatabase();

    bool transaction();
    bool rollback();
    bool commit();

    bool scene(HsSceneData &scene);
    bool updateScene(const HsSceneData &scene);
    
    bool pages(QList<HsPageData> &pages);
    bool page(int id, HsPageData &page, bool getChildren = true);
    bool insertPage(HsPageData &page);
    bool updatePage(const HsPageData &page, bool updateChildren = true);
    bool deletePage(int id);

    bool widget(int id, HsWidgetData &widget, bool getChildren = true);
    bool insertWidget(HsWidgetData &widget);
    bool insertWidget(const HsWidgetData &widget,int &databaseId);
    bool updateWidget(const HsWidgetData &widget, bool updateChildren = true);
    bool deleteWidget(int id);
    bool deleteWidgets(const QString &uri);

    bool widgetPresentation(int widgetId, const QString &key, HsWidgetPresentationData &presentation);
    bool insertWidgetPresentation(HsWidgetPresentationData &presentation);
    bool updateWidgetPresentation(const HsWidgetPresentationData &presentation);
    bool deleteWidgetPresentation(int id);

    bool setWidgetPreferenceForKey(int widgetId, const QString &key, const QString &value);
    bool widgetPreferenceForKey(int widgetId, const QString &key, QString &value);
    bool setWidgetPreferences(int widgetId, const QVariantMap &preferences);
    bool widgetPreferences(int widgetId, QVariantMap &preferences);
    bool widgetIds(const QString &uri, QList<int>& ids);

private:
    HsDatabase();
    bool openDatabase(const QString &databaseName);
    Q_DISABLE_COPY(HsDatabase)

    bool parsePage(const QSqlQuery &query, bool getChildren, HsPageData &page);
    bool parseWidget(const QSqlQuery &query, bool getChildren, HsWidgetData &widget);
    bool parseWidgetPresentation(const QSqlQuery &query, HsWidgetPresentationData &presentation);

    QVariant columnValue(const QSqlQuery &query, const QString &columnName) const;
    QSqlDatabase database() const;

private:
    static QScopedPointer<HsDatabase> mInstance;
    static QString  mDatabaseName;
    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSDATABASE_H
