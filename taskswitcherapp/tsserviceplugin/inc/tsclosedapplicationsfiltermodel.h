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

#ifndef TSCLOSEDAPPLICATIONSFILTERMODEL_H
#define TSCLOSEDAPPLICATIONSFILTERMODEL_H

#include <QSortFilterProxyModel>

/**
 * @ingroup group_tsserviceprovider
 * @brief TsClosedApplicationsFilterModel filtering model to hide closed apps.
 *
 * Filter closed apps by task switcher from currently presented.
 *
 * @lib tsserviceprovider.lib
 * @since S60 ?S60_version
 */
class TsClosedApplicationsFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TsClosedApplicationsFilterModel(QObject *parent = 0);

public:
    void addId(int id);
    void clearIds();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    /*
     * List containing result
     */
    QList<int> mClosedIds;
};

#endif // TSCLOSEDAPPLICATIONSFILTERMODEL_H
