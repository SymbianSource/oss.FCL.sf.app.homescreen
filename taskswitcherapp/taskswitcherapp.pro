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

TEMPLATE = subdirs

symbian:SUBDIRS += taskswitcherappecom
SUBDIRS += tsserviceplugin
SUBDIRS += runtimeplugins
SUBDIRS += tsapplication

tests {
    symbian:SUBDIRS += taskswitcherappecom\tsrc
    SUBDIRS += tsserviceplugin\tsrc
    SUBDIRS += runtimeplugins\tsdefaultruntimeplugin\tsrc
    SUBDIRS += tsapplication\tsrc
    SUBDIRS += internal\tstestrunner    
}

symbian:include(rom.pri)

CONFIG += ordered
