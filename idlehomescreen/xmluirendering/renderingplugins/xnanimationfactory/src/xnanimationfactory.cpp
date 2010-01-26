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
#include "xnanimationfactory.h"
#include "xnanimationadapter.h"
#include "xnanimation.h"
#include <aisystemuids.hrh>

// ============================ MEMBER FUNCTIONS ===============================


_LIT8(KXnAnimation, "animation");
// -----------------------------------------------------------------------------
// CXnAnimationFactory::MakeXnComponentL
// -----------------------------------------------------------------------------
//

MXnComponentFactory::TXnComponentFactoryResponse CXnAnimationFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
										)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    // Check that the given type of a control is parent (or ancestor) of this control
    if (aNode.Type()->Type() == KXnAnimation)
        {
        aTargetComponent = CXnAnimation::NewL();
        retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
	    }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnAnimationFactory::MakeXnControlAdapterL
// -----------------------------------------------------------------------------
//

CXnControlAdapter* CXnAnimationFactory::MakeXnControlAdapterL( 
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* /*aParent*/
										)
    {
    return CXnAnimationAdapter::NewL(aNode);
    }


// -----------------------------------------------------------------------------
// CXnAnimationFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnAnimationFactory* CXnAnimationFactory::NewL()
    {
    CXnAnimationFactory* self = new( ELeave ) CXnAnimationFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnAnimationFactory::CXnAnimationFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnAnimationFactory::CXnAnimationFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnAnimationFactory::~CXnAnimationFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnAnimationFactory::~CXnAnimationFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnAnimationFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnAnimationFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    #ifdef __EABI__ 
    	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_ANIMATIONFACTORY_IMPLEMENTATION, CXnAnimationFactory::NewL)
    #else
        {{AI3_UID_RENDERING_PLUGIN_ANIMATIONFACTORY_IMPLEMENTATION}, CXnAnimationFactory::NewL}
    #endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
