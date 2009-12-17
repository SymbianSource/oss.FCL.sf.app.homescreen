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
* Description:  Implementation for wrapper for Netscape Plugin API.
*
*/


// INCLUDE FILES
#include "xnnpplugin.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNPPlugin::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnNPPlugin* CXnNPPlugin::NewL()
    {
    CXnNPPlugin* self = new( ELeave ) CXnNPPlugin;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;    
    }

// -----------------------------------------------------------------------------
// CXnNPPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNPPlugin::ConstructL()
    {
    }
    
// -----------------------------------------------------------------------------
// CXnNPPlugin::CXnNPPlugin
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnNPPlugin::CXnNPPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CXnNPPlugin::~CXnNPPlugin
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnNPPlugin::~CXnNPPlugin()
    {
    }
    
// End of file

