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
#ifndef TSITEMPROVIDERINTERFACE_H
#define TSITEMPROVIDERINTERFACE_H

#include <QObject>

class QAbstractItemModel;
class QModelIndex;

class TsItemProviderInterface : public QObject
{

    Q_OBJECT

public:
    TsItemProviderInterface(QObject *parent = 0) : QObject(parent) {}
    virtual ~TsItemProviderInterface() {};

public:
    Q_INVOKABLE virtual QAbstractItemModel *model() = 0;

public slots:
    virtual void openApplication(const QModelIndex &index) = 0;
    virtual void closeApplication(const QModelIndex &index) = 0;
    virtual void closeAllApplications() = 0;
    virtual void clearClosedApplicationList() = 0;

};

#endif // TSITEMPROVIDERINTERFACE_H
