/*
* ============================================================================
*  Name        : hgteleportapplogging.h
*  Part of     : Classic UI / Teleport
*  Description : Defines logs for hgteleportapp
*  Version     : %version:  1 % 
*
*  Copyright (c) 2009 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
* Template version: 4.1.1
*/

#ifdef _DEBUG
#include "../inc/hgteleportuid.hrh"
_LIT( KHgLogFile, "hgteleportapp.txt" );
_LIT( KHgLogPath, "teleport" );
#define _HGLOG_LOG_COMPONENT_ID KHgTeleportAppUidValue

#endif
#include "hglogging.h"
