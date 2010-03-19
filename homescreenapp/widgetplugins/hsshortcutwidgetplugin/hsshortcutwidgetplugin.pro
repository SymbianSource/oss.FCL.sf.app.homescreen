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
MOBILITY = serviceframework

win32: PLUGIN_SUBDIR = /hsresources/import/widgetregistry/20022F46
symbian: PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20022F46

include(../../common.pri)

LIBS += -lhsdomainmodel \
        -lcaclient

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \                
                ../../hsdomainmodel/inc

symbian: {
    TARGET.UID3 = 0x20022F46
}


include(hsshortcutwidgetplugin.pri)

exportResources(./resource/*.manifest, $$PLUGIN_SUBDIR)
