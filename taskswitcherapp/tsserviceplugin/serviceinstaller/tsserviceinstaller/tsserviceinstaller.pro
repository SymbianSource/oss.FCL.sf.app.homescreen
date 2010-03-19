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

QT = core
#TODO - temporarily disabled, until the installer issues on HW are resolved
#CONFIG += no_icon

CONFIG += console
CONFIG += mobility
MOBILITY = serviceframework

SOURCES += tsserviceinstaller.cpp

RESOURCES += tsserviceinstaller.qrc

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
