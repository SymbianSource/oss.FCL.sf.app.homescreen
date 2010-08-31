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
* Description:  Photos image fetcher client.
*
*/


#include "hsimagefetcherclient.h"

#include <QDir>
#include <QDebug>
#include <QVariant>
#ifdef Q_OS_SYMBIAN
#include <xqaiwinterfacedescriptor.h>
#include <xqaiwdecl.h>
#endif // Q_OS_SYMBIAN

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT


/*!
    \class HsImageFetcherClient
    \ingroup group_hsutils
    \brief 
*/

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef Q_OS_SYMBIAN
HsImageFetcherClient::HsImageFetcherClient(QObject *parent)
  : QObject(parent),
    mReq(0)
{
}
#else
HsImageFetcherClient::HsImageFetcherClient(QObject *parent)
  : QObject(parent)
{
}
#endif // Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsImageFetcherClient::~HsImageFetcherClient()
{
#ifdef Q_OS_SYMBIAN
    delete mReq;
#endif // Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsImageFetcherClient::fetch()
{
#ifdef Q_OS_SYMBIAN
    qDebug() << "HsImageFetcherClient::fetch START";

    if (mReq) {
        delete mReq;
        mReq = 0;
    }

    mReq = mAppMgr.create(XQI_IMAGE_FETCH, XQOP_IMAGE_FETCH, false);
    if (mReq) {        
        connect(mReq, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
        connect(mReq, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));
    } else {
        qCritical() << "HsImageFetcherClient::fetch -> Create request failed";
        return;
    }

    if (!(mReq)->send()) {
        qCritical() << "HsImageFetcherClient::fetch -> Send failed" << mReq->lastErrorMessage();;
    }

    qDebug() << "HsImageFetcherClient::fetch END";
#endif // Q_OS_SYMBIAN
}



// Aiw request responses
void HsImageFetcherClient::handleOk(const QVariant &result)
{
#ifdef Q_OS_SYMBIAN
    // disconnect error signal as it will give -4999 (EConnectionClosed) error afterwards even though image
    // is fetched successfully
    disconnect(mReq, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

    XQAiwRequest *request = static_cast<XQAiwRequest *>(sender());
    int implementationId=-1;
    implementationId = (request->descriptor().property(XQAiwInterfaceDescriptor::ImplementationId)).toInt();

    if (result.canConvert<QString>()) {
        qDebug("HsImageFetcherClient::%x:handleOk result=%s,%s",
               implementationId,
               result.typeName(),
               qPrintable(result.value<QString>()));

        emit fetchCompleted(result.value<QString>());
    } else if (result.canConvert<QStringList>()) {
        QStringList resultList = result.value<QStringList>();
	    if (resultList.size() > 0) {
	        //for debug
	        QString msg = QString("HsImageFetcherClient::handleOk, size=")+QString::number( resultList.size() )+QString("\n");
	        for (int i(0), size(resultList.size()); i < size; ++i) {
                msg += QString::number(i) + QString(":") + resultList[i] + QString("\n");
	        }
	        qDebug()<<msg;
		}
        emit fetchCompleted(resultList[0]);
	} else {
        qCritical("AppMgrClient::%x:handleOk result=%s. Not displayable",
            implementationId,
            result.typeName());
        emit fetchFailed(-1, QString());
    }
#else
	Q_UNUSED(result)
#endif // Q_OS_SYMBIAN
}

void HsImageFetcherClient::handleError(int errorCode, const QString &errorMessage)
{
#ifdef Q_OS_SYMBIAN
    XQAiwRequest *request = static_cast<XQAiwRequest *>(sender());
    int implementationId = -1;
    implementationId = (request->descriptor().property(XQAiwInterfaceDescriptor::ImplementationId)).toInt();

    qCritical("HsImageFetcherClient::%x:handleError code=%d, errorMessage:%s",
        implementationId, errorCode, qPrintable(errorMessage));
    emit fetchFailed(errorCode, errorMessage);
#else
	Q_UNUSED(errorCode)
	Q_UNUSED(errorMessage)
#endif // Q_OS_SYMBIAN
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
