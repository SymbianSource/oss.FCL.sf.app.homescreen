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
* Description:  Default implementation of the home screen runtime.
*
*/

#ifndef HSCONTENTPUBLISH_H
#define HSCONTENTPUBLISH_H

#include <QObject>
#include <QVariant>


class XQServiceRequest;

class HsContentPublish : public QObject
{
    Q_OBJECT

public:
    HsContentPublish(QObject *parent = 0);
    virtual ~HsContentPublish();
    Q_INVOKABLE bool open();
    Q_INVOKABLE bool close();
public slots:
    bool addWidgetToHomescreen(const QString &uri, const QVariantMap &preferences = QVariantMap());

private:
    XQServiceRequest* sndAsync;
};

#endif
