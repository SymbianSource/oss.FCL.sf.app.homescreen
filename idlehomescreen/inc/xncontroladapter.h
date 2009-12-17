/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Xuikon control adapter header file
*
*/

#ifndef XNCONTROLADAPTER_H
#define XNCONTROLADAPTER_H

// System includes
#include <e32base.h>
#include <coecntrl.h>
#include <aknappui.h>
#include <aknlongtapdetector.h>

// Forward declarations
class CXnNodePluginIf;
class CXnComponent;
class CXnControlAdapterImpl;
class CXnProperty;
class CXnNode;

// Class declaration
/**
 * Control adapter interface class
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
class CXnControlAdapter : public CCoeControl,
                          public MCoeControlObserver,
                          public MAknLongTapDetectorCallBack
    {
public:
    // Data types
    enum TModeEvent
        {
        EEventNone = 0,
        EEventBackground,
        EEventLight,
        EEventDisplay,
        EEventVisibility
        };

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CXnControlAdapter* NewL( CXnNodePluginIf& aNode );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnControlAdapter();

public:
    // New functions

    /**
     * Sets component pointer.
     *
     * @since Series 60 3.1
     * @param aComponent Component pointer.
     */
    IMPORT_C void SetComponent( CXnComponent* aComponent );

    /**
     * Set component visibility
     *
     * @since Series 60 3.1
     * @param aVisible ETrue for making visible, EFalse for making invisible
     */
    IMPORT_C void SetVisible( TBool aVisible );

    /**
     * Gets the component pointer.
     *
     * @since Series 60 3.1
     * @return Component pointer.
     */
    IMPORT_C CXnComponent* Component();

    /**
     * Checks if the control should be drawn, according to visibility and
     * display properties.
     *
     * @since Series 60 3.1
     * @return ETrue if drawing is ok, EFalse otherwise
     */
    IMPORT_C TBool IsDrawingAllowed() const;

    /**
     * Gets content bitmaps. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aBitmap Bitmap to draw
     * @param aMask Mask to use
     */
    IMPORT_C void ContentBitmaps( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

    /**
     * Sets content bitmaps. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmap Bitmap to draw
     * @param aMask Mask to use
     */
    IMPORT_C void SetContentBitmaps( CFbsBitmap* aBitmap, CFbsBitmap* aMask );

    /**
     * Sets content bitmaps. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmapUrl Url of the bitmap to draw
     * @param aMaskUrl Url of the bitmap mask to use
     */
    IMPORT_C void SetContentBitmaps( const TDesC& aBitmapUrl,
        const TDesC& aMaskUrl );

    /**
     * Load a bitmap from the server. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmapUrl Url of the bitmap to load
     * @return Bitmap
     */
    IMPORT_C CFbsBitmap* LoadBitmap( const TDesC& aBitmapUrl );

    /**
     * Load a bitmap and mask from the server. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmapUrl Url of the bitmap to load
     * @param aMaskUrl Url of the mask to load, if any
     * @param aBitmap  Fetched bitmap
     * @param aMask Fetched mask
     */
    IMPORT_C void GetBitmapAndMask(
        const TDesC& aBitmapUrl,
        const TDesC& aMaskUrl,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask );

    /**
     * Skin change notification.
     *
     * @since Series 60 3.1
     */
    IMPORT_C virtual void SkinChanged();

    /**
     * Draws the control using a client-specified gc.
     *
     * @param aRect TRect to draw
     * @param aGc Gc used for drawing
     */
    IMPORT_C virtual void Draw( const TRect& aRect, CWindowGc& aGc ) const;

    /**
     * Enter power save mode. This is done when application goes to background
     * or lights go off.
     * Derived classes should override this function and stop animations, timers
     * etc. when the function is called.
     */
    IMPORT_C void EnterPowerSaveModeL( TModeEvent aEvent = EEventNone );

    /*
     * Template method, called by EnterPowerSaveModeL(). Additional
     * implementations are done in derived classes.
     */
    IMPORT_C virtual void DoEnterPowerSaveModeL( TModeEvent aEvent );

    /**
     * Exit power save mode. This is done when application comes to foreground
     * or lights go on.
     * Derived classes should override this function and restart animations,
     * timers etc. when the function is called.
     */
    IMPORT_C void ExitPowerSaveModeL( TModeEvent aEvent = EEventNone );

    /*
     * Template method, called by ExitPowerSaveModeL(). Additional
     * implementations are done in derived classes.
     */
    IMPORT_C virtual void DoExitPowerSaveModeL( TModeEvent aEvent );

    /**
     * Sets the application local zoom
     *
     * @param aZoom application local zoom
     * @since 3.1
     */
    IMPORT_C virtual void SetLocalUiZoomL( TAknUiZoom aZoom );

    /**
     * This called to measure adaptive content dimensions
     *
     * @since 3.2
     * @param aAvailableSize where the component's content must fit
     * @return The measure content dimensions
     */
    IMPORT_C virtual TSize MeasureAdaptiveContentL(
        const TSize& aAvailableSize );


    /**
     * Add a child adapter
     * @since Series 60 3.1
     * @param aChild Child adapter
     * @param aNode Child layout node.
     */
    void AppendChildL(CXnControlAdapter& aChild, CXnNode& aNode);


    /**
     * Resets the stylus state counter, so that the next tap causes 1st stylus
     * action.
     *
     * @since 3.2
     */
    IMPORT_C void ResetStylusCounter();

    /**
     * Sets the control blank
     *
     * @since S60 5.1
     * @param aBlank, ETrue sets blank, EFalse resets blank
     */
    IMPORT_C void SetBlank( TBool aBlank );

    /**
     * Gets the control's long tap detector
     *
     * @since 5.0
     */
    IMPORT_C CAknLongTapDetector* LongTapDetector() const;

    /**
     * Gets the control's associated window.
     *
     * @since 3.2
     */
    inline RWindow& Window() const;

    /**
     * Gets the component of this control which is grabbing the pointer.
     *
     * @since 5.0
     */
    inline CCoeControl* GrabbingComponent() const;

public:
    /**
     * From CCoeControl.
     * Handles key events.
     *
     * @since Series 60 3.1
     * @param aKeyEvent Key event.
     * @param aType Event type.
     * @return Key response
     */
    IMPORT_C TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * From CCoeControl.
     * Handles control events.
     *
     * @since Series 60 3.1
     * @param aControl Control.
     * @param aEventType Event type.
     */
    IMPORT_C void HandleControlEventL(
        CCoeControl* aControl,
        TCoeEvent aEventType );

    /**
     * From CCoeControl.
     * Handles pointer events
     *
     * This method must always be called, even when it's overridden by derived
     * classes
     *
     * @param aPointerEvent pointer event
     */
    IMPORT_C void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * From CCoeControl.
     * Handles pointer events
     *
     * This method must always be called, even when it's overridden by derived
     * classes
     *
     * @param aDrawNow Contains the value that was passed to it by SetFocus().      
     */    
    IMPORT_C void FocusChanged( TDrawNow aDrawNow );
    
    /**
     * From MAknLongTapDetectorCallBack.
     * Handles the long tap events.
     *
     * @since Series 60 3.1
     * @param aPenEventLocation Long tap event location relative to parent
     *        control.
     * @param aPenEventScreenLocation Long tap event location relative to
     *        screen.
     */
    IMPORT_C void HandleLongTapEventL(
        const TPoint& aPenEventLocation,
        const TPoint& aPenEventScreenLocation );

protected:
    // New functions

    /**
     * C++ default constructor.
     */
    IMPORT_C CXnControlAdapter();

    /**
     * Two-phased constructor.
     */
    IMPORT_C void ConstructL( CXnNodePluginIf& aNode );

    /**
     * Const-version of getting the component pointer.
     * For internal use only.
     *
     * @since Series 60 3.1
     * @return Component pointer.
     */
    CXnComponent* Component() const;

    /**
     * Draws content image.
     *
     * @since Series 60 3.1
     */
    IMPORT_C void DrawContentImage() const;

protected:
    /**
     * From CCoeControl.
     * Draws the control.
     *
     * This method must always be called, even when it's overridden by derived
     * classes
     *
     * @param aRect TRect to draw
     */
    IMPORT_C virtual void Draw( const TRect& aRect ) const;

    /**
     * From CCoeControl.
     * Return a child control by index.
     *
     * This method must not be overridden by derived classes
     *
     * @param aIndex Index of the child to return
     * @return Control at the given index
     */

    IMPORT_C CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     * Return count of children.
     *
     * This method must not be overridden by derived classes.
     *
     * @return Count of children
     */
    IMPORT_C TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * Size change notification
     */
    IMPORT_C virtual void SizeChanged();

public:
    /**
     * From CCoeAppUiBase.
     * Informs the components about the screen layout switch.
     */
    IMPORT_C virtual void HandleScreenDeviceChangedL();

    /**
     * Informs the components about the property change.
     */
    IMPORT_C void HandlePropertyChangeL( CXnProperty* aProperty = NULL );

    /*
     * Template method, called by HandlePropertyChangeL().
     * Additional implementations are done in derived classes.
     */
    IMPORT_C virtual void DoHandlePropertyChangeL(
        CXnProperty* aProperty = NULL );

    /**
     * Sets the state of the long tap animation
     */
    IMPORT_C void EnableLongTapAnimation( const TBool aAnimation );

    /**
     * Remove all child adapters
     */
    void RemoveChildAdapters();

    /**
     * Get child adapter list i.e which are not window owning controls
     */
    RPointerArray< CXnControlAdapter >& ChildAdapters();

    /**
     * Determines wheter control refuses focus loss
     */            
    virtual TBool RefusesFocusLoss() const { return EFalse; };
        
private:
    // Data

    /**
     * class implementation.
     * Own.
     */
    CXnControlAdapterImpl* iImpl;

    /**
     * Avkon long tap detector implementation.
     * Own.
     */
    CAknLongTapDetector* iLongTapDetector;
    };

// Inline functions
#include "xncontroladapter.inl"

#endif
