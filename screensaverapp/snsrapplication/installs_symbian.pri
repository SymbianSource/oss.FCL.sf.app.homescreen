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

deploy.path = c:

#style

styleexports1.path = /resource/hb/apptheme/snsrapplication/style/redtheme/variables/color
styleexports1.sources += themes/style/theme1/hbcolorgroup.css
for(styleexport1, styleexports1.sources):BLD_INF_RULES.prj_exports += "./$$styleexport1 $$deploy.path$$styleexports1.path/$$basename(styleexport1)"

styleexports2.path = /resource/hb/apptheme/snsrapplication/style/bluetheme/variables/color
styleexports2.sources += themes/style/theme2/hbcolorgroup.css
for(styleexport2, styleexports2.sources):BLD_INF_RULES.prj_exports += "./$$styleexport2 $$deploy.path$$styleexports2.path/$$basename(styleexport2)"

styleexports3.path = /resource/hb/apptheme/snsrapplication/style/hbdefault/variables/color
styleexports3.sources += themes/style/defaulttheme/hbcolorgroup.css
for(styleexport3, styleexports3.sources):BLD_INF_RULES.prj_exports += "./$$styleexport3 $$deploy.path$$styleexports3.path/$$basename(styleexport3)"

#icons

iconsexports11.path = /resource/hb/themes/icons/redtheme/scalable
iconsexports11.sources += themes/icons/theme1/analog-clock-hour-hand.svg
iconsexports11.sources += themes/icons/theme1/analog-clock-minute-hand.svg
for(iconsexport11, iconsexports11.sources):BLD_INF_RULES.prj_exports += "./$$iconsexport11 $$deploy.path$$iconsexports11.path/$$basename(iconsexport11)"

iconsexports12.path = /resource/hb/themes/icons/redtheme/pixmap
iconsexports12.sources += themes/icons/theme1/analog-clock-background.png
for(iconsexport12, iconsexports12.sources):BLD_INF_RULES.prj_exports += "./$$iconsexport12 $$deploy.path$$iconsexports12.path/$$basename(iconsexport12)"

iconsexports2.path = /resource/hb/themes/icons/bluetheme/scalable
iconsexports2.sources += themes/icons/theme2/analog-clock-background.svg
iconsexports2.sources += themes/icons/theme2/analog-clock-hour-hand.svg
iconsexports2.sources += themes/icons/theme2/analog-clock-minute-hand.svg
for(iconsexport2, iconsexports2.sources):BLD_INF_RULES.prj_exports += "./$$iconsexport2 $$deploy.path$$iconsexports2.path/$$basename(iconsexport2)"
