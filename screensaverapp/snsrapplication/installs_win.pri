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

stylefortheme1.path = "$$(USERPROFILE)/Application Data/hb/apptheme/snsrapplication/style/redtheme/variables/color"
stylefortheme1.files = ./themes/style/theme1/hbcolorgroup.css

stylefortheme2.path = "$$(USERPROFILE)/Application Data/hb/apptheme/snsrapplication/style/bluetheme/variables/color"
stylefortheme2.files = ./themes/style/theme2/hbcolorgroup.css

stylefortheme3.path = "$$(USERPROFILE)/Application Data/hb/apptheme/snsrapplication/style/hbdefault/variables/color"
stylefortheme3.files = ./themes/style/defaulttheme/hbcolorgroup.css

themeableimagesfortheme11.path = "$$(USERPROFILE)/Application Data/hb/themes/icons/redtheme/scalable"
themeableimagesfortheme11.files = ./themes/icons/theme1/analog-clock-hour-hand.svg \
                                  ./themes/icons/theme1/analog-clock-minute-hand.svg

themeableimagesfortheme12.path = "$$(USERPROFILE)/Application Data/hb/themes/icons/redtheme/pixmap"
themeableimagesfortheme12.files = ./themes/icons/theme1/analog-clock-background.png

themeableimagesfortheme2.path = "$$(USERPROFILE)/Application Data/hb/themes/icons/bluetheme/scalable"
themeableimagesfortheme2.files = ./themes/icons/theme2/analog-clock-background.svg \
                                 ./themes/icons/theme2/analog-clock-hour-hand.svg \
                                 ./themes/icons/theme2/analog-clock-minute-hand.svg
								 
INSTALLS += stylefortheme1 stylefortheme2 stylefortheme3 themeableimagesfortheme11 themeableimagesfortheme12 themeableimagesfortheme2
