/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Homescreen client api
*
*/
#include "hshomescreenclient.h"
#include <xqservicerequest.h>
#include <QEventLoop>

const char INTERFACE_NAME[] = "com.nokia.services.hsapplication.IHomeScreenClient";
HsHomescreenClient::HsHomescreenClient(QObject *parent)
    :QObject(parent),
    mAsyncRequest(0),
    mRequestResult(false)
{   
}

HsHomescreenClient::~HsHomescreenClient()
{
    delete mAsyncRequest;
}

bool HsHomescreenClient::addWidget(
    const QString &uri, 
    const QVariantHash &preferences)
{
    QEventLoop eventLoop;
    connect(this, SIGNAL(requestFinished()), &eventLoop, SLOT(quit()));
    QMetaObject::invokeMethod(this, "doAddWidget", Qt::QueuedConnection,
            Q_ARG(QString,uri),Q_ARG(QVariantHash,preferences));
    eventLoop.exec();
   
    return mRequestResult;
}
    
void HsHomescreenClient::onRequestCompleted(const QVariant &result)
{
    mRequestResult = result.toBool();
    emit requestFinished();
}
    
void HsHomescreenClient::onRequestError(int error)
{
    Q_UNUSED(error)
    emit requestFinished();
}
    
void HsHomescreenClient::doAddWidget(
    const QString &uri, 
    const QVariantHash &preferences)
{
    delete mAsyncRequest;
    mAsyncRequest = 0;
    mAsyncRequest = new XQServiceRequest(INTERFACE_NAME,
                       "addWidget(QString,QVariantHash)",false);
    *mAsyncRequest << uri;
    *mAsyncRequest << preferences;
    
    connect(mAsyncRequest, SIGNAL(requestCompleted(QVariant)), 
           SLOT(onRequestCompleted(QVariant)));
    connect(mAsyncRequest, SIGNAL(requestError(int)), 
              SLOT(onRequestError(int)));
       
    mRequestResult = false;
    bool res=mAsyncRequest->send();
    if(!res){
       emit requestFinished();
    }
    
}
