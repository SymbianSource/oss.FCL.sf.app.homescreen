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
* Description: tsmodel.cpp
*
*/
#include "tsmodel.h"
#include "tsmodelitem.h"
#include "tsentrymodelitem.h"
#include "tsactivitymodelitem.h"
#include "tsdataroles.h"

#include <HbIcon>
#include <qvariant.h>
#include <qlist.h>
#include <cadefs.h>

#include <canotifier.h>
#include <canotifierfilter.h>
#include <hsactivitydbclientinterface.h>

#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>

const int TSDeviceDialogUid = 0x2002677F;
const int ItemsLimit = 0x00000001;
#endif

const char entryTypeKeyword[] = "application";
const char applicationUidKeyword [] = "application:uid";
const int maxItems(10);
/*!
    \class TsModel
    \ingroup group_tsdevicedialogplugin
    \brief Model storing running application and activieties.
*/

/*!
    Constructor
    \param query used to create model
    \param pointer to parent object
*/
TsModel::TsModel(CaService &applicationSrv, QObject &activitySrv, QObject *parent) :
    QAbstractListModel(parent),
    mEntries(),
    mApplicationService(applicationSrv),
    mActivityService(activitySrv),
    mNotifier(NULL),
    mSize(240, 240),
    mMaxItems(maxItems)
{
    // creating query
    mQuery.setEntryRoles(ItemEntryRole);
    mQuery.addEntryTypeName(entryTypeKeyword);
    mQuery.setFlagsOn(VisibleEntryFlag);
    mQuery.setFlagsOff(MissingEntryFlag);
    mQuery.setFlagsOn(UsedEntryFlag);
    mQuery.setSort(LastUsedSortAttribute, Qt::DescendingOrder);
    // requesting new data
    updateModel();
    // creating notyfication filter
    CaNotifierFilter filter(mQuery);
    mNotifier = mApplicationService.createNotifier(filter);
    connect(mNotifier, SIGNAL(entryChanged(CaEntry,ChangeType)), this, SLOT(entryChanged(CaEntry, ChangeType)));
    connect(mNotifier, SIGNAL(entryTouched(int)), this, SLOT(updateModel()));
    if (mQuery.parentId() > 0) {
        connect(mNotifier, SIGNAL(groupContentChanged(int)), this, SLOT(updateModel()));
    }

#ifdef Q_OS_SYMBIAN
    XQSettingsManager *crManager = new XQSettingsManager;
    XQCentralRepositorySettingsKey itemsNumberKey(TSDeviceDialogUid, ItemsLimit);
    QVariant itemsNumberVariant = crManager->readItemValue(itemsNumberKey, XQSettingsManager::TypeInt);
    if (!itemsNumberVariant.isNull()) {
        int number = itemsNumberVariant.toInt();
        if (number > 0) {
            mMaxItems = number;
        }
    }
#endif
}

/*!
    Destructor
*/
TsModel::~TsModel()
{
    qDeleteAll(mEntries);
    delete mNotifier;
}

/*!
    Returns count of rows in model
    \retval number of rows
*/
int TsModel::rowCount(
    const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEntries.count();
}

/*!
    Returns appropiate model's data
    \param index model index
    \param role which data role to return
    \retval models data
*/
QVariant TsModel::data(const QModelIndex &index,
                       int role) const
{
    return index.isValid() ? entry(index)->data(role) : QVariant();
}

/*!
    Returns maximum anount of data allowed for model
    \retval maximum data count
*/

int TsModel::maxRowCount()const
{
    return mMaxItems;
}

/*!
    Activate one of model entries
*/
void TsModel::openApplication(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    entry(index)->open();
}

/*!
    Close one of moder entries
*/
void TsModel::closeApplication(const QModelIndex &index)
{
    if (!index.isValid() || !entry(index)->data(TsDataRoles::Closable).toBool()) {
        return;
    }
    entry(index)->close();
}

/*!
    Updates model with fresh entries
*/
void TsModel::updateModel()
{
    //clear current data
    qDeleteAll(mEntries);
    mEntries.clear();

    beginResetModel();
    getApplications();
    getActivities();
    endResetModel();

}

/*!
    Read list of running applications
*/
void TsModel::getApplications()
{
    //get running applications
    TsModelItem *entry(0);
    QList< QSharedPointer<CaEntry> > applications(mApplicationService.getEntries(mQuery));
    foreach(QSharedPointer<CaEntry> application, applications) {
        //CA entry ownership is transfered to model item
        entry = new TsEntryModelItem(mApplicationService, application, mSize);
        if (entry) {
            //add running application filtering
            if (entry->data(TsDataRoles::Closable).toBool() && //running application filtering
                    entry->data(TsDataRoles::Visible).toBool()) { //visible applications filtering
                mEntries.append(entry);
            } else {
                delete entry;
            }
        }
    }
}

/*!
    Read current activities
*/
void TsModel::getActivities()
{
    //get activities
    TsModelItem *entry(0);
    QList<QVariantHash> activities;
    QMetaObject::invokeMethod(&mActivityService, "activitiesList", Q_RETURN_ARG(QList<QVariantHash>, activities));
    foreach(QVariantHash activity, activities) {
        prepareActivityEntry(activity);
        entry = new TsActivityModelItem(*this, mActivityService, activity);
        if (entry) {
			if (maxRowCount() <= mEntries.count()) {
				break;
			}
            if (entry->data(TsDataRoles::Visible).toBool()) { //visible activity filtering
                mEntries.append(entry);
            } else {
                delete entry;
            }
        }
    }
}

/*!
    Modify activity entry replacing application id with name
*/
void TsModel::prepareActivityEntry(QVariantHash &activity)
{
    activity.insert(TsActivityModelItem::applicationKeyword(),
                    activity.find(ActivityActivityKeyword) == activity.end() ?
                    QString::null :
                    getApplicationName(activity[ActivityApplicationKeyword].toInt()));
}

/*!
    Return application name
    \param id - reqiested application identyfier
*/
QString TsModel::getApplicationName(int id)
{
    CaQuery query;
    QString retVal;
    query.setEntryRoles(ItemEntryRole);
    query.addEntryTypeName(entryTypeKeyword);
    query.setAttribute(applicationUidKeyword, QString::number(id));
    QList< QSharedPointer<CaEntry> > applications(mApplicationService.getEntries(query));
    if (applications.begin() != applications.end()) {
        retVal = (*applications.begin())->text();
    }
    return retVal;
}

/*!
    Called when some entry was changed
    \param updatedEntry entry that was changed
    \param change change type
*/
void TsModel::entryChanged(CaEntry updatedEntry,
                           ChangeType change)
{
    Q_UNUSED(updatedEntry);
    Q_UNUSED(change)
    updateModel();
}

/*!
    Called when some item was changed
    \param itemPtr - address of updated item
*/
void TsModel::entryChanged(TsModelItem *itemPtr)
{
    QList<TsModelItem *>::const_iterator iter(mEntries.constBegin());
    for (int offset(0); iter != mEntries.constEnd(); ++iter, ++offset) {
        if ((*iter) == itemPtr) {
            emit dataChanged(index(offset, 0), index(offset, 0));
            break;
        }
    }
}

/*!
    Returns an entry from model
    \param index of entry in model
    \retval pointer to an entry
*/
TsModelItem *TsModel::entry(const QModelIndex &index) const
{
    return mEntries.at(index.row());
}

