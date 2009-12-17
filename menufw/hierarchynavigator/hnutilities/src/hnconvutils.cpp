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


#include <utf.h> 
#include <e32std.h>

#include "hnconvutils.h"
#include "hnglobals.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* HnConvUtils::Str8ToStr( const TDesC8& aStr )
    {
    HBufC* ret = HBufC::New( aStr.Length() );
    if (ret)
    	{
    	TPtr dest( ret->Des() );
    	CnvUtfConverter::ConvertToUnicodeFromUtf8( dest, aStr );
    	}
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* HnConvUtils::Str8ToStrLC( const TDesC8& aStr )
    {
    HBufC* ret = HBufC::NewLC( aStr.Length() );
    TPtr dest( ret->Des() );
    CnvUtfConverter::ConvertToUnicodeFromUtf8( dest, aStr );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* HnConvUtils::StrToStr8LC( const TDesC16& aStr )
    {
    TInt length = aStr.Length();
    HBufC8* ret = HBufC8::NewL( length );
    TPtr8 dest( ret->Des() );
    TInt num = CnvUtfConverter::ConvertFromUnicodeToUtf8( dest, aStr );
    while( num != 0 )
    	{
    	delete ret;
    	length += num;
    	ret = HBufC8::NewL( length );
    	dest.Set( ret->Des() );
    	num = CnvUtfConverter::ConvertFromUnicodeToUtf8( dest, aStr );
    	}
    CleanupStack::PushL(ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* HnConvUtils::StrToStr8L( const TDesC16& aStr )
    {
    HBufC8* buf = HnConvUtils::StrToStr8LC( aStr );
    CleanupStack::Pop( buf );
    return buf;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* HnConvUtils::NumToStr8LC( const TInt& aNum )
    {
    HBufC8* ret = HBufC8::NewLC( KMaxLength );
    ret->Des().AppendNum( aNum );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnConvUtils::Str8ToInt( const TDesC8& aValue, TInt& aResult )
    {
    // if a string starts from '0x' then convert aStr from hex to int.
    // else send aStr to the StrDec2Uint    
    TInt err( KErrNotFound );
    TInt position( aValue.Find( KHexStart8 ) );
    
    TLex8 lexer( aValue );
    if ( position == 0 )
        {
        // is hex, it it can only unsinged
        lexer.SkipAndMark( KHexStart8().Length() );
        TUint tmp;
        err = lexer.Val( tmp, EHex );
        aResult = tmp;
        }
    else
        {
        err = lexer.Val( aResult );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnConvUtils::StrToInt( const TDesC& aValue, TInt& aResult )
    {
    TInt err( KErrNotFound );
    RBuf8 buf;
    err = buf.Create( aValue.Length() );
    if ( KErrNone == err )
        {
        buf.Copy( aValue );
        err = HnConvUtils::Str8ToInt( buf, aResult );
        }
    buf.Close();
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnConvUtils::Str8ToInt( const TDesC8& aValue, TInt32& aResult )
    {
    // if a string starts from '0x' then convert aStr from hex to int.
    // else send aStr to the StrDec2Uint    
    TInt err( KErrNotFound );
    TInt position( aValue.Find( KHexStart8 ) );
    
    TLex8 lexer( aValue );
    if ( position == 0 )
        {
        // is hex, it it can only unsinged
        TUint32 tmp;
        lexer.SkipAndMark( KHexStart8().Length() );
        err = lexer.Val( tmp, EHex );
        aResult = tmp;
        }
    else
        {
        err = lexer.Val( aResult );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt  HnConvUtils::StrToInt( const TDesC& aValue, TInt32& aResult )
    {
    TInt err( KErrNotFound );
    RBuf8 buf; 
    err = buf.Create( aValue.Length() );
    if ( KErrNone == err )
        {
        buf.Copy( aValue );
        err = HnConvUtils::Str8ToInt( buf, aResult );
        }
    buf.Close();
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnConvUtils::Str8ToUint( const TDesC8& aValue, TUint& aResult )
    {
    // if a string starts from '0x' then convert aStr from hex to int.
    // else send aStr to the StrDec2Uint    
    TInt err( KErrNotFound );
    TInt position( aValue.Find( KHexStart8 ) );
    
    TLex8 lexer( aValue );
    if ( position == 0 )
        {
        // is hex
        lexer.SkipAndMark( KHexStart8().Length() );
        err = lexer.Val( aResult, EHex );
        }
    else
        {
        err = lexer.Val( aResult );
        }
    return err;
    }
