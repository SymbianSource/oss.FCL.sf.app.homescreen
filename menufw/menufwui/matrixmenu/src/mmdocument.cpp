/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application Document class
*
*/


// INCLUDE FILES
#include "mmappui.h"
#include "mmdocument.h"
#include <apgwgnam.h>

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmDocument* CMmDocument::NewL( CEikApplication& aApp )
	{
	CMmDocument* self = NewLC( aApp );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmDocument* CMmDocument::NewLC( CEikApplication& aApp )
	{
	CMmDocument* self =
		new ( ELeave ) CMmDocument( aApp );

	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmDocument::ConstructL()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmDocument::CMmDocument( CEikApplication& aApp )
	: CAknDocument( aApp )
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmDocument::~CMmDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEikAppUi* CMmDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it
	// the framework takes ownership of this object
	return ( static_cast <CEikAppUi*> ( new ( ELeave ) CMmAppUi ) );
	}

// End of File
