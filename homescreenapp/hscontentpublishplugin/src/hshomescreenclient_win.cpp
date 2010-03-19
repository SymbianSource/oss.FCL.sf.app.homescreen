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
* Description:  Content publishing implementation
*
*/
#include <QDebug>
#include "hshomescreenclient.h"
#include "hsipcchannelclient.h"

const char SERVERNAME[] = "hs_content_publish";

HsContentPublish::HsContentPublish(QObject *parent)
    :QObject(parent),mChannel(new HsIpcChannelClient())
{
    mChannel->setParent(this);
 
}
HsContentPublish::~HsContentPublish()
{

}

bool HsContentPublish::open()
{
    if(!mChannel->isConnected() && !mChannel->waitForOpenConnection(SERVERNAME)){
        return false;
    }
    return true;
}

bool HsContentPublish::close()
{
    if( mChannel->isConnected() && !mChannel->waitForCloseConnection()){
        qDebug()<< "HsContentPublish connection failed";
        return false;
    }
    return true;
}

bool HsContentPublish::addWidgetToHomescreen(
    const QString &uri, 
    const QVariantMap &preferences)
{
    if(!open()){
        return false;
    }
    // format message
    QVariantMap message;
    message.insert("messageType",QString("addWidget"));
    message.insert("uri",uri);
    message.insert("preferences",preferences);
    if(!mChannel->waitForSendMessage(message)){
        qDebug()<< "HsContentPublish sendMessageWait failed";
        return false;
    }
    // get reply
    QVariantMap reply;
    if(!mChannel->waitForReadMessage(reply)){
        qDebug()<< "HsContentPublish reply wait failed";
        return false;
    }
    QString replyResult = reply.value("result").toString();
    if( replyResult != QLatin1String("true")){
        qDebug()<< "HsContentPublish reply result: " << replyResult;
        return false;
    }
   
    return true;
}

