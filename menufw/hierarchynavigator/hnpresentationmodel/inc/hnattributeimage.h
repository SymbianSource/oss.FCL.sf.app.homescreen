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
* Description:   image attribute model
*
*/


#ifndef C_HNATTRIBUTEIMAGE_H
#define C_HNATTRIBUTEIMAGE_H

#include <AknsSkinInstance.h> 
#include "hnattributebase.h"

class CFbsBitmap;
class CHnAttrImgProvider;
class CGulIcon;
class TLiwVariant;

/**
 *  Image attribute.
 * 
 *  Class defining behaviour of the CHnAttributeBase interface
 *  to support images. The model is responsible for creation 
 *  of the image, appropriately, depending on the source 
 *  of the image.
 * 
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttributeImage ) : public CHnAttributeBase
{
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttributeImage* NewL();

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttributeImage* NewLC();

    
    /**
     * Standard C++ virtual destructor.
     * @since S60 5.0
     */
    ~CHnAttributeImage();
    
    /**
     * Sets skin id.
     * 
     * @since S60 5.0
     * @param aSkinId Skind id.
     */
    IMPORT_C void SetSkinId ( TAknsItemID aSkinId );

    /**
     * Sets file name source.
     * Takes ownership of the buffer. Therefore the buffer must not be NULL.
     * 
     * @since S60 5.0
     * @param aFileNameSrc A source filename, as a heap buffer pointer.
     */
    IMPORT_C void SetFileNameSrc ( HBufC* aFileNameSrc );

    /**
     * Sets mif file.
     * Takes ownership of the buffer. Therefore the buffer must not be NULL.
     * 
     * @since S60 5.0
     * @param aMifFile Mif filename as a heap buffer pointer.
     */
    IMPORT_C void SetMifFile ( HBufC* aMifFile );

    /**
     * Sets bitmap id.
     * 
     * @since S60 5.0
     * @param aBitmapId Bitmap id.
     */
    IMPORT_C void SetBitmapId ( TInt aBitmapId );

    /**
     * Sets mask id.
     * 
     * @since S60 5.0
     * @param aMaskId Mask id.
     */
    IMPORT_C void SetMaskId ( TInt aMaskId );

    /**
     * Sets application uid.
     * 
     * @since S60 5.0
     * @param aApplicationUid Application uid.
     */
    IMPORT_C void SetApplicationUid( TUid aApplicationUid);

    /**
     * Sets bitmap.
     * Takes ownership.
     * 
     * @since S60 5.0
     * @param aBitmap Pointer to a bitmap.
     */
    IMPORT_C void SetBitmap ( TLiwVariant* aBitmap );

    /**
     * Sets mask.
     * Takes ownership.
     * 
     * @since S60 5.0
     * @param aMask Pointer to a mask.
     */
    IMPORT_C void SetMask ( TLiwVariant* aMask );
    
    /**
     * Gets icon, packed in CGulIcon.
     * 
     * @see CGulIcon
     * @since S60 5.0
     * @return Icon as a poitner to CGulIcon.
     */
    IMPORT_C CGulIcon* GetIcon( TSize* aDesiredIconSize );
    
    /**
     * Please see the description in the base class @c CHnAttributeBase.
     */
    CHnIconHolder* GetIconHolder( TSize* aDesiredIconSize );
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    void RemoveLiwObjects();
    
private:
    
    /**
     * Standard C++ constructor.
     * 
     * @since S60 5.0
     */
    CHnAttributeImage();

    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL();

    /**
     * Method is used to prepare the image provider,
     * if the icon is being fetched for the first time.
     * Image providers are created appropriately depending on 
     * the image source, and they are responsible for providing
     * bitmaps.
     *
     * @since S60 5.0
     * @return Error code when provider cannot be created.
     */
    TInt Prepare();

    /**
     * Method is used to prepare the image provider.
     * Image providers are created appropriately depending on 
     * the image source, and they are responsible for providing
     * bitmaps.
     * 
     * @since S60 5.0
     */
    void PrepareProviderL();

    /**
     * Returns true if the attribute loaded a mif icon.
     * 
     * @since S60 5.0
     * @return True if image was loaded from mif.
     */
    TBool IsThemeMif();
    
    /**
     * Returns true if the attribute is loaded from application uid.
     * 
     * @since S60 5.0
     * @return True if image was loaded from application uid.
     */
    TBool IsApplicationImage();
    
    /**
     * Returns true if the attribute is loaded from image file.
     * 
     * @since S60 5.0
     * @return True if image was loaded from image file.
     */
    TBool IsFileImageL();

    /**
     * Returns true if the attribute is loaded from liw buffer.
     * 
     * @since S60 5.0
     * @return True if image was loaded from liw buffer.
     */
    TBool IsLiwImage();

private:

    /**
     * Skin id.
     */
    TAknsItemID iSkinId;
    
    /**
     * Mif file name.
     */
    RBuf iMifFile;

    /**
    * Bitmap id.
    */
   TInt iBitmapId;

   /**
    * Mask id.
    */
   TInt iMaskId;
   
   /**
    * Image source file name.
    */
    RBuf iFileNameSrc;
    
    /**
     * Application uid to fetch the image from.
     */
    TUid iApplicationUid;
    
    /**
     * Bitmap from Liw Buffer.
     * Own.
     */
    TLiwVariant* iBitmap;
    
    /**
     * Mask from Liw Buffer.
     * Own.
     */
    TLiwVariant* iMask;
    
    /**
     * Hn image provider
     * Own.
     */
    CHnAttrImgProvider* iAttrImgProvider;

    };

#endif // C_HNATTRIBUTEIMAGE_H



