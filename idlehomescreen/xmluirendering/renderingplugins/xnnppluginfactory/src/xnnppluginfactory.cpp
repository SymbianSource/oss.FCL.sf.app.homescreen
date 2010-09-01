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
* Description:  Factory class for XnNPPlugin, which wraps the Netscape Plugin API.
*
*/



// INCLUDE FILES
#include "xnnppluginfactory.h"
#include "xnnppluginadapter.h"
#include "xnnpplugin.h"
#include "xntype.h"
#include "xnnodepluginif.h"
#include <aisystemuids.hrh>

// MACROS
_LIT8( KXnObject, "object" );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CXnEikLabelFactory::MakeXnComponentL
// Creates the Xuikon component for label control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnNPPluginFactory::MakeXnComponentL( 
    CXnNodePluginIf& aNode,
    CXnComponent*& aTargetComponent )
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = 
    MXnComponentFactory::EXnFactoryResponseComponentNotSupported;

    // Check that the given type of a control is parent (or ancestor) of this control
    if ( aNode.Type()->Type() == KXnObject )
        {
        aTargetComponent = CXnNPPlugin::NewL();
        retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;
        }
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnTextFactory::MakeXnControlAdapterL
// Creates the control adapter for label control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnNPPluginFactory::MakeXnControlAdapterL( 
    CXnNodePluginIf& aNode,
    CXnControlAdapter* aParent )
    {
    return CXnNPPluginAdapter::NewL( aParent, aNode );
    }


// -----------------------------------------------------------------------------
// CXnNPPluginFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNPPluginFactory* CXnNPPluginFactory::NewL()
    {
    CXnNPPluginFactory* self = new( ELeave ) CXnNPPluginFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnNPPluginFactory::CXnNPPluginFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNPPluginFactory::CXnNPPluginFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnNPPluginFactory::~CXnNPPluginFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnNPPluginFactory::~CXnNPPluginFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnNPPluginFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNPPluginFactory::ConstructL()
    {
    }
 
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    #ifdef __EABI__ 
        IMPLEMENTATION_PROXY_ENTRY( AI3_UID_RENDERING_PLUGIN_NPPLUGINFACTORY_IMPLEMENTATION, CXnNPPluginFactory::NewL )
    #else
        {{AI3_UID_RENDERING_PLUGIN_NPPLUGINFACTORY_IMPLEMENTATION}, CXnNPPluginFactory::NewL}
    #endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }
    
// End of file
