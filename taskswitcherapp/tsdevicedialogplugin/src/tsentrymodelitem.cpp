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
* Description: tsentrymodelitem.cpp
*
*/
#include "tsentrymodelitem.h"
#include "tsdataroles.h"

#include <HbIcon>

/*!
    \class TsEntryModelItem
    \ingroup group_tsdevicedialogplugin
    \brief Item presenting running apps in the grid.
*/

/*!
    Standard C++ constructor
    /param service - reference to initialized Content Aresnal client instance
    /param entry - Content Arsenal data
    /param size - icon size
*/
TsEntryModelItem::TsEntryModelItem(CaService& service, QSharedPointer<CaEntry> entry, QSize size)
    :
    mService(service),
    mEntry(entry),
    mSize(size)
{
    //no implementation required
}

/*!
    Standard C++ destructor
*/
TsEntryModelItem::~TsEntryModelItem()
{
}

/*!
    Returns the data stored under the given role.
    /param role - requested data role
    /return data encapulated by QVariant
*/
QVariant TsEntryModelItem::data(int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QVariant(mEntry->text());
        case Qt::DecorationRole:
            return QVariant(mEntry->makeIcon(mSize));
        case TsDataRoles::Closable:
            return QVariant(closable());
        case TsDataRoles::Visible:
            return QVariant(true);
        default:
            return QVariant(QVariant::Invalid);
    }
}

/*!
    Close running application repesented by entry
*/
void TsEntryModelItem::close()
{
    mService.executeCommand(mEntry->id(), QString("close"));
}

/*!
    Open or move to foreground application repesented by entry
*/
void TsEntryModelItem::open()
{
    mService.executeCommand(mEntry->id(), QString("open"));
}

/*!
    Check if application represented by entry is running.
    /return true if application is running, false otherwise
*/
bool TsEntryModelItem::closable() const
{
    return (mEntry->flags().testFlag(RunningEntryFlag) &&
            !mEntry->flags().testFlag(SystemEntryFlag));
}
