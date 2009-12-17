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
* Description:  image provider for liw buffer
*
*/


#include <fbs.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <liwcommon.h>
#include <liwbufferextension.h>
#include <s32mem.h>
#include "hnattrimgprovliwimage.h"
#include "hnextbmpiconholder.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvLiwImage::CHnAttrImgProvLiwImage()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvLiwImage::ConstructL( TLiwVariant*& aBitmap, 
                                         TLiwVariant*& aMaskBitmap )
    {
    ASSERT( aBitmap );
    CFbsBitmap* mask = NULL;
    CFbsBitmap* bitmap = CreateBitmapFromVariantL( *aBitmap );
    if ( bitmap )
        {
        if( aMaskBitmap )
            {
            mask = CreateBitmapFromVariantL( *aMaskBitmap );
            }
        CreateIconL( bitmap, mask );
        }
    else if ( aBitmap->TypeId() == LIW::EVariantTypeBuffer )
        {
        CLiwBitmapBuffer* bmpBuff =
            static_cast<CLiwBitmapBuffer*>( aBitmap->AsBuffer() ); 
        CLiwBitmapBuffer* maskBuff =
            static_cast<CLiwBitmapBuffer*>(
                    aMaskBitmap ? aMaskBitmap->AsBuffer() : NULL );
        if ( bmpBuff )
            {
            bitmap = bmpBuff->AsBitmap();
            }
        if ( maskBuff )
            {
            mask = maskBuff->AsBitmap();
            }
        if ( bitmap )
            {
            CreateExtIconL( bitmap, mask, bmpBuff, maskBuff );
            }
        }
    if ( !bitmap )
        {
        //No point to keep variant as bitmap was not created
        aBitmap->Reset();
        delete aBitmap;
        aBitmap = NULL;
        }
    if ( aMaskBitmap && !mask )
        {
        //No point to keep variant as bitmap was not created
        aMaskBitmap->Reset();
        delete aMaskBitmap;
        aMaskBitmap = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvLiwImage::CreateExtIconL( CFbsBitmap* aBitmap,
        CFbsBitmap* aMask, CLiwBitmapBuffer* aExtBmp,
        CLiwBitmapBuffer* aExtMask )
    {
    // the condition in the assertion below means: if you provide aMask then
    // you must also provide aExtMask
    __ASSERT_DEBUG( aBitmap && aExtBmp && ( !aMask || aExtMask ),
            User::Invariant() );
    
    CGulIcon* newIcon = CGulIcon::NewL( aBitmap, aMask );
    CleanupStack::PushL( newIcon );
    newIcon->SetBitmapsOwnedExternally( ETrue );
    CHnExtBmpIconHolder *newIconHolder = new (ELeave) CHnExtBmpIconHolder();
    newIconHolder->SetGulIcon( newIcon , aExtBmp, aExtMask );
    CleanupStack::Pop( newIcon );
    
    if ( iIconHolder )
        {
        iIconHolder->Close();
        }
    iIconHolder = newIconHolder;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvLiwImage* CHnAttrImgProvLiwImage::NewL( 
        TLiwVariant*& aBitmap, TLiwVariant*& aMaskBitmap )
    {
    CHnAttrImgProvLiwImage* self = NewLC( aBitmap, aMaskBitmap );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvLiwImage* CHnAttrImgProvLiwImage::NewLC( 
        TLiwVariant*& aBitmap, TLiwVariant*& aMaskBitmap )
    {
    CHnAttrImgProvLiwImage* self = new( ELeave ) CHnAttrImgProvLiwImage;
    CleanupStack::PushL( self );
    self->ConstructL( aBitmap, aMaskBitmap );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvLiwImage::~CHnAttrImgProvLiwImage()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvLiwImage::RemoveLiwObjects()
    {
    if ( iIconHolder )
        {
        iIconHolder->Close();
        iIconHolder = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnAttrImgProvLiwImage::Value( )
    {
    return iValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CFbsBitmap* CHnAttrImgProvLiwImage::CreateBitmapFromVariantL( 
                                                const TLiwVariant& aVariant )
    {
    CFbsBitmap* bitmap( NULL );
    TInt handle = 0;

    if ( ExtractBitmapHandleFromVariant( aVariant, handle ) == KErrNone )
        {
        CFbsBitmap* tempBitmap  = new( ELeave ) CFbsBitmap;
        if ( tempBitmap->Duplicate( handle ) == KErrNone )
            {
            bitmap = tempBitmap;
            }
        else
            {
            delete tempBitmap;
            }
        }
    else if ( aVariant.TypeId() == LIW::EVariantTypeDesC8 )
        {
        CFbsBitmap* tempBitmap = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( tempBitmap );
        TPtrC8 buf = aVariant.AsData();
        RDesReadStream stream( buf );
        CleanupClosePushL( stream );
        tempBitmap->InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream );
        CleanupStack::Pop( tempBitmap );
        bitmap = tempBitmap;
        }
    return bitmap;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt CHnAttrImgProvLiwImage::ExtractBitmapHandleFromVariant(
        const TLiwVariant& aVariant, TInt& aHandle )
    {
    TInt err = KErrNone;
    switch ( aVariant.TypeId() )
        {
        case LIW::EVariantTypeTInt32:
            aHandle = static_cast<TInt>( aVariant.AsTInt32() );         
            break;
        case LIW::EVariantTypeTUint:
            aHandle = static_cast<TInt>( aVariant.AsTUint() );         
            break;
        case LIW::EVariantTypeTInt64:
            aHandle = static_cast<TInt>( aVariant.AsTInt64() );         
            break;
        case LIW::EVariantTypeTBool:
            // Putting a TInt into a TLiwVariant will result in the variant
            // storing a TBool variable. CFbsBitmap::Handle() returns TInt, so
            // we can assume that the TBool stored in aVariant is in fact TInt.
            aHandle = static_cast<TInt>( aVariant.AsTBool() );
            break;
        default:
            err = KErrNotFound;
        }
    return err;
    }
