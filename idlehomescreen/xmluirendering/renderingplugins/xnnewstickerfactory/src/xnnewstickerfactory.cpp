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
* Description:  Implementation for factory creating CXn3Newsticker-element
*
*/


#include "xntype.h"
#include "xnnodepluginif.h"
#include "xnproperty.h"
#include <ecom/implementationproxy.h>

#include "xnnewstickerfactory.h"
#include "xnnewstickeradapter.h"
#include <aisystemuids.hrh>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Dll implements the Symbian OS dll entry point
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt  E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

// -----------------------------------------------------------------------------
// CXnEikLabelFactory::MakeXnComponentL
// Creates the Xuikon component for label control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnNewstickerFactory::MakeXnComponentL(
    CXnNodePluginIf& aNode, CXnComponent*& aTargetComponent)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = 
        MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if(aNode.Type()->Type() == XnPropertyNames::newsticker::KXnNewsticker)
        {
    	CXnNewsticker* container = CXnNewsticker::NewL();
    	aTargetComponent = static_cast<CXnComponent*>(container);
	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
	    return retVal;
	    }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerFactory::MakeXnControlAdapterL
// Creates the control adapter for label control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnNewstickerFactory::MakeXnControlAdapterL(
    CXnNodePluginIf& aNode, CXnControlAdapter* aParent)
    {
    return CXnNewstickerAdapter::NewL(aParent, aNode);
    }

// -----------------------------------------------------------------------------
// CXnNewstickerFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerFactory* CXnNewstickerFactory::NewL()
    {
    CXnNewstickerFactory* self = new(ELeave) CXnNewstickerFactory;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerFactory::CXnNewstickerFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNewstickerFactory::CXnNewstickerFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerFactory::~CXnNewstickerFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerFactory::~CXnNewstickerFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerFactory::ConstructL()
    {
    }

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_NEWSTICKERFACTORY_IMPLEMENTATION, CXnNewstickerFactory::NewL) 
#else
    {{AI3_UID_RENDERING_PLUGIN_NEWSTICKERFACTORY_IMPLEMENTATION}, CXnNewstickerFactory::NewL}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of File
