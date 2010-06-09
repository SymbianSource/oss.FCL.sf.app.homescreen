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
* Description:  Implements Xuikon image component.
*
*/



#ifndef _XNBITMAPADAPTER_H
#define _XNBITMAPADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"


// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CXnImageDecoder;

// CLASS DECLARATION
/**
*  @ingroup group_xnbitmapfactory
*  @lib xn3bitmapfactory.dll
*  @since Series 60 3.1
*/
class CXnBitmapAdapter : public CXnControlAdapter
   {
public:
	/**
	 * 2 phase construction.
	 */
	static CXnBitmapAdapter* NewL(CXnNodePluginIf& aNode);

	/**
	 * Destructor.
	 */
	virtual ~CXnBitmapAdapter();
	

public: // New functions
    /**
    * Sets content bitmaps. Ownership is transferred.
    * @since Series 60 3.1
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    */        
    void SetContentBitmaps(CFbsBitmap* aBitmap, CFbsBitmap* aMask);   

    /**
    * Sets content bitmaps from file.
    * @since S60 5.2
    * @param aFilename Bitmap filename.    
    */            
    void SetContentBitmaps( TFileName& aFilename );
    
    /**
    * Gets content bitmaps. Ownership not is transferred.
    * @since Series 60 3.1
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    */        
    void ContentBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask);
   
public: // From Base classes 
    /**
    * From CXnControlAdapter Handles the property changes.
    * @since Series 60 3.1
    * @return void.
    */    
    void DoHandlePropertyChangeL(CXnProperty* aProperty = NULL);
    
    /**
    * From CCoeControl Handles the skin change
    * @since Series 60 3.2
    * @return void.
    */
    void SkinChanged();

protected:
	/**
    * Size change notification
    */ 
    void SizeChanged();    
	
private:
	CXnBitmapAdapter(CXnNodePluginIf& aNode);
	void ConstructL(CXnNodePluginIf& aNode);

    void InitializeBitmapsL();
private: // Data
    // UI node, not owned
    CXnNodePluginIf& iNode;
    // Image decoder, owned
    CXnImageDecoder* iDecoder;
    // Whether the bitmaps has been loaded or not.
    mutable TBool iAreBitmapsLoaded;
    // Whether the data API has been used or not.
    TBool iAreBitmapsSet;
    // Path of the bitmap, owned
    HBufC* iPath;
    // The size of the current bitmap
    TSize iBitmapSize;
	// The path of the fallback image, owned
    HBufC* iFallbackPath;
	// Whether fallback path has changed and bitmaps need to be reloaded
    TBool iFallbackPathChange;
   };



#endif      // XNBITMAPADAPTER_H
            
// End of File
