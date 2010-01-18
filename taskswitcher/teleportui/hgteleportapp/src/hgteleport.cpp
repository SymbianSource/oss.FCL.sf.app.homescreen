/*
 * ============================================================================
 *  Name        : hgteleport.cpp
 *  Part of     : Hg Teleport
 *  Description : Main cpp of application. Definition of start function
 *  Version     : %version: 5 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 *
 */

#include <eikstart.h>

#include "hgteleportapplication.h"

// -----------------------------------------------------------------------------
// NewApplication
// Create main class of application
// -----------------------------------------------------------------------------
//
CApaApplication* NewApplication( )
    {
    return new CHgTeleportApplication;
    }

// -----------------------------------------------------------------------------
// E32Main
// Application enty point.
// -----------------------------------------------------------------------------
//
TInt E32Main( )
    {
    return EikStart::RunApplication( NewApplication );
    }


// end of file
