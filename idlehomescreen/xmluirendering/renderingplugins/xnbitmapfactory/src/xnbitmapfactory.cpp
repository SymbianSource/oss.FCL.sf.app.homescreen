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
* Description:  Implementation for factory creating container for Active Idle plugins.
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>
#include "xntype.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"

#include "xnbitmapfactory.h"
#include "xnbitmapadapter.h"

#include "xnbitmap.h"
#include <aisystemuids.hrh>

// ============================ MEMBER FUNCTIONS ===============================


_LIT8(KXnBitmap, "image");
// -----------------------------------------------------------------------------
// CXnBitmaplFactory::MakeXnComponentL
// -----------------------------------------------------------------------------
//

MXnComponentFactory::TXnComponentFactoryResponse CXnBitmapFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
										)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if (aNode.Type()->Type() == KXnBitmap)
        {
        aTargetComponent = CXnBitmap::NewL();
        retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
	    }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnBitmaplFactory::MakeXnControlAdapterL
// -----------------------------------------------------------------------------
//

CXnControlAdapter* CXnBitmapFactory::MakeXnControlAdapterL( 
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* /*aParent*/
										)
    {
    return CXnBitmapAdapter::NewL(aNode);
    }


// -----------------------------------------------------------------------------
// CXnBitmaplFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnBitmapFactory* CXnBitmapFactory::NewL()
    {
    CXnBitmapFactory* self = new( ELeave ) CXnBitmapFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnBitmapFactory::CXnBitmapFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnBitmapFactory::CXnBitmapFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnBitmapFactory::~CXnBitmapFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnBitmapFactory::~CXnBitmapFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnBitmapFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnBitmapFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    #ifdef __EABI__ 
    	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_BITMAPFACTORY_IMPLEMENTATION, CXnBitmapFactory::NewL)
    #else
        {{AI3_UID_RENDERING_PLUGIN_BITMAPFACTORY_IMPLEMENTATION}, CXnBitmapFactory::NewL}
    #endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
