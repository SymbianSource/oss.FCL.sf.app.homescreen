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
* Description:  Implementation for factory creating a popup.
*
*/


#include <e32base.h>
#include <coecntrl.h>

#include "xnpopupfactory.h"
#include "xnpopup.h"
#include "xntype.h"
#include "xncomponent.h"
#include "xnpopupadapter.h"
#include "xnnodepluginif.h"
#include <aisystemuids.hrh>

// CONSTANTS
_LIT8(KPopup, "tooltip");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// E32Dll implements the Symbian OS dll entry point
// Entry point needed for EKA1 kernel only
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt  E32Dll(
    TDllReason)
    {

    return KErrNone;

    }
#endif

MXnComponentFactory::TXnComponentFactoryResponse CXnPopupFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
        										
										)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if(aNode.Type()->Type() == KPopup)
        {
    	CXnPopup* container = CXnPopup::NewL();
      aTargetComponent = static_cast<CXnComponent*>(container);
	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
	    return retVal;
	    }
    return retVal;
    }

CXnControlAdapter* CXnPopupFactory::MakeXnControlAdapterL( 
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* aParent
										)
    {
    CXnControlAdapter* controlAdapter = CXnPopupAdapter::NewL(aNode, aParent);
    return controlAdapter;
    }

// -----------------------------------------------------------------------------
// CXnPopupFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnPopupFactory* CXnPopupFactory::NewL()
    {
    CXnPopupFactory* self = new( ELeave ) CXnPopupFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnPopupFactory::CXnPopupFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnPopupFactory::CXnPopupFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnPopupFactory::~CXnPopupFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnPopupFactory::~CXnPopupFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnPopupFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnPopupFactory::ConstructL()
    {
    }

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_POPUPFACTORY_IMPLEMENTATION, CXnPopupFactory::NewL) 
#else
    {{AI3_UID_RENDERING_PLUGIN_POPUPFACTORY_IMPLEMENTATION}, CXnPopupFactory::NewL}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
