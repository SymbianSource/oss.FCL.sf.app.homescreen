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


#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <liwservicehandler.h>
#include <f32fsys.h>
#include "hnmdbasekey.h"
#include "hnglobals.h"
#include "hnliwutils.h"
#include "hnconvutils.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C void HnLiwUtils::SetGenericParamListL(
    const RPointerArray<CHnMdBaseKey> & aKeys,
    CLiwGenericParamList & aInParam )
    {
    // for each key
    for ( TInt i( 0 ); i < aKeys.Count(); i++ )
        {
        CHnMdBaseKey* key = aKeys[i];
        const TDesC8& keyName = key->KeyName();
        TLiwVariant value;
        value.PushL();
        key->ToVariantL( value );
        aInParam.AppendL( TLiwGenericParam( keyName, value ) );      
        CleanupStack::PopAndDestroy( &value );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool HnLiwUtils::VariantToStringL(
        TLiwVariant& aVariant,
        RBuf& aBuf )
    {
    TBool accepted( ETrue );
    switch ( aVariant.TypeId() )
        {
        case LIW::EVariantTypeDesC:
            {
            aBuf.CreateL( aVariant.AsDes() );
            break;
            }
        case LIW::EVariantTypeDesC8:
            {
            HBufC* tmp = HnConvUtils::Str8ToStr( aVariant.AsData() );
            if ( !tmp )
                {
                User::Leave( KErrNoMemory );
                }
                aBuf.Assign( tmp );
            break;
            }
        case LIW::EVariantTypeTUid:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTUid().iUid, EHex );
            break;
            }
        case LIW::EVariantTypeTInt32:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTInt32() );
            break;
            }
        case LIW::EVariantTypeTInt64:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTInt64() );
            break;
            }
        case LIW::EVariantTypeTUint:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTUint() );
            break;
            }
        case LIW::EVariantTypeTBool:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTBool() );
            break;
            }
        default:
            {
            accepted = EFalse;
            }
        }
    return accepted;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool HnLiwUtils::VariantToStringL(
        TLiwVariant& aVariant,
        RBuf8& aBuf )
    {
    TBool accepted( ETrue );
    switch ( aVariant.TypeId() )
        {
        case LIW::EVariantTypeDesC:
            {
            aBuf.Assign( HnConvUtils::StrToStr8L( aVariant.AsDes() ) );
            break;
            }
        case LIW::EVariantTypeDesC8:
            {
            aBuf.CreateL( aVariant.AsData() );
            break;
            }
        case LIW::EVariantTypeTUid:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTUid().iUid, EHex );
            break;
            }
        case LIW::EVariantTypeTInt32:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTInt32() );
            break;
            }
        case LIW::EVariantTypeTInt64:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTInt64() );
            break;
            }
        case LIW::EVariantTypeTUint:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTUint() );
            break;
            }
        case LIW::EVariantTypeTBool:
            {
            aBuf.CreateL( KMaxLength );
            aBuf.AppendNum( aVariant.AsTBool() );
            break;
            }
        default:
            {
            accepted = EFalse;
            }
        }
    return accepted;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetIterableItemCountL( CLiwIterable& aIterable )
    {
    TInt count = 0;
    TLiwVariant variant;
    variant.PushL();
    aIterable.Reset();
    while( aIterable.NextL(variant))
        {
        ++count;
        }
    CleanupStack::PopAndDestroy( &variant );
    return count;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetStringL(
    const CLiwGenericParamList& aParam,
    const TDesC8& aPath,
    RBuf& aRet )
    {
    TInt ret( KErrNone );
    TLiwVariant value;
    value.PushL();
    
    ret = GetVariantL( aParam, aPath, value );
    if ( ret == KErrNone && !VariantToStringL( value, aRet ) )
        {
        ret = KErrBadDescriptor;
        }
    
    CleanupStack::PopAndDestroy( &value );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetStringL(
    const CLiwGenericParamList& aParam,
    const TDesC8& aPath,
    RBuf8& aRet )
    {
    TInt ret( KErrNone );
    TLiwVariant value;
    value.PushL();
    
    ret = GetVariantL( aParam, aPath, value );
    if ( ret == KErrNone && !VariantToStringL( value, aRet ) )
        {
        ret = KErrBadDescriptor;
        }
    
    CleanupStack::PopAndDestroy( &value );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetStringL( const CLiwGenericParamList& aParam,
    const TDesC8& aPath, TInt aPos, RBuf& aRet )
    {
    TInt ret( KErrNone );
    TLiwVariant value;
    value.PushL();
    
    ret = GetVariantL( aParam, aPath, aPos, value );
    if ( ret == KErrNone && !VariantToStringL( value, aRet ) )
        {
        ret = KErrBadDescriptor;
        }
    
    CleanupStack::PopAndDestroy( &value );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetStringL( const CLiwGenericParamList& aParam,
    const TDesC8& aPath, TInt aPos, RBuf8& aRet )
    {
    TInt ret( KErrNone );
    TLiwVariant value;
    value.PushL();
    
    ret = GetVariantL( aParam, aPath, aPos, value );
    if ( ret == KErrNone && !VariantToStringL( value, aRet ) )
        {
        ret = KErrBadDescriptor;
        }
    
    CleanupStack::PopAndDestroy( &value );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetInt64L( const CLiwGenericParamList& aParam,
    const TDesC8& aPath, TInt aPos, TInt64& aRet )
    { 
    RBuf8 buf;
    TInt err = HnLiwUtils::GetStringL( aParam, aPath, aPos, buf );
    
    if ( KErrNone == err )
    	{
        TInt64 value( KErrNotFound );
        TLex8 lex( buf );

    	if ( KErrNone == lex.Val( value ) )
    		{
    		aRet = value;
    		err = KErrNone;
    		}
    	}
    
    buf.Close();
    
    return err;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetVariantL(
    const CLiwGenericParamList& aParam,
    const TDesC8& aPath,
    TInt aPos,
    TLiwVariant& aRet )
    {
    TInt ret( KErrNone );
    RBuf8 path;
    CleanupClosePushL( path );
    path.CreateL( aPath );
    // replace index if exist in the path
    HnLiwUtils::ReplaceIndexL( path, aPos );
    // fetch variant
    ret = HnLiwUtils::GetVariantL( aParam, path, aRet );
    if ( ret == KErrNotFound )
        {
        TInt colonFound( aPath.Find( KColon8 ) );
        if ( KErrNotFound != colonFound )
            {
            /*TInt pos( 0 );
            TInt rest( aPos );
            while ( ( rest = rest / 10 ) != 0 )
                { 
                pos++;
                }*/
            
            path.SetLength( colonFound ); 
            TLiwVariant tempVariant;
            tempVariant.PushL();
            if ( KErrNotFound != HnLiwUtils::GetVariantL( aParam, path, aRet ) )
                {
                ret = KErrHidden;
                }
            else
                {
                ret = KErrNotFound;
                }
            CleanupStack::PopAndDestroy( &tempVariant );
            }
        }
    CleanupStack::PopAndDestroy( &path );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HnLiwUtils::ExtractNameSpaceL(
    const CLiwGenericParamList& aParam, 
    const TDesC8& aNameSpace,
    TLiwVariant& aRet )
    {
        TInt pos( 0 );
        const TLiwGenericParam* param;
        
        if ( &aParam != NULL )
	    	{
	    	param = aParam.FindFirst( pos, aNameSpace );
	        if ( pos >= 0 )
		        {
		        aRet.SetL( param->Value() );
		        }
	    	}
        else
        	{
        	pos = KErrNotFound;
        	}
        
        return pos;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt HnLiwUtils::GetVariantL(
    const CLiwGenericParamList& aParam,
    const TDesC8& aPath,
    TLiwVariant& aRet )
    {
    RArray< TPtrC8 > path;
    CleanupClosePushL( path );
    TInt ret( KErrNone );
    TLiwVariant variant;
    TLiwVariant lastVariant;
    variant.PushL();
    lastVariant.PushL();
    
    aRet.SetL( TLiwVariant( aPath ) );
    
    ParsePathL( aPath, path );

    if ( path.Count() > 0
        && ExtractNameSpaceL( aParam, path[0], lastVariant ) != KErrNotFound )
        {
        TBool found( ETrue );
        for ( TInt i( 1 ); i < path.Count() && found; i++ )
            {
            TPtrC8 name = path[ i ];
            LIW::TVariantTypeId typeId = lastVariant.TypeId();
            // LIW::Type ID 7
            if ( typeId == LIW::EVariantTypeList ) 
                {
                TInt pos( GetPosition( name ) );
                found = (pos != KErrNotFound) ? lastVariant.AsList()->AtL( pos, variant ) : EFalse;
                }
            // LIW::Type ID 8
            else if ( typeId == LIW::EVariantTypeMap ) 
                {
                found = lastVariant.AsMap()->FindL( name, variant );
                }
            // LIW::Type ID 9
            else if ( typeId == LIW::EVariantTypeIterable ) 
                {
                TInt pos( GetPosition( name ) );
                found = GetIterableByPositionL( *lastVariant.AsIterable(), pos, variant );
                }
            lastVariant.SetL( variant ); 
            }
        ret = found ? KErrNone : KErrNotFound;
        //aRet.SetL( ( ret != KErrNotFound ) ? variant : TLiwVariant( aPath ) );
        if ( found == 0 )
        	{
        	aRet.SetL( TLiwVariant( KNullDesC8 ) );
        	}
        else
        	{
        	aRet.SetL( variant );
        	}
        }

    CleanupStack::PopAndDestroy( &lastVariant );
    CleanupStack::PopAndDestroy( &variant );
    CleanupStack::PopAndDestroy( &path );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HnLiwUtils::ParsePathL(
    const TDesC8& aPath, 
    RArray< TPtrC8 >& aPathParts )
    {
    TChar ch; // token separator

    TInt colonPos = aPath.Find( KColon8 );

    if ( colonPos > 0 )
        {
        TLex8 lex( aPath );

        while ( !lex.Eos() )
            {
            ch = lex.Get();

            // namespace
            if ( ch == KColon()[0] )
                {
                lex.UnGet();
                TPtrC8 nextToken = lex.MarkedToken();
                lex.SkipAndMark( 1 );
                aPathParts.AppendL( nextToken );
                }
            // list or map
            else if ( ch == KSlash()[0] )
                {
                lex.UnGet();
                TPtrC8 nextToken = lex.MarkedToken();
                lex.SkipAndMark( 1 );
                aPathParts.AppendL( nextToken );
                }
            // last token
            else if ( lex.Eos() )
                {
                TPtrC8 nextToken = lex.MarkedToken();
                aPathParts.AppendL( nextToken );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool HnLiwUtils::GetIterableByPositionL( CLiwIterable & aIterable, TInt pos, 
                                          TLiwVariant & aVariant )
	{
    TBool ret(EFalse);
    if ( pos >= 0 )
        {
        TInt counter = 0;
        aIterable.Reset();
        ret = aIterable.NextL( aVariant );
        while ( ret )
        	{
        	if (counter == pos)
        		{
        		break;
        		}
        	counter++;
        	ret = aIterable.NextL( aVariant );
        	}
        }
    return ret;   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HnLiwUtils::GetPosition( const TDesC8& aPosition )
    {
    TInt ret( KErrNotFound );
    TInt pos1 = aPosition.Find( KOpenSquareBracket );

    if ( pos1 >= 0 )
        {
        TInt pos2 = aPosition.Find( KCloseSquareBracket );
        // extract string number ( +1 a character after "[",
        // -1 a character before "]"
        TPtrC8 num = aPosition.Mid( pos1 + 1, pos2 - 1 );
        // extract number value
        TLex8 lex( num );
        lex.Val( ret );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HnLiwUtils::ReplaceIndexL( RBuf8& aPath8, TInt aPos )
    {
    TInt ret(KErrNotFound);
    TInt indexFound( aPath8.Find( KIndex8 ) );

    if ( !aPath8.Compare( KIndexWithBrackets8 ) )
        {
        aPath8.Close();
        aPath8.CreateL( KMaxLength );
        aPath8.AppendNum( aPos );
        ret = aPath8.Length();
        }
    else if ( KErrNotFound != indexFound )
        {
        RBuf8 indexNum;
        CleanupClosePushL( indexNum );
        indexNum.CreateL( KMaxLength );
        indexNum.AppendNum( aPos );
    
        TInt indexPos = ret = aPath8.Find( KIndex8 );
        TInt indexLength = KIndex().Length();
    
        aPath8.Replace( indexPos, indexLength, indexNum );
        ret += indexNum.Length();
        
        CleanupStack::PopAndDestroy( &indexNum );
        }
    
    return ret;
    }

