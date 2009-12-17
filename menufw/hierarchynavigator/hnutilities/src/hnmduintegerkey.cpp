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

#include "hnmduintegerkey.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUIntegerKey* CHnMdUIntegerKey::NewL()
    {
    CHnMdUIntegerKey* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUIntegerKey* CHnMdUIntegerKey::NewLC()
    {
    CHnMdUIntegerKey* self = new ( ELeave ) CHnMdUIntegerKey();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUIntegerKey* CHnMdUIntegerKey::NewL( const CHnMdUIntegerKey* aKey )
    {
    CHnMdUIntegerKey* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUIntegerKey* CHnMdUIntegerKey::NewLC( const CHnMdUIntegerKey* aKey )
    {
    CHnMdUIntegerKey* self = new ( ELeave ) CHnMdUIntegerKey();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdUIntegerKey::CopyLC()
    {
    CHnMdUIntegerKey* self = CHnMdUIntegerKey::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUIntegerKey::~CHnMdUIntegerKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUIntegerKey::CHnMdUIntegerKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUIntegerKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUIntegerKey::ConstructL( const CHnMdUIntegerKey* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUIntegerKey::ToVariantL( TLiwVariant& aRet ) const
    {
    TLex lex( KeyContent() );
    TUint uIntVal( KErrNone );
    lex.Val( uIntVal );
    aRet.SetL( TLiwVariant( uIntVal ) ); // crates a copy of the data
    }

