/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for xuikon clock face.
*
*/


// SYSTEM INCLUDE FILES
#include <e32math.h>
#include <bitdev.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <AknBidiTextUtils.h>
#include <AknLayoutFont.h>
#include <AknUtils.h>
#include <gdi.h>

// USER INCLUDE FILES
#include "xnnodepluginif.h"
#include "xnclockadapter.h"
#include "xntext.h"

#include "xnclockface.h"


// LOCAL CONSTANTS AND MACROS
_LIT( KAmPmFormat, "%B" );
_LIT( KTimeFormat, "%J%:1%T" );

_LIT( KClockFont, "EAknLogicalFontSecondaryFont" );
_LIT( KAmpmFont, "EAknLogicalFontSecondaryFont" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::CXnClockFaceDigital
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnClockFaceDigital::CXnClockFaceDigital()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnClockFaceDigital::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnClockFaceDigital* CXnClockFaceDigital::NewL()
    {
    CXnClockFaceDigital* self =
        new ( ELeave ) CXnClockFaceDigital();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CXnClockFaceDigital::~CXnClockFaceDigital()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::DrawL
// -----------------------------------------------------------------------------
//
void CXnClockFaceDigital::DrawL( CXnClockAdapter& aAdapter, CWindowGc& aGc, 
        CXnNodePluginIf& aNode, const TTime& aTime, CXnNodePluginIf* aAmpm )
    {    
    TBuf< KMaxTimeFormatSpec > time;
    aTime.FormatL( time, KTimeFormat() );

    AknTextUtils::LanguageSpecificNumberConversion( time );

    const CAknLayoutFont* clockFont( CreateFontL( aAdapter, aNode, EClock ) );
    if ( !clockFont )
        {
        return;
        }

    const TRgb& color( CreateColorL( aAdapter, aNode, EClock) );

    const TInt deltaHeight( aNode.Rect().Height() - clockFont->TextPaneHeight() );
    TInt offset( clockFont->TextPaneTopToBaseline() + deltaHeight / 2 );
    
    CGraphicsContext::TTextAlign align = aAdapter.GetTextAlignL( aNode );

    aGc.SetPenColor( color );
    aGc.UseFont( clockFont );
    aGc.DrawText( time, aNode.Rect(), offset, align );
    aGc.DiscardFont();

    if( TLocale().TimeFormat() == ETime12 && aAmpm )
        {
        DrawAmpmL( aAdapter, aGc, aTime, *aAmpm );
        }
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::DrawAmpmL
// -----------------------------------------------------------------------------
//
void CXnClockFaceDigital::DrawAmpmL( CXnClockAdapter& aAdapter,
                                     CWindowGc& aGc,
                                     const TTime& aTime,
                                     CXnNodePluginIf& aAmpm )
    {
    TBuf< KMaxTimeFormatSpec > ampm;
    aTime.FormatL( ampm, KAmPmFormat() );
    ampm.Trim();
    
    const CAknLayoutFont* ampmFont( CreateFontL( aAdapter, aAmpm, EAmpm ) );
    if ( !ampmFont )
        {
        return;
        }
    
    const TRgb& color( CreateColorL( aAdapter, aAmpm, EAmpm) );
    
    const TInt deltaHeight( aAmpm.Rect().Height() - ampmFont->TextPaneHeight() );
    TInt offset( ampmFont->TextPaneTopToBaseline() + deltaHeight / 2 );
    
    CGraphicsContext::TTextAlign align = aAdapter.GetTextAlignL( aAmpm );

    aGc.SetPenColor( color );
    aGc.UseFont( ampmFont );
    aGc.DrawText( ampm, aAmpm.Rect(), offset, align );
    aGc.DiscardFont();
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::CreateFontL
// -----------------------------------------------------------------------------
//
const CAknLayoutFont* CXnClockFaceDigital::CreateFontL( CXnClockAdapter& aAdapter,
        CXnNodePluginIf& aNode,
        TXnClockFontType aFontType)
    {    
    if ( aFontType == EClock )
        {
        if ( !iClockFont )
            {
            aAdapter.CreateFontL( aNode, KClockFont, iClockFont );
            }
        return CAknLayoutFont::AsCAknLayoutFontOrNull( iClockFont );
        }
    else
        {
        if ( !iAmpmFont )
            {
            aAdapter.CreateFontL( aNode, KAmpmFont, iAmpmFont );
            }
        return CAknLayoutFont::AsCAknLayoutFontOrNull( iAmpmFont );
        }
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::CreateColorL
// -----------------------------------------------------------------------------
//
const TRgb& CXnClockFaceDigital::CreateColorL( CXnClockAdapter& aAdapter,
        CXnNodePluginIf& aNode,
        TXnClockFontType aFontType )
    {
    if ( aFontType == EClock )
        {
        if ( !iIsFaceColorSet )
            {
            aAdapter.CreateColorL( aNode, iFaceColor );
            iIsFaceColorSet = ETrue;
            }
        return iFaceColor;
        }
    else
        {
        if ( !iIsAmpmColorSet )
            {
            aAdapter.CreateColorL( aNode, iAmpmColor );
            iIsAmpmColorSet = ETrue;
            }
        return iAmpmColor;
        }
    }

// -----------------------------------------------------------------------------
// CXnClockFaceDigital::ResetFont
// -----------------------------------------------------------------------------
//
void CXnClockFaceDigital::ResetFont()
    {
    iClockFont = iAmpmFont = NULL;
    iIsFaceColorSet = iIsAmpmColorSet = EFalse;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::CXnClockFaceAnalog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnClockFaceAnalog::CXnClockFaceAnalog()     
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnClockFaceAnalog::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnClockFaceAnalog* CXnClockFaceAnalog::NewL()    
    {
    CXnClockFaceAnalog* self =
        new( ELeave ) CXnClockFaceAnalog();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CXnClockFaceAnalog::~CXnClockFaceAnalog()
    {
    }

// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::DrawL
// -----------------------------------------------------------------------------
//
void CXnClockFaceAnalog::DrawL( CXnClockAdapter& /*aAdapter*/, CWindowGc& aGc, 
        CXnNodePluginIf& aNode, const TTime& aTime, CXnNodePluginIf* /*aAmpm*/ )
    {    
    TSize faceSize( aNode.Rect().Size() );
    
    TDateTime dateTime( aTime.DateTime() );
    
    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
 
    // 1-2: Face number 1
    CFbsBitmap* skinBmp( NULL );
    CFbsBitmap* skinMask( NULL );

    AknsUtils::GetCachedMaskedBitmap( skin,
                                      KAknsIIDQsnHomeClockAnalogueFace,
                                      skinBmp,
                                      skinMask );

    // If there is no bitmap, don't draw
    if( !skinBmp )
        {
        return;
        }

    User::LeaveIfError( AknIconUtils::SetSize( skinBmp, faceSize ) );

    if( skinMask )
        {
        User::LeaveIfError( AknIconUtils::SetSize( skinMask, faceSize ) );
        
        aGc.BitBltMasked( aNode.Rect().iTl,
                          skinBmp,
                          TRect( TPoint( 0, 0 ), skinBmp->SizeInPixels() ),
                          skinMask,
                          ETrue );
        }
    else
        {
        aGc.BitBlt( aNode.Rect().iTl, skinBmp );
        }

    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    aGc.SetPenStyle( CGraphicsContext::ESolidPen );
  
    DrawHandsL( aGc, aNode.Rect(), dateTime );
    }
            
// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::DrawHands
// -----------------------------------------------------------------------------
//
void CXnClockFaceAnalog::DrawHandsL( CWindowGc& aGc,
                                     const TRect& aRect,
                                     const TDateTime& aDateTime )                                    
    {
    TRect rect( aRect );
            
    CFbsBitmap* hrbitmap( NULL );
    CFbsBitmap* hrmask( NULL );

    CFbsBitmap* minbitmap( NULL );
    CFbsBitmap* minmask( NULL );

    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );

    AknsUtils::GetCachedMaskedBitmap(
            skin, KAknsIIDQsnHomeClockAnaloguePointerHour, 
            hrbitmap, hrmask );

    AknsUtils::GetCachedMaskedBitmap(
            skin, KAknsIIDQsnHomeClockAnaloguePointerMinute,
            minbitmap, minmask );

    User::LeaveIfError( AknIconUtils::SetSizeAndRotation(        
                            hrbitmap,
                            rect.Size(),
                            EAspectRatioPreserved,
                            aDateTime.Hour() * 30 + aDateTime.Minute() / 2 ) );

    aGc.BitBltMasked( rect.iTl,
                      hrbitmap,
                      TRect( TPoint( 0, 0 ), hrbitmap->SizeInPixels() ),
                      hrmask,
                      EFalse );


    User::LeaveIfError( AknIconUtils::SetSizeAndRotation(
                            minbitmap,
                            rect.Size(),
                            EAspectRatioPreserved,
                            aDateTime.Minute() * 6 ) );

    aGc.BitBltMasked( rect.iTl,
                      minbitmap,
                      TRect( TPoint( 0, 0 ), minbitmap->SizeInPixels() ),
                      minmask,
                      EFalse );
    }

// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::ResetFont
// -----------------------------------------------------------------------------
//
void CXnClockFaceAnalog::ResetFont()
    {
    }
    
//  End of File
