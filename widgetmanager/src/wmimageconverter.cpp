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
#include <WidgetRegistryConstants.h> // KWidgetUidLowerBound, KWidgetUidUpperBound

#include "wmiconfileprovider.h"
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
    iIconFileProvider = CWmIconFileProvider::NewL( iFs );
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
    if (iBitmap) 
        {
        delete iBitmap;
        iBitmap = NULL;
        }    
    if (iMask) 
        {
        delete iMask; 
        iMask = NULL;
        }    
    delete iScaler;
    delete iIconFileProvider;
    }

// ---------------------------------------------------------
// CWmImageConverter::HandleIconStringL
// ---------------------------------------------------------
//
void CWmImageConverter::HandleIconStringL( 
                            TInt aWidth, TInt aHeight, 
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
            iConversionMethod = EUnrecognized;
            User::Leave( KErrArgument );
            }
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
    
        CArrayFixFlat<TSize>* sizeArray = new (ELeave) CArrayFixFlat<TSize>(3);
        CleanupStack::PushL( sizeArray );
        User::LeaveIfError( lsSession.GetAppIconSizes( aUid, *sizeArray ) );
        TInt sizeCount = sizeArray->Count();
        TSize size;
        if ( sizeCount > 0 )
            {
            for( TInt i=0; i < sizeArray->Count(); i++ )
                {
                size = (*sizeArray)[i];
                if ( size == iSize )
                    {
                    break;
                    }
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
        iObserver->NotifyCompletion( err );
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
    TInt domHandle;
    CFbsBitmap* dummyBitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( dummyBitmap );
    
    dummyBitmap->Create( TSize( 0, 0 ), EGray2 );
    TFontSpec fontSpec;
    CSvgEngineInterfaceImpl* svgEngine =
        CSvgEngineInterfaceImpl::NewL( dummyBitmap, NULL, fontSpec );
    CleanupStack::PushL( svgEngine );
    
    svgEngine->SetBackgroundColor( 0 );
    svgEngine->SetDRMMode( EFalse );
   
    User::LeaveIfError( svgEngine->PrepareDom( 
                aFileName, domHandle )->SystemErrorCode() );
    
    iBitmap = new ( ELeave ) CFbsBitmap;
    
    User::LeaveIfError( iBitmap->Create( iSize, EColor16M ) );
    
    iMask = new ( ELeave ) CFbsBitmap;
    User::LeaveIfError( iMask->Create( iSize, EGray256 ) );
    
    svgEngine->UseDom( domHandle, iBitmap, iMask );
    svgEngine->SetPreserveAspectRatio( NULL, ESvgPreserveAspectRatio_XmidYmid, 
            ESvgMeetOrSlice_Meet, ETrue );
    svgEngine->SetSvgDimensionToFrameBuffer(
        iSize.iWidth, iSize.iHeight );

    svgEngine->Start();
 
    if ( iBitmap->SizeInPixels() != iSize ||
        svgEngine->ContentDimensions() != iSize )    
        {        
        iScaleNeeded = ETrue;
        ScaleBitmap( iSize.iWidth, iSize.iHeight );
        }
    else
        {
        // notify observer
        iObserver->NotifyCompletion( KErrNone );
        }
    
    svgEngine->UseDom( domHandle, NULL, NULL );
    svgEngine->DeleteDom( domHandle );
    
    CleanupStack::PopAndDestroy( 2, dummyBitmap ); // dummyBitmap, svgEngine
    }

// ---------------------------------------------------------
// CWmImageConverter::CreateIconFromOtherL
// ---------------------------------------------------------
//
void CWmImageConverter::CreateIconFromOtherL( const TDesC& aFileName )
    {
    if ( IsActive() || iState != EIdle )
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
    iState = EDecoding;
    iImageDecoder->Convert( &iStatus, *iBitmap, *iMask );
    SetActive();
    }

// ---------------------------------------------------------
// CWmImageConverter::DoCancel
// ---------------------------------------------------------
//
void CWmImageConverter::DoCancel()
    {
    if( iState == EDecoding )
        {
        iImageDecoder->Cancel();        
        if ( iObserver )
            {
            iObserver->NotifyCompletion( KErrCancel );
            }
        }    
    else if( iState == EScalingBitmap ||
        iState == EScalingMask )
        {
        iScaler->Cancel();
        if ( iObserver )
            {
            iObserver->NotifyCompletion( KErrCancel );
            }
        }    
    else
        {
        // State is EIdle, do nothing
        }
    iState = EIdle;
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
        // the maintain aspect ratio is by default set to true
        iScaler->Scale( &iStatus, *iBitmap, TSize( aWidth,aHeight ), EFalse );
        iState = EScalingBitmap;
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
        // the maintain aspect ratio is by default set to true
        iScaler->Scale( &iStatus, *iMask, TSize(aWidth,aHeight), EFalse );
        iState = EScalingMask;
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
        // Skip skin token
        pos += KUid().Length();

        // Initialize lexer
        TLex lex( aPath.Mid( pos ) );

        // Check left parenthesis
        if ( lex.Get() == KLeftParenthesis )
            {
            lex.SkipSpaceAndMark();
            lex.SkipCharacters();
            
            TPtrC mtoken = lex.MarkedToken();
            pos = mtoken.FindF( KHexPrefix );
            if ( pos == 0 )
                {
                TLex lex( mtoken.Mid( KHexPrefix().Length() ) );
                TUint id = 0;
                error = lex.Val( id, EHex );
                aUid = TUid::Uid( (TInt)id );
                }
            else
                {
                TInt id( 0 );
                error = lex.Val( id );
                aUid.iUid = id;
                }
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
           pos++;
           TLex lex( aPath.Mid( pos ) );
           lex.SkipSpaceAndMark();
           
           TPtrC mtoken = lex.MarkedToken();
           pos = mtoken.FindF( KHexPrefix );
           if ( pos == 0 )
              {
              TUint majorId( 0 );
              TUint minorId( 0 );              
              lex.Assign( mtoken.Mid( KHexPrefix().Length() ) );
              error = lex.Val( majorId, EHex );
              lex.SkipSpace();
              lex.SkipAndMark( KHexPrefix().Length() );
              error |= lex.Val( minorId, EHex );
              aItemId.Set( majorId, minorId );
              }
          else
              {
              TInt majorId(0);
              TInt minorId(0);
              error = lex.Val( majorId );
              lex.SkipSpace();
              error |= lex.Val( minorId );                      
              aItemId.Set( majorId, minorId );
              }
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

        // Check left parenthesis
        if ( lex.Get() == KLeftParenthesis )
           {
           lex.SkipSpaceAndMark();
           lex.SkipCharacters();
           // Resolve MifFile name
           aFileName.Copy(lex.MarkedToken());
           if( aFileName.Length()!= 0)
               {
               // Resolve bitmap id  
               lex.SkipSpace();
               error = lex.Val( aBitmapId );
               
               // Resolve mask id
               // dont return error if it is not found, that is ok
               lex.SkipSpace();
               lex.Val( aMaskId );
               }
           else
               {
               error = KErrNotFound;
               }
           }        
        }    
    return (error == KErrNone );
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
	iIconFileProvider->SetIconFile( aFileName );

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
                  bitmap, mask, *iIconFileProvider, aBitmapId, aMaskId );
          }
        else
          {
          bitmap = AknIconUtils::CreateIconL( *iIconFileProvider, aBitmapId );
          }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    iBitmap = bitmap;
    iMask = mask;
            
    TInt err = AknIconUtils::SetSize( iBitmap , iSize, EAspectRatioNotPreserved );
    if ( KErrNone == err )
        {
        err = AknIconUtils::SetSize( iMask , iSize, EAspectRatioNotPreserved );
        }

    // notify observer
    iObserver->NotifyCompletion( err );
    }

// ---------------------------------------------------------------------------
// CWmImageConverter::ConversionMethod()
// ---------------------------------------------------------------------------
//
CWmImageConverter::TConversionMethod CWmImageConverter::ConversionMethod()
    {
    return iConversionMethod;
    }

// End of file

