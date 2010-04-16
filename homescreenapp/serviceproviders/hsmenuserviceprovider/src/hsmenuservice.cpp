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
 * Description: hsmenuservice.cpp
 *
 */

#include <QDebug>
#include <QStandardItem>
#include <qvaluespacepublisher.h>

#include "hsapp_defs.h"
#include "hsmenuservice.h"
#include "hsmenuserviceutils.h"
#include "caquery.h"
#include "caitemmodel.h"
#include "hsmenuitemmodel.h"
#include "hsmenucollectionsitemmodel.h"

QTM_USE_NAMESPACE

// ======== MEMBER FUNCTIONS ========

// Initialization of a static member variable.
int HsMenuService::mAllCollectionsId = 0;

/*!
 Returns all applications model
 \param sortAttribute ::  SortAttribute
 \retval HsMenuItemModel: AllApplicationsModel
 */
HsMenuItemModel *HsMenuService::getAllApplicationsModel(
    HsSortAttribute sortAttribute)
{
    qDebug() << "HsMenuService::getAllApplicationsModel sortAttribute:"
             << sortAttribute;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::getAllApplicationsModel");
    CaQuery query;
    query.setEntryRoles(ItemEntryRole);
    query.addEntryTypeName(applicationTypeName());
    query.addEntryTypeName(urlTypeName());
    query.addEntryTypeName(widgetTypeName());
    query.addEntryTypeName(templatedApplicationTypeName());
    query.setFlagsOn(VisibleEntryFlag);
    query.setFlagsOff(MissingEntryFlag);
    query.setSort(HsMenuServiceUtils::sortBy(sortAttribute),
                  HsMenuServiceUtils::sortOrder(sortAttribute));
    HsMenuItemModel *model = new HsMenuItemModel(query);
    HSMENUTEST_FUNC_EXIT("HsMenuService::getAllApplicationsModel");
    return model;
}

/*!
 Returns all collections model
 \param sortAttribute ::  SortAttribute
 \param details : switch to return details or not
 \retval HsMenuItemModel: all collections model
 */
HsMenuItemModel *HsMenuService::getAllCollectionsModel(
    HsSortAttribute sortAttribute)
{
    qDebug() << "HsMenuService::getAllCollectionsModel" << "sortAttribute:"
             << sortAttribute;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::getAllCollectionsModel");
    CaQuery query;
    query.setParentId(allCollectionsId());
    query.setFlagsOn(VisibleEntryFlag);
    query.setFlagsOff(MissingEntryFlag);
    query.setSort(HsMenuServiceUtils::sortBy(sortAttribute),
                  HsMenuServiceUtils::sortOrder(sortAttribute));
    HsMenuItemModel *model = new HsMenuCollectionsItemModel(query);
    HSMENUTEST_FUNC_EXIT("HsMenuService::getAllCollectionsModel");
    return model;
}

/*!
 Returns collection model
 \param collectionId ::  id of this collection
 \param sortAttribute ::  SortAttribute
 \retval HsMenuItemModel: collection model
 */
HsMenuItemModel *HsMenuService::getCollectionModel(int collectionId,
        HsSortAttribute sortAttribute, const QString &collectionType)
{
    qDebug() << "HsMenuService::getCollectionModel sortAttribute:"
             << sortAttribute;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::getCollectionModel");
    CaQuery query;
    query.setFlagsOff(MissingEntryFlag);
    if (collectionType == collectionDownloadedTypeName()) {
        query.setFlagsOn(RemovableEntryFlag | VisibleEntryFlag);
    } else {
        query.setFlagsOn(VisibleEntryFlag);
    }
    query.setParentId(collectionId);
    query.setSort(HsMenuServiceUtils::sortBy(sortAttribute),
                  HsMenuServiceUtils::sortOrder(sortAttribute));
    HsMenuItemModel *model = new HsMenuItemModel(query);
    HSMENUTEST_FUNC_EXIT("HsMenuService::getCollectionModel");
    return model;
}

/*!
 Returns all collections names
 \retval CaItemModel: list with collections names
 */
QStringList HsMenuService::getCollectionNames()
{
    qDebug() << "HsMenuService::getCollectionNames";
    HSMENUTEST_FUNC_ENTRY("HsMenuService::getCollectionNames");
    CaQuery query;
    query.setParentId(allCollectionsId());
    query.setFlagsOn(VisibleEntryFlag);
    query.setFlagsOff(MissingEntryFlag);
    QList<CaEntry *> collections = CaService::instance()->getEntries(query);
    QStringList resultList;
    while (!collections.isEmpty()) {
        CaEntry *entry = collections.takeFirst();
        resultList << entry->text();
        delete entry;
    }

    qDebug() << "HsMenuService::getCollectionNames resultList:"
             << resultList;
    HSMENUTEST_FUNC_EXIT("HsMenuService::getCollectionNames");
    return resultList;
}

/*!
 Returns name of an entry
 \param id of this entry
 \retval QString with name of the entry
 */
QString HsMenuService::getName(int entryId)
{
    qDebug() << "HsMenuService::getName entryId:" << entryId;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::getName");
    CaEntry *entry = CaService::instance()->getEntry(entryId);

    QString name;
    if (entry) {
        name = entry->text();
        delete entry;
    }
    qDebug() << "HsMenuService::getName name: " << name;
    HSMENUTEST_FUNC_EXIT("HsMenuService::getName");
    return name;
}

/*!
 Executes action on an item
 \param entryId of this item
 \param actionName string with action name
 \retval boolean error code
 */
bool HsMenuService::executeAction(int entryId, const QString &actionName)
{
    qDebug() << "HsMenuService::executeAction entryId:" << entryId
             << "actionName:" << actionName;

    return CaService::instance()->executeCommand(entryId, actionName);
}

/*!
 Launch task switcher
 \retval boolean launching status
 */
bool HsMenuService::launchTaskSwitcher()
{
    qDebug() << "HsMenuService::launchTS";
    QScopedPointer<CaEntry> tsEntry(new CaEntry);
    tsEntry->setEntryTypeName(applicationTypeName());
    tsEntry->setAttribute(
    		applicationUidEntryKey(), QString::number(taskSwitcherUid)); 
    int retval = CaService::instance()->executeCommand(*tsEntry, 
            openActionIdentifier());
    if(retval) {
        QValueSpacePublisher publisher("/TaskSwitcher");
        publisher.setValue("Activation", taskSwitcherPropertyValue);
    }
    return retval;
}

/*!
 Adds new collection
 \param name of the collection
 \retval entryId of new collection
 */
int HsMenuService::createCollection(const QString &name)
{
    qDebug() << "HsMenuService::addCollection name: " << name;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::createCollection");
    int entryId = 0;
    CaEntry collection(GroupEntryRole);
    collection.setEntryTypeName(collectionTypeName());
    collection.setText(name);
    CaIconDescription iconDescription;
    iconDescription.setFilename(defaultCollectionIconId());
    collection.setIconDescription(iconDescription);
    CaEntry *entry = CaService::instance()->createEntry(collection);
    if (entry) {
        qDebug() << "HsMenuService::addCollection entry" << entry;
        entryId = entry->id();
        delete entry;
        CaService::instance()->appendEntryToGroup(allCollectionsId(),
                entryId);
    }
    HSMENUTEST_FUNC_EXIT("HsMenuService::createCollection");
    return entryId;
}

/*!
 Renames a collection
 \param id of this collection
 \param new name for collection
 \retval boolean error code
 */
bool HsMenuService::renameCollection(int collectionId,
                                     const QString &newCollectionName)
{
    qDebug() << "HsMenuService::renameCollection collectionId:"
             << collectionId << "newCollectionName" << newCollectionName;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::renameCollection");
    bool result(false);
    CaEntry *collection = CaService::instance()->getEntry(collectionId);
    if (collection) {
        qDebug() << "HsMenuService::renameCollection collection"
                 << collection;

        collection->setText(newCollectionName);
        result = CaService::instance()->updateEntry(*collection);
        delete collection;
    }
    HSMENUTEST_FUNC_EXIT("HsMenuService::renameCollection");
    return result;
}

/*!
 Removes a collection
 \param id of this collection
 \retval boolean error code
 */
bool HsMenuService::removeCollection(int collectionId)
{
    qDebug() << "HsMenuService::removeCollection collectionId:"
             << collectionId;

    return CaService::instance()->removeEntry(collectionId);
}

/*!
 Adds applications to collection
 \param list with applications entries ids
 \param collection id
 \retval boolean error code
 */
bool HsMenuService::addApplicationsToCollection(
    const QList<int> &applicationList, int collectionId)
{
    qDebug() << "HsMenuService::addApplicationsToCollection"
             << "applicationList ids:" << applicationList << "collectionId:"
             << collectionId;

    return CaService::instance()->appendEntriesToGroup(collectionId,
            applicationList);
}

/*!
 Removes application from collection
 \param application id
 \param collection id
 \retval boolean error code
 */
bool HsMenuService::removeApplicationFromCollection(int applicationId,
        int collectionId)
{
    qDebug() << "HsMenuService::removeApplicationFromCollection"
             << "applicationId:" << applicationId << "collectionId:"
             << collectionId;

    return CaService::instance()->removeEntryFromGroup(collectionId,
            applicationId);
}

/*!
 Organizes collection's entries.
 \param groupId Group id.
 \param entryIdList Group's entries' ids list.
 \retval Return true if organize collection is done correctly,
  otherwise return false.
 */
bool HsMenuService::organizeCollection(int groupId, QList<int> &entryIdList)
{
    qDebug() << "HsMenuService::organizeCollection"
             << "groupId:" << groupId
             << "collectionIds:" << entryIdList;
    return CaService::instance()->customSort(groupId, entryIdList);
}

/*!
 Retrives the all collections entry id
 \retval all collections id
 */
int HsMenuService::allCollectionsId()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuService::allCollectionsId");
    if (mAllCollectionsId <= 0) {
        CaQuery collectionsQuery;
        collectionsQuery.setEntryRoles(GroupEntryRole);
        //sorting is set to (default, ascending) to assure that
        //proper entry is fetched, somebody can add item with
        //"menucollections" typename to the storage, but even if he or she
        //do this we fetch entry that we wanted
        collectionsQuery.setSort(DefaultSortAttribute, Qt::AscendingOrder);
        collectionsQuery.addEntryTypeName(menuCollectionsTypeName());
        QList<int> ids = CaService::instance()->getEntryIds(
                             collectionsQuery);
        Q_ASSERT(ids.count() > 0);
        mAllCollectionsId = ids.at(0);
        qDebug() << "HsMenuService::HsMenuService mAllCollectionsId"
                 << mAllCollectionsId;
    }
    HSMENUTEST_FUNC_EXIT("HsMenuService::allCollectionsId");
    return mAllCollectionsId;
}

/*!
 Touch action on an entry.
 \param entryId of this entry.
 \retval boolean error code.
 */
bool HsMenuService::touch(int entryId)
{
    CaEntry *entry = CaService::instance()->getEntry(entryId);
    return CaService::instance()->touch(* entry);
}
