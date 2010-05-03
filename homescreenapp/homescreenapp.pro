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
# Description: Main project file for homescreen application
#

TEMPLATE = subdirs

SUBDIRS  += hsutils \
            hsdomainmodel \
            runtimeplugins \
            serviceproviders \
            stateplugins \
            widgetplugins \
            hsapplication \
            hsmenuclientplugin

symbian:SUBDIRS  += hshomescreenclientplugin

SUBDIRS  += ./../tsrc

CONFIG += ordered

symbian:include(homescreenapp_exports_to_rom.pri)
