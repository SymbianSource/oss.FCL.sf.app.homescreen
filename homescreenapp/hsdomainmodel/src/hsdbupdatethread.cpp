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

#include "hsdbupdatethread.h"
#include "hsdatabase.h"
#include "hsdomainmodeldatastructures.h"


HsDbUpdateThread *HsDbUpdateThread::mInstance = 0;

/*!
    \class HsDbUpdateThread
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!
    Constructs a new asynchronous database object with the given \a connectionName, 
    \a databaseName and a \a parent object.
*/
HsDbUpdateThread::HsDbUpdateThread(
    const QString &connectionName, 
    const QString &databaseName, 
    QObject *parent):
    QThread(parent),mConnectionName(connectionName),mDatabaseName(databaseName)
{
    
}
/*!
    Destructor
*/
HsDbUpdateThread::~HsDbUpdateThread()
{
    quit();
}
/*!
    Destructor
*/
HsDbUpdateThread *HsDbUpdateThread::instance()
{
    return mInstance;
}
/*!
    Set \a instance in place.
*/
void HsDbUpdateThread::setInstance(HsDbUpdateThread *instance)
{
    delete mInstance;
    mInstance = instance;
}
/*!
    Update given \a data widgets zvalues for passed \a orientation
*/
void HsDbUpdateThread::slotUpdateWidgetZValues(const QHash<int, qreal> &data, Qt::Orientation orientation)
{
    emit updateWidgetZValues(data,orientation);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!
    Initialize database
*/
void HsDbUpdateThread::run()
{
    HsDatabase db;
    db.setConnectionName(mConnectionName);
    db.setDatabaseName(mDatabaseName);
    db.open();
    qRegisterMetaType< QHash<int, qreal> >("QHash<int, qreal>");
    qRegisterMetaType< Qt::Orientation >("Qt::Orientation");
    connect(this, SIGNAL(updateWidgetZValues(QHash<int, qreal>, Qt::Orientation)),
        &db, SLOT(updateWidgetZValues(QHash<int, qreal>, Qt::Orientation)));
    
    exec();
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


