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
#include "tsexternalactivationwatcher_p.h"
#include "tsexternalactivationwatcher.h"

const TUid KTaskswitcherCategory = { 0x2002677D };
const TUint KExternalActivationKey = 0x1;
const TInt KTaskswitcherActivateFlag = 1;

/*!
    \class TsExternalActivationWatcherPrivate
    \ingroup group_tsserviceplugin
    \brief Private implementation of external activation watcher.
*/

TsExternalActivationWatcherPrivate::TsExternalActivationWatcherPrivate(TsExternalActivationWatcher *parent) : CActive(CActive::EPriorityStandard), mParent(parent)
{
    CActiveScheduler::Add(this);
    
    TInt err = RProperty::Define(KTaskswitcherCategory, KExternalActivationKey, RProperty::EInt);
    if (err == KErrNone || err == KErrAlreadyExists) {
        err = mProperty.Attach(KTaskswitcherCategory, KExternalActivationKey);
    }
    
    Q_ASSERT_X(err == KErrNone, "TsExternalActivationWatcherPrivate", qPrintable(QString("Cannot define/attach to activation watcher property %1").arg(err)));
    
    mProperty.Subscribe(iStatus);
    SetActive();
}

TsExternalActivationWatcherPrivate::~TsExternalActivationWatcherPrivate()
{
    Cancel();
    mProperty.Close();
}

void TsExternalActivationWatcherPrivate::ResetActivationFlag()
{
    TInt value(0);
    mProperty.Set(value);
}

void TsExternalActivationWatcherPrivate::DoCancel()
{
    mProperty.Cancel();
}

void TsExternalActivationWatcherPrivate::RunL()
{
    if (iStatus.Int() != KErrCancel) {
        
        if (CheckActivationFlag()) {
            ResetActivationFlag();
            QT_TRYCATCH_LEAVING(emit mParent->activationRequested());
        }
        mProperty.Subscribe(iStatus);
        SetActive();
    }
}

bool TsExternalActivationWatcherPrivate::CheckActivationFlag()
{
    TInt value(0);
    TInt result = mProperty.Get(value);
    return (result == KErrNone && value & KTaskswitcherActivateFlag);
}
