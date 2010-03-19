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
* Description:  Implementation for SQLlite content store.
*
*/

#include "hsdatabase.h"
#include "hsscenedata.h"
#include "hspagedata.h"
#include "hswidgetdata.h"
#include "hswidgetpresentationdata.h"
#include "hstest_global.h"

namespace 
{
    const char DATABASE_CONNECTION_NAME[] = "hsdb.connection";
}

#ifdef Q_OS_SYMBIAN
QString HsDatabase::mDatabaseName("c:\\private\\20022f35\\homescreendb");
#else
QString HsDatabase::mDatabaseName("homescreendb");
#endif //Q_OS_SYMBIAN

QScopedPointer<HsDatabase> HsDatabase::mInstance(0);

/*!
    Utility to create variant list from given \a list
*/
template<class T>
QVariantList toVariantList(const QList<T> &list)
{
    QVariantList vlist;
    foreach (T item, list) {
        vlist << item;
    }
    return vlist;
}

/*!
    Utility to insert given \a item to variant list \a count times
*/
template<class T>
QVariantList toVariantList(const T &item, int count)
{
    QVariantList vlist;
    for (int i = 0; i < count; ++i) {
        vlist << item;
    }
    return vlist;
}
/*!
    Set name of the database to use
*/
 void HsDatabase::setDatabaseName(const QString& dbName)
 {
    mDatabaseName = QDir::toNativeSeparators(dbName);
 }

/*!
    \class HsDatabase
    \ingroup group_hsdatamodel
    \brief Implementation for SQLlite content store.

    Home screen content store that uses an SQLite database
    as the data store.

*/

/*!
    Singleton. 
*/
HsDatabase *HsDatabase::instance()
{
    if (mInstance.isNull()) {
        mInstance.reset(new HsDatabase());
        if(!mInstance->openDatabase(mDatabaseName)){
            mInstance.reset();  
        }
    }
    return mInstance.data();
}

/*!
    Closes database and removes it.
*/
HsDatabase::~HsDatabase()
{
    // Database calls must be in scope, see info from
    // QSqlDatabase::removeDatabase() documentation.
    {
        QSqlDatabase db = database();
        db.close();
    }

    QSqlDatabase::removeDatabase(DATABASE_CONNECTION_NAME);
}

bool HsDatabase::transaction()
{
    return database().transaction();
}

bool HsDatabase::rollback()
{
    return database().rollback();
}

bool HsDatabase::commit()
{
    return database().commit();
}

/*!
    Returns scene data in \a scene
*/
bool HsDatabase::scene(HsSceneData &scene)
{   
    HsSceneData temp;

    QSqlDatabase db = database();        
    QSqlQuery query(db);

    query.prepare("SELECT * FROM Scene");

    if (!query.exec() || !query.next()) {
        return false;
    }
    
    temp.setId(columnValue(query, "Id").toInt());
    temp.setPortraitWallpaper(columnValue(query, "PortraitWallpaper").toString());
    temp.setLandscapeWallpaper(columnValue(query, "LandscapeWallpaper").toString());
        
    int defaultPageId = columnValue(query, "DefaultPageId").toInt();

    query.prepare("SELECT * FROM Pages WHERE Id = ?");
    query.addBindValue(defaultPageId);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    HsPageData page;    
    if (!parsePage(query, true, page)) {
        return false;
    }
    temp.setDefaultPage(page);

    scene = temp;

    return true;
}

/*!
    Updates the scene with \a scene.
*/
bool HsDatabase::updateScene(const HsSceneData &scene)
{
    QSqlDatabase db = database();

    QString queryString = "UPDATE Scene "
                          "SET PortraitWallpaper = ?, LandscapeWallpaper = ? "
                          "WHERE Id = ?";

  
    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(scene.portraitWallpaper());
    query.addBindValue(scene.landscapeWallpaper());    
    query.addBindValue(scene.id());

    return query.exec();
}

/*!
    Returns page data in \a pages list
*/
bool HsDatabase::pages(QList<HsPageData> &pages)
{
    QList<HsPageData> temp;

    QSqlDatabase db = database();           
    QSqlQuery query(db);

    query.prepare("SELECT * FROM Pages ORDER BY PageIndex");
    
    if (!query.exec()) {
        return false;
    }
    
    while (query.next()) {
        HsPageData page;
        if (!parsePage(query, true, page)) {
            return false;
        }
        temp << page;
    }

    pages = temp;

    return true;
}

/*!
    Returns a\ page data for the page with given \a id. Includes children
    data if \a getChildren is true
*/
bool HsDatabase::page(int id, HsPageData &page, bool getChildren)
{
    QSqlDatabase db = database();           
    QSqlQuery query(db);

    QString queryString = "SELECT * FROM Pages "
                          "WHERE Id = ?";

    query.prepare(queryString);
    query.addBindValue(id);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return parsePage(query, getChildren, page);
}

/*!
    Insert page based on given \a page data 
*/
bool HsDatabase::insertPage(HsPageData &page)
{
    QSqlDatabase db = database();

    QString queryString = "INSERT INTO Pages "
                          "(PageIndex) "
                          "VALUES(?)";

    QSqlQuery query(db);

    query.prepare(queryString);    
    query.addBindValue(page.index());
    
    if (!query.exec()) {
        return false;
    }

    page.setId(query.lastInsertId().toInt());

    QList<HsWidgetData> &widgets = page.widgets();
    for (int i = 0; i < widgets.count(); ++i) {
        widgets[i].setPageId(page.id());
        if (!insertWidget(widgets[i])) {
            return false;
        }
    }

    return true;
}


/*!
    Updates a page based on given \a page data. Includes children
    data if \a getChildren is true
*/
bool HsDatabase::updatePage(const HsPageData &page, bool updateChildren)
{
    QSqlDatabase db = database();

    QString queryString = "UPDATE Pages "
                          "SET PageIndex = ? "
                          "WHERE Id = ?";

  
    QSqlQuery query(db);

    query.prepare(queryString);    
    query.addBindValue(page.index());    
    query.addBindValue(page.id());

    if (!query.exec()) {
        return false;
    }

    if (!updateChildren) {
        return true;
    }

    QList<HsWidgetData> widgets = page.widgets();
    for (int i = 0; i < widgets.count(); ++i) {
        if (!updateWidget(widgets[i])) {
            return false;
        }
    }
    return true;
}

/*!
    Deletes page with given \a id
*/
bool HsDatabase::deletePage(int id)
{
    HsPageData pageToBeDeleted;

    if (!page(id, pageToBeDeleted, false)) {
        return false;
    }
   
    QSqlDatabase db = database();
              
    QString queryString = "DELETE FROM Pages "
                          "WHERE Id = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(id);
    
    if (!query.exec()) {
        return false;
    }

    //update other indexes
    QList<HsPageData> allPages;
    if (!pages(allPages)) {
        return false;    
    }

    HsPageData page;
    foreach (page, allPages) {
        if (page.index() > pageToBeDeleted.index()) {
            page.setIndex(page.index() - 1);
            if (!updatePage(page, false)) {
                return false;   
            }
        }
    }

    return true;

}

/*!
    Returns widget data for the widget with given id. Includes children
    data if \a getChildren is true
*/
bool HsDatabase::widget(int id, HsWidgetData &widget, bool getChildren)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);

    QString queryString = "SELECT * FROM Widgets "
                          "WHERE Id = ?";
    
    query.prepare(queryString);
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return parseWidget(query, getChildren, widget);
}

/*!
    Insert widget based on given \a widget data
*/
bool HsDatabase::insertWidget(HsWidgetData &widget)
{    
    QSqlDatabase db = database();

    QString queryString = "INSERT INTO Widgets "
                          "(Uri, PageId) "
                          "VALUES(?, ?)";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(widget.uri());
    query.addBindValue(widget.pageId());

    if (!query.exec()) {
        return false;
    }

    widget.setId(query.lastInsertId().toInt());

    QList<HsWidgetPresentationData> &presentations = widget.presentations();
    for (int i = 0; i < presentations.count(); ++i) {
        presentations[i].setWidgetId(widget.id());
        if (!insertWidgetPresentation(presentations[i])) {
            return false;
        }
    }

    return true;
}

/*!
    Insert widget based on given \a widget data. Widget \a databaseId
    is written on return.
*/
bool HsDatabase::insertWidget(const HsWidgetData &widget,
                              int &databaseId)
{
    QSqlDatabase db = database();

    QString queryString = "INSERT INTO Widgets "
                          "(Uri, PageId) "
                          "VALUES(?, ?)";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(widget.uri());
    query.addBindValue(widget.pageId());

    if (!query.exec()) {
        QSqlError err = query.lastError();
        return false;
    }

    databaseId = query.lastInsertId().toInt();

    QList<HsWidgetPresentationData> presentations = widget.presentations();
    for (int i = 0; i < presentations.count(); ++i) {
        presentations[i].setWidgetId(databaseId);
        if (!insertWidgetPresentation(presentations[i])) {
            return false;
        }
    }

    return true;
}

/*!
    Updates a widget based on given \a widget data. Includes children
    if \a getChildren is true
*/
bool HsDatabase::updateWidget(const HsWidgetData &widget, 
                              bool updateChildren)
{
    QSqlDatabase db = database();

    QString queryString = "UPDATE Widgets "
                          "SET Uri = ?";

    if (widget.pageId() != -1) {
        queryString += ", PageId = ?";
    }

    queryString += " WHERE Id = ?";

  
    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(widget.uri());
    if (widget.pageId() != -1) {
        query.addBindValue(widget.pageId());
    }
    query.addBindValue(widget.id());

    if (!query.exec()) {
        return false;
    }

    if (!updateChildren) {
        return true;
    }

    QList<HsWidgetPresentationData> presentations = widget.presentations();
    for (int i = 0; i < presentations.count(); ++i) {
        if (!updateWidgetPresentation(presentations[i])) {
            return false;
        }
    }

    return true;
}

/*!
    Deletes widget with given \a widget id
*/
bool HsDatabase::deleteWidget(int id)
{
    QSqlDatabase db = database();
       
    QString queryString = "DELETE FROM Widgets "
                          "WHERE Id = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(id);

    return query.exec();
}

/*!
    Deletes all widgets with given \a uri
*/
bool HsDatabase::deleteWidgets(const QString &uri)
{
    QSqlDatabase db = database();
       
    QString queryString = "DELETE FROM Widgets "
                          "WHERE Uri = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(uri);

    return query.exec();
}

/*!
    Returns widget \a presentation data for the widget with given \a widgetId. 
    Presentations can be distinguished by given \a key.
*/
bool HsDatabase::widgetPresentation(int widgetId, 
                                    const QString &key, 
                                    HsWidgetPresentationData &presentation)
{
    Q_UNUSED(key);

    QSqlDatabase db = database();
    QSqlQuery query(db);
    
    QString queryString = "SELECT * FROM WidgetPresentations "
                          "WHERE WidgetId = ? "
                          "AND Key = ?";
    
    query.prepare(queryString);
    query.addBindValue(widgetId);
    query.addBindValue(key);
    
    if (!query.exec() || !query.next()) {
        return false;
    }

    return parseWidgetPresentation(query, presentation);
}

/*!
    Inserts a widget \a presentation. 
*/
bool HsDatabase::insertWidgetPresentation(HsWidgetPresentationData &presentation)
{
    QSqlDatabase db = database();
 
    QString queryString = "INSERT INTO WidgetPresentations "
                          "(Key, Height, Width, YPosition, XPosition, ZValue, WidgetId) "
                          "VALUES(?, ?, ?, ?, ?, ?, ?)";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(presentation.key());
    query.addBindValue(presentation.size().height());
    query.addBindValue(presentation.size().width());
    query.addBindValue(presentation.position().y());
    query.addBindValue(presentation.position().x());
    query.addBindValue(presentation.zValue()); 
    query.addBindValue(presentation.widgetId());

    if (!query.exec()) {
        return false;
    }

    presentation.setId(query.lastInsertId().toInt());

    return true;
}

/*!
    Update a widget \a presentation. 
*/
bool HsDatabase::updateWidgetPresentation(const HsWidgetPresentationData &presentation)
{
    QSqlDatabase db = database();

    QString queryString = "UPDATE WidgetPresentations "
                          "SET Key = ?, Height = ?, Width = ?, YPosition = ?, XPosition = ?, ZValue = ?";
    
    if (presentation.widgetId() != -1) {
        queryString += ", WidgetId = ?";
    }
     
    queryString += " WHERE Id = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(presentation.key());
    query.addBindValue(presentation.size().height());
    query.addBindValue(presentation.size().width());
    query.addBindValue(presentation.position().y());
    query.addBindValue(presentation.position().x());
    query.addBindValue(presentation.zValue()); 
    if (presentation.widgetId() != -1) {
        query.addBindValue(presentation.widgetId());
    }    
    query.addBindValue(presentation.id());

    return query.exec();
}

/*!
    Delete widget \a presentation width given \a id. 
*/
bool HsDatabase::deleteWidgetPresentation(int id)
{
    QSqlDatabase db = database();

    QString queryString = "DELETE FROM WidgetPresentations "
                          "WHERE Id = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(id);

    return query.exec();
}

/*!
    Set widget preference \a value for given \a key for widget with given \a widgetId 
*/
bool HsDatabase::setWidgetPreferenceForKey(int widgetId, 
                                           const QString &key, 
                                           const QString &value)
{
    QSqlDatabase db = database();

    QString queryString = "SELECT COUNT() "
                          "FROM WidgetPreferences "
                          "WHERE WidgetId = ? "
                          "AND Key = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(widgetId);
    query.addBindValue(key);

    if (!query.exec() || !query.next()) {
        return false;
    }

    int count = query.value(0).toInt();

    if (count) {
        queryString = "UPDATE WidgetPreferences "
                      "SET Value = :value "
                      "WHERE WidgetId = :widgetId "
                      "AND Key = :key";
    } else {
        queryString = "INSERT INTO WidgetPreferences "
                      "(WidgetId, Key, Value) "
                      "VALUES(:widgetId, :key, :value)";
    }

    query.prepare(queryString);
    query.bindValue(":widgetId", widgetId);
    query.bindValue(":value", value);
    query.bindValue(":key", key);

    return query.exec();
}

/*!
    Returns widget preference \a value for given \a key for widget with given \a widgetId 
*/
bool HsDatabase::widgetPreferenceForKey(int widgetId, 
                                        const QString &key, 
                                        QString &value)
{
    QSqlDatabase db = database();

    QString queryString = "SELECT Value "
                          "FROM WidgetPreferences "
                          "WHERE WidgetId = ? "
                          "AND Key = ?";

    QSqlQuery query(db);

    query.prepare(queryString);
    query.addBindValue(widgetId);
    query.addBindValue(key);

    if (!query.exec() || !query.next()) {
        return false;
    }

    value = query.value(0).toString();

    return true;
}

/*!
    Store widget \a preferences for given \a widgetId. Returns true
    if successfull.
*/
bool HsDatabase::setWidgetPreferences(int widgetId, const QVariantMap &preferences)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);

    QList<QString> deleteNames = preferences.keys(QVariant());
    QVariantMap insertOrReplaceMap(preferences);
    foreach (QString deleteName, deleteNames) {
        insertOrReplaceMap.remove(deleteName);
    }

    if (!deleteNames.isEmpty()) {
        QString queryString = "DELETE FROM WidgetPreferences "
                              "WHERE WidgetId = ? "
                              "AND Key = ?";
        
        if (query.prepare(queryString)) {
            query.addBindValue(toVariantList(widgetId, deleteNames.count()));
            query.addBindValue(toVariantList(deleteNames));
            if (!query.execBatch()) {
                return false;
            }
        } else {
            return false;
        }
    }

    if (!insertOrReplaceMap.isEmpty()) {
        QString queryString = "REPLACE INTO WidgetPreferences "
                              "(WidgetId, Key, Value) "
                              "VALUES (?, ?, ?)";
            
        if (query.prepare(queryString)) {
            query.addBindValue(toVariantList(widgetId, insertOrReplaceMap.count()));
            query.addBindValue(toVariantList(insertOrReplaceMap.keys()));
            query.addBindValue(toVariantList(insertOrReplaceMap.values()));
            if (!query.execBatch()) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

/*!
    Fetch widget \a preferences based on given \a widgetId. Returns
    true if successfull.
*/
bool HsDatabase::widgetPreferences(int widgetId, QVariantMap &preferences)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);
    
    QString queryString = "SELECT Key, Value "
                          "FROM WidgetPreferences "
                          "WHERE WidgetId = ?";

    query.prepare(queryString);
    query.addBindValue(widgetId);
  
    if (query.exec()) {
        while (query.next()) {
            preferences.insert(query.value(0).toString(), 
                               query.value(1));
        }
    } else {
        return false;
    }
    return true;
}

/*!
    Returns instance \a ids of the widget identified by the given \a uri
*/
bool HsDatabase::widgetIds(const QString &uri, QList<int> &ids)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);

    QString queryString = "SELECT Id "
                          "FROM Widgets "
                          "WHERE Uri = ?";
    
    query.prepare(queryString);
    query.addBindValue(uri);

    if (!query.exec() || !query.next()) {
        return false;
    }

    ids << query.value(0).toInt();
    while (query.next()) {
        ids << query.value(0).toInt();        
    }

    return true;
}

/*!
    Constructor
*/
HsDatabase::HsDatabase()
    : QObject()
{
}
/*!
    Open database from given \a databaseName
*/
bool HsDatabase::openDatabase(const QString &databaseName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", DATABASE_CONNECTION_NAME);
    db.setDatabaseName(databaseName);
    if (!db.open()) {
        HSDEBUG( db.lastError().text().toLatin1() );
        return false;
    }
    return true;
}

/*!
    Parses \a page data from given \a query. Include child 
    data when \a getChildren is set to true.
*/
bool HsDatabase::parsePage(const QSqlQuery &query, 
                           bool getChildren, 
                           HsPageData &page)
{
    HsPageData temp;

    temp.setId(columnValue(query, "Id").toInt());    
    temp.setIndex(columnValue(query, "PageIndex").toInt());    
        
    if (getChildren) {
        QSqlDatabase db = database();
        QSqlQuery q(db);        
        
        QString queryString = "SELECT * "
                              "FROM Widgets "
                              "WHERE PageId = ?";
        
        q.prepare(queryString);
        q.addBindValue(temp.id());

        if (!q.exec()) {
            return false;
        }

        while (q.next()) {
            HsWidgetData widget;
            if (!parseWidget(q, true, widget)) {
                return false;
            }
            temp.widgets() << widget;
        }
    }

    page = temp;

    return true;
}

/*!
    Parses \a widget data from given \a query. Include child data 
    when \a getChildren is set to true.
*/
bool HsDatabase::parseWidget(const QSqlQuery &query, 
                             bool getChildren, 
                             HsWidgetData &widget)
{
    HsWidgetData temp;

    temp.setId(columnValue(query, "Id").toInt());
    temp.setUri(columnValue(query, "Uri").toString());
    temp.setPageId(columnValue(query, "PageId").toInt());

    if (getChildren) {
        QSqlDatabase db = database();
        QSqlQuery q(db);
        
        QString queryString = "SELECT * "
                              "FROM WidgetPresentations "
                              "WHERE WidgetId = ?";

        q.prepare(queryString);
        q.addBindValue(temp.id());

        if (!q.exec()) {
            return false;
        }

        while (q.next()) {
            HsWidgetPresentationData presentation;
            if (!parseWidgetPresentation(q, presentation)) {
                return false;
            }
            temp.presentations() << presentation;
        }
    }

    widget = temp;

    return true;
}

/*!
    Parses widget \a presentation data from given SQL \a query.
*/
bool HsDatabase::parseWidgetPresentation(const QSqlQuery &query, 
                                         HsWidgetPresentationData &presentation)
{
    presentation.setId(columnValue(query, "Id").toInt());
    presentation.setKey(columnValue(query, "Key").toString());
    presentation.setPosition(QPointF(columnValue(query, "XPosition").toDouble(),
                                     columnValue(query, "YPosition").toDouble()));
    presentation.setSize(QSizeF(columnValue(query, "Width").toDouble(),
                                columnValue(query, "Height").toDouble()));    
    presentation.setZValue(columnValue(query, "ZValue").toDouble());
    presentation.setWidgetId(columnValue(query, "WidgetId").toInt());

    return true;
}

/*!
    Parses column value with given \a columnName from given SQL \a query.
*/
QVariant HsDatabase::columnValue(const QSqlQuery &query, 
                                 const QString &columnName) const
{
    return query.value(query.record().indexOf(columnName));
}

/*!
    Returns database connection
*/
QSqlDatabase HsDatabase::database() const
{
    return QSqlDatabase::database(DATABASE_CONNECTION_NAME);
}
