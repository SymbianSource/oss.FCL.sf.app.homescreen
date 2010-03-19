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
# Description: Example of home screen widget
#

TEMPLATE = lib
CONFIG += plugin mobility hb
MOBILITY = serviceframework

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

DESTDIR = $${EPOCROOT}epoc32/data/c/private/20022F35/import/widgetregistry/20022F7E
win32: PLUGIN_SUBDIR = /hsresources/import/widgetregistry/20022F7E
symbian: PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20022F7E
include(../../../common.pri)

INCLUDEPATH += ./inc               

symbian: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x20022F7E
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll
    
    widgetResources.path = $${DESTDIR}
    widgetResources.sources += resource/$${TARGET}.xml    
    widgetResources.sources += resource/$${TARGET}.manifest
    widgetResources.sources += resource/$${TARGET}.png
    
    DEPLOYMENT += plugins \
                  widgetResources
}


exportResources(./resource/*.manifest, $$PLUGIN_SUBDIR)
exportResources(./resource/*.png, $$PLUGIN_SUBDIR)
exportResources(./resource/*.xml, $$PLUGIN_SUBDIR)
