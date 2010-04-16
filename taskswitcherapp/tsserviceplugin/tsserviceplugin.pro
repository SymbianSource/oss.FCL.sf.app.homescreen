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

win32 {
    MOBILITY += publishsubscribe
}

INCLUDEPATH += . inc

HEADERS +=  inc/tsserviceplugin.h \
            inc/tsdataroles.h \
            inc/tsactivationinterface.h \
            inc/tsactivation.h \
            inc/tslongpresswatcher.h \
            inc/tsexternalactivationwatcher.h \
            inc/tsdeactivationinterface.h \
            inc/tsdeactivation.h \
            inc/tsitemproviderinterface.h \
            inc/tsitemprovider.h \
            inc/tsclosedapplicationsfiltermodel.h \
            inc/tsrecentapplicationsmodel.h \
            inc/tspresentationinterface.h \
            inc/tspresentation.h \
            inc/tstasksgriditem.h \
            inc/tstasksgrid.h \
            inc/tstaskpopuphandler.h \
            inc/tsdocumentloader.h \
            
SOURCES +=  src/tsserviceplugin.cpp \
            src/tsactivation.cpp \
            src/tslongpresswatcher.cpp \
            src/tsexternalactivationwatcher.cpp \
            src/tsdeactivation.cpp \
            src/tsitemprovider.cpp \
            src/tsclosedapplicationsfiltermodel.cpp \
            src/tsrecentapplicationsmodel.cpp \
            src/tspresentation.cpp \
            src/tstasksgriditem.cpp \
            src/tstaskpopuphandler.cpp \
            src/tsdocumentloader.cpp \

LIBS +=     -lcaclient           
            
TRANSLATIONS = taskswitcher.ts

RESOURCES += tspresentation.qrc

symbian {
    load(data_caging_paths) 

    plugin.sources = tsserviceplugin.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR     

    # ugly hack
    translation.sources = ./*.qm
    translation.path = $$QT_PLUGINS_BASE_DIR/../translations

    # another ugly hack 
    crml.sources = ./resource/*.qcrml
    crml.path = $$QT_PLUGINS_BASE_DIR/../crml
    
    DEPLOYMENT += plugin translation crml
}

symbian {
    INCLUDEPATH += inc/s60
    HEADERS +=  inc/s60/tslongpresswatcher_p.h \
                inc/s60/tsexternalactivationwatcher_p.h \
                
    SOURCES +=  src/s60/tslongpresswatcher_p.cpp \
                src/s60/tsexternalactivationwatcher_p.cpp \
                
    LIBS += -lcone \
            -lapgrfx \
            -lws32 \

}

win32 {
    INCLUDEPATH +=  inc/win

    HEADERS +=  inc/win/tslongpresswatcher_p.h \
                inc/win/tsexternalactivationwatcher_p.h \
    
    SOURCES +=  src/win/tslongpresswatcher_p.cpp \
                src/win/tsexternalactivationwatcher_p.cpp \
                
    LIBS += -lUser32
}

include(../common.pri)
