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
* Description:  Content service.
*
*/

#ifndef HSCONTENTSERVICE_H
#define HSCONTENTSERVICE_H

#include <QObject>
#include <QMetaType>
#include <QVariant>

#include "hsdomainmodel_global.h"

class HsWidgetHost;

class HSDOMAINMODEL_EXPORT HsContentService : public QObject
{
    Q_OBJECT

public:
    HsContentService(QObject *parent = 0);
    ~HsContentService();

    bool createWidget(const QVariantHash &params);
    HsWidgetHost *createWidgetForPreview(const QVariantHash &params);

    bool addWidget(const QString &uri, const QVariantHash &preferences);

    static HsContentService *instance();

private:
    static QScopedPointer<HsContentService> mInstance;

    Q_DISABLE_COPY(HsContentService)
};

Q_DECLARE_METATYPE(HsContentService*)

#endif
