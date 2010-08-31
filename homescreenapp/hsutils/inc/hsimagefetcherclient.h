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
* Description:
*
*/

#ifndef HSIMAGEFETCHERCLIENT_H
#define HSIMAGEFETCHERCLIENT_H

#include <QObject>

#ifdef Q_OS_SYMBIAN
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#endif // Q_OS_SYMBIAN

#include "hsutils_global.h"

class HSUTILS_EXPORT HsImageFetcherClient : public QObject
{
    Q_OBJECT
    
public:
    HsImageFetcherClient(QObject *parent = 0);
    ~HsImageFetcherClient();
    
public slots:
    void fetch();
    
private slots: 
    // Aiw request responses
    void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString &errorMessage);

signals:
    void fetchCompleted(const QString &imageFile);
    void fetchFailed(int errorCode, const QString &errorMessage);

private:
    Q_DISABLE_COPY(HsImageFetcherClient)

#ifdef Q_OS_SYMBIAN
    XQApplicationManager mAppMgr;                
    XQAiwRequest *mReq;
#endif // Q_OS_SYMBIAN
};

#endif // HSIMAGEFETCHERCLIENT_H
