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

#include <QLocalServer>
#include <QDataStream>
#include <QDebug>
#include "hsipcserver.h"
#include "hsipcchannelhost.h"

/*!
    \class HsIpcServer
    \ingroup group_hsutils
    \brief Homescreen ipc server api.
    
    Server to host ipc communication. Signals when new
    connection arrives.

*/

HsIpcServer::HsIpcServer(QObject *parent)
    : QObject(parent),
      mServer(0)
{
}

HsIpcServer::~HsIpcServer()
{
    stopServer();
}

bool HsIpcServer::startServer(const QString &serverName)
{
    stopServer();
    QLocalServer::removeServer(serverName);
    mServer = new QLocalServer;
    connect(mServer, SIGNAL(newConnection()), SLOT(onNewConnection()));
    if(!mServer->listen(serverName)){
        qDebug()<< "HsIpcServer::startServer listen failed";
        delete mServer;
        mServer = NULL;
        return false;
    }
    return true;
}


void HsIpcServer::stopServer()
{
    if (mServer) {
        mServer->close();
        delete mServer;
        mServer = NULL;
       
    }
    while(!mConnections.isEmpty()){
        delete mConnections.dequeue();    
        }
   
}
HsIpcChannelHost* HsIpcServer::takeConnection()
{
    if(!mConnections.isEmpty()){
        return mConnections.dequeue();
    }
    return 0;
}

void HsIpcServer::onNewConnection()
{
    QLocalSocket* socket = mServer->nextPendingConnection();
    if (socket) {
        qDebug() << "HsIpcServer::onNewConnection";
        QScopedPointer<HsIpcChannelHost> channel( new HsIpcChannelHost(socket) );
        mConnections.enqueue(channel.data());
        channel.take();
        emit newConnection();
    }
}
