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
# Description: snsruseractivityservice.pro
#

TEMPLATE = lib

include(../../common.pri)

DEFINES += SNSRUSERACTIVITYSERVICE_LIB

symbian: {
TARGET.UID3 = 0x2002704D
LIBS += -lactivitymanager

include(snsruseractivityservice_s60.pri)
} else {
include(snsruseractivityservice_stub.pri)
}

include(snsruseractivityservice.pri)
