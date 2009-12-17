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
* Description:  Implementation for wrapper for a box
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "view.h"
#include "xncomponent.h"
#include "xncontroladapter.h"


// ============================ MEMBER FUNCTIONS ===============================




// -----------------------------------------------------------------------------
// CXnViewPlugin::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnViewPlugin* CXnViewPlugin::NewL()
    {
	CXnViewPlugin* self = new( ELeave ) CXnViewPlugin;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnViewPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnViewPlugin::ConstructL()
    {
    }
    
// -----------------------------------------------------------------------------
// CXnViewPlugin::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnViewPlugin::CXnViewPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CXnViewPlugin::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnViewPlugin::~CXnViewPlugin()
    {
    }

// End of file
