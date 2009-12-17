/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for factory creating CXnVolumeControl -element
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xnvolumecontrolfactory.h"
#include "xnvolumecontroladapter.h"
#include "xntype.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include "xndomnode.h"
#include "xncomponentnodeimpl.h"
#include <aisystemuids.hrh>
#include "xnvolumecontrol.h"

// ============================ MEMBER FUNCTIONS ===============================

_LIT8(KSlider, "slider" );

// -----------------------------------------------------------------------------
// CXnVolumeControlFactory::MakeXnComponentL
// Creates the Xuikon component for label control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnVolumeControlFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
        								)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if(aNode.Type()->Type() == KSlider)
        {
    	  CXnVolumeControl* container = CXnVolumeControl::NewL();	
        aTargetComponent = static_cast<CXnComponent*>(container);
  	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
	    }
    return retVal;  
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlFactory::MakeXnControlAdapterL
// Creates the control adapter for label control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnVolumeControlFactory::MakeXnControlAdapterL(
                                            CXnNodePluginIf& aNode,
                                            CXnControlAdapter* aParent)
    {
    return CXnVolumeControlAdapter::NewL(aParent, aNode);
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnVolumeControlFactory* CXnVolumeControlFactory::NewL()
    {
    CXnVolumeControlFactory* self = new( ELeave ) CXnVolumeControlFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlFactory::CXnVolumeControlFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnVolumeControlFactory::CXnVolumeControlFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlFactory::~CXnVolumeControlFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnVolumeControlFactory::~CXnVolumeControlFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnVolumeControlFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_VOLUMECONTROLFACTORY_IMPLEMENTATION, CXnVolumeControlFactory::NewL) 
#else
    {{AI3_UID_RENDERING_PLUGIN_VOLUMECONTROLFACTORY_IMPLEMENTATION}, CXnVolumeControlFactory::NewL}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
