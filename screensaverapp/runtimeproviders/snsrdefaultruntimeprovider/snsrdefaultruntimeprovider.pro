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
# Description:  snsrdefaultruntimeprovider.pro
#

TEMPLATE = lib
CONFIG += plugin
PLUGIN_SUBDIR = /snsrresources/plugins/runtimeproviders

include (../../common.pri)

LIBS += -lappruntimemodel \
        -lstatemodel

CONFIG += hb

symbian: {
TARGET.UID3 = 0x2002704F
}

include(snsrdefaultruntimeprovider.pri)
