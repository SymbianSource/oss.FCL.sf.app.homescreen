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
* Description:   image provider for mif file
*
*/



#ifndef C_HNATTRIMGPROVTHEMEMIF_H
#define C_HNATTRIMGPROVTHEMEMIF_H

#include "hnattrimgprovider.h"
#include "hnglobals.h"

/**
 *  Image provider class for empty image.
 *  Image provider specialization that loads image
 *  froma an image file path.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvThemeMif ) : public CHnAttrImgProvider
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aSkinId Skin id
     * @param aFile File name
     * @param aBitmapId Bitmap id
     * @param aMaskId Mask id
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvThemeMif* NewL( TAknsItemID aSkinId,
            const TDesC& aFile, TInt aBitmapId, TInt aMaskId );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aSkinId Skin id
     * @param aFile File name
     * @param aBitmapId Bitmap id
     * @param aMaskId Mask id
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvThemeMif* NewLC( TAknsItemID aSkinId,
            const TDesC& aFile, TInt aBitmapId, TInt aMaskId );

    /**
     * Virtual destructor.
     *      
     * @since S60 5.0
     */
    virtual ~CHnAttrImgProvThemeMif();

// from base class CHnAttrImgProvider
public:
    
    /**
     * Returns the value.
     *
     * @since S60 5.0
     * @return Value
     */
    const TDesC8& Value( );

    /**
     * This method should create a bitmap and its mask, 
     * by retrieving it form the application by a given uid (passed
     * in the constructor)
     *
     * @param aId          Parameter not used.
     * @param aBitmap      Output pointer to a bitmap.     
     * @param aMaskBitmap  Output pointer to a mask.
     */
     void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, 
             CFbsBitmap*& aMaskBitmap);


private:

    /**
     * Default constructor.
     * 
     * @since S60 5.0
     */
    CHnAttrImgProvThemeMif();

    /**
     * Standard 2nd pahse symbian constructor.
     * 
     * @since S60 5.0
     * @param aSkinId Skin id
     * @param aFile File name
     * @param aBitmapId Bitmap id
     * @param aMaskId Mask id
     */
    void ConstructL( TAknsItemID aSkinId,
            const TDesC& aFile, TInt aBitmapId, TInt aMaskId );
    
    /**
     * Checks if mask id and bitmap id are in the range of mif file ids.
     * 
     * @since S60 5.0
     * @return ETrue if ids are in the range of mif file ids. Else return EFalse.
     */
    TBool ValidateIconIds();

private: // data
    
    /**
     * Value.
     */
    TBuf8<KMaxLength> iValue;

    /**
     * Skin id.
     */
    TAknsItemID iSkinId;
    
    /**
     * File name.
     */
    TPtrC iFile;

    /**
     * Bitmap id.
     */
    TInt iBitmapId;
    
    /**
     * Mask id.
     */
    TInt iMaskId;
    
    };


#endif // C_HNATTRIMGPROVTHEMEMIF_H

