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
#include <QDebug>
#include "hsipcchannelhost.h"

/*!
    \class HsIpcChannelHost
    \ingroup group_hsutils
    \brief Homescreen ipc transport channel server api.
    
    Server side communication channel management. 

*/

HsIpcChannelHost::HsIpcChannelHost(QLocalSocket *socket,QObject *parent)
    : HsIpcChannel(parent)
{
    Q_ASSERT(socket);

    mSocket = socket;
    connect(mSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), 
            SLOT(onError(QLocalSocket::LocalSocketError)));
    connect(mSocket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(mSocket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    qDebug()<< "HsIpcChannelHost::HsIpcChannelHost(): " << (int)this;
}

HsIpcChannelHost::~HsIpcChannelHost()
{
    qDebug()<< "HsIpcChannelHost::~HsIpcChannelHost(): " << (int)this;
    delete mSocket;
}

void HsIpcChannelHost::closeConnection()
{
    mSocket->close();
}