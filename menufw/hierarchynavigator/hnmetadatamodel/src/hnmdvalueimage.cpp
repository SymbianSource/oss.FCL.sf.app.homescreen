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


#include <xmlengdom.h>
#include <bautils.h>
#include <liwcommon.h>
#include <AknsSkinInstance.h> 
#include <AknsDrawUtils.h> 
#include <gulicon.h> 

#include "hnmdvalueimage.h"
#include "hnglobals.h"
#include "hnmdmodel.h"
#include "AknsUtils.h"
#include "hnconvutils.h"
#include "hnliwutils.h"
#include "hnutils.h"
#include "bitdev.h"
#include "hnattributebase.h"
#include "hnattributeimage.h"
#include "menudebug.h"
#include "hnbitmapidcache.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueImage* CHnMdValueImage::NewLC(
        const TXmlEngElement& aElement, THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdValueImage* self = 
        new (ELeave)CHnMdValueImage();
    CleanupStack::PushL(self);
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueImage* CHnMdValueImage::NewL(
        const TXmlEngElement& aElement, THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdValueImage* self =
        CHnMdValueImage::NewLC( aElement, aCmnPtrs );
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdValueImage::ConstructL( const TXmlEngElement& aElement,
        THnMdCommonPointers* aCmnPtrs )
    {
    iCmnPtrs = aCmnPtrs;
    iPos = 0;
    
    HnUtils::SetAttributeL( aElement, KFile8, iFileNameSrc8 );
    if ( iFileNameSrc8.Length( ) == 0 )
        {
        HnUtils::SetAttributeL( aElement, iFileNameSrc8 );
        }
    HnUtils::SetAttributeL( aElement, KSkinIdMinor8, iSkinIdMinor8 );
    HnUtils::SetAttributeL( aElement, KSkinIdMajor8, iSkinIdMajor8 );
    HnUtils::SetAttributeL( aElement, KBitmapId8, iBitmapId8 );
    HnUtils::SetAttributeL( aElement, KMaskId8, iMaskId8 );
    HnUtils::SetAttributeL( aElement, KMifFile8, iMifFile8 );
    HnUtils::SetAttributeL( aElement, KAppUid8, iApplicationUid8 );
    HnUtils::SetAttributeL( aElement, KBitmap8, iBitmap8 );
    HnUtils::SetAttributeL( aElement, KMask8, iMask8 );
    
    iMifFileName = GetMifFileNameL();
    }

// ---------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CHnMdValueImage::GetResourceIdL( const TDesC8& aResourceFileContents,
        const TDesC8& aResourceName )
    {
    HBufC8* resourceWithSeparator = 
        HBufC8::NewLC( aResourceName.Length() + KResourceSeparator().Length() );   
    resourceWithSeparator->Des().Append( aResourceName ); 
    resourceWithSeparator->Des().Append( KResourceSeparator() );
    
    TInt position = aResourceFileContents.Find( *resourceWithSeparator );

    TInt id( KErrNotFound );
    
    if ( position >= 0 )
        {
        TPtrC8 contentAfterMid = aResourceFileContents.Mid(
                    position + resourceWithSeparator->Length() );    
    
        TChar comma( KComma()[0] );
        TInt posEnd = contentAfterMid.Locate( comma );
        
        if ( posEnd >= 0 )
            {
    
            TPtrC8 resourceId = contentAfterMid.Left( posEnd );
        
            HBufC8* resourceTrimmed = resourceId.AllocL();
            resourceTrimmed->Des().Trim();
        
            // GET ICON ID
            HnConvUtils::Str8ToInt( *resourceTrimmed, id );
            
            delete resourceTrimmed; 
            }
        }
    
    //cleanup    
    CleanupStack::PopAndDestroy( resourceWithSeparator );
    
    return id;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CHnMdValueImage::SetImageSourceL()
    {
    if ( iBitmapId8.Length() > 0 && iMaskId8.Length() > 0 &&
            iMifFile8.Length() > 0 )
        {
        iImageSource = EImageSourceResource;
        // check if file exists
        if ( !iMifFileName ||
                !BaflUtils::FileExists( iCmnPtrs->iFs, *iMifFileName ) )
            {
            iImageSource = EImageSourceUndefined;
            }
        }
    else if ( iFileNameSrc8.Length() > 0  )
        {
        iImageSource = EImageSourceFile;
        // check if file exists
        HBufC* fn = GetFileNameSrcL();
        CleanupStack::PushL( fn );
        if ( EFalse == BaflUtils::FileExists( iCmnPtrs->iFs, *fn ) )
            {
            iImageSource = EImageSourceUndefined;
            }
        CleanupStack::PopAndDestroy( fn );
        }
    else if ( iBitmap8.Length() > 0 && iMask8.Length() > 0 ) 
        {
        iImageSource = EImageSourceLiw;
        }
    else if ( iApplicationUid8.Length() > 0 )
        {
        iImageSource = EImageSourceApplicationUid;
        }
    else
        {
        iImageSource = EImageSourceUndefined;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueImage::CHnMdValueImage()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueImage::~CHnMdValueImage()
    {
    iFileNameSrc8.Close();
    //iFileName.Close();
    iBitmap8.Close();
    iMask8.Close();
    iSkinIdMinor8.Close();
    iSkinIdMajor8.Close();
    iBitmapId8.Close();
    iMaskId8.Close();
    iApplicationUid8.Close();
    iMifFile8.Close();
    iReturnValue.Close();
    delete iMifFileName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnAttributeBase* CHnMdValueImage::CreateAttributeL( 
        const CLiwGenericParamList* aParamList,
        TInt aPos )
    {
    iParamList = aParamList;
    iPos = aPos;
    CHnAttributeImage* attr = CHnAttributeImage::NewLC();
    SetAttributeDataL( attr );
    CleanupStack::Pop( attr );
    return attr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CHnMdValueImage::SetAttributeDataL( CHnAttributeImage* aAttr )
    {
    TInt bitmapId = 0;
    TInt maskId = 0;
    delete iMifFileName;
    iMifFileName = NULL;
    iMifFileName = GetMifFileNameL(); 
    GetBitmapAndMaskIdL( bitmapId, maskId );
    
    if ( iMifFileName )
        {
        aAttr->SetMifFile( iMifFileName->AllocL() );
        }
    else
        {
        aAttr->SetMifFile( NULL );
        }
    aAttr->SetSkinId( SkinIdL() );
    aAttr->SetBitmapId( bitmapId );
    aAttr->SetMaskId( maskId );
    aAttr->SetApplicationUid( ApplicationUidL() );
    aAttr->SetBitmap( BitmapL() );
    aAttr->SetMask( MaskL() );
    aAttr->SetFileNameSrc( GetFileNameSrcL() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnMdValueImage::GetFileNameSrcL()
    {
    RBuf tmpBuf;
    CleanupClosePushL( tmpBuf );
    
    HnLiwUtils::GetStringL( *iParamList, iFileNameSrc8, iPos, tmpBuf );
    HBufC* ret = tmpBuf.AllocL();
    CleanupStack::PopAndDestroy( &tmpBuf );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnMdValueImage::FindMatchigMifFileL( const RFs& aFs, TDesC& aFileName )
    {
    DEBUG16(("_MM_: CHnMdValueImage::FindMatchigMifFileL IN"));

    _LIT( KGridRootMif, "gridroot.mif" );

    // whole path to the mif file was not specified
    TDriveList driveList;
    HBufC* ret = NULL;

    if ( aFileName == KGridRootMif )
        {
        ret = HBufC::NewL(
                1 + KRscPath().Length() + // 1 - drive letter len.
                aFileName.Length() );
        TPtr ptr( ret->Des() );
        TChar driveLetter;
        aFs.DriveToChar( EDriveZ, driveLetter );
        ptr.Append( driveLetter );
        ptr.Append( KRscPath );
        ptr.Append( aFileName );
        }
    else if ( KErrNone == aFs.DriveList( driveList ) )
        {
        TInt driveNumber( EDriveY ); // start with drive Y
        do // iterates through drives: Y, X, W, ..., C, B, A, Z
            {
            if ( driveList[ driveNumber ] )
                {
                ret = HBufC::NewLC(
                        1 + KRscPath().Length() + // 1 - drive letter len. 
                        aFileName.Length() );
                TPtr ptr( ret->Des() );
                TChar driveLetter;
                aFs.DriveToChar( driveNumber, driveLetter );
                ptr.Append( driveLetter );
                ptr.Append( KRscPath );
                ptr.Append( aFileName );
                
                if ( BaflUtils::FileExists( aFs, *ret ) )
                    {
                    // break if file was found
                    CleanupStack::Pop( ret );
                    break;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( ret );
                    ret = NULL;
                    }
                }
            /* The below formula is created so that the loop itarates
               through drives in the following order:
               Y, X, W, ..., D, C, B, A, Z.
               Drive Y is most important. */
            ( --driveNumber >= 0 ) ? driveNumber %= KMaxDrives :
                driveNumber = EDriveZ;
            }
        while( EDriveY != driveNumber );
        }
    DEBUG16(("_MM_: CHnMdValueImage::FindMatchigMifFileL OUT"));
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnMdValueImage::GetMifFileNameL()
    {
    DEBUG16(("_MM_: CHnMdValueImage::GetMifFileNameL IN"));
    HBufC* ret = NULL;
    RBuf tmpBuf;
    CleanupClosePushL( tmpBuf );

    TInt errCode = HnLiwUtils::GetStringL( *iParamList, iMifFile8, iPos, tmpBuf );
    if ( tmpBuf.Length() )
        {
        if ( BaflUtils::FileExists( iCmnPtrs->iFs, tmpBuf ) )
            {
            // return fileName as it is
            ret = tmpBuf.AllocL();
            }
        else
            {
            // fileName was retrieved but corresponding file was not found
            ret = FindMatchigMifFileL( iCmnPtrs->iFs, tmpBuf );
            }
        }
    
    // clean up
    CleanupStack::PopAndDestroy( &tmpBuf );
   
    DEBUG16(("_MM_: CHnMdValueImage::GetMifFileNameL OUT"));
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
TAknsItemID CHnMdValueImage::SkinIdL()
    {
    TAknsItemID retItemId( KAknsIIDNone );
    
    TInt skinIdMinor( KErrNotFound );
    TInt skinIdMajor( KErrNotFound );
    
    RBuf8 bufMajor;
    RBuf8 bufMinor;
    CleanupClosePushL(bufMinor);
    CleanupClosePushL(bufMajor);
    
    HnLiwUtils::GetStringL( *iParamList, iSkinIdMajor8, iPos, bufMajor );
    HnLiwUtils::GetStringL( *iParamList, iSkinIdMinor8, iPos, bufMinor );

    if ( KErrNone == HnConvUtils::Str8ToInt( bufMinor, skinIdMinor) &&
            KErrNone == HnConvUtils::Str8ToInt( bufMajor, skinIdMajor) )
        {
        retItemId.Set( skinIdMajor, skinIdMinor );
        }
     
    CleanupStack::PopAndDestroy(&bufMajor);
    CleanupStack::PopAndDestroy(&bufMinor);
    return retItemId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CHnMdValueImage::GetBitmapAndMaskIdL( TInt& aBitmapId, TInt& aMaskId )
    {
    aBitmapId = KErrNotFound;
    aMaskId = KErrNotFound;
        
    RBuf8 bitmapIdBuf;
    CleanupClosePushL( bitmapIdBuf );
    RBuf8 maskIdBuf;
    CleanupClosePushL( maskIdBuf );
    
    HnLiwUtils::GetStringL( *iParamList, iBitmapId8, iPos, bitmapIdBuf );
    HnLiwUtils::GetStringL( *iParamList, iMaskId8, iPos, maskIdBuf );
    TBool bitmapIdLookupNeeded =
            HnConvUtils::Str8ToInt( bitmapIdBuf, aBitmapId ) != KErrNone;
    TBool maskIdLookupNeeded =
            HnConvUtils::Str8ToInt( maskIdBuf, aMaskId ) != KErrNone;
    
    // if bitmap and mas ids are stored in mbg file
    if ( ( bitmapIdLookupNeeded || maskIdLookupNeeded ) && iMifFileName )
        {
        // look up in cache
        RBuf8 keyBitmap;
        RBuf8 keyMask;
        CreateKeysLC( keyBitmap, bitmapIdBuf, keyMask, maskIdBuf );
        
        iCmnPtrs->iBitmapIdCache->GetL( keyBitmap, aBitmapId );
        iCmnPtrs->iBitmapIdCache->GetL( keyMask, aMaskId );
        
        // if not found in cache read ids from mbg file
        if ( aBitmapId == KErrNotFound || aMaskId == KErrNotFound )
            {
            HBufC* path = ReplaceFileExtensionLC( *iMifFileName, KMbg() );
            HBufC8* fileContents = NULL;
            TRAPD( readFileErr, fileContents = HnUtils::ReadFileL( *path ) );
            if ( !readFileErr && fileContents )
                {
                CleanupStack::PushL( fileContents );
                if ( bitmapIdLookupNeeded )
                    {
                    aBitmapId = GetResourceIdL( *fileContents,  bitmapIdBuf );
                    iCmnPtrs->iBitmapIdCache->AddL( keyBitmap, aBitmapId );
                    }
                if ( maskIdLookupNeeded )
                    {
                    aMaskId = GetResourceIdL( *fileContents,  maskIdBuf );
                    iCmnPtrs->iBitmapIdCache->AddL( keyMask, aMaskId );
                    }
                CleanupStack::PopAndDestroy( fileContents );
                }
            CleanupStack::PopAndDestroy( path );
            }
        CleanupStack::PopAndDestroy( &keyMask );
        CleanupStack::PopAndDestroy( &keyBitmap );
        }
    
    CleanupStack::PopAndDestroy( &maskIdBuf );
    CleanupStack::PopAndDestroy( &bitmapIdBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnMdValueImage::ReplaceFileExtensionLC(
        const TDesC& aPath, const TDesC& aNewExt )
    {
    HBufC* newPath = HBufC::NewLC( aPath.Length() );
    newPath->Des().Copy( aPath );
    TInt extPos = newPath->LocateReverse( '.' );
    User::LeaveIfError( extPos );
    newPath->Des().Replace( extPos, newPath->Length() - extPos, aNewExt );
    return newPath;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
TUid CHnMdValueImage::ApplicationUidL()
    {
    TUid uid;
    TUint result( 0 );
    
    RBuf8 buf;
    CleanupClosePushL(buf);
    HnLiwUtils::GetStringL( *iParamList, iApplicationUid8, iPos, buf );
    
    if( buf.Length() )
        {
        TLex8 lex( buf );
        if( !buf.Find( KHexPrefix8 ) )
            {
            lex.Inc( KHexPrefix8().Length() );
            lex.Mark();
            }
        lex.Val( result, EHex );
        }
    uid.iUid = result;
    
    CleanupStack::PopAndDestroy(&buf);
    return uid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TLiwVariant* CHnMdValueImage::BitmapFromLiwL( const TDesC8& aPath )
    {
    TLiwVariant* ret = new( ELeave ) TLiwVariant;
    TInt err( KErrNone ); 
    TInt check( KErrNone );
    TRAP( err, check = HnLiwUtils::GetVariantL( *iParamList, aPath, iPos, *ret ) );
    if( err != KErrNone || check != KErrNone )
        {
        ret->Reset();
        delete ret;
        ret = NULL;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TLiwVariant* CHnMdValueImage::BitmapL()
    {
    TLiwVariant* ret = NULL;
    if( iBitmap8.Length() )
        {
        ret = BitmapFromLiwL( iBitmap8 );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TLiwVariant* CHnMdValueImage::MaskL()
    {
    TLiwVariant* ret = NULL;
    if( iMask8.Length() )
        {
        ret = BitmapFromLiwL( iMask8 );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CHnMdValueImage::CreateKeysLC( RBuf8& aKeyBitmap, RBuf8& aBitmapIdBuf, 
		RBuf8& aKeyMask, RBuf8& aMaskIdBuf )	
	{
	CleanupClosePushL( aKeyBitmap );
	CleanupClosePushL( aKeyMask );
	aKeyBitmap.CreateL( iMifFile8.Length() + aBitmapIdBuf.Length() );
	aKeyBitmap.Copy( iMifFile8 );
	aKeyBitmap.Append( aBitmapIdBuf );
	aKeyMask.CreateL( iMifFile8.Length() + aMaskIdBuf.Length() );
	aKeyMask.Copy( iMifFile8 );
	aKeyMask.Append( aMaskIdBuf );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
//CGulIcon* CHnMdValueImage::GetIconL(
//        const CLiwGenericParamList* aQueriesResultsList, TInt aPos )
//    {
//    CGulIcon* gulIcon = NULL;
//
//    iParamList = aQueriesResultsList;
//    iPos = aPos;
//    
//    // only to set iImageSource, to be optimized    
//    SetImageSourceL();
//    
//    TSize defaultSize( KDefaultIconSize, KDefaultIconSize );
//    
//    CFbsBitmap* bitmap = NULL; 
//    CFbsBitmap* mask = NULL;
//        
//    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
//    
//    switch( iImageSource )
//        {
//        case EImageSourceResource:
//            {
//            TInt bitmapId = 0;
//            TInt maskId = 0;
//            GetBitmapAndMaskIdL(bitmapId, maskId);
//            if ( iMifFileName && bitmapId != KErrNotFound )
//                {
//                AknsUtils::CreateIconL(
//                        skin, SkinIdL(), bitmap, mask,
//                        *iMifFileName, bitmapId, maskId );
//                }
//            }
//            break;
//        default:
//            {
//            User::Panic( KMatrixPanic , 1 );
//            }    
//        }
//       
//    if ( EImageSourceUndefined != iImageSource )
//        {        
//        if ( EImageSourceFile != iImageSource )
//            {
//            gulIcon = CGulIcon::NewL( bitmap, mask );
//            AknIconUtils::SetSize( mask, defaultSize );
//            AknIconUtils::SetSize( bitmap, defaultSize );
//            }
//        else
//            {
//            gulIcon = CGulIcon::NewL( bitmap );
//            AknIconUtils::SetSize( bitmap, defaultSize );
//            }
//        }
//    
//    return gulIcon;
//    }

