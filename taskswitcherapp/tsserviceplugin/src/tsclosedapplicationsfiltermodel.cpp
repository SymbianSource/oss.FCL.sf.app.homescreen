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
#include "tsclosedapplicationsfiltermodel.h"
#include "tsdataroles.h"

/*!
    Constructor
    \param pointer to parent object
*/
TsClosedApplicationsFilterModel::TsClosedApplicationsFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

/*!
    Add id that should be filtered
    \param id that should be added to list
*/
void TsClosedApplicationsFilterModel::addId(int id)
{
    mClosedIds.append(id);
    invalidateFilter();
}

/*!
    Clear filtered ids list
*/
void TsClosedApplicationsFilterModel::clearIds()
{
    mClosedIds.clear();
    invalidateFilter();
}

/*!
    Function to compare items
    \param sourceRow row to check
    \param sourceParent parent of source index
    \return true if row is accepted
*/
bool TsClosedApplicationsFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    bool res = true;
    if (mClosedIds.contains(sourceModel()->data(sourceModel()->index(sourceRow,0), TsDataRoles::EntryId).toInt())) {
        res = false;
    }
    return res;
}
