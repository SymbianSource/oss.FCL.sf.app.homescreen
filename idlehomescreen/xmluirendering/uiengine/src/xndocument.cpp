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
* Description:  Implementation for document base class
*
*/

// INCLUDE FILES
#include "xndocument.h"
#include "xnappuiadapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDocument::CXnDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDocument::CXnDocument( CEikApplication& aApp, TUid aApplicationUid )
    : CAknDocument( aApp )
    {
    iApplicationUid = aApplicationUid;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDocument::~CXnDocument()
    {
    }

// -----------------------------------------------------------------------------
// CXnDocument::CreateAppUiL
// Creates an AppUi
// -----------------------------------------------------------------------------
//
EXPORT_C CEikAppUi* CXnDocument::CreateAppUiL()
    {
    CEikAppUi* appUi = new ( ELeave ) CXnAppUiAdapter( iApplicationUid );
    return appUi;
    }

// -----------------------------------------------------------------------------
// CXnDocument::Uid
// Get the application uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CXnDocument::Uid()
    {
    return iApplicationUid;
    }
