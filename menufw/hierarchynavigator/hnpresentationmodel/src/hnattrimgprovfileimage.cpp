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
* Description:  image provider for image file
*
*/


#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include "hnconvutils.h"
#include "hnglobals.h"
#include "hnattrimgprovfileimage.h"
#include "imageconversion.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvFileImage::CHnAttrImgProvFileImage()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvFileImage::CorrectBitmapSize(
        CImageDecoder& aImageDec, TSize& aBitmapSize )
    {
    const TFrameInfo& frameInfo = aImageDec.FrameInfo();
    TSize imageSize = frameInfo.iOverallSizeInPixels;
    if ( imageSize.iWidth && imageSize.iHeight )
        {
        if ( imageSize.iWidth <= aBitmapSize.iWidth
                && imageSize.iHeight <= aBitmapSize.iHeight)
            {
            aBitmapSize = imageSize;
            }
        else
            {
            TInt reductionFactor = aImageDec.ReductionFactor(
                    imageSize, aBitmapSize );
            TInt err = aImageDec.ReducedSize(
                    imageSize, reductionFactor, aBitmapSize );
            ASSERT( KErrNone == err );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvFileImage::ConstructL( const TDesC& aFileName )
    {
    TSize bitmapSize( KMaxIconBitmapSize, KMaxIconBitmapSize );
    
    HBufC8* buf = HnConvUtils::StrToStr8LC( aFileName );
    iValue.CreateL( aFileName.Length() + 1  );
    iValue.Copy( *buf );
    CleanupStack::PopAndDestroy( buf );
    
    CImageDecoder* imageDec = CImageDecoder::FileNewL(CEikonEnv::Static()->FsSession(), 
            aFileName, CImageDecoder::EOptionAlwaysThread);
    CleanupStack::PushL( imageDec );            
    
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    TDismissableCleaner bitmapCleaner( bitmap );
    CleanupClosePushL( bitmapCleaner );
    CFbsBitmap* mask = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( mask );

    CorrectBitmapSize( *imageDec, bitmapSize );
    User::LeaveIfError( bitmap->Create( bitmapSize, EColor64K ) );
    User::LeaveIfError( mask->Create( bitmapSize, EGray256  ) );
    TRequestStatus status = KRequestPending;
    imageDec->Convert( &status, *bitmap, *mask );
    User::WaitForRequest( status );  

    CleanupStack::Pop( mask );
    CFbsBitmap* avkonishMask = AknIconUtils::CreateIconL( mask );
    mask = NULL;
    CleanupStack::PushL( avkonishMask );

    bitmapCleaner.Dismiss();
    CFbsBitmap* avkonishBitmap = AknIconUtils::CreateIconL( bitmap );
    bitmap = NULL;
    CleanupStack::PushL( avkonishBitmap );
    
    CreateIconL( avkonishBitmap, avkonishMask );
    CleanupStack::Pop( avkonishBitmap );
    CleanupStack::Pop( avkonishMask );
    
    CleanupStack::PopAndDestroy( &bitmapCleaner );
    CleanupStack::PopAndDestroy( imageDec ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvFileImage* CHnAttrImgProvFileImage::NewL( 
        const TDesC& aFileName )
    {
    CHnAttrImgProvFileImage* self = NewLC( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvFileImage* CHnAttrImgProvFileImage::NewLC( 
        const TDesC& aFileName )
    {
    CHnAttrImgProvFileImage* self = new( ELeave ) CHnAttrImgProvFileImage;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvFileImage::~CHnAttrImgProvFileImage()
    {
    iValue.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnAttrImgProvFileImage::Value()
    {
    return iValue;
    }

