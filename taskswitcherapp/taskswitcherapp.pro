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

SUBDIRS += tsdevicedialogplugin

tests {
    SUBDIRS += internal\tstestrunner    
    SUBDIRS += tsdevicedialogplugin\tsrc
    SUBDIRS += ..\..\homescreensrv\activityfw\activityserviceplugin\unittests
    SUBDIRS += ..\..\homescreensrv\activityfw\activitydatabase\hsactivitydbclient\t_hsactivitydbclient
    SUBDIRS += ..\..\homescreensrv\hsappkeyhandler\tsrc\t_hsappkeyhandler
    SUBDIRS += ..\..\homescreensrv\activityfw\activityserviceplugin\s60\tsrc
}

coverage {
    SUBDIRS += ..\..\homescreensrv\activityfw
    SUBDIRS += ..\..\homescreensrv\hsappkeyhandler
}

symbian:include(rom.pri)

CONFIG += ordered
