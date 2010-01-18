/*
 * ============================================================================
 *  Name        : hgteleportdocument.cpp
 *  Part of     : Hg Teleport
 *  Description : Document class
 *  Version     : %version: 4 %
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

#include <apgwgnam.h>

#include "hgteleportappui.h"
#include "hgteleportdocument.h"

// -----------------------------------------------------------------------------
// CHgTeleportDocument::NewLC( )
// Create a CHgTeleportDocument object and return a pointer to it
// -----------------------------------------------------------------------------
//
CHgTeleportDocument* CHgTeleportDocument::NewL( CEikApplication& aApp )
    {
    CHgTeleportDocument* self = NewLC ( aApp );
    CleanupStack::Pop ( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportDocument::NewLC( )
// Create a CHgTeleportDocument object and return a pointer to it
// -----------------------------------------------------------------------------
//
CHgTeleportDocument* CHgTeleportDocument::NewLC( CEikApplication& aApp )
    {
    CHgTeleportDocument* self = new ( ELeave ) CHgTeleportDocument( aApp );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportDocument::ConstructL( )
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CHgTeleportDocument::ConstructL( )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CHgTeleportDocument::~CHgTeleportDocument( )
// First phase constructor
// -----------------------------------------------------------------------------
//
CHgTeleportDocument::CHgTeleportDocument( CEikApplication& aApp ) :
    CAknDocument( aApp )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CHgTeleportDocument::~CHgTeleportDocument( )
// Destructor
// -----------------------------------------------------------------------------
//
CHgTeleportDocument::~CHgTeleportDocument( )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CHgTeleportDocument::CreateAppUiL( )
// Create a CHgTeleportAppUi object and return a pointer to it
// -----------------------------------------------------------------------------
//
CEikAppUi* CHgTeleportDocument::CreateAppUiL( )
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CHgTeleportAppUi;
    return appUi;
    }

// -----------------------------------------------------------------------------
// CHgTeleportDocument::UpdateTaskNameL( )
// Makes app hidden in fastswap window
// -----------------------------------------------------------------------------
//
void CHgTeleportDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    CEikDocument::UpdateTaskNameL ( aWgName );
    aWgName->SetHidden ( ETrue );
    }


// End of file
