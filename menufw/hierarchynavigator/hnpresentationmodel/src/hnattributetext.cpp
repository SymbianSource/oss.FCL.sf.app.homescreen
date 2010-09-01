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
* Description:  text attribute model 
*
*/


#include "hnattributetext.h"


// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnAttributeText::CHnAttributeText()
    {
    iAttributeType = ETextAttribute;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnAttributeText::~CHnAttributeText()
    {
    iValue.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttributeText* CHnAttributeText::NewL()
    {
    CHnAttributeText* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttributeText* CHnAttributeText::NewLC()
    {
    CHnAttributeText* self = new ( ELeave ) CHnAttributeText;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnAttributeText::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeText::SetValueL ( const TDesC8& aValue )
    {
    iValue.Close();
    iValue.CreateL( aValue.Length() + 1 );
    iValue.Copy( aValue );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC8& CHnAttributeText::Value( )
    {
    return iValue;
    }

