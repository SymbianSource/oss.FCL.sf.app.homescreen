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

#include <QDataStream>
#include <QDebug> 
#include "hsipcchannelclient.h"

/*!
    \class HsIpcChannel
    \ingroup group_hsutils
    \brief Homescreen ipc transport channel.
    
    Responsible for reading and writing ipc messages. Uses QLocalSocket for transport 
    mechanism. 

*/
HsIpcChannel::~HsIpcChannel()
{
}

bool HsIpcChannel::isConnected() const
{
    if(!mSocket)return false;
    return mSocket->state() == QLocalSocket::ConnectedState;
}

bool HsIpcChannel::sendMessage(const QVariantMap &message)
{
    if (!isConnected()) {
        return false;
    }

    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint32)0; //fill size placeholder
    out << message;
    out.device()->seek(0); // now write right value to it
    out << (quint32)(byteArray.size() - sizeof(quint32));

    mSocket->write(byteArray);

    return true;
}

bool HsIpcChannel::waitForSendMessage(const QVariantMap &message)
{
    if (!isConnected()) {
        return false;
    }
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint32)0; //fill size placeholder
    out << message;
    out.device()->seek(0); // now write right value to it
    out << (quint32)(byteArray.size() - sizeof(quint32));

    mSocket->write(byteArray);
    bool ok = true;
    while(mSocket->bytesToWrite() && ok){
        ok = mSocket->waitForBytesWritten(1000);// timeout in 1sec
    }
    
    return ok;  
}

bool HsIpcChannel::waitForReadMessage(QVariantMap &message)
{
    if (!isConnected()) {
        return false;
    }
   
    QDataStream in(mSocket);
    in.setVersion(QDataStream::Qt_4_6);
    quint32 blockSize = 0;
    bool ok = mSocket->waitForReadyRead();
    while( ok && mSocket->bytesAvailable() < (int)sizeof(quint32) ){
        ok = mSocket->waitForReadyRead(1000);
    }
    if(!ok){
        return false;
    }
    in >> blockSize;

    while(ok && mSocket->bytesAvailable() < blockSize){
        ok = mSocket->waitForReadyRead(1000);
    }
    
    if(!ok){
        return false;
    }
    
    // now we have all data
    message.clear();
    in >> message;
    return true;
}

void HsIpcChannel::onReadyRead()
{
    QDataStream in(mSocket);
    in.setVersion(QDataStream::Qt_4_6);

    if (mBlockSize == 0) {
        if (mSocket->bytesAvailable() < (int)sizeof(quint32)){
            return;
        }
        in >> mBlockSize;
    } 

    if (mSocket->bytesAvailable() < mBlockSize){
        return;
    }
    mBlockSize = 0;
    // now we have all data
    QVariantMap message;
    in >> message;
    
    emit messageReceived(message);
}

void HsIpcChannel::onError(QLocalSocket::LocalSocketError socketError)
{
    Q_UNUSED(socketError);
    emit connectionError();
}



HsIpcChannel::HsIpcChannel(QObject *parent) 
    : QObject(parent),mBlockSize(0),mSocket(0)
{
    
}
