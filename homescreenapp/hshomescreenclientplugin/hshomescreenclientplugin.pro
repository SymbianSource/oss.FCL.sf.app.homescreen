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
# Description:
#


TEMPLATE = lib
CONFIG += plugin hb mobility console
MOBILITY = serviceframework

PLUGIN_SUBDIR = /hsresources/plugins/homescreenclientplugin

include (../common.pri)

DEPENDPATH += 	./inc \
              	./src
              
INCLUDEPATH += 	./inc 

symbian: {
    TARGET.UID3 = 0x20022F72
    LIBS += -lxqservice
}

win32: {
    error("Only Symbian supported!")
}

include(hshomescreenclientplugin.pri)