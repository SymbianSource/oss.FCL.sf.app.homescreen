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

#include <qvaluespacesubscriber.h>
#include <qvaluespacepublisher.h>

QTM_USE_NAMESPACE

TsExternalActivationWatcherPrivate::TsExternalActivationWatcherPrivate(QObject *parent) : QObject()
{
    mSubscriber = new QValueSpaceSubscriber("/TaskSwitcher/Activation", this);
    mPublisher = new QValueSpacePublisher("/TaskSwitcher", this);
    connect(mSubscriber, SIGNAL(contentsChanged()), this, SLOT(checkActivationFlag()));
    connect(this, SIGNAL(activationRequested()), parent, SIGNAL(activationRequested()));
}

void TsExternalActivationWatcherPrivate::checkActivationFlag()
{
    if (mSubscriber->value().isValid()) {
        mPublisher->resetValue("Activation");
        mPublisher->sync();
        emit activationRequested();
    }
}
