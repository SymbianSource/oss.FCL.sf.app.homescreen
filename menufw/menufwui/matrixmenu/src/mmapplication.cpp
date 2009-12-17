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
* Description:  Application class
*
*/


// INCLUDE FILES

#include "mmgui.hrh"
#include "mmdocument.h"
#include "mmapplication.h"
#include "mmguiconstants.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CApaDocument* CMmApplication::CreateDocumentL()
	{
	// Create a Matrix Menu document, and return a pointer to it
	return ( static_cast<CApaDocument*>
					( CMmDocument::NewL( *this ) ) );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUid CMmApplication::AppDllUid() const
	{
	// Return the UID for the Matrix Menu application
	return KUidMatrixMenuApp;
	}

// End of File
