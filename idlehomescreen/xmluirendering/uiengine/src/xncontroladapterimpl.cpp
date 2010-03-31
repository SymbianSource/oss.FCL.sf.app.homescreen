/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Xuikon control adapter source file
*
*/


// System includes
#include <utf.h>
#include <SVGEngineInterfaceImpl.h>
#include <s32file.h>
#include <mifconvdefs.h>
//skinning support
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <gulicon.h>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK

// User includes
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xnviewmanager.h"
#include "xncontroladapterimpl.h"
#include "xncontroladapter.h"
#include "xnmenuadapter.h"
#include "xnviewcontroladapter.h"
#include "xncomponentnodeimpl.h"
#include "xntype.h"
#include "xnnodepluginif.h"
#include "xnuiengine.h"
#include "xnviewnodeimpl.h"
#include "c_xnutils.h"
#include "xndomdocument.h"
#include "xndomattribute.h"
#include "xndomnode.h"
#include "xnproperty.h"
#include "xndomlist.h"
#include "xnodt.h"
#include "xnresource.h"
#include "xnhittest.h"
#include "xnplugindata.h"
#include "xnnode.h"
#include "xnpanic.h"
#include "xnviewdata.h"
#include "xnscrollablecontroladapter.h"
#include "xnfocuscontrol.h"
#include "xneditmode.h"

_LIT8(KScrollableBoxNodeName, "scrollablebox");
        
using namespace XnGestureHelper;
#include "xngesture.h"

// Constants
const TInt KSkinGfxInnerRectShrink = 5;
const TInt KFocusGrowValue = 3;

const TInt KLongTapStartShortDelay( 150000 ); // 0.15s for Sk
const TInt KLongTapStartLongDelay( 500000 ); // 0.5s
const TInt KLongTapTimeShortDelay( 600000 ); // 0.6s for SK
const TInt KLongTapTimeLongDelay( 1500000 ); // 1.5s 

// LOCAL FUNCTION PROTOTYPES
static TRgb ConvertHslToRgb( TInt aHue, TInt aSaturation, TInt aLightness );
static void ConvertRgbToHsl( TRgb aRGB, TUint& aHue, TUint& aSaturation,
    TUint& aLightness );
static void LoadFbsBitmapL(
    CXnResource& aResource, CFbsBitmap*& aTarget, RFs& aFsSession );
static void LoadMbmBitmapL( CXnResource& aResource,
    TInt aBitmapIndex, CFbsBitmap*& aTarget, RFs& aFsSession );
static void LoadSVGBitmapL( CXnResource& aResource,
    CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask,
    TRect aRect, RFs& aFsSession );
static TBool GetBitmapL( CXnUiEngine& aEngine, CXnNode& aNode,
    const TDesC& aResourceFileName, TInt& aBitmapIndex,
    CFbsBitmap*& aTarget, CFbsBitmap*& aBitmapMask,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider,
    TRect aRect, RFs& aFsSession );

static void LimitRectToPaddingRect( CXnNode& aNode, TRect& aRect );
static void DrawSpaceBackgroundY( CWindowGc& aGc, TInt aYRepeatCount,
    CXnNode& aNode, TRect& aRect, CFbsBitmap* aBitmap, TRect aBitmapRect,
    TRect aImageRect, TInt& aYOffset );
static void DrawRepeatBackgroundY( CWindowGc& aGc, TInt aYRepeatCount,
    CXnNode& aNode, TRect& aRect, CFbsBitmap* aBitmap, TRect aBitmapRect,
    TRect aImageRect, TInt& aYOffset );
static void StripQuotes( HBufC*& aString );
static void DrawBackgroundColorL(
    const TRect& /*aRect*/, CXnNode& aNode, CWindowGc& aGc );
static void GetBackroundRepeatValuesL( CXnNode& aNode, TBool& aRepeatX,
    TBool& aRepeatY, TBool& aSpaceX, TBool& aSpaceY );
static void DrawRepeatedBackgroundImage( CWindowGc& aGc, CXnNode& aNode,
    TRect& aRect, TRect aImageRect, TRect aBitmapRect,
    CFbsBitmap* aBitmap, TBool aRepeatX, TBool aRepeatY, TBool aSpaceX,
    TBool aSpaceY, TInt aXRepeatCount, TInt aYRepeatCount );
static TBool GetBackgroundPositionFromPropertyL(
    CXnNode& aNode, TBool aScaleImage, TRect& aRect,
    TRect aImageRect, CFbsBitmap* aBitmap, TBool& aHeightSet );
static TBool GetBackgroundSizeFromPropertyL( TRect& aRect, CXnNode& aNode,
    CFbsBitmap* aBitmap, TBool& aScaleImage );
static TRgb LighterColor( const TRgb& aRgb );
static TRgb DarkerColor( const TRgb& aRgb );
static void DrawSolidTopBorder( CArrayFix< TPoint >* aArray, CWindowGc& aGc );
static void SplitTopBorderPolygonL( CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aUpperPart, CArrayFix< TPoint >*& aLowerPart );
static void DrawRidgeTopBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawGrooveTopBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawInsetTopBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawOutsetTopBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static TBool GetBorderColorL(
    const TDesC8& aPropertyName, CXnNode& aNode, CWindowGc& aGc, TRgb& aRgb );
static void DrawTopBorderL(
    const TRect& /*aRect*/, CXnNode& aNode, CWindowGc& aGc );
static void DrawBottomBorderL(
    const TRect& /*aRect*/, CXnNode& aNode, CWindowGc& aGc );
static void DrawSolidBottomBorder( CArrayFix< TPoint >* aArray, CWindowGc& aGc );
static void SplitBottomBorderPolygonL( CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aUpperPart, CArrayFix< TPoint >*& aLowerPart );
static void DrawRidgeBottomBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawGrooveBottomBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawInsetBottomBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawOutsetBottomBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawSolidLeftBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc );
static void SplitLeftBorderPolygonL( CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aLeftPart, CArrayFix< TPoint >*& aRightPart );
static void DrawRidgeLeftBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawGrooveLeftBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawInsetLeftBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawOutsetLeftBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawLeftBorderL(
    const TRect& /*aRect*/, CXnNode& aNode, CWindowGc& aGc );
static void DrawRightBorderL(
    const TRect& /*aRect*/, CXnNode& aNode, CWindowGc& aGc );
static void DrawSolidRightBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc );
static void SplitRightBorderPolygonL( CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aLeftPart, CArrayFix< TPoint >*& aRightPart );
static void DrawRidgeRightBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawGrooveRightBorderL(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawInsetRightBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawOutsetRightBorder(
    CArrayFix< TPoint >* aArray, CWindowGc& aGc, TRgb& aRgb );
static void DrawBorderImagesL(
    const TRect& /*aRect*/, CXnNode& aNode, CWindowGc& aGc,
    CFbsBitmap* aBitmap, TInt aBorderBitmapDividerTop,
    TInt aBorderBitmapDividerRight,
    TInt aBorderBitmapDividerBottom, TInt aBorderBitmapDividerLeft );
static TBool IsPropertyNone( CXnProperty& aProperty );
static void DrawBordersL( const TRect& aRect, CXnNode& aNode, CWindowGc& aGc );
static void SwapChildArrays( RPointerArray< CXnControlAdapter >& originalArray,
    RPointerArray< CXnControlAdapter >& sortedArray );
static CXnProperty* GetZIndexL( CXnControlAdapter* aAdapter );
static void InsertChildToSortedArrayL(
    RPointerArray< CXnControlAdapter >& aTargetArray,
    CXnControlAdapter* aChild );
static void InitializeBackgroundBitmapL( CXnUiEngine& aEngine, CXnNode& aNode,
    TInt& aBitmapIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider, RFs& aFsSession );
static HBufC* GetBackgroundImagePathLC( CXnNode& aNode );
static CFbsBitmap* InitializeBorderBitmapL(
    CXnUiEngine& aEngine, CXnNode& aNode,
    TInt& aBitmapIndex, TInt& aBorderBitmapDividerTop,
    TInt& aBorderBitmapDividerRight, TInt& aBorderBitmapDividerBottom,
    TInt& aBorderBitmapDividerLeft,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider, RFs& aFsSession );
static void DrawDottedBorder(
    TRect aBorderRect, CWindowGc& aGc, TRgb& aRgb, TBool aHorizontal );
static HBufC* GetBackgroundImageMaskPathLC( CXnNode& aNode );
static CFbsBitmap* CreateBitmapFromColorL( TSize aSize, TRgb aColor );
static TBool CreateGestureHelperL( CXnNode& aNode );
static TInt AccessResourceFileL(
    CXnResource& aResource, RFile& aFile, RFs& aFsSession );
static void DrawFocusAppearance( CXnNode& aNode, CWindowGc& aGc );
static CXnDomNode* HasHoldTrigger( CXnDomNode* aNode );
static void CancelFocusRefusalL( CXnUiEngine& aUiEngine );
    
// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// 
static CXnScrollableControlAdapter* IsChildOfScrollableControl( CXnNode& aNode )
    {
    CXnNode* parent = aNode.Parent();
    while( parent )
        {
        if( parent->Type()->Type() == KScrollableBoxNodeName )
            {
            CXnScrollableControlAdapter* ret = static_cast<CXnScrollableControlAdapter*>(parent->Control());
            return ret;
            }
        parent = parent->Parent();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// SetControlContext
// 
// -----------------------------------------------------------------------------
// 
static void SetControlContext(CXnControlAdapter& aChild, CXnNode& aNode)
    {
    CXnScrollableControlAdapter* scrollableControl = IsChildOfScrollableControl( aNode );
    if(scrollableControl)
        {
        aNode.SetScrollableControl(scrollableControl);
        aChild.CopyControlContextFrom(scrollableControl);
        }        
    }

// -----------------------------------------------------------------------------
// AccessResourceFileL
// Opens a resource file for accessing. The caller must close the file.
// -----------------------------------------------------------------------------
//
static TInt AccessResourceFileL(
    CXnResource& aResource,
    RFile& aFile,
    RFs& aFsSession )
    {
    TInt err = KErrNone;

    // parse file name and extension
    const TDesC& fileName = aResource.FileName();
    TParsePtrC fileParser( fileName );
    TPtrC filepath = fileParser.DriveAndPath();

    // open resource file
    aFsSession.SetSessionPath( filepath );
    aFsSession.ShareProtected();
    err = aFile.Open( aFsSession, fileName, EFileShareReadersOnly );
    if ( err != KErrNone )
        {
        aFile.Close();
        }

    return err;
    }

// -----------------------------------------------------------------------------
// LoadFbsBitmapL
// Loads a bitmap file from the theme server and creates a CFbsBitmap
// -----------------------------------------------------------------------------
//
static void LoadFbsBitmapL(
    CXnResource& aResource,
    CFbsBitmap*& aTarget,
    RFs& aFsSession )
    {
     RFile file;

     TInt ret = AccessResourceFileL( aResource, file, aFsSession );
     if ( ret == KErrNone )
        {
        CleanupClosePushL( file );
        aTarget = new ( ELeave ) CFbsBitmap;
        if ( !aTarget )
            {
            CleanupStack::PopAndDestroy();
            return;
            }
        RFileReadStream readStream( file );
        CleanupClosePushL( readStream );
        aTarget->InternalizeL( readStream );
        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( &file );
        }
    }

// -----------------------------------------------------------------------------
// LoadMifBitmapL
// Loads a bitmap file from the theme server and creates a CFbsBitmap
// -----------------------------------------------------------------------------
//
static void LoadMifBitmapL(
    CXnResource& aResource,
    TInt aBitmapIndex,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider,
    RFs& aFsSession,
    const TRect& aRect )
    {
    RFile file;

    TInt ret = AccessResourceFileL( aResource, file, aFsSession );
    if ( ret == KErrNone )
        {
        aIconProvider = new ( ELeave ) CXnControlAdapterImpl::TIconProvider( file );
        }

    if ( ret == KErrNone )
        {
        // Mask is next after bitmap
        AknIconUtils::CreateIconL( aBitmap, aMask, *aIconProvider,
            aBitmapIndex + KMifIdFirst, aBitmapIndex + KMifIdFirst + 1 );
        AknIconUtils::SetSize( aBitmap, aRect.Size(), EAspectRatioNotPreserved );
        }
    }

// -----------------------------------------------------------------------------
// LoadMbmBitmapL
// Loads a bitmap file from the theme server and creates a CFbsBitmap
// -----------------------------------------------------------------------------
//
static void LoadMbmBitmapL(
    CXnResource& aResource,
    TInt aBitmapIndex,
    CFbsBitmap*& aTarget,
    RFs& aFsSession )
    {
    RFile file;

    TInt ret = AccessResourceFileL( aResource, file, aFsSession );
    if ( ret == KErrNone )
        {
        CleanupClosePushL( file );
        aTarget = new ( ELeave ) CFbsBitmap;
        aTarget->Load( file, aBitmapIndex );
        CleanupStack::PopAndDestroy();
        }
    }

// -----------------------------------------------------------------------------
// LoadSVGBitmapL
// Loads an SVG file from the theme server and creates a CFbsBitmap.
// -----------------------------------------------------------------------------
//
static void LoadSVGBitmapL(
    CXnResource& aResource,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aBitmapMask,
    TRect aRect,
    RFs& aFsSession )
    {
    RFile file;

    TInt ret = AccessResourceFileL( aResource, file, aFsSession );
    if ( ret == KErrNone )
        {
        CleanupClosePushL( file );

        CFbsBitmap* frameBuffer = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( frameBuffer );

        TSize contentSize( aRect.Size() );

        frameBuffer->Create( contentSize, EColor16M );

        TFontSpec fontSpec;
        CSvgEngineInterfaceImpl* svgEngine =
            CSvgEngineInterfaceImpl::NewL( frameBuffer, NULL, fontSpec );

        CleanupStack::PushL( svgEngine );

        svgEngine->Load( file );

        TInt domHandle;
        svgEngine->PrepareDom( file, domHandle );

        CFbsBitmap* target = new ( ELeave ) CFbsBitmap;
        target->Create( contentSize, EColor16M );
        svgEngine->RenderDom( domHandle, target );

        aBitmap = target;

        CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
        mask->Create( contentSize, EGray256 );
        svgEngine->GenerateMask( mask );

        aBitmapMask = mask;

        CleanupStack::PopAndDestroy( 3, &file ); // frameBuffer, svgEngine, file
        }
    }


// -----------------------------------------------------------------------------
// ResolveMifIDL
// Resolves a mif id from a given string.
// -----------------------------------------------------------------------------
//
static TBool ResolveMifIDL(
    const TDesC& aMifId,
    TInt& aMifID )
    {
    HBufC* str = aMifId.AllocL();
    TPtr ptrMif = str->Des();
    ptrMif.TrimAll();
    TInt pos( ptrMif.FindF( KMif ) );
    if ( KErrNotFound != pos )
        {
        pos += KMif().Length();
        TPtr ptr = ptrMif.MidTPtr( pos, ( ptrMif.Length() - pos ) );
        ptr.TrimAll();
        TInt offset( ptr.Locate( KRightParenthesis ) );
        if ( KErrNotFound != offset )
            {
            TLex lex( ptr.Left( offset ) );
            lex.SkipSpace();
            lex.Val( aMifID );
            delete str;
            return ETrue;
            }
        }
    delete str;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// LoadSkinBitmapL
// Loads a skin bitmap and creates a CFbsBitmap from it.
// -----------------------------------------------------------------------------
//
static TBool LoadSkinBitmapL(
    const TDesC& aSkinId,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aBitmapMask,
    TRect aRect )
    {
    TAknsItemID itemID;

    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );

    TInt mifId( 0 );

    TInt mifFound( ResolveMifIDL( aSkinId, mifId ) );

    TInt colorIndex( KErrNotFound );

    if ( skin && CXnUtils::ResolveSkinItemIDL( aSkinId, itemID, colorIndex ) )
        {
        if ( itemID.iMajor == EAknsMajorAppIcon ) // Check if appl. icon request
            {
            TUid applUid;
            applUid = TUid::Uid( itemID.iMinor );

            AknsUtils::CreateAppIconLC(
                skin, applUid, EAknsAppIconTypeList, aBitmap, aBitmapMask );
            AknIconUtils::SetSize(
                aBitmap, aRect.Size(), EAspectRatioNotPreserved );
            CleanupStack::Pop( 2 ); //aBitmap, aBitmapMask

            return ETrue;
            }
        else
            {
            TInt error( KErrNone );

            // if we have a color index, then we create a colored icon
            if ( colorIndex != KErrNotFound )
                {
                TRgb defaultColor( 0, 0, 0 );

                TRAP( error, AknsUtils::CreateColorIconL(
                    skin, itemID, KAknsIIDQsnTextColors,
                    colorIndex, aBitmap, aBitmapMask,
                    KNullDesC, mifId, mifId + 1, defaultColor ) );
                }
            else
                {
                TRAP( error, AknsUtils::CreateIconL(
                    skin, itemID, aBitmap, aBitmapMask, KNullDesC,
                    mifId, mifId + 1 ) );
                }

            if ( error != KErrNone )
                {
                error = KErrNone;
                TRAP( error, AknsUtils::CreateIconL(
                    skin, itemID, aBitmap, KNullDesC, mifId ) );
                }

            if ( error == KErrNone )
                {
                AknIconUtils::SetSize(
                    aBitmap, aRect.Size(), EAspectRatioNotPreserved );

                return ETrue;
                }
            }
        }

    if ( mifFound )
        {
        AknIconUtils::CreateIconL(
            aBitmap, aBitmapMask, KAvkonBitmapFile, mifId, mifId + 1 );
        AknIconUtils::SetSize(
            aBitmap, aRect.Size(), EAspectRatioNotPreserved );

        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// GetBitmap
// Fetches a bitmap
// -----------------------------------------------------------------------------
//
static TBool GetBitmapL(
    CXnUiEngine& aEngine,
    CXnNode& /*aNode*/,
    const TDesC& aResourceFileName,
    TInt& aBitmapIndex,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aBitmapMask,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider,
    TRect aRect,
    RFs& aFsSession )
    {
    if ( LoadSkinBitmapL( aResourceFileName, aBitmap, aBitmapMask, aRect ) )
        {
        return ETrue;
        }

    CXnResource* resource( CXnUtils::FindResource(
        aEngine.Resources(), aResourceFileName, aBitmapIndex ) );

    if ( resource )
        {
        if ( resource->ResourceType() == EResourceSVG )
            {
            LoadSVGBitmapL( *resource, aBitmap, aBitmapMask, aRect, aFsSession );

            return ETrue;
            }
        else if ( resource->ResourceType() == EResourceMIF )
            {
            LoadMifBitmapL( *resource, aBitmapIndex, aBitmap,
                aBitmapMask, aIconProvider, aFsSession, aRect );

            return ETrue;
            }
        else if ( resource->ResourceType() == EResourceMBM )
            {
            LoadMbmBitmapL( *resource, aBitmapIndex, aBitmap, aFsSession );

            return ETrue;
            }
        else if ( resource->ResourceType() == EResourceFBS )
            {
            LoadFbsBitmapL( *resource, aBitmap, aFsSession );

            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// DrawBackgroundColorL
// Draws background color to the padding rect
// -----------------------------------------------------------------------------
//
static void DrawBackgroundColorL(
    const TRect& /*aRect*/,
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    CXnProperty* colorProperty( aNode.BackgroundColorL() );

    if ( colorProperty )
        {
        TRect paddingRect = aNode.PaddingRect();

        CXnDomPropertyValue* value =
            static_cast< CXnDomPropertyValue* >(
                colorProperty->Property()->PropertyValueList().Item( 0 ) );

        if ( value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor )
            {
            TRgb rgb( value->RgbColorValueL() );

            aGc.SetPenColor( rgb );

            aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            aGc.SetBrushColor( rgb );

            aGc.DrawRect( paddingRect );
            }
        }
    }

// -----------------------------------------------------------------------------
// StripQuotes
// Strips enclosing quotes from a string
// -----------------------------------------------------------------------------
//
static void StripQuotes( HBufC*& aString )
    {
    TInt pos1 = aString->Locate( '\"' );
    TInt pos2 = aString->Locate( '\'' );
    // check, if the first character is " or '
    if ( pos1 == 0 || pos2 == 0 )
        {
        TInt len = aString->Length() - 2;
        TPtr ptr = aString->Des();
        TPtrC16 newString( aString->Mid( 1, len ).Ptr(), len );
        ptr = newString;
        }
    }

// -----------------------------------------------------------------------------
// DrawRepeatBackgroundY
// Draws repeated background in the vertical direction
// -----------------------------------------------------------------------------
//
static void DrawRepeatBackgroundY(
    CWindowGc& aGc, TInt aYRepeatCount, CXnNode& aNode, TRect& aRect,
    CFbsBitmap* aBitmap, TRect aBitmapRect,
    TRect aImageRect, TInt& aYOffset )
    {
    for (TInt i = 0; i <= aYRepeatCount && aRect.iTl.iY < aNode.PaddingRect().iBr.iY;)
        {
        TRect newRect;
        newRect = TRect(
            aRect.iTl,
            TPoint(
                aRect.iTl.iX + aImageRect.Width(),
                aRect.iTl.iY + aImageRect.Height() ) );
        aGc.DrawBitmap( newRect, aBitmap, aBitmapRect );
        aRect.Move( 0, newRect.Height() );
        aYOffset += newRect.Height();
        if ( ++i == aYRepeatCount || aYRepeatCount == 0 )
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// DrawSpaceBackgroundY
// Draws spaced background in the vertical direction
// -----------------------------------------------------------------------------
//
static void DrawSpaceBackgroundY(
    CWindowGc& aGc, TInt aYRepeatCount, CXnNode& aNode, TRect& aRect,
    CFbsBitmap* aBitmap, TRect aBitmapRect,
    TRect aImageRect, TInt& aYOffset )
    {
    for (TInt j = 0; j <= aYRepeatCount && aRect.iTl.iY < aNode.PaddingRect().iBr.iY;)
        {
        TRect newRect = aRect;
        newRect = TRect(
            aRect.iTl,
            TPoint(
                aRect.iTl.iX + aImageRect.Width(),
                aRect.iTl.iY + aImageRect.Height() ) );
        aGc.DrawBitmap( newRect, aBitmap, aBitmapRect );
        aRect.Move( 0, newRect.Height() );
        aYOffset += newRect.Height();
        if ( ++j == aYRepeatCount || aYRepeatCount == 0 )
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// LimitRectToPaddingRect
// Limits a rect so that it does not exceed padding rect
// -----------------------------------------------------------------------------
//
static void LimitRectToPaddingRect( CXnNode& aNode, TRect& aRect )
    {
    TRect tmpRect = aNode.PaddingRect();
    if ( aRect.iTl.iX < tmpRect.iTl.iX )
        {
        aRect.SetRect( tmpRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY );
        }
    if ( aRect.iTl.iY < tmpRect.iTl.iY )
        {
        aRect.SetRect( aRect.iTl.iX, tmpRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY );
        }
    if ( aRect.iBr.iX > tmpRect.iBr.iX )
        {
        aRect.SetRect( aRect.iTl.iX, aRect.iTl.iY, tmpRect.iBr.iX, aRect.iBr.iY );
        }
    if ( aRect.iBr.iY > tmpRect.iBr.iY )
        {
        aRect.SetRect( aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, tmpRect.iBr.iY );
        }
    }

// -----------------------------------------------------------------------------
// GetBackroundRepeatValuesL
// Gets background repeat information from node properties
// -----------------------------------------------------------------------------
//
static void GetBackroundRepeatValuesL(
    CXnNode& aNode,
    TBool& aRepeatX,
    TBool& aRepeatY,
    TBool& aSpaceX,
    TBool& aSpaceY )
    {
    CXnProperty* backgroundRepeatProperty = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KBackGroundRepeat );
    if ( backgroundRepeatProperty )
        {
        CXnDomList& propertyValueList =
            backgroundRepeatProperty->Property()->PropertyValueList();
        TInt valueCount = propertyValueList.Length();
        for ( TInt i = 0; i < valueCount; ++i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                propertyValueList.Item( i ) );
            if ( value->PrimitiveValueType() == CXnDomPropertyValue::EString ||
                 value->PrimitiveValueType() == CXnDomPropertyValue::EIdent )
                {
                switch ( i )
                    {
                    case 0:
                        {
                        if ( value->StringValueL() ==
                             XnPropertyNames::appearance::common::backgroundrepeat::KRepeat )
                            {
                            aRepeatX = ETrue;
                            aRepeatY = ETrue;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundrepeat::KSpace )
                            {
                            aSpaceX = ETrue;
                            aSpaceY = ETrue;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundrepeat::KRepeatX )
                            {
                            aRepeatX = ETrue;
                            aRepeatY = EFalse;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundrepeat::KRepeatY )
                            {
                            aRepeatX = EFalse;
                            aRepeatY = ETrue;
                            }
                        break;
                        }
                    case 1:
                        {
                        if ( value->StringValueL() ==
                             XnPropertyNames::appearance::common::backgroundrepeat::KRepeat )
                            {
                            aRepeatY = ETrue;
                            aSpaceY = EFalse;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundrepeat::KSpace )
                            {
                            aRepeatY = EFalse;
                            aSpaceY = ETrue;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundrepeat::KNoRepeat )
                            {
                            aRepeatY = EFalse;
                            aSpaceY = EFalse;
                            }
                        break;
                        }
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// DrawRepeatedBackgroundImage
// Draws a repeated background image
// -----------------------------------------------------------------------------
//
static void DrawRepeatedBackgroundImage(
    CWindowGc& aGc,
    CXnNode& aNode,
    TRect& aRect,
    TRect aImageRect,
    TRect aBitmapRect,
    CFbsBitmap* aBitmap,
    TBool aRepeatX,
    TBool aRepeatY,
    TBool aSpaceX,
    TBool aSpaceY,
    TInt aXRepeatCount,
    TInt aYRepeatCount )
    {
    TRect paddingRect = aNode.PaddingRect();
    if ( aRepeatX && !aRepeatY && !aSpaceX && !aSpaceY )
        {
        aRect.SetRect( TPoint( paddingRect.iTl.iX, aRect.iTl.iY ), aRect.Size() );
        for ( TInt i = 0; i <= aXRepeatCount && aRect.iTl.iX < paddingRect.iBr.iX; )
            {
            TRect newRect = TRect(
                aRect.iTl,
                TPoint(
                    aRect.iTl.iX + aImageRect.Width(),
                    aRect.iTl.iY + aImageRect.Height() ) );
            aGc.DrawBitmap( newRect, aBitmap, aBitmapRect );
            aRect.Move( newRect.Width(), 0 );
            if ( ++i == aXRepeatCount || aXRepeatCount == 0 )
                {
                break;
                }
            }
        }
    else if ( !aRepeatX && !aRepeatY && aSpaceX && !aSpaceY )
        {
        aRect.SetRect( TPoint( paddingRect.iTl.iX, aRect.iTl.iY ), aRect.Size() );
        if ( aImageRect.Width() < paddingRect.Width() && aXRepeatCount > 0 )
            {
            aRect.Move(
                ( paddingRect.Width() - aImageRect.Width() * aXRepeatCount ) / 2,
                0 );
            }
        for ( TInt i = 0; i <= aXRepeatCount && aRect.iTl.iX < paddingRect.iBr.iX; )
            {
            TRect newRect = TRect(
                aRect.iTl,
                TPoint(
                    aRect.iTl.iX + aImageRect.Width(),
                    aRect.iTl.iY + aImageRect.Height() ) );
            aGc.DrawBitmap( newRect, aBitmap, aBitmapRect );
            aRect.Move( newRect.Width(), 0 );
            if ( ++i == aXRepeatCount || aXRepeatCount == 0 )
                {
                break;
                }
            }
        }
    else if ( !aRepeatX && aRepeatY && !aSpaceX && !aSpaceY )
        {
        aRect.SetRect( TPoint( aRect.iTl.iX, paddingRect.iTl.iY ), aRect.Size() );
        TInt yOffset = 0;
        DrawRepeatBackgroundY(
            aGc, aYRepeatCount, aNode, aRect,
            aBitmap, aBitmapRect,
            aImageRect, yOffset );
        }
    else if ( !aRepeatX && !aRepeatY && !aSpaceX && aSpaceY )
        {
        aRect.SetRect( TPoint( aRect.iTl.iX, paddingRect.iTl.iY ), aRect.Size() );
        if ( aImageRect.Height() < paddingRect.Height() && aYRepeatCount > 0 )
            {
            aRect.Move(
                0,
                ( paddingRect.Height() - aImageRect.Height() * aYRepeatCount ) / 2 );
            }
        TInt yOffset = 0;
        DrawSpaceBackgroundY(
            aGc, aYRepeatCount, aNode, aRect,
            aBitmap, aBitmapRect,
            aImageRect, yOffset );
        }
    else if ( aRepeatX && aRepeatY && !aSpaceX && !aSpaceY )
        {
        aRect.SetRect( paddingRect.iTl, aRect.Size() );
        TInt yOffset = 0;
        for ( TInt i = 0; i <= aXRepeatCount && aRect.iTl.iX < paddingRect.iBr.iX; )
            {
            aRect.Move( 0, -yOffset );
            yOffset = 0;
            TRect newRect = TRect(
                aRect.iTl,
                TPoint(
                    aRect.iTl.iX + aImageRect.Width(),
                    aRect.iTl.iY + aImageRect.Height() ) );
            aRect = newRect;
            DrawRepeatBackgroundY(
                aGc, aYRepeatCount, aNode, aRect,
                aBitmap, aBitmapRect,
                aImageRect, yOffset );
            aRect.Move( newRect.Width(), 0 );
            if ( ++i == aXRepeatCount || aXRepeatCount == 0 )
                {
                break;
                }
            }
        }
    else if ( !aRepeatX && !aRepeatY && aSpaceX && aSpaceY )
        {
        aRect.SetRect( paddingRect.iTl, aRect.Size() );
        if ( aImageRect.Width() < paddingRect.Width() && aXRepeatCount > 0 )
            {
            aRect.Move(
                ( paddingRect.Width() - aImageRect.Width() * aXRepeatCount ) / 2,
                0 );
            }
        if ( aImageRect.Height() < paddingRect.Height() && aYRepeatCount > 0 )
            {
            aRect.Move(
                0,
                ( paddingRect.Height() - aImageRect.Height() * aYRepeatCount ) / 2 );
            }
        TInt yOffset = 0;
        for ( TInt i = 0; i <= aXRepeatCount && aRect.iTl.iX < paddingRect.iBr.iX; )
            {
            aRect.Move( 0, -yOffset );
            yOffset = 0;
            TRect newRect = TRect(
                aRect.iTl,
                TPoint(
                    aRect.iTl.iX + aImageRect.Width(),
                    aRect.iTl.iY + aImageRect.Height() ) );
            aRect = newRect;
            DrawSpaceBackgroundY(
                aGc, aYRepeatCount, aNode, aRect,
                aBitmap, aBitmapRect,
                aImageRect, yOffset );
            aRect.Move( newRect.Width(), 0 );
            if ( ++i == aXRepeatCount || aXRepeatCount == 0 )
                {
                break;
                }
            }
        }
    else if ( aRepeatX && !aRepeatY && !aSpaceX && aSpaceY )
        {
        aRect.SetRect( paddingRect.iTl, aRect.Size() );
        if ( aImageRect.Height() < paddingRect.Height() && aYRepeatCount > 0 )
            {
            aRect.Move(
                0,
                ( paddingRect.Height() - aImageRect.Height() * aYRepeatCount ) / 2 );
            }
        TInt yOffset = 0;
        for ( TInt i = 0; i <= aXRepeatCount && aRect.iTl.iX < paddingRect.iBr.iX; )
            {
            aRect.Move( 0, -yOffset );
            yOffset = 0;
            TRect newRect = TRect(
                aRect.iTl,
                TPoint(
                    aRect.iTl.iX + aImageRect.Width(),
                    aRect.iTl.iY + aImageRect.Height() ) );
            aRect = newRect;
            DrawSpaceBackgroundY(
                aGc, aYRepeatCount, aNode, aRect,
                aBitmap, aBitmapRect,
                aImageRect, yOffset );
            aRect.Move( newRect.Width(), 0 );
            if ( ++i == aXRepeatCount || aXRepeatCount == 0 )
                {
                break;
                }
            }
        }
    else if ( !aRepeatX && aRepeatY && aSpaceX && !aSpaceY )
        {
        aRect.SetRect( paddingRect.iTl, aRect.Size() );
        TInt yOffset = 0;
        if ( aImageRect.Width() < paddingRect.Width() && aXRepeatCount > 0 )
            {
            aRect.Move(
                ( paddingRect.Width() - aImageRect.Width() * aXRepeatCount ) / 2,
                -yOffset );
            }
        for ( TInt i = 0; i <= aXRepeatCount && aRect.iTl.iX < paddingRect.iBr.iX; )
            {
            aRect.Move( 0, -yOffset );
            yOffset = 0;
            TRect newRect = TRect(
                aRect.iTl,
                TPoint(
                    aRect.iTl.iX + aImageRect.Width(),
                    aRect.iTl.iY + aImageRect.Height() ) );
            aRect = newRect;
            DrawRepeatBackgroundY(
                aGc, aYRepeatCount, aNode, aRect,
                aBitmap, aBitmapRect,
                aImageRect, yOffset );
            aRect.Move( newRect.Width(), 0 );
            if ( ++i == aXRepeatCount || aXRepeatCount == 0 )
                {
                break;
                }
            }
       }
    }

// -----------------------------------------------------------------------------
// GetBackgroundPositionFromPropertyL
// Gets background position information from node properties
// -----------------------------------------------------------------------------
//
static TBool GetBackgroundPositionFromPropertyL(
    CXnNode& aNode,
    TBool aScaleImage,
    TRect& aRect,
    TRect aImageRect,
    CFbsBitmap* aBitmap,
    TBool& aHeightSet )
    {
    CXnProperty* backgroundPositionProperty = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KBackGroundPosition );
    TBool centerHorizontal = ETrue;
    if ( backgroundPositionProperty )
        {
        CXnDomList& propertyValueList =
            backgroundPositionProperty->Property()->PropertyValueList();
        TSize size;
        for ( TInt i = propertyValueList.Length() - 1; i >= 0; --i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                propertyValueList.Item( i ) );
            if ( value->PrimitiveValueType() == CXnDomPropertyValue::EString ||
                 value->PrimitiveValueType() == CXnDomPropertyValue::EIdent )
                {
                if ( aScaleImage )
                    {
                    size = aImageRect.Size();
                    }
                else
                    {
                    size = aBitmap->SizeInPixels();
                    }
                switch ( i )
                    {
                    case 0:
                        {
                        // StringValueL will not leave, we check the type first
                        if ( value->StringValueL() ==
                             XnPropertyNames::appearance::common::backgroundposition::KRight )
                            {
                            centerHorizontal = EFalse;
                            aRect.Move( aRect.Width(), 0 );
                            aRect.Move( -size.iWidth, 0 );
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundposition::KLeft )
                            {
                            centerHorizontal = EFalse;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundposition::KBottom )
                            {
                            aHeightSet = ETrue;
                            aRect.Move( 0, aRect.Height() );
                            aRect.Move( 0, -size.iHeight );
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundposition::KTop )
                            {
                            aHeightSet = ETrue;
                            }
                        break;
                        }
                    case 1:
                        {
                        // StringValueL will not leave, we check the type first
                        if ( value->StringValueL() ==
                             XnPropertyNames::appearance::common::backgroundposition::KRight )
                            {
                            aRect.Move( aRect.Width(), 0 );
                            aRect.Move( -size.iWidth, 0 );
                            centerHorizontal = EFalse;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundposition::KLeft )
                            {
                            centerHorizontal = EFalse;
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundposition::KBottom )
                            {
                            aHeightSet = ETrue;
                            aRect.Move( 0, aRect.Height() );
                            aRect.Move( 0, -size.iHeight );
                            }
                        else if ( value->StringValueL() ==
                                  XnPropertyNames::appearance::common::backgroundposition::KTop )
                            {
                            aHeightSet = ETrue;
                            }
                        break;
                        }
                    }
                }
           else if ( value->PrimitiveValueType() == CXnDomPropertyValue::EPercentage )
                {
                // FloatValueL will not leave, we check the type first
                TReal percentage = value->FloatValueL();
                if ( percentage > 100 )
                    {
                    percentage = 100;
                    }
                TSize size = aImageRect.Size();
                switch ( i )
                    {
                    case 0:
                        {
                        TInt rectOffset = ( percentage / 100.0 ) * aRect.Width();
                        TInt bitmapOffset = ( percentage / 100.0 ) * size.iWidth;
                        aRect.Move( rectOffset, 0 );
                        aRect.Move( -bitmapOffset, 0 );
                        centerHorizontal = EFalse;
                        break;
                        }
                    case 1:
                        {
                        aHeightSet = ETrue;
                        TInt rectOffset = ( percentage / 100.0 ) * aRect.Height();
                        TInt bitmapOffset = ( percentage / 100.0 ) * size.iHeight;
                        aRect.Move( 0, rectOffset );
                        aRect.Move( 0, -bitmapOffset );
                        break;
                        }
                    }
                }
            else
                {
                CXnDomPropertyValue* tmpValue = NULL;
                TRAPD( error, tmpValue = value->CloneL(); );
                if ( error != KErrNone )
                    {
                    return EFalse;
                    }
                CXnProperty* tmpProperty = NULL;
                TRAP( error, tmpProperty = CXnProperty::NewL(
                    KNullDesC8,
                    tmpValue,
                    *aNode.UiEngine()->ODT()->DomDocument().StringPool() ); );
                if ( error != KErrNone )
                    {
                    delete tmpValue;
                    return EFalse;
                    }
                switch ( i )
                    {
                    case 0:
                        {
                        TInt intValue = 0;
                        TRAP( error,
                            intValue = aNode.UiEngine()->HorizontalPixelValueL(
                                tmpProperty, 0 ); );
                        if ( error != KErrNone )
                            {
                            delete tmpProperty;
                            return EFalse;
                            }
                        aRect.Move( intValue, 0 );
                        centerHorizontal = EFalse;
                        break;
                        }
                    case 1:
                        {
                        aHeightSet = ETrue;
                        TInt intValue = 0;
                        TRAP( error,
                            intValue = aNode.UiEngine()->VerticalPixelValueL(
                                tmpProperty, 0 ); );
                        if ( error != KErrNone )
                            {
                            delete tmpProperty;
                            return EFalse;
                            }
                        aRect.Move( 0, intValue );
                        break;
                        }
                    }
                delete tmpProperty;
                }
            }
        if ( centerHorizontal )
            {
            aRect.Move( aRect.Width() / 2, 0 );
            aRect.Move( -size.iWidth / 2, 0 );
            }
        }
    else
        {
        aHeightSet = ETrue;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// GetBackgroundSizeFromPropertyL
// Gets background size information from node properties
// -----------------------------------------------------------------------------
//
static TBool GetBackgroundSizeFromPropertyL(
    TRect& aRect,
    CXnNode& aNode,
    CFbsBitmap* aBitmap,
    TBool& aScaleImage )
    {
    CXnProperty* backgroundSizeProperty = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KBackGroundSize );
    TBool widthAuto = ETrue;
    TBool heightAuto = ETrue;
    TBool heightSet = EFalse;
    TSize size = aBitmap->SizeInPixels();
    if ( backgroundSizeProperty )
        {
        widthAuto = EFalse;
        heightAuto = EFalse;
        CXnDomList& propertyValueList =
            backgroundSizeProperty->Property()->PropertyValueList();
        for ( TInt i = propertyValueList.Length() - 1; i >= 0; --i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                propertyValueList.Item( i ) );
            if ( value->IsAutoIdent() )
                {
                switch ( i )
                    {
                    case 0:
                        {
                        widthAuto = ETrue;
                        break;
                        }
                    case 1:
                        {
                        heightAuto = ETrue;
                        break;
                        }
                    }
                }
            }
        for ( TInt i = propertyValueList.Length() - 1; i >= 0; --i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                propertyValueList.Item( i ) );
            if ( value->IsAutoIdent() )
                {
                continue;
                }
            else if ( value->PrimitiveValueType() == CXnDomPropertyValue::EPercentage )
                {
                // FloatValueL will not leave, we check the type first
                TReal percentage = 100;
                TRAP_IGNORE( percentage = value->FloatValueL() );
                if ( percentage > 100 )
                    {
                    percentage = 100;
                    }
                switch ( i )
                    {
                    case 0:
                        {
                        TInt widthOffset = ( 1 - ( percentage / 100.0 ) ) * aRect.Width();
                        if ( heightAuto )
                            {
                            TReal aspectRatio =
                                static_cast< TReal >( size.iHeight ) /
                                static_cast< TReal >( size.iWidth );
                            aRect.Resize( -widthOffset, 0 );
                            aRect.SetHeight(
                                static_cast< TInt >(
                                    static_cast< TReal >(
                                        aRect.Width() ) * aspectRatio + 0.5 ) );
                            aScaleImage = ETrue;
                            heightSet = ETrue;
                            }
                        else
                            {
                            aRect.Resize( -widthOffset, 0 );
                            aScaleImage = ETrue;
                            }
                        if ( !heightSet )
                            {
                            TInt heightOffset = ( 1 - ( percentage / 100.0 ) ) * aRect.Height();
                            aRect.Resize( 0, -heightOffset );
                            }
                        break;
                        }
                    case 1:
                        {
                        TInt heightOffset = ( 1 - ( percentage / 100.0 ) ) * aRect.Height();
                        if ( widthAuto )
                            {
                            TReal aspectRatio =
                                static_cast< TReal >( size.iHeight ) /
                                static_cast< TReal >( size.iWidth );
                            aRect.Resize( 0, -heightOffset );
                            aRect.SetWidth(
                                static_cast< TInt >(
                                    static_cast< TReal >(
                                        aRect.Height() ) / aspectRatio + 0.5 ) );
                            aScaleImage = ETrue;
                            heightSet = ETrue;
                            }
                        else
                            {
                            aRect.Resize( 0, -heightOffset );
                            aScaleImage = ETrue;
                            heightSet = ETrue;
                            }
                        break;
                        }
                    }
                }
            else
                {
                CXnDomPropertyValue* tmpValue = NULL;
                TRAPD( error, tmpValue = value->CloneL(); );
                if ( error != KErrNone )
                    {
                    return EFalse;
                    }
                CXnProperty* tmpProperty = NULL;
                TRAP( error, tmpProperty = CXnProperty::NewL(
                    KNullDesC8, tmpValue,
                    *aNode.UiEngine()->ODT()->DomDocument().StringPool() ); );
                if ( error != KErrNone )
                    {
                    delete tmpValue;
                    return EFalse;
                    }
                switch ( i )
                    {
                    case 0:
                        {
                        TInt intValue = 0;
                        TRAP( error,
                            intValue = aNode.UiEngine()->HorizontalPixelValueL(
                                tmpProperty, 0 ); );
                        TInt widthOffset = aRect.Width() - intValue;
                        if ( heightAuto )
                            {
                            TReal aspectRatio =
                                static_cast< TReal >( size.iHeight ) /
                                static_cast< TReal >( size.iWidth );
                            aRect.Resize( -widthOffset, 0 );
                            aRect.SetHeight(
                                static_cast< TInt >(
                                    static_cast< TReal >(
                                        aRect.Width() ) * aspectRatio + 0.5 ) );
                            aScaleImage = ETrue;
                            heightSet = ETrue;
                            }
                        else
                            {
                            aRect.Resize( -widthOffset, 0 );
                            aScaleImage = ETrue;
                            }
                        if ( !heightSet )
                            {
                            TInt heightOffset = aRect.Height() - intValue;
                            aRect.Resize( 0, -heightOffset );
                            }
                        break;
                        }
                    case 1:
                        {
                        TInt intValue = 0;
                        TRAP( error,
                            intValue = aNode.UiEngine()->VerticalPixelValueL(
                                tmpProperty, 0 ); );
                        TInt heightOffset = aRect.Height() - intValue;
                        if ( widthAuto )
                            {
                            TReal aspectRatio =
                                static_cast< TReal >( size.iHeight ) /
                                static_cast< TReal >( size.iWidth );
                            aRect.Resize( 0, -heightOffset );
                            aRect.SetWidth(
                                static_cast< TInt >(
                                    static_cast< TReal >(
                                        aRect.Height() ) / aspectRatio + 0.5 ) );
                            aScaleImage = ETrue;
                            heightSet = ETrue;
                            }
                        else
                            {
                            aRect.Resize( 0, -heightOffset );
                            aScaleImage = ETrue;
                            heightSet = ETrue;
                            }
                        break;
                        }
                    }
                delete tmpProperty;
                }
            }
        }
    if ( widthAuto && heightAuto )
        {
        if ( size.iHeight < aRect.Height() )
            {
            aRect.SetHeight( size.iHeight );
            }
        if ( size.iWidth < aRect.Width() )
            {
            aRect.SetWidth( size.iWidth );
            }
        aScaleImage = EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// LighterColor
// Gets a color that has more HSL lightness (used for border effects)
// -----------------------------------------------------------------------------
//
static TRgb LighterColor( const TRgb& aRgb )
    {
    TUint hue;
    TUint saturation;
    TUint lightness;
    ConvertRgbToHsl( aRgb, hue, saturation, lightness );
    if ( ( static_cast< TInt >( lightness ) + 10 ) <= 100 )
        {
        lightness += 10;
        }
    else
        {
        lightness = 100;
        }
    return ConvertHslToRgb( hue, saturation, lightness );
    }

// -----------------------------------------------------------------------------
// DarkerColor
// Gets a color that has less HSL lightness (used for border effects)
// -----------------------------------------------------------------------------
//
static TRgb DarkerColor( const TRgb& aRgb )
    {
    TUint hue;
    TUint saturation;
    TUint lightness;
    ConvertRgbToHsl( aRgb, hue, saturation, lightness );
    if ( ( static_cast< TInt >( lightness ) - 10 ) >= 0 )
        {
        lightness -= 10;
        }
    else
        {
        lightness = 0;
        }
    return ConvertHslToRgb( hue, saturation, lightness );
    }

// -----------------------------------------------------------------------------
// DrawSolidTopBorder
// Draw a top border with solid style
// -----------------------------------------------------------------------------
//
static void DrawSolidTopBorder( CArrayFix< TPoint >* aArray, CWindowGc& aGc )
    {
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// SplitTopBorderPolygonL
// Split top border to two parts for border effect drawing
// -----------------------------------------------------------------------------
//
static void SplitTopBorderPolygonL(
    CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aUpperPart,
    CArrayFix< TPoint >*& aLowerPart )
    {
    TPoint leftHalf =
        TPoint(
            ( *aArray )[0].iX + ( ( *aArray )[3].iX - ( *aArray )[0].iX ) / 2,
            ( *aArray )[0].iY + ( ( *aArray )[3].iY - ( *aArray )[0].iY ) / 2);
    TPoint rightHalf =
        TPoint(
            ( *aArray )[2].iX + ( ( *aArray )[1].iX - ( *aArray )[2].iX ) / 2,
            ( *aArray )[1].iY + ( ( *aArray )[2].iY - ( *aArray )[1].iY ) / 2);
    aUpperPart->AppendL( ( *aArray )[0] );
    aUpperPart->AppendL( ( *aArray )[1] );
    aUpperPart->AppendL( rightHalf );
    aUpperPart->AppendL( leftHalf );
    aLowerPart->AppendL( leftHalf );
    aLowerPart->AppendL( rightHalf );
    aLowerPart->AppendL( ( *aArray )[2] );
    aLowerPart->AppendL( ( *aArray )[3] );
    }

// -----------------------------------------------------------------------------
// DrawRidgeTopBorderL
// Draw top border with ridge style
// -----------------------------------------------------------------------------
//
static void DrawRidgeTopBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc, TRgb& aRgb )
    {
    CArrayFix< TPoint >* upperPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( upperPart );
    CArrayFix< TPoint >* lowerPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( lowerPart );
    SplitTopBorderPolygonL( aArray, upperPart, lowerPart );
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( upperPart );
    rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( lowerPart );
    CleanupStack::PopAndDestroy( lowerPart );
    CleanupStack::PopAndDestroy( upperPart );
    }

// -----------------------------------------------------------------------------
// DrawGrooveTopBorderL
// Draw top border with groove style
// -----------------------------------------------------------------------------
//
static void DrawGrooveTopBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc, TRgb& aRgb )
    {
    CArrayFix< TPoint >* upperPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( upperPart );
    CArrayFix< TPoint >* lowerPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( lowerPart );
    SplitTopBorderPolygonL( aArray, upperPart, lowerPart );
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( upperPart );
    rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( lowerPart );
    CleanupStack::PopAndDestroy( lowerPart );
    CleanupStack::PopAndDestroy( upperPart );
    }

// -----------------------------------------------------------------------------
// DrawInsetTopBorder
// Draw top border with inset style
// -----------------------------------------------------------------------------
//
static void DrawInsetTopBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc, TRgb& aRgb )
    {
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawOutsetTopBorder
// Draw top border with outset style
// -----------------------------------------------------------------------------
//
static void DrawOutsetTopBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc, TRgb& aRgb )
    {
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// GetBorderColorL
// Get border color from properties
// -----------------------------------------------------------------------------
//
static TBool GetBorderColorL(
    const TDesC8& aPropertyName,
    CXnNode& aNode,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CXnProperty* colorProperty = aNode.GetPropertyL( aPropertyName );
    CXnProperty* currentColorProperty = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KColor );
    CXnProperty* borderColorProperty = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KBorderColor );
    if ( colorProperty )
        {
        CXnDomProperty* domProperty = colorProperty->Property();
        CXnDomPropertyValue* propertyValue = NULL;
        propertyValue = static_cast< CXnDomPropertyValue* >(
            domProperty->PropertyValueList().Item( 0 ) );
        if ( propertyValue->IsAutoIdent() )
            {
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            TInt error = AknsUtils::GetCachedColor(
                skinInstance,
                aRgb,
                KAknsIIDQsnLineColors,
                EAknsCIQsnLineColorsCG4 );
            if ( error != KErrNone )
                {
                return EFalse;
                }
            }
        else
            {
            TRAPD( error, aRgb = propertyValue->RgbColorValueL(); );
            if ( error != KErrNone )
                {
                return EFalse;
                }
            }
        aGc.SetPenColor( aRgb );
        aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        aGc.SetBrushColor( aRgb );
        }
    else if ( borderColorProperty )
        {
        CXnDomProperty* domProperty = borderColorProperty->Property();
        CXnDomPropertyValue* propertyValue = NULL;
        propertyValue = static_cast< CXnDomPropertyValue* >(
            domProperty->PropertyValueList().Item( 0 ) );
        if ( propertyValue->IsAutoIdent() )
            {
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            TInt error = AknsUtils::GetCachedColor(
                skinInstance,
                aRgb,
                KAknsIIDQsnLineColors,
                EAknsCIQsnLineColorsCG4 );
            if ( error != KErrNone )
                {
                return EFalse;
                }
            }
        else
            {
            TRAPD( error, aRgb = propertyValue->RgbColorValueL(); );
            if ( error != KErrNone )
                {
                return EFalse;
                }
            }
        aGc.SetPenColor( aRgb );
        aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        aGc.SetBrushColor( aRgb );
        }
    else if ( currentColorProperty )
        {
        CXnDomProperty* domProperty = currentColorProperty->Property();
        CXnDomPropertyValue* propertyValue = NULL;
        propertyValue = static_cast< CXnDomPropertyValue* >(
            domProperty->PropertyValueList().Item( 0 ) );
        if ( propertyValue->IsAutoIdent() )
            {
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            TInt error = AknsUtils::GetCachedColor(
                skinInstance,
                aRgb,
                KAknsIIDQsnLineColors,
                EAknsCIQsnLineColorsCG4 );
            if ( error != KErrNone )
                {
                return EFalse;
                }
            }
        else
            {
            TRAPD( error, aRgb = propertyValue->RgbColorValueL(); );
            if ( error != KErrNone )
                {
                return EFalse;
                }
            }
        aGc.SetPenColor( aRgb );
        aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        aGc.SetBrushColor( aRgb );
        }
    else
        {
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// DrawTopBorderL
// Draw top border
// -----------------------------------------------------------------------------
//
static void DrawTopBorderL(
    const TRect& /*aRect*/,
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    TRect borderRect = aNode.BorderRect();
    TRect paddingRect = aNode.PaddingRect();
    TRgb rgb;
    if ( !GetBorderColorL( XnPropertyNames::appearance::common::KBorderTopColor, aNode, aGc, rgb ) )
        {
        return;
        }
    TPoint tl = borderRect.iTl;
    TPoint bl = paddingRect.iTl;
    bl.iY -= 1;
    bl.iX -= 1;
    TPoint tr = borderRect.iTl;
    tr.iX += borderRect.Width()-1;
    TPoint br = paddingRect.iTl;
    br.iX += paddingRect.Width();
    br.iY -= 1;
    CArrayFix< TPoint >* array = NULL;
    array = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    TRAPD( error,
        array->AppendL( tl );
        array->AppendL( tr );
        array->AppendL( br );
        array->AppendL( bl ) );
    if ( error != KErrNone )
        {
        delete array;
        return;
        }
    CXnProperty* borderTopStyle = aNode.BorderTopStyleL();
    CXnProperty* borderStyle = aNode.BorderStyleL();
    const TDesC8& borderStyleString = ( borderTopStyle ) ?
        borderTopStyle->StringValue() :
        ( borderStyle ) ? borderStyle->StringValue() : KNullDesC8;
    if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KSolid )
        {
        DrawSolidTopBorder( array, aGc );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KInset )
        {
        DrawInsetTopBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KOutset )
        {
        DrawOutsetTopBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KRidge )
        {
        TRAP_IGNORE( DrawRidgeTopBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KGroove )
        {
        TRAP_IGNORE( DrawGrooveTopBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KDotted )
        {
        TPoint end( borderRect.iBr.iX, paddingRect.iTl.iY );
        TRect rect( borderRect.iTl, end );
        DrawDottedBorder( rect, aGc, rgb, ETrue );
        }
    delete array;
    }

// -----------------------------------------------------------------------------
// DrawSolidBottomBorder
// Draw bottom border with solid style
// -----------------------------------------------------------------------------
//
static void DrawSolidBottomBorder( CArrayFix< TPoint >* aArray, CWindowGc& aGc )
    {
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// SplitBottomBorderPolygonL
// Split bottom border to two parts for border effect drawing
// -----------------------------------------------------------------------------
//
static void SplitBottomBorderPolygonL(
    CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aUpperPart,
    CArrayFix< TPoint >*& aLowerPart )
    {
    TPoint leftHalf =
        TPoint(
            ( *aArray )[3].iX +  ( ( *aArray )[0].iX - ( *aArray )[3].iX ) / 2,
            ( *aArray )[3].iY + ( ( *aArray )[0].iY - ( *aArray )[3].iY ) / 2);
    TPoint rightHalf =
        TPoint(
            ( *aArray )[1].iX + ( ( *aArray )[2].iX - ( *aArray )[1].iX ) / 2,
            ( *aArray )[2].iY + ( ( *aArray )[1].iY - ( *aArray )[2].iY ) / 2);
    aUpperPart->AppendL( ( *aArray )[0] );
    aUpperPart->AppendL( ( *aArray )[1] );
    aUpperPart->AppendL( rightHalf );
    aUpperPart->AppendL( leftHalf );
    aLowerPart->AppendL( leftHalf );
    aLowerPart->AppendL( rightHalf );
    aLowerPart->AppendL( ( *aArray )[2] );
    aLowerPart->AppendL( ( *aArray )[3] );
    }

// -----------------------------------------------------------------------------
// DrawRidgeBottomBorderL
// Draw bottom border with ridge style
// -----------------------------------------------------------------------------
//
static void DrawRidgeBottomBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CArrayFix< TPoint >* upperPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( upperPart );
    CArrayFix< TPoint >* lowerPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( lowerPart );
    SplitBottomBorderPolygonL( aArray, upperPart, lowerPart );
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( upperPart );
    rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( lowerPart );
    CleanupStack::PopAndDestroy( lowerPart );
    CleanupStack::PopAndDestroy( upperPart );
    }

// -----------------------------------------------------------------------------
// DrawGrooveBottomBorderL
// Draw bottom border with groove style
// -----------------------------------------------------------------------------
//
static void DrawGrooveBottomBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CArrayFix< TPoint >* upperPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( upperPart );
    CArrayFix< TPoint >* lowerPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( lowerPart );
    SplitBottomBorderPolygonL( aArray, upperPart, lowerPart );
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( upperPart );
    rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( lowerPart );
    CleanupStack::PopAndDestroy( lowerPart );
    CleanupStack::PopAndDestroy( upperPart );
    }

// -----------------------------------------------------------------------------
// DrawInsetBottomBorder
// Draw bottom border with inset style
// -----------------------------------------------------------------------------
//
static void DrawInsetBottomBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawOutsetBottomBorder
// Draw bottom border with outset style
// -----------------------------------------------------------------------------
//
static void DrawOutsetBottomBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawDottedBorder
// Draw dotted border
// -----------------------------------------------------------------------------
//
static void DrawDottedBorder(
    TRect aBorderRect,
    CWindowGc& aGc,
    TRgb& aRgb,
    TBool aHorizontal )
    {
    TInt height( aBorderRect.Height() );
    TInt width( aBorderRect.Width() );

    TInt squareSide( Min( height, width ) );

    if ( !squareSide )
        {
        return;
        }

    // Square size 1, 2 and 3 can be drawn with pen patterns
    if ( squareSide < 4 )
        {
        aGc.SetBrushStyle( CGraphicsContext::ENullBrush );

        aGc.SetPenColor( aRgb );

        for ( TInt i = 0; i < squareSide; i++ )
            {
            TPoint start;
            TPoint end;

            if ( aHorizontal )
                {
                start = TPoint( aBorderRect.iTl.iX, aBorderRect.iTl.iY + i );
                end = TPoint(
                    aBorderRect.iTl.iX + aBorderRect.Width(),
                    aBorderRect.iTl.iY + i );
                }
            else
                {
                start = TPoint( aBorderRect.iTl.iX + i, aBorderRect.iTl.iY );
                end = TPoint(
                    aBorderRect.iTl.iX + i,
                    aBorderRect.iTl.iY + aBorderRect.Height() );
                }

            if ( squareSide == 3 )
                {
                // dashed pen pattern 111000...
                aGc.SetPenStyle( CGraphicsContext::EDashedPen );
                aGc.DrawLine( start, end );
                }
            else
                {
                // dotted pen pattern 1000...
                aGc.SetPenStyle( CGraphicsContext::EDottedPen );
                aGc.DrawLine( start, end );

                if ( aHorizontal )
                    {
                    start.iX += ( squareSide == 1 ) ? 2 : 1;
                    }
                else
                    {
                    start.iY += ( squareSide == 1 ) ? 2 : 1;
                    }

                aGc.SetPenStyle( CGraphicsContext::EDottedPen );
                aGc.DrawLine( start, end );
                }
            }
        }
    else
        {
        aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        aGc.SetPenStyle( CGraphicsContext::ENullPen );

        aGc.SetBrushColor( aRgb );

        TInt divideCount( 0 );

        if ( aHorizontal )
            {
            divideCount = width / squareSide;
            }
        else
            {
            divideCount = height / squareSide;
            }

        TRect drawingRect( aBorderRect.iTl, TSize( squareSide, squareSide ) );

        // every other is drawn
        TInt count( divideCount / 2 );

        for ( TInt i = 0; i <= count; i++ )
            {
            aGc.DrawRect( drawingRect );

            if ( aHorizontal )
                {
                drawingRect.Move( squareSide * 2, 0 );
                }
            else
                {
                drawingRect.Move( 0, squareSide * 2 );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// DrawBottomBorder
// Draw bottom border
// -----------------------------------------------------------------------------
//
static void DrawBottomBorderL(
    const TRect& /*aRect*/,
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    TRect borderRect = aNode.BorderRect();
    TRect paddingRect = aNode.PaddingRect();
    TRgb rgb;
    if ( !GetBorderColorL( XnPropertyNames::appearance::common::KBorderBottomColor, aNode, aGc, rgb ) )
        {
        return;
        }
    TPoint tl = paddingRect.iBr;
    tl.iX -= paddingRect.Width() + 1;
    TPoint bl = borderRect.iBr;
    bl.iX -= borderRect.Width();
    bl.iY-= 1;
    TPoint tr = paddingRect.iBr;
    TPoint br = borderRect.iBr;
    br.iY -= 1;
    br.iX -= 1;
    CArrayFix< TPoint >* array = NULL;
    array = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    TRAPD( error,
        array->AppendL( tl );
        array->AppendL( tr );
        array->AppendL( br );
        array->AppendL( bl ); );
    if ( error != KErrNone )
        {
        delete array;
        return;
        }
    CXnProperty* borderBottomStyle = aNode.BorderBottomStyleL();
    CXnProperty* borderStyle = aNode.BorderStyleL();
    const TDesC8& borderStyleString = ( borderBottomStyle ) ?
        borderBottomStyle->StringValue() :
        ( borderStyle ) ? borderStyle->StringValue() : KNullDesC8;
    if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KSolid )
        {
        DrawSolidBottomBorder( array, aGc );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KInset )
        {
        DrawInsetBottomBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KOutset )
        {
        DrawOutsetBottomBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KRidge )
        {
        TRAP_IGNORE( DrawRidgeBottomBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KGroove )
        {
        TRAP_IGNORE( DrawGrooveBottomBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KDotted )
        {
        TPoint origo( borderRect.iTl.iX, paddingRect.iBr.iY );
        TRect rect( origo, borderRect.iBr );
        DrawDottedBorder( rect, aGc, rgb, ETrue );
        }
    delete array;
    }

// -----------------------------------------------------------------------------
// DrawSolidLeftBorder
// -----------------------------------------------------------------------------
//
static void DrawSolidLeftBorder( CArrayFix< TPoint >* aArray, CWindowGc& aGc )
    {
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// SplitLeftBorderPolygonL
// -----------------------------------------------------------------------------
//
static void SplitLeftBorderPolygonL(
    CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aLeftPart,
    CArrayFix< TPoint >*& aRightPart )
    {
    TPoint upHalf =
        TPoint(
            ( *aArray )[0].iX + ( ( *aArray )[1].iX - ( *aArray )[0].iX ) / 2,
            ( *aArray )[0].iY + ( ( *aArray )[1].iY - ( *aArray )[0].iY ) / 2);
    TPoint downHalf =
        TPoint(
            ( *aArray )[3].iX + ( ( *aArray )[2].iX - ( *aArray )[3].iX ) / 2,
            ( *aArray )[2].iY + ( ( *aArray )[3].iY - ( *aArray )[2].iY ) / 2);
    aLeftPart->AppendL( ( *aArray )[0] );
    aLeftPart->AppendL( upHalf );
    aLeftPart->AppendL( downHalf );
    aLeftPart->AppendL( ( *aArray )[3] );

    aRightPart->AppendL( upHalf );
    aRightPart->AppendL( ( *aArray )[1] );
    aRightPart->AppendL( ( *aArray )[2]) ;
    aRightPart->AppendL( downHalf );
    }

// -----------------------------------------------------------------------------
// DrawRidgeLeftBorderL
// -----------------------------------------------------------------------------
//
static void DrawRidgeLeftBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CArrayFix< TPoint >* leftPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( leftPart );
    CArrayFix< TPoint >* rightPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( rightPart );
    SplitLeftBorderPolygonL( aArray, leftPart, rightPart );
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( leftPart );
    rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( rightPart );
    CleanupStack::PopAndDestroy( rightPart );
    CleanupStack::PopAndDestroy( leftPart );
    }

// -----------------------------------------------------------------------------
// DrawGrooveLeftBorderL
// -----------------------------------------------------------------------------
//
static void DrawGrooveLeftBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CArrayFix< TPoint >* leftPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( leftPart );
    CArrayFix< TPoint >* rightPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( rightPart );
    SplitLeftBorderPolygonL( aArray, leftPart, rightPart );
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( leftPart );
    rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( rightPart );
    CleanupStack::PopAndDestroy( rightPart );
    CleanupStack::PopAndDestroy( leftPart );
    }

// -----------------------------------------------------------------------------
// DrawInsetLeftBorder
// -----------------------------------------------------------------------------
//
static void DrawInsetLeftBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawOutsetLeftBorder
// -----------------------------------------------------------------------------
//
static void DrawOutsetLeftBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawLeftBorderL
// -----------------------------------------------------------------------------
//
static void DrawLeftBorderL(
    const TRect& /*aRect*/,
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    TRect borderRect = aNode.BorderRect();
    TRect paddingRect = aNode.PaddingRect();
    TRgb rgb;
    if ( !GetBorderColorL( XnPropertyNames::appearance::common::KBorderLeftColor, aNode, aGc, rgb ) )
        {
        return;
        }
    TPoint tl = borderRect.iTl;
    TPoint bl = borderRect.iBr;
    bl.iX -= borderRect.Width();
    TPoint tr = paddingRect.iTl;
    tr.iX -= 1;
    tr.iY -= 1;
    TPoint br = paddingRect.iBr;
    br.iX -= paddingRect.Width() + 1;
    CArrayFix< TPoint >* array = NULL;
    array = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    TRAPD( error,
        array->AppendL( tl );
        array->AppendL( tr );
        array->AppendL( br );
        array->AppendL( bl ); );
    if ( error != KErrNone )
        {
        delete array;
        return;
        }
    CXnProperty* borderLeftStyle = aNode.BorderLeftStyleL();
    CXnProperty* borderStyle = aNode.BorderStyleL();
    const TDesC8& borderStyleString = ( borderLeftStyle ) ?
        borderLeftStyle->StringValue() :
        ( borderStyle ) ? borderStyle->StringValue() : KNullDesC8;
    if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KSolid )
        {
        DrawSolidLeftBorder( array, aGc );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KInset )
        {
        DrawInsetLeftBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KOutset )
        {
        DrawOutsetLeftBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KRidge )
        {
        TRAP_IGNORE( DrawRidgeLeftBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KGroove )
        {
        TRAP_IGNORE( DrawGrooveLeftBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KDotted )
        {
        TPoint origo( borderRect.iTl.iX, paddingRect.iTl.iY );
        TPoint end( paddingRect.iTl.iX, paddingRect.iBr.iY );
        TRect rect( origo, end );
        DrawDottedBorder( rect, aGc, rgb, EFalse );
        }
    delete array;
    }

// -----------------------------------------------------------------------------
// DrawSolidRightBorder
// -----------------------------------------------------------------------------
//
static void DrawSolidRightBorder( CArrayFix< TPoint >* aArray, CWindowGc& aGc )
    {
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// SplitRightBorderPolygonL
// -----------------------------------------------------------------------------
//
static void SplitRightBorderPolygonL(
    CArrayFix< TPoint >* aArray,
    CArrayFix< TPoint >*& aLeftPart,
    CArrayFix< TPoint >*& aRightPart )
    {
    TPoint upHalf =
        TPoint(
            ( *aArray )[0].iX + ( ( *aArray )[1].iX - ( *aArray )[0].iX ) / 2,
            ( *aArray )[1].iY + ( ( *aArray )[0].iY - ( *aArray )[1].iY ) / 2);
    TPoint downHalf =
        TPoint(
            ( *aArray )[3].iX + ( ( *aArray )[2].iX - ( *aArray )[3].iX ) / 2,
            ( *aArray )[3].iY + ( ( *aArray )[2].iY - ( *aArray )[3].iY ) / 2);
    aLeftPart->AppendL( ( *aArray )[0] );
    aLeftPart->AppendL( upHalf );
    aLeftPart->AppendL( downHalf );
    aLeftPart->AppendL( ( *aArray )[3] );

    aRightPart->AppendL( upHalf );
    aRightPart->AppendL( ( *aArray )[1] );
    aRightPart->AppendL( ( *aArray )[2]) ;
    aRightPart->AppendL( downHalf );
    }

// -----------------------------------------------------------------------------
// DrawRidgeRightBorderL
// -----------------------------------------------------------------------------
//
static void DrawRidgeRightBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CArrayFix< TPoint >* leftPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( leftPart );
    CArrayFix< TPoint >* rightPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( rightPart );
    SplitRightBorderPolygonL( aArray, leftPart, rightPart );
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( leftPart );
    rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( rightPart );
    CleanupStack::PopAndDestroy( rightPart );
    CleanupStack::PopAndDestroy( leftPart );
    }

// -----------------------------------------------------------------------------
// DrawGrooveRightBorderL
// -----------------------------------------------------------------------------
//
static void DrawGrooveRightBorderL(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    CArrayFix< TPoint >* leftPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( leftPart );
    CArrayFix< TPoint >* rightPart = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    CleanupStack::PushL( rightPart );
    SplitRightBorderPolygonL( aArray, leftPart, rightPart );
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( leftPart );
    rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( rightPart );
    CleanupStack::PopAndDestroy( rightPart );
    CleanupStack::PopAndDestroy( leftPart );
    }

// -----------------------------------------------------------------------------
// DrawInsetRightBorder
// -----------------------------------------------------------------------------
//
static void DrawInsetRightBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    TRgb rgb = LighterColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawOutsetRightBorder
// -----------------------------------------------------------------------------
//
static void DrawOutsetRightBorder(
    CArrayFix< TPoint >* aArray,
    CWindowGc& aGc,
    TRgb& aRgb )
    {
    TRgb rgb = DarkerColor( aRgb );
    aGc.SetPenColor( rgb );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetBrushColor( rgb );
    aGc.DrawPolygon( aArray );
    }

// -----------------------------------------------------------------------------
// DrawRightBorderL
// -----------------------------------------------------------------------------
//
static void DrawRightBorderL(
    const TRect& /*aRect*/,
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    TRect borderRect = aNode.BorderRect();
    TRect paddingRect = aNode.PaddingRect();
    TRgb rgb;
    if ( !GetBorderColorL( XnPropertyNames::appearance::common::KBorderRightColor, aNode, aGc, rgb ) )
        {
        return;
        }
    TPoint tl = paddingRect.iTl;
    tl.iX += paddingRect.Width();
    TPoint bl = paddingRect.iBr;
    TPoint tr = borderRect.iTl;
    tr.iX += borderRect.Width()-1;
    TPoint br = borderRect.iBr;
    br.iX -= 1;
    br.iY -= 1;
    CArrayFix< TPoint >* array = NULL;
    array = new ( ELeave ) CArrayFixFlat< TPoint >( 3 );
    TRAPD( error,
        array->AppendL( tl );
        array->AppendL( tr );
        array->AppendL( br );
        array->AppendL( bl ); );
    if ( error != KErrNone )
        {
        delete array;
        return;
        }
    CXnProperty* borderRightStyle = aNode.BorderRightStyleL();
    CXnProperty* borderStyle = aNode.BorderStyleL();
    const TDesC8& borderStyleString = ( borderRightStyle ) ?
        borderRightStyle->StringValue() :
        ( borderStyle ) ? borderStyle->StringValue() : KNullDesC8;
    if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KSolid )
        {
        DrawSolidRightBorder( array, aGc );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KInset )
        {
        DrawInsetRightBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KOutset )
        {
        DrawOutsetRightBorder( array, aGc, rgb );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KRidge )
        {
        TRAP_IGNORE( DrawRidgeRightBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KGroove )
        {
        TRAP_IGNORE( DrawGrooveRightBorderL( array, aGc, rgb ) );
        }
    else if ( borderStyleString == XnPropertyNames::appearance::common::borderstyle::KDotted )
        {
        TPoint origo( paddingRect.iBr.iX, paddingRect.iTl.iY );
        TPoint end( borderRect.iBr.iX, paddingRect.iBr.iY );
        TRect rect( origo, end );
        DrawDottedBorder( rect, aGc, rgb, EFalse );
        }
    delete array;
    }

// -----------------------------------------------------------------------------
// DrawBorderImagesL
// -----------------------------------------------------------------------------
//
static void DrawBorderImagesL(
    const TRect& /*aRect*/,
    CXnNode& aNode,
    CWindowGc& aGc,
    CFbsBitmap* aBitmap,
    TInt aBorderBitmapDividerTop,
    TInt aBorderBitmapDividerRight,
    TInt aBorderBitmapDividerBottom,
    TInt aBorderBitmapDividerLeft )
    {
    CXnProperty* pathProperty = aNode.BorderImageL();
    if ( !pathProperty )
        {
        return;
        }
    TRect borderRect = aNode.BorderRect();
    TRect paddingRect = aNode.PaddingRect();
    TRect topLeft = TRect( borderRect.iTl, paddingRect.iTl );
    TRect topLeftSource = TRect(
        TPoint( 0, 0 ),
        TPoint( aBorderBitmapDividerLeft, aBorderBitmapDividerTop ) );
    aGc.DrawBitmap( topLeft, aBitmap, topLeftSource );
    TRect bottomLeft = TRect(
        TPoint( borderRect.iTl.iX, paddingRect.iTl.iY + paddingRect.Height() ),
        TPoint( paddingRect.iTl.iX, borderRect.iTl.iY + borderRect.Height() ) );
    TSize imageSize = aBitmap->SizeInPixels();
    TRect bottomLeftSource = TRect(
        TPoint( 0, imageSize.iHeight - aBorderBitmapDividerBottom ),
        TPoint( aBorderBitmapDividerLeft, imageSize.iHeight ) );
    aGc.DrawBitmap( bottomLeft, aBitmap, bottomLeftSource );
    TRect topRight = TRect(
        TPoint( paddingRect.iBr.iX, borderRect.iTl.iY ),
        TPoint( borderRect.iBr.iX, paddingRect.iTl.iY ) );
    TRect topRightSource = TRect(
        TPoint( imageSize.iWidth - aBorderBitmapDividerRight, 0 ),
        TPoint( imageSize.iWidth, aBorderBitmapDividerTop ) );
    aGc.DrawBitmap( topRight, aBitmap, topRightSource );
    TRect bottomRight = TRect( paddingRect.iBr, borderRect.iBr );
    TRect bottomRightSource = TRect(
        TPoint(
            imageSize.iWidth - aBorderBitmapDividerRight,
            imageSize.iHeight - aBorderBitmapDividerBottom ),
        TPoint(
            imageSize.iWidth,
            imageSize.iHeight ) );
    aGc.DrawBitmap( bottomRight, aBitmap, bottomRightSource );
    TBool xStretch = ETrue;
    TBool xRepeat = EFalse;
    TBool xRound = EFalse;
    TBool yStretch = ETrue;
    TBool yRepeat = EFalse;
    TBool yRound = EFalse;
    if ( pathProperty )
        {
        CXnDomList& propertyValueList = pathProperty->Property()->PropertyValueList();
        TBool xHandled = EFalse;
        TInt count = propertyValueList.Length();
        for ( TInt i = 0; i < count; ++i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                propertyValueList.Item( i ) );
            if ( value->PrimitiveValueType() == CXnDomPropertyValue::EString ||
                 value->PrimitiveValueType() == CXnDomPropertyValue::EIdent )
                {
                const TDesC8& stringValue = value->StringValueL();
                if ( !xHandled )
                    {
                    xHandled = ETrue;
                    if ( stringValue == XnPropertyNames::appearance::common::borderimage::KStretch )
                        {
                        xStretch = ETrue;
                        yStretch = ETrue;
                        }
                    else if ( stringValue == XnPropertyNames::appearance::common::borderimage::KRepeat )
                        {
                        xStretch = EFalse;
                        yStretch = EFalse;
                        xRepeat = ETrue;
                        yRepeat = ETrue;
                        }
                    else if ( stringValue == XnPropertyNames::appearance::common::borderimage::KRound )
                        {
                        xStretch = EFalse;
                        yStretch = EFalse;
                        xRound = ETrue;
                        yRound = ETrue;
                        }
                    }
                else
                    {
                    if ( stringValue == XnPropertyNames::appearance::common::borderimage::KStretch )
                        {
                        yStretch = ETrue;
                        yRepeat = EFalse;
                        yRound = EFalse;
                        }
                    else if ( stringValue == XnPropertyNames::appearance::common::borderimage::KRepeat )
                        {
                        yStretch = EFalse;
                        yRepeat = ETrue;
                        yRound = EFalse;
                        }
                    else if ( stringValue == XnPropertyNames::appearance::common::borderimage::KRound )
                        {
                        yStretch = EFalse;
                        yRepeat = EFalse;
                        yRound = ETrue;
                        }
                    }
                }
            }
        }
    TRect top = TRect(
        TPoint( paddingRect.iTl.iX, borderRect.iTl.iY ),
        TPoint( paddingRect.iBr.iX, paddingRect.iTl.iY ) );
    TRect topSource = TRect(
        TPoint( aBorderBitmapDividerLeft, 0 ),
        TPoint( imageSize.iWidth - aBorderBitmapDividerRight, aBorderBitmapDividerTop ) );
    if ( xStretch )
        {
        aGc.DrawBitmap( top, aBitmap, topSource );
        }
    else if ( xRepeat && topSource.Width() )
        {
        TInt count = top.Width() / topSource.Width();
        TInt topWidth = top.Width();
        top.SetWidth( topSource.Width() );
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( top, aBitmap, topSource );
            top.Move( topSource.Width(), 0 );
            }
        top.SetWidth( topWidth -  count * topSource.Width() );
        aGc.DrawBitmap( top, aBitmap, topSource );
        }
    else if ( xRound && topSource.Width() )
        {
        TInt count = top.Width() / topSource.Width();
        TInt topWidth = top.Width();
        //top.Move((topWidth - count * topSource.Width()) / 2, 0);
        if ( ( count * topSource.Width() ) < topWidth )
            {
            ++count;
            top.SetWidth( topWidth / count );
            }
        else
            {
            top.SetWidth( topSource.Width() );
            }
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( top, aBitmap, topSource );
            top.Move( top.Width(), 0 );
            }
        }
    TRect right = TRect(
        TPoint( paddingRect.iBr.iX, paddingRect.iTl.iY ),
        TPoint( borderRect.iBr.iX, paddingRect.iBr.iY ) );
    TRect rightSource = TRect(
        TPoint( imageSize.iWidth - aBorderBitmapDividerRight, aBorderBitmapDividerTop ),
        TPoint( imageSize.iWidth, imageSize.iHeight - aBorderBitmapDividerBottom ) );
    if ( yStretch )
        {
        aGc.DrawBitmap( right, aBitmap, rightSource );
        }
    else if ( yRepeat && rightSource.Height() )
        {
        TInt count = right.Height() / rightSource.Height();
        TInt rightHeight = right.Height();
        right.SetHeight( rightSource.Height() );
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( right, aBitmap, rightSource );
            right.Move( 0, rightSource.Height() );
            }
        right.SetHeight( rightHeight - count * rightSource.Height() );
        aGc.DrawBitmap( right, aBitmap, rightSource );
        }
    else if ( yRound && rightSource.Height() )
        {
        TInt count = right.Height() / rightSource.Height();
        TInt rightHeight = right.Height();
        if ( ( count * rightSource.Height() ) < rightHeight )
            {
            ++count;
            right.SetHeight( rightHeight / count );
            }
        else
            {
            right.SetHeight( rightSource.Width() );
            }
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( right, aBitmap, rightSource );
            right.Move( 0, right.Height() );
            }
        }
    TRect bottom = TRect(
        TPoint( paddingRect.iTl.iX, paddingRect.iBr.iY ),
        TPoint( paddingRect.iBr.iX, borderRect.iBr.iY ) );
    TRect bottomSource = TRect(
        TPoint( aBorderBitmapDividerLeft, imageSize.iHeight - aBorderBitmapDividerBottom ),
        TPoint( imageSize.iWidth - aBorderBitmapDividerRight, imageSize.iHeight ) );
    if ( xStretch )
        {
        aGc.DrawBitmap( bottom, aBitmap, bottomSource );
        }
    else if ( xRepeat && bottomSource.Width() )
        {
        TInt count = bottom.Width() / bottomSource.Width();
        TInt bottomWidth = bottom.Width();
        bottom.SetWidth( bottomSource.Width() );
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( bottom, aBitmap, bottomSource );
            bottom.Move( bottomSource.Width(), 0 );
            }
        bottom.SetWidth( bottomWidth - count * bottomSource.Width() );
        aGc.DrawBitmap( bottom, aBitmap, bottomSource );
        }
    else if ( xRound && bottomSource.Width() )
        {
        TInt count = bottom.Width() / bottomSource.Width();
        TInt bottomWidth = bottom.Width();
        if ( ( count * bottomSource.Width() ) < bottomWidth )
            {
            ++count;
            bottom.SetWidth( bottomWidth / count );
            }
        else
            {
            bottom.SetWidth( bottomSource.Width() );
            }
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( bottom, aBitmap, bottomSource );
            bottom.Move( bottom.Width(), 0 );
            }
        }
    TRect left = TRect(
        TPoint( borderRect.iTl.iX, paddingRect.iTl.iY ),
        TPoint( paddingRect.iTl.iX, paddingRect.iBr.iY ) );
    TRect leftSource = TRect(
        TPoint( 0, aBorderBitmapDividerTop ),
        TPoint( aBorderBitmapDividerLeft, imageSize.iHeight - aBorderBitmapDividerBottom ) );
    if ( yStretch )
        {
        aGc.DrawBitmap( left, aBitmap, leftSource );
        }
    else if ( yRepeat && leftSource.Height() )
        {
        TInt count = left.Height() / leftSource.Height();
        TInt leftHeight = left.Height();
        left.SetHeight( leftSource.Height() );
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( left, aBitmap, leftSource );
            left.Move( 0, leftSource.Height() );
            }
        left.SetHeight( leftHeight - count * leftSource.Height() );
        aGc.DrawBitmap( left, aBitmap, leftSource );
        }
    else if ( yRound && leftSource.Height() )
        {
        TInt count = left.Height() / leftSource.Height();
        TInt leftHeight = left.Height();
        if ( ( count * leftSource.Height() ) < leftHeight )
            {
            ++count;
            left.SetHeight( leftHeight / count );
            }
        else
            {
            left.SetHeight( leftSource.Width() );
            }
        for ( TInt i = 0; i < count; ++i )
            {
            aGc.DrawBitmap( left, aBitmap, leftSource );
            left.Move( 0, left.Height() );
            }
        }
    }

// -----------------------------------------------------------------------------
// IsPropertyNone
// -----------------------------------------------------------------------------
//
static TBool IsPropertyNone( CXnProperty& aProperty )
    {
    if ( &aProperty )
        {
        TInt index = 0;
        CXnDomProperty* domProperty = aProperty.Property();
        CXnDomPropertyValue* domPropertyValue =
            static_cast< CXnDomPropertyValue* >(
                domProperty->PropertyValueList().Item( index ) );
        return domPropertyValue->IsNoneIdent();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// DrawBordersL
// -----------------------------------------------------------------------------
//
static void DrawBordersL( const TRect& aRect, CXnNode& aNode, CWindowGc& aGc )
    {
    TRect borderRect( aNode.BorderRect() );

    if( aNode.PaddingRect() == borderRect )
        {
        return;
        }

    CXnProperty* commonBorderStyle( aNode.BorderStyleL() );
    CXnProperty* borderStyle( aNode.BorderTopStyleL() );

    if ( ( borderStyle && !IsPropertyNone( *borderStyle ) ) ||
         ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) )
        {
        DrawTopBorderL( aRect, aNode, aGc );
        }

    borderStyle = aNode.BorderLeftStyleL();

    if ( ( borderStyle && !IsPropertyNone( *borderStyle ) ) ||
         ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) )
        {
        DrawLeftBorderL( aRect, aNode, aGc );
        }

    borderStyle = aNode.BorderRightStyleL();

    if ( ( borderStyle && !IsPropertyNone( *borderStyle ) ) ||
         ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) )
        {
        DrawRightBorderL( aRect, aNode, aGc );
        }

    borderStyle = aNode.BorderBottomStyleL();

    if ( ( borderStyle && !IsPropertyNone( *borderStyle ) ) ||
         ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) )
        {
        DrawBottomBorderL( aRect, aNode, aGc );
        }
    }

// -----------------------------------------------------------------------------
// SwapChildArrays
// -----------------------------------------------------------------------------
//
static void SwapChildArrays(
    RPointerArray< CXnControlAdapter >& originalArray,
    RPointerArray< CXnControlAdapter >& sortedArray )
    {
    originalArray.Reset();
    TInt count = sortedArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        originalArray.Append( sortedArray[i] );
        }
    sortedArray.Reset();
    }

// -----------------------------------------------------------------------------
// IsChildZIndexLowerThanCandidateZIndexL
// -----------------------------------------------------------------------------
//
static TBool IsChildZIndexLowerThanCandidateZIndexL(
    CXnProperty* aChildZIndex,
    CXnDomPropertyValue* aChildZIndexValue,
    CXnProperty* aCandidateZIndex )
    {
    if ( !aChildZIndex && !aCandidateZIndex )
        {
        return ETrue;
        }
    if ( !aChildZIndex )
        {
        CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
            aCandidateZIndex->Property()->PropertyValueList().Item( 0 ) );
        if ( value->IsAutoIdent() )
            {
            return ETrue;
            }
        TReal num = value->FloatValueL();
        if ( num > 0 )
            {
            return ETrue;
            }
        }
    else if ( !aCandidateZIndex )
        {
        if ( aChildZIndexValue->IsAutoIdent() )
            {
            return ETrue;
            }
        TReal num = aChildZIndexValue->FloatValueL();
        if ( num < 0 )
            {
            return ETrue;
            }
        }
    else
        {
        CXnDomPropertyValue* candidateValue =
            static_cast< CXnDomPropertyValue* >(
                aCandidateZIndex->Property()->PropertyValueList().Item( 0 ) );
        if ( aChildZIndexValue->IsAutoIdent() && candidateValue->IsAutoIdent() )
            {
            return ETrue;
            }
        else if ( aChildZIndexValue->IsAutoIdent() && !candidateValue->IsAutoIdent() )
            {
            return ETrue;
            }
        else if ( !aChildZIndexValue->IsAutoIdent() && candidateValue->IsAutoIdent() )
            {
            return EFalse;
            }
        TReal childNum = aChildZIndexValue->FloatValueL();
        TReal candidateNum = candidateValue->FloatValueL();
        if ( childNum <= candidateNum )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// GetZIndex
// -----------------------------------------------------------------------------
//
static CXnProperty* GetZIndexL( CXnControlAdapter* aAdapter )
    {
    CXnComponent* component = aAdapter->Component();
    if ( !component )
        {
        return NULL;
        }
    CXnNode& node = component->Node()->Node();
    return node.ZIndexL();
    }

// -----------------------------------------------------------------------------
// InsertChildToSortedArrayL
// -----------------------------------------------------------------------------
//
static void InsertChildToSortedArrayL(
    RPointerArray< CXnControlAdapter >& aTargetArray,
    CXnControlAdapter* aChild )
    {
    CXnProperty* childZIndex = GetZIndexL( aChild );
    CXnDomPropertyValue* childValue = NULL;
    if ( childZIndex )
        {
        childValue = static_cast< CXnDomPropertyValue* >(
            childZIndex->Property()->PropertyValueList().Item( 0 ) );
        }
    TInt count = aTargetArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnControlAdapter* candidate = aTargetArray[i];
        if ( IsChildZIndexLowerThanCandidateZIndexL(
                 childZIndex, childValue, GetZIndexL( candidate ) ) )
            {
            aTargetArray.Insert( aChild, i );
            return;
            }
        }
    aTargetArray.Append( aChild );
    }

// -----------------------------------------------------------------------------
// InitializeBackgroundBitmapL
// -----------------------------------------------------------------------------
//
static void InitializeBackgroundBitmapL( CXnUiEngine& aEngine, CXnNode& aNode,
    TInt& aBitmapIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider, RFs& aFsSession )
    {
    HBufC* bgPath( GetBackgroundImagePathLC( aNode ) );
    TPtr ptr( bgPath->Des() );

    GetBitmapL( aEngine, aNode, ptr, aBitmapIndex,
                aBitmap, aMask, aIconProvider, aNode.PaddingRect(), aFsSession );

    CleanupStack::PopAndDestroy( bgPath );

    if ( !aMask )
        {
        HBufC* bgMaskPath( GetBackgroundImageMaskPathLC( aNode ) );
        TPtr ptr( bgMaskPath->Des() );

        if ( ptr != KNullDesC )
            {
            CFbsBitmap* tmpMask( NULL );

            TRAP_IGNORE( GetBitmapL( aEngine, aNode, ptr, aBitmapIndex, aMask,
                tmpMask, aIconProvider, aNode.PaddingRect(), aFsSession ) );

            delete tmpMask;
            }

        CleanupStack::PopAndDestroy( bgMaskPath );
        }
    }

// -----------------------------------------------------------------------------
// GetBackgroundImagePathLC
// -----------------------------------------------------------------------------
//
static HBufC* GetBackgroundImagePathLC( CXnNode& aNode )
    {
    CXnProperty* pathProperty( aNode.BackgroundImageL() );

    if ( pathProperty && pathProperty->StringValue() != KNullDesC8 )
        {
        HBufC* path( pathProperty->StringValueL() );
        CleanupStack::PushL( path );

        CXnUtils::StripQuotes( path );

        return path;
        }

    return KNullDesC().AllocLC();
    }

// -----------------------------------------------------------------------------
// GetBackgroundImageMaskPathLC
// -----------------------------------------------------------------------------
//
static HBufC* GetBackgroundImageMaskPathLC( CXnNode& aNode )
    {
    CXnProperty* pathProperty( aNode.GetPropertyL(
        XnPropertyNames::common::KBackgroundMask ) );

    if ( pathProperty && pathProperty->StringValue() != KNullDesC8 )
        {
        HBufC* path( pathProperty->StringValueL() );
        CleanupStack::PushL( path );

        CXnUtils::StripQuotes( path );

        return path;
        }

    return KNullDesC().AllocLC();
    }

// -----------------------------------------------------------------------------
// InitializeBorderBitmapL
// -----------------------------------------------------------------------------
//
static CFbsBitmap* InitializeBorderBitmapL(
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    TInt& aBitmapIndex,
    TInt& aBorderBitmapDividerTop,
    TInt& aBorderBitmapDividerRight,
    TInt& aBorderBitmapDividerBottom,
    TInt& aBorderBitmapDividerLeft,
    CXnControlAdapterImpl::TIconProvider*& aIconProvider,
    RFs& aFsSession )
    {
    CXnProperty* pathProperty = aNode.BorderImageL();
    CFbsBitmap* returnValue = NULL;
    CFbsBitmap* bitmapMask = NULL;

    if ( !pathProperty )
        {
        return NULL;
        }
    
    CXnDomList& propertyValueList = pathProperty->Property()->PropertyValueList();
    TInt count = propertyValueList.Length();

    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
            propertyValueList.Item( i ) );

        if ( value->PrimitiveValueType() == CXnDomPropertyValue::EUri )
            {
            const TDesC8& path = value->StringValueL();

            HBufC* utfPath = HBufC::NewL( path.Length() );
            TPtr16 ptr = utfPath->Des();

            CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr, path );
            CleanupStack::PushL( utfPath );

            StripQuotes( utfPath );

            GetBitmapL( aEngine, aNode, *utfPath, aBitmapIndex, returnValue,
                    bitmapMask, aIconProvider, aNode.BorderRect(), aFsSession );

            delete bitmapMask;

            CleanupStack::PopAndDestroy( utfPath );
            }
       else if ( value->PrimitiveValueType() == CXnDomPropertyValue::EPercentage )
            {
            if ( returnValue )
                {
                CXnDomPropertyValue* tmpValue = NULL;
                tmpValue = value->CloneL();
                CXnProperty* tmpProperty = NULL;
                CleanupStack::PushL( tmpValue );
                tmpProperty = CXnProperty::NewL(
                    KNullDesC8,
                    tmpValue,
                *aNode.UiEngine()->ODT()->DomDocument().StringPool() );
                CleanupStack::Pop( tmpValue );
                TSize imageSize = returnValue->SizeInPixels();
                CleanupStack::PushL( tmpProperty );
                TInt intValue = static_cast< TInt >( value->FloatValueL() );
                TInt dividerValue = 0;
    
                switch ( i )
                    {
                    case 1:
                        {
                        dividerValue = aNode.UiEngine()->VerticalPixelValueL(
                            tmpProperty, imageSize.iHeight );
                        aBorderBitmapDividerTop = dividerValue;
                        break;
                        }
                    case 2:
                        {
                        dividerValue = aNode.UiEngine()->HorizontalPixelValueL(
                            tmpProperty, imageSize.iWidth );
                        aBorderBitmapDividerRight = dividerValue;
                        break;
                        }
                    case 3:
                        {
                        dividerValue = aNode.UiEngine()->VerticalPixelValueL(
                            tmpProperty, imageSize.iHeight );
                        aBorderBitmapDividerBottom = dividerValue;
                        break;
                        }
                    case 4:
                        {
                        dividerValue = aNode.UiEngine()->HorizontalPixelValueL(
                            tmpProperty, imageSize.iWidth );
                        aBorderBitmapDividerLeft = dividerValue;
                        break;
                        }
                    }
    
                CleanupStack::PopAndDestroy( tmpProperty );
                }
            }
       else if ( value->PrimitiveValueType() == CXnDomPropertyValue::EString ||
                 value->PrimitiveValueType() == CXnDomPropertyValue::EIdent )
            {
            }
       else
            {
            TInt intValue = static_cast< TInt >( value->FloatValueL() );

            switch ( i )
                {
                case 1:
                    {
                    aBorderBitmapDividerTop = intValue;
                    break;
                    }
                case 2:
                    {
                    aBorderBitmapDividerRight = intValue;
                    break;
                    }
                case 3:
                    {
                    aBorderBitmapDividerBottom = intValue;
                    break;
                    }
                case 4:
                    {
                    aBorderBitmapDividerLeft = intValue;
                    break;
                    }
                }
            }
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// BuildTriggerTypeNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerTypeNodeL( const TDesC8& aName,
    const TDesC8& aValue, CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KProperty );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aName );
    CXnProperty* name = CXnProperty::NewL( XnPropertyNames::action::KName,
        nameValue, *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnDomPropertyValue* valueValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( valueValue );
    valueValue->SetStringValueL( CXnDomPropertyValue::EString, aValue );
    CXnProperty* value = CXnProperty::NewL( XnPropertyNames::action::KValue,
        valueValue, *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( valueValue );
    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildTriggerNodeL
// Builds a trigger node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerNodeL(
    CXnUiEngine& aUiEngine,
    const TDesC8& aTriggerName,
    const TDesC8& aTriggerValueName,
    const TDesC8& aTriggerValue )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue =
        CXnDomPropertyValue::NewL( aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );
    CXnProperty* name = CXnProperty::NewL( XnPropertyNames::action::trigger::KName,
        nameValue, *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnNode* triggerType = BuildTriggerTypeNodeL( aTriggerValueName,
        aTriggerValue, aUiEngine );
    CleanupStack::PushL( triggerType );
    node->AddChildL( triggerType );
    CleanupStack::Pop( triggerType );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildTriggerNodeL
// Builds a trigger node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerNodeL(
    CXnUiEngine& aUiEngine,
    const TDesC8& aTriggerName )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue =
        CXnDomPropertyValue::NewL( aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );
    CXnProperty* name = CXnProperty::NewL( XnPropertyNames::action::trigger::KName,
        nameValue, *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// Create scaled bitmap from source bitmap.
// -----------------------------------------------------------------------------
//
void CreateScaledBitmapL(
    const TRect& aRect,
    CFbsBitmap*& aTrgBitmap,
    CFbsBitmap* aSrcBitmap,
    TBool aPreserveAspectRatio,
    TBool aForceFallBack )
    {
    TRect destRect = aRect;

    if ( aPreserveAspectRatio )
        {
        // Calculate the bitmap image dimensions so that it uses maximum space
        // of the given rectangle and maintains aspect ratio.
        TInt srcHeight = aSrcBitmap->SizeInPixels().iHeight;
        TInt srcWidth = aSrcBitmap->SizeInPixels().iWidth;
        TReal scaleRatio( 1 ); //no scale as defaul

        //If any dimension is 0, then we do not bother to scale
        if ( aRect.Width() > 0 && aRect.Height() > 0 )
            {
            TReal xRatio = ( ( TReal )srcWidth / ( TReal )aRect.Width() );
            TReal yRatio = ( ( TReal )srcHeight / ( TReal )aRect.Height() );
            //Find out appropriate scaling factor
            xRatio > yRatio ? ( scaleRatio = xRatio ) : ( scaleRatio = yRatio );
            }

        //Scale the size for target bitmap
        destRect.SetHeight( srcHeight / scaleRatio );
        destRect.SetWidth( srcWidth / scaleRatio );
        }

    // see if there's a need to scale. If source and destination size are the same,
    // then we don't need to duplicate the bitmap. aTrgBitmap will be null.
    TSize srcSize = aSrcBitmap->SizeInPixels();
    TSize destSize = destRect.Size();
    if ( srcSize == destSize )
        {
        return;
        }

    aTrgBitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( aTrgBitmap );
    //It is allowed to create zero height or width bitmap.
    TInt err( aTrgBitmap->Create( destRect.Size(), aSrcBitmap->DisplayMode() ) );

    if ( err == KErrNone )
        {
        CXnUtils::ScaleBitmapExtL( destRect, aTrgBitmap, aSrcBitmap, aForceFallBack );
        }

    //we do not own the bitmap so just Pop.
    CleanupStack::Pop( aTrgBitmap );
    }

// -----------------------------------------------------------------------------
// SoftkeyNode
// Gets the node of softkey according to index
// -----------------------------------------------------------------------------
//
static CXnNode* SoftkeyNodeL( CXnNode* aMenuBarNode, const TDesC8& aSoftkey )
    {
    if ( aMenuBarNode )
        {
        XnMenuInterface::MXnMenuInterface* menuIf( NULL );
        XnComponentInterface::MakeInterfaceL( menuIf, aMenuBarNode->AppIfL() );
        CXnNodePluginIf* skNode( NULL );
        if ( menuIf )
            {
            if ( aSoftkey == XnPropertyNames::softkey::type::KLeft )
                {
                skNode = menuIf->SoftKeyL(
                    XnMenuInterface::MXnMenuInterface::ELeft );
                }
            else if ( aSoftkey == XnPropertyNames::softkey::type::KMiddle )
                {
                skNode = menuIf->SoftKeyL(
                    XnMenuInterface::MXnMenuInterface::ECenter );
                }
            else if ( aSoftkey == XnPropertyNames::softkey::type::KRight )
                {
                skNode = menuIf->SoftKeyL(
                    XnMenuInterface::MXnMenuInterface::ERight );
                }
            }
        if ( skNode )
            {
            return &skNode->Node();
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// SoftkeyNode
// Gets the node of softkey according to pointer location
// -----------------------------------------------------------------------------
//
static CXnNode* SoftkeyNode( CXnNode* aMenuBarNode, const TPoint aPosition )
    {
    if ( aMenuBarNode )
        {
        CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >(
            aMenuBarNode->ComponentNodeImpl()->Component()->ControlAdapter() );

        CXnNodePluginIf* node( NULL );

        if ( adapter->FindSoftKeyNodeByPosition( aPosition, node ) )
            {
            return &node->Node();
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// DrawFocusAppearance
// Draws focus appearance
// -----------------------------------------------------------------------------
//
static void DrawFocusAppearance( CXnNode& aNode, CWindowGc& aGc )    
    {    
    TBool grow( EFalse );
    
    const TDesC8& name( aNode.DomNode()->Name() );
        
    if( name == XnPropertyNames::KPlugin )
        {
        grow = ETrue;
        }
    else
        {
        TRect marginRect( aNode.MarginRect() );
        
        CXnNode* parent( aNode.Parent() );
        
        for( ; parent; parent = parent->Parent() )
            {
            if( parent->DomNode()->Name() == XnPropertyNames::KPlugin )
                {
                if( parent->Rect() == marginRect )
                    {
                    grow = ETrue;
                    }
                
                break;
                }
            }                
        }
    
    TRect rect( aNode.PaddingRect() );
    
    if( grow )
        {
        rect.Grow( KFocusGrowValue, KFocusGrowValue );
        }

    CXnAppUiAdapter* appui( 
        static_cast< CXnAppUiAdapter* >( iAvkonAppUi ) );
    
    appui->ViewAdapter().FocusControl().Draw( rect, aGc );    
    }

// -----------------------------------------------------------------------------
// HasHoldTrigger
// Queries whether this node has a hold trigger defined
// -----------------------------------------------------------------------------
//
static CXnDomNode* HasHoldTrigger( CXnDomNode* aNode )
    {    
    if ( !aNode )
        {        
        return NULL;
        }

    if ( aNode->Name() == XnPropertyNames::action::KTrigger )
        {
        CXnDomList& list( aNode->AttributeList() );
        
        CXnDomAttribute* name( NULL );
        
        name = static_cast< CXnDomAttribute* >(  
            list.FindByName( XnPropertyNames::action::KName ) );
                    
        if ( name && name->Value() == XnPropertyNames::action::trigger::name::KHold )
            {
            return aNode;            
            }
        }

    CXnDomList& list( aNode->ChildNodes() );

    for ( TInt i = 0; i < list.Length(); i++ )
        {
        CXnDomNode* retval( HasHoldTrigger(
            static_cast< CXnDomNode* >( list.Item( i ) ) ) );

        if ( retval )
            {
            return retval;
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CancelFocusRefusalL
// Cancels focus refusal
// -----------------------------------------------------------------------------
//
static void CancelFocusRefusalL( CXnUiEngine& aUiEngine )
    {
    CXnNode* focused( aUiEngine.FocusedNode() );
    
    if ( focused )
        {
        CXnControlAdapter* control( focused->Control() );
        
        if ( control && control->RefusesFocusLoss() )
            {
            focused->HideTooltipsL();
            
            // It is now time to give up holding focus
            focused->UnsetStateL( 
                XnPropertyNames::style::common::KFocus );
            }
        }    
    }

// -----------------------------------------------------------------------------
// BuildSwipeTriggerNodeLC
// Build trigger node for swipe event
// -----------------------------------------------------------------------------
//
CXnNode* CXnControlAdapterImpl::BuildSwipeTriggerNodeLC(
    CXnUiEngine& aUiEngine,
    const TDesC8& aDirection )
    {
    CXnDomStringPool* sp( aUiEngine.ODT()->DomDocument().StringPool() );

    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );

    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );

    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );

    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );

    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL( sp );
    CleanupStack::PushL( nameValue );

    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::KSwipe );

    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue, *sp );

    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );

    node->SetPropertyL( name );
    CleanupStack::Pop( name );

    CXnDomPropertyValue* reasonValue = CXnDomPropertyValue::NewL( sp );
    CleanupStack::PushL( reasonValue );

    reasonValue->SetStringValueL( CXnDomPropertyValue::EString, aDirection );

    CXnProperty* reason = CXnProperty::NewL(
        XnPropertyNames::action::trigger::name::swipe::KDirection,
        reasonValue, *sp );

    CleanupStack::Pop( reasonValue );

    CleanupStack::PushL( reason );
    node->SetPropertyL( reason );
    CleanupStack::Pop( reason );

    return node;
    }

// -----------------------------------------------------------------------------
// CreateGestureHelperL
// Checks whether gesture is needed to be detected for this node
// -----------------------------------------------------------------------------
//
static TBool CreateGestureHelperL( CXnNode& aNode )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        CXnProperty* prop( aNode.GetPropertyL( XnPropertyNames::common::KSwipe ) );

        if ( prop && prop->StringValue() == XnPropertyNames::KTrue )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CXnControlAdapterImpl* CXnControlAdapterImpl::NewL(
    CXnNodePluginIf& aNode,
    CXnControlAdapter& aAdapter,
    CWindowGc& aGc )
    {
    CXnControlAdapterImpl* self = new ( ELeave ) CXnControlAdapterImpl( aNode );
    CleanupStack::PushL( self );
    self->ConstructL( aNode, aAdapter, aGc );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::~CXnControlAdapterImpl
// Destructor.
// -----------------------------------------------------------------------------
//
CXnControlAdapterImpl::~CXnControlAdapterImpl()
    {
    delete iBackgroundBitmap;
    delete iBackgroundMask;
    delete iBorderBitmap;
    delete iContentBitmap;
    delete iContentMask;

    delete iScaledContentBitmap;
    delete iScaledContentMask;

    delete iScaledTransparentColor;
    delete iScaledBackgroundSkin;
    delete iScaledBackgroundImage;

    iChildren.Reset();

    if ( iAnimation )
        {
        iAnimation->Stop();
        delete iAnimation;
        }

    if ( iGestureHelper )
        {
        delete iGestureHelper;
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::CXnControlAdapterImpl
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnControlAdapterImpl::CXnControlAdapterImpl( CXnNodePluginIf& aNode )
    : iNode( aNode )
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::SetComponent
// Sets component object to adapter.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::SetComponent( CXnComponent* aComponent )
    {
    iComponent = aComponent;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::Component
// Gets component object from adapter.
// -----------------------------------------------------------------------------
//
CXnComponent* CXnControlAdapterImpl::Component()
    {
    return iComponent;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::Component
// Gets component object from adapter.
// -----------------------------------------------------------------------------
//
CXnComponent* CXnControlAdapterImpl::Component() const
    {
    return iComponent;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::OfferKeyEventL
// Handles key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CXnControlAdapterImpl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CXnNode* node( &iNode.Node() );

    // Need to get softkey nodes
    CXnUiEngine* engine( node->UiEngine() );

    CXnNode* menuBar( engine->MenuBarNode() );

    CXnNode* temp( node );

    for ( ; temp; temp = temp->Parent() )
        {
        if ( temp == menuBar )
            {
            // This is softkey node
            const TDesC8* pos( NULL );
            node = NULL;

            if ( aKeyEvent.iScanCode == EStdKeyDevice0 )
                {
                pos = &XnPropertyNames::softkey::type::KLeft;
                }
            else if ( aKeyEvent.iScanCode == EStdKeyDevice1 )
                {
                pos = &XnPropertyNames::softkey::type::KRight;
                }
            else if ( aKeyEvent.iScanCode == EStdKeyDevice3 )
                {
                pos = &XnPropertyNames::softkey::type::KMiddle;
                }

            if ( pos )
                {
                node = SoftkeyNodeL( menuBar, *pos );
                }

            break;
            }
        }

    if ( !node )
        {
        return EKeyWasNotConsumed;
        }

    if ( aKeyEvent.iScanCode == EStdKeyDevice0 || // RSK
         aKeyEvent.iScanCode == EStdKeyDevice1 || // LSK
         aKeyEvent.iScanCode == EStdKeyDevice3 )  // MSK
        {
        if ( aType == EEventKeyDown )
            {
            iKeyDownNode = node;
            iLongtap = EFalse;
            
            if ( aKeyEvent.iScanCode == EStdKeyDevice3 ||
                aKeyEvent.iScanCode == EStdKeyEnter )
                {
                if ( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
                    {
                    // Set "pressed down"
                    node->SetStateL( 
                        XnPropertyNames::style::common::KPressedDown );
                    
                    node->UiEngine()->RenderUIL();
                    }                
                }
            
            _LIT8( KDown, "3" ); // EEventKeyDown == 3
            
            CXnNode* keydown( BuildTriggerNodeL( *engine,                
                XnPropertyNames::action::trigger::name::KActivate,
                XnPropertyNames::action::trigger::name::keyevent::KEventType, KDown ) );                
            CleanupStack::PushL( keydown );

            node->ReportXuikonEventL( *keydown );

            CleanupStack::PopAndDestroy( keydown );                              
            }
        else if( aType == EEventKey )
            {
            if( iKeyDownNode != node)
            	{
                node = iKeyDownNode;
            	}
            if ( aKeyEvent.iRepeats == 0 )
                {
                if ( !HasHoldTrigger( node->DomNode() ) )
                    {
                    // If node doesn't define hold trigger, then report activate
                    // immediately. Otherwise activate is triggered with keyup event
                    node->SetStateL( 
                        XnPropertyNames::style::common::KActive );                     
                    }
                }
            else
                {
                CXnNode* hold( BuildTriggerNodeL( *engine,
                    XnPropertyNames::action::trigger::name::KHold ) );
                CleanupStack::PushL( hold );

                node->ReportXuikonEventL( *hold );

                CleanupStack::PopAndDestroy( hold );
                
                iLongtap = ETrue;
                }
            }
                
        else if ( aType == EEventKeyUp )
            {
            if ( !iLongtap )
                {
              if( iKeyDownNode != node)
                {
                node = iKeyDownNode;
                }   
                if ( HasHoldTrigger( node->DomNode() ) )
                    {
                    // Hold trigger defined, report activate event here                   
                    node->SetStateL( 
                        XnPropertyNames::style::common::KActive );                     
                    }
                else
                    {
                    _LIT8( KUp, "2" ); // EEventKeyUp == 2
                    
                    CXnNode* keyup( BuildTriggerNodeL( *engine,                
                        XnPropertyNames::action::trigger::name::KActivate,
                        XnPropertyNames::action::trigger::name::keyevent::KEventType, KUp ) );                
                    CleanupStack::PushL( keyup );
        
                    node->ReportXuikonEventL( *keyup );
                    
                    CleanupStack::PopAndDestroy( keyup );                    
                    }
                }
            
            if ( aKeyEvent.iScanCode == EStdKeyDevice3 ||
                aKeyEvent.iScanCode == EStdKeyEnter )
                {
                // Reset "pressed down"
                node->UnsetStateL( 
                    XnPropertyNames::style::common::KPressedDown );
                
                node->UiEngine()->RenderUIL();
                }                                    
            
            iLongtap = EFalse;
            }
        }
    else
        {
        // Other than softkey node
        node->OfferKeyEventL( aKeyEvent, aType );
        }
    
    CXnNode* viewNode( engine->ActiveView() );

    if ( viewNode && node != viewNode )
        {
        viewNode->OfferKeyEventL( aKeyEvent, aType );
        }

    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::HandleLongTapEventL
// Handles the long tap events.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::HandleLongTapEventL(
    const TPoint& /*aPenEventLocation*/, const TPoint& aPenEventScreenLocation )
    {
    CXnNode* node( &iNode.Node() );

    CXnUiEngine* engine( node->UiEngine() );

    TBool menuBar( node == engine->MenuBarNode() );

    if ( menuBar )
        {
        // Need to update node to match to the real softkey node
        node = SoftkeyNode( engine->MenuBarNode(), aPenEventScreenLocation );
        }

    if ( node )
        {
        CXnProperty* prop( node->GetPropertyL(
            XnPropertyNames::common::KLongTap ) );

        if ( prop && prop->StringValue() == XnPropertyNames::KTrue )
            {                        
            CXnUiEngine* engine( node->UiEngine() );
            CXnAppUiAdapter& appui( engine->AppUiAdapter() );

            CancelFocusRefusalL( *engine );
            
            appui.HideFocus();

            if ( !menuBar )
                {
                CXnViewControlAdapter* control = static_cast< CXnViewControlAdapter* >(
                    appui.ViewManager().ActiveViewData().ViewNode()->Control() );
                
                control->IgnoreEventsUntilNextPointerUp();
                control->ResetGrabbing();                      
                }

          // Indicate long tap has taken place
            iLongtap = ETrue;
            
            CXnNode* hold = BuildTriggerNodeL( *engine,
                XnPropertyNames::action::trigger::name::KHold );

            CleanupStack::PushL( hold );

            node->ReportXuikonEventL( *hold );

            CleanupStack::PopAndDestroy( hold );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::HandlePointerEventL
// Handle pointer events
// -----------------------------------------------------------------------------
//
TBool CXnControlAdapterImpl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    const TPointerEvent& event( aPointerEvent );
    
    CXnNode* node( &iNode.Node() );
    CXnUiEngine* engine( node->UiEngine() );
 
    if(!engine->IsPartialInputActive())
        {
        // Forward event to gesture helper
        if( PassEventToGestureHelperL( aPointerEvent ) )
            { 
            CXnAppUiAdapter& appui( engine->AppUiAdapter() );
            CXnViewData& data( appui.ViewManager().ActiveViewData() );
            
            CXnViewControlAdapter* control = 
                static_cast< CXnViewControlAdapter* >( data.ViewNode()->Control() );
            
            control->ResetGrabbing();

            // Swipe took place, consume this event
            return ETrue;
            }

        }
    
    TBool menuBar( node == engine->MenuBarNode() );

    if ( menuBar )
        {
        // Need to update node to match to the real softkey node
        node = SoftkeyNode( engine->MenuBarNode(), event.iParentPosition );

        if ( !node )
            {
            // No softkey node found, consume event
            return ETrue;
            }
        }
    
    CAknLongTapDetector* detector( iAdapter->LongTapDetector() );
    
    if ( ( detector && ( !engine->IsPartialInputActive() || 
        event.iType == TPointerEvent::EButton1Up ) ) ) 
        {
        if ( menuBar )
            {
            // CXnMenuAdapter owns the longtap detector, but only
            // softkey nodes can have longtap functionality, so
            // need to check whether this softkey node has a longtap
            // property defined
            CXnProperty* prop(
                node->GetPropertyL( XnPropertyNames::common::KLongTap ) );

            if ( prop && prop->StringValue() == XnPropertyNames::KTrue )
                {
                iAdapter->SetLongTapDelays( KLongTapStartShortDelay,
                                            KLongTapTimeShortDelay );
                detector->PointerEventL( event );
                }
            }
        else
            {
            iAdapter->SetLongTapDelays( KLongTapStartLongDelay,
                                        KLongTapTimeLongDelay );
            detector->PointerEventL( event );
            }
        }
    
    CXnProperty* focusable( 
        node->GetPropertyL( XnPropertyNames::common::KFocusable ) );
            
    if ( !menuBar && ( !focusable ||  
        focusable->StringValue() == XnPropertyNames::KFalse ) ) 
        {
        iLongtap = EFalse;
        
        return EFalse;
        }                

    if ( event.iType == TPointerEvent::EButton1Down )
        {
        iLongtap = EFalse;
        node->HideTooltipsL();
        
        if ( !menuBar )
            {
            CancelFocusRefusalL( *engine );
            }
                        
        if ( !menuBar && !engine->FocusedNode() )
            {
            // Require focus to be shown
            engine->AppUiAdapter().ShowFocus();                
            
#ifdef RD_TACTILE_FEEDBACK            
            MTouchFeedback* feedback( MTouchFeedback::Instance() );
            
            if ( feedback )
                {
                feedback->InstantFeedback( ETouchFeedbackBasic );
                }
#endif                           
            node->SetStateL( 
                XnPropertyNames::style::common::KFocus, 
                XnEventSource::EStylus );
            node->SetStateL( 
                XnPropertyNames::style::common::KPressedDown );
            }        
        }
    else if ( event.iType == TPointerEvent::EDrag )
        {        
        if ( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
            {
            if ( !node->MarginRect().Contains( event.iPosition ) )
                {
                // Remove pressed down
                node->UnsetStateL(
                    XnPropertyNames::style::common::KPressedDown );
                }
            if ( node->MarginRect().Contains( event.iPosition ) )
                {
                // Add pressed down
                node->SetStateL(
                    XnPropertyNames::style::common::KPressedDown );
                }
            }
        }
    else if ( event.iType == TPointerEvent::EButton1Up )
        {
        if ( !iLongtap )
            {
            if ( menuBar ) 
                {
                node->SetStateL( XnPropertyNames::style::common::KActive );
                }          
            else if ( ( node->MarginRect().Contains( event.iPosition ) &&
               node->IsStateSet( XnPropertyNames::style::common::KFocus ) &&
               node->IsStateSet( XnPropertyNames::style::common::KPressedDown ) ) )
                {
#ifdef RD_TACTILE_FEEDBACK            
                MTouchFeedback* feedback( MTouchFeedback::Instance() );
                
                if ( feedback )
                    {
                    feedback->InstantFeedback( iAdapter, ETouchFeedbackBasic, 
                		                       ETouchFeedbackVibra,
                		                       aPointerEvent );
                    }
#endif
                node->SetStateL( XnPropertyNames::style::common::KActive );
                }                      
            }
        
        // Remove focus
        engine->AppUiAdapter().HideFocus();                                           
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DoDrawL
// Leaving draw function.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DoDrawL( const TRect& aRect, CWindowGc& aGc ) const
    {
    CXnNode& node = iComponent->Node()->Node();
        
    DrawBordersL( aRect, node, aGc );

    const_cast< CXnControlAdapterImpl* >( this )->DrawBackgroundDataL( aRect, node, aGc );
    
    if ( iAdapter->IsFocused() )
        {
        DrawFocusAppearance( node, aGc );
        }
    
    // Draw plus sign for empty plugins in edit mode.
    if( node.UiEngine()->EditMode()->EditState() )
        {
        const_cast< CXnControlAdapterImpl* >( this )->DrawPlusSign( node, aGc );
        }
    
    if ( iAnimation )
        {
        TRect rect = iComponent->Node()->Rect();
        iAnimation->Render( aGc, rect );
        }

    if ( iBorderBitmap )
        {
        DrawBorderImagesL(
            aRect, node, aGc, iBorderBitmap,
            iBorderBitmapDividerTop, iBorderBitmapDividerRight,
            iBorderBitmapDividerBottom, iBorderBitmapDividerLeft );
        }

    DrawContentImageL( aGc );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawBackgroundDataL
// Draws the background (color and image)
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawBackgroundDataL(
    const TRect& aRect,
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    if( aNode.UiEngine()->EditMode()->EditState() )
        {
        DrawEditModeBgData( aNode, aGc );
        return;
        }

    if ( !iBackgroundBitmap && iBackgroundMask )
        // mask, but no bitmap: draw masked color (if available)
        {
        DrawTransparentColorL( aNode, aGc, iBackgroundMask );
        }
    else if ( !iBackgroundBitmap || !iBackgroundMask )
        // no mask or no bitmap: draw color (if available)
        {
        DrawBackgroundColorL( aRect, aNode, aGc );
        }

    CXnProperty* colorProperty( aNode.BackgroundColorL() );
    DrawBackgroundSkinL( aNode, iBackgroundMask, aGc, colorProperty );

    if ( !iBackgroundBitmap )
        {
        return;
        }

    CXnProperty* pathProperty( aNode.BackgroundImageL() );

    if ( pathProperty && pathProperty->StringValue() != KNullDesC8 )
        {
        if ( iBackgroundMask )
            {
            // draw transparent background image
            DrawBackgroundImageL( aRect, aNode, aGc, iBackgroundBitmap, iBackgroundMask );
            }
        else
            {
            // draw background image without mask
            DrawBackgroundImageL( aRect, aNode, aGc, iBackgroundBitmap, NULL );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawEditModeBgDataL
// 
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawEditModeBgData(
    CXnNode& aNode,
    CWindowGc& aGc )
    {
    // Draw rastered background for plugins    
    const TDesC8& widgetType = aNode.DomNode()->Name();
    if( widgetType == XnPropertyNames::KPlugin )
        {
        TRect rect = aNode.PaddingRect();
        DrawBackgroundSkin( KAknsIIDQgnHomeEditBgWidget, aGc, rect );
        }    
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawPlusSign
// 
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawPlusSign( CXnNode& aNode, CWindowGc& aGc )
    {
    const TDesC8& widgetType = aNode.DomNode()->Name();
    if( widgetType == XnPropertyNames::KPlugin )
        {
        CXnPluginData* data( aNode.UiEngine()->ViewManager()->ActiveViewData().Plugin( &aNode ) );
        if( data && !data->Occupied() ) // Empty widget
            {
            // Draw + -icon
            // Make rect as 50% of the widget's height.
            // It needs to be square in order to keep aspect ratio.
            TRect rect = aNode.PaddingRect();
            TInt w = rect.Width();
            TInt h = rect.Height();
            rect.Shrink( ( w - h * 0.5 ) * 0.5, h * 0.25 );            
            DrawBackgroundSkin( KAknsIIDQgnHomeAdd, aGc, rect );
            }
        }    
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawTransparentColorL
// Draws masked background by RGB color.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawTransparentColorL(
    CXnNode& aNode,
    CWindowGc& aGc,
    CFbsBitmap* aMask )
    {
    CXnProperty* colorProperty( aNode.BackgroundColorL() );

    if ( colorProperty )
        {
        // check value type
        CXnDomPropertyValue* value =
            static_cast< CXnDomPropertyValue* >(
                colorProperty->Property()->PropertyValueList().Item( 0 ) );

        if ( value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor )
            {
	        TRect paddingRect = aNode.PaddingRect();    
            aMask->SetDisplayMode( EGray256 );

            aGc.SetBrushColor( NULL );
            aGc.SetBrushStyle( CGraphicsContext::ENullBrush );

            if ( !iScaledTransparentColor )
                {
                CFbsBitmap* bitmap( NULL );
                CFbsBitmap* mask( NULL );

                mask = new ( ELeave ) CFbsBitmap;
                CleanupStack::PushL( mask );

                mask->Create( paddingRect.Size(), EGray256 );

                CXnUtils::ScaleBitmapL(
                    TRect( 0, 0, paddingRect.Width(), paddingRect.Height() ),
                    mask, aMask );

                CXnDomPropertyValue* value =
                    static_cast< CXnDomPropertyValue* >(
                        colorProperty->Property()->PropertyValueList().Item( 0 ) );

                TRgb rgb( value->RgbColorValueL() );

                bitmap = CreateBitmapFromColorL( paddingRect.Size(), rgb );
                CleanupStack::PushL( bitmap );

                iScaledTransparentColor = CGulIcon::NewL( bitmap, mask );

                CleanupStack::Pop( 2 );
                }

            aGc.DrawBitmapMasked( paddingRect,
                iScaledTransparentColor->Bitmap(),
                TRect( TPoint( 0, 0 ), paddingRect.Size() ),
                iScaledTransparentColor->Mask(),
                EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawBackgroundSkinL
// Draws a skin item to the given rect
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawBackgroundSkin(const TAknsItemID& aSkinId, 
    CWindowGc& aGc, TRect aRect)
    {            
    TRect shrunkRect = aRect;

    shrunkRect.Shrink(
        KSkinGfxInnerRectShrink,
        KSkinGfxInnerRectShrink );
    
    AknsDrawUtils::DrawFrame( AknsUtils::SkinInstance(), aGc, 
        aRect, shrunkRect, aSkinId, KAknsIIDDefault );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawBackgroundSkinL
// Draws a skin item to the padding rect
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawBackgroundSkinL(CXnNode& aNode, 
    CFbsBitmap* aMask, CWindowGc& aGc, CXnProperty* aBgColor,
    TRect aRect)
    {
    if( aRect == TRect::EUninitialized )
        {
        aRect = aNode.PaddingRect();
        }
    
    CXnProperty* colorProperty( aBgColor );
    if ( colorProperty )
        {
        HBufC* skinID = colorProperty->StringValueL();
        CleanupStack::PushL( skinID );

        if ( skinID->Length() != 0 )
            {
            TAknsItemID itemID;
            StripQuotes( skinID );

            if ( CXnUtils::ResolveSkinItemIDL( *skinID, itemID ) )
                {
                if ( aMask ) // If there is a mask, draw masked.
                    {
                    TRect paddingRect = aRect;
                    TRect outerRect = TRect( paddingRect.Size() );
                    TRect innerRect = outerRect;
                    innerRect.Shrink(
                        KSkinGfxInnerRectShrink,
                        KSkinGfxInnerRectShrink );

                    if ( !iScaledBackgroundSkin )
                        {
                        CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
                        CleanupStack::PushL( bitmap );

                        CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
                        CleanupStack::PushL( mask );

                        bitmap->Create( outerRect.Size(), aGc.Device()->DisplayMode() );

                        CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bitmap );
                        CleanupStack::PushL( device );

                        CBitmapContext* bc( NULL );

                        User::LeaveIfError( device->CreateBitmapContext( bc ) );
                        CleanupStack::PushL( bc );

                        CAknsFrameBackgroundControlContext* frameContext =
                            CAknsFrameBackgroundControlContext::NewL(
                            itemID, outerRect, innerRect, EFalse );
                        CleanupStack::PushL( frameContext );

                        AknsDrawUtils::Background( AknsUtils::SkinInstance(),
                            frameContext, NULL,
                            static_cast< CWindowGc& >( *bc ),
                            outerRect, KAknsDrawParamNoClearUnderImage );

                        aMask->SetDisplayMode( EGray256 );

                        aGc.SetBrushColor( NULL );
                        aGc.SetBrushStyle( CGraphicsContext::ENullBrush );

                        mask->Create( paddingRect.Size(), EGray256 );

                        CXnUtils::ScaleBitmapL( outerRect, mask, aMask );

                        CleanupStack::PopAndDestroy( 3 );

                        iScaledBackgroundSkin = CGulIcon::NewL( bitmap, mask );

                        CleanupStack::Pop( 2 );
                        }

                    CFbsBitmap* bitmap( iScaledBackgroundSkin->Bitmap() );
                    CFbsBitmap* mask( iScaledBackgroundSkin->Mask() );

                    aGc.BitBltMasked( paddingRect.iTl, bitmap, outerRect, mask, ETrue );
                    }
                else // draw background skin graphics without mask
                    {
	                TRect paddingRect = aRect;
                    TRect shrunkRect = paddingRect;

                    shrunkRect.Shrink(
                        KSkinGfxInnerRectShrink,
                        KSkinGfxInnerRectShrink );

                    CAknsFrameBackgroundControlContext* frameContext =
                        CAknsFrameBackgroundControlContext::NewL(
                        itemID, paddingRect, shrunkRect, EFalse );

                    CleanupStack::PushL( frameContext );

                    AknsDrawUtils::Background( AknsUtils::SkinInstance(),
                        frameContext,
                        NULL,
                        aGc,
                        paddingRect,
                        KAknsDrawParamNoClearUnderImage );

                    CleanupStack::PopAndDestroy( frameContext );
                    }
                }
            }

        CleanupStack::PopAndDestroy( skinID );
        }
    }

// -----------------------------------------------------------------------------
// DrawBackgroundImageL
// Draws the background image
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawBackgroundImageL(
    const TRect& aRect,
    CXnNode& aNode,
    CWindowGc& aGc,
    CFbsBitmap* aBitmap,
    CFbsBitmap* aMask )
    {
    if ( !aBitmap )
        {
        return;
        }

    TRect rect = aNode.PaddingRect();
    TBool scaleImage = EFalse;

    if (!GetBackgroundSizeFromPropertyL( rect, aNode, aBitmap, scaleImage ) )
        {
        return;
        }
    if ( AknIconUtils::IsMifIcon( aBitmap ) )
        {
        if ( scaleImage )
            {
            AknIconUtils::SetSize( aBitmap, rect.Size(), EAspectRatioNotPreserved );
            }
        else
            {
            AknIconUtils::SetSize( aBitmap, rect.Size(), EAspectRatioPreserved );
            }
        }
    TRect imageRect = rect;
    rect = aNode.PaddingRect();
    TBool heightSet = EFalse;
    TBool backgroundPosFound = EFalse;
    TRAP_IGNORE( backgroundPosFound = GetBackgroundPositionFromPropertyL(
        aNode, scaleImage, rect, imageRect, aBitmap, heightSet ); );
    if ( !backgroundPosFound )
        {
        return;
        }
    if ( !heightSet )
        {
        rect.Move( 0, rect.Height() / 2 );
        if ( scaleImage )
            {
            rect.Move( 0, -imageRect.Height() / 2 );
            }
        else
            {
            rect.Move( 0, -aBitmap->SizeInPixels().iHeight / 2 );
            }
        }
    TRect tmpRect = aNode.PaddingRect();
    LimitRectToPaddingRect( aNode, rect );
    TBool repeatX = EFalse;
    TBool repeatY = EFalse;
    TBool spaceX = EFalse;
    TBool spaceY = EFalse;
    TRAP_IGNORE( GetBackroundRepeatValuesL( aNode, repeatX, repeatY, spaceX, spaceY ) );
    TRect bitmapRect = TRect(
        TPoint( 0, 0 ),
        TPoint( aBitmap->SizeInPixels().iWidth, aBitmap->SizeInPixels().iHeight ) );
    TInt xRepeatCount = 0;
    TInt yRepeatCount = 0;
    TRect paddingRect = aNode.PaddingRect();
    if ( scaleImage )
        {
        if ( imageRect.Width() && imageRect.Height() )
            {
            xRepeatCount = paddingRect.Width() / imageRect.Width();
            yRepeatCount = paddingRect.Height() / imageRect.Height();
            }
        }
    else
        {
        TSize size = aBitmap->SizeInPixels();
        if ( size.iWidth == 0 || size.iHeight == 0 )
            {
            return;
            }
        xRepeatCount = paddingRect.Width() / size.iWidth;
        yRepeatCount = paddingRect.Height() / size.iHeight;
        }

    if ( xRepeatCount == 0 && yRepeatCount == 0 ||
         !repeatX && !repeatY && !spaceX && !spaceY )
        {
        TRect newRect = TRect(
            rect.iTl,
            TPoint(
                rect.iTl.iX + imageRect.Width(),
                rect.iTl.iY + imageRect.Height() ) );
        aGc.SetClippingRect( aRect );

        if ( aMask )
            {
            aMask->SetDisplayMode( EGray256 );

            aGc.SetBrushColor( NULL );
            aGc.SetBrushStyle( CGraphicsContext::ENullBrush );

            if ( !iScaledBackgroundImage )
                {
                CFbsBitmap* mask( NULL );

                mask = new ( ELeave ) CFbsBitmap;
                CleanupStack::PushL( mask );

                mask->Create( bitmapRect.Size(), EGray256 );

                CXnUtils::ScaleBitmapL(
                    TRect(
                        0,
                        0,
                        bitmapRect.Width(),
                        bitmapRect.Height() ),
                    mask,
                    aMask );

                iScaledBackgroundImage = mask;

                CleanupStack::Pop( mask );
                }

            aGc.DrawBitmapMasked( newRect, aBitmap, bitmapRect,
                iScaledBackgroundImage, EFalse );
            }
        else
            {
            TRect newRect = TRect(
                rect.iTl,
                TPoint(
                    rect.iTl.iX + imageRect.Width(),
                    rect.iTl.iY + imageRect.Height() ) );
            aGc.SetClippingRect( aRect );

            aGc.DrawBitmap( newRect, aBitmap, bitmapRect );
            }

        aGc.CancelClippingRect();

        return;
        }

    DrawRepeatedBackgroundImage(
        aGc,
        aNode,
        rect,
        imageRect,
        bitmapRect,
        aBitmap,
        repeatX,
        repeatY,
        spaceX,
        spaceY,
        xRepeatCount,
        yRepeatCount );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::Draw
// Draws the control
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::Draw( const TRect& aRect ) const
    {
    if ( iBlank )
        {
        return;
        }

    CWindowGc& gc = iAdapter->SystemGc();
    //gc.SetClippingRect(aRect);
#ifdef _XN_DEBUG_
    CXnComponent* comp = Component();
    CXnNode& node = comp->Node()->Node();
    TRect contentRect = node.Rect();
    TRect marginRect = node.MarginRect();
    TRect paddingRect = node.PaddingRect();

    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetPenColor( KRgbBlue );
    gc.DrawRect( marginRect );
    gc.SetPenColor( KRgbGray );
    gc.DrawRect( paddingRect );
    gc.SetPenColor( KRgbRed );
    gc.DrawRect( contentRect );
#endif
    TRAP_IGNORE( DoDrawL( aRect, gc ));
    //gc.CancelClippingRect();
    RPointerArray< CXnControlAdapter > sortedArray;
    TBool needsSorting = EFalse;
    for ( TInt count = iChildren.Count() - 1; count >= 0; --count )
        {
        needsSorting = ETrue;
        CXnControlAdapter* child = iChildren[count];
        TRAP_IGNORE( InsertChildToSortedArrayL( sortedArray, child ) );
        }
    if ( !needsSorting )
        {
        return;
        }
    SwapChildArrays( iChildren, sortedArray );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::Draw
// Draws the control
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::Draw( const TRect& aRect, CWindowGc& aGc ) const
    {
    if ( iBlank )
        {
        return;
        }

    aGc.SetClippingRect( aRect );
#ifdef _XN_DEBUG_
    CXnComponent* comp = Component();
    CXnNode& node = comp->Node()->Node();
    TRect contentRect = node.Rect();
    TRect marginRect = node.MarginRect();
    TRect paddingRect = node.PaddingRect();
    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    aGc.SetPenColor( KRgbBlue );
    aGc.DrawRect( marginRect );
    aGc.SetPenColor( KRgbGray );
    aGc.DrawRect( paddingRect );
    aGc.SetPenColor( KRgbRed );
    aGc.DrawRect( contentRect );
#endif
    TRAP_IGNORE( DoDrawL( aRect, aGc ) );
    aGc.CancelClippingRect();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::ComponentControl
// Return a child control by index
// -----------------------------------------------------------------------------
//
CCoeControl* CXnControlAdapterImpl::ComponentControl( TInt aIndex ) const
    {
    return iChildren[aIndex];
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::CountComponentControls
// Return count of children
// -----------------------------------------------------------------------------
//
TInt CXnControlAdapterImpl::CountComponentControls() const
    {
    return iChildren.Count();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::AppendChildL
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::AppendChildL(CXnControlAdapter& aChild, CXnNode& aNode)
    {
    if( !aChild.OwnsWindow() )
        {
        SetControlContext( aChild, aNode );
        if( iChildren.Append( &aChild ) != KErrNone )
            {
            User::Leave( KXnErrAppendingChildToControlAdapterFailed );
            }   
        aChild.SetContainerWindowL( *iAdapter );
        }
    }


// -----------------------------------------------------------------------------
// ConvertHslToRgb
// Converts a HSL color to RGB color
// -----------------------------------------------------------------------------
//
static TRgb ConvertHslToRgb(
    TInt aHue, TInt aSaturation, TInt aLightness )
    {
    TRgb rgb( 0, 0, 0 );
    TReal h( aHue );
    h /= 360;
    TReal s( aSaturation );
    s /= 100;
    TReal l( aLightness );
    l /= 100;

    if ( 0 == aSaturation )
        {
        rgb.SetRed( TInt( l * 255 ) );
        rgb.SetGreen( TInt( l * 255 ) );
        rgb.SetBlue( TInt( l * 255 ) );
        }
    else
        {
        TReal m1( 0 );
        TReal m2( 0 );

        // l<=0.5: PUT l*(s+1) IN m2
        if ( aLightness < 0.5 )
            {
            m2 = l * ( s + 1.0 );
            }
        // PUT l+s-l*s IN m2
        else
            {
            m2 = l + s - ( l * s );
            }

        // PUT l*2-m2 IN m1
        m1 = l * 2.0 - m2;

        TReal helperH( h );
        TReal helperM1( m1 );
        TReal helperM2( m2 );

        // calculate Red
        helperH = h + ( 1.0 / 3.0 );

        // IF h<0: PUT h+1 IN h
        if ( helperH < 0 )
            {
            helperH += 1.0;
            }

        // IF h>1: PUT h-1 IN h
        else if ( helperH > 1 )
            {
            helperH -= 1.0;
            }
        else
            {
            }

        TReal val( 0 );

        // IF h*6<1: RETURN m1+(m2-m1)*h*6
        if ( ( helperH * 6 ) < 1 )
            {
            val = ( 255 * ( helperM1 + ( helperM2 - helperM1 ) * helperH * 6.0 ) );
            }

        // IF h*2<1: RETURN m2 )
        else if ( ( helperH * 2.0 ) < 1.0 )
            {
            val = ( 255.0 * helperM2 );
            }

        // IF h*3<2: RETURN m1+(m2-m1)*(2/3-h)*6
        else if ( ( helperH * 3.0 ) < 2.0 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) *
                            ( 2.0 / 3.0 - helperH ) * 6.0 ) );
            }
        else
            {
            val = 255.0 * helperM1;
            }
        TInt tmpVal = TInt( val + 0.5 );
        rgb.SetRed( ( tmpVal >= 0) ? tmpVal : -tmpVal );

        // calculate Green
        // PUT hue.to.rgb(m1, m2, h    ) IN g
        helperH = h;
        helperM1 = m1;
        helperM2 = m2;

        // IF h<0: PUT h+1 IN h
        if ( helperH < 0 )
            {
            helperH += 1.0;
            }
        // IF h>1: PUT h-1 IN h
        else if ( helperH > 1 )
            {
            helperH -= 1.0;
            }
        else
            {
            }

        // IF h*6<1: RETURN m1+(m2-m1)*h*6
        if ( ( helperH * 6 ) < 1 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) * helperH * 6.0 ) );
            }
        // IF h*2<1: RETURN m2
        else if ( ( helperH * 2 ) < 1 )
            {
            val = ( 255.0 * helperM2 );
            }
        // IF h*3<2: RETURN m1+(m2-m1)*(2/3-h)*6
        else if ( ( helperH * 3 ) < 2 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) *
                            ( 2.0 / 3.0 - helperH ) * 6.0 ) );
            }
        else
            {
            val = 255.0 * helperM1;
            }

        tmpVal = TInt( val + 0.5 );
        rgb.SetGreen( ( tmpVal >= 0) ? tmpVal : tmpVal );

        // calculate Blue
        // PUT hue.to.rgb(m1, m2, h-1/3) IN b
        helperH = h - ( 1.0 / 3.0 );
        helperM1 = m1;
        helperM2 = m2;

        // IF h<0: PUT h+1 IN h
        if ( helperH < 0 )
            {
            helperH += 1.0;
            }
        // IF h>1: PUT h-1 IN h
        else if ( helperH > 1 )
            {
            helperH -= 1.0;
            }
        else
            {
            }

        // IF h*6<1: RETURN m1+(m2-m1)*h*6
        if ( ( helperH * 6 ) < 1 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) * helperH * 6.0 ) );
            }
        // IF h*2<1: RETURN m2
        else if ( ( helperH * 2 ) < 1 )
            {
            val = ( 255.0 * helperM2 );
            }
        // IF h*3<2: RETURN m1+(m2-m1)*(2/3-h)*6
        else if ( ( helperH * 3 ) < 2 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) *
                            ( 2.0 / 3.0 - helperH ) * 6.0 ) );
            }
        else
            {
            val = 255.0 * helperM1;
            }

        tmpVal = TInt( val + 0.5 );
        rgb.SetBlue( ( tmpVal >= 0) ? tmpVal : -tmpVal );

        }
    return rgb;
    }

// -----------------------------------------------------------------------------
// ConvertRgbToHsl
// Converts a RGB color to HSL color
// -----------------------------------------------------------------------------
//
static void ConvertRgbToHsl(
    TRgb aRGB,
    TUint& aHue,
    TUint& aSaturation,
    TUint& aLightness )
    {
    TReal red = static_cast< TReal >( aRGB.Red() ) / 255.0;
    TReal green = static_cast< TReal >( aRGB.Green() ) / 255.0;
    TReal blue = static_cast< TReal >( aRGB.Blue() ) / 255.0;
    TReal maxColor = ( red > green && red > blue ) ?
        red : ( green > red && green > blue ) ? green : blue;
    TReal minColor = ( red <= green && red <= blue ) ?
        red : ( green <= red && green <= blue ) ? green : blue;
    TReal lightness = ( maxColor + minColor ) / 2.0;
    if ( maxColor == minColor )
        {
        aSaturation = 0;
        aHue = 0;
        aLightness = static_cast< TInt >( 100.0 * lightness + 0.5 );
        return;
        }

    if ( lightness < 0.5 )
        {
        TReal saturation = ( maxColor - minColor ) / ( maxColor + minColor );
        aSaturation = static_cast< TInt >( 100.0 * saturation + 0.5 );
        }
    else
        {
        TReal saturation = ( maxColor - minColor ) / ( 2.0 - maxColor - minColor );
        aSaturation = static_cast< TInt >( 100.0 * saturation + 0.5 );
        }

    if ( red == maxColor )
        {
        TReal hue = ( green - blue ) / ( maxColor - minColor );
        TInt tmp = static_cast< TInt >( 60.0 * hue + 0.5 );
        aHue = ( tmp >= 0 ) ? tmp : tmp + 360;
        }
    else if ( green == maxColor )
        {
        TReal hue = 2.0 + ( ( blue - red ) / ( maxColor - minColor ) );
        TInt tmp = static_cast< TInt >( 60.0 * hue + 0.5 );
        aHue = ( tmp >= 0 ) ? tmp : tmp + 360;
        }
    else
        {
        TReal hue = 4.0 + ( ( red - green ) / ( maxColor - minColor ) );
        TInt tmp = static_cast< TInt >( 60.0 * hue + 0.5 );
        aHue = ( tmp >= 0 ) ? tmp : tmp + 360;
        }

    aLightness = static_cast< TInt >( 100.0 * lightness + 0.5 );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::IsDrawingAllowed
// Checks whether drawing is allowed
// -----------------------------------------------------------------------------
//
TBool CXnControlAdapterImpl::IsDrawingAllowed() const
    {
    TBool retVal = EFalse;
    CXnNode& node = Component()->Node()->Node();
    CXnProperty* propVisibility = NULL;
    TRAP_IGNORE( propVisibility = node.VisibilityL() );
    if ( propVisibility )
        {
        const TDesC8& visibility = propVisibility->StringValue();
        if ( visibility != KNullDesC8 )
            {
            if ( visibility == XnPropertyNames::style::common::visibility::KHidden )
                {
                return EFalse;
                }
            else
                {
                retVal = ETrue;
                }
            }
        }
    else
        {
        // default value for visibility
        retVal = ETrue;
        }
    CXnProperty* propDisplay = NULL;
    TRAP_IGNORE( propDisplay = node.DisplayL() );
    if ( propDisplay )
        {
        const TDesC8& display = propDisplay->StringValue();
        if ( display != KNullDesC8 )
            {
            if ( display == XnPropertyNames::style::common::display::KNone )
                {
                return EFalse;
                }
            else
                {
                retVal = ETrue;
                }
            }
        }
    if ( node.IsDropped() )
        {
        return EFalse;
        }
    else
        {
        retVal = ETrue;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::ContentBitmaps
// Gets content bitmaps
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::ContentBitmaps(
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask )
    {
    aBitmap = iContentBitmap;
    aMask = iContentMask;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::SetContentBitmaps
// Sets content bitmaps
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::SetContentBitmaps(
    CFbsBitmap* aBitmap,
    CFbsBitmap* aMask )
    {
    delete iContentBitmap;
    iContentBitmap = NULL;

    delete iContentMask;
    iContentMask = NULL;

    delete iScaledContentBitmap;
    iScaledContentBitmap = NULL;

    delete iScaledContentMask;
    iScaledContentMask = NULL;

    iContentBitmap = aBitmap;
    iContentMask = aMask;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::SetContentBitmapsL
// Sets content bitmaps
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::SetContentBitmapsL( const TDesC& aBitmapUrl,
    const TDesC& aMaskUrl )
    {
    delete iContentBitmap;
    iContentBitmap = NULL;

    delete iContentMask;
    iContentMask = NULL;

    delete iScaledContentBitmap;
    iScaledContentBitmap = NULL;

    delete iScaledContentMask;
    iScaledContentMask = NULL;

    CXnNode& node( iNode.Node() );
    CXnUiEngine* engine( node.UiEngine() );

    TRect rect( node.Rect() );

    GetBitmapL( *engine, node, aBitmapUrl, iContentBitmapIndex,
        iContentBitmap, iContentMask, iIconProvider, rect, FsSession() );
    if ( !iContentBitmap )
        {
        CXnProperty* prop( node.GetPropertyL(
            XnPropertyNames::appearance::common::KFallBackImage ) );

        if ( prop )
            {
            HBufC* fallBackImage( prop->StringValueL( ) );

            CleanupStack::PushL( fallBackImage );

            if ( *fallBackImage != KNullDesC )
                {
                StripQuotes( fallBackImage );

                GetBitmapL( *engine, node, *fallBackImage, iContentBitmapIndex,
                    iContentBitmap, iContentMask, iIconProvider, rect, FsSession() );
                }

            CleanupStack::PopAndDestroy( fallBackImage );
            }
        }

    if ( !iContentMask )
        {
        if ( aMaskUrl != KNullDesC )
            {
            CFbsBitmap* bitmapMask( NULL );

            GetBitmapL( *engine, node, aMaskUrl, iContentMaskIndex,
                iContentMask, bitmapMask, iIconProvider, rect, FsSession() );

            delete bitmapMask;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawContentImage
// Draws content image
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawContentImage() const
    {
    TRAP_IGNORE( DrawContentImageL( *iGc ) );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DrawContentImage
// Draws content image
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DrawContentImageL( CWindowGc& aGc ) const
    {
    if ( !iContentBitmap )
        {
        return;
        }

    TRect rect = iComponent->Node()->Node().Rect();

    // Check if the node has a property "preserve image aspect ratio".
    TBool preserveAspectRatio = EFalse;
    CXnNode& currentNode = iComponent->Node()->Node();

    CXnProperty* aspectProperty = currentNode.GetPropertyL(
        XnPropertyNames::image::KS60AspectRatio );
    if ( aspectProperty )
        {
        const TDesC8& value = aspectProperty->StringValue();

        if ( value.CompareF( XnPropertyNames::image::KPreserve ) == 0 )
            {
            preserveAspectRatio = ETrue;
            }
        }

    TRect bitmapRect = rect;
    bitmapRect.Move( -rect.iTl.iX, -rect.iTl.iY );

    if ( AknIconUtils::IsMifIcon( iContentBitmap ) )
        {
        if ( preserveAspectRatio )
            {
            AknIconUtils::SetSize(
                iContentBitmap, bitmapRect.Size(), EAspectRatioPreserved );
            }
        else
            {
            AknIconUtils::SetSize(
                iContentBitmap, bitmapRect.Size(), EAspectRatioNotPreserved );
            }
        //Calculate new point to start draw in order to center bitmap to drawing area
        rect.iTl.iY +=
            ( rect.Height() - iContentBitmap->SizeInPixels().iHeight ) / 2;
        rect.iTl.iX +=
            ( rect.Width() - iContentBitmap->SizeInPixels().iWidth ) / 2;

        if ( iContentMask )
            {
            // Based on the avkon's assumtion that mask is always inverted, the
            // value of aInvertMask parameter is set to 'ETrue'
            aGc.BitBltMasked( rect.iTl, iContentBitmap, bitmapRect, iContentMask,
                ETrue );
            }
        else
            {
            aGc.BitBlt( rect.iTl, iContentBitmap, bitmapRect );
            }
        }
    else
        {
        if ( !iScaledContentBitmap )
            {
            TBool forceFallBack = CXnUtils::DoesScaleBitmapUseFallBack(
                iContentBitmap );

            TRAPD( err, CreateScaledBitmapL( bitmapRect,
                iScaledContentBitmap,
                iContentBitmap,
                preserveAspectRatio,
                forceFallBack ) );
            if ( err )
                {
                // return if CreateScaledBitmapL() leaves. This prevents the
                // drawing of the original content bitmap which is wrong size.
                return;
                }
            }
        if ( iContentMask && !iScaledContentMask )
            {
            TBool forceFallBack = CXnUtils::DoesScaleBitmapUseFallBack(
                iContentMask );

            TRAPD( err, CreateScaledBitmapL( bitmapRect,
                iScaledContentMask,
                iContentMask,
                preserveAspectRatio,
                forceFallBack ) );
            if ( err )
                {
                // return if CreateScaledBitmapL() leaves. This prevents the
                // drawing of the original content mask which is wrong size
                // (and may distort the image).
                return;
                }
            }

        CFbsBitmap* bitmap( 0 );
        CFbsBitmap* mask( 0 );

        if ( iScaledContentBitmap )
            {
            bitmap = iScaledContentBitmap;
            }
        else
            {
            bitmap = iContentBitmap;
            }

        if ( iScaledContentMask )
            {
            mask = iScaledContentMask;
            }
        else
            {
            mask = iContentMask;
            }

        if ( bitmap && mask )
            {
            //Calculate new point to start draw in order to center bitmap to
            // drawing area
            rect.iTl.iY +=
                ( rect.Height() - bitmap->SizeInPixels().iHeight ) / 2;
            rect.iTl.iX +=
                ( rect.Width() - bitmap->SizeInPixels().iWidth ) / 2;

            // Based on the avkon's assumtion that mask is always inverted, the
            // value of aInvertMask parameter is set to 'ETrue'
            aGc.BitBltMasked( rect.iTl, bitmap, bitmapRect, mask, ETrue );
            }
        else if ( bitmap )
            {
            //Calculate new point to start draw in order to center bitmap to
            // drawing area
            rect.iTl.iY +=
                ( rect.Height() - bitmap->SizeInPixels().iHeight ) / 2;
            rect.iTl.iX +=
                ( rect.Width() - bitmap->SizeInPixels().iWidth ) / 2;

            aGc.BitBlt( rect.iTl, bitmap );
            }
        }
   }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::SizeChanged
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::SizeChanged()
    {
    delete iScaledContentBitmap;
    iScaledContentBitmap = NULL;

    delete iScaledContentMask;
    iScaledContentMask = NULL;

    delete iScaledTransparentColor;
    iScaledTransparentColor = NULL;

    delete iScaledBackgroundSkin;
    iScaledBackgroundSkin = NULL;

    delete iScaledBackgroundImage;
    iScaledBackgroundImage = NULL;

    TRAP_IGNORE( InitializeBackgroundAndBorderBitmapsL() );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::SkinChanged
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::SkinChanged()
    {
    delete iScaledContentBitmap;
    iScaledContentBitmap = NULL;

    delete iScaledContentMask;
    iScaledContentMask = NULL;

    delete iScaledTransparentColor;
    iScaledTransparentColor = NULL;

    delete iScaledBackgroundSkin;
    iScaledBackgroundSkin = NULL;

    delete iScaledBackgroundImage;
    iScaledBackgroundImage = NULL;

    TRAP_IGNORE( InitializeBackgroundAndBorderBitmapsL() );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::FocusChangedL
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::FocusChangedL( TBool aFocused )
    {
    CXnNode& node( iNode.Node() );

    CXnUiEngine* engine( node.UiEngine() );

    CXnNode* menubar( engine->MenuBarNode() );

    if ( engine->IsEditMode() || !node.IsLayoutCapable() || !menubar )
        {
        return;
        }

    XnMenuInterface::MXnMenuInterface* menuIf( NULL );

    XnComponentInterface::MakeInterfaceL( menuIf, menubar->AppIfL() );

    if ( !menuIf )
        {
        return;
        }

    CXnViewManager* manager( engine->ViewManager() );

    const TDesC8& ns( node.Namespace() );

    CXnPluginData* data( manager->ActiveViewData().Plugin( ns ) );

    if ( !data )
        {
        return;
        }

    CXnNode* plugin( data->Node()->LayoutNode() );

    CXnNode* extension( NULL );

    if ( plugin == engine->ActiveView() )
        {
        // Pretend that this node is extension
        extension = &node;
        }
    else
        {
        RPointerArray< CXnNode >& children( plugin->Children() );

        for ( TInt i = 0; i < children.Count(); i++ )
            {
            CXnNode* child( children[i] );

            const TDesC8& type( child->Type()->Type() );

            if ( type == XnPropertyNames::menu::KMenuExtension )
                {
                extension = child;
                break;
                }
            }
        }

    if ( !extension )
        {
        // No extension available     
        return;
        }
         
    // Find proper softkey from extension
    RPointerArray< CXnNode >& children( extension->Children() );
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNode* child( children[i] );

        const TDesC8& type( child->Type()->Type() );

        if ( type == XnPropertyNames::softkey::KNodeName )
            {

            CXnProperty* prop( child->GetPropertyL( 
                    XnPropertyNames::softkey::KTypeAttribute ) ); 

            if ( prop )
                {
                const TDesC8& type( prop->StringValue() );

                XnMenuInterface::MXnMenuInterface::TSoftKeyPosition pos;

                if ( type == XnPropertyNames::softkey::type::KLeft )
                    {
                    pos = XnMenuInterface::MXnMenuInterface::ELeft;
                    }
                else if ( type == XnPropertyNames::softkey::type::KMiddle )
                    {
                    pos = XnMenuInterface::MXnMenuInterface::ECenter;
                    }
                else if ( type == XnPropertyNames::softkey::type::KRight )
                    {
                    pos = XnMenuInterface::MXnMenuInterface::ERight;
                    }
                else
                    {
                    // Declaration error
                    __ASSERT_DEBUG( EFalse, Panic( EXnInvalidState ) );
                    break;
                    }

                if ( aFocused )
                    {
                    // Set softkey
                    menuIf->SetSoftKeyL( &child->PluginIfL(), pos );
                    }
                else
                    {
                    // Remove softkey
                    menuIf->SetSoftKeyL( NULL, pos );
                    }
                
                // This is not a correct way to do dirty setting,
                // but here in case of menubar it is acceptable
                engine->AddDirtyNodeL( menubar, XnDirtyLevel::ERender );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::InitializeBackgroundandBorderBitmapsL
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::InitializeBackgroundAndBorderBitmapsL()
    {
    CXnNode& node( iNode.Node() );
    CXnUiEngine* engine( node.UiEngine() );

    CXnProperty* backgroundImageProperty( node.BackgroundImageL() );

    if ( backgroundImageProperty )
        {
        delete iBackgroundBitmap;
        iBackgroundBitmap = NULL;

        delete iBackgroundMask;
        iBackgroundMask = NULL;

        delete iScaledTransparentColor;
        iScaledTransparentColor = NULL;

        delete iScaledBackgroundSkin;
        iScaledBackgroundSkin = NULL;

        delete iScaledBackgroundImage;
        iScaledBackgroundImage = NULL;

        InitializeBackgroundBitmapL( *engine, node, iBackgroundBitmapIndex,
            iBackgroundBitmap, iBackgroundMask, iIconProvider, FsSession() );

        HBufC* bgPath( GetBackgroundImagePathLC( node ) );

        if ( *bgPath != KNullDesC )
            {
            iAnimIDResolved = CXnUtils::ResolveSkinItemIDL( *bgPath, iAnimIID );
            }

        CleanupStack::PopAndDestroy( bgPath );
        }

    CXnProperty* borderImageProperty( node.BorderImageL() );

    if ( borderImageProperty )
        {
        delete iBorderBitmap;
        iBorderBitmap = NULL;

        iBorderBitmap = InitializeBorderBitmapL(
            *engine, node,
            iBorderBitmapIndex, iBorderBitmapDividerTop,
            iBorderBitmapDividerRight,
            iBorderBitmapDividerBottom, iBorderBitmapDividerLeft,
            iIconProvider, FsSession() );
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::ConstructL
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::ConstructL(
    CXnNodePluginIf& aNode,
    CXnControlAdapter& aAdapter,
    CWindowGc& aGc )
    {
    iGc = &aGc;

    iAdapter = &aAdapter;

    if ( CreateGestureHelperL( aNode.Node() ) )
        {
        iGestureHelper = CXnGestureHelper::NewL( aNode.Node() );
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::LoadBitmap
// Load a bitmap from the server. Ownership is transferred.
// -----------------------------------------------------------------------------
//
CFbsBitmap* CXnControlAdapterImpl::LoadBitmap( const TDesC& aBitmapUrl )
    {
    CXnNode& node( iNode.Node() );
    CXnUiEngine* engine( node.UiEngine() );

    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );

    TRect rect( node.Rect() );

    TInt index( 0 );

    TRAP_IGNORE( GetBitmapL(
        *engine, node,
        aBitmapUrl, index,
        bitmap, mask,
        iIconProvider, rect, FsSession() ) );

    delete mask;

    return bitmap;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::GetBitmapAndMask
// Load a bitmap and mask from the server. Ownership is transferred.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::GetBitmapAndMask(
    const TDesC& aBitmapUrl,
    const TDesC& aMaskUrl,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask )
    {
    CXnNode& node( iNode.Node() );
    CXnUiEngine* engine( node.UiEngine() );

    TRect rect( node.Rect() );

    TInt index( 0 );

    TRAP_IGNORE( GetBitmapL(
        *engine, node,
        aBitmapUrl, index,
        aBitmap, aMask,
        iIconProvider, rect, FsSession() ) );

    if ( !aMask )
        {
        if ( aMaskUrl != KNullDesC )
            {
            CFbsBitmap* bitmapMask( NULL );

            TRAP_IGNORE( GetBitmapL(
                *engine, node,
                aMaskUrl, index,
                aMask, bitmapMask,
                iIconProvider, rect, FsSession() ) );

            delete bitmapMask;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::AnimFrameReady
// Callback of the animated skin background.
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::AnimFrameReady( TInt aError, TInt /* aAnimId */ )
    {
    if ( !aError )
        {
        iAdapter->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::CreateAnimationL
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::CreateAnimationL( /* TDesC& aPath */ )
    {
    if ( iAnimation )
        {
        delete iAnimation;
        iAnimation = NULL;
        }

    iAnimation = CAknsEffectAnim::NewL( this );
    TBool ok = iAnimation->ConstructFromSkinL( iAnimIID );
    if ( !ok )
        {
        delete iAnimation;
        iAnimation = NULL;
        User::Leave( KErrNotSupported );
        }

    CXnNode& node = iComponent->Node()->Node();
    if ( !node.IsLaidOut() )
        {
        // Animation is created before layout of nodes has been calculated
        node.UiEngine()->LayoutUIL( &node );
        }
    TRect rect( node.Rect() );

    iAnimation->BeginConfigInputLayersL( rect.Size(), ETrue );

    if ( iAnimation->InputRgbGc() )
        {
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        TRect mainPaneRect( mainPane.Size() );

        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        MAknsControlContext* cc = NULL;
        const TAknsItemID* id = &KAknsIIDNone;
        if ( AknsUtils::AvkonSkinEnabled() )
            {
            cc = CAknsListBoxBackgroundControlContext::NewL(
                KAknsIIDQsnBgAreaMainListGene,
                mainPaneRect,
                EFalse,
                *id,
                mainPaneRect );

            AknsDrawUtils::DrawBackground( AknsUtils::SkinInstance(),
                cc,
                NULL,
                *iAnimation->InputRgbGc(),
                TPoint(),
                rect,
                KAknsDrawParamBottomLevelRGBOnly );
            }
        }

    iAnimation->EndConfigInputLayersL();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::StartHighlightAnimation()
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::StartHighlightAnimation( TBool aAnimRestart )
    {
    TBool focused = EFalse;

    // No need to start animation again if already running
    if ( iAnimation )
        {
        if ( iAnimation->State() == EAknsAnimStateRunning && !aAnimRestart )
            {
            return;
            }
        }
    // Check control's focus
    if ( iComponent )
        {
        CXnNodePluginIf* pluginIf = iComponent->Node();
        if ( pluginIf )
            {
            focused = pluginIf->IsFocusedState();
            }
        else
            {
            return;
            }
        }

    // If the node is not focused or no Anim ID, return.
    if ( !focused || !iAnimIDResolved )
        {
        return;
        }

    // Restart animation
    if ( aAnimRestart )
        {
        if ( iAnimation )
            {
            iAnimation->Stop();
            delete iAnimation;
            iAnimation = NULL;

            TRAPD( error, CreateAnimationL() );

            if ( !error )
                {
                iAnimation->Start();
                return;
                }
            }
        }

    // Node is focused. Try to start animation.
    if ( iAnimation )
        {
        iAnimation->Start();
        }
    else
        {
        TRAPD( error, CreateAnimationL() );

        if ( !error )
            {
            iAnimation->Start();
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::StopHighlightAnimation()
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::StopHighlightAnimation()
    {
    if ( iAnimation )
        {
        // Stop animation and delete.
        iAnimation->Stop();
        delete iAnimation;
        iAnimation = NULL;

        iAdapter->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::MeasureAdaptiveContentL
// -----------------------------------------------------------------------------
//
TSize CXnControlAdapterImpl::MeasureAdaptiveContentL(
    const TSize& /* aAvailableSize */ )
    {
    // By default the size is 0
    return TSize( 0, 0 );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::SetBlank
//
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::SetBlank( TBool aBlank )
    {
    iBlank = aBlank;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::DoHandlePropertyChangeL( CXnProperty* /*aProperty*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::UpdateBackgroundImageL
// -----------------------------------------------------------------------------
//
CXnControlAdapterImpl::TPropertyChangeResponse CXnControlAdapterImpl::UpdateBackgroundImageL(
    CXnProperty* aProperty )
    {
    if ( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );

        if ( name == XnPropertyNames::appearance::common::KBackGroundImage )
            {
            delete iBackgroundBitmap;
            iBackgroundBitmap = NULL;

            delete iBackgroundMask;
            iBackgroundMask = NULL;

            delete iScaledTransparentColor;
            iScaledTransparentColor = NULL;

            delete iScaledBackgroundSkin;
            iScaledBackgroundSkin = NULL;

            delete iScaledBackgroundImage;
            iScaledBackgroundImage = NULL;

            InitializeBackgroundBitmapL( *( iNode.Node().UiEngine() ), iNode.Node(),
                iBackgroundBitmapIndex, iBackgroundBitmap, iBackgroundMask,
                iIconProvider, FsSession());

            return EPropertyChangeConsumed;
            }
        }

    return EPropertyChangeNotConsumed;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapterImpl::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::HandleScreenDeviceChangedL()
    {
    }

// -----------------------------------------------------------------------------
// CreateBitmapFromColorL
// Creates a bitmap object with the given size and fill color
// -----------------------------------------------------------------------------
//
static CFbsBitmap* CreateBitmapFromColorL( TSize aSize, TRgb aColor )
    {
    CFbsBitmap* newBitmap = new ( ELeave ) CFbsBitmap;
    newBitmap->Create( aSize, EColor16M );
    CleanupStack::PushL( newBitmap );

    CFbsBitmapDevice* bitmapDev = CFbsBitmapDevice::NewL( newBitmap );
    CleanupStack::PushL( bitmapDev );

    CFbsBitGc* bc = NULL;
    User::LeaveIfError( bitmapDev->CreateContext( bc ) );
    CleanupStack::PushL( bc );

    bc->SetBrushColor( aColor );
    bc->Clear(); // area is filled with the brush color

    CleanupStack::PopAndDestroy( bc );
    CleanupStack::PopAndDestroy( bitmapDev );
    CleanupStack::Pop( newBitmap );
    return newBitmap;
    }

// -----------------------------------------------------------------------------
// PassEventToGestureHelperL
// Forwards event to gesturehelper
// -----------------------------------------------------------------------------
//
TBool CXnControlAdapterImpl::PassEventToGestureHelperL(
    const TPointerEvent& aPointerEvent )
    {
    TBool ret( EFalse );
    
    CXnNode* node( &iNode.Node() );
    CXnUiEngine* engine( node->UiEngine() );

    if ( iGestureHelper && iGestureHelper->Owner() == node )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {                               
            // Set default destination                     
            iGestureHelper->SetDestination( iGestureHelper->Owner() );
            
            RPointerArray< CXnPluginData >& plugins( 
                engine->ViewManager()->ActiveViewData().PluginData() );
            
            for ( TInt i = 0; i < plugins.Count(); i++ )
                {
                CXnPluginData* plugin( plugins[i] );
                                           
                if ( plugin->Occupied() )
                    {
                    CXnNode* widget( plugin->Node()->LayoutNode() );
                    
                    if ( widget->MarginRect().Contains( aPointerEvent.iPosition ) )
                        {
                        // Resolve swipe destination
                        CXnProperty* prop( widget->GetPropertyL( 
                            XnPropertyNames::common::KSwipeDestination ) );
                        
                        if ( prop )
                            {
                            const TDesC8& value( prop->StringValue() );
                                
                            if( value == XnPropertyNames::KWidget )
                                {
                                // Set widget as destination    
                                iGestureHelper->SetDestination( widget );
                                }
                            else if( value == XnPropertyNames::KNone )
                                {
                                // Set no destination    
                                iGestureHelper->SetDestination( NULL );
                                }
                            }
                        
                        break;
                        }
                    }
                }
            }

        TXnGestureCode result( iGestureHelper->HandlePointerEventL( aPointerEvent ) );
        
        const TDesC8* swipe( NULL );
    
        if ( result == EGestureSwipeLeft )
            {
            swipe = &XnPropertyNames::action::trigger::name::swipe::direction::KLeft;
            }
        else if ( result == EGestureSwipeRight )
            {
            swipe = &XnPropertyNames::action::trigger::name::swipe::direction::KRight;
            }

        if ( swipe )
            {
            CXnNode* destination( iGestureHelper->Destination() );
            
            if ( destination )
                {
                if ( destination == iGestureHelper->Owner() )
                    {
                    CancelFocusRefusalL( *engine );
                    }
                
                // Remove focus
                engine->AppUiAdapter().HideFocus();

                CXnNode* trigger( BuildSwipeTriggerNodeLC( *engine, *swipe ) );
                destination->ReportXuikonEventL( *trigger );
                CleanupStack::PopAndDestroy( trigger );
                       
                // If needed we can call here for example HandleSwipeL()
                
                // Consume this event
                ret = ETrue;
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::RemoveChildAdapters()
    {
    iChildren.Reset();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
RPointerArray< CXnControlAdapter >& CXnControlAdapterImpl::ChildAdapters()
    {
    return iChildren;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
RFs& CXnControlAdapterImpl::FsSession()
    {
    return iAdapter->ControlEnv()->FsSession();
    }


// ============================= TIconProvider ===============================

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CXnControlAdapterImpl::TIconProvider::TIconProvider( RFile& aFile ) :
    iFile( aFile )
    {
    }
        
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CXnControlAdapterImpl::TIconProvider::~TIconProvider()
    {
    iFile.Close();
    }

// -----------------------------------------------------------------------------
// Retreive Icon handle
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::TIconProvider::RetrieveIconFileHandleL( 
    RFile& aFile, const TIconFileType /*aType*/)
    {
    aFile.Duplicate( iFile );
    }
        
// -----------------------------------------------------------------------------
// Finish
// -----------------------------------------------------------------------------
//
void CXnControlAdapterImpl::TIconProvider::Finished()
    {
    iFile.Close();
    delete this;
    }

