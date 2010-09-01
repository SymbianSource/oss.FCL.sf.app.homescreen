/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef MMEXTBMPICONHOLDER_H
#define MMEXTBMPICONHOLDER_H

#include "hniconholder.h"

class CLiwBitmapBuffer;

/**
 * This is an extended version of @c CHnIconHolder which holds a CGulIcon that does
 * not own the bitmaps and CLiwBitmapBuffer objects that do.
 *
 * Such class was needed because some of the bitmaps for icons are obtained from
 * CLiwBitmapBuffer objects and there is no way to transfer ownership of that
 * bitmaps from them. It is not possible to make copies of that bitmaps either,
 * because some of them are not CFbsBitmaps but CAknBitmaps.
 */
class CHnExtBmpIconHolder: public CHnIconHolder
    {
public:
    /**
     * Sets the icon to be stored in this icon holder along with the bitmap buffers.
     * This method should be called only once. If you want to store another icon
     * simply call Close() on this icon holder and then create a new icon holder
     * to store the new icon.
     *
     * @param aGulIcon Icon to store in this icon holder.
     * @param aBmpBuffer The buffer that owns the primary bitmap.
     * @param aMaskBuffer The buffer that owns the mask bitmap.
     */
    IMPORT_C void SetGulIcon( CGulIcon* aGulIcon, CLiwBitmapBuffer* aBmpBuffer, CLiwBitmapBuffer* aMaskBuffer );

    /**
     * Clean both liw bitmap buffer and liw maskbitmap buffer.
     * Allows set new mask and new bitmap for the icon.
     */
    virtual void CleanBmpBuffer();

    /**
     * Standard C++ virtual destructor.
     */
    virtual ~CHnExtBmpIconHolder();

private: // data

    /**
     * Liw bitmap buffer that owns the primary bitmap for the icon.
     * Co-owns (CLiwBitmapBuffer are ref-counted).
     */
    CLiwBitmapBuffer* iBmpBuffer;

    /**
     * Liw bitmap buffer that owns the mask bitmap for the icon.
     * Co-owns (CLiwBitmapBuffer are ref-counted).
     */
    CLiwBitmapBuffer* iMaskBuffer;
    };

#endif // MMEXTBMPICONHOLDER_H
