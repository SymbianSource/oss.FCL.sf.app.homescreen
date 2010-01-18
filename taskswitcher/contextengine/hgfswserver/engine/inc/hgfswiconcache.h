/*
* ============================================================================
*  Name        : hgfswiconcache.h
*  Part of     : Hg Teleport / Fast Swap module
*  Description : Icon cache
*  Version     : %version: 2 %
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

#ifndef HGFSWICONCACHE_H_
#define HGFSWICONCACHE_H_

#include <e32base.h>
#include <e32hashtab.h>

class CFbsBitmap;
class CGulIcon;

/**
 * App icon cache.
 */
NONSHARABLE_CLASS( CHgFswIconCache ) : public CBase
    {
public:

    /**
     * Two phased constructor.
     */
    static CHgFswIconCache* NewL();

    /**
     * Two phased constructor.
     */
    static CHgFswIconCache* NewLC();

    /**
     * Destructor
     */
    ~CHgFswIconCache();

    /**
     * Gets the bitmaps associated with the given app from the cache.
     * If not found then creates the app icon, stores it to the cache,
     * and returns that.
     * @param   aAppUid application uid
     * @param   aBitmap bitmap ptr, ownership NOT transferred to caller, or NULL
     * @param   aMask   mask ptr, ownership NOT transferred to caller, or NULL
     */
    void GetIconL( const TUid& aAppUid,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

    /**
     * Sets the ptrs to the default icon.
     * Ownership not transferred to caller.
     */
    void DefaultIcon( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

private:
    /**
     * Default constructor.
     */
    CHgFswIconCache();

    /**
     * Second phase construction
     */
    void ConstructL();

    /**
     * Retrieves the bitmap/mask for the icon of the given app.
     * @param   aAppUid application uid
     * @param   aBitmap bitmap ptr, ownership transferred to caller, or NULL
     * @param   aMask   mask ptr, ownership transferred to caller, or NULL
     */
    void GetAppIconL( const TUid& aAppUid,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

private:
    struct SHgBitmapPair
        {
        SHgBitmapPair( CFbsBitmap* aBitmap, CFbsBitmap* aMask )
            : iBitmap( aBitmap ), iMask( aMask ) { }
        CFbsBitmap* iBitmap;
        CFbsBitmap* iMask;
        };
    RHashMap<TInt, SHgBitmapPair> iAppIcons;
    
    CGulIcon* iDefaultIcon;
    };

#endif
