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
* statuspane's background cleaner implementation
*
*/

// System includes
#include <e32base.h>
#include <w32std.h>
#include <AknUtils.h>

// User includes
#include "xnspbgcleaner.h"

// CONSTANTS

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CXnSpBgCleaner::NewL
// ---------------------------------------------------------
//
CXnSpBgCleaner* CXnSpBgCleaner::NewL()
    {
    CXnSpBgCleaner* self = CXnSpBgCleaner::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CXnSpBgCleaner::NewLC
// ---------------------------------------------------------
//
CXnSpBgCleaner* CXnSpBgCleaner::NewLC()
    {
    CXnSpBgCleaner* self = new ( ELeave ) CXnSpBgCleaner();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CXnSpBgCleaner::CXnSpBgCleaner
// ---------------------------------------------------------
//
CXnSpBgCleaner::CXnSpBgCleaner()
    {
    }

// ---------------------------------------------------------
// CXnSpBgCleaner::ConstructL
// ---------------------------------------------------------
//
void CXnSpBgCleaner::ConstructL()
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
// CXnSpBgCleaner::~CXnSpBgCleaner
// ---------------------------------------------------------
//
CXnSpBgCleaner::~CXnSpBgCleaner()
    {
    }

// ---------------------------------------------------------
// CXnSpBgCleaner::SizeChanged
// ---------------------------------------------------------
//
void CXnSpBgCleaner::SizeChanged()
    {
    CCoeControl::SizeChanged();
    DrawDeferred();
    }

// ---------------------------------------------------------
// CXnSpBgCleaner::Draw
// ---------------------------------------------------------
//
void CXnSpBgCleaner::Draw( const TRect& /*aRect*/ ) const
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
// CXnSpBgCleaner::HandleResourceChange
// ---------------------------------------------------------
//
void CXnSpBgCleaner::HandleResourceChange( TInt aType )
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
