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
* Description:  Implementation for factory creating container for a box
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "viewfactory.h"
#include "view.h"
#include "viewcontrol.h"
#include "xncontroladapter.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include "xntype.h"
#include <aisystemuids.hrh>

// CONSTANTS
_LIT8(KView, "view");

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnViewFactory::MakeXnComponentL
// Creates the Xuikon component for view control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnViewFactory::MakeXnComponentL(  
    CXnNodePluginIf& aNode, CXnComponent*& aTargetComponent )
    {     
    // Check that the given type of a control is parent (or ancestor) of this control
    if( aNode.Type()->Type() == KView )
        {
        CXnViewPlugin* container = CXnViewPlugin::NewL();
        aTargetComponent = static_cast< CXnComponent* >( container );
        
        return MXnComponentFactory::EXnFactoryResponseComponentConstructed;;
	    }
    
    return MXnComponentFactory::EXnFactoryResponseComponentNotSupported;;
    }


// -----------------------------------------------------------------------------
// CXnViewFactory::MakeXnControlAdapterL
// Creates the control adapter for view control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnViewFactory::MakeXnControlAdapterL( 
   CXnNodePluginIf& aNode,
   CXnControlAdapter* /*aParent*/)
    {
    return CViewControl::NewL( aNode ); 
    }

// -----------------------------------------------------------------------------
// CXnViewFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnViewFactory* CXnViewFactory::NewL()
    {
    CXnViewFactory* self = new( ELeave ) CXnViewFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewFactory::CXnViewFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnViewFactory::CXnViewFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnViewFactory::~CXnViewFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnViewFactory::~CXnViewFactory()
    {    
    }

// -----------------------------------------------------------------------------
// CXnViewFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnViewFactory::ConstructL()
    {
    }

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY(AI3_UID_RENDERING_PLUGIN_VIEWFACTORY_IMPLEMENTATION, CXnViewFactory::NewL) 
#else
    {{AI3_UID_RENDERING_PLUGIN_VIEWFACTORY_IMPLEMENTATION}, CXnViewFactory::NewL}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

// End of File
