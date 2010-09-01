/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for CEikLabel
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>
#include "xncomponent.h"
#include "xntext.h"
#include "xntextadapter.h"

// ============================ MEMBER FUNCTIONS ===============================




// -----------------------------------------------------------------------------
// CXnText::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnText* CXnText::NewL()
    {
	CXnText* self = new( ELeave ) CXnText;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnText::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnText::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnText::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnText::CXnText()
    {
    }

// -----------------------------------------------------------------------------
// CXnText::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnText::~CXnText()
    {
    }
    
// ---------------------------------------------------------
// CXnText::SetTextL
// ---------------------------------------------------------
//
void CXnText::SetTextL(const TDesC& aText)
    {
    (static_cast<CXnTextAdapter*>(ControlAdapter()))->SetTextL(aText);
    }
	
// ---------------------------------------------------------
// CXnText::Text
// ---------------------------------------------------------
//
const TDesC* CXnText::Text()
    {
    return (static_cast<CXnTextAdapter*>(ControlAdapter()))->Text();   
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnText::MakeInterfaceL(const TDesC8& aType)
    {
    if (aType != XnTextInterface::KType)
        {
        return NULL;
        }
    XnTextInterface::MXnTextInterface* textintr = static_cast<XnTextInterface::MXnTextInterface*>(this);
    return textintr;
    }        
