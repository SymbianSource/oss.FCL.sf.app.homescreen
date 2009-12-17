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

#include "hnmduidkey.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUidKey* CHnMdUidKey::NewL()
    {
    CHnMdUidKey* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUidKey* CHnMdUidKey::NewLC()
    {
    CHnMdUidKey* self = new ( ELeave ) CHnMdUidKey();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUidKey* CHnMdUidKey::NewL(const CHnMdUidKey* aKey)
    {
    CHnMdUidKey* self = NewLC(aKey);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUidKey* CHnMdUidKey::NewLC( const CHnMdUidKey* aKey )
    {
    CHnMdUidKey* self = new ( ELeave ) CHnMdUidKey();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdUidKey::CopyLC()
    {
    CHnMdUidKey* self = CHnMdUidKey::NewL( this);
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUidKey::~CHnMdUidKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUidKey::GetKeyContent( TInt& aKeyContent ) const
    {
    // converts hexadecimal string into integer
    TLex lex( iContent );
    TUint a;
    lex.Inc( 2 );
    lex.Mark();

    TInt err = lex.Val( a, EHex );

    if ( err == KErrNone )
        {
        aKeyContent = a;
        }
    else
        {
        aKeyContent = err;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUidKey::CHnMdUidKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUidKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUidKey::ConstructL( const CHnMdUidKey* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUidKey::ToVariantL( TLiwVariant& aRet ) const
    {
    TLiwVariant ret;
    TInt intVal( KErrGeneral );
    GetKeyContent( intVal );
    aRet.SetL( TLiwVariant( TUid::Uid( intVal ) ) ); // creates a copy of the data
    }

