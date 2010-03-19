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
#include "hshomescreenclient.h"
#include <xqservicerequest.h>



HsContentPublish::HsContentPublish(QObject *parent)
    :QObject(parent)
{
 
}
HsContentPublish::~HsContentPublish()
{

}

bool HsContentPublish::open()
{
  
    return true;
}

bool HsContentPublish::close()
{
  
    return true;
}

bool HsContentPublish::addWidgetToHomescreen(
    const QString &uri, 
    const QVariantMap &preferences)
{
    XQServiceRequest snd("com.nokia.services.hshomescreenclient",
                        "bool addWidget(QString, QVariantMap)");
    snd << uri;
    snd << preferences;
    QVariant retValue;
    bool res=snd.send(retValue);
    if (!res) {
        int returnvalue = snd.latestError();
    }
    return res;
}


