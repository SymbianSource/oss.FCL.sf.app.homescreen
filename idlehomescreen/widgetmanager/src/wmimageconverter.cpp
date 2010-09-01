/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmImageConverter implementation
*
*/

#include <fbs.h>
#include <eikdef.h>
#include <eikenv.h>
#include <bautils.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include <AknIconUtils.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <avkon.mbg>
#include <apgcli.h>
#include <SVGEngineInterfaceImpl.h>
#include <widgetregistryconstants.h> // KWidgetUidLowerBound, KWidgetUidUpperBound

#include "wmimageconverter.h"

_LIT( KSkin, 		  "skin" );
_LIT( KMif, 		  "mif" );
_LIT( KUid,           "uid" );
_LIT( KColon,		  ":" );
_LIT( KSvgExt,        ".svg" );
_LIT( KHexPrefix,     "0x" );
const TUint KLeftParenthesis = '(';

// ---------------------------------------------------------
// CWmImageConverter::NewL
// ---------------------------------------------------------
//
CWmImageConverter* CWmImageConverter::NewL()
    {
    CWmImageConverter* self = 
        new(ELeave) CWmImageConverter();
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop(self);
    return self; 
    }

// ---------------------------------------------------------
// CWmImageConverter::CWmImageConverter 
// ---------------------------------------------------------
//
CWmImageConverter::CWmImageConverter() 
    {
    iBitmap = NULL;
    iMask = NULL;
    }

// ---------------------------------------------------------
// CWmImageConverter::ConstructL
// ---------------------------------------------------------
//
void CWmImageConverter::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    iFs.ShareProtected();
    }

// ---------------------------------------------------------
// CWmImageConverter::~CWmImageConverter
// ---------------------------------------------------------
//
CWmImageConverter::~CWmImageConverter()
    {
    iFs.Close(); 
    if ( iBitmap ) 
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    if ( iMask )
        {
        delete iMask; 
        iMask = NULL;
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::HandleIconString
// ---------------------------------------------------------
//
TInt CWmImageConverter::HandleIconString( 
                            const TSize& aIconSize, 
                            const TDesC& aIconStr,
                            CFbsBitmap*& aBitmap,
                            CFbsBitmap*& aMask )
    {
    delete aBitmap; aBitmap = NULL;
    delete aMask; aMask = NULL;

    TInt err( KErrNone );
    TRAP( err, HandleIconStringL( aIconSize, aIconStr ); );
    if ( err == KErrNone && iBitmap && iMask )
        {
        // ownership transferred
        aBitmap = iBitmap;
        iBitmap = NULL;
        aMask = iMask;        
        iMask = NULL;
        }
    else
        {
        if ( iBitmap ) 
            {
            delete iBitmap;
            iBitmap = NULL;
            }
        if ( iMask )
            {
            delete iMask; 
            iMask = NULL;
            }
        }

    return err;
    }

// ---------------------------------------------------------
// CWmImageConverter::HandleIconStringL
// ---------------------------------------------------------
//
void CWmImageConverter::HandleIconStringL( 
                            const TSize& aIconSize, 
                            const TDesC& aIconStr )
    {
    if ( aIconStr.Length() )
        {
        TAknsItemID skinItemId;
        skinItemId.iMajor = 0;
        skinItemId.iMinor = 0;
        TInt bitmapId( KErrNotFound );
        TInt maskId( KErrNotFound );
        TUid appUid;
        iFilename = KNullDesC;
        iSize = aIconSize;
        
        if ( ResolveSkinIdAndMifId( 
                aIconStr, skinItemId, bitmapId, maskId, iFilename ) )
            {
            CreateSkinOrMifIconL( 
                    skinItemId, bitmapId, maskId, iFilename );
            }
        else if ( ResolveUid( aIconStr, appUid ) )
            {
            CreateIconFromUidL( appUid );
            }
        else if ( EndsWith( aIconStr, KSvgExt ) )
            {
            // filename_with_full_path.svg
            CreateIconFromSvgL( aIconStr );
            }
        else if ( BaflUtils::FileExists( iFs, aIconStr ) )
            {
            // filename_with_full_path.png/jpg
            CreateIconFromOtherL( aIconStr );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {            
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::CreateIconFromUidL
// ---------------------------------------------------------
//
void CWmImageConverter::CreateIconFromUidL( const TUid& aUid )
    {
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
   
    if ( aUid.iUid >= KWidgetUidLowerBound &&
       aUid.iUid < KWidgetUidUpperBound  )
        {
        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );
        CleanupClosePushL( lsSession );
        
        const TInt KAppSizeArraySize = 3;
        CArrayFixFlat<TSize>* sizeArray = new (ELeave)
            CArrayFixFlat<TSize>( KAppSizeArraySize );
        CleanupStack::PushL( sizeArray );

        User::LeaveIfError( lsSession.GetAppIconSizes( aUid, *sizeArray ) );
        
        // there should be atleast one size available
        if ( sizeArray->Count() == 0 ){ User::Leave( KErrNotReady ); };
        
        TSize size(0,0);
        for( TInt i=0; i < sizeArray->Count(); i++ )
            {
            TSize s = (*sizeArray)[i];
            if ( size.iWidth < s.iWidth || size.iHeight < s.iHeight )
                { size = s; }
            if ( size == iSize ) { break; }
            }

        CApaMaskedBitmap* maskedBmp = CApaMaskedBitmap::NewLC();
        User::LeaveIfError( lsSession.GetAppIcon( aUid, size, *maskedBmp ) );
        
        // handle bitmap
        iBitmap = new ( ELeave ) CFbsBitmap;       
        CopyBitmapL( *iBitmap, *maskedBmp );
        
        // handle mask
        if ( maskedBmp->Mask() )
            {
            iMask = new ( ELeave ) CFbsBitmap;
            CopyBitmapL( *iMask, *maskedBmp->Mask() );
            }
        
        // cleanup
        CleanupStack::PopAndDestroy( maskedBmp );
        CleanupStack::PopAndDestroy( sizeArray ); 
        CleanupStack::PopAndDestroy( &lsSession );
        }
    else if ( aUid.iUid != KNullUid.iUid )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TInt err( KErrNone );
        TRAP( err,
                {
                AknsUtils::CreateAppIconLC( skin, aUid,
                        EAknsAppIconTypeContext, bitmap, mask );
                CleanupStack::Pop( 2 ); // for trap
                }
            );
        
        if( KErrNone != err )
            {
            // If icon is not created, try to create default application icon
            AknsUtils::CreateIconLC( skin,
                KAknsIIDQgnMenuUnknownLst, bitmap, mask,
                AknIconUtils::AvkonIconFileName(),
                EMbmAvkonQgn_menu_unknown_lst,
                EMbmAvkonQgn_menu_unknown_lst_mask );
            CleanupStack::Pop( 2 ); // for trap
            }
        
        iBitmap = bitmap;
        iMask = mask;
        err = AknIconUtils::SetSize( iBitmap , iSize, EAspectRatioPreserved );
        if ( KErrNone == err )
            {
            err = AknIconUtils::SetSize( iMask , iSize, EAspectRatioPreserved );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::CreateIconFromSvgL
// ---------------------------------------------------------
//
void CWmImageConverter::CreateIconFromSvgL( const TDesC& aFileName )
    {
    if ( iBitmap )
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    if ( iMask )
        {
        delete iMask;
        iMask = NULL;
        }

    TDisplayMode mode = CEikonEnv::Static()->ScreenDevice()->DisplayMode();
    if ( mode >= ERgb ) // currently svg engine doesn't render correctly
        { mode = EColor16M; } // in this or above mode ( ou1cimx1#229434 )
    TFontSpec fontspec;
    
    CFbsBitmap* frameBuffer = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( frameBuffer );
    frameBuffer->Create( iSize, mode );
    
    CSvgEngineInterfaceImpl* svgEngine = CSvgEngineInterfaceImpl::NewL( 
            frameBuffer, NULL, fontspec );
    CleanupStack::PushL( svgEngine );
    
    svgEngine->SetDRMMode( EFalse );

    TInt domHandle;
    CheckSvgErrorL( svgEngine->PrepareDom( aFileName, domHandle ) );
    
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;    
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( iSize, mode ) );
    
    CFbsBitmap* mask = new(ELeave) CFbsBitmap;    
    CleanupStack::PushL( mask );
    User::LeaveIfError( mask->Create( iSize, EGray256 ) );
    
    CheckSvgErrorL( svgEngine->UseDom( domHandle, bitmap, mask ) );

    MSvgError* err;
    svgEngine->Start( err );
    CheckSvgErrorL( err );
    
    CheckSvgErrorL( svgEngine->UseDom( domHandle, NULL, NULL ) );
    CheckSvgErrorL( svgEngine->DeleteDom( domHandle ) );

    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    CleanupStack::PopAndDestroy( svgEngine );
    CleanupStack::PopAndDestroy( frameBuffer );
    
    iBitmap = bitmap;
    iMask = mask;
    }

// ---------------------------------------------------------
// CWmImageConverter::CreateIconFromOtherL
// ---------------------------------------------------------
//
void CWmImageConverter::CreateIconFromOtherL( const TDesC& aFileName )
    {
    if (iBitmap) {delete iBitmap; iBitmap = NULL;}
    if (iMask) {delete iMask; iMask = NULL;}
    
    iFilename.Copy( aFileName );
    
    // create the decoder
    CImageDecoder* imageDecoder = CImageDecoder::FileNewL( 
            iFs, iFilename, CImageDecoder::EOptionAlwaysThread );
    CleanupStack::PushL( imageDecoder );
    
    TSize size = imageDecoder->FrameInfo().iOverallSizeInPixels;

    // create the destination bitmap
    iBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iBitmap->Create( 
            size, imageDecoder->FrameInfo().iFrameDisplayMode ) ); 
    
    iMask = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iMask->Create( size, EGray256 ) ); 
    
    // start conversion to bitmap
    TRequestStatus status;
    imageDecoder->Convert( &status, *iBitmap, *iMask );
    User::WaitForRequest( status );
    if( status.Int() == KErrUnderflow ) 
        {
        imageDecoder->ContinueConvert( &status );
        User::WaitForRequest( status );
        }
    User::LeaveIfError( status.Int() );    
    CleanupStack::PopAndDestroy( imageDecoder );
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::CreateSkinOrMifIconL
// ---------------------------------------------------------------------------
//
void CWmImageConverter::CreateSkinOrMifIconL( 
                const TAknsItemID& aItemId, TInt aBitmapId, 
                TInt aMaskId, const TDesC& aFileName )
    {
    iFilename = aFileName;
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    // Load from skin 
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( skin && aItemId.iMajor != 0 && aItemId.iMinor != 0 )
        {
        TInt err( KErrNone );
        CAknsMaskedBitmapItemData* itemData = NULL;
        TRAP( err, itemData = 
                    static_cast<CAknsMaskedBitmapItemData*>(
                    skin->CreateUncachedItemDataL( aItemId, EAknsITMaskedBitmap ) ); );
        if( itemData && KErrNone == err )
            {
            CleanupStack::PushL( itemData );
            // Detach bitmaps
            bitmap = itemData->Bitmap();
            itemData->SetBitmap( NULL );
            mask = itemData->Mask();
            itemData->SetMask( NULL );
            CleanupStack::PopAndDestroy( itemData );
            }
        else
            {
            // look in imagetable
            CAknsImageTableItemData* iconData = NULL;
            TRAP( err, iconData = static_cast<CAknsImageTableItemData*>(
                            skin->CreateUncachedItemDataL( aItemId, EAknsITImageTable ) ); );
            if( iconData && KErrNone == err )
                {
                CleanupStack::PushL( iconData );
                if( iconData->NumberOfImages() )
                    {                        
                    TAknsItemID iconIId;
                    iconIId.Set( iconData->ImageIID(0) );
                    TRAP( err, AknsUtils::CreateIconL( 
                            skin, iconIId, bitmap, mask, KNullDesC, -1, -1 ); );
                    }
                CleanupStack::PopAndDestroy( iconData );
                }
            }
        
        if ( KErrNone == err && bitmap )
            {
            TInt err = AknIconUtils::SetSize( 
                    bitmap, 
                    iSize, 
                    EAspectRatioPreserved );
            if ( KErrNone == err  )
                {
                if ( mask )
                    {
                    err = AknIconUtils::SetSize( 
                            mask, 
                            iSize, 
                            EAspectRatioPreserved );
                    }
                iBitmap = bitmap;
                iMask = mask;
                return;
                }
            else
                {
                if ( bitmap ) { delete bitmap; bitmap = NULL; }
                if ( mask ){ delete mask; mask = NULL; }
                }
            }
        }
    
    if( aBitmapId != KErrNotFound && !bitmap && 
       aFileName.Length() && BaflUtils::FileExists( iFs, aFileName ) )
        {
        if ( aMaskId != KErrNotFound )
          {
          // Create icon from Mif filename , bitmap id and mask id          
          AknIconUtils::CreateIconL(
                  bitmap, mask, *this, aBitmapId, aMaskId );
          }
        else
          {
          bitmap = AknIconUtils::CreateIconL( *this, aBitmapId );
          }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    iBitmap = bitmap;
    iMask = mask;

    TInt err = AknIconUtils::SetSize( iBitmap , iSize, EAspectRatioPreserved );
    if ( KErrNone == err && iMask )
        {
        err = AknIconUtils::SetSize( iMask , iSize, EAspectRatioPreserved );
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::CheckSvgErrorL
// ---------------------------------------------------------
//
void CWmImageConverter::CheckSvgErrorL( MSvgError* aError )
    {
    if ( aError )
        {
        User::LeaveIfError( aError->SystemErrorCode() );
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::EndsWith
// ---------------------------------------------------------
//
TBool CWmImageConverter::EndsWith( const TDesC& aString, 
                                const TDesC& aPattern )
    {
    return ( aString.Right( aPattern.Length() ) == aPattern );
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::ResolveUid
// ---------------------------------------------------------------------------
//
TBool CWmImageConverter::ResolveUid( 
                const TDesC& aPath, TUid& aUid )
    {
    // Syntax: uid(0x12345678)
    TInt error = KErrNotFound;
    TInt pos = aPath.FindF( KUid );
    if( pos == 0 )
        {
        // Skip uid token
        pos += KUid().Length();

        // Initialize lexer
        TLex lex( aPath.Mid( pos ) );
        lex.SkipSpaceAndMark();
        
        // Check left parenthesis
        if ( lex.Get() == KLeftParenthesis )
            {
            error = ParseNextUint( lex, (TUint&)aUid.iUid );
            }
        }
    
    return (error == KErrNone );
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::ResolveSkinId
// ---------------------------------------------------------------------------
//
TBool CWmImageConverter::ResolveSkinId( 
                const TDesC& aPath, TAknsItemID& aItemId )
    {
    // Syntax: skin(major minor)
    TInt error = KErrNotFound;
    TInt pos = aPath.FindF( KSkin );
    if( pos == 0 )
        {
        // Skip skin token
        pos += KSkin().Length();

        // Initialize lexer
        TLex lex( aPath.Mid( pos ) );
        lex.SkipSpaceAndMark();
        
        // Check left parenthesis
        if ( lex.Get() == KLeftParenthesis )
           {
           TUint majorId = 0;
           TUint minorId = 0;
           error = ParseNextUint( lex, majorId );
           error |= ParseNextUint( lex, minorId );
           aItemId.Set( majorId, minorId );
           }        
        }

    return (error == KErrNone );
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::ResolveMifId
// ---------------------------------------------------------------------------
//
TBool CWmImageConverter::ResolveMifId( 
                const TDesC& aPath, TInt& aBitmapId,
                TInt& aMaskId, TDes& aFileName )
    {
    // Syntax: mif(filename bimapId maskId)
   TInt error = KErrNotFound;
   TInt pos = aPath.FindF( KMif );
   if( pos == 0 )
        {
        // Skip mif token
        pos += KMif().Length();
        // Initialize lexer
        TLex lex( aPath.Mid( pos ) );
        lex.SkipSpaceAndMark();
        
        // Check left parenthesis
        if ( lex.Get() == KLeftParenthesis )
           {
           lex.SkipSpaceAndMark();
           lex.SkipCharacters();
           // Resolve MifFile name
           aFileName.Copy(lex.MarkedToken());
           if( aFileName.Length()!= 0)
               {
               TUint bitmap, mask;
               error = ParseNextUint( lex, bitmap );
               ParseNextUint( lex, mask ); // mask is not mandatory
               aBitmapId = bitmap;
               aMaskId = mask;
               }
           }        
        }    
    return ( error == KErrNone );
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::ResolveSkinIdAndMifId
// ---------------------------------------------------------------------------
//
TBool CWmImageConverter::ResolveSkinIdAndMifId( 
                const TDesC& aPath, TAknsItemID& aItemId,
                TInt& aBitmapId, TInt& aMaskId, TDes& aFileName )
   {
   // Syntax: skin(major minor):mif(filename bimapId maskId) 
   TBool result = ResolveSkinId( aPath, aItemId );
   if ( result )
	   {
	   TInt pos = aPath.FindF( KColon );
	   if ( pos != KErrNotFound )
	       {
	       TPtrC ptr = aPath.Mid( pos+1 );
	       result = ResolveMifId( ptr, aBitmapId, aMaskId, aFileName );
	       }
	   }
   else
       {
       result = ResolveMifId( aPath, aBitmapId, aMaskId, aFileName );
       }
   return result;
   }

// ---------------------------------------------------------------------------
// CWmImageConverter::ParseNextUint()
// ---------------------------------------------------------------------------
//
TInt CWmImageConverter::ParseNextUint( TLex& aLex, TUint& aValue )
    {
    TInt error = KErrNone;
    aLex.SkipSpaceAndMark();
    aLex.SkipCharacters();

    TPtrC mtoken = aLex.MarkedToken();
    TInt pos = mtoken.FindF( KHexPrefix );
    if ( pos == 0 )
        {
		TLex innerLex( mtoken.Mid( KHexPrefix().Length() ) );
        error = innerLex.Val( aValue, EHex );
        }
    else
        {
        TLex innerLex( mtoken );
        error = innerLex.Val( aValue, EDecimal );
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::RetrieveIconFileHandleL
// ---------------------------------------------------------------------------
//
void CWmImageConverter::RetrieveIconFileHandleL( 
    RFile& aFile, const TIconFileType /*aType*/ )
    {
	TInt err = aFile.Open( iFs, iFilename, 
	        EFileRead | EFileShareReadersOnly );
	if ( KErrNone != err )
	    {
        User::Leave( err );
	    }
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::CopyBitmapL
// ---------------------------------------------------------------------------
//
void CWmImageConverter::CopyBitmapL( 
                            CFbsBitmap& aTrgBitmap,
                            CFbsBitmap& aSrcBitmap )
    {
    TSize size( aSrcBitmap.SizeInPixels() );
    TDisplayMode displayMode( aSrcBitmap.DisplayMode() );
    User::LeaveIfError( aTrgBitmap.Create( size, displayMode ) );
    
    HBufC8* scanLine = HBufC8::NewL( aSrcBitmap.ScanLineLength( 
            size.iWidth, displayMode ) ); 
    TPtr8 scanPtr( scanLine->Des() );                                                   
    TPoint pp;                                                                          
    for( pp.iY = 0; pp.iY < size.iHeight; ++pp.iY )                                    
        {                                                                               
        aSrcBitmap.GetScanLine( scanPtr, pp, size.iWidth, displayMode );                 
        aTrgBitmap.SetScanLine( scanPtr, pp.iY );                                     
        }                                                                               
    delete scanLine;                  
    }  

// ---------------------------------------------------------------------------
// CWmImageConverter::Finished
// ---------------------------------------------------------------------------
//
void CWmImageConverter::Finished()
    {
    // finishes using the icon file. No actions needed here.
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::UpdateImageSize
// ---------------------------------------------------------------------------
//
void CWmImageConverter::UpdateImageSize( 
        const TSize& aSize,
        const TDesC& aIconStr, 
        CFbsBitmap& aBitmap, 
        CFbsBitmap& aMask )
    {
    if ( aIconStr.Length() && iSize != aSize )
        {
        TAknsItemID skinItemId;
        skinItemId.iMajor = 0;
        skinItemId.iMinor = 0;
        TInt bitmapId( KErrNotFound );
        TInt maskId( KErrNotFound );
        TUid appUid;
        iFilename = KNullDesC;
        iSize = aSize;
        
        if ( ResolveSkinIdAndMifId( 
                aIconStr, skinItemId, bitmapId, maskId, iFilename ) )
            {
            AknIconUtils::SetSize( &aBitmap, iSize );
            AknIconUtils::SetSize( &aMask, iSize );
            }
        else if ( ResolveUid( aIconStr, appUid ) )
            {
            if ( appUid.iUid >= KWidgetUidLowerBound &&
                    appUid.iUid < KWidgetUidUpperBound  )
                {
                //WRT. No resize needed here
                return;
                }
            else if ( appUid.iUid != KNullUid.iUid )
                {
                AknIconUtils::SetSize( &aBitmap, iSize );
                AknIconUtils::SetSize( &aMask, iSize );
                }            
            }
        }
    }

// End of file

