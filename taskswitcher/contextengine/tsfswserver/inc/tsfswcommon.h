/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  common constants
 *
*/


#ifndef TSFSWCOMMON_H
#define TSFSWCOMMON_H

#include <e32std.h>

_LIT( KTsFswServerName, "tsfswserver" );
_LIT( KTsFswServerImg, "tsfswserver" );

const TUid KTsFswServerUid = { 0x2001CBE1 };

enum TTsFswServerMessages
    {
    ETsFswSubscribe,
    ETsFswCancel,
    ETsFswGetBufferSize,
    ETsFswGetBuffer, // there must be a GetBufferSize request before GetBuffer
    ETsFswCloseApp,
    ETsFswSwitchToApp,
    ETsFswForegroundAppUid
    };

#endif