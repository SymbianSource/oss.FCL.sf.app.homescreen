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

#include "tsitemprovider.h"

#include <QModelIndex>
#include <QAbstractItemModel>

#include <caservice.h>

#include "tsrecentapplicationsmodel.h"
#include "tsclosedapplicationsfiltermodel.h"
#include "tsdataroles.h"

/*!
    \class TsItemProvider
    \ingroup group_tsserviceplugin
    \brief Returns items that should be presented in TS.
	
	Service providing information about items that should be presented in TS. It also allows to
	start some action on it (open/close).
*/

TsItemProvider::TsItemProvider(QObject *parent) : TsItemProviderInterface(parent), mService(CaService::instance())
{
    TsRecentApplicationsModel *recentAppModel = new TsRecentApplicationsModel(this);
    mModel = new TsClosedApplicationsFilterModel(this);
    mModel->setSourceModel(recentAppModel);
}

TsItemProvider::~TsItemProvider()
{
}

QAbstractItemModel *TsItemProvider::model()
{
    return mModel;
}

void TsItemProvider::openApplication(const QModelIndex &index)
{
    QVariant entryId = index.data(TsDataRoles::EntryId);
    if (entryId.isValid()) {
        mService->executeCommand(entryId.toInt(), QString("open"));
    }
}

void TsItemProvider::closeApplication(const QModelIndex &index)
{
    if (index.data(TsDataRoles::Closable).toBool()) {
        QVariant entryId = index.data(TsDataRoles::EntryId);
        if (entryId.isValid()) {
            mModel->addId(entryId.toInt());
            mService->executeCommand(entryId.toInt(), QString("close"));
        }
    }
}

void TsItemProvider::closeAllApplications()
{
    QList<int> closableList;
    for (int row(0); row < mModel->rowCount(); ++row) {
        if (mModel->index(row, 0).data(TsDataRoles::Closable).toBool()) {
            QVariant entryId = mModel->index(row, 0).data(TsDataRoles::EntryId);
            if (entryId.isValid()) {
                closableList.append(entryId.toInt());
            }
        }
    }
    foreach (int entryId, closableList) {
        mModel->addId(entryId);
        mService->executeCommand(entryId, QString("close"));        
    }
}

void TsItemProvider::clearClosedApplicationList()
{
    mModel->clearIds();
}
