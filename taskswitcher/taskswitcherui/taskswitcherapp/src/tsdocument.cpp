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
* Description:  Document class
 *
*/


#include <apgwgnam.h>

#include "tsappui.h"
#include "tsdocument.h"

// -----------------------------------------------------------------------------
// CTsDocument::NewLC( )
// Create a CTsDocument object and return a pointer to it
// -----------------------------------------------------------------------------
//
CTsDocument* CTsDocument::NewL( CEikApplication& aApp )
    {
    CTsDocument* self = NewLC ( aApp );
    CleanupStack::Pop ( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsDocument::NewLC( )
// Create a CTsDocument object and return a pointer to it
// -----------------------------------------------------------------------------
//
CTsDocument* CTsDocument::NewLC( CEikApplication& aApp )
    {
    CTsDocument* self = new ( ELeave ) CTsDocument( aApp );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsDocument::ConstructL( )
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CTsDocument::ConstructL( )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CTsDocument::~CTsDocument( )
// First phase constructor
// -----------------------------------------------------------------------------
//
CTsDocument::CTsDocument( CEikApplication& aApp ) :
    CAknDocument( aApp )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CTsDocument::~CTsDocument( )
// Destructor
// -----------------------------------------------------------------------------
//
CTsDocument::~CTsDocument( )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CTsDocument::CreateAppUiL( )
// Create a CTsAppUi object and return a pointer to it
// -----------------------------------------------------------------------------
//
CEikAppUi* CTsDocument::CreateAppUiL( )
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CTsAppUi;
    return appUi;
    }

// -----------------------------------------------------------------------------
// CTsDocument::UpdateTaskNameL( )
// Makes app hidden in fastswap window
// -----------------------------------------------------------------------------
//
void CTsDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    CEikDocument::UpdateTaskNameL ( aWgName );
    aWgName->SetHidden ( ETrue );
    }


// End of file
