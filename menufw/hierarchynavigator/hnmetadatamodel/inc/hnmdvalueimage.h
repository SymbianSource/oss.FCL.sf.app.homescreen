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
* Description:   
*
*/


#ifndef CHNMDIMAGE_H
#define CHNMDIMAGE_H

#include <e32base.h>
#include <AknsItemID.h> 
#include "hnmdvaluebase.h"
#include "hnglobals.h"

class TXmlEngElement;
class CLiwGenericParamList;
struct THnMdCommonPointers;
class CGulIcon;
class CFbsBitmap;
class CHnAttributeBase;
class CHnAttributeImage;

// CLASS DECLARATION

/**
 * Value Image.
 * 
 * Specific class inherigint from CHnMdValue Base. This
 * class is responsible for images.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdValueImage) : public CHnMdValueBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CHnMdValueImage();

    /**
     * Two-phased constructor.
     * 
     * @param aElement XML element.
     * @param aCmnPtrs pointer to common pointer structure.
     * @return Object instance of meta data value image.
     */
    static CHnMdValueImage* NewL( const TXmlEngElement& aElement,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Two-phased constructor.
     * 
     * @param aElement XML element.
     * @param aCmnPtrs pointer to common pointer structure.
     * @return Object instance of meta data value image.
     */
    static CHnMdValueImage* NewLC( const TXmlEngElement& aElement,
            THnMdCommonPointers* aCmnPtrs );
    
    /**
     * Evaluates image object changing references into values
     * if possible.
     * 
     * @param aList Parameters list used in evaluation.
     * @param aPos Position of the corresponding record in a paramters list.
     */
    void EvaluateL( CLiwGenericParamList* aList, TInt aPos );
    
// From CHnMdValueBase.
    
    /**
     * Retruns path to the file or the id of the file.
     * 
     * @param aQueriesResultsList Parameters list used when creating an attribute.
     * @param aPos Position of the corresponding record in a paramters list.
     * @return Attribute.
     */
    CHnAttributeBase* CreateAttributeL( const CLiwGenericParamList* aQueriesResultsList,
            TInt aPos );
                
    /**
     * Returns CGulIcon object.
     * 
     * @param aQueriesResultsList Parameters list used when creating an attribute.
     * @param aPos Position of the corresponding record in a paramters list.
     * @return CGulIcon object.
     */
//    CGulIcon* GetIconL( const CLiwGenericParamList* aQueriesResultsList = NULL,
//            TInt aPos = 0 );
    
    /**
     * Returns variant containing bitmap.
     * 
     * @param aPath Path to the bitmap file.
     * @return Variant object. Ownership is transferred.
     */
    TLiwVariant* BitmapFromLiwL( const TDesC8& aPath );

    /**
     * Returns bitmap.
     * 
     * @return Variant containing bitmap object.
     */
    TLiwVariant* BitmapL();
    
    /**
     * Returns mask.
     * 
     * @return Variant containing bitmap object.
     */
    TLiwVariant* MaskL();
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CHnMdValueImage();

    /**
     * EPOC default constructor for performing 2nd stage construction
     * @param aElement Xml element.
     * @param aCmnPtrs Meta data common pointers.
     */
    void ConstructL( const TXmlEngElement& aElement,
            THnMdCommonPointers* aCmnPtrs );
   
    /**
     * Sets image source.
     */
    void SetImageSourceL();
    
    /**
     * Searches for resource id in the given file using
     * resource name.
     * 
     * If not found return KErrNotFound
     * 
     * @param aResourceFileContents contents of the file to search
     * @param aResourceName name of the resource (string)
     * @return KErrNotFound in case of error, otherwise id if the resource
     */
    TInt GetResourceIdL( const TDesC8& aResourceFileContents,
            const TDesC8& aResourceName );
    
    /**
     * File name getter.
     * 
     * @return file name
     */
    const TDesC8& FileName8();
    
    /**
     * Skin id getter.
     * 
     * @return skin id.
     */
    TAknsItemID SkinIdL();
    
    /**
     * Determines bitmap id and mask id in the MIF file.
     * If bitmap id or/and mask id cannot be determined, KErrNotFound
     * will be returned in aBitmapId or/and aMaskId. 
     * 
     * @param aBitmapId on successful execution will be set to bitmap id
     * @param aMaskId on successful execution will be set to mask id
     */
    void GetBitmapAndMaskIdL( TInt& aBitmapId, TInt& aMaskId );
    
    /**
     * Replaces file extension in a path to a file.
     * Please note that the new extension must not be longer than the old one,
     * or the method will leave. This means that this method cannot be applied
     * to files without extension.
     * 
     * @param aPath path to a file with extension
     * @param aNewExt a new extension
     * @return path to the same file as aPath but with extension replaced
     */
    static HBufC* ReplaceFileExtensionLC(
            const TDesC& aPath, const TDesC& aNewExt );
    
    /**
     * Application UID getter.
     * 
     * @return application uid
     */
    TUid ApplicationUidL();
    
    /**
     * Sets attribute data.
     * 
     * @param aAttr Attribute data to be set.
     */
    void SetAttributeDataL( CHnAttributeImage* aAttr );
    
    /**
     * Return Mif file name.
     * 
     * @return Mif file name.
     */
    HBufC* GetMifFileNameL();

    /**
     * Returns file name.
     * 
     * @return File name.
     */
    HBufC* GetFileNameSrcL();
    
    /**
     * Returns a path to the best matching mif file corresponding
     * to the given file name.
     * 
     * @param aFs a reference to the file server
     * @param aFileName a name of the file to be localized
     * @return a valid path to the mif file
     * 
     */
    HBufC* FindMatchigMifFileL( const RFs& aFs, TDesC& aFileName );
    
    /**
     * Creates keys for caching bitmap and mask ids.
     * 
     * @param aKeyBitmap A bitmap key name to be created. 
     * @param aBitmapIdBuf A bitmap id.
     * @param aKeyMask A mask key to be created.
     * @param aMaskIdBuf A mask id.
     */
    void CreateKeysLC( RBuf8& aKeyBitmap, RBuf8& aBitmapIdBuf, 
    		RBuf8& aKeyMask, RBuf8& aMaskIdBuf );
    

private: // data
    
    /**
     * Own - Skin id minor.
     */
    RBuf8 iSkinIdMinor8;
    
    /**
     * Own - Skin id major.
     */
    RBuf8 iSkinIdMajor8;

    /**
     * Own - File name.
     */
    RBuf8 iFileNameSrc8;
    
    /**
     * Own - Mif file name.
     */
    RBuf8 iMifFile8;
    
    /**
     * Own - File name.
     */
    RBuf iFileName;
    
    /**
     * Own - Bitmap id.
     */
    RBuf8 iBitmapId8;
    
    /**
     * Own - Mask id.
     */
    RBuf8 iMaskId8;
    
    /**
     * Own - Application uid.
     */
    RBuf8 iApplicationUid8;
    
    /**
     * Own - Bitmap.
     */
    RBuf8 iBitmap8;
    
    /**
     * Own - Mask;
     */
    RBuf8 iMask8;
    
    /**
     * Own - Image source.
     */
    TImageSource iImageSource;
    
    /**
     * A value contining path to the physical image
     * or its id.
     */
    RBuf iReturnValue;

    /**
     * Common pointers.
     */
    THnMdCommonPointers* iCmnPtrs;
    
    /**
     * Parameters list.
     */
    const CLiwGenericParamList* iParamList;
    
    /**
     * Position of the corresponding record in parameters list.
     */
    TInt iPos;
    
    /**
     * Full path to the MIF file or NULL if there is no MIF file.
     * Own.
     */
    HBufC* iMifFileName;

    };

#endif // CHNMDIMAGE_H
