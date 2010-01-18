/*
 * ===========================================================================
 *  Name        : hgfswcommon.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : common constants
 *  Version     : %version: sa1spcx1#5 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#ifndef __HGFSWCOMMON_H
#define __HGFSWCOMMON_H

#include <e32std.h>

_LIT( KHgFswServerName, "hgfswserver" );
_LIT( KHgFswServerImg, "hgfswserver" );

const TUid KHgFswServerUid = { 0x2001CBE1 };

enum THgFswServerMessages
    {
    EHgFswSubscribe,
    EHgFswCancel,
    EHgFswGetBufferSize,
    EHgFswGetBuffer, // there must be a GetBufferSize request before GetBuffer
    EHgFswCloseApp,
    EHgFswSwitchToApp,
    EHgFswForegroundAppUid
    };

#endif
