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


#include <xmlengelement.h>
#include <xmlengnodelist.h>

#include "hnmdtoolbar.h"
#include "hnmdbutton.h"
#include "hnconvutils.h"
#include "hnglobals.h"
#include "hntoolbarmodel.h"
#include "hnbuttonmodel.h"
#include "hnattributebase.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdToolbar* CHnMdToolbar::NewLC( TXmlEngElement aElement,
                                            THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdToolbar* self = new( ELeave ) CHnMdToolbar;
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdToolbar* CHnMdToolbar::NewL( TXmlEngElement aElement,
      THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdToolbar* self = CHnMdToolbar::NewLC( aElement, aCmnPtrs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdToolbar::EvaluateL( CHnToolbarModel* aToolbar, 
                              const CLiwGenericParamList& aQueryResults, 
                              TInt aPos )
    {
    TInt buttonCount = iButtons.Count();
    for( TInt i( 0 ); i < buttonCount; i++ )
        {
        if( iButtons[ i ]->ValidateToAddL( aQueryResults, aPos ) && 
                aToolbar->GetButton( iButtons[ i ]->GetIndex() ) == NULL )
            {
            CHnButtonModel* button = CHnButtonModel::NewLC( 
                    iButtons[ i ]->GetIndex() );

        	const TDesC& helpText = iButtons[ i ]->EvaluateButtonTextL( aQueryResults, aPos );
        	button->SetHelpTextL( helpText );
            
            button->SetEventId( iButtons[ i ]->GetEventId() );
            button->SetDimmed( 
                    iButtons[ i ]->ValidateToDimmL( aQueryResults, aPos ) );
            
            CHnAttributeBase* attributeImage = iButtons[ i ]->EvaluateIconL( aQueryResults, aPos );
            button->SetIcon( attributeImage );
            
            aToolbar->AddButtonL( button );
            CleanupStack::Pop( button );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdToolbar::~CHnMdToolbar( )
    {
    iButtons.ResetAndDestroy();    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdToolbar::CHnMdToolbar()
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdToolbar::ConstructL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs )
    {
    TXmlEngElement toolbarElement;
    if ( aElement.Name().Compare( KMenuToolbar8 ) != 0 )
        {
        RXmlEngNodeList< TXmlEngElement > children;
        CleanupClosePushL( children );
        aElement.GetChildElements( children );
        TInt count = children.Count();
        
        for ( TInt i = 0; i < count; i++ )
            {
            TXmlEngElement child = children.Next();
            if ( !child.Name().Compare( KMenuToolbar8 ) )
                {
                CreateButtonsL( child, aCmnPtrs );
                }
            }
        CleanupStack::PopAndDestroy( &children );
        }
     
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdToolbar::CreateButtonsL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs )
    {
    RXmlEngNodeList< TXmlEngElement > children;
    CleanupClosePushL( children );
    aElement.GetChildElements( children );
    TInt count = children.Count();
        
    for ( TInt i = 0; i < count; i++ )
        {
        TXmlEngElement child = children.Next();
        if ( !child.Name().Compare( KMenuButton8 ) )
            {
            CHnMdButton* button = CHnMdButton::NewLC( child, aCmnPtrs );
            iButtons.AppendL( button );
            CleanupStack::Pop( button );
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

