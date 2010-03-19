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
#include "hsipcconnectiondispatcher.h"
#include "hscontentpublishhandler.h"
#include "hsipcserver.h"
#include "hsipcchannelhost.h"

/*!
    \class HsIpcConnectionDispatcher
    \ingroup group_hsutils
    \brief Dispatch ipc connections.
    
    Creates homescreen ipc communication server and dispatch ipc connections 
    asynchronously for connection handlers. 

*/

HsIpcConnectionDispatcher::HsIpcConnectionDispatcher(
    const QString &connectionName,QObject *parent)
    : QObject(parent),
    mConnectionName(connectionName),
    mIpcServer(new HsIpcServer())
{
    mIpcServer->setParent(this);

}

HsIpcConnectionDispatcher::~HsIpcConnectionDispatcher()
{
   
}

bool HsIpcConnectionDispatcher::start()
{
    if(!mIpcServer->startServer(mConnectionName)){
        return false;
    }
    connect(mIpcServer, SIGNAL(newConnection()),
        SLOT(onConnectionEstablished()));
    return true;
}
void HsIpcConnectionDispatcher::stop()
{
    mIpcServer->stopServer();
}

void HsIpcConnectionDispatcher::onConnectionEstablished()
{
    QScopedPointer<HsIpcChannelHost> newChannel(mIpcServer->takeConnection());
    new HsContentPublishHandler(newChannel.data());
    newChannel.take();
}

