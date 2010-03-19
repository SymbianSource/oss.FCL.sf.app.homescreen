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
* Description: 
*
*/
#ifndef TSACTIVATION_H
#define TSACTIVATION_H

#include "tsactivationinterface.h"

class TsActivation : public TsActivationInterface
{

    Q_OBJECT

public:
    TsActivation(QObject *parent = 0);

};

#endif // TSACTIVATION_H
