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

coverage {
    SUBDIRS += ../../homescreensrv/activityfw
    SUBDIRS += ../../homescreensrv/hsappkeyhandler
    SUBDIRS += ../../homescreensrv/tstaskmonitor
}

SUBDIRS += tsdevicedialogplugin

tests {
    SUBDIRS += internal/tstestrunner    
    SUBDIRS += tsdevicedialogplugin/tsrc
    SUBDIRS += ../../homescreensrv/activityfw/activityserviceplugin/tsrc
    SUBDIRS += ../../homescreensrv/activityfw/activitydatabase/hsactivitydbclient/t_hsactivitydbclient
    SUBDIRS += ../../homescreensrv/hsappkeyhandler/tsrc/t_hsappkeyhandler
    SUBDIRS += ../../homescreensrv/tstaskmonitor/client/tsrc
    SUBDIRS += ../../homescreensrv/tstaskmonitor/server/tsrc
    SUBDIRS += ../../homescreensrv/tstaskmonitor/screenshotplugin/tsrc
    SUBDIRS += ../../homescreensrv/activityfw/tsutils/tsrc
    SUBDIRS += ../../homescreensrv/tstaskmonitor/utils/tsrc
    SUBDIRS += ../../homescreensrv/tstaskmonitor/backstepping/tsrc
}

symbian:include(rom.pri)

CONFIG += ordered
