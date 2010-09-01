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

#include "hnglobals.h"
#include "hnmdbooleankey.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBooleanKey* CHnMdBooleanKey::NewL()
    {
    CHnMdBooleanKey* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBooleanKey* CHnMdBooleanKey::NewLC()
    {
    CHnMdBooleanKey* self = new ( ELeave ) CHnMdBooleanKey();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBooleanKey* CHnMdBooleanKey::NewL( const CHnMdBooleanKey* aKey )
    {
    CHnMdBooleanKey* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBooleanKey* CHnMdBooleanKey::NewLC( const CHnMdBooleanKey* aKey )
    {
    CHnMdBooleanKey* self = new ( ELeave ) CHnMdBooleanKey();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdBooleanKey::CopyLC()
    {
    CHnMdBooleanKey* self = CHnMdBooleanKey::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBooleanKey::~CHnMdBooleanKey()
    {
    }
  
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnMdBooleanKey::GetBooleanValueL() const
    {
    TBool ret( EFalse );
    
    if ( !iContent.Compare( KStringFalse ) )
        {
        ret = EFalse;
        }
    else if ( !iContent.Compare( KStringTrue ) )
        {
        ret = ETrue;
        }
    else
        {
        TLex lex( iContent );
        User::LeaveIfError( lex.Val( ret ) );    
        }
        
    return ret;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBooleanKey::CHnMdBooleanKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBooleanKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBooleanKey::ConstructL( const CHnMdBooleanKey* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBooleanKey::ToVariantL( TLiwVariant& aRet ) const
    {
    aRet.SetL( TLiwVariant( ( TBool ) ( GetBooleanValueL() ) ) );   
    }
// End of file
