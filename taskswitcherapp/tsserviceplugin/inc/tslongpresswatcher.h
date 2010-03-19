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
#ifndef TSLONGPRESSWATCHER_H
#define TSLONGPRESSWATCHER_H

#include <QObject>

class TsLongPressWatcherPrivate;

class TsLongPressWatcher : public QObject
{

    Q_OBJECT

public:
    TsLongPressWatcher(QObject *parent = 0);
    ~TsLongPressWatcher();

signals:
    void applicationKeyLongPress();

private:
    TsLongPressWatcherPrivate *d_ptr;

    friend class TsLongPressWatcherPrivate;

};

#endif // TSLONGPRESSWATCHER_H
