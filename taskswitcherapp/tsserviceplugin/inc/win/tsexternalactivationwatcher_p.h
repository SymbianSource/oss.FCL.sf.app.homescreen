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
#ifndef TSEXTERNALACTIVATIONWATCHERPRIVATE_H
#define TSEXTERNALACTIVATIONWATCHERPRIVATE_H

#include <QObject>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
    class QValueSpacePublisher;
    class QValueSpaceSubscriber;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
    
class TsExternalActivationWatcherPrivate : public QObject
{
    Q_OBJECT

public:
    TsExternalActivationWatcherPrivate(QObject *parent);

private slots:
    void checkActivationFlag();
    
signals:
    void activationRequested();

private:
    QValueSpacePublisher *mPublisher;
    QValueSpaceSubscriber *mSubscriber;
    
};

#endif // TSEXTERNALACTIVATIONWATCHERPRIVATE_H
