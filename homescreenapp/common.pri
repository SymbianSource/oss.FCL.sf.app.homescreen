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
# Description: homescreenapp project - common QMake settings
#

CONFIG += debug_and_release

# On win32 and mac, debug and release libraries are named differently.
# We must follow the debug and release settings Qt was compiled with:
# build debug iff Qt built debug, build release iff Qt built release.
win32|mac {
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug):  CONFIG+=debug
        contains(QT_CONFIG,release):CONFIG+=release
    }
}

CONFIG(debug, debug|release) {
    SUBDIRPART = debug
} else {
    SUBDIRPART = release
}

win32: OUTPUT_DIR = $$PWD/../../bin/$$SUBDIRPART
symbian: OUTPUT_DIR = $$PWD/bin

SOURCE_DIR = $$PWD/inc

#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/tmp/$$TARGET
    DESTDIR = $$OUTPUT_DIR
    MOC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/tmp/$$TARGET/ui
} else { # test part is NOT DONE
    QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/bin/tests/$$SUBDIRPART/tmp/$$TARGET
    DESTDIR = $$OUTPUT_DIR/bin/tests/$$SUBDIRPART
    MOC_DIR = $$OUTPUT_DIR/bin/tests/$$SUBDIRPART/tmp/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/bin/tests/$$SUBDIRPART/tmp/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/bin/tests/$$SUBDIRPART/tmp/$$TARGET/ui
    LIBS += -L$$OUTPUT_DIR/bin/$$SUBDIRPART/bin  #link against library that we test
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR
#For some reason the default include path doesn't include MOC_DIR on symbian
symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA=1
}

win32 {
    # add platfrom API for windows
    INCLUDEPATH += \                
                $$PWD/../../homescreensrv/homescreensrv_plat/contentstorage_api \
                $$PWD/../../homescreensrv/homescreensrv_plat/hswidgetmodel_api \               
                $$PWD/../../homescreensrv/homescreensrv_plat/homescreen_information_api/inc
}

plugin: !isEmpty(PLUGIN_SUBDIR): DESTDIR = $$OUTPUT_DIR/$$PLUGIN_SUBDIR

win32: plugin { # copy manifiers
    manifest.path = $$DESTDIR
    manifest.files = ./resource/*.manifest ./resource/*.xml
    manifest.CONFIG += no_build

    INSTALLS += manifest
    PRE_TARGETDEPS += install_manifest

}

symbian: plugin { # copy qtstub and manifest

    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_SUBDIR

    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_SUBDIR
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin

    message(Remove "contains(MOBILITY, serviceframework)" after the QtSF refactorig is done!)
        
    !contains(MOBILITY, serviceframework):qtplugins.sources += resource/$${TARGET}.manifest
    contains(MOBILITY, serviceframework):BLD_INF_RULES.prj_exports += "resource/$${TARGET}.s60xml z:$$qtplugins.path/$${TARGET}.xml"
    
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)"
}

defineTest(exportResources) {
symbian {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) : BLD_INF_RULES.prj_exports += "./$$entry z:/$$replace(2, ^/,)/$$basename(entry)"
    }
    export ( BLD_INF_RULES.prj_exports)
}
win32 {
    name = $$replace(1, [/\\\\\.\*], _)
    eval ($${name}.path = $${OUTPUT_DIR}/$${2})
    eval ($${name}.files = $$1)
    eval ($${name}.CONFIG += no_build)

    INSTALLS += $$name
    PRE_TARGETDEPS += install_$${name}

    export ( $${name}.path )
    export ( $${name}.files )
    export ( $${name}.CONFIG )
    export ( INSTALLS )
    export ( PRE_TARGETDEPS )
}
}

!exists($${EPOCROOT}epoc32/include/mw/XQSettingsManager) {
	DEFINES += NO_QT_EXTENSIONS
}

# support for NFT
nft:DEFINES += NFT

release:DEFINES+=QT_NO_DEBUG_OUTPUT

nft:release {
DEFINES -=QT_NO_DEBUG_OUTPUT
}

symbian {
SYMBIAN_PLATFORMS = WINSCW ARMV5
# ONLY FOR DEVELOPMENT! REMOVE THIS BEFORE EACH HS_Domain RELEASE!
MMP_RULES += EXPORTUNFROZEN
}
