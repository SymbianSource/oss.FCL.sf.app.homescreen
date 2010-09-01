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
* Description:  image attribute model
 *
*/


#include <bautils.h>
#include <f32file.h>
#include <liwcommon.h>

#include "hnglobals.h"
#include "hnattributeimage.h"
#include "hnattrimgprovider.h"
#include "hnattrimgprovthememif.h"
#include "hnattrimgprovappimage.h"
#include "hnattrimgprovfileimage.h"
#include "hnattrimgprovliwimage.h"
#include "hnattrimgprovemptyimage.h"
#include "hnattrimgprovsvgimage.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnAttributeImage::CHnAttributeImage()
    {
    iSkinId = KAknsIIDNone;
    iAttributeType = EImageAttribute;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnAttributeImage::~CHnAttributeImage()
    {
    iFileNameSrc.Close();
    iMifFile.Close();
    RemoveLiwObjects();
    delete iAttrImgProvider;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttributeImage* CHnAttributeImage::NewL()
    {
    CHnAttributeImage* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttributeImage* CHnAttributeImage::NewLC()
    {
    CHnAttributeImage* self = new ( ELeave ) CHnAttributeImage;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnAttributeImage::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetSkinId(TAknsItemID aSkinId)
    {
    iSkinId = aSkinId ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetFileNameSrc(HBufC* aFileNameSrc)
    {
    iFileNameSrc.Assign( aFileNameSrc );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetMifFile(HBufC* aMifFile)
    {
    iMifFile.Assign( aMifFile );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetBitmapId(TInt aBitmapId)
    {
    iBitmapId = aBitmapId ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetMaskId(TInt aMaskId)
    {
    iMaskId = aMaskId ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetApplicationUid(TUid aApplicationUid)
    {
    iApplicationUid = aApplicationUid ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetBitmap( TLiwVariant* aBitmap )
    {
    iBitmap = aBitmap ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeImage::SetMask( TLiwVariant* aMask )
    {
    iMask = aMask ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnAttributeImage::Prepare()
    {
    TInt error = KErrNone;
    if( !iAttrImgProvider )
        {
        TRAP( error, PrepareProviderL() );
        }
    return error;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnAttributeImage::PrepareProviderL()
    {
    ASSERT( !iAttrImgProvider );
    if ( IsThemeMif() )
        {
        iAttrImgProvider = CHnAttrImgProvThemeMif::NewL( iSkinId, iMifFile, 
                iBitmapId, iMaskId );
        }
    else if ( IsApplicationImage() )
        {
        iAttrImgProvider = CHnAttrImgProvAppImage::NewL( iApplicationUid );
        }
    else if ( IsLiwImage() )
        {
        iAttrImgProvider = CHnAttrImgProvLiwImage::NewL( iBitmap, iMask );
        }
    else if ( IsFileImageL() )
        {
        if ( iFileNameSrc.Find( KSvgFileExtension() ) != KErrNotFound )
        	{
        	iAttrImgProvider = CHnAttrImgProvSvgImage::NewL( iFileNameSrc );
        	}
        else
        	{
        	iAttrImgProvider = CHnAttrImgProvFileImage::NewL( iFileNameSrc );
        	}
        }
    // if strin couldn't be resolved then provide empty one
    else
        {
        iAttrImgProvider = CHnAttrImgProvEmptyImage::NewL();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnAttributeImage::IsThemeMif()
    {
    if( iSkinId != KAknsIIDNone )
        {
        return ETrue;
        }
    if( iMifFile.Length() && iBitmapId && iMaskId )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnAttributeImage::IsApplicationImage()
    {
    TBool ret( EFalse );
    if ( iApplicationUid.iUid != 0 )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TBool CHnAttributeImage::IsFileImageL()
    {
    TBool ret( EFalse );
    if ( iFileNameSrc.Length() > 0  )
        {
        RFs& fs = CEikonEnv::Static()->FsSession();
        
        if ( BaflUtils::FileExists( fs, iFileNameSrc ) )
            {
            ret = ETrue;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TBool CHnAttributeImage::IsLiwImage()
    {
    TBool ret( EFalse );
    if ( iBitmap )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CGulIcon* CHnAttributeImage::GetIcon( TSize* aDesiredIconSize )
    { 
    if ( Prepare() == KErrNone )
        {
        return iAttrImgProvider->GetIcon( aDesiredIconSize );
        }
    else 
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnIconHolder* CHnAttributeImage::GetIconHolder( TSize* aDesiredIconSize )
{
    CHnIconHolder* iconHolder = NULL;
    if ( Prepare() == KErrNone )
        {
        iconHolder = iAttrImgProvider->GetIconHolder( aDesiredIconSize );
        }
    return iconHolder;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CHnAttributeImage::RemoveLiwObjects()
    {
    if ( iAttrImgProvider )
        {
        iAttrImgProvider->RemoveLiwObjects();
        }

    if( iBitmap )
        {
        iBitmap->Reset();
        delete iBitmap;
        iBitmap = NULL;
        }

    if( iMask )
        {
        iMask->Reset();
        delete iMask;
        iMask = NULL;
        }
    }
