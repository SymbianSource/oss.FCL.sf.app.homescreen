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
#include "tslongpresswatcher_p.h"
#include "tslongpresswatcher.h"

// AknCapServer UID, used for P&S category
const TUid KTaskswitcherStateCategory = { 0x10207218 };
// Taskswitcher UI, used as P&S key
const TUint KTaskswitcherStateKey = 0x2002677D;
// Values for Taskswitcher launching P&S
const TInt KTaskswitcherBackgroundValue = 1;
const TInt KTaskswitcherForegroundValue = KTaskswitcherBackgroundValue << 1;
const TInt KTaskswitcherLongAppKeyPressed = KTaskswitcherForegroundValue << 1;

/*!
    \class TsLongPressWatcherPrivate
    \ingroup group_tsserviceplugin
    \brief Private implementation of long press app key watcher.
*/

TsLongPressWatcherPrivate::TsLongPressWatcherPrivate(TsLongPressWatcher *parent) : CActive(CActive::EPriorityStandard), mParent(parent)
{
    CActiveScheduler::Add(this);
    TInt err = mProperty.Attach(KTaskswitcherStateCategory, KTaskswitcherStateKey);
    if (err == KErrNone) {
        mProperty.Subscribe(iStatus);
        SetActive();
    }
}

TsLongPressWatcherPrivate::~TsLongPressWatcherPrivate()
{
    Cancel();
    mProperty.Close();
}

void TsLongPressWatcherPrivate::setBackgroundState()
{
    TInt value(0);
    mProperty.Get(value);
    value &= ~KTaskswitcherForegroundValue;
    value &= ~KTaskswitcherLongAppKeyPressed;
    value |= KTaskswitcherBackgroundValue;
    mProperty.Set(value);
}

void TsLongPressWatcherPrivate::DoCancel()
{
    mProperty.Cancel();
}

void TsLongPressWatcherPrivate::RunL()
{
    if (iStatus.Int() != KErrCancel) {
        if (checkLongPressFlag()) {
            setBackgroundState();
            QT_TRYCATCH_LEAVING(emit mParent->applicationKeyLongPress());
        }
        mProperty.Subscribe(iStatus);
        SetActive();
    }
}

bool TsLongPressWatcherPrivate::checkLongPressFlag()
{
    TInt value(0);
    bool res = false;
    mProperty.Get(KTaskswitcherStateCategory, KTaskswitcherStateKey, value);
    if (value & KTaskswitcherLongAppKeyPressed) {
        res = true;
    }
    return res;
}
