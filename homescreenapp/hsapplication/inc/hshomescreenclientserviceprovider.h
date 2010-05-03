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

#ifndef HSHOMESCREENCLIENTSERVICEPROVIDER_H
#define HSHOMESCREENCLIENTSERVICEPROVIDER_H

#include <QVariant>

#include <xqserviceprovider.h>

class HsHomeScreenClientServiceProvider : public XQServiceProvider
{
    Q_OBJECT

public:
    HsHomeScreenClientServiceProvider(QObject *parent = 0);
    ~HsHomeScreenClientServiceProvider();
    
public slots:
    bool addWidget(const QString &uri,const QVariantHash &preferences);
    bool setWallpaper(const QString &fileName);
};

#endif // HSHOMESCREENCLIENTSERVICEPROVIDER_H
