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
* Description: recentapplicationsmodel.h
*
*/

#ifndef TSRECENTAPPLICATIONSMODEL_H
#define TSRECENTAPPLICATIONSMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QSize>
#include <QSharedPointer>
#include <caquery.h>
#include <caentry.h>

class CaService;
class CaNotifier;

class TsRecentApplicationsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TsRecentApplicationsModel(QObject *parent = 0);
    ~TsRecentApplicationsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void updateModel();
    void entryChanged(CaEntry, ChangeType);

private:
    bool closeable(const QModelIndex &index) const;
    QVariant entryId(const QModelIndex &index) const;
    CaEntry *entry(const QModelIndex &index) const;

public:
    /*
     * List containing result
     */
    QList<CaEntry *> mEntries;

    /*
     * Query
     */
    CaQuery mQuery;

    /*
     * Content Arsenal Service
     */
    QSharedPointer<CaService> mService;

    /*
     * Content Arsenal Notifier
     */
    CaNotifier *mNotifier;

    /*
     * Icon size in model
     */
    QSize mSize;
};

#endif // TSRECENTAPPLICATIONSMODEL_H
