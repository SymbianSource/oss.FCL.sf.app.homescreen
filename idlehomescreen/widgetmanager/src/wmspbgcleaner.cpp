/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies)..
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
* statuspane transparency handling implementation
*
*/

#include <e32base.h>
#include <w32std.h>
#include <AknUtils.h>

#include "wmspbgcleaner.h"

// CONSTANTS

// ---------------------------------------------------------
// CWmSpBgCleaner::NewL
// ---------------------------------------------------------
//
CWmSpBgCleaner* CWmSpBgCleaner::NewL()
    {
    CWmSpBgCleaner* self = CWmSpBgCleaner::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::NewLC
// ---------------------------------------------------------
//
CWmSpBgCleaner* CWmSpBgCleaner::NewLC()
    {
    CWmSpBgCleaner* self = new ( ELeave ) CWmSpBgCleaner();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::CWmSpBgCleaner
// ---------------------------------------------------------
//
CWmSpBgCleaner::CWmSpBgCleaner()
    {
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::ConstructL
// ---------------------------------------------------------
//
void CWmSpBgCleaner::ConstructL()
    {
    CreateWindowL();
    
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EStatusPane, statusPaneRect );
    
    SetRect( statusPaneRect );
    SetNonFocusing();

    Window().SetOrdinalPosition( -1 );
    Window().SetBackgroundColor( TRgb::Color16MA(0) );
    Window().SetVisible( ETrue );    
    
    ActivateL();
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::~CWmSpBgCleaner
// ---------------------------------------------------------
//
CWmSpBgCleaner::~CWmSpBgCleaner()
    {
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::SizeChanged
// ---------------------------------------------------------
//
void CWmSpBgCleaner::SizeChanged()
    {
    CCoeControl::SizeChanged();
    DrawDeferred();
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::Draw
// ---------------------------------------------------------
//
void CWmSpBgCleaner::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    TRgb color( TRgb::Color16MA(0) );
    gc.SetPenColor( color );
    gc.SetBrushColor( color );    
    gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    gc.SetPenStyle(CGraphicsContext::ESolidPen );
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush );
    gc.Clear();
    }

// ---------------------------------------------------------
// CWmSpBgCleaner::HandleResourceChange
// ---------------------------------------------------------
//
void CWmSpBgCleaner::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    
    if ( KEikDynamicLayoutVariantSwitch == aType )
        {
        TRect statusPaneRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EStatusPane, statusPaneRect );
        SetRect( statusPaneRect );
        }
    }

// End of file
