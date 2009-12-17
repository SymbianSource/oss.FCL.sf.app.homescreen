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
* Description:  image provider for mif file
*
*/


#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <mifconvdefs.h>
#include "hnattrimgprovthememif.h"


// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvThemeMif::CHnAttrImgProvThemeMif( )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvThemeMif::ConstructL( TAknsItemID aSkinId,
        const TDesC& aFile, TInt aBitmapId, TInt aMaskId )
    {
    iSkinId = aSkinId;
    iFile.Set( aFile );
    iBitmapId = aBitmapId;
    iMaskId = aMaskId;

    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    if ( ValidateIconIds() )
        {
        ProvideBitmapL( NULL, bitmap, mask );
        CreateIconL( bitmap, mask );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvThemeMif* CHnAttrImgProvThemeMif::NewL( 
        TAknsItemID aSkinId, const TDesC& aFile, 
        TInt aBitmapId, TInt aMaskId )
    {
    CHnAttrImgProvThemeMif* self = NewLC( aSkinId, aFile, 
            aBitmapId, aMaskId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnAttrImgProvThemeMif::ValidateIconIds()
    {
    return ((iSkinId.iMajor != KErrNotFound && iSkinId.iMinor != KErrNotFound)
            || (iBitmapId >= KMifIdFirst && iMaskId >= KMifIdFirst));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvThemeMif* CHnAttrImgProvThemeMif::NewLC( 
        TAknsItemID aSkinId, const TDesC& aFile,
        TInt aBitmapId, TInt aMaskId )
    {
    CHnAttrImgProvThemeMif* self = new( ELeave ) CHnAttrImgProvThemeMif;
    CleanupStack::PushL( self );
    self->ConstructL( aSkinId, aFile, aBitmapId, aMaskId );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvThemeMif::~CHnAttrImgProvThemeMif()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnAttrImgProvThemeMif::Value( )
    {
    return iValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvThemeMif::ProvideBitmapL( TInt /*aId*/, CFbsBitmap*& aBitmap, 
        CFbsBitmap*& aMaskBitmap )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::CreateIconL( skin, iSkinId, aBitmap, aMaskBitmap,
            iFile, iBitmapId, iMaskId );
    }


