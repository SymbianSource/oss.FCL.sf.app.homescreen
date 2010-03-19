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

#include <QAbstractEventDispatcher>
#include <QDebug>

#include <windows.h>

TsLongPressWatcherPrivate *TsLongPressWatcherPrivate::instance = NULL;

bool TsLongPressWatcherPrivate::hotKeyFilter(void *message)
{
    MSG *m = (MSG *)(message);
    if (m->message == WM_HOTKEY) {
        unsigned long key = (m->lParam & 0xFFFF0000) >> 16;
        unsigned long mods = (m->lParam & 0x0000FFFF);
        if (key == VK_OEM_3 && mods == (MOD_CONTROL | MOD_SHIFT)) {
            emit TsLongPressWatcherPrivate::instance->applicationKeyLongPress();
            return true;
        }
    }
    return false;
}

TsLongPressWatcherPrivate::TsLongPressWatcherPrivate(TsLongPressWatcher *parent) : QObject(parent)
{
    connect(this, SIGNAL(applicationKeyLongPress()), parent, SIGNAL(applicationKeyLongPress()));
    TsLongPressWatcherPrivate::instance = this;

    QAbstractEventDispatcher::instance()->setEventFilter(&TsLongPressWatcherPrivate::hotKeyFilter);

    if (!RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_SHIFT, VK_OEM_3)) {
        qCritical("Failed to register hot key, taskswitcher might not work as expected");
    } else {
        qDebug("To activate task switcher press Ctrl+Shift+~");
    }
}

TsLongPressWatcherPrivate::~TsLongPressWatcherPrivate()
{
    UnregisterHotKey(NULL, 1);
    TsLongPressWatcherPrivate::instance = NULL;
}
