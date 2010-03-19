# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
TEMPLATE = lib
CONFIG += plugin hb mobility
MOBILITY = serviceframework

PLUGIN_SUBDIR = /hsresources/plugins/stateplugins
include(../../common.pri)
LIBS += -lhsdomainmodel \
        -lhsutils \
        -lhsmenuserviceprovider \
        -lcaclient

DEPENDPATH += ./inc \
    ./src
INCLUDEPATH += ./inc \
    ../hsmenuworkerstateplugin/inc \
    ../../serviceproviders/hsmenuserviceprovider/inc \
    ../../hsdomainmodel/inc \
    ../../hsutils/inc
TRANSLATIONS = hsapplibrary.ts
include(hsapplibrarystateplugin.pri)
symbian: {
    TARGET.UID3 = 0x20022F97
    nft::LIBS += -lhal
    include(hsapplibrarystateplugin_exports_to_rom.pri)
    exists($${EPOCROOT}epoc32/include/mw/XQSettingsManager):LIBS += -lxqsettingsmanager
}
exportResources(./*.qm, resource/qt/translations)
RESOURCES = hsapplibrarystateplugin.qrc

