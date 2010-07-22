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
#include <HbDeviceDialog>

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
 Returns native and java applications.
 \param sortAttribute :: SortAttribute
 \retval HsMenuItemModel: installed model
 */
HsMenuItemModel *HsMenuService::getInstalledModel(
    HsSortAttribute sortAttribute)
{
    //TODO get proper items
    qDebug() << "HsMenuService::getInstalledModel" << "sortAttribute:"
             << sortAttribute;
    HSMENUTEST_FUNC_ENTRY("HsMenuService::getInstalledModel");
    CaQuery query;
    query.addEntryTypeName(packageTypeName());
    query.setFlagsOn(VisibleEntryFlag | RemovableEntryFlag);
    query.setFlagsOff(MissingEntryFlag);
    query.setSort(HsMenuServiceUtils::sortBy(sortAttribute),
                  HsMenuServiceUtils::sortOrder(sortAttribute));
    HsMenuItemModel *model = new HsMenuItemModel(query);
    HSMENUTEST_FUNC_EXIT("HsMenuService::getInstalledModel");
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
    if(!collectionId) {
        collectionId = collectionIdByType(collectionType);
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
    QList< QSharedPointer<CaEntry> > collections = CaService::instance()->getEntries(query);
    QStringList resultList;
    foreach(QSharedPointer<CaEntry> entry, collections) {
        resultList << entry->text();
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
    QSharedPointer<CaEntry> entry = CaService::instance()->getEntry(entryId);

    QString name;
    if (!entry.isNull()) {
        name = entry->text();
    }
    qDebug() << "HsMenuService::getName name: " << name;
    HSMENUTEST_FUNC_EXIT("HsMenuService::getName");
    return name;
}

/*!
 Executes action on an item
 \param entryId of this item
 \param actionName string with action name
 \retval int error code, 0 if no error
 */
int HsMenuService::executeAction(int entryId, const QString &actionName)
{
    qDebug() << "HsMenuService::executeAction entryId:" << entryId
             << "actionName:" << actionName;
    int ret = CaService::instance()->executeCommand(entryId, actionName);

/*    // if its remove action we need to mark all items 
    // that are being uninstalled
    if (actionName == caCmdRemove && ret == 0) {
        QSharedPointer<CaEntry> entry(
                CaService::instance()->getEntry(entryId));
        if (!entry.isNull()) {
            QString componentId = entry->attribute(
                    componentIdAttributeName());
            
            CaQuery query;
            query.setAttribute(componentIdAttributeName(), componentId);
            QList< QSharedPointer<CaEntry> > entries 
                    = CaService::instance()->getEntries(query);
            
            for (int i = 0; i < entries.count(); i++) {
                entries[i]->setFlags(
                        entries.first()->flags()| UninstallEntryFlag);
                CaService::instance()->updateEntry(*entries[i]);
            }
        }
    }*/
    return ret;
}

/*!
 Launch task switcher
 \retval boolean launching status
 */
bool HsMenuService::launchTaskSwitcher()
{
    qDebug() << "HsMenuService::launchTaskSwitcher";
    HbDeviceDialog deviceDialog;
    QVariantMap params;
    return deviceDialog.show(TS_DEVICE_DIALOG_URI, params);
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
    collection.setAttribute(groupNameAttributeName(),name);
    CaIconDescription iconDescription;
    iconDescription.setFilename(defaultCollectionIconId());
    collection.setIconDescription(iconDescription);
    QSharedPointer<CaEntry> entry = CaService::instance()->createEntry(collection);
    if (!entry.isNull()) {
        qDebug() << "HsMenuService::addCollection entry" << entry;
        entryId = entry->id();
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
    QSharedPointer<CaEntry> collection = CaService::instance()->getEntry(collectionId);
    if (collection) {
        qDebug() << "HsMenuService::renameCollection collection"
                 << collection;

        collection->setText(newCollectionName);
        result = CaService::instance()->updateEntry(*collection);
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
 Retrives the first found collection entry id
 \param collectionType collection type.
 \retval collectionType id
 */
int HsMenuService::collectionIdByType(const QString& collectionType)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuService::collectionsIdByType");
    int collectionId;
    CaQuery collectionsQuery;
    collectionsQuery.setEntryRoles(GroupEntryRole);
    //sorting is set to (default, ascending) to assure that
    //proper entry is fetched, somebody can add item with
    //"menucollections" typename to the storage, but even if he or she
    //do this we fetch entry that we wanted
    collectionsQuery.setSort(DefaultSortAttribute, Qt::AscendingOrder);
    collectionsQuery.addEntryTypeName(collectionType);
    QList<int> ids = CaService::instance()->getEntryIds(
                         collectionsQuery);
    Q_ASSERT(ids.count() > 0);
    collectionId = ids.at(0);
    qDebug() << "HsMenuService::HsMenuService collectionsIdByType"
             << collectionId;
    HSMENUTEST_FUNC_EXIT("HsMenuService::collectionsIdByType");
    return collectionId;
}

/*!
 Touch action on an entry.
 \param entryId of this entry.
 \retval boolean error code.
 */
bool HsMenuService::touch(int entryId)
{
    QSharedPointer<CaEntry> entry = CaService::instance()->getEntry(entryId);
    return CaService::instance()->touch(* entry);
}

/*!
 Launch SoftwareUpdateApplication
 \retval int launching status, 0 if no errors
 */
int HsMenuService::launchSoftwareUpdate()
{
    qDebug() << "HsMenuService::launchSoftwareUpdate";
    QScopedPointer<CaEntry> tsEntry(new CaEntry);
    tsEntry->setEntryTypeName(applicationTypeName());
    tsEntry->setAttribute(
            applicationUidEntryKey(), QString::number(softwareUpdateApplicationUid));
    int retval = CaService::instance()->executeCommand(*tsEntry,
            openActionIdentifier());
    return retval;
}
