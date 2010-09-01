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
* Description:  Transaction element for publishing empty content
*
*/


#include "xntype.h"
#include "xnnewsticker.h"
#include "xnbitmap.h"
#include "xntext.h"
#include "xntexteditor.h"
#include "emptycontenttransactionelement.h"
#include "aixmluiconstants.h"
#include "aixmluiutils.h"

using namespace AiXmlUiController;
using namespace XnImageInterface;
using namespace XnTextInterface;
using namespace AiUiDef::xml;

const TInt KControlIndexOffset = 1;

// ======== MEMBER FUNCTIONS ========

CEmptyContentTransactionElement::CEmptyContentTransactionElement(
                                AiUtility::CContentPriorityMap& aContentPriorityMap)
    : CTransactionElement( aContentPriorityMap ),
      iIndex( KErrNotFound )
    {
    }

CEmptyContentTransactionElement* CEmptyContentTransactionElement::NewL(
                                AiUtility::CContentPriorityMap& aContentPriorityMap)
    {
    CEmptyContentTransactionElement* self =
            new( ELeave ) CEmptyContentTransactionElement( aContentPriorityMap );
    return self;
    }

CEmptyContentTransactionElement::~CEmptyContentTransactionElement()
    {
    }

void CEmptyContentTransactionElement::InitializeL( CXnNodeAppIf& aTarget, TInt aIndex )
    {
    SetTarget( aTarget );
    
    // Indexes in XML theme run 1...n, but in control interface 0...n-1
    iIndex = aIndex - KControlIndexOffset;
    }

void CEmptyContentTransactionElement::Reset()
    {
    CTransactionElement::Reset();
    iIndex = KErrNotFound;
    }

void CEmptyContentTransactionElement::UpdateDataL()
    {
    CXnType* type = Target().Type();

    if ( type->Type() == XnImageInterface::MXnImageInterface::Type() )
        {
        MXnImageInterface* image( NULL );
        XnComponentInterface::MakeInterfaceL( image, Target() );

        LeaveIfNull( image, KErrNotSupported );
        
        // Clears the bitmaps from component
        image->SetContentBitmaps( NULL, NULL );
        }
    else if ( type->Type() == XnTextInterface::MXnTextInterface::Type() ) // text element
        {
        // Get control interface
        XnTextInterface::MXnTextInterface* text( NULL );
        XnComponentInterface::MakeInterfaceL( text, Target() );
        
        LeaveIfNull( text, KErrNotSupported );
        
        // Clears the text from component
        text->SetTextL( KNullDesC );
        }
    else if ( type->Type() == XnTextEditorInterface::MXnTextEditorInterface::Type() ) // texteditor element
        {
        // Get control interface
        XnTextEditorInterface::MXnTextEditorInterface* editor( NULL );
        XnComponentInterface::MakeInterfaceL( editor, Target() );
        
        LeaveIfNull( editor, KErrNotSupported );
        
        // Clears the text from component
        editor->SetTextL( KNullDesC );
        }
    else
        {
        CXnNodeAppIf* parent( Target().ParentL() );
        LeaveIfNull( parent, KErrNotSupported );
        
        const TDesC8& parentType( parent->Type()->Type() );
            
        if ( parentType == XnNewstickerInterface::MXnNewstickerInterface::Type() )
            {
            XnNewstickerInterface::MXnNewstickerInterface* newsticker( NULL );
            XnComponentInterface::MakeInterfaceL( newsticker, *parent );
            
            LeaveIfNull( newsticker, KErrNotSupported );
            
            CXnProperty* prop( Target().GetPropertyL( 
                XnPropertyNames::title::KTitleIndex ) );
    
            LeaveIfNull( prop, KErrNotSupported );

            TInt index( KErrNotFound );
            
            TLex8 lex( prop->StringValue() );
            lex.Val( index );
            
            newsticker->DeleteTitle( index );
            }
        }
    
    // Clear current content priority
    ClearContentPriorityL();
    }

// End of file
