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
CWmImageConverter* CWmImageConverter::NewL( MConverterObserver* aObserver )
    {
    CWmImageConverter* self = 
        new(ELeave) CWmImageConverter();
    CleanupStack::PushL( self );    
    self->ConstructL( aObserver );
    CleanupStack::Pop(self);
    return self; 
    }

// ---------------------------------------------------------
// CWmImageConverter::CWmImageConverter 
// ---------------------------------------------------------
//
CWmImageConverter::CWmImageConverter() 
    : CActive( EPriorityStandard )
    {
    iState = EIdle;
    iBitmap = NULL;
    iMask = NULL;
    iObserver = NULL;
    iConversionMethod = EUnrecognized;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CWmImageConverter::ConstructL
// ---------------------------------------------------------
//
void CWmImageConverter::ConstructL( MConverterObserver* aObserver )
    {
    User::LeaveIfError( iFs.Connect() );
    iFs.ShareProtected();
    iScaler = CBitmapScaler::NewL();
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CWmImageConverter::~CWmImageConverter
// ---------------------------------------------------------
//
CWmImageConverter::~CWmImageConverter()
    {
    Cancel();
    delete iImageDecoder;
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
    delete iScaler;
    }

// ---------------------------------------------------------
// CWmImageConverter::HandleIconString
// ---------------------------------------------------------
//
TInt CWmImageConverter::HandleIconString( 
                            TInt aWidth, TInt aHeight, 
                            const TDesC& aIconStr )
    {
    TInt err( KErrNone );
    TRAP( err, HandleIconStringL( aWidth, aHeight, aIconStr ); );
    if ( KErrNone != err )
        {
        iState = EFailed;
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
                            TInt aWidth, TInt aHeight, 
                            const TDesC& aIconStr )
    {
    iConversionMethod = EUnrecognized;
    iState = EDecoding;
    if ( aIconStr.Length() )
        {
        TAknsItemID skinItemId;
        skinItemId.iMajor = 0;
        skinItemId.iMinor = 0;
        TInt bitmapId( KErrNotFound );
        TInt maskId( KErrNotFound );
        TUid appUid;
        iFilename = KNullDesC;
        iScaleNeeded = EFalse;
        iSize.SetSize( aWidth, aHeight );
        
        if ( ResolveSkinIdAndMifId( 
                aIconStr, skinItemId, bitmapId, maskId, iFilename ) )
            {
            if ( bitmapId >= 0 && skinItemId.iMajor > 0 )
                iConversionMethod = ESkinAndMifIcon;
            else if ( bitmapId >= 0 )
                iConversionMethod = EMifIcon;
            else
                iConversionMethod = ESkinIcon;
            CreateSkinOrMifIconL( 
                    skinItemId, bitmapId, maskId, iFilename );
            }
        else if ( ResolveUid( aIconStr, appUid ) )
            {
            iConversionMethod = EUidIcon;
            CreateIconFromUidL( appUid );
            }
        else if ( EndsWith( aIconStr, KSvgExt ) )
            {
            // filename_with_full_path.svg
            iConversionMethod = ESvgIcon;
            CreateIconFromSvgL( aIconStr );
            }
        else if ( BaflUtils::FileExists( iFs, aIconStr ) )
            {
            // filename_with_full_path.png/jpg
            iConversionMethod = EImageIcon;
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
        User::LeaveIfError( iBitmap->Create( 
                maskedBmp->SizeInPixels(), 
                maskedBmp->DisplayMode() ) );

        // scale bitmap
        ScaleBitmapL( iSize, iBitmap, maskedBmp );  

        // handle mask
        if ( maskedBmp->Mask() )
            {
            iMask = new ( ELeave ) CFbsBitmap;
            User::LeaveIfError( iMask->Create( 
                    maskedBmp->Mask()->SizeInPixels(), 
                    maskedBmp->Mask()->DisplayMode() ) );
            
            // scale mask
            ScaleBitmapL( iSize, iMask, maskedBmp->Mask() );
            }
        
        // cleanup
        CleanupStack::PopAndDestroy( maskedBmp );
        CleanupStack::PopAndDestroy( sizeArray ); 
        CleanupStack::PopAndDestroy( &lsSession );
        
        // notify
        iState = EIdle;
        iObserver->NotifyCompletion( KErrNone );
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
                
        err = AknIconUtils::SetSize( iBitmap , iSize, EAspectRatioNotPreserved );
        if ( KErrNone == err )
            {
            err = AknIconUtils::SetSize( iMask , iSize, EAspectRatioNotPreserved );
            }

        // notify observer
        iState = EIdle;
        iObserver->NotifyCompletion( err );
        }
    else
        {
        iState = EIdle;
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
    iState = EIdle;
    iObserver->NotifyCompletion( KErrNone );
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
// CWmImageConverter::CreateIconFromOtherL
// ---------------------------------------------------------
//
void CWmImageConverter::CreateIconFromOtherL( const TDesC& aFileName )
    {
    if ( IsActive() )
        {
        User::Leave( KErrNotReady );
        }
    
    if ( iImageDecoder ) delete iImageDecoder; iImageDecoder = NULL;
    if (iBitmap) {delete iBitmap; iBitmap = NULL;}
    if (iMask) {delete iMask; iMask = NULL;}
    
    iFilename.Copy( aFileName );
    
    // create the decoder
    iImageDecoder = CImageDecoder::FileNewL( iFs, iFilename );
    
    TSize size = iImageDecoder->FrameInfo().iOverallSizeInPixels;

    // create the destination bitmap
    iBitmap = new (ELeave) CFbsBitmap();
    iBitmap->Create( size,
                     iImageDecoder->FrameInfo().iFrameDisplayMode ); 
    
    iMask = new (ELeave) CFbsBitmap();
    iMask->Create( size, EGray256 ); 

    if ( size != iSize )
        {
        iScaleNeeded = ETrue;
        }

    // start conversion to bitmap
    iImageDecoder->Convert( &iStatus, *iBitmap, *iMask );
    SetActive();
    }

// ---------------------------------------------------------
// CWmImageConverter::DoCancel
// ---------------------------------------------------------
//
void CWmImageConverter::DoCancel()
    {
    if( iState == EDecoding && 
       iConversionMethod == EImageIcon )
        {
        iImageDecoder->Cancel();
        iState = EIdle;
        if ( iObserver )
            {            
            iObserver->NotifyCompletion( KErrCancel );
            }
        }    
    else if( iState == EScalingBitmap ||
        iState == EScalingMask )
        {
        iScaler->Cancel();
        iState = EIdle;
        if ( iObserver )
            {            
            iObserver->NotifyCompletion( KErrCancel );
            }
        }    
    else
        {
        // State is EIdle, do nothing
        }    
    iScaleNeeded = EFalse;
    }

// ---------------------------------------------------------
// CWmImageConverter::RunL
// ---------------------------------------------------------
//
void CWmImageConverter::RunL()
    {
    switch( iState ) 
        {
        case EDecoding:
            {
            if( iStatus.Int() == KErrNone )
                {
                if ( iScaleNeeded )
                    {
                    ScaleBitmap( iSize.iWidth, iSize.iHeight );
                    }
                else
                    {
                    iState = EIdle;
                    if ( iObserver )
                        {
                        iObserver->NotifyCompletion( KErrNone );
                        }
                    }
                break;
                }
            else if( iStatus.Int() == KErrUnderflow ) 
                {
                iImageDecoder->ContinueConvert( &iStatus );
                SetActive();
                break;
                }
            else if ( iStatus.Int() == KErrCorrupt )
                {
                iState = EIdle;
                iScaleNeeded = EFalse;
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
                if ( iObserver )
                    {
                    iObserver->NotifyCompletion( KErrCorrupt );
                    }
                break;
                }
            else
                {
                // Unknown error
                iState = EIdle;
                iScaleNeeded = EFalse;
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
                if ( iObserver )
                    {
                    iObserver->NotifyCompletion( iStatus.Int() );
                    }
                break;
                }
            }
        case EScalingBitmap:
            {
            if( iStatus.Int() == KErrNone && iMask )
                {
                ScaleMask( iSize.iWidth, iSize.iHeight );
                }
            else
                {
                iState = EIdle;
                iScaleNeeded = EFalse;
                if ( iObserver )
                    {
                    iObserver->NotifyCompletion( iStatus.Int() );
                    }                
                }
            break;
            }
        case EScalingMask:
            {
            iState = EIdle;
            iScaleNeeded = EFalse;
            if ( iObserver )
                {
                iObserver->NotifyCompletion( iStatus.Int() );
                }
            break;
            }
        default:
            break;
        }   
    }

// ---------------------------------------------------------
// CWmImageConverter::RunError
// ---------------------------------------------------------
//
TInt CWmImageConverter::RunError(TInt /*aError*/)
    {
    // Our RunL does not contain any method calls that would leave, so this method
    // should never be called.
    iScaleNeeded = EFalse;
    return KErrNone;
    }

// ---------------------------------------------------------
// CWmImageConverter::ScaleBitmap
// ---------------------------------------------------------
//
void CWmImageConverter::ScaleBitmap( TInt aWidth, TInt aHeight )
    {
    if ( !IsActive() && 
         iBitmap &&
        ( iState == EDecoding || iState == EIdle ) )
        {
        iState = EScalingBitmap;
        // the maintain aspect ratio is by default set to true
        iScaler->Scale( &iStatus, *iBitmap, TSize( aWidth,aHeight ), EFalse );        
        SetActive();
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::ScaleMask
// ---------------------------------------------------------
//
void CWmImageConverter::ScaleMask( TInt aWidth, TInt aHeight )
    {
    if ( !IsActive() && 
        iState == EScalingBitmap &&
        iMask )
        {
        iState = EScalingMask;
        // the maintain aspect ratio is by default set to true
        iScaler->Scale( &iStatus, *iMask, TSize(aWidth,aHeight), EFalse );        
        SetActive();
        }
    }

// ---------------------------------------------------------
// CWmImageConverter::Bitmap
// ---------------------------------------------------------
//
CFbsBitmap* CWmImageConverter::Bitmap()
    {
    CFbsBitmap* bitmap = NULL;
    if (iState == EIdle &&
        iBitmap )
        {
        bitmap = iBitmap;  // ownership taken
        iBitmap = NULL;
        }
    return bitmap;
    }

// ---------------------------------------------------------
// CWmImageConverter::Mask
// ---------------------------------------------------------
//
CFbsBitmap* CWmImageConverter::Mask()
    {
    CFbsBitmap* mask = NULL;
    if (iState == EIdle &&
        iMask )
        {
        mask = iMask;  // ownership taken
        iMask = NULL;
        }
    return mask;
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
            TInt err = AknIconUtils::SetSize( bitmap , iSize, EAspectRatioNotPreserved );
            if ( KErrNone == err  )
                {
                if ( mask )
                    {
                    err = AknIconUtils::SetSize( mask , iSize, EAspectRatioNotPreserved );
                    }
                iBitmap = bitmap;
                iMask = mask;
                iState = EIdle;
                // notify observer                    
                iObserver->NotifyCompletion( KErrNone );
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
        iState = EIdle;
        User::Leave( KErrArgument );
        }
    
    iBitmap = bitmap;
    iMask = mask;

    TInt err = AknIconUtils::SetSize( iBitmap , iSize, EAspectRatioNotPreserved );
    if ( KErrNone == err && iMask )
        {
        err = AknIconUtils::SetSize( iMask , iSize, EAspectRatioNotPreserved );
        }
    
    iState = EIdle;
    // notify observer
    iObserver->NotifyCompletion( err );
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
// CWmImageConverter::SetLogoSize()
// ---------------------------------------------------------------------------
//
void CWmImageConverter::SetLogoSize( const TSize& aSize )
    {
    iSize = aSize;
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::ConversionMethod()
// ---------------------------------------------------------------------------
//
CWmImageConverter::TConversionMethod CWmImageConverter::ConversionMethod()
    {
    return iConversionMethod;
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
        iState = EIdle;
        User::Leave( err );
	    }
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::DoesScaleBitmapUseFallBack
// ---------------------------------------------------------------------------
//
TBool CWmImageConverter::DoesScaleBitmapUseFallBack( CFbsBitmap* aSrcBitmap )
    {
    if ( !aSrcBitmap )
        {
        return EFalse;
        }

    TDisplayMode displayMode = aSrcBitmap->DisplayMode();
    TBool fallbackOnly = EFalse;

    switch ( displayMode )
        {
        case EGray2:
        case EGray4:
        case EGray16:
        case EColor16:
        case EColor16M:
        case ERgb:
        case EColor16MA:
            fallbackOnly = ETrue;
            break;
        case EGray256:
        case EColor4K:
        case EColor64K:
        case EColor256:
        case EColor16MU:
            // These are the supported modes
            break;
        default:
            fallbackOnly = ETrue;
        }

    return fallbackOnly;
    }


// ---------------------------------------------------------------------------
// CWmImageConverter::ScaleBitmapL
// ---------------------------------------------------------------------------
//
void CWmImageConverter::ScaleBitmapL( 
                            const TSize& aSize,
                            CFbsBitmap* aTrgBitmap,
                            CFbsBitmap* aSrcBitmap )
    {
    if ( !aSrcBitmap ) User::Leave( KErrArgument );
    if ( !aTrgBitmap ) User::Leave( KErrArgument );
    if ( aSrcBitmap->DisplayMode() != aTrgBitmap->DisplayMode() )
        {
        User::Leave( KErrArgument );
        }
    
    TRect targetRect( aSize );

    // calculate aspect ratio
    TInt srcHeight = aSrcBitmap->SizeInPixels().iHeight;
    TInt srcWidth = aSrcBitmap->SizeInPixels().iWidth;
    TReal scaleRatio( 1 ); //no scale as defaul
    
    //If any dimension is 0, then we do not bother to scale
    if ( targetRect.Width() > 0 && targetRect.Height() > 0 )
        {
        TReal xRatio = ( ( TReal )srcWidth / ( TReal )targetRect.Width() );
        TReal yRatio = ( ( TReal )srcHeight / ( TReal )targetRect.Height() );
        //Find out appropriate scaling factor
        xRatio > yRatio ? ( scaleRatio = xRatio ) : ( scaleRatio = yRatio );
        }

    //Scale the size for target bitmap
    targetRect.SetHeight( srcHeight / scaleRatio );
    targetRect.SetWidth( srcWidth / scaleRatio );    
    
    TSize trgBitmapSize = aTrgBitmap->SizeInPixels();
    
    // calculate the valid drawing area
    TRect drawRect = targetRect;
    drawRect.Intersection( TRect( TPoint( 0, 0 ), trgBitmapSize ) );

    if( drawRect.IsEmpty() || 
        aSrcBitmap->SizeInPixels().iHeight <= 0 || 
        aSrcBitmap->SizeInPixels().iWidth <= 0 )
        {
        User::Leave( KErrArgument );
        }

    TSize srcSize = aSrcBitmap->SizeInPixels();

    TBool srcTemporary = EFalse;
    if ( aSrcBitmap->IsRomBitmap() )
        {
        srcTemporary = ETrue;
        }

    TDisplayMode displayMode = aSrcBitmap->DisplayMode();
    TBool fallbackOnly = DoesScaleBitmapUseFallBack( aSrcBitmap );

    if ( fallbackOnly )
        {
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( aTrgBitmap );
        CleanupStack::PushL( dev );
        CFbsBitGc* gc = NULL;
        User::LeaveIfError( dev->CreateContext( gc ) );
        CleanupStack::PushL( gc );

        // write alpha information if it exists
        if ( aSrcBitmap->DisplayMode() == EColor16MA )
            {
            gc->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
            }

        // targetRect is used because DrawBitmap handles clipping automatically
        gc->DrawBitmap( targetRect, aSrcBitmap );
        CleanupStack::PopAndDestroy( 2 ); // dev, gc
        return;
        }

    // Heap lock for FBServ large chunk to prevent background
    // compression of aSrcBitmap after if IsCompressedInRAM returns EFalse
    aSrcBitmap->LockHeapLC( ETrue ); // fbsheaplock
    TBool fbsHeapLock = ETrue;
    if ( aSrcBitmap->IsCompressedInRAM() )
        {
        srcTemporary = ETrue;
        }

    CFbsBitmap* realSource = aSrcBitmap;
    if ( srcTemporary )
        {
        CleanupStack::PopAndDestroy(); // fbsheaplock
        fbsHeapLock = EFalse;

        realSource = new ( ELeave ) CFbsBitmap();
        CleanupStack::PushL( realSource );
        User::LeaveIfError(
            realSource->Create( srcSize, aSrcBitmap->DisplayMode() ) );
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( realSource );
        CleanupStack::PushL( dev );
        CFbsBitGc* gc = NULL;
        User::LeaveIfError( dev->CreateContext( gc ) );
        CleanupStack::PushL( gc );
        gc->BitBlt( TPoint( 0, 0 ), aSrcBitmap );
        CleanupStack::PopAndDestroy( 2 ); // dev, gc
        }

    if ( !fbsHeapLock )
        {
        // Heap lock for FBServ large chunk is only needed with large bitmaps.
        if ( realSource->IsLargeBitmap() || aTrgBitmap->IsLargeBitmap() )
            {
            aTrgBitmap->LockHeapLC( ETrue ); // fbsheaplock
            }
        else
            {
            CleanupStack::PushL( ( TAny* )NULL );
            }
        }

    TUint32* srcAddress = realSource->DataAddress();
    TUint32* trgAddress = aTrgBitmap->DataAddress();

    const TInt xSkip = ( srcSize.iWidth << 8 ) / targetRect.Width();
    const TInt ySkip = ( srcSize.iHeight << 8 ) / targetRect.Height();

    const TInt drawWidth  = drawRect.Width();
    const TInt drawHeight = drawRect.Height();

    TRect offsetRect( targetRect.iTl, drawRect.iTl );
    const TInt yPosOffset = ySkip * offsetRect.Height();
    const TInt xPosOffset = xSkip * offsetRect.Width();

    if ( ( displayMode == EGray256 ) || ( displayMode == EColor256 ) )
        {
        TInt srcScanLen8 = CFbsBitmap::ScanLineLength(
            srcSize.iWidth, displayMode );
        TInt trgScanLen8 = CFbsBitmap::ScanLineLength(
            trgBitmapSize.iWidth, displayMode );

        TUint8* trgAddress8 = reinterpret_cast< TUint8* >( trgAddress );

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress8 += trgScanLen8 * drawRect.iTl.iY + drawRect.iTl.iX;

        for ( TInt y = 0; y < drawHeight; y++ )
            {
            TUint8* srcAddress8 = reinterpret_cast< TUint8* >( srcAddress ) +
                ( srcScanLen8 * ( yPos >> 8 ) );

            TInt xPos = xPosOffset;
            for ( TInt x = 0; x < drawWidth; x++ )
                {
                *( trgAddress8++ ) = srcAddress8[xPos >> 8];
                xPos += xSkip;
                }

            yPos += ySkip;

            trgAddress8 += trgScanLen8 - drawWidth;
            }
        }
    else if ( displayMode == EColor4K || displayMode == EColor64K )
        {
        TInt srcScanLen16 = CFbsBitmap::ScanLineLength(
            srcSize.iWidth, displayMode ) /2;
        TInt trgScanLen16 = CFbsBitmap::ScanLineLength(
            trgBitmapSize.iWidth, displayMode ) /2;

        TUint16* trgAddress16 = reinterpret_cast< TUint16* >( trgAddress );

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress16 += trgScanLen16 * drawRect.iTl.iY + drawRect.iTl.iX;

        for ( TInt y = 0; y < drawHeight; y++ )
            {
            TUint16* srcAddress16 = reinterpret_cast< TUint16* >( srcAddress ) +
                ( srcScanLen16 * ( yPos >> 8 ) );

            TInt xPos = xPosOffset;
            for ( TInt x = 0; x < drawWidth; x++ )
                {
                *( trgAddress16++ ) = srcAddress16[xPos >> 8];
                xPos += xSkip;
                }

            yPos += ySkip;

            trgAddress16 += trgScanLen16 - drawWidth;
            }
        }
    else if ( displayMode == EColor16MU )
        {
        TInt srcScanLen32 = CFbsBitmap::ScanLineLength(
            srcSize.iWidth, displayMode ) /4;
        TInt trgScanLen32 = CFbsBitmap::ScanLineLength(
            trgBitmapSize.iWidth, displayMode ) /4;

        TUint32* trgAddress32 = reinterpret_cast< TUint32* >( trgAddress );

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress32 += trgScanLen32 * drawRect.iTl.iY + drawRect.iTl.iX;

        for ( TInt y = 0; y < drawHeight; y++ )
            {
            TUint32* srcAddress32 = reinterpret_cast< TUint32* >( srcAddress ) +
                ( srcScanLen32 * ( yPos >> 8 ) );

            TInt xPos = xPosOffset;
            for ( TInt x = 0; x < drawWidth; x++ )
                {
                *( trgAddress32++ ) = srcAddress32[xPos >> 8];
                xPos += xSkip;
                }

            yPos += ySkip;

            trgAddress32 += trgScanLen32 - drawWidth;
            }
        }
    else
        {
        User::Leave( KErrUnknown );
        }

    CleanupStack::PopAndDestroy(); // fbsheaplock

    if ( srcTemporary )
        {
        CleanupStack::PopAndDestroy(); // realSource
        }
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
// CWmImageConverter::IsProcessing
// ---------------------------------------------------------------------------
//
TBool CWmImageConverter::IsProcessing()
    {
    return ( ( ( iState != EIdle && iState != EFailed ) ||
            IsActive() ) ? ETrue : EFalse );
    }

// End of file

