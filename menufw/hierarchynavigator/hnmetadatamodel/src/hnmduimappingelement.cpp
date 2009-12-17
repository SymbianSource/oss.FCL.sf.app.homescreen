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


#include "hnmduimappingelement.h"
#include "hnmdkeyfactory.h"
#include "hnmdbasekey.h"
#include "hnconvutils.h"
#include "menudebug.h"
#include "hnitemmodel.h"
#include "bautils.h"
#include "hnglobals.h"
#include "hnmdvaluebase.h"
#include "hnmdmodel.h"
#include "hninterface.h"
#include "hnconditioninterface.h"
#include "hnattributebase.h"
#include "hnattributeimage.h"
#include "hnconditionfactory.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUiMappingElement::ConstructL( const TXmlEngElement& aElement,
                                        THnMdCommonPointers* aCmnPtrs )
    {
    ASSERT( !aElement.Name().Compare( KOutputElementItem8 ) );
 
    // set alias
    SetAliasL( aElement.AttributeNodeL( KAliasAttrName8 ) );
    
    // set condition
    if( aElement.AttributeValueL( KMenuConditionAttrName8 ).Length() )
        {
        iCondition = HnConditionFactory::NewL( 
                aElement.AttributeNodeL( KMenuConditionAttrName8 ) );
        }
    
    // common pointers
    iCmnPtrs = aCmnPtrs;
    
    if ( aElement.IsSimpleTextContents() == EFalse &&
            aElement.HasChildNodes() )
        {
        // get child element from output and pass to set value
        RXmlEngNodeList< TXmlEngElement > childElements;
        CleanupClosePushL( childElements );
        aElement.GetChildElements( childElements );
        TXmlEngElement childElement = childElements.Next();
        // it could be text/image node
        SetValueL( childElement, aCmnPtrs );
        CleanupStack::PopAndDestroy( &childElements );
        }
    else
        {
        // handle output node
        SetValueL( aElement, aCmnPtrs );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnMdUiMappingElement::Alias() const
    {
    return iAlias;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUiMappingElement::SetAliasL( const TXmlEngAttr& aName )
    {
    iAlias.Close();
    iAlias.CreateL( aName.Value() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUiMappingElement::SetValueL( const TXmlEngElement& aValue,
        THnMdCommonPointers* aCmnPtrs )
    {
    delete iValue;
    iValue = NULL;
    iValue = CHnMdValueBase::CreateL( aValue, aCmnPtrs );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMappingElement* CHnMdUiMappingElement::NewL( TXmlEngElement aElement,
                                              THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdUiMappingElement* self = CHnMdUiMappingElement::NewLC( aElement,
                                                                aCmnPtrs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMappingElement* CHnMdUiMappingElement::NewLC( TXmlEngElement aElement,
                                              THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdUiMappingElement* self = new( ELeave ) CHnMdUiMappingElement;
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMappingElement::CHnMdUiMappingElement()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMappingElement::~CHnMdUiMappingElement()
    {
    iAlias.Close();
    delete iValue;
    delete iCondition;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdUiMappingElement::FillGraphicalItemL( CHnItemModel* aItemModel ,
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    TBool ret = EFalse;
    CHnAttributeBase* attr = iValue->CreateAttributeL( 
            &aQueriesResultsList, aPos );
    
    if( attr )
        {
        CleanupStack::PushL( attr );
        attr->SetNameL( iAlias );
        aItemModel->SetAttributeL( attr ); 
        CleanupStack::Pop( attr );
        ret = ETrue;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdUiMappingElement::IsValidL( 
                    const CLiwGenericParamList& aQueryResults, TInt aPos )    
    {
    TBool ret( EFalse );
    
    if( !iCondition || iCondition->ResultL( aQueryResults, aPos ) )
        {
        ret = ETrue;
        }
    
    return ret;
    }

