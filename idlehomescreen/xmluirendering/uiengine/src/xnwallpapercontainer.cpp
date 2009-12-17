/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wallpaper view's container.
*
*/



// INCLUDE FILES
#include "xnwallpapercontainer.h"

// SYSTEM INCLUDE FILES
#include <aknappui.h>
#include <AknsSkinInstance.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperContainer::CXnWallpaperContainer()
    {
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperContainer* CXnWallpaperContainer::NewL()
    {
    CXnWallpaperContainer* self = new (ELeave) CXnWallpaperContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnWallpaperContainer::ConstructL()
    {
    CreateWindowL();
    TRect rect = iAvkonAppUi->ClientRect();
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
            KAknsIIDQsnBgScreen, rect, EFalse );
    SetRect( rect );
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperContainer::~CXnWallpaperContainer()
    {
    if ( iBgContext )
        {
        delete iBgContext;
        iBgContext = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CXnWallpaperContainer::Draw
// -----------------------------------------------------------------------------
//
void CXnWallpaperContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
     
    // draw background skin.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, iBgContext, this, gc, aRect );
    }

// -----------------------------------------------------------------------------
// CXnWallpaperContainer::SizeChanged
// -----------------------------------------------------------------------------
//
void CXnWallpaperContainer::SizeChanged()
    {
    if ( iBgContext )
        {
        iBgContext->SetRect(Rect());
        TRect rect = Rect();
        }
    }
 
// -----------------------------------------------------------------------------
// CXnWallpaperContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CXnWallpaperContainer::HandleResourceChange(TInt aType)
    {
    TRect rect;
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
    CCoeControl::HandleResourceChange(aType);
    }

//  End of File
