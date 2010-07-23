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

PLUGIN_SUBDIR = /private/20022F35/plugins/stateplugins

include(../../common.pri)

LIBS += -lhsdomainmodel \
        -lhsmenuserviceprovider \
        -lhsutils \
        -lcaclient \
        -lcasoftwareregistry

DEPENDPATH += ./inc \
    ./src
INCLUDEPATH += ./inc \
    ../../serviceproviders/hsmenuserviceprovider/inc \
    ../../hsutils/inc \
    ../../hsdomainmodel/inc

symbian: {
    TARGET.UID3 = 0x20022F99

nft: {
    LIBS += -lhal
}
}

include(hsmenuworkerstateplugin.pri)

RESOURCES = hsmenuworkerstateplugin.qrc



