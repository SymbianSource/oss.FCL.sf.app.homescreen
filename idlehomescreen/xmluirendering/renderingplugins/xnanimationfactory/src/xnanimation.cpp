/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Xuikon Animation component.
*
*/



#include "xnanimation.h"
#include "xnanimationadapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnAnimation::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnAnimation* CXnAnimation::NewL()
    {
	CXnAnimation* self = new( ELeave ) CXnAnimation;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnAnimation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnAnimation::ConstructL()
    {
    }
    
// -----------------------------------------------------------------------------
// CXnAnimation::CXnAnimation
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnAnimation::CXnAnimation()
    {
    }

// -----------------------------------------------------------------------------
// CXnAnimation::~CXnAnimation
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnAnimation::~CXnAnimation()
    {
    }

// ---------------------------------------------------------
// CXnAnimation::MakeInterfaceL
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnAnimation::MakeInterfaceL(const TDesC8& aType)
    {
    if (aType != XnAnimationInterface::KType)
        {
        return NULL;
        }
    XnAnimationInterface::MXnAnimationInterface* animationintr = static_cast<XnAnimationInterface::MXnAnimationInterface*>(this);
    return animationintr;
    }           
