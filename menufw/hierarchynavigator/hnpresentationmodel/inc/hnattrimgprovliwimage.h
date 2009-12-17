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
* Description:   image provider for liw buffer
*
*/


#ifndef HNATTRIMGPROVLIWIMAGE_H_
#define HNATTRIMGPROVLIWIMAGE_H_

#include <AknUtils.h>
#include "hnattrimgprovider.h"
#include "hnglobals.h"

class TLiwVariant;
class CLiwBitmapBuffer;

/**
 *  Image provider class for empty image.
 *  Image provider specialization that loads image
 *  froma an liw buffer.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvLiwImage ) : public CHnAttrImgProvider
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aBitmap Pointer to bitmap.
     * @param aMaskBitmap Poitner to mask.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvLiwImage* NewL( TLiwVariant*& aBitmap, 
            TLiwVariant*& aMaskBitmap );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aBitmap Pointer to bitmap.
     * @param aMaskBitmap Poitner to mask.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvLiwImage* NewLC( TLiwVariant*& aBitmap, 
            TLiwVariant*& aMaskBitmap );

    /**
     * Virtual destructor.
     * 
     * @since S60 5.0
     */
    virtual ~CHnAttrImgProvLiwImage();
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    virtual void RemoveLiwObjects();

// from base class CHnAttrImgProvider
public:
    
    /**
     * Returns the value.
     *
     * @since S60 5.0
     * @return Value
     */
    const TDesC8& Value();

private:

    /**
     * Defualt constructor.
     * 
     * @since S60 5.0
     */
    CHnAttrImgProvLiwImage();

    /**
     * Standard 2nd pahse symbian constructor.
     * 
     * @since S60 5.0
     * @param aBitmap Pointer to bitmap.
     * @param aMaskBitmap Poitner to mask.
     */
    void ConstructL( TLiwVariant*& aBitmap, TLiwVariant*& aMaskBitmap );
    
    /**
     * Creates icon whose bitmaps are owned externally by CLiwBitmapBuffer objects.
     * 
     * @param aBitmap Primary icon bitmap.
     * @param aMask Icon mask.
     * @param aExtBmp Buffer that owns the primary bitmap of the icon.
     * @param aExtMask Buffer that owns the mask.
     */
    void CreateExtIconL( CFbsBitmap* aBitmap, CFbsBitmap* aMask,
            CLiwBitmapBuffer* aExtBmp, CLiwBitmapBuffer* aExtMask );
    
    /**
     * Creates Bitmap from variant which can be a handle or a descriptor.  
     * 
     * @since S60 5.0
     * @param aVariant Contains bitmap.
     * @return Bitmap.
     */
    CFbsBitmap* CreateBitmapFromVariantL( const TLiwVariant& aVariant );
    
    /**
     * Retrieves bitmap handle from a TLiwVariant object.
     * The reason why such function is needed is that bitmap handle can be
     * stored in many integer types.
     * 
     * @param aVariant a variant from which to extract the handle
     * @param aHandle will contain the handle on successful execution
     * @return KErrNone if handle extracted successfully, KErrNotFound if
     *         the variant did not contain anything that can be cast to
     *         integer. 
     */
    TInt ExtractBitmapHandleFromVariant( const TLiwVariant& aVariant,
            TInt& aHandle );
    
private: // data
   
    /**
     * Value.
     */
    TBuf8<KMaxLength> iValue;
    
    };
    
#endif // HNATTRIMGPROVLIWIMAGE_H_
    

