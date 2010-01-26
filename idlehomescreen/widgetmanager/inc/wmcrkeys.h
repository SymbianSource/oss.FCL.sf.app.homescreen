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
// Key Id's from 0x1 to 0x28 are reserved for languages.
// LangId_0                 0x1
// OviStoreBrowserUrl_0     0x2
// OperatorUrl_0            0x3
// OperatorButtonText_0     0x4
// ...
// LangId_1                 0x5
// ...
// LangId_9                 0x25
// OviStoreBrowserUrl_9     0x26
// OperatorUrl_9            0x27
// OperatorButtonText_9     0x28 
// ************************************************************

/** localised set #0, language ID */
const TUint32 KLangId0 = 0x1;
/** localised set #0, Ovi store browser URL */
const TUint32 KOviStoreBrowserUrl0 = 0x2;
/** localised set #0, Operator button URL */
const TUint32 KOperatorButtonUrl0 = 0x3;
/** localised set #0, Operator button text */
const TUint32 KOperatorButtonText0 = 0x4;

/** localised set #1, language ID */
const TUint32 KLangId1 = 0x5;

/** localised set #9, language ID */
const TUint32 KLangId9 = 0x25;


// ************************************************************
// NON-LOCALISED parameters
// ************************************************************

/** Key to Ovi Store Client param */
const TUint32 KOviStoreClientParam = 0x29;

/** Key to Operator button icon */
const TUint32 KOperatorButtonIcon = 0x2A;

/** Key to Ovi Store client BundleId */
const TUint32 KOviStoreBundleId = 0x2B;



#endif // __WMCRKEYS_H__
