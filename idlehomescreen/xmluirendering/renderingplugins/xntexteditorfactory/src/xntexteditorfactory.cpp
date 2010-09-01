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
* Description:  Implementation for factory creating CXnEditor-element
* 
*/

#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xntexteditorfactory.h"
#include "xntexteditoradapter.h"
#include "xntype.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include "xndomnode.h"
#include "aisystemuids.hrh"

#include "xntexteditor.h"

_LIT8( KXnTextEditor, "texteditor" );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CXnTextEditorFactory::MakeXnComponentL
// Creates the Xuikon component for editor control
// -----------------------------------------------------------------------------
//
MXnComponentFactory::TXnComponentFactoryResponse CXnTextEditorFactory::MakeXnComponentL( 
                                        CXnNodePluginIf& aNode,
                                        CXnComponent*& aTargetComponent
        								)
    {
    MXnComponentFactory::TXnComponentFactoryResponse retVal = MXnComponentFactory::EXnFactoryResponseComponentNotSupported;
    
    // Check that the given type of a control is parent (or ancestor) of this control
    if( aNode.Type()->Type() == KXnTextEditor )
        {
    	CXnTextEditor* container = CXnTextEditor::NewL();
    	
        aTargetComponent = static_cast< CXnComponent* >( container );
        
  	    retVal = MXnComponentFactory::EXnFactoryResponseComponentConstructed;	    	    
	    }
	    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnTextEditorFactory::MakeXnControlAdapterL
// Creates the control adapter for editor control
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnTextEditorFactory::MakeXnControlAdapterL(
                                        CXnNodePluginIf& aNode,
                                        CXnControlAdapter* aParent)
    {
    return CXnTextEditorAdapter::NewL( aParent, aNode );
    }

// -----------------------------------------------------------------------------
// CXnTextEditorFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnTextEditorFactory* CXnTextEditorFactory::NewL()
    {
    CXnTextEditorFactory* self = new( ELeave ) CXnTextEditorFactory;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnTextEditorFactory::CXnTextEditorFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnTextEditorFactory::CXnTextEditorFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnTextEditorFactory::~CXnTextEditorFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CXnTextEditorFactory::~CXnTextEditorFactory()
    {
    }

// -----------------------------------------------------------------------------
// CXnTextEditorFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnTextEditorFactory::ConstructL()
    {
    }


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY( AI3_UID_RENDERING_PLUGIN_TEXTEDITORFACTORY_IMPLEMENTATION, CXnTextEditorFactory::NewL ) 
#else
    { { AI3_UID_RENDERING_PLUGIN_TEXTEDITORFACTORY_IMPLEMENTATION }, CXnTextEditorFactory::NewL }
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// End of file
