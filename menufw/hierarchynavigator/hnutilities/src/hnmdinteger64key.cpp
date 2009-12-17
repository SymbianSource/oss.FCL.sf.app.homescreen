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

#include "hnmdinteger64key.h"
#include "hnglobals.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdInteger64Key* CHnMdInteger64Key::NewL()
    {
    CHnMdInteger64Key* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdInteger64Key* CHnMdInteger64Key::NewLC()
    {
    CHnMdInteger64Key* self = new ( ELeave ) CHnMdInteger64Key();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdInteger64Key* CHnMdInteger64Key::NewL( const CHnMdInteger64Key* aKey )
    {
    CHnMdInteger64Key* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdInteger64Key* CHnMdInteger64Key::NewLC( const CHnMdInteger64Key* aKey )
    {
    CHnMdInteger64Key* self = new ( ELeave ) CHnMdInteger64Key();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// From class CHnMdBaseKey
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdInteger64Key::CopyLC()
    {
    CHnMdInteger64Key* self = CHnMdInteger64Key::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdInteger64Key::~CHnMdInteger64Key()
    {
    }

// ---------------------------------------------------------------------------
// From class CHnMdBaseKey
// converts decimal or hexadecimal string into integer 
// ---------------------------------------------------------------------------
//
void CHnMdInteger64Key::GetKeyContent( TInt64& aKeyContent ) const
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
CHnMdInteger64Key::CHnMdInteger64Key()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdInteger64Key::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdInteger64Key::ConstructL( const CHnMdInteger64Key* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// From class CHnMdBaseKey
// ---------------------------------------------------------------------------
//
void CHnMdInteger64Key::ToVariantL( TLiwVariant& aRet ) const
    {
    TInt64 decimalValue( KErrNotFound );
    GetKeyContent( decimalValue );
    aRet.SetL( TLiwVariant( ( TInt64 ) decimalValue ) );
    }

