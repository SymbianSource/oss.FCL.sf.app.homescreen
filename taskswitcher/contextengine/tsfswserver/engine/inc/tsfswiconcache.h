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


#ifndef TSFSWICONCACHE_H_
#define TSFSWICONCACHE_H_

#include <e32base.h>
#include <e32hashtab.h>

class CFbsBitmap;
class CGulIcon;

/**
 * App icon cache.
 */
NONSHARABLE_CLASS( CTsFswIconCache ) : public CBase
    {
public:

    /**
     * Two phased constructor.
     */
    static CTsFswIconCache* NewL();

    /**
     * Two phased constructor.
     */
    static CTsFswIconCache* NewLC();

    /**
     * Destructor
     */
    ~CTsFswIconCache();

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
    CTsFswIconCache();

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
    struct STsBitmapPair
        {
        STsBitmapPair( CFbsBitmap* aBitmap, CFbsBitmap* aMask )
            : iBitmap( aBitmap ), iMask( aMask ) { }
        CFbsBitmap* iBitmap;
        CFbsBitmap* iMask;
        };
    RHashMap<TInt, STsBitmapPair> iAppIcons;
    
    CGulIcon* iDefaultIcon;
    };

#endif
