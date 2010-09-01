/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   
*
*/


#include <e32base.h>
#include <e32cmn.h>
#include <xmlengelement.h> 
#include <gulicon.h>
#include <xmlengnodelist.h>
#include <liwcommon.h>

#include "hnmdbutton.h"
#include "hnmdkeyfactory.h"
#include "hnconditioninterface.h"
#include "hnconditionfactory.h"
#include "hnmdvalueimage.h"
#include "hnmdvaluetext.h"
#include "hnglobals.h"
#include "hnattributebase.h"
#include "hnattributeimage.h"
#include "hnattributebase.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdButton* CHnMdButton::NewLC( TXmlEngElement aElement,
                                            THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdButton* self = new( ELeave ) CHnMdButton;
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdButton::~CHnMdButton( )
    {
    delete iButtonText;
    delete iConditionDimm;
    delete iConditionAdd;
    delete iIcon;
    }
       
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdButton::ValidateToAddL( const CLiwGenericParamList& aQueryResults, 
                                            TInt aPos )
    {
    TBool ret( EFalse );
    if( !iConditionAdd || iConditionAdd->ResultL( aQueryResults, aPos ) )
        {
        ret = ETrue;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdButton::ValidateToDimmL( 
        const CLiwGenericParamList& aQueryResults, TInt aPos )
    {
    TBool ret( EFalse );
    if( iConditionDimm )
        {
        ret = iConditionDimm->ResultL( aQueryResults, aPos );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//       
TInt CHnMdButton::GetEventId() const
    {
    return iEventId;
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//       
TInt CHnMdButton::GetIndex() const
    {
    return iIndex;
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//       
const TPtrC CHnMdButton::EvaluateButtonTextL(
        const CLiwGenericParamList& aQueryResults, TInt aPos )
    {
    if( !iButtonText )
        {
        return KNullDesC();
        }
    
    TPtrC value;
    iButtonText->GetL( &aQueryResults, aPos, value );
    return value;
    }
       
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttributeBase* CHnMdButton::EvaluateIconL( 
        const CLiwGenericParamList& aQueriesResultsList , TInt aPos ) const
    {
    //CGulIcon* icon = NULL;
    CHnAttributeBase* iconAttribute = NULL;
    if( iIcon )
        {
        iconAttribute =
            iIcon->CreateAttributeL( &aQueriesResultsList, aPos );
        }
    return iconAttribute;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdButton::CHnMdButton() : iIndex( KErrNone )
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdButton::ConstructL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs )
    {
    // index
    if( aElement.HasAttributeL( KIndexAttrName8 ) )
        {
        TPtrC8 index = aElement.AttributeValueL( KIndexAttrName8 );
        TLex8 lexIndex( index );
        User::LeaveIfError( lexIndex.Val( iIndex ) );
        }

    // event
    if( aElement.HasAttributeL( KEventAttrName8 ) )
        { 
        TPtrC8 event = aElement.AttributeValueL( KEventAttrName8 );
        TLex8 lex( event );
        User::LeaveIfError( lex.Val( iEventId ) );
        }
        
    // condition
    if( aElement.HasAttributeL( KMenuConditionAttrName8 ) )
        {
        iConditionAdd = HnConditionFactory::NewL(  
                aElement.AttributeNodeL( KMenuConditionAttrName8 ) );
        }
    
    // dimmed
    if( aElement.HasAttributeL( KMenuDimmAttrName8 ) )
        {
        iConditionDimm = HnConditionFactory::NewL(  
                aElement.AttributeNodeL( KMenuDimmAttrName8 ) );
        }
    
    // help text
    RXmlEngNodeList< TXmlEngElement > children;
    CleanupClosePushL( children );
    aElement.GetChildElements( children );
    TInt count = children.Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        TXmlEngElement child = children.Next();
        if ( !child.Name().Compare( KTextElementItem8 ) )
            {
            iButtonText = CHnMdValueText::NewL( child, aCmnPtrs );
            }
        else if( !child.Name().Compare( KImageElementItem8 ) )
            {
            iIcon = CHnMdValueImage::NewL( child, aCmnPtrs );
            }
        }
    
    CleanupStack::PopAndDestroy( &children );
    }

