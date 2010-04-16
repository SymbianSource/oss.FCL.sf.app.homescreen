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
* Description: Example of home screen content publishing client
*
*/

#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <QMetaObject>
#include <QMetaMethod>

#include "contentpublishclient.h"

/*!
    \ingroup group_content_publish_client
    \class ContentPublishClient
    \brief Example implementation for home screen content publish client.

    ContentPublishClient is derived from QObject and implements 
    needed functions for the home screen content publish client. 
*/

/*!
    Constructor.
*/
ContentPublishClient::ContentPublishClient(QServiceManager &manager, QObject *parent)
    : QObject(parent),
      mManager(&manager)
{
}

/*!
    Destructor
*/
ContentPublishClient::~ContentPublishClient()
{
}

/*!
    Loads service interface
*/
bool ContentPublishClient::load()
{    
    QServiceFilter filter("com.nokia.symbian.IHomeScreenClient");
    filter.setServiceName("hshomescreenclientplugin");
    QList<QServiceInterfaceDescriptor> interfaces = mManager->findInterfaces(filter);

    if(interfaces.isEmpty()) {
        QServiceManager::Error error = mManager->error();
        return false;
    }
    qDebug() << interfaces.first().interfaceName()
             << interfaces.first().serviceName()
             <<  interfaces.first().isValid();

    mService = mManager->loadInterface(interfaces.first());
        
    return (mService);
}

/*!
    Adds widget utilizing service interface and invoke call
*/
void ContentPublishClient::addWidget()
{   
    QByteArray signature = QMetaObject::normalizedSignature("addWidget(QString,QVariantHash)");
    int methodIndex = mService->metaObject()->indexOfMethod(signature);   
    QMetaMethod method = mService->metaObject()->method(methodIndex);
    bool retVal(false);

    QString widget = "hsclockwidgetplugin";
    
    bool ret = method.invoke( mService,
                              Qt::DirectConnection,
                              Q_RETURN_ARG(bool, retVal),
                              Q_ARG(QString,widget),
                              Q_ARG(QVariantHash,QVariantHash()));
                    
    if(!ret){
        qDebug()<< "method invoke failed!";
    }
    if(!retVal){
        qDebug() << "addWidget() failed!!";
    }
}
