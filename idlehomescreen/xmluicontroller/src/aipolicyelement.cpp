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
* Description:  Policy element
*
*/


#include "aipolicyelement.h"
#include "aixmluiconstants.h"
#include "xmluicontrollerpanic.h"

using namespace AiXmlUiController;

// ======== MEMBER FUNCTIONS ========

TAiPolicyElement::TAiPolicyElement()
    : iTarget( NULL ),
      iPolicy( KNullDesC8 ),
      iSeparator( KErrNotFound )
    {
    }

TAiPolicyElement::TAiPolicyElement( CXnNodeAppIf& aTarget, const TDesC8& aPolicy )
    : iTarget( &aTarget ),
      iPolicy( aPolicy )
    {
    iSeparator = iPolicy.Find( AiUiDef::xml::css::KSeparator );
    }

CXnNodeAppIf& TAiPolicyElement::Target() const
    {
    if ( !iTarget )
        {
        AiXmlUiController::Panic( ETargetNodeNull );
        }
        
    return *iTarget;
    }

TPtrC8 TAiPolicyElement::Name() const
    {
    if ( iSeparator < 0 )
        {
        return TPtrC8( KNullDesC8 );
        }
        
    return TLex8( iPolicy.Left( iSeparator ) ).NextToken();
    }

TPtrC8 TAiPolicyElement::Policy() const
    {
    return iPolicy;
    }

TPtrC8 TAiPolicyElement::Value() const
    {
    if ( iSeparator < 0 )
        {
        return TPtrC8( KNullDesC8 );
        }
        
    TPtrC8 value( TLex8( iPolicy.Mid( iSeparator + 1 ) ).NextToken() );
    
    TInt terminatorPos = value.Find( AiUiDef::xml::css::KValueTerminator );
    
    if ( terminatorPos > KErrNotFound )
        {
        return value.Left( terminatorPos );
        }
    else
        {
        return value;
        }
    }
