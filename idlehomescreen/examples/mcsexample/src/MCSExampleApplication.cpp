/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

// INCLUDE FILES
#include "MCSExample.hrh"
#include "MCSExampleDocument.h"
#include "MCSExampleApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMCSExampleApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CMCSExampleApplication::CreateDocumentL()
    {
    // Create an MCSExample document, and return a pointer to it
    return CMCSExampleDocument::NewL(*this);
    }

// -----------------------------------------------------------------------------
// CMCSExampleApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CMCSExampleApplication::AppDllUid() const
    {
    // Return the UID for the MCSExample application
    return KUidMCSExampleApp;
    }

// End of File
