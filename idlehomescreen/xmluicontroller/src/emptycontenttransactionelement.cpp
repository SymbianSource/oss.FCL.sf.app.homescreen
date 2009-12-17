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
        MXnImageInterface* imageIntr = NULL;
        if ( !XnComponentInterface::MakeInterfaceL( imageIntr, Target() ) )
            {
            User::Leave( KErrNotSupported );
            }
        // Clears the bitmaps from component
        imageIntr->SetContentBitmaps( NULL, NULL );
        }
    else if ( type->Type() == XnTextInterface::MXnTextInterface::Type() ) // text element
            {
            // Get control interface
            XnTextInterface::MXnTextInterface* textControl = NULL;
            XnComponentInterface::MakeInterfaceL( textControl, Target() );
            LeaveIfNull( textControl, KErrNotSupported );
            
            // Clears the text from component
            textControl->SetTextL( KNullDesC );
            }
    else
        {
        CXnNodeAppIf* parent = Target().ParentL();
        LeaveIfNull( parent, KErrNotSupported );
        const TDesC8& type = LeaveIfNull( parent->Type(), KErrNotSupported )->Type();
            
        if ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() )
            {
            XnNewstickerInterface::MXnNewstickerInterface* newsticker = NULL;
            XnComponentInterface::MakeInterfaceL( newsticker, *parent );
            LeaveIfNull( newsticker, KErrGeneral );
            TInt titleIndexVal = -1;
            CXnProperty* titleIndex = Target().GetPropertyL( XnPropertyNames::title::KTitleIndex );
            if ( titleIndex )
                {
                TLex8 lex( titleIndex->StringValue() );
                TInt err = lex.Val( titleIndexVal );
                if ( err == KErrNone && titleIndexVal > -1 )
                    {
                    newsticker->DeleteTitle( titleIndexVal );
                    }
                }
            }
        }
    // Clear current content priority
    ClearContentPriorityL();
    }
