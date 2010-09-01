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
* Description:  Implementation for a popup 
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xnpopup.h"
#include "xnpopupadapter.h"

// ============================ MEMBER FUNCTIONS ===============================




// -----------------------------------------------------------------------------
// CXnPopup::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnPopup* CXnPopup::NewL()
    {
	CXnPopup* self = new( ELeave ) CXnPopup;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnPopup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnPopup::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnPopup::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnPopup::CXnPopup()
    {
    }

// -----------------------------------------------------------------------------
// CXnPopup::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnPopup::~CXnPopup()
    {
    }


// -----------------------------------------------------------------------------
// CXnPopup::ShowPopup
// Shows the popup
// -----------------------------------------------------------------------------
void CXnPopup::ShowPopupL(const TRect& aContainerRect)
    {
		static_cast<CXnPopupAdapter*>(ControlAdapter())->ShowPopupL(aContainerRect);    	
    }
void CXnPopup::ShowPopupL(const TRect& aContainerRect, TTimeIntervalMicroSeconds32 aShowDelay, TTimeIntervalMicroSeconds32 aDisplayTime)
    {
		static_cast<CXnPopupAdapter*>(ControlAdapter())->ShowPopupL(aContainerRect, aShowDelay, aDisplayTime);    	
    }

// -----------------------------------------------------------------------------
// CXnPopup::HidePopup
// Hides the popup
// -----------------------------------------------------------------------------
void CXnPopup::HidePopupL()
    {
    static_cast<CXnPopupAdapter*>(ControlAdapter())->HidePopupL();    		
    }
// -----------------------------------------------------------------------------
// CXnPopup::MakeInterfaceL
// -----------------------------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnPopup::MakeInterfaceL(const TDesC8& aType)
    {
   if (aType != XnPopupInterface::KType)
       {
       return NULL;
       }
    XnPopupInterface::MXnPopupInterface* popup = static_cast<XnPopupInterface::MXnPopupInterface*>(this);
    return popup;
    }            
