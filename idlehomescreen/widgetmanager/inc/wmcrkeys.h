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
const TUint32 KCrWidgetManagerm = 0x20026F53;



// ************************************************************
// LOCALISED parameters start here
// Key Id's from 0x1 to 0x1E are reserved for languages.
// LangId_0                 0x1
// OperatorUrl_0            0x2
// OperatorButtonText_0     0x3
// ...
// LangId_1                 0x4
// ...
// LangId_9                 0x1C
// OperatorUrl_9            0x1D
// OperatorButtonText_9     0x1E 
// ************************************************************

/** localised set #0, language ID */
const TUint32 KLangId0 = 0x1;
/** localised set #0, Operator button URL */
const TUint32 KOperatorButtonUrl0 = 0x2;
/** localised set #0, Operator button text */
const TUint32 KOperatorButtonText0 = 0x3;

/** localised set #1, language ID */
const TUint32 KLangId1 = 0x4;

/** localised set #9, language ID */
const TUint32 KLangId9 = 0x1C;


// ************************************************************
// NON-LOCALISED OVI parameters
// ************************************************************

/** Key to Ovi Store Client param */
const TUint32 KOviStoreClientParam = 0x1F;

/** Key to Ovi Store client Uid */
const TUint32 KOviStoreUid = 0x20;

// ************************************************************
// NON-LOCALISED Operator parameters
// ************************************************************

/** Key to Operator button icon */
const TUint32 KOperatorButtonEnabled = 0x21;

/** Key to Operator button icon */
const TUint32 KOperatorButtonIcon = 0x22;

/** 
 * Key to Operator application type and id.
 * Give ':' as separator between type and id.
 * Type can be s60, cwrt, wrt, java, qt
 * Examples:
 * s60:0x10005902                           Application Uid as id
 * cwrt:ovi.serviceui.SSOUI                 BundleId as id
 * wrt:com.accuweather.widget.touchNG       BundleId as id
 * java:c:/sys/bin/javastore.jar            full path for java app
 * qt:0x2001A9CA                            Application Uid as id
 */
const TUint32 KOperatorAppTypeAndId = 0x23;

/** Key to Operator application param */
const TUint32 KOperatorParam = 0x24;

/** 
 * Key to define if operator button needs to be placed to left in landscape and
 * top on portraid. By default it's right and bottom. 
 */
const TUint32 KOperatorButtonHigherPriority = 0x25;


#endif // __WMCRKEYS_H__
