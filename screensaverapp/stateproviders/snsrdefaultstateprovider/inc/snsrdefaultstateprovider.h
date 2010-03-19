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
* Description: Default Screensaver provider.
*
*/

#ifndef SNSRDEFAULTSTATEPROVIDER_H
#define SNSRDEFAULTSTATEPROVIDER_H

#include <hsistateprovider.h>

class QState;

class SnsrDefaultStateProvider : public QObject,
    public IHsStateProvider
{

    Q_OBJECT
    Q_INTERFACES(IHsStateProvider)

public:

    SnsrDefaultStateProvider();
    virtual ~SnsrDefaultStateProvider();

    QList<HsStateToken> states();
    QState *createState(const HsStateToken &token);

private:

    HsStateToken mSnsrRootStateToken;

};

#endif // SNSRDEFAULTSTATEPROVIDER_H
