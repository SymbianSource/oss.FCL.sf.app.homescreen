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
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockFaceDigital::DrawL( CXnClockAdapter& aAdapter, CWindowGc& aGc, 
    const TRect& aRect, const TTime& aTime )
    {
    TBuf< KMaxTimeFormatSpec > time;
    aTime.FormatL( time, KTimeFormat() );

    TBuf< KMaxTimeFormatSpec > ampm;
    aTime.FormatL( ampm, KAmPmFormat() );

    AknTextUtils::LanguageSpecificNumberConversion( time );

    const CAknLayoutFont* clockFont( 
            aAdapter.FontL( CXnClockAdapter::EDigitalFont ) );

    const CAknLayoutFont* ampmFont( 
            aAdapter.FontL( CXnClockAdapter::EAmPmFont ) );

    const CAknLayoutFont* dateFont( 
            aAdapter.FontL( CXnClockAdapter::EDateFont ) );

    const TRgb& color( aAdapter.TextColorL() );

    CXnNodePluginIf* date( aAdapter.Date() );

    TInt ampmWidth( 0 );

    if( TLocale().TimeFormat() == ETime12 )
        {
        // Measure the full width of the ampm string 
        ampmWidth = AknBidiTextUtils::MeasureTextBoundsWidth( *ampmFont, ampm, 
            CFont::TMeasureTextInput::EFVisualOrder );
        }


    const TInt deltaHeight( aRect.Height() - clockFont->TextPaneHeight() );
    TInt offset( clockFont->TextPaneTopToBaseline() + deltaHeight / 2 );

    if( date )
        {
        // When date string is shown, time string must be lifted up
        offset -= ( dateFont->TextPaneHeight() / 2 );
        }

    // Measure the full width of the time string 
    TInt textWidth( AknBidiTextUtils::MeasureTextBoundsWidth( *clockFont, time, 
        CFont::TMeasureTextInput::EFVisualOrder ) );

    TInt extraWidth( aRect.Width() - textWidth );

    TInt margin( extraWidth / 2 );

    aGc.SetPenColor( color );

    aGc.UseFont( clockFont );

    CGraphicsContext::TTextAlign align;

    TBool mirrored( AknLayoutUtils::LayoutMirrored() );

    if( mirrored )
        {
        align = CGraphicsContext::ERight;
        }
    else
        {
        align = CGraphicsContext::ELeft;
        }

    aGc.DrawText( time, aRect, offset, align, margin );

    aGc.DiscardFont();

    if( TLocale().TimeFormat() == ETime12 )
        {
        TRect ampmRect( aRect );
        TInt ampmHeight( ampmFont->TextPaneHeight() );

        if( mirrored )
            {
            align = CGraphicsContext::ELeft;
            }
        else
            {
            align = CGraphicsContext::ERight;
            }

        ampmRect.iTl.iY += ampmHeight;

        aGc.UseFont( ampmFont );

        aGc.DrawText( ampm, ampmRect, offset, align, margin );

        aGc.DiscardFont();
        }

    if( date )
        {
        const TDesC* dateStr( &KNullDesC() );

        CXnText* textIf( NULL );

        XnComponentInterface::MakeInterfaceL( textIf, date->AppIfL() );

        if( textIf )
            {
            dateStr = textIf->Text();
            }

        // Measure the full width of the time string 
        TInt dateWidth( AknBidiTextUtils::MeasureTextBoundsWidth( *dateFont, *dateStr, 
            CFont::TMeasureTextInput::EFVisualOrder ) );

        TInt width( aRect.Width() - dateWidth );
        TInt margin( width / 2 );

        // Move date string down by text pane height and 5% of rect height
        offset += dateFont->TextPaneHeight() + ( aRect.Height() / 20 );

        aGc.UseFont( dateFont );

        aGc.DrawText( *dateStr, aRect, offset, align, margin );

        aGc.DiscardFont();
        }
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
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockFaceAnalog::DrawL( CXnClockAdapter& /*aAdapter*/, CWindowGc& aGc, 
    const TRect& aRect, const TTime& aTime )
    {
    TSize faceSize( aRect.Size() );
    
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
        
        aGc.BitBltMasked( aRect.iTl,
                          skinBmp,
                          TRect( TPoint( 0, 0 ), skinBmp->SizeInPixels() ),
                          skinMask,
                          ETrue );
        }
    else
        {
        aGc.BitBlt( aRect.iTl, skinBmp );
        }

    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    aGc.SetPenStyle( CGraphicsContext::ESolidPen );
  
    DrawHandsL( aGc, aRect, dateTime );
    }
            
// -----------------------------------------------------------------------------
// CXnClockFaceAnalog::DrawHands
// (other items were commented in a header).
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
    
//  End of File
