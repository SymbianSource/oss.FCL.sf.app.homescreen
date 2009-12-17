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


#ifndef HNATTRIMGPROVSVGIMAGE_H_
#define HNATTRIMGPROVSVGIMAGE_H_

#include "hnattrimgprovider.h"

class CSvgEngineInterfaceImpl;

/**
 *  Image provider class for empty image.
 *  Image provider specialization that loads image
 *  from an image file path.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvSvgImage ):
    public CHnAttrImgProvider,
    public MSvgRequestObserver
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aFileName Image file name.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvSvgImage* NewL( const TDesC& aFileName );
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aFileName Image file name.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvSvgImage* NewLC( const TDesC& aFileName );

    /**
     * Virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnAttrImgProvSvgImage();

    /**
     * Gets icon.
     * 
     * @since S60 5.0
     * @param aDesiredSize Desired size of the icon.
     * @return Icon as a CGulIcon pointer.
     */
    CGulIcon* GetIcon( TSize *aDesiredIconSize );
    
    /**
     * Gets icon holder which contains the icon that GetIcon returns.
     * 
     * @param aDesiredSize Desired size of the icon.
     * @return icon hoder that contains the icon (may return NULL).
     */
    CHnIconHolder* GetIconHolder( TSize* aDesiredIconSize );
    

private:

    /**
     * Renders the icon.
     * 
     * @since S60 5.0
     * @param aDesiredSize Desired size of the icon.
     * @return Error code.
     */
    TInt RenderIcon( TSize aDesiredIconSize );
    
    /**
     * Defualt constructor.
     * 
     * @since S60 5.0
     */
    CHnAttrImgProvSvgImage();
    
    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     * @param aFileName Image file name.
     */
    void ConstructL( const TDesC& aFileName );
    
public : //from MSvgRequestObserver
	
	/**
	 * @see MSvgRequestObserver:UpdateScreen
	 */
    void UpdateScreen();

    /**
	 * @see MSvgRequestObserver:MSvgRequestObserver
     */
    TBool ScriptCall( const TDesC& aScript,	CSvgElementImpl* aCallerElement );

    /**
	 * @see MSvgRequestObserver:FetchImage
     */
    TInt FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );

	/**
	 * @see MSvgRequestObserver:FetchFont    
	 */
	TInt FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );

    /**
	 * @see MSvgRequestObserver:UpdatePresentation
     */
    void UpdatePresentation(const TInt32&  aNoOfAnimation);    

private: // data

    /**
     * Bitmap. Not Own. GulIcon takes ownership.
     */
    CFbsBitmap* iBitmap;

    /**
     * Mask. Not Own. GulIcon takes ownership.
     */
    CFbsBitmap* iMask;

    /**
     * Mask. Own. Dummy for the engine.
     */
    CFbsBitmap* iDummyBitmap;
    
	/**
	* SVG Engine instance
	*/    
    CSvgEngineInterfaceImpl* iSvgEngine;
    
	/**
	* Handle to the svg dom tree.
	*/    
    TInt iHandle;
    
    };

#endif // HNATTRIMGPROVSVGIMAGE_H_

