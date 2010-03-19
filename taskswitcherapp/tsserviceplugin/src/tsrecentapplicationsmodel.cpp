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
* Description: recentapplicationsmodel.cpp
*
*/
#include "tsrecentapplicationsmodel.h"

#include <HbIcon>
#include <cadefs.h>
#include <caservice.h>
#include <canotifier.h>
#include <canotifierfilter.h>

#include "tsdataroles.h"

/*!
    Constructor
    \param query used to create model
    \param pointer to parent object
*/
TsRecentApplicationsModel::TsRecentApplicationsModel(QObject *parent) :
    QAbstractListModel(parent),
    mEntries(),
    mService(CaService::instance()),
    mNotifier(NULL),
    mSize(240, 240)
{
    // creating query
    mQuery.setEntryRoles(ItemEntryRole);
    mQuery.addEntryTypeName(QString("application"));
    mQuery.setFlagsOn(VisibleEntryFlag);
    mQuery.setFlagsOff(MissingEntryFlag);
    mQuery.setFlagsOn(UsedEntryFlag);
    mQuery.setSort(LastUsedSortAttribute, Qt::DescendingOrder);
    // requesting new data
    updateModel();
    // creating notyfication filter
    CaNotifierFilter filter(mQuery);
    mNotifier = mService->createNotifier(filter);
    connect(mNotifier, SIGNAL(entryChanged(CaEntry,ChangeType)), this, SLOT(entryChanged(CaEntry, ChangeType)));
    connect(mNotifier, SIGNAL(entryTouched(int)), this, SLOT(updateModel()));
    if (mQuery.parentId() > 0) {
        connect(mNotifier, SIGNAL(groupContentChanged(int)), this, SLOT(updateModel()));
    }
}

/*!
    Destructor
*/
TsRecentApplicationsModel::~TsRecentApplicationsModel()
{
    qDeleteAll(mEntries);
    delete mNotifier;
}

/*!
    Returns count of rows in model
    \retval number of rows
*/
int TsRecentApplicationsModel::rowCount(
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
QVariant TsRecentApplicationsModel::data(const QModelIndex &index,
        int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return QVariant(entry(index)->text());
        case Qt::DecorationRole:
            return QVariant(entry(index)->makeIcon(mSize));
        case TsDataRoles::Closable:
            return QVariant(closeable(index));
        case TsDataRoles::EntryId:
            return entryId(index);
        default:
            return QVariant(QVariant::Invalid);
    }
}

/*!
    Updates model with fresh entries
*/
void TsRecentApplicationsModel::updateModel()
{
    qDeleteAll(mEntries);
    mEntries.clear();
    mEntries = mService->getEntries(mQuery);
    reset();
}

/*!
    Called when some entry was changed
    \param updatedEntry entry that was changed
    \param change change type
*/
void TsRecentApplicationsModel::entryChanged(CaEntry updatedEntry,
        ChangeType change)
{
    Q_UNUSED(updatedEntry);
    Q_UNUSED(change)
    updateModel();
}

/*!
    Returns an entry from model
    \param index of entry in model
    \retval pointer to an entry
*/
CaEntry *TsRecentApplicationsModel::entry(const QModelIndex &index) const
{
    return mEntries.at(index.row());
}

/*!
    Allows to check if item is closable
    \param index item index to be checked
    \retval true if item is closable
*/
bool TsRecentApplicationsModel::closeable(const QModelIndex &index) const
{
    CaEntry *caEntry = entry(index);
    return (caEntry && caEntry->flags().testFlag(RunningEntryFlag) &&
            !caEntry->flags().testFlag(SystemEntryFlag));
}

/*!
    Allows to check entry id
    \param index item index to be checked
    \param entry id
*/
QVariant TsRecentApplicationsModel::entryId(const QModelIndex &index) const
{
    QVariant res;
    CaEntry *caEntry = entry(index);
    if (caEntry) {
        res = QVariant(caEntry->id());
    }
    return res;
}
