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


#include <liwservicehandler.h>
#include <e32hashtab.h>

#include "menudebug.h"
#include "hnmduimapping.h"
#include "hnmduimappingelement.h"
#include "hnutils.h"
#include "hnglobals.h"
#include "hnitemmodel.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUiMapping::ConstructL( TXmlEngElement aElement,
                                 THnMdCommonPointers* aCmnPtrs )
    {
    RXmlEngNodeList< TXmlEngElement > children;
    CleanupClosePushL(children);
    aElement.GetChildElements(children);
    TInt amount = children.Count();
    
    TPtrC8 n = aElement.Name();

    for (TInt j = 0; j < amount; j++ )
        {
        TXmlEngElement item = children.Next();
        
        if ( !item.Name().Compare( KOutputElementItem8 ) )
            {
            CHnMdUiMappingElement *element =
                CHnMdUiMappingElement::NewL( item , aCmnPtrs );
            this->AddUiMappingElementL( element );
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUiMapping::AddUiMappingElementL( 
    CHnMdUiMappingElement *aUiMappingElement )
    {
    iMappings.AppendL( aUiMappingElement );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMapping* CHnMdUiMapping::NewL( TXmlEngElement aElement,
                                      THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdUiMapping* self = CHnMdUiMapping::NewLC( aElement, aCmnPtrs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMapping* CHnMdUiMapping::NewLC( TXmlEngElement aElement,
                                       THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdUiMapping* self = new( ELeave ) CHnMdUiMapping;
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMapping::CHnMdUiMapping()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUiMapping::~CHnMdUiMapping()
    {
    iMappings.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdUiMapping::FillGraphicalItemL( CHnItemModel* aItemModel ,
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    RPointerArray< TDesC8 > aliases;
    CleanupClosePushL( aliases );
    
    TBool ret( ETrue );
    for ( TInt i(0); i < iMappings.Count(); i++ )
        {     
        CHnMdUiMappingElement* uiElem = iMappings[ i ];
        ASSERT( uiElem );
        TBool alreadyFilled( EFalse );
        const TDesC8* alias = &uiElem->Alias();
        for( TInt index( 0 ); index < aliases.Count(); index++ )
            {
            if ( !aliases[ index ]->Compare( *alias ) )
                {
                alreadyFilled = ETrue;
                }
            }
        if (  !alreadyFilled && uiElem->IsValidL( aQueriesResultsList, aPos ) )
            {
            aliases.AppendL( alias );
            ret = uiElem->FillGraphicalItemL( aItemModel, aQueriesResultsList, aPos );
            if ( !ret )
                {
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( &aliases );
    return ret;
    }

