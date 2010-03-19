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
#ifndef TSITEMPROVIDER_H
#define TSITEMPROVIDER_H

#include "tsitemproviderinterface.h"

#include <QSharedPointer>

class TsClosedApplicationsFilterModel;
class CaService;

class TsItemProvider : public TsItemProviderInterface
{

    Q_OBJECT

public:
    TsItemProvider(QObject *parent = 0);
    virtual ~TsItemProvider();

public:
    virtual QAbstractItemModel *model();

public slots:
    virtual void openApplication(const QModelIndex &index);
    virtual void closeApplication(const QModelIndex &index);
    virtual void closeAllApplications();
    virtual void clearClosedApplicationList();

private:
    TsClosedApplicationsFilterModel *mModel;
    QSharedPointer<CaService> mService;

};

#endif // TSITEMPROVIDER_H
