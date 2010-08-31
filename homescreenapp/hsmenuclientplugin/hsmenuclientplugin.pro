#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:
#

TEMPLATE = lib

CONFIG += plugin mobility hs_public_plugin
MOBILITY = serviceframework

PLUGIN_SUBDIR = /resource/qt/plugins

LIBS += -lcaclient

INCLUDEPATH +=  inc \
				../../homescreen_plat/homescreenclient_api

HEADERS +=  ../../homescreen_plat/homescreenclient_api/hsmenuclient.h \
            inc/hsmenuclientplugin.h

SOURCES +=  src/hsmenuclient.cpp \
            src/hsmenuclientplugin.cpp

symbian: {
	TARGET.UID3 = 0x20028715
}

include(../common.pri)


