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
#include "MCSExampleAppUi.h"
#include "MCSExampleDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMCSExampleDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMCSExampleDocument* CMCSExampleDocument::NewL(CEikApplication& aApp)
    {
    CMCSExampleDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMCSExampleDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMCSExampleDocument* CMCSExampleDocument::NewLC(CEikApplication& aApp)
    {
    CMCSExampleDocument* self = new (ELeave) CMCSExampleDocument(aApp);

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMCSExampleDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMCSExampleDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CMCSExampleDocument::CMCSExampleDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMCSExampleDocument::CMCSExampleDocument(CEikApplication& aApp) :
    CAknDocument(aApp)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CMCSExampleDocument::~CMCSExampleDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CMCSExampleDocument::~CMCSExampleDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CMCSExampleDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CMCSExampleDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return new (ELeave) CMCSExampleAppUi;
    }

// End of File
