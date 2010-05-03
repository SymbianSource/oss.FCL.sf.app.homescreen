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
# Description: tsdevicedialogplugin.pro
#

TEMPLATE = lib
CONFIG += hb plugin mobility
MOBILITY = serviceframework 

include (../common.pri)

HEADERS +=  inc/tsdevicedialog.h \
            inc/tsdevicedialogplugin.h \
            inc/tsmodel.h \
            inc/tsmodelitem.h \
            inc/tsentrymodelitem.h \
            inc/tsactivitymodelitem.h \
            inc/tstasksgrid.h \
            inc/tstasksgriditem.h \
            inc/tsdocumentloader.h \
            inc/tsdataroles.h \

SOURCES +=  src/tsdevicedialog.cpp \
            src/tsdevicedialogplugin.cpp \
            src/tsmodel.cpp \
            src/tsentrymodelitem.cpp \
            src/tsactivitymodelitem.cpp \
            src/tstasksgriditem.cpp \
            src/tsdocumentloader.cpp \

INCLUDEPATH += . \
               ./inc \

LIBS +=     -lcaclient           
            
TRANSLATIONS = taskswitcher.ts

RESOURCES += tsdevicedialogplugin.qrc
               
symbian {
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN
    TARGET.UID3 = 0x2002677F
    pluginstub.sources = tsdevicedialogplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs

    # ugly hack
    translation.sources = ./*.qm
    translation.path = $$QT_PLUGINS_BASE_DIR/../translations
	
    DEPLOYMENT += pluginstub translation
}

win32 {
    include ($$[QMAKE_MKSPECS]/features/hb_install.prf)
    DESTDIR = $$HB_PLUGINS_DIR/devicedialogs
}
