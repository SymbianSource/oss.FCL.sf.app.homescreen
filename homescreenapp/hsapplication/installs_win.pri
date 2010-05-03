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

kqtihswallpapers.CONFIG += no_build
kqtihswallpapers.path = $$DESTDIR/resource/kqtihswallpapers
kqtihswallpapers.files = ./resource_win/bg_1.png \
                         ./resource_win/bg_2.png \
                         ./resource_win/bg_3.png

homescreendb.CONFIG += no_build
homescreendb.path = $$DESTDIR/private/20022f35
homescreendb.files = ./resource_win/homescreen.db

wallpapers.CONFIG += no_build
wallpapers.path = $$DESTDIR/private/20022f35/wallpapers
wallpapers.files = ./resource_win/d_landscape.png \
                   ./resource_win/d_portrait.png

INSTALLS += kqtihswallpapers homescreendb wallpapers 
PRE_TARGETDEPS += install_kqtihswallpapers install_homescreendb install_wallpapers
