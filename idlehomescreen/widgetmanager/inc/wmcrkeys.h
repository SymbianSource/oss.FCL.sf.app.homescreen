/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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


#ifndef __WMCRKEYS_H__
#define __WMCRKEYS_H__

// WidgetManager CR key
const TUint32 KCrWidgetManager = 0x20026F53;

// ************************************************************
// NON-LOCALISED parameters
// ************************************************************

/**  */
const TUint32 KOviStoreEnabled = 0x1;

/**  */
const TUint32 KOviOrder = 0x2;

/**  */
const TUint32 KOviUid = 0x3;

/**  */
const TUint32 KOviAppParam = 0x4;

/**  */
const TUint32 KStoreEnabled = 0x5;

/**  */
const TUint32 KOrder = 0x6;

// ************************************************************
// LOCALISED parameters start here
// KLangId_0                 0x7
// KUrl_0                    0x8
// KTitle_0                  0x9
// ...
// KLangId_1                 0xA
// ...
// KLangId_9                 0x22
// KUrl_9                    0x23
// KTitle_9                  0x24 
// ************************************************************

/** localised set #0, language ID */
const TUint32 KLangId_0 = 0x7;
/** localised set #0, store  URL */
const TUint32 KUrl_0 = 0x8;
/** localised set #0, store  text */
const TUint32 KTitle_0 = 0x9;

/** localised set #1, language ID */
const TUint32 KLangId_1 = 0xA;

/** localised set #9, language ID */
const TUint32 KLangId_9 = 0x22;

// ************************************************************
// NON-LOCALISED parameters
// ************************************************************

/** 
 * Key to Store application type and id.
 * Give ':' as separator between type and id.
 * Type can be s60, cwrt, wrt, java, qt
 * Examples:
 * s60:0x10005902                           Application Uid as id
 * cwrt:ovi.serviceui.SSOUI                 BundleId as id
 * wrt:com.accuweather.widget.touchNG       BundleId as id
 * java:c:/sys/bin/javastore.jar            full path for java app
 * qt:0x2001A9CA                            Application Uid as id
 */
const TUint32 KAppTypeAndId = 0x25;

/** */
const TUint32 KAppParam = 0x26;

/** Key to Store icon */
const TUint32 KIcon = 0x27;


#endif // __WMCRKEYS_H__
