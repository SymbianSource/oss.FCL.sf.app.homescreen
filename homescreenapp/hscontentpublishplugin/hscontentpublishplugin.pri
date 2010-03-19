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


HEADERS += ./inc/hscontentpublishplugin.h
SOURCES += ./src/hscontentpublishplugin.cpp


win32:{
    HEADERS += ./inc_win/hshomescreenclient.h
    SOURCES += ./src/hshomescreenclient_win.cpp
    
}

symbian:{
    HEADERS += ./inc_symbian/hshomescreenclient.h
    SOURCES += ./src/hshomescreenclient_symbian.cpp

}


