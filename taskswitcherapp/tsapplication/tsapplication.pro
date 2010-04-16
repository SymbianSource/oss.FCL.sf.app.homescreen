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
CONFIG += hb console mobility
MOBILITY = serviceframework

INCLUDEPATH += inc

SOURCES +=  src/main.cpp \
            src/tstaskswitcher.cpp \
            
HEADERS +=  inc/tstaskswitcher.h \

#temporary workaround for epoc bug in QtSF
RESOURCES += tsapplication.qrc 

symbian {
    TARGET.EPOCHEAPSIZE = 0x700000 0xF00000    
    TARGET.UID3 = 0x2002677D
}

include(../common.pri)
