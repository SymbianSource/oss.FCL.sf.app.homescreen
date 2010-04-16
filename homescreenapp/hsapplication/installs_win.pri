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
homescreendb.path = $$DESTDIR
homescreendb.files = ./resource_win/homescreen.db

wallpapers.CONFIG += no_build
wallpapers.path = $$DESTDIR/wallpapers
wallpapers.files = ./resource_win/d_landscape.png \
                   ./resource_win/d_portrait.png

themeableimagesfortheme11.path = "$$(USERPROFILE)/Application Data/hb/themes/icons/redtheme/scalable"
themeableimagesfortheme11.files = ./themes/theme1/hs_pageindicatoritem_active.svg \
                                  ./themes/theme1/hs_pageindicatoritem_nonactive.svg \
                                  ./themes/theme1/hs_shortcut_bg.svg 
                                  
themeableimagesfortheme12.path = "$$(USERPROFILE)/Application Data/hb/themes/icons/redtheme/pixmap"
themeableimagesfortheme12.files = ./themes/theme1/hs_trashbin_active.png \
                                  ./themes/theme1/hs_trashbin_nonactive.png
                                 
themeableimagesfortheme2.path = "$$(USERPROFILE)/Application Data/hb/themes/icons/bluetheme/scalable"
themeableimagesfortheme2.files = ./themes/theme2/hs_pageindicatoritem_active.svg \
                                 ./themes/theme2/hs_pageindicatoritem_nonactive.svg \
                                 ./themes/theme2/hs_shortcut_bg.svg \
                                 ./themes/theme2/hs_trashbin_active.svg \
                                 ./themes/theme2/hs_trashbin_nonactive.svg

INSTALLS += kqtihswallpapers homescreendb wallpapers themeableimagesfortheme11 themeableimagesfortheme12 themeableimagesfortheme2
PRE_TARGETDEPS += install_kqtihswallpapers install_homescreendb install_wallpapers
