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
* Description: Default runtime provider.
*
*/

#ifndef SNSRDEFAULTRUNTIMEPROVIDER_H
#define SNSRDEFAULTRUNTIMEPROVIDER_H

#include <hsiruntimeprovider.h>

class SnsrDefaultRuntimeProvider : public QObject, 
    public IHsRuntimeProvider
{

    Q_OBJECT
    Q_INTERFACES(IHsRuntimeProvider)

public:

    SnsrDefaultRuntimeProvider(QObject *parent = 0);
    ~SnsrDefaultRuntimeProvider();

    QList<HsRuntimeToken> runtimes();
    HsRuntime *createRuntime(const HsRuntimeToken& token);

};

#endif // SNSRDEFAULTRUNTIMEPROVIDER_H
