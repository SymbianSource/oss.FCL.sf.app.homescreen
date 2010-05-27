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
# Description:
#

TEMPLATE = lib
CONFIG += plugin hb mobility
MOBILITY = serviceframework publishsubscribe

PLUGIN_SUBDIR = /private/20022F35/plugins/runtimeplugins

include (../../common.pri)

LIBS += -lhsdomainmodel \
        -lhsutils
        
QT += xml sql

DEPENDPATH += ./inc \
              ./src
INCLUDEPATH += ./inc \
             ../../hsutils/inc \
             ../../hsdomainmodel/inc \

symbian: {
    TARGET.UID3 = 0x20022F3E
    LIBS += -lxqkeycapture
}


include(hsdefaultruntimeplugin.pri)
