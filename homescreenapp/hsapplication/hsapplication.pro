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

TEMPLATE = app

include(../common.pri)

LIBS += -lhsutils
CONFIG += console

QT += xml

CONFIG += hb mobility
MOBILITY = serviceframework

fute:DEFINES += FUTE

DEPENDPATH += . \
              ./inc \
              ./src

INCLUDEPATH += . \
               ./inc \
                ../hsutils/inc

TRANSLATIONS = homescreen.ts

RESOURCES += hsapplication_fallbacktheme.qrc

symbian: {
    TARGET.UID3 = 0x20022F35
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles TrustedUI
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000 // 128kB - 23MB
    ICON = resource_s60/qgn_menu_appsgrid.svg

    appkey:DEFINES += S60APP_KEY
    LIBS += -lefsrv
    include(installs_symbian.pri)
}

win32: {
    include(installs_win.pri)
}

include(hsapplication.pri)

exportResources(./*.qm, resource/qt/translations)
