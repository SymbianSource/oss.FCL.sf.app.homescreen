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
* Description:  Xuikon control adapter header file
*
*/


#ifndef __XNCONTROLADAPTERIMPL_H__
#define __XNCONTROLADAPTERIMPL_H__

// System includes
#include <e32base.h>
#include <coecntrl.h>
#include <AknIconUtils.h>
#include <AknsEffectAnim.h>
#include <AknsItemID.h>
#include <akntouchgesturefw.h>

// Forward declarations
class CXnNodePluginIf;
class CXnComponent;
class CWindowGc;
class CXnControlAdapter;
class CXnNode;
class CXnProperty;
class CGulIcon;
class CXnUiEngine;
class CAknPointerEventSuppressor;

//Constants
_LIT( KMif, "mif(" );
_LIT8( KMenuBarNode, "menubar" );
const TInt KSpaceChar = 32;
const TInt KRightParenthesis = ')';

using namespace AknTouchGestureFw;
// Class declaration

/**
*  Control adapter class implementation.
* 
*  @ingroup group_xnlayoutengine 
*  @lib xn3layoutengine.lib
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CXnControlAdapterImpl ) : public CBase, 
    public MAknsEffectAnimObserver,
    public MAknTouchGestureFwObserver
    {
public:
    
    /**
     * Response of property change handled/not handled
     */
    enum TPropertyChangeResponse
        {
        /** The property change was not handled. */
        EPropertyChangeNotConsumed,
        /** The property change was handled. */
        EPropertyChangeConsumed
        };

public:
    
    /**
     *  Icon provider used by AknIconUtils
     *  
     *  @since Series 60 3.1
     */
    class TIconProvider : public MAknIconFileProvider
        {
    public:
        /**
         * Constructor
         * 
         * @param aFile File handle to use
         */
        TIconProvider( RFile& aFile );
        
    public:
        
        /**
         * Destructor.
         */
        virtual ~TIconProvider();
        
        // Functions from base classes
        /**
         * From MAknIconFileProvider Returns an open file handle to the icon 
         * file.
         * This method should leave if an icon file with specified type does
         * not exist. That may be the case e.g. with MBM file,
         * if there are no bitmap icons.
         *
         * Note! RFs::ShareProtected must be called to the RFs instance used
         * for opening the file.
         *
         * @param aFile Icon file should be opened in this file handle, which
         * is an empty file handle, when the AknIcon framework calls this 
         * method. The AknIcon framework takes care of closing the file handle 
         * after having used it.
         * 
         * @param aType Icon file type.
         */
        virtual void RetrieveIconFileHandleL( RFile& aFile,
            const TIconFileType /*aType*/);
        
        /**
         * From MAknIconFileProvider With this method, AknIcon framework 
         * informs that it does not use this MAknIconFileProvider instance 
         * any more. After this call,
         * it is ok to delete the object. This can be implemented simply
         * e.g. by deleting self in this callback.
         * Normally, this callback is invoked when the icon in question
         * is deleted.
         * Note, however, that if the same MAknIconFileProvider instance is
         * supplied in multiple CreateIcon calls, then it must be accessible
         * by AknIcon framework until it has signalled a matching amount
         * of these callbacks.
         */
        virtual void Finished();
        
    private:
        
        // file to use
        RFile iFile;
        };

public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     * 
     * @param aNode 
     * @param aDadapter Control adapter
     * @param gc Window gc
     */
    static CXnControlAdapterImpl* NewL( CXnNodePluginIf& aNode,
        CXnControlAdapter& aAdapter, CWindowGc& gc );

    /**
     * Destructor.
     */
    virtual ~CXnControlAdapterImpl();

public:
    // New functions
    
    /**
     * Sets component pointer.
     *
     * @since Series 60 3.1
     * @param aComponent Component pointer.
     */
    void SetComponent( CXnComponent* aComponent );

    /**
     * Gets the component pointer.
     *
     * @since Series 60 3.1
     * @return Component pointer.
     */
    CXnComponent* Component();

    /**
     * Const-version of getting the component pointer. 
     *
     * @since Series 60 3.1
     * @return Component pointer.
     */
    CXnComponent* Component() const;

    /**
     * Add a child adapter
     *
     * @since Series 60 3.1
     * @param aChild Child adapter
     */
    void AppendChildL(CXnControlAdapter& aChild, CXnNode& aNode);
       
    /**
     * Checks if the control should be drawn, according to visibility and 
     * display properties.
     *
     * @since Series 60 3.1
     * @return ETrue if drawing is ok, EFalse otherwise
     */
    TBool IsDrawingAllowed() const;

    /**
     * Gets content bitmaps. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aBitmap Bitmap to draw
     * @param aMask Mask to use
     */
    void ContentBitmaps( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

    /**
     * Sets content bitmaps. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmap Bitmap to draw
     * @param aMask Mask to use
     */
    void SetContentBitmaps( CFbsBitmap* aBitmap, CFbsBitmap* aMask );

    /**
     * Sets content bitmaps. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmapUrl Url of the bitmap to draw
     * @param aMaskUrl Url of the bitmap mask to use
     */
    void SetContentBitmapsL( const TDesC& aBitmapUrl, const TDesC& aMaskUrl );

    /**
     * Load a bitmap from the server. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmapUrl Url of the bitmap to load
     * @return Bitmap
     */
    CFbsBitmap* LoadBitmap( const TDesC& aBitmapUrl );

    /**
     * Load a bitmap and mask from the server. Ownership is transferred.
     *
     * @since Series 60 3.1
     * @param aBitmapUrl Url of the bitmap to load
     * @param aMaskUrl Url of the mask to load, if any
     * @param aBitmap  Fetched bitmap
     * @param aMask Fetched mask
     * @return void
     */
    void GetBitmapAndMask( const TDesC& aBitmapUrl, const TDesC& aMaskUrl,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

    /**
     * Handles pointer events
     * This method must always be called, even when it's overridden by derived
     * classes
     *
     * @param aPointerEvent pointer event
     */
    TBool HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
    /**
     * Draws the control
     * This method must always be called, even when it's overridden by derived
     * classes
     *
     * @param aRect TRect to draw
     */
    void Draw( const TRect& aRect ) const;
    
    /**
     * Draws the control using a client-specified gc.
     *
     * @param aRect TRect to draw
     * @param aGc Gc used for drawing
     */
    void Draw( const TRect& aRect, CWindowGc& aGc ) const;

    /**
     * Return a child control by index
     * This method must not be overridden by derived classes
     *
     * @param aIndex Index of the child to return
     * @return Control at the given index
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
    /**
     * From CCoeControl Return count of children
     * This method must not be overridden by derived classes
     *
     * @return Count of children
     */
    TInt CountComponentControls() const;

    /**
     * Draws content image.
     *
     * @since Series 60 3.1
     */
    void DrawContentImage() const;

    /**
     * Draws content image.
     *
     * @param aGc Gc used for drawing
     * @since Series 60 3.1
     */
    void DrawContentImageL( CWindowGc& aGc ) const;

    /**
     * Size change notification
     */
    void SizeChanged();

    /**
     * Skin change notification
     */
    void SkinChanged();

    /**
     * Focus changed notification
     * 
     * @since S60 5.0
     * @param aFocused Control's current foucs state      
     */
    void FocusChangedL( TBool aFocused );

    /**
     * Initialize or refresh background and border bitmaps
     * This is called when for example the skin or control size is changed
     */
    void InitializeBackgroundAndBorderBitmapsL( TBool aForceRecreate = EFalse );

    /** 
     * This is called to measure adaptive content dimensions.
     *
     * @since 3.2
     * @param aAvailableSize Not used
     * @return For now it returns default size - zero
     */
    TSize MeasureAdaptiveContentL( const TSize& aAvailableSize );

    /** 
     * Sets the control blank
     * 
     * @since S60 5.1
     * @param aBlank, ETrue sets blank, EFalse resets blank
     */
    void SetBlank( TBool aBlank );

    /** 
     * Creates trigger node for swipe event
     * 
     * @since S60 5.1
     * @param aUiEngine reference to uiengine
     * @param aDirection Direction of swipe
     * @return trigger node for swipe
     */
    CXnNode* BuildSwipeTriggerNodeLC( CXnUiEngine& aUiEngine,
        const TDesC8& aDirection );

public:
    // Functions from base classes

    /**
     * From CCoeControl Handles key events.
     *
     * @since Series 60 3.1
     * @param aKeyEvent Key event.
     * @param aType Event type.
     * @return Key response
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        TEventCode aType );

    /**
     * From MAknLongTapDetectorCallBack. Handles the long tap events.
     *
     * @since Series 60 3.1
     * @param aPenEventLocation Long tap event location relative to parent 
     *        control.
     * @param aPenEventScreenLocation Long tap event location relative to 
     *        screen.
     */
    void HandleLongTapEventL( const TPoint& aPenEventLocation,
        const TPoint& aPenEventScreenLocation );

    /**
     * Creates highlight animation from a skin id when one is available.
     */
    void CreateAnimationL();

    /**
     * MAknsEffectAnimObserver callback. Draws the adapter again and renders
     * the animated area.
     * 
     * @param aError Error - zero if error, nonzero otherwise
     */
    virtual void AnimFrameReady( TInt aError, TInt );

    /**
     * Starts highlight animation.
     *
     * @param aAnimRestart When true, the animation is stopped and then 
     *        started again.
     */
    void StartHighlightAnimation( TBool aAnimRestart = EFalse );

    /**
     * Stops highlight animation.
     */
    void StopHighlightAnimation();

    /**
     * Handles property change
     */
    void DoHandlePropertyChangeL( CXnProperty* aProperty = NULL );

    /**
     * Handles change of device screen
     */
    void HandleScreenDeviceChangedL();

    /**
     * Updates background image
     * 
     * @param aProperty If it is background image, initializes background 
     *        bitmap
     * @return CXnControlAdapterImpl::TPropertyChangeResponse whether it was 
     *         consumed
     */
    CXnControlAdapterImpl::TPropertyChangeResponse UpdateBackgroundImageL(
        CXnProperty* aProperty );

    /**
     * Sets up gesture on buttondown event
     * 
     * @param aPointerEvent Event to be forwarded
     */
    void InitializeGestureL(
        const TPointerEvent& aPointerEvent );

    /*
     * Resets child adapters
     */
    void RemoveChildAdapters();
    
    /*
     * Returns Child adapters
     * 
     * @return Returns child adapters 
     */
    RPointerArray< CXnControlAdapter >& ChildAdapters();

private: // from MAknTouchGestureFwObserver
    /**
     * Implements gesture handling
     * 
     * @see MAknTouchGestureFwObserver
     */
    void HandleTouchGestureL( MAknTouchGestureFwEvent& aEvent );

protected:
    // New functions

    /**
     * C++ default constructor.
     */
    CXnControlAdapterImpl( CXnNodePluginIf& aNode );

    /**
     * Two-phased constructor.
     */
    void ConstructL( CXnNodePluginIf& aNode, CXnControlAdapter& aAdapter,
        CWindowGc& gc );

private:
    void DoDrawL( const TRect& aRect, CWindowGc& aGc ) const;
    void DrawBackgroundDataL( const TRect& aRect, CXnNode& aNode,
        CWindowGc& aGc );
    void DrawEditModeBgData( CXnNode& aNode, CWindowGc& aGc );
    void DrawPlusSign( CXnNode& aNode, CWindowGc& aGc );
    void DrawTransparentColorL( CXnNode& aNode, CWindowGc& aGc,
        CFbsBitmap* aMask );
     void DrawBackgroundSkinL( CXnNode& aNode, CFbsBitmap* aMask, 
        CWindowGc& aGc,CXnProperty* aBgColor, TRect aRect = TRect::EUninitialized );
    void DrawBackgroundSkin(const TAknsItemID& aSkinId, CWindowGc& aGc, 
        TRect aRect );
    void DrawBackgroundImageL( const TRect& aRect, CXnNode& aNode,
        CWindowGc& aGc, CFbsBitmap* aBitmap, CFbsBitmap* aMask );
    RFs& FsSession();
    TBool IsDragThresholdExceeded( const TPoint& aPoint );

private:
    // Data 
    
    /** Node */
    CXnNodePluginIf& iNode;
    /** Pointer to component base object. */
    CXnComponent* iComponent;
    /** child controls */
    mutable RPointerArray< CXnControlAdapter > iChildren;
    /** content image */
    mutable CFbsBitmap* iContentBitmap;
    /** content bitmap index */
    mutable TInt iContentBitmapIndex;
    /** content mask */
    mutable CFbsBitmap* iContentMask;
    /** content mask index */
    mutable TInt iContentMaskIndex;
    /** background image */
    mutable CFbsBitmap* iBackgroundBitmap;
    /** background mask */
    mutable CFbsBitmap* iBackgroundMask;
    /** background image index */
    mutable TInt iBackgroundBitmapIndex;
    /** border image */
    mutable CFbsBitmap* iBorderBitmap;
    /** border image index */
    mutable TInt iBorderBitmapIndex;
    /** border image split value for top border */
    mutable TInt iBorderBitmapDividerTop;
    /** border image split value for right border */
    mutable TInt iBorderBitmapDividerRight;
    /** border image split value for bottom border */
    mutable TInt iBorderBitmapDividerBottom;
    /** border image split value for left border */
    mutable TInt iBorderBitmapDividerLeft;
    /** graphics context */
    mutable CWindowGc* iGc;
    /** control adapter */
    mutable CXnControlAdapter* iAdapter;
    /** icon provider, for AknIconUtils */
    mutable TIconProvider* iIconProvider;
    
    /** skin animation object for skin highlight animation */
    CAknsEffectAnim* iAnimation;
    /** skin animation identifier */
    TAknsItemID iAnimIID;
    /** flag: skin animation id is resolved from the CSS property */
    TBool iAnimIDResolved;
    /** flag: for storing blank state */
    TBool iBlank;
    /** flag: to detect whether longtap occured */
    TBool iLongtap;    
    /** Gesture framework, owned. */
    CAknTouchGestureFw* iGestureFw;
    /** Gesture destination, Not owned. */        
    CXnNode* iGestureDestination;
    /** Starting point of button down event. */        
    TPoint iButtonDownStartPoint;
    /** Tells is background images are allready created*/
    TBool iBackgrondInitialized;
    /** Tells is border images are allready created*/
    TBool iBorderInitialized;
    /** Suppress drag events, owned */
    CAknPointerEventSuppressor* iSuppressor;
    };

#endif //__XNCONTROLADAPTERIMPL_H__
