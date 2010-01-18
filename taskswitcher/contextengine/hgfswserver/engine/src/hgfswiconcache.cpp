/*
 * ============================================================================
 *  Name        : hgfswiconcache.cpp
 *  Part of     : Hg Teleport
 *  Description : Icon cache
 *  Version     : %version: sa1spcx1#5 %
 *
 *  Copyright 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 */

#include "hgfswiconcache.h"
#include <fbs.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <avkon.mbg>

// size for the created app icons
const TInt KAppIconWidth = 96;
const TInt KAppIconHeight = 96;

// --------------------------------------------------------------------------
// CHgFswIconCache::NewL
// --------------------------------------------------------------------------
//
CHgFswIconCache* CHgFswIconCache::NewL()
    {
    CHgFswIconCache* self = new (ELeave) CHgFswIconCache;
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHgFswIconCache::CHgFswIconCache
// --------------------------------------------------------------------------
//
CHgFswIconCache::CHgFswIconCache()
    {
    }

// --------------------------------------------------------------------------
// CHgFswIconCache::~CHgFswIconCache
// --------------------------------------------------------------------------
//
CHgFswIconCache::~CHgFswIconCache( )
    {
    THashMapIter<TInt, SHgBitmapPair> iter( iAppIcons );
    while ( const TInt* key = iter.NextKey() )
        {
        SHgBitmapPair* value = iter.CurrentValue();
        delete value->iBitmap;
        delete value->iMask;
        }
    iAppIcons.Close();
    delete iDefaultIcon;
    }

// --------------------------------------------------------------------------
// CHgFswIconCache::ConstructL
// --------------------------------------------------------------------------
//
void CHgFswIconCache::ConstructL()
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
// CHgFswIconCache::GetAppIconL
// --------------------------------------------------------------------------
//
void CHgFswIconCache::GetAppIconL( const TUid& aAppUid,
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
// CHgFswIconCache::GetIconL
// --------------------------------------------------------------------------
//
void CHgFswIconCache::GetIconL( const TUid& aAppUid,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = aMask = 0;
    SHgBitmapPair* bp = iAppIcons.Find( aAppUid.iUid );
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
        iAppIcons.InsertL( aAppUid.iUid, SHgBitmapPair( aBitmap, aMask ) );
        }
    }

// --------------------------------------------------------------------------
// CHgFswIconCache::DefaultIcon
// --------------------------------------------------------------------------
//
void CHgFswIconCache::DefaultIcon( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = iDefaultIcon->Bitmap();
    aMask = iDefaultIcon->Mask();
    }


// end of file
