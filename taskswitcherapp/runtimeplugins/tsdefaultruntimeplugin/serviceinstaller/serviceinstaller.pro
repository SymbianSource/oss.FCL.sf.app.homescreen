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
SUBDIRS = tsdefaultruntimeinstaller

symbian {
    load(data_caging_paths) 

    plugin.sources = tsdefaultruntimeplugin.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR 
    
    installer.pkg_postrules += "\"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/tsdefaultruntimeinstaller.exe\" \  
                                         - \"!:\sys\bin\tsdefaultruntimeinstaller.exe\",FR,RI,RW"
    
    DEPLOYMENT += plugin installer
} else {
    message("You might need to run tsdefaultruntimeinstaller.exe before running any application that uses task switcher service")
}
