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

#include "tslongpresswatcher.h"
#include "tslongpresswatcher_p.h"

/*!
    \class TsLongPressWatcher
    \ingroup group_tsserviceplugin
    \brief Class watching app key events.
*/
TsLongPressWatcher::TsLongPressWatcher(QObject *parent) : QObject(parent)
{
    d_ptr = new TsLongPressWatcherPrivate(this);
}

TsLongPressWatcher::~TsLongPressWatcher()
{
    delete d_ptr;
}
