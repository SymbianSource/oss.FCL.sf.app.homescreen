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
* Description:  Wrapper class for CEikLabel.
*
*/



#ifndef _XNTEXTADAPTER_H
#define _XNTEXTADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"
#include <e32base.h>
#include <eiklabel.h>
#include <AknPictographDrawerInterface.h> // for japanese variants

// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CAknPictographInterface;

// CLASS DECLARATION
/**
*  @ingroup group_xntextfactory
*  @lib xn3textfactory.lib
*  @since Series 60 3.1
*/
class CXnTextAdapter : public CXnControlAdapter,
					   public MAknPictographAnimatorCallBack
   {
public:
	/**
	 * 2 phase construction.
	 */
	static CXnTextAdapter* NewL(CXnControlAdapter* aParent, CXnNodePluginIf& aNode);

	/**
	 * Destructor.
	 */
	virtual ~CXnTextAdapter();

public: // New functions
    /**
     * Sets the new content to the underlying CEikLabel
     * @param aText The new content
     */
	void SetTextL(const TDesC& aText);
	
	/**
	 * Returns the text contained in the underlying CEikLabel
	 * @return TDesC* The text
	 */
	const TDesC* Text() const;

public: // from base classes
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
    virtual void SizeChanged();
    virtual void SkinChanged();
    
    /**
    * From CXnControlAdapter Handles the property changes.
    * @since Series 60 3.1
    * @return void.
    */  
    void DoHandlePropertyChangeL(CXnProperty* aProperty = NULL); 
    
    /**
    * From CCoeControl Handles the resource change.
    * @since Series 60 3.1
    * @param aType A type of the resource change
    * @return void.
    */
    void HandleScreenDeviceChangedL();

    /**
    * See CCoeControl documentation
    */    	
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
    /**
    * See MAknPictographInterface documentation
    */
    void DrawPictographArea();

    /**
    * See CXnControlAdapter documentation
    */    	   
    TSize MeasureAdaptiveContentL( const TSize& aAvailableSize );
    
private:

	CXnTextAdapter(CXnControlAdapter* aParent, CXnNodePluginIf& aNode);
	void ConstructL();
	void ConstructTextL();
    virtual void Draw(const TRect& aRect) const;
    
    void UpdateBackgroundL() const;
    void DrawBackground(const TRect& aRect, CWindowGc& aGc) const;
    TInt RestrictSizeL();
    
    void SetLineSpacingL( TInt aReference );
    TSize MeasureTextL( const TSize& aAvailableSize );
    void UpdateTextL( const TDesC* aText );
    void UpdateTextL( const TDesC8& aText );

	/**
	 * Sets text properties, such as color, font, etc.
     * @param aNode UI node
     * @return ETrue if text needs reconstruction, EFalse otherwise
	 */
	TBool SetTextPropertiesL( CXnNodePluginIf& aNode );	
    
   	/**
    * Same as SizeChanged(), but this is an L function.
    */
    void SizeChangedL();

	
private:
    CEikLabel*                  iLabel;         // Label to draw text    
    CXnControlAdapter*  		iParent;        // Parent control, not owned
    CXnNodePluginIf&    		iNode;          // UI node, not owned
    CFont*              		iFont;          // Used font, owned    
    TBool               		iReleaseFont;   // Whether the font needs to be released or not.
    TUint8               		iFlags;         // Flags for text properties
    CAknPictographInterface* 	iPictographInterface; // Pictograph interface.    
    CWindowGc*					iGc;            // Window graphics context. Not own.
    mutable CFbsBitmap*			iBackgroundBitmap; // Pointer to background bitmap. Own.
    TInt                        iMaxLines;      //Max lines for wrapped text string    
    HBufC*                      iText;    
   };



#endif      // XNTEXTADAPTER_H
            
// End of File
