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

#ifndef HSDATABASE_H
#define HSDATABASE_H

#include <QObject>
#include <QScopedPointer>
#include <QVariantHash>

#include "hsdomainmodel_global.h"

#include <HbFeedback>

class HsSceneData;
class HsPageData;
class HsWidgetData;
class HsWidgetPresentationData;
class HsGeneralConfiguration;
class HsFeedbackConfiguration;
class HsSnapConfiguration;

class HSDOMAINMODEL_EXPORT HsDatabase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName)
    Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName)
    
public:
    HsDatabase(QObject *parent = 0);
    ~HsDatabase();

    void setConnectionName(const QString &name);
    QString connectionName() const;    
    void setDatabaseName(const QString &name);
    QString databaseName() const;

    bool open();
    void close();

    bool transaction();
    bool rollback();
    bool commit();

    bool scene(HsSceneData &data);
    bool updateScene(const HsSceneData &data);

    bool pages(QList<HsPageData> &data);
    bool page(HsPageData &data);
    bool insertPage(HsPageData &data);
    bool updatePage(const HsPageData &data);
    bool deletePage(int id);
    
    bool widgets(int pageId, QList<HsWidgetData> &data);
    bool widgets(const QString &uri, QList<HsWidgetData> &data);
    bool widget(HsWidgetData &data);
    bool insertWidget(HsWidgetData &data);
    bool updateWidget(const HsWidgetData &data);
    bool deleteWidget(int id);
    bool deleteWidgets(const QString &uri);

    bool widgetPresentation(HsWidgetPresentationData &data);
    bool setWidgetPresentation(const HsWidgetPresentationData &data);
    bool deleteWidgetPresentation(int widgetId, Qt::Orientation orientation);

    bool widgetPreferences(int widgetId, QVariantHash &data);
    bool widgetPreference(int widgetId, const QString &key, QVariant &value);
    bool setWidgetPreferences(int widgetId, const QVariantHash &data);

    bool generalConfiguration(HsGeneralConfiguration &data);
    bool feedbackConfiguration(HsFeedbackConfiguration &data);
    bool snapConfiguration(HsSnapConfiguration &data);
    
public:
    static void setInstance(HsDatabase *instance);
    static HsDatabase *instance();
    static HsDatabase *takeInstance();

private:    
    Q_DISABLE_COPY(HsDatabase)
    bool checkConnection() const;

private:
    QString mConnectionName;
    QString mDatabaseName;

    static HsDatabase *mInstance;
};

#endif // HSDATABASE_H
