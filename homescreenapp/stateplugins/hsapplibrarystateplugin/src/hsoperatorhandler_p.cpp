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
 * Description: hsoperatorhandler_p.cpp
 *
 */
#ifndef NO_QT_EXTENSIONS

#include <QDebug>
#include <XQSettingsManager>
#include <XQSettingsKey>
#include "hsoperatorhandler_p.h"
#include "hsmenuservice.h"
#include "caentry.h"
#include "caservice.h"

const int AppLibUid         = 0x20022F97;
const int IconId            = 0x00000001;
const int OperatorAppUid    = 0x00000002;
const int OperatorUrl       = 0x00000003;
const int OperatorText      = 0x00000004;

// ======== MEMBER FUNCTIONS ========

/*!
 Constructor
 \param parent parent object
 */
HsOperatorHandlerPrivate::HsOperatorHandlerPrivate(QObject *parent):
    QObject(parent),
    mOperatorEntry(0),
    mIcon(),
    mText()
{
    readCentralRepository();
}

/*!
 Destructor
 */
HsOperatorHandlerPrivate::~HsOperatorHandlerPrivate()
{
    delete mOperatorEntry;
}

/*!
 Executes action
 */
void HsOperatorHandlerPrivate::action()
{
    qDebug("HsOperatorHandlerPrivate::action");
    CaService::instance()->executeCommand(*mOperatorEntry);
}

/*!
 Returns Operator icon predefined in Central Repositoryicon
 \retval icon
 */
HbIcon HsOperatorHandlerPrivate::icon()
{
    return mIcon;
}

/*!
 Returns Operator icon predefined in Central Repositoryicon
 \retval icon
 */
QString HsOperatorHandlerPrivate::text()
{
    return mText;
}

/*!
 Creates icon based on Central Repository
 \param crManager settings manager
 \retval icon
 */
HbIcon HsOperatorHandlerPrivate::createIcon(XQSettingsManager *crManager)
{
    qDebug("HsOperatorHandlerPrivate::createIcon");
    HbIcon icon;

    XQCentralRepositorySettingsKey iconKey(AppLibUid, IconId);
    QVariant iconVarian = crManager->readItemValue(
                              iconKey, XQSettingsManager::TypeString);

    if (!iconVarian.isNull() && !iconVarian.toString().isEmpty()) {
        qDebug() << "HsOperatorHandlerPrivate::createIcon"
                 << iconVarian.toString();
        icon = HbIcon(iconVarian.toString());
    }
    return icon;
}

/*!
 Creates text based on Central Repository
 \param crManager settings manager
 \retval icon
 */
QString HsOperatorHandlerPrivate::readText(XQSettingsManager *crManager)
{
    qDebug("HsOperatorHandlerPrivate::readText");
    QString text;

    XQCentralRepositorySettingsKey textKey(AppLibUid, OperatorText);
    QVariant textVariant = crManager->readItemValue(
                               textKey, XQSettingsManager::TypeString);

    if (!textVariant.isNull() && textVariant.isValid()) {
        qDebug() << "HsOperatorHandlerPrivate::createIcon"
                 << textVariant.toString();
        text = textVariant.toString();
    }
    return text;
}

/*!
 Creates application entry based on Central Repository
 \param crManager settings manager
 \retval entry
 */
CaEntry *HsOperatorHandlerPrivate::createAppEntry(
    XQSettingsManager *crManager)
{
    qDebug("HsOperatorHandlerPrivate::createAppEntry");
    XQCentralRepositorySettingsKey appUidKey(AppLibUid, OperatorAppUid);
    QVariant appUidVariant = crManager->readItemValue(
                                 appUidKey, XQSettingsManager::TypeString);

    if (!appUidVariant.isNull() && !appUidVariant.toString().isEmpty()) {
        qDebug() << "HsOperatorHandlerPrivate::createAppEntry"
                 << appUidVariant.toString();

        CaEntry *operatorEntry = new CaEntry;
        operatorEntry->setEntryTypeName(applicationTypeName());
        operatorEntry->setAttribute(
            applicationUidEntryKey(), appUidVariant.toString());
        return operatorEntry;
    }
    return NULL;
}

/*!
 Creates url entry based on Central Repository
 \param crManager settings manager
 \retval entry
 */
CaEntry *HsOperatorHandlerPrivate::createUrlEntry(
    XQSettingsManager *crManager)
{
    qDebug("HsOperatorHandlerPrivate::createUrlEntry");
    CaEntry *operatorEntry = new CaEntry;

    XQCentralRepositorySettingsKey urlKey(AppLibUid, OperatorUrl);
    QVariant urlVariant = crManager->readItemValue(
                              urlKey, XQSettingsManager::TypeString);

    if (!urlVariant.isNull() && !urlVariant.toString().isEmpty()) {
        operatorEntry->setEntryTypeName(urlTypeName());
        operatorEntry->setAttribute(urlEntryKey(), urlVariant.toString());
        qDebug() << "HsOperatorHandlerPrivate::createUrlEntry"
                 << urlVariant.toString();
    }
    return operatorEntry;
}

/*!
 Reads Central Repository
 */
void HsOperatorHandlerPrivate::readCentralRepository()
{
    XQSettingsManager *crManager = new XQSettingsManager;

    mIcon = createIcon(crManager);
    mText = readText(crManager);
    mOperatorEntry = createAppEntry(crManager);
    if (!mOperatorEntry) {
        mOperatorEntry = createUrlEntry(crManager);
    }
    delete crManager;
}
#endif
