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


#ifndef WMCOMMON_H
#define WMCOMMON_H

//includes
#include <e32base.h>

// widgetmanager UID
const TUid KUidWidgetManager = { 0x200255FF };

// HsContentInfo types
_LIT8( KContentWidget, "widget" );
_LIT8( KContentTemplate, "template" );

// UI related const
const TInt KWidgetHeight = 90;

// UI component ID's
enum TWmUiControlIds
    {
    EPortalOne = 0x1,
    EPortalTwo,
    ELastPortal,
    EWidgetsList,
    EFindBox,
    EMaximumWidgets = 0xC8
    };

#endif //WMCOMMON_H
