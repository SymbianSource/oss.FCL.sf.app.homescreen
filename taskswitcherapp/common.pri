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
# Description: taskswitcherapp project - common QMake settings
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
    OBJECTS_DIR = $$OUTPUT_DIR/tests/tmp/$$TARGET
    DESTDIR = $$OUTPUT_DIR
    MOC_DIR = $$OUTPUT_DIR/tests/tmp/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/tests/tmp/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/tests/tmp/$$TARGET/ui
    coverage {
        DEFINES += COVERAGE_MEASUREMENT
        DEFINES += QT_NO_DEBUG  # omit ASSERTS in coverage measurements
    }
    
    symbian {
        TARGET.SID = 0x2002677D
    }
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

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
}
