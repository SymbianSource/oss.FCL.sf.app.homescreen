#
# Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  snsrbigclockscreensaverplugin.pro
#

TEMPLATE = lib
CONFIG += plugin hb mobility

MOBILITY = serviceframework
MOBILITY += systeminfo

PLUGIN_SUBDIR = /snsrresources/plugins/screensaverplugins

#TRANSLATIONS = snsrbigclockscreensaverplugin.ts

include (../../common.pri)

LIBS += -lscreensavermodel \
        -lsnsrutils \
        -lxqsettingsmanager

INCLUDEPATH += ../../snsrutils/inc

symbian: {
    TARGET.UID3 = 0x20027052
}

include(snsrbigclockscreensaverplugin.pri)

RESOURCES = snsrbigclockscreensaver.qrc

exportResources(./snsrbigclockscreensaverplugin.qm, resource/qt/translations)

