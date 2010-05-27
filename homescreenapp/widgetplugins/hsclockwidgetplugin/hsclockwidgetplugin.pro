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

HB += hbfeedback

LIBS += -lhsdomainmodel

PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20022F6C
include(../../common.pri)

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
                ../../hsdomainmodel/inc
		
symbian: {
    TARGET.UID3 = 0x20022F6C
    LIBS += -lbafl -lapgrfx -lcone
    
    
}

win32:include(hsclockwidgetplugin_win.pri)
symbian:include(hsclockwidgetplugin_symbian.pri)

RESOURCES = hsclockwidgetplugin.qrc

exportResources(./resource/*.manifest, $$PLUGIN_SUBDIR)
exportResources(./resource/*.png, $$PLUGIN_SUBDIR)
