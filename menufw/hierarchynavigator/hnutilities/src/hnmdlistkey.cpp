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
#include <liwcommon.h>

#include "hnmdlistkey.h"
#include "hnconvutils.h"
#include "hnliwutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdListKey* CHnMdListKey::NewL()
    {
    CHnMdListKey* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdListKey* CHnMdListKey::NewLC()
    {
    CHnMdListKey* self = new ( ELeave ) CHnMdListKey();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdListKey* CHnMdListKey::NewL( const CHnMdListKey* aKey )
    {
    CHnMdListKey* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdListKey* CHnMdListKey::NewLC( const CHnMdListKey* aKey )
    {
    CHnMdListKey* self = new ( ELeave ) CHnMdListKey();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdListKey::CopyLC()
    {
    CHnMdListKey* self = CHnMdListKey::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdListKey::~CHnMdListKey()
    {
    iVariantContent.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdListKey::CHnMdListKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdListKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdListKey::ConstructL( const CHnMdListKey* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdListKey::ToVariantL( TLiwVariant& aRet ) const
    {
    if ( iVariantContent.TypeId() == LIW::EVariantTypeNull )
        {
        CLiwDefaultList* list = CLiwDefaultList::NewL();
        list->PushL();
    
        // handle children
        for ( TInt i( 0 ); i < SubKeys().Count(); i++ )
            {
            TLiwVariant variant;
            variant.PushL();
            SubKeys()[ i ]->ToVariantL( variant );
            list->AppendL( variant );
            CleanupStack::Pop( &variant );
            variant.Reset();
            }
        
        aRet.SetL( TLiwVariant( list ) );
        CleanupStack::PopAndDestroy( list );
        }
    else
        {
        aRet.SetL( iVariantContent );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdListKey::EvaluateKeyValueL( const CLiwGenericParamList& aParamList,
        TInt aPos )
    {
    HBufC8 *path8 = HnConvUtils::StrToStr8LC(iContent);
    iVariantContent.Reset();
    
    TLiwVariant variant;
    if (HnLiwUtils::GetVariantL(aParamList, *path8, aPos, variant) == KErrNone
            &&  variant.TypeId() == LIW::EVariantTypeList)
        {
        iVariantContent = variant;
        }
    else
        {
        variant.Reset();
        }
        
    CleanupStack::PopAndDestroy(path8);
    }
