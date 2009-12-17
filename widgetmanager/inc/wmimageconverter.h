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

// Forward declarations
class CFbsBitmap;
class CBitmapScaler;
class CImageDecoder;
class TAknsItemID;
class CWmUnitTest;
class CWmIconFileProvider;

// Class declaration
/**
 *  MConverterObserver
 *  To notify when image is converted
 */
NONSHARABLE_CLASS( MConverterObserver )
    {
    public:
        virtual void NotifyCompletion( TInt aError ) = 0;
    };

// Class declaration
/**
 * CWmImageConverter
 * Image converter
 */
NONSHARABLE_CLASS ( CWmImageConverter ) : public CActive
    {
    /** states for this converter */
    enum TState 
        {
        EIdle = 0,
        EDecoding,
        EScalingBitmap,
        EScalingMask
        };

public: //contructors/destructors
    /**
     * Two-phased constructor.
     * @param aObserver observer
     */
    static CWmImageConverter* NewL( MConverterObserver* aObserver );    
    
    /** Destructor */
    ~CWmImageConverter();
        
public: // interface methods
    
    /**
     * Parses icon string and prepares logo image.
     * 
     * @param aWidth wanted widht
     * @param aHeight wanted height
     * @param aIconStr str containing logo icon
     * Supported values:
     * - skin(<major id> <minor id>):mif(<path> <bitmapid> <maskid>)
     * - mif(<path> <bitmapid> <maskid>)
     * - uid(<application uid>)
     * - <file name>.<png/svg>
     */
    void HandleIconStringL( TInt aWidth, TInt aHeight, const TDesC& aIconStr );

    /**
     * Returns converted bitmap. Caller takes ownership
     * 
     * @return CFbsBitmap
     */
    CFbsBitmap* Bitmap();
    
    /**
     * Returns converted mask. Caller takes ownership
     * 
     * @return CFbsBitmap
     */
    CFbsBitmap* Mask();

    /**
     * sets the size for decoding
     * @param aSize the logo size
     */
    inline void SetLogoSize( const TSize& aSize );

    /** supported image conversion methods */
    enum TConversionMethod
        {
        EUnrecognized, // we could not recognise the icon string
        EUidIcon, // App.UID-coded icon
        ESvgIcon, // Icon from SVG-file
        ESkinIcon, // Icon from SKIN id
        EMifIcon, // Icon from MIF file, known graphic index
        ESkinAndMifIcon, // Icon from SKIN and MIF combination 
        EImageIcon, // Icon from image file (jpeg or png)
        };

    /**
     * The type of currently converted image.
     * This method can be used to check if correct conversion was used.
     * @return conversion method used for current image
     */
    TConversionMethod ConversionMethod();

protected: // implementation of CActive
    /**
     * Implements cancellation of an outstanding request.
     * 
     * @see CActive::DoCancel
     */
    void DoCancel();
    
    /**
     * Handles an active object's request completion event.
     * 
     * @see CActive::RunL
     */
    void RunL();
    
    /**
     * RunError
     * 
     * @see CActive::RunError
     */
    TInt RunError(TInt aError);
        
private:
    CWmImageConverter(); 
    void ConstructL( MConverterObserver* aObserver );
    
private:

    inline TState EngineState();
    void ScaleBitmap( TInt aWidth, TInt aHeight );
    void ScaleMask( TInt aWidth, TInt aHeight );
    void CreateIconFromUidL( const TUid& aUid );
    void CreateIconFromSvgL( const TDesC& aFileName );
    void CreateIconFromOtherL( const TDesC& aFileName );
    void CreateSkinOrMifIconL( 
                    const TAknsItemID& aItemId, TInt aBitmapId, 
                    TInt aMaskId, const TDesC& aFileName );
    // resolvers
    TBool ResolveUid( const TDesC& aPath, TUid& aUid );
    TBool ResolveSkinId( const TDesC& aPath, TAknsItemID& aItemId );
    TBool ResolveMifId( const TDesC& aPath, TInt& aBitmapId, 
                        TInt& aMaskId, TDes& aFileName );
    TBool ResolveSkinIdAndMifId( const TDesC& aPath, TAknsItemID& aItemId,
                        TInt& aBitmapId, TInt& aMaskId, TDes& aFileName );
    TBool EndsWith( const TDesC& aString, const TDesC& aPattern );

private:
    /**
     * Converter observer (not owned)
     */
    MConverterObserver*     iObserver;

    /**
     * decoded image
     */
    CFbsBitmap*             iBitmap;
    
    /**
     * decoded image mask
     */
    CFbsBitmap*             iMask;
    
    /**
     * Icon file provider
     */
    CWmIconFileProvider*    iIconFileProvider;
    
    /**
     * decoder from ICL API
     */
    CImageDecoder*          iImageDecoder;
    
    /**
     * bitmap scaler
     */
    CBitmapScaler*          iScaler;
    
    /**
     * internal state
     */
    TState                  iState;
    
    /**
     * File name to convert
     */
    TFileName               iFilename;
    
    /**
     * size to convert
     */
    TSize                   iSize;
    
    /**
     * is scaling needed
     */
    TBool                   iScaleNeeded;
    
    /**
     * File handle
     */
    RFs                     iFs;

    /**
     * Conversion method for current image
     */
    TConversionMethod       iConversionMethod;

    };

#include "wmimageconverter.inl"
#endif // #ifndef WMIMAGECONVERTER_H

// end of file
