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

INCLUDEPATH += inc

HEADERS +=  inc/hsmenuclient.h \
            inc/hsmenuclientplugin.h \

SOURCES +=  src/hsmenuclient.cpp \
            src/hsmenuclientplugin.cpp \

symbian: {
	TARGET.UID3 = 0x20028715
#export interface header to platform
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
BLD_INF_RULES.prj_exports += "inc/hsmenuclient.h \
	APP_LAYER_PLATFORM_EXPORT_PATH(hsmenuclient.h)"
}

include(../common.pri)


