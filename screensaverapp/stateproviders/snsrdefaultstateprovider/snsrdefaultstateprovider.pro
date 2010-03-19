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
# Description: snsrdefaultstateprovider.pro
#

TEMPLATE = lib
CONFIG += plugin
PLUGIN_SUBDIR = /snsrresources/plugins/stateproviders

include(../../common.pri)

LIBS += -lstatemodel \
        -lscreensavermodel \
        -lsnsruseractivityservice
CONFIG += hb

symbian: {
TARGET.UID3 = 0x20027050
# TODO: temporary until screensavermodel goes to platform API
INCLUDEPATH += ../../../../homescreensrv/screensavermodel/inc
}

include(snsrdefaultstateprovider.pri)
