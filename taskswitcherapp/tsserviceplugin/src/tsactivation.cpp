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

TsActivation::TsActivation(QObject *parent) : TsActivationInterface(parent)
{
    TsLongPressWatcher *watcher = new TsLongPressWatcher(this);
    connect(watcher, SIGNAL(applicationKeyLongPress()), this, SIGNAL(activated()));
}
