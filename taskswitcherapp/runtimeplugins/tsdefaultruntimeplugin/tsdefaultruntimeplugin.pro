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
# Description:  snsrdefaultruntimeplugin.pro
#

TEMPLATE = lib
CONFIG += plugin mobility
MOBILITY = serviceframework

INCLUDEPATH += inc

HEADERS +=  inc/tsdefaultruntime.h \
            inc/tsdefaultruntimeplugin.h \

SOURCES +=  src/tsdefaultruntime.cpp \
            src/tsdefaultruntimeplugin.cpp \

symbian {
    load(data_caging_paths) 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    plugin.sources = tsdefaultruntimeplugin.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR     
    DEPLOYMENT += plugin 
}
