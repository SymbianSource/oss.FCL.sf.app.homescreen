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
* Description:  Implementation wrapper for CEikEdwin
*
*/

#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>
#include "xncomponent.h"
#include "xntexteditor.h"
#include "xntexteditoradapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnTextEditor::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnTextEditor* CXnTextEditor::NewL()
    {
	CXnTextEditor* self = new( ELeave ) CXnTextEditor;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnTextEditor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnTextEditor::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnTextEditor::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnTextEditor::CXnTextEditor()
    {
    }

// -----------------------------------------------------------------------------
// CXnTextEditor::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnTextEditor::~CXnTextEditor()
    {
    }

// ---------------------------------------------------------
// CXnTextEditor::SetTextL
// ---------------------------------------------------------
//
void CXnTextEditor::SetTextL(const TDesC& aText)
    {
    (static_cast<CXnTextEditorAdapter*>(ControlAdapter()))->SetTextL(aText);
    }
    
// ---------------------------------------------------------
// CXnTextEditor::Text
// ---------------------------------------------------------
//
const HBufC* CXnTextEditor::Text()
    {
    return (static_cast<CXnTextEditorAdapter*>(ControlAdapter()))->Text();   
    }
  
// ---------------------------------------------------------
// CXnTextEditor::HandleEditorEvent
// ---------------------------------------------------------
//
void CXnTextEditor::HandleEditorEvent( TInt aReason )
    {
    static_cast<CXnTextEditorAdapter*>(ControlAdapter())->
            HandleEditorEvent( aReason );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnTextEditor::MakeInterfaceL(const TDesC8& aType)
    {
    if( aType != XnTextEditorInterface::KType )
        {
        return NULL;
        }
        
    XnTextEditorInterface::MXnTextEditorInterface* editorIf = static_cast< XnTextEditorInterface::MXnTextEditorInterface* >( this );
    
    return editorIf;
    }        

// End of file
