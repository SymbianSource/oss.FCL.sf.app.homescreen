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
 * Description: hsmenuservice.h
 *
 */

#ifndef HSMENUSERVICE_H
#define HSMENUSERVICE_H

// System includes
#include <QObject>
#include <QStringList>

// User includes
#include "hsapp_defs.h"
#include "hsmenuservice_global.h"
#include "cadefs.h"
#include "caservice.h"

// Forward declarations
class HsMenuItemModel;

// Class declaration
class MENUSERVICE_EXPORT HsMenuService
{

public:

    // Function declarations
    static HsMenuItemModel *getAllApplicationsModel(
        HsSortAttribute sortAttribute = DescendingNameHsSortAttribute);
    static HsMenuItemModel *getAllCollectionsModel(
        HsSortAttribute sortAttribute = LatestOnTopHsSortAttribute);
    static HsMenuItemModel *getInstalledModel(
        HsSortAttribute sortAttribute = DescendingNameHsSortAttribute);
    static HsMenuItemModel *getCollectionModel(int collectionId,
            HsSortAttribute sortAttribute = LatestOnTopHsSortAttribute,
            const QString &collectionType = collectionTypeName());
    static QStringList getCollectionNames();
    static QString getName(int entryId);
    static bool executeAction(int entryId, const QString &actionName =
                                  openActionIdentifier());
    static bool launchTaskSwitcher();

    static int createCollection(const QString &name);
    static bool renameCollection(int collectionId,
                                 const QString &newCollectionName);
    static bool removeCollection(int collectionId);
    static bool addApplicationsToCollection(
        const QList<int> &applicationList, int collectionId);
    static bool removeApplicationFromCollection(int applicationId,
            int collectionId);

    static bool organizeCollection(int groupId, QList<int> &entryIdList);

    static int allCollectionsId();

    static bool touch(int entryId);
    
    static bool launchSoftwareUpdate();

private:
    // data

    static int mAllCollectionsId;

};

#endif // HSMENUSERVICE_H
