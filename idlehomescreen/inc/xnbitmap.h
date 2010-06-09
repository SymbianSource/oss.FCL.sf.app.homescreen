/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for Bitmap.
*
*/



#ifndef _XNBITMAP_H
#define _XNBITMAP_H

//  INCLUDES
#include "xncomponent.h"
#include "mxncomponentinterface.h"

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

namespace XnImageInterface
{
_LIT8(KType, "image");
class MXnImageInterface : public XnComponentInterface::MXnComponentInterface
   {

public:
		
    static inline const TDesC8& Type()
        {
        return KType;
        }
		
public: // New functions

    /**
    * Sets content bitmaps. Ownership is transferred.
    * @since Series 60 3.1
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    */        
    virtual void SetContentBitmaps(CFbsBitmap* aBitmap, CFbsBitmap* aMask) = 0;   

    /**
    * Sets content bitmaps from file path.
    * @since S60 5.2
    * @param aFilename Bitmap filename.    
    */            
    virtual void SetContentBitmaps( TFileName& aFilename ) = 0;
    
    /**
    * Gets content bitmaps. Ownership is not transferred.
    * @since Series 60 3.1
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    */        
    virtual void ContentBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask) = 0;

private:

   };
 }   
    
/**
*  @ingroup group_xnbitmapfactory
*  @lib xn3bitmapfactory.dll
*  @since Series 60 3.1
*/
class CXnBitmap : public CXnComponent, public XnImageInterface::MXnImageInterface
   {

public:

	/**
	 * 2 phase construction.
	 */
	static CXnBitmap* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnBitmap();

public: // New functions
    /**
    * Sets content bitmaps. Ownership is transferred.
    * @since Series 60 3.1
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    */        
    void SetContentBitmaps(CFbsBitmap* aBitmap, CFbsBitmap* aMask);   

    /**
    * Sets content bitmaps from file path.
    * @since S60 5.2
    * @param aFilename Bitmap filename.    
    */            
    void SetContentBitmaps( TFileName& aFilename );
    
    /**
    * Gets content bitmaps. Ownership is not transferred.
    * @since Series 60 3.1
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    */        
    void ContentBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask);


public: // from CCoeControl
   	/**
      * Create a component interface according to the given type.
      * @param aType Type of the interface to create
      * @return Created interface or NULL if the provided type is not supported.
      */ 
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(const TDesC8& aType); 
  
	        
private:

	CXnBitmap();
	void ConstructL();

private:
   };

#endif      // _XNBITMAP_H
            
// End of File
