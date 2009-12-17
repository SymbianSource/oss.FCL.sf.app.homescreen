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
* Description:  image provider for application uid
*
*/


#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <avkon.mbg>
#include "hnglobals.h"
#include "hnattrimgprovappimage.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvAppImage::CHnAttrImgProvAppImage( )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvAppImage::ConstructL( TUid aApplicationUid )
    {
    iApplicationUid = aApplicationUid;
    
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    ProvideBitmapL( NULL, bitmap, mask );
    CreateIconL( bitmap, mask );
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvAppImage* CHnAttrImgProvAppImage::NewL( 
        TUid aApplicationUid )
    {
    CHnAttrImgProvAppImage* self = NewLC( aApplicationUid );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvAppImage* CHnAttrImgProvAppImage::NewLC( 
        TUid aApplicationUid )
    {
    CHnAttrImgProvAppImage* self = new( ELeave ) CHnAttrImgProvAppImage;
    CleanupStack::PushL( self );
    self->ConstructL( aApplicationUid );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvAppImage::~CHnAttrImgProvAppImage()
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnAttrImgProvAppImage::Value( )
    {
    return iValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvAppImage::ProvideBitmapL( TInt /*aId*/, CFbsBitmap*& aBitmap, 
        CFbsBitmap*& aMaskBitmap )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    TInt err( KErrNone );
    TRAP( err,
        {
        AknsUtils::CreateAppIconLC( skin, iApplicationUid,
            EAknsAppIconTypeContext, aBitmap, aMaskBitmap );
        CleanupStack::Pop( 2 ); // for trap
        }
        );
    
    if( err != KErrNone )
        {
        // If icon is not created, try to create default application icon
        TRAP( err,
            {
            AknsUtils::CreateIconLC( skin,
                KAknsIIDQgnMenuUnknownLst, aBitmap, aMaskBitmap,
                AknIconUtils::AvkonIconFileName(),
                EMbmAvkonQgn_menu_unknown_lst,
                EMbmAvkonQgn_menu_unknown_lst_mask );
            CleanupStack::Pop( 2 ); // for trap
            }
            );
        }
    }


