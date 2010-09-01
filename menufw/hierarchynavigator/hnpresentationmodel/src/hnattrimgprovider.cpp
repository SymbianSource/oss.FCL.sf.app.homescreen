/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  base class for all image providers
*
*/


#include <AknsSkinInstance.h>
#include <AknIconUtils.h>
#include <AknsUtils.h>
#include "hnattrimgprovider.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDismissableCleaner::TDismissableCleaner( TAny* aToBeDeletedOnLeave )
    : mToBeDeletedOnLeave( aToBeDeletedOnLeave )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TDismissableCleaner::Close()
    {
    delete mToBeDeletedOnLeave;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TDismissableCleaner::Dismiss()
    {
    mToBeDeletedOnLeave = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvider::CHnAttrImgProvider()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvider::~CHnAttrImgProvider()
    {
    if ( iIconHolder )
        {
        iIconHolder->Close();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvider::CreateIconL( CFbsBitmap* aBitmap, 
                                      CFbsBitmap* aMaskBitmap )
    {
    CGulIcon* newIcon = CGulIcon::NewL( aBitmap, aMaskBitmap );
    CleanupStack::PushL( newIcon );
    CHnIconHolder *newIconHolder = new (ELeave) CHnIconHolder();
    newIconHolder->SetGulIcon( newIcon );
    CleanupStack::Pop( newIcon );
    
    if ( iIconHolder )
        {
        iIconHolder->Close();
        }
    iIconHolder = newIconHolder;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CGulIcon* CHnAttrImgProvider::GetIcon( TSize* /*aDesiredIconSize*/ )
    {   
    return iIconHolder ? iIconHolder->GetGulIcon() : NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnIconHolder* CHnAttrImgProvider::GetIconHolder( TSize* /*aDesiredIconSize*/ )
    {
    return iIconHolder;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvider::RemoveLiwObjects()
    {
    }
