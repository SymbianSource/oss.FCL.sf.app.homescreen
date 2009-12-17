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

#include "hnmdmapkey.h"
#include "hnconvutils.h"
#include "hnliwutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMapKey* CHnMdMapKey::NewL()
    {
    CHnMdMapKey* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMapKey* CHnMdMapKey::NewLC()
    {
    CHnMdMapKey* self = new ( ELeave ) CHnMdMapKey();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMapKey* CHnMdMapKey::NewL( const CHnMdMapKey* aKey )
    {
    CHnMdMapKey* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMapKey* CHnMdMapKey::NewLC( const CHnMdMapKey* aKey )
    {
    CHnMdMapKey* self = new ( ELeave ) CHnMdMapKey();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdMapKey::CopyLC()
    {
    CHnMdMapKey* self = CHnMdMapKey::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMapKey::~CHnMdMapKey()
    {
    iVariantContent.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMapKey::CHnMdMapKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMapKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMapKey::ConstructL( const CHnMdMapKey* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMapKey::ToVariantL( TLiwVariant& aRet ) const
    {
    if ( iVariantContent.TypeId() == LIW::EVariantTypeNull )
        {
        CLiwDefaultMap* map = CLiwDefaultMap::NewL();
        map->PushL();
          
        // handle children
        for( TInt i( 0 ); i < SubKeys().Count(); i++ )
            {
            TLiwVariant variant;
            SubKeys()[ i ]->ToVariantL( variant );
            map->InsertL( SubKeys()[ i ]->KeyName(), variant );
            variant.Reset();
            }
    
        aRet.SetL( TLiwVariant( map ) );
        CleanupStack::PopAndDestroy( map );
        }
    else
        {
        aRet.SetL(iVariantContent);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMapKey::EvaluateKeyValueL( const CLiwGenericParamList& aParamList,
        TInt aPos )
    {
    HBufC8 *path8 = HnConvUtils::StrToStr8LC(iContent);
    iVariantContent.Reset();
    
    TLiwVariant variant;
    if ( HnLiwUtils::GetVariantL(aParamList, *path8, aPos, variant) == KErrNone
            &&  variant.TypeId() == LIW::EVariantTypeMap )
        {
        iVariantContent = variant;
        }
    else
        {
        variant.Reset();
        }
        
    CleanupStack::PopAndDestroy(path8);
    }

