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
* Description:  News ticker transaction element
*
*/


#include    "newstickertransactionelement.h"
#include    "aixmluiutils.h"

#include    "xntype.h"
#include    "xnproperty.h"
#include    "xntext.h"
#include    "xnmenuadapter.h"
#include    "xnnewsticker.h"
//#include    <xnmarquee.h>
#include    "mxncomponentinterface.h"
#include    "xnnodeappif.h"
#include    "xnuiengineappif.h"
#include    "xnproperty.h"
#include    "xndompropertyvalue.h"
#include    "xnvolumecontrol.h"
#include    "aistrcnv.h"

// ============================ MEMBER FUNCTIONS ===============================

using namespace AiXmlUiController;
using namespace XnTextInterface;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CNewsTickerTransactionElement::CNewsTickerTransactionElement(AiUtility::CContentPriorityMap& aContentPriorityMap)
    : CTransactionElement(aContentPriorityMap)
    {
    }    

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CNewsTickerTransactionElement* CNewsTickerTransactionElement::NewL(AiUtility::CContentPriorityMap& aContentPriorityMap)
    {
    CNewsTickerTransactionElement* self = new( ELeave ) CNewsTickerTransactionElement(aContentPriorityMap);
    
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CNewsTickerTransactionElement::~CNewsTickerTransactionElement()
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CNewsTickerTransactionElement::InitializeL( CXnNodeAppIf& aTarget, const TDesC& aText, TInt aIndex )
    {
    CheckTypeL( aTarget );
    SetTarget( aTarget );
    iNewText.Set( aText );
    iIndex = aIndex;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CNewsTickerTransactionElement::UpdateDataL()
    {
    // Set new text
    SetTextL();
    
    // Update content priority
    UpdateContentPriorityL();
    }
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CNewsTickerTransactionElement::Reset()
    {
    CTransactionElement::Reset();
    
    iNewText.Set( KNullDesC );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CNewsTickerTransactionElement::IsSupported( CXnNodeAppIf& aTarget )
    {
    //  Is supported only if parent is <newsticker>
    CXnNodeAppIf* ntNode = NULL;
    
    TRAP_IGNORE( ntNode = aTarget.ParentL() );
    
    if ( !ntNode )
        {
        return EFalse;
        }
    
    CXnType* typeInfo = ntNode->Type();
    
    if ( !typeInfo )
        {
        return EFalse;
        }
    
    const TDesC8& type = typeInfo->Type();
    
    // Text element, menu item, newsticker, and marquee supported
    return ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CNewsTickerTransactionElement::CheckTypeL( CXnNodeAppIf& aTarget )
    {
    if ( !IsSupported( aTarget ) )
        {
        User::Leave( KErrNotSupported );
        }
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CNewsTickerTransactionElement::SetTextL()
    {      
    // Titles are published to <newsticker> children elements
    CXnNodeAppIf* ntNode = Target().ParentL();
    LeaveIfNull( ntNode, KErrNotSupported );
    
    // Get type info
    const TDesC8& type = LeaveIfNull( ntNode->Type(), KErrNotSupported )->Type();
    if ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() )
        {
        // Get Newsticker control interface
        XnNewstickerInterface::MXnNewstickerInterface* newsTicker = NULL;

        XnComponentInterface::MakeInterfaceL( newsTicker, *ntNode );
        LeaveIfNull( newsTicker, KErrNotSupported );
        
        TInt titleIndexVal = -1;
        CXnProperty* titleIndex = Target().GetPropertyL( XnPropertyNames::title::KTitleIndex );
        if ( titleIndex )
            {
            TLex8 lex( titleIndex->StringValue() );
            TInt err = lex.Val( titleIndexVal );
            if ( err == KErrNone && titleIndexVal > -1 )
                {
                newsTicker->UpdateTitleL( iNewText, titleIndexVal );            
                }            
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }
    
//  End of File
