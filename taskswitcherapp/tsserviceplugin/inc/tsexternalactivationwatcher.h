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
#ifndef TSEXTERNALACTIVATIONWATCHER_H
#define TSEXTERNALACTIVATIONWATCHER_H

#include <QObject>
    
class TsExternalActivationWatcherPrivate;    
    
class TsExternalActivationWatcher : public QObject
{
    Q_OBJECT

public:
    TsExternalActivationWatcher(QObject *parent = 0);
    virtual ~TsExternalActivationWatcher();
    
signals:
    void activationRequested();

private:
    TsExternalActivationWatcherPrivate *d_ptr;    
    friend class TsExternalActivationWatcherPrivate;    
    
};

#endif // TSEXTERNALACTIVATIONWATCHER_H
