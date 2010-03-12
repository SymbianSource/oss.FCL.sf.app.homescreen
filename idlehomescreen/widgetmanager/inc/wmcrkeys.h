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
// NON-LOCALISED parameters
// ************************************************************

/** Key to Ovi Store Client param */
const TUint32 KOviStoreClientParam = 0x1F;

/** Key to Operator button icon */
const TUint32 KOperatorButtonIcon = 0x20;

/** Key to Ovi Store client Uid */
const TUint32 KOviStoreUid = 0x21;



#endif // __WMCRKEYS_H__
