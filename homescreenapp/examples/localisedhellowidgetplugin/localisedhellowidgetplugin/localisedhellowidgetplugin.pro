#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Example of home screen widget
#

TEMPLATE = lib
CONFIG += plugin mobility hb
MOBILITY = serviceframework

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc               

symbian: {
		    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x20022F80
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    DESTDIR = /private/20022F35/import/widgetregistry/20022F80
    
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll 
    
    widgetResources.path = $${DESTDIR}
    widgetResources.sources += resource/$${TARGET}.xml    
    widgetResources.sources += resource/$${TARGET}.manifest
    widgetResources.sources += resource/$${TARGET}.png
    
    localisedFiles.path = /resource/qt/translations
    localisedFiles.sources += ./locales/*.qm 
    
    DEPLOYMENT += plugins \
                  widgetResources \
                  localisedFiles
}

win32: {

    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }    				 
   
    PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20022F80
    
    DESTDIR = $$PWD/../../../../../bin/$${SUBDIRPART}/$${PLUGIN_SUBDIR}
	
    manifest.path = $${DESTDIR}
    manifest.files = ./resource/*.manifest ./resource/*.xml ./resource/*.png
    
    widgetLocalisation.path = $$PWD/../../../../../bin/$${SUBDIRPART}/resource/qt/translations
    widgetLocalisation.files += ./locales/*.qm 
        
    INSTALLS += manifest widgetLocalisation      
}
