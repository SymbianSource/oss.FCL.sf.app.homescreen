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
#include "hsipcchannelclient.h"

/*!
    \class HsIpcChannelClient
    \ingroup group_hsutils
    \brief Homescreen ipc transport channel client api.
    
    Client side communication channel management. 

*/
HsIpcChannelClient::HsIpcChannelClient(QObject *parent)
: HsIpcChannel(parent)
{
}

HsIpcChannelClient::~HsIpcChannelClient()
{
    delete mSocket;
}

bool HsIpcChannelClient::openConnection(const QString &serverName)
{
    closeConnection();
    delete mSocket;
    mSocket = NULL;
    mSocket = new QLocalSocket;
    connect(mSocket, SIGNAL(connected()), SLOT(onConnected()));
    connect(mSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), 
            SLOT(onError(QLocalSocket::LocalSocketError)));

    mSocket->connectToServer(serverName);
    
    return true;
}
bool HsIpcChannelClient::waitForOpenConnection(const QString &serverName)
{
    delete mSocket;
    mSocket = NULL;
    mSocket = new QLocalSocket;
    connect(mSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), 
            SLOT(onError(QLocalSocket::LocalSocketError)));

    mSocket->connectToServer(serverName);
    return mSocket->waitForConnected(1000);
}

void HsIpcChannelClient::closeConnection()
{
    Q_ASSERT(mSocket);
    mSocket->disconnectFromServer();
}

bool HsIpcChannelClient::waitForCloseConnection()
{
    Q_ASSERT(mSocket);
    mSocket->disconnectFromServer();
    return mSocket->waitForDisconnected(1000);
}



void HsIpcChannelClient::onConnected()
{
    disconnect(mSocket,SIGNAL(connected()),this, SLOT(onConnected()) );
  
    connect(mSocket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(mSocket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    emit connectionEstablished();
}
