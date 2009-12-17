/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Utility functions of Xuikon.
*
*/

#ifndef C_XNUTILS_H
#define C_XNUTILS_H

// System includes
#include <e32base.h>
#include <gdi.h>
#include <gulalign.h>

// Forward declarations
class CXnNodePluginIf;
class CFont;
class CXnNode;
class CXnResource;
class TAknsItemID;

// Class declaration
/**
 * Utility functions for Xuikon.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 v3.1
 */
class CXnUtils : public CBase
    {
public:
    /**
    * Creates the font according the parameters found from the given node.
    * Avkon font ids or font-family names can be used. If font family is
    * missing or defined as normal, system normal font is created.
    *
    * @since S60 v3.1
    * @param aNode Text element node.
    * @param aFont Font to be created.
    * @param aReleaseFont Whether the font needs to be released or not.
    */
    IMPORT_C static void CreateFontL(
        CXnNodePluginIf& aNode,
        CFont*& aFont,
        TInt& aReleaseFont );

    /**
    * Creates the font according the given parameters.
    * Avkon font ids or font-family names can be used. If font family is
    * missing or defined as normal, system normal font is created.
    *
    * @since S60 v3.2
    * @param aFontName Font name.
    * @param aFontSizeInPixels Defines a font size.
    * @param aFontStyle Defines the style, i.e. italic, bold, etc.
    * @param aFont Font to be created.
    * @param aReleaseFont Whether the font needs to be released or not.
    */
    IMPORT_C static void CreateFontL(
        const TDesC& aFontName,
        TInt aFontSizeInPixels,
        TFontStyle aFontStyle,
        CFont*& aFont,
        TInt& aReleaseFont );

    /**
    * Collapses the white spaces and special charachters from the given string
    * according to properties found from the given node.
    *
    * @since S60 v3.1
    * @param aNode Text element node.
    * @param aDes Descriptor to be modified.
    */
    IMPORT_C static void CollapseWhiteSpace(
        CXnNodePluginIf& aNode, TDes &aDes );

    /**
    * Returns the text alignment according to properties found from the given
    * node.
    *
    * @since S60 v3.1
    * @param aNode Text element node.
    * @return The text alignment.
    */
    IMPORT_C static TGulAlignmentValue TextAlignment( CXnNodePluginIf& aNode );

    /**
    * Scales the source bitmap into a given rectangle of the target bitmap.
    * Source and target bitmaps must have the same display mode.
    *
    * @since S60 v3.1
    * @param aTrgRect A rectangle area where the source bitmap is scaled.
    * @param aTrgBitmap A newly created bitmap.
    * @param aSrcBitmap An original bitmap.
    */
    IMPORT_C static void ScaleBitmapL(
        const TRect& aTrgRect,
        CFbsBitmap* aTrgBitmap,
        CFbsBitmap* aSrcBitmap );

    /**
     * Strips quotes from a descriptor string.
     *
     * @param aString A string.
     */
    IMPORT_C static void StripQuotes( HBufC*& aString );

    /**
     * Resolves a skin item ID from a descriptor string. The skin item ID is
     * stored in aItemID
     *
     * @param aSkinId A skin ID as a string descriptor.
     * @param aItemID An item ID to which the values are set from the
     *        descriptor.
     */
    IMPORT_C static TBool ResolveSkinItemIDL(
        const TDesC& aSkinId,
        TAknsItemID& aItemID );

    /**
     * Resolves a skin item ID and an integer index from a descriptor string.
     * The skin item ID is stored in aItemID and the index is stored in aIndex.
     *
     * @param aSkinId A skin ID as a descriptor string.
     * @param aItemID An item ID to which the values are set from the
     *        descriptor.
     * @param aIndex An integer index.
     */
    IMPORT_C static TBool ResolveSkinItemIDL(
        const TDesC& aSkinId,
        TAknsItemID& aItemID,
        TInt& aIndex );

    /**
     * Resolves RGB colour value from a string.
     *
     * @param aValue Resolved RGB value.
     * @param aData String from which RGB value is resolved.
     */
    IMPORT_C static TBool GetRgbValue( TRgb& aValue, const TDesC8& aData );

    /**
     * Converts HSL value to RGB and returns the RGB value.
     *
     * @param aHue Hue.
     * @param aSaturation Saturation.
     * @param aLightness Lightness.
     */
    IMPORT_C static TRgb ConvertHslToRgb(
        TInt aHue,
        TInt aSaturation,
        TInt aLightness );

    /**
     * Handles power save mode
     *
     * @param aNode A node where to start iterate.
     * @param aEvent Mode event.
     * @param aEnter enter or exit the mode.
     * @return ETrue if entered to the mode, EFalse if exited
     */
    IMPORT_C static TBool HandlePowerSaveModeL(
        CXnNode* aNode,
        TBool aEnter,
        TInt aEvent );

    /**
     * Finds a CXnResource object by the resource file name.
     *
     * @param aArray UIEngine's resource array
     * @param aResourceFileName resource file name
     * @param aBitmapIndex bitmap index
     */
    IMPORT_C static CXnResource* FindResource(
        CArrayPtrSeg< CXnResource >& aArray,
        const TDesC& aResourceFileName,
        TInt& aBitmapIndex );

    /**
     * @internal
     */
    static TBool DoesScaleBitmapUseFallBack( CFbsBitmap* aSrcBitmap );

    /**
     * @internal
     */
    static void ScaleBitmapExtL(
        const TRect& aTrgRect,
        CFbsBitmap* aTrgBitmap,
        CFbsBitmap* aSrcBitmap,
        TBool aForceFallBack );
    };

#endif
