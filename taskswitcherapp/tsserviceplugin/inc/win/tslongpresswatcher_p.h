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
#ifndef TSLONGPRESSWATCHERPRIVATE_H
#define TSLONGPRESSWATCHERPRIVATE_H

#include <QObject>

class TsLongPressWatcher;

class TsLongPressWatcherPrivate : public QObject
{

    Q_OBJECT

public:
    TsLongPressWatcherPrivate(TsLongPressWatcher *parent);
    virtual ~TsLongPressWatcherPrivate();

    static bool hotKeyFilter(void *message);
    static TsLongPressWatcherPrivate *instance;

signals:
    void applicationKeyLongPress();

};

#endif // TSLONGPRESSWATCHERPRIVATE_H
