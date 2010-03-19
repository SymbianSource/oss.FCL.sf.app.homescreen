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
#ifndef TSACTIVATIONINTERFACE_H
#define TSACTIVATIONINTERFACE_H

#include <QObject>

class TsActivationInterface : public QObject
{

    Q_OBJECT

public:
    TsActivationInterface(QObject *parent = 0) : QObject(parent) {}
    virtual ~TsActivationInterface() {};

signals:
    void activated();

};

#endif // TSACTIVATIONINTERFACE_H
