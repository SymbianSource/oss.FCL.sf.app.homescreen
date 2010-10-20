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

#ifndef HSDBUPDATETHREAD_H
#define HSDBUPDATETHREAD_H

#include <QThread>
#include <QVariantHash>

#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsDbUpdateThread : public QThread
{
    Q_OBJECT
    
public:
    HsDbUpdateThread(const QString &connectionName, const QString &databaseName, QObject *parent = 0);
    ~HsDbUpdateThread();
public:
    static HsDbUpdateThread *instance();
    static void setInstance(HsDbUpdateThread *instance);

signals:
    void updateWidgetZValues(const QHash<int, qreal> &data, Qt::Orientation orientation);
public slots:
     
    void slotUpdateWidgetZValues(const QHash<int, qreal> &data, Qt::Orientation orientation);

protected:
    void run();

private:    
    Q_DISABLE_COPY(HsDbUpdateThread)

private:
    QString mConnectionName;
    QString mDatabaseName;
    static HsDbUpdateThread *mInstance;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSDBUPDATETHREAD_H
