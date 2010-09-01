/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class of all Xuikon-based applications
*
*/

// INCLUDE FILES
#include "xnapplication.h"
#include "xndocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnApplication::CXnApplication
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnApplication::CXnApplication( TUid aApplicationUid )
    {
    iApplicationUid = aApplicationUid;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CXnApplication::~CXnApplication()
    {
    }

// -----------------------------------------------------------------------------
// CXnApplication::AppDllUid
// Returns the application uid.
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CXnApplication::AppDllUid() const
    {
    return iApplicationUid;
    }

// -----------------------------------------------------------------------------
// CXnApplication::CreateDocumentL
// Creates a document
// -----------------------------------------------------------------------------
//
EXPORT_C CApaDocument* CXnApplication::CreateDocumentL()
    {
    return new ( ELeave ) CXnDocument( *this, iApplicationUid );
    }
