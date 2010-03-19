#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  snsrbigclockscreensaverprovider.pro
#

TEMPLATE = lib

CONFIG += plugin
PLUGIN_SUBDIR = /snsrresources/plugins/screensaverproviders

include (../../common.pri)

LIBS += -lscreensavermodel \
        -lsnsrutils

INCLUDEPATH += ../../snsrutils/inc
        
CONFIG += hb

symbian: {
    TARGET.UID3 = 0x20027052
    # TODO: temporary until screensavermodel goes to platform API
    INCLUDEPATH += ../../../../homescreensrv/screensavermodel/inc
}

include(snsrbigclockscreensaverprovider.pri)

RESOURCES = snsrbigclockscreensaver.qrc
