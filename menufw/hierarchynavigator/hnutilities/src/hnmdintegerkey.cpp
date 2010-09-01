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

#include "hnmdintegerkey.h"
#include "hnglobals.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdIntegerKey* CHnMdIntegerKey::NewL()
    {
    CHnMdIntegerKey* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdIntegerKey* CHnMdIntegerKey::NewLC()
    {
    CHnMdIntegerKey* self = new ( ELeave ) CHnMdIntegerKey();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdIntegerKey* CHnMdIntegerKey::NewL( const CHnMdIntegerKey* aKey )
    {
    CHnMdIntegerKey* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdIntegerKey* CHnMdIntegerKey::NewLC( const CHnMdIntegerKey* aKey )
    {
    CHnMdIntegerKey* self = new ( ELeave ) CHnMdIntegerKey();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdIntegerKey::CopyLC()
    {
    CHnMdIntegerKey* self = CHnMdIntegerKey::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdIntegerKey::~CHnMdIntegerKey()
    {
    }

// ---------------------------------------------------------------------------
// converts decimal or hexadecimal string into integer 
// ---------------------------------------------------------------------------
//
void CHnMdIntegerKey::GetKeyContent( TInt& aKeyContent ) const
    {
    TLex lex( iContent );
    TInt err( KErrNone );
    if ( !iContent.Find( KHexPrefix16 ) )
        {
        // hexadecimal
        TUint hex;
        lex.Inc( KHexPrefix16().Length() );
        lex.Mark();
        err = lex.Val( hex, EHex );
        aKeyContent = hex;
        }
    else
        {
        // decimal
        err = lex.Val( aKeyContent );
        }

    if ( err != KErrNone )
        {
        aKeyContent = err;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdIntegerKey::CHnMdIntegerKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdIntegerKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdIntegerKey::ConstructL( const CHnMdIntegerKey* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdIntegerKey::ToVariantL( TLiwVariant& aRet ) const
    {
    TInt decimalValue( KErrNotFound );
    GetKeyContent( decimalValue );
    aRet.SetL( TLiwVariant( ( TInt32 ) decimalValue ) );
    }

