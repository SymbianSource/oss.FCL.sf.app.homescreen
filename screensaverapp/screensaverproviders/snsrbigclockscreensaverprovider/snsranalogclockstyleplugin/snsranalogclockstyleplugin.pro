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
# Description:  snsranalogclockstyleplugin.pro
#

TEMPLATE = lib

CONFIG += plugin hb

PLUGIN_SUBDIR = /snsrresources/plugins/snsrstyleplugins

include (../../../common.pri)

symbian: {
    TARGET.UID3 = 0x20027054
}

include(snsranalogclockstyleplugin.pri)

RESOURCES += snsranalogclockstyleplugin.qrc