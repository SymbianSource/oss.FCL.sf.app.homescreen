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
        // AknsUtils::CreateAppIconLC panics incase of WRT

        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );
        CleanupClosePushL( lsSession );
        
        const TInt KAppSizeArraySize = 3;
        CArrayFixFlat<TSize>* sizeArray = new (ELeave)
            CArrayFixFlat<TSize>( KAppSizeArraySize );
        CleanupStack::PushL( sizeArray );
        User::LeaveIfError( lsSession.GetAppIconSizes( aUid, *sizeArray ) );
        TSize size;
        for( TInt i=0; i < sizeArray->Count(); i++ )
            {
            size = (*sizeArray)[i];
            if ( size == iSize )
                {
                break;
                }
            }
        CApaMaskedBitmap* maskedBmp = CApaMaskedBitmap::NewLC();
        User::LeaveIfError( lsSession.GetAppIcon( aUid, size, *maskedBmp ) );
        iBitmap = static_cast<CFbsBitmap*>( maskedBmp );  // ownership transfered
        
        iMask = new ( ELeave ) CFbsBitmap;
        User::LeaveIfError( iMask->Create( 
                                    maskedBmp->Mask()->SizeInPixels(), 
                                    maskedBmp->Mask()->DisplayMode() ) );
        // duplicate mask, origional is owned by maskedBmp
        iMask->Duplicate( maskedBmp->Mask()->Handle() );
        CleanupStack::Pop( maskedBmp );
        maskedBmp = NULL;
        CleanupStack::PopAndDestroy(sizeArray); 
        CleanupStack::PopAndDestroy( &lsSession );

        // scale or notify
        if ( size == iSize )
            {
            iState = EIdle;
            iObserver->NotifyCompletion( KErrNone );
            }
        else
            {
            iScaleNeeded = ETrue;
            ScaleBitmap( iSize.iWidth, iSize.iHeight );
            }
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
    if ( KErrNone == err )
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

