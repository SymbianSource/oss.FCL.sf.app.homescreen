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

#include "hnmdkeyfactory.h"
#include "hnconvutils.h"
#include "hnliwutils.h"
#include "hnglobals.h"
#include "hnmdbasekey.h"

// ======== MEMBER FUNCTIONS ========
   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetKeyOrNameL( const TXmlEngElement& aElement )
    {
    // set key or name
    if ( !aElement.Name().Compare( KKeyElementItem8 ) )
        {
        if ( aElement.HasAttributeL( KKeyAttrName8 ) )
            {
            // key
            SetKeyNameL( aElement.AttributeNodeL( KKeyAttrName8 ) );
            }
        else if ( aElement.HasAttributeL( KNameAttrName8 ) )
            {
            // name
            SetKeyNameL( aElement.AttributeNodeL( KNameAttrName8 ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const RPointerArray<CHnMdBaseKey>& CHnMdBaseKey::SubKeys() const
    {
    return iSubKeys;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::BaseConstructL( const CHnMdBaseKey* aKey )
    {
    iKeyName.CreateL( aKey->KeyName() );
    iContent.CreateL( aKey->KeyContent() );
    SetSubKeysL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::BaseConstructL( TXmlEngElement aElement )
    {
    ASSERT( !aElement.Name().Compare( KKeyElementItem8 )
        || !aElement.Name().Compare( KContentTypeElementItem8 ) );

    SetKeyOrNameL( aElement );
    SetValueL( aElement );

    if ( aElement.HasChildNodes() )
        {
        SetSubKeysL( aElement  );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetValueL( const TXmlEngElement& aElement )
    {
    if ( aElement.HasAttributeL( KValueAttrName8 ) )
        {
        SetKeyContentL( aElement.AttributeNodeL( KValueAttrName8 ) );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetValueL( const TDesC8& aValue )
    {
    iContent.Close();
    iContent.CreateL( aValue.Length() );
    iContent.Copy( aValue );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetValueL( const TDesC& aValue )
    {
    iContent.Close();
    iContent.CreateL( aValue );
    }	
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey::CHnMdBaseKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey::~CHnMdBaseKey()
    {
    iKeyName.Close();
    iContent.Close();
    iSubKeys.ResetAndDestroy();
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
const TDesC8& CHnMdBaseKey::KeyName() const
    {  
    return iKeyName;
    } 

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CHnMdBaseKey::KeyContent() const
    {
    return iContent;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::GetKeyContent( TInt& /* aKeyContent */ ) const
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::GetKeyContent( TInt64& /* aKeyContent */ ) const
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetKeyNameL( const TXmlEngAttr& aName )
    {
    iKeyName.Close();
    iKeyName.CreateL( aName.Value() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetKeyNameL( const TDesC8& aName )
    {
    iKeyName.Close();
    iKeyName.CreateL( aName );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetSubKeysL( const TXmlEngElement& aElement )
    {    
    RXmlEngNodeList< TXmlEngElement > subkeys;
    CleanupClosePushL( subkeys );
    aElement.GetChildElements( subkeys );
    
    TInt subkeysAmount( subkeys.Count() );
    
    for ( TInt j( 0 ); j < subkeysAmount; j++ )
        {
        TXmlEngElement subkeyElement( subkeys.Next() );
        
        if ( !subkeyElement.Name().Compare( KKeyElementItem8 ) )
            {
            CHnMdBaseKey *subkey = HnMdKeyFactory::CreateL( subkeyElement );
            CleanupStack::PushL( subkey );
            AddSubKeyL( subkey );
            CleanupStack::Pop( subkey );
            }
        }
    CleanupStack::PopAndDestroy( &subkeys );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetSubKeysL( const CHnMdBaseKey* aKey )
    {
    const RPointerArray<CHnMdBaseKey>& subKeys = aKey->SubKeys();
    TInt count = subKeys.Count();
    for( TInt i( 0 ); i < count ; i++ )
        {
        CHnMdBaseKey* subKey = subKeys[ i ]->CopyLC();
        AddSubKeyL( subKey );
        CleanupStack::Pop( subKey );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdBaseKey::AddSubKeyL( CHnMdBaseKey* aKey )
    {
    iSubKeys.AppendL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::SetKeyContentL( const TXmlEngAttr& aContent )
    {
    HBufC* value = HnConvUtils::Str8ToStrLC( aContent.Value() );
    iContent.Close();
    iContent.Assign( value );
    // clean up
    CleanupStack::Pop( value );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdBaseKey* CHnMdBaseKey::EvaluateLC(
    const CLiwGenericParamList& aParamList,
    TInt aPos )
    {
    CHnMdBaseKey* copy = CopyLC();
    copy->DoEvaluateL(  aParamList, aPos );
    return copy;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::DoEvaluateL(
    const CLiwGenericParamList& aParamList,
    TInt aPos )
    {
    EvaluateKeyValueL( aParamList, aPos );
    // evaluate subkeys
    TInt count( iSubKeys.Count() );
    for( TInt i( 0 ); i < count; i++ )
        {
        CHnMdBaseKey* key = iSubKeys[ i ];
        key->DoEvaluateL( aParamList, aPos );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdBaseKey::EvaluateKeyValueL(
    const CLiwGenericParamList& aParamList,
    TInt aPos )
    {
    HBufC8* tempContent = HnConvUtils::StrToStr8LC( iContent );
    RBuf tempResult;
    CleanupClosePushL( tempResult );
    if ( HnLiwUtils::GetStringL( aParamList, *tempContent, aPos, tempResult ) 
        != KErrBadDescriptor )
        {
        iContent.Swap( tempResult );
        }
    CleanupStack::PopAndDestroy( &tempResult );
    CleanupStack::PopAndDestroy( tempContent );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

EXPORT_C void CHnMdBaseKey::ToVariantL( TLiwVariant& /* aRet */ ) const
    {
    
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdBaseKey::ToGenericParamListL( CLiwGenericParamList & aRet ) const
    {

    // for each key
    for ( TInt i( 0 ); i < iSubKeys.Count(); i++ )
        {
        CHnMdBaseKey* key = iSubKeys[i];
        const TDesC8& keyName = key->KeyName();
        TLiwVariant value;
        value.PushL();
        key->ToVariantL( value );
        aRet.AppendL( TLiwGenericParam( keyName, value ) );      
        CleanupStack::PopAndDestroy( &value );
        }
    }
