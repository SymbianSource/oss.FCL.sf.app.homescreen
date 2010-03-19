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
HEADERS +=  ./inc/hscontentservice.h \
                       ./inc/hsdatabase.h  \ 
                       ./inc/hsdomainmodel_global.h  \
                       ./inc/hspage.h \  
                        ./inc/hspagedata.h \  
                        ./inc/hsscene.h \
                        ./inc/hsscene_p.h \ 
                        ./inc/hsscenedata.h \
                        ./inc/hsshortcutservice.h \
                         ./inc/hsshortcutservice_p.h \
                         ./inc/hswallpaper.h \
                        ./inc/hswallpaper_p.h \
                        ./inc/hswidgetdata.h \                      
                          ./inc/hswidgethost.h \  
                           ./inc/hswidgetpresentationdata.h 
                               
SOURCES += ./src/hscontentservice.cpp   \
                        ./src/hsdatabase.cpp  \
                        ./src/hspage.cpp  \
                        ./src/hspagedata.cpp  \
                        ./src/hsscene.cpp \
                        ./src/hsscenedata.cpp \
                        ./src/hsshortcutservice.cpp \   
                       ./src/hswallpaper.cpp \ 
                       ./src/hswidgetdata.cpp \       
                       ./src/hswidgethost.cpp \       
                       ./src/hswidgetpresentationdata.cpp 
                              
win32:{
HEADERS +=  ./inc/hscontentpublishhandler.h \
                        ./inc/hsipcchannel.h \
                        ./inc/hsipcchannelclient.h \
                        ./inc/hsipcchannelhost.h \
                        ./inc/hsipcconnectiondispatcher.h \   
                        ./inc/hsipcserver.h     
                      
SOURCES +=  ./src/hscontentpublishhandler.cpp \
                        ./src/hsipcchannel.cpp \
                        ./src/hsipcchannelclient.cpp \
                        ./src/hsipcchannelhost.cpp \
                        ./src/hsipcconnectiondispatcher.cpp \   
                        ./src/hsipcserver.cpp 

}

