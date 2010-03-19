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
 * Description: hsovistorehandler_p.cpp
 *
 */
#ifndef NO_QT_EXTENSIONS

#include <QDebug>
#include <XQSettingsManager>
#include <XQSettingsKey>
#include "hsovistorehandler_p.h"
#include "hsmenuservice.h"
#include "caentry.h"
#include "caservice.h"

const int AppLibUid         = 0x20022F97;
const int IconId            = 0x00000001;
const int OviStoreAppUid    = 0x00000002;
const int OviStoreUrl       = 0x00000003;

// ======== MEMBER FUNCTIONS ========

/*!
 Constructor
 \param parent parent object
 */
HsOviStoreHandlerPrivate::HsOviStoreHandlerPrivate(QObject *parent):
    QObject(parent),
    mOviStore(0),
    mIcon()
{
    readCentralRepository();
}

/*!
 Destructor
 */
HsOviStoreHandlerPrivate::~HsOviStoreHandlerPrivate()
{
    delete mOviStore;
}

/*!
 Executes action
 */
void HsOviStoreHandlerPrivate::action()
{
    qDebug("HsOviStoreHandlerPrivate::action");
    CaService::instance()->executeCommand(*mOviStore);
}

/*!
 Returns Ovi Store icon predefined in Central Repositoryicon
 \retval icon
 */
HbIcon HsOviStoreHandlerPrivate::icon()
{
    return mIcon;
}

/*!
 Creates icon based on Central Repository
 \param crManager settings manager
 \retval icon
 */
HbIcon HsOviStoreHandlerPrivate::createIcon(XQSettingsManager *crManager)
{
    qDebug("HsOviStoreHandlerPrivate::createIcon");
    HbIcon icon;

    XQCentralRepositorySettingsKey iconKey(AppLibUid, IconId);
    QVariant iconVarian = crManager->readItemValue(
                              iconKey, XQSettingsManager::TypeString);

    if (!iconVarian.isNull() && !iconVarian.toString().isEmpty()) {
        qDebug() << "HsOviStoreHandlerPrivate::createIcon"
                 << iconVarian.toString();
        icon = HbIcon(iconVarian.toString());
    }
    return icon;
}

/*!
 Creates application entry based on Central Repository
 \param crManager settings manager
 \retval entry
 */
CaEntry *HsOviStoreHandlerPrivate::createAppEntry(XQSettingsManager *crManager)
{
    qDebug("HsOviStoreHandlerPrivate::createAppEntry");
    XQCentralRepositorySettingsKey appUidKey(AppLibUid, OviStoreAppUid);
    QVariant appUidVariant = crManager->readItemValue(
                                 appUidKey, XQSettingsManager::TypeString);

    if (!appUidVariant.isNull() && !appUidVariant.toString().isEmpty()) {
        qDebug() << "HsOviStoreHandlerPrivate::createAppEntry"
                 << appUidVariant.toString();

        CaEntry *oviEntry = new CaEntry;
        oviEntry->setEntryTypeName(applicationTypeName());
        oviEntry->setAttribute(applicationUidEntryKey(), appUidVariant.toString());
        return oviEntry;
    }
    return NULL;
}

/*!
 Creates url entry based on Central Repository
 \param crManager settings manager
 \retval entry
 */
CaEntry *HsOviStoreHandlerPrivate::createUrlEntry(XQSettingsManager *crManager)
{
    qDebug("HsOviStoreHandlerPrivate::createUrlEntry");
    CaEntry *oviEntry = new CaEntry;

    XQCentralRepositorySettingsKey urlKey(AppLibUid, OviStoreUrl);
    QVariant urlVarian = crManager->readItemValue(
                             urlKey, XQSettingsManager::TypeString);

    if (!urlVarian.isNull() && !urlVarian.toString().isEmpty()) {
        oviEntry->setEntryTypeName(urlTypeName());
        oviEntry->setAttribute(urlEntryKey(), urlVarian.toString());
        qDebug() << "HsOviStoreHandlerPrivate::createUrlEntry"
                 << urlVarian.toString();

    }
    return oviEntry;
}

/*!
 Reads Central Repository
 */
void HsOviStoreHandlerPrivate::readCentralRepository()
{
    XQSettingsManager *crManager = new XQSettingsManager;

    mIcon = createIcon(crManager);
    mOviStore = createAppEntry(crManager);
    if (!mOviStore) {
        mOviStore = createUrlEntry(crManager);
    }
    delete crManager;
}
#endif
