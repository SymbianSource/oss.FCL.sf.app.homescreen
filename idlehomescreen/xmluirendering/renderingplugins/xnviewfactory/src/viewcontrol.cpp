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

// System includes
#include <e32base.h>
#include <coecntrl.h>

// User includes
#include "viewcontrol.h"
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CViewControl::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CViewControl* CViewControl::NewL(CXnNodePluginIf& aNode)
    {
	CViewControl* self = new( ELeave ) CViewControl;

    CleanupStack::PushL( self );
    self->ConstructL( aNode );
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CViewControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CViewControl::ConstructL(CXnNodePluginIf& aNode)
    {
    CXnControlAdapter::ConstructL( aNode );
    }
    
// -----------------------------------------------------------------------------
// CViewControl::ViewControl
// C++ default constructor
// -----------------------------------------------------------------------------
//
CViewControl::CViewControl()
    {    
    }

// -----------------------------------------------------------------------------
// CViewControl::~ViewControl
// C++ destructor
// -----------------------------------------------------------------------------
//
CViewControl::~CViewControl()
    {    
    }
    
//end of file
