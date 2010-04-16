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

#include "tsactivation.h"

#include "tslongpresswatcher.h"
#include "tsexternalactivationwatcher.h"

/*!
    \class TsActivation
    \ingroup group_tsserviceplugin
    \brief Activation service.
    
    Service responsible for observing all events that might show TS. When any of them
    occurs it is emiting activation signal.
*/
TsActivation::TsActivation(QObject *parent) : TsActivationInterface(parent)
{
    TsLongPressWatcher *longPressWatcher = new TsLongPressWatcher(this);
    connect(longPressWatcher, SIGNAL(applicationKeyLongPress()), this, SIGNAL(activated()));
    
    TsExternalActivationWatcher *externalActivationWatcher = new TsExternalActivationWatcher(this);
    connect(externalActivationWatcher, SIGNAL(activationRequested()), this, SIGNAL(activated()));    
}
