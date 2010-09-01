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
* Description:  Image decoder
*
*/


#ifndef _XNIMAGEDECODER_H
#define _XNIMAGEDECODER_H

// System includes
#include <e32base.h>

// User includes
class CXnBitmapAdapter;
class CFbsBitmap;
class CImageDecoder;
class RFs;

// Forward declarations

/**
 * Image decoder
 * 
 * @ingroup group_xnbitmapfactory
 * @lib xn3bitmapfactory.lib
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CXnImageDecoder ) : public CActive
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnImageDecoder* NewL( CXnBitmapAdapter& aAdapter, RFs& aFs );        

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnImageDecoder* NewLC( CXnBitmapAdapter& aAdapter, RFs& aFs );         

    /**
     * Destructor
     */
    ~CXnImageDecoder();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CXnImageDecoder( CXnBitmapAdapter& aAdapter, RFs& aFs );         
        
private:
    // from CActive
    
    /**
     * @see CActive
     */
    void RunL();

    /**
     * @see CActive
     */    
    TInt RunError( TInt aError );
    
    /**
     * @see CActive
     */    
    void DoCancel();
    
public: 
    // new functions
    
    /**
     * Starts decode.
     * 
     * @param aFile Image file
     */
    void DecodeL( TFileName& aFilename );
            
private:
    // new functions
    
    void Cleanup();
    
private:
    // data

    /** Bitmapadapter, not owned */
    CXnBitmapAdapter& iAdapter; 
    /** File server session, not owned */
    RFs& iFs;
    /** Image decoder, owned */
    CImageDecoder* iDecoder;
    /** Bitmap, owned during conversion */
    CFbsBitmap* iBitmap;
    /** Mask, owned during conversion */
    CFbsBitmap* iMask;    
    };

#endif // _XNIMAGEDECODER_H

// End of file
