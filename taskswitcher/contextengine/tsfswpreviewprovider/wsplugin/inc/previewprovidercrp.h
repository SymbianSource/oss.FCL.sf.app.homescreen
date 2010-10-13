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
* Description:  Preview provider ECOM plugin
*
*/


#ifndef PREVIEWPROVIDERCRP_H
#define PREVIEWPROVIDERCRP_H

// INCLUDES
#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>
#include <graphics/wsgraphicdrawer.h>

// Number of screenshots used at the same time,
// each window group change will cause a screenshot
// to one of these, the index for the current one is
// switched after each screenshot capture. (this is needed
// to prevent overwriting screenshots before the client could
// make a copy of them)
const TInt KMaxShots = 4;

/**
 * Preview provider window server ECOM plugin class.
 */
NONSHARABLE_CLASS( CPreviewProviderCRP ) : public CWsGraphicDrawer, 
                                           public MWsEventHandler 
    {
public:

    /**
     * Creates a new instance of the plguin.
     *
     * @return Created instance.
     */ 
    static CWsGraphicDrawer* CreateL();
    
    /**
     * Destructor
     */ 
    ~CPreviewProviderCRP();
    
private:

    /**
     * From CWsGraphicDrawer.
     *
     * @param aGc
     * @param aRect
     * @param aData
     */ 
    void DoDraw( MWsGc& aGc, const TRect& aRect, const TDesC8& aData ) const;
    
    /**
     * Handles message from PreviewProviderClient.
     *
     * @param aData Message data.
     */ 
    void HandleMessage( const TDesC8& aData );

    /**
     * 2nd phase constructor.
     *
     * @param aEnv
     * @param aId
     * @param aOwner
     * @param aData
     */     
    void ConstructL( MWsGraphicDrawerEnvironment& aEnv, 
                     const TGraphicDrawerId& aId, 
                     MWsClient& aOwner, 
                     const TDesC8& aData );
    
    /**
     * From MWsEventHandler.
     * 
     * Observer for TWservCrEvent::EWindowGroupChanged to take the screenshot
     * when application changes.
     *
     * @param aEvent
     */     
    void DoHandleEvent( const TWservCrEvent& aEvent );
    
private:

    /**
     * 2nd phase constructor.
     */ 
    void ConstructL();
    
    /**
     * Handles application registration.
     *
     * @param aWgId Window group id.
     */ 
    void Register( TInt aWgId );
    
    /**
     * Handler application unregistration.
     *
     * @param aWgId Window group id.
     */     
    void Unregister( TInt aWgId );
    
    /*
     * Allocate resources and take screenshot
     */
    void ScreenshotL();
    
    /**
     * Takes the screenshot.
     */     
    void ScreenshotL( CFbsBitmap& aBitmap );
    
    /**
     * Remove bitmap reference fom list
     */
    void RemoveScreenshot( TInt aBitmapHandle );
    
    /**
     * Remove all bitmaps overflow
     */
    void CheckOverflow();
    
    /**
     * Handles message from PreviewProviderClient.
     *
     * @param aData Message data.
     */     
    void DoHandleMessageL( const TDesC8& aData );
    
    /**
     * Notifies the PreviewProviderClient when screenshot is ready for copying.
     */     
    void ScaleCompleteL(const CFbsBitmap& aBitmap);
    
    /**
     * Notifies the PreviewProviderClient when an application has been exited.
     */
    void UnregisterComplete( TInt aWgId );
    
    /**
     * Notifies the PreviewProviderClient that specific bitmap needs rotation.
     */
    void BitmapRotationNeeded( TInt aWgId, TBool aClockwise );
    
    /**
     * Uses window server front buffer to create a screenshot.
     * @param aFront preallocated destination bitmap
     * @param aFront Front buffer. 
     * @param aSrcSize Size of the source front buffer in pixels.
     * @param aSrcStride Source stride.
     */
    void FrontBufferScreenShot(
        CFbsBitmap& aBitmap,
        MWsFrontBuffer& aFront, const TSize& aSrcSize, TInt aSrcStride );  
    
private:
    TInt iScreen;
    TInt iPrevId;
    TInt iPrevReg; // to work around the problem with wgid not set when app brought 
                   // to foreground first time
    RPointerArray<CFbsBitmap> iScreenshots;
    TSize iScreenshotSize;
    TDisplayMode iScreenshotMode;
    RArray<TInt> iWgIds;
    TTime iScreenChangedTime; // fix for orientation changed/wg group changed problem
    TBool iClockwiseRot; // for marking rotation direction
    TBool iLastWgIdRedraw;
    };

#endif // PREVIEWPROVIDERCRP_H

// End of file
