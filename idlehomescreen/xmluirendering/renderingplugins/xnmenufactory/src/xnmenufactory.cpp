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
* Description:  Implementation for factory creating container for a menu
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xnmenuadapter.h"
#include "xnmenufactory.h"
#include "xnmenu.h"

#include "xntype.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include <aisystemuids.hrh>
#include "xndomnode.h"


// ============================ MEMBER FUNCTIONS ===============================

MXnComponentFactory::TXnComponentFactoryResponse CXnMenuFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
        										
										)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = 
        MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if(aNode.Type()->Type() == KXnMenuBar)
        {
    	  CXnMenu* container = CXnMenu::NewL();
        aTargetComponent = static_cast<CXnComponent*>(container);
   	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
  	    return retVal;
	    }
    return retVal;
    }

CXnControlAdapter* CXnMenuFactory::MakeXnControlAdapterL( 
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* aParent
										)
    {
    CXnControlAdapter* controlAdapter = CXnMenuAdapter::NewL( aParent, aNode );
    return controlAdapter;
    }

// -----------------------------------------------------------------------------
// CXnMenuFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnMenuFactory* CXnMenuFactory::NewL()
    {
    CXnMenuFactory* self = new( ELeave ) CXnMenuFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnMenuFactory::CXnMenuFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnMenuFactory::CXnMenuFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnMenuFactory::~CXnMenuFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnMenuFactory::~CXnMenuFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnMenuFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnMenuFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_MENUFACTORY_IMPLEMENTATION, CXnMenuFactory::NewL) 
#else
    {{AI3_UID_RENDERING_PLUGIN_MENUFACTORY_IMPLEMENTATION}, CXnMenuFactory::NewL}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
