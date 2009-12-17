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
* Description:  Implementation for factory creating CXnText-element
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xntextfactory.h"
#include "xntextadapter.h"
#include "xntype.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include <aisystemuids.hrh>
#include "xndomnode.h"

#include "xntext.h"

// ============================ MEMBER FUNCTIONS ===============================


_LIT8(KXnText, "text");

// -----------------------------------------------------------------------------
// CXnEikLabelFactory::MakeXnComponentL
// Creates the Xuikon component for label control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnTextFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
        								)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if(aNode.Type()->Type() == KXnText)
        {
    	  CXnText* container = CXnText::NewL();
        aTargetComponent = static_cast<CXnComponent*>(container);
  	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
	      return retVal;
	    }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnTextFactory::MakeXnControlAdapterL
// Creates the control adapter for label control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnTextFactory::MakeXnControlAdapterL(
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* aParent)
    {
    return CXnTextAdapter::NewL(aParent, aNode);
    }

// -----------------------------------------------------------------------------
// CXnTextFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnTextFactory* CXnTextFactory::NewL()
    {
    CXnTextFactory* self = new( ELeave ) CXnTextFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnTextFactory::CXnTextFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnTextFactory::CXnTextFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnTextFactory::~CXnTextFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnTextFactory::~CXnTextFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnTextFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnTextFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_TEXTFACTORY_IMPLEMENTATION, CXnTextFactory::NewL) 
#else
    {{AI3_UID_RENDERING_PLUGIN_TEXTFACTORY_IMPLEMENTATION}, CXnTextFactory::NewL}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
