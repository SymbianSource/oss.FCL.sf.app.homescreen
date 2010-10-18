/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef SCREENSAVERDOMAINCRKEYS_H
#define SCREENSAVERDOMAINCRKEYS_H

#include <qglobal.h>

/**
* Screensaver Settings repository UID.
*
* @publishedPartner
* @released
*/
const long int KCRUidValueScreensaverSettings = 0x2002FFAB;

#ifdef Q_OS_SYMBIAN
#include <e32cmn.h>
const TUid KCRUidScreensaverSettings = {KCRUidValueScreensaverSettings};
#endif


/*
* Unlock button 
*
* Boolean value for unlock Button on active mode. 
* 
* Possible integer values:
*
* 0 = unlock button not used
* 1 = unlock button exists on active mode
*
* Default value: 1
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverUnlockWidget                   = 0x00000001;

/**
* Timeout from active mode to power save mode
* 
* Screensaver timeout from active mode to power save mode 
* Power save mode is clock display in OLED-devices, blank display in other devices
*
* Possible integer values:
*
* 5 - 60 Seconds
*
* Default value: 7
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverTimeToPowerSave           = 0x00000002;

/**
* User setting whether screensaver is shown in power save mode
* 
* Boolean value for screensaver on/off via control panel.
* Setting is visible only in OLED-devices, i.e. if FF_AVC_DISPLAY_OLED_LOWPOWER_SUPPORT is enabled 
* Non-OLED devices have allways blank display in power save mode
*
* Possible integer values:
*
* 0 = Screensaver is off
* 1 = Screensaver is on
*
* Default value: 1
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverStatus           = 0x00000003;

/**
* Type of display (for legacy reasons, prefer use of  FF_AVC_DISPLAY_OLED_LOWPOWER_SUPPORT)
*  
* 0 = Regular display type
* 1 = RegularAndOled
* 2 = OLED
* 
*
* Default value: 2
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverDisplayType     = 0x00000004;

/**
* Initial state of the screensaver
*  
* 1 = ViewTypeActive
* 2 = ViewTypePowerSave
* 
* Initial view type for startup 
*
* Default value: 2
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverStartupView     = 0x00000005;


#endif  // SCREENSAVERDOMAINCRKEYS_H
            
// End of File
