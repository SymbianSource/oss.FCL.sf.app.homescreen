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
* Description:  Application class
 *
*/


#include "tsdocument.h"
#include "tsapplication.h"
#include "tsuid.hrh"

// UID for the application, this should correspond
// to the uid defined in the mmp file
static const TUid KUidTsApp = { KTsAppUidValue };

// -----------------------------------------------------------------------------
// CTsApplication::CreateDocumentL( )
// Create a CApaDocument object and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CTsApplication::CreateDocumentL( )
    {
    CApaDocument* document = CTsDocument::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CTsApplication::AppDllUid( )
// Returns the application DLL UID value
// -----------------------------------------------------------------------------
//
TUid CTsApplication::AppDllUid( ) const
    {
    // Return the UID for the task switcher application
    return KUidTsApp;
    }


// end of file
