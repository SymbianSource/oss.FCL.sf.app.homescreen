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
kqtihswallpapers.files = ./resource/bg_1.png \
                         ./resource/bg_2.png \
                         ./resource/bg_3.png

homescreendb.CONFIG += no_build
homescreendb.path = $$DESTDIR/private/20022f35
homescreendb.files = ./resource/resource_win/homescreen.db

scenewallpapers.CONFIG += no_build
scenewallpapers.path = $$DESTDIR/private/20022f35/wallpapers/scene
scenewallpapers.files = ./resource/scene/default_landscape.png \
                   ./resource/scene/default_portrait.png

pagewallpapers.CONFIG += no_build
pagewallpapers.path = $$DESTDIR/private/20022f35/wallpapers/page
pagewallpapers.files = ./resource/page/default_landscape.png \
                       ./resource/page/default_portrait.png \
                       ./resource/page/1_portrait.png \
                       ./resource/page/1_landscape.png \
                       ./resource/page/2_portrait.png \
                       ./resource/page/2_landscape.png \
                       ./resource/page/3_portrait.png \
                       ./resource/page/3_landscape.png

INSTALLS += kqtihswallpapers homescreendb scenewallpapers pagewallpapers
PRE_TARGETDEPS += install_kqtihswallpapers install_homescreendb install_scenewallpapers install_pagewallpapers
