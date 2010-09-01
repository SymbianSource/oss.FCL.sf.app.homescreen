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
* Description:   image provider for image file
*
*/


#ifndef HNATTRIMGPROVFILEIMAGE_H_
#define HNATTRIMGPROVFILEIMAGE_H_

#include "hnattrimgprovider.h"

class CImageDecoder;

/**
 *  Image provider class for empty image.
 *  Image provider specialization that loads image
 *  from an image file path.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvFileImage ) : public CHnAttrImgProvider
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aFileName Image file name.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvFileImage* NewL( const TDesC& aFileName );
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aFileName Image file name.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvFileImage* NewLC( const TDesC& aFileName );

    /**
     * Virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnAttrImgProvFileImage();

// from base class CHnAttrImgProvider
public:
    
    /**
     * Returns value.
     *
     * @return Value.
     * @since S60 5.0
     */
    const TDesC8& Value();

private:

    /**
     * Defualt constructor.
     * 
     * @since S60 5.0
     */
    CHnAttrImgProvFileImage();
    
    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     * @param aFileName Image file name.
     */
    void ConstructL( const TDesC& aFileName );
    
    /**
     * Determines optimal size for a bitmap to store an image from file.
     * 
     * The bitmap whose size is determindd by this method is supposed to
     * be passed to CImageDecoder::Convert method.
     * The size will be chosen so that the following two requirements are
     * satisfied:
     * 1. Bitmap size does not exceed the initial bitmap size passed in
     *    aBitmapSize.
     * 2. CImageDecoder::Convert fills the whole bitmap with image data
     *    (there will be no empty space in the bitmap).
     * Please note that the information about the size of the image in the
     * file is accessible through the aImageDec object.
     * @param aImageDec reference to a CImageDecoder object
     * @param aBitmapSize maximal bitmap size, optimal bitmap size after
     *        execution
     */
    void CorrectBitmapSize( CImageDecoder& aImageDec, TSize& aBitmapSize );

private: // data

    /**
     * Value
     */
    RBuf8 iValue;

    };

#endif // HNATTRIMGPROVFILEIMAGE_H_

