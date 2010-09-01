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
* Description:  Icon cache
 *
*/


#include "tsfswiconcache.h"
#include <fbs.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <avkon.mbg>

// size for the created app icons
const TInt KAppIconWidth = 300;
const TInt KAppIconHeight = 300;

// --------------------------------------------------------------------------
// CTsFswIconCache::NewL
// --------------------------------------------------------------------------
//
CTsFswIconCache* CTsFswIconCache::NewL()
    {
    CTsFswIconCache* self = new (ELeave) CTsFswIconCache;
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswIconCache::CTsFswIconCache
// --------------------------------------------------------------------------
//
CTsFswIconCache::CTsFswIconCache()
    {
    }

// --------------------------------------------------------------------------
// CTsFswIconCache::~CTsFswIconCache
// --------------------------------------------------------------------------
//
CTsFswIconCache::~CTsFswIconCache( )
    {
    THashMapIter<TInt, STsBitmapPair> iter( iAppIcons );
    while ( const TInt* key = iter.NextKey() )
        {
        STsBitmapPair* value = iter.CurrentValue();
        delete value->iBitmap;
        delete value->iMask;
        }
    iAppIcons.Close();
    delete iDefaultIcon;
    }

// --------------------------------------------------------------------------
// CTsFswIconCache::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswIconCache::ConstructL()
    {
    iDefaultIcon = AknsUtils::CreateGulIconL( 
        AknsUtils::SkinInstance(),
        KAknsIIDQgnMenuUnknownCxt,
        AknIconUtils::AvkonIconFileName(),
        EMbmAvkonQgn_menu_unknown_cxt,
        EMbmAvkonQgn_menu_unknown_cxt_mask );
    TSize sz( KAppIconWidth, KAppIconHeight );
    AknIconUtils::SetSize( iDefaultIcon->Bitmap(), sz );
    AknIconUtils::SetSize( iDefaultIcon->Mask(), sz );
    }

// --------------------------------------------------------------------------
// CTsFswIconCache::GetAppIconL
// --------------------------------------------------------------------------
//
void CTsFswIconCache::GetAppIconL( const TUid& aAppUid,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = aMask = 0;
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    if ( skinInstance )
        {
        AknsUtils::CreateAppIconLC( skinInstance, aAppUid, EAknsAppIconTypeList,
            aBitmap, aMask );
        // The CreateAppIconLC method leaves both the bitmaps in the cleanup stack.
        // The order in which they are pushed into the stack and types of 
        // the items in the stack are both undefined.
        CleanupStack::Pop( 2 ); // codescanner::cleanup
        TSize sz( KAppIconWidth, KAppIconHeight );
        AknIconUtils::SetSize( aBitmap, sz );
        AknIconUtils::SetSize( aMask, sz );
        }
    }

// --------------------------------------------------------------------------
// CTsFswIconCache::GetIconL
// --------------------------------------------------------------------------
//
void CTsFswIconCache::GetIconL( const TUid& aAppUid,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = aMask = 0;
    STsBitmapPair* bp = iAppIcons.Find( aAppUid.iUid );
    if ( bp )
        {
        // ownership stays with the hash table
        aBitmap = bp->iBitmap;
        aMask = bp->iMask;
        }
    else
        {
        GetAppIconL( aAppUid, aBitmap, aMask ); // ownership of the bitmap is ours
        // insert to hash table and pass ownership
        iAppIcons.InsertL( aAppUid.iUid, STsBitmapPair( aBitmap, aMask ) );
        }
    }

// --------------------------------------------------------------------------
// CTsFswIconCache::DefaultIcon
// --------------------------------------------------------------------------
//
void CTsFswIconCache::DefaultIcon( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = iDefaultIcon->Bitmap();
    aMask = iDefaultIcon->Mask();
    }


// end of file
