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
CONFIG += plugin hb mobility console
MOBILITY = serviceframework

PLUGIN_SUBDIR = /hsresources/plugins/contentpublish

include (../common.pri)

win32:{
LIBS += -lhsdomainmodel \
        -lhsutils
}
QT += xml sql network

DEPENDPATH += ./inc \
              ./inc_win \
              ./inc_symbian \
              ./src
INCLUDEPATH += ./inc \
               ./inc_win \
              ./inc_symbian \
             ../hsutils/inc \
             ../hsdomainmodel/inc \

symbian: {
    TARGET.UID3 = 0x20022F72
    LIBS += -lxqservice
}


include(hscontentpublishplugin.pri)
