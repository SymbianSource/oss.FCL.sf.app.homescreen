/*
 * ============================================================================
 *  Name        : hgteleportapplication.cpp
 *  Part of     : Hg Teleport
 *  Description : Application class
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

#include "hgteleportdocument.h"
#include "hgteleportapplication.h"
#include "hgteleportuid.hrh"

// UID for the application, this should correspond
// to the uid defined in the mmp file
static const TUid KUidHgTeleportApp = { KHgTeleportAppUidValue };

// -----------------------------------------------------------------------------
// CHgTeleportApplication::CreateDocumentL( )
// Create a CApaDocument object and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CHgTeleportApplication::CreateDocumentL( )
    {
    CApaDocument* document = CHgTeleportDocument::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CHgTeleportApplication::AppDllUid( )
// Returns the application DLL UID value
// -----------------------------------------------------------------------------
//
TUid CHgTeleportApplication::AppDllUid( ) const
    {
    // Return the UID for the hgteleport application
    return KUidHgTeleportApp;
    }


// end of file
