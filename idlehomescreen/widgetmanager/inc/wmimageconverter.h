/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmImageConverter declaration
*
*/

#ifndef WMIMAGECONVERTER_H
#define WMIMAGECONVERTER_H

//includes
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <AknIconUtils.h> // MAknIconFileProvider

// Forward declarations
class CFbsBitmap;
class TAknsItemID;
class CWmUnitTest;
class MSvgError;

// Class declaration
/**
 * CWmImageConverter
 * Image converter
 */
NONSHARABLE_CLASS ( CWmImageConverter ) : public MAknIconFileProvider
    {
public: //contructors/destructors
    /**
     * Two-phased constructor.
     * @param aObserver observer
     */
    static CWmImageConverter* NewL();    
    
    /** Destructor */
    ~CWmImageConverter();
        
public: // interface methods
    
    /**
     * Parses icon string and prepares logo image.
     * 
     * @param aIconSize wanted size
     * @param aIconStr str containing logo icon
	 * @param aBitmap bitmap to create. Empty if fails
	 * @param aMask mask to create. Empty if fails
	 * @param aForceScale force scaling for non-scalable icons.
     * Supported values:
     * - skin(<major id> <minor id>):mif(<path> <bitmapid> <maskid>)
     * - mif(<path> <bitmapid> <maskid>)
     * - uid(<application uid>)
     * - <file name>.<png/svg>
     * 
     * @return Error code 
     */
    TInt HandleIconString( 
            const TSize& aIconSize,
            const TDesC& aIconStr,
            CFbsBitmap*& aBitmap,
            CFbsBitmap*& aMask,
            TBool aForceScale = EFalse );
    
	/**
     * Parses icon string and resizes given bitmaps if needed.
	 * Errors are ignored and previos size image will be used.
     * 
     * @param aIconSize wanted size
     * @param aIconStr icon string to find out what to resize
	 * @param aBitmap bitmap to resize
	 * @param aMask mask to resize
	 */
    void UpdateImageSize( 
            const TSize& aSize,
            const TDesC& aIconStr, 
            CFbsBitmap& aBitmap, 
            CFbsBitmap& aMask );

    /**
     * Parses icon string. 
     * 
     * @param aIconStr icon string to be parsed
     * @param aItemId fetched skin id
     * @param aBitmapId fetched bitmap id
     * @param aMaskId fetched mask id 
     * @param aFileName fetched file name 
     * 
     * @return ETrue if success with parsing.   
     */
    
    TBool ParseIconString( 
            const TDesC& aIconStr, 
            TAknsItemID& aItemId,
            TInt& aBitmapId, 
            TInt& aMaskId, 
            TDes& aFileName );
    
private:
    CWmImageConverter(); 
    void ConstructL();

private:
    void CheckSvgErrorL( MSvgError* aError );
    void HandleIconStringL( 
            const TSize& aIconSize, 
            const TDesC& aIconStr, 
            TBool aForceScale );
    
    void CreateIconFromUidL( const TUid& aUid, TBool aForceScale );
    void CreateIconFromSvgL( const TDesC& aFileName );
    void CreateIconFromOtherL( const TDesC& aFileName, TBool aForceScale );
    void CreateSkinOrMifIconL( 
                    const TAknsItemID& aItemId, TInt aBitmapId, 
                    TInt aMaskId, const TDesC& aFileName );
    // resolvers
    TBool ResolveUid( const TDesC& aStr, TUid& aUid );
    TBool ResolveSkinId( const TDesC& aStr, TAknsItemID& aItemId );
    TBool ResolveMifId( const TDesC& aStr, TInt& aBitmapId, 
                        TInt& aMaskId, TDes& aFileName );
    TBool ResolveSkinIdAndMifId( const TDesC& aStr, TAknsItemID& aItemId,
                        TInt& aBitmapId, TInt& aMaskId, TDes& aFileName );
    TBool EndsWith( const TDesC& aString, const TDesC& aPattern );

    // helpers
    TInt ParseNextUint( TLex& aLex, TUint& aValue );
    void CopyBitmapL( CFbsBitmap& aTrgBitmap, CFbsBitmap& aSrcBitmap );
    
private: // from MAknIconFileProvider

    /** Returns an open file handle to the icon file. */
	void RetrieveIconFileHandleL( RFile& aFile, const TIconFileType aType );
    
    /** Stops using this MAknIconFileProvider instance */
    void Finished();
    
private:
    /**
     * decoded image
     */
    CFbsBitmap*             iBitmap;
    
    /**
     * decoded image mask
     */
    CFbsBitmap*             iMask;

    /**
     * File name to convert
     */
    TFileName               iFilename;
    
    /**
     * size to convert
     */
    TSize                   iSize;

    /**
     * File handle
     */
    RFs                     iFs;
    };

#endif // #ifndef WMIMAGECONVERTER_H

// end of file
