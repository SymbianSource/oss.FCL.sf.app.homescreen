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
* Description:  Wrapper class for Newsticker text label.
*
*/


#ifndef _XNNEWSTICKERADAPTER_H
#define _XNNEWSTICKERADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"
#include "xnnewsticker.h"

// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CXnNewstickerControl;
class CXnNewstickerSvgControl;

// CONSTANTS
// The default values for the newsticker control
const TInt KXnNewstickerDelay = 1000000;   // start scrolling after a delay of 1 second
const TInt KXnNewstickerInterval = 100000; // scroll 10 times in a second
const TInt KXnNewstickerScrollAmount = 6;  // scroll 6 pixels at time
const TInt KXnNewstickerLoops = 1;         // loop amount
const TInt KNoDelay = 0;                   //  No delay to start the animation
const TInt KAnimationTime = 6000000;       //  The animation lasts 6 seconds

// CLASS DECLARATION

/**
* @ingroup group_xnnewstickerfactory
* 
*  Newsticker adapter class implementation.
*
*  @lib xnnewstickerfactory.dll
*  @since Series 60 3.2
*/
class CXnNewstickerAdapter : public CXnControlAdapter,
            XnNewstickerInterface::MXnNewstickerCallbackInterface
    {
    public:

        /**
        * State of the whole control.
        */
        enum TState
            {
            ENotStarted = 1,            //  No scrolling has been done
            EPaused,                //  Text is shown, but not scrolled
            EText,                  //  Text is shown and scrolled continuously
            EAnimation,             //  SVG animation is shown
            EScrollEnded            //  Looping is false and scrolling has ended
            };
            
        enum TScrollBehaviour
            {
            EScroll,
            ESlide,
            EAlternate,
            EScrollAlternate
            };            
            
    public:

    	/**
    	* 2 phase construction.
        * @param aParent Parent control
        * @param aNode UI node
    	*/
    	static CXnNewstickerAdapter* NewL(CXnControlAdapter* aParent, CXnNodePluginIf& aNode);

    	/**
    	* Destructor.
    	*/
    	virtual ~CXnNewstickerAdapter();

    public: // New functions

        /**
         * Append new title to the end of the title list.
         * @param aTitle The new title to add.
         */
        void AppendTitleL(const TDesC& aTitle);

        /**
         * Insert new title to the end of the title list.
         * @param aTitle The new title to add.
         * @param aIndex The place to add the title.
         */
        void InsertTitleL(const TDesC& aTitle, TInt aIndex);

        /**
         * Update title text.
         * @param aTitle The new title to add.
         * @param aIndex The place to update the title.
         */
        void UpdateTitleL(const TDesC& aTitle, TInt aIndex);

        /**
         * Delete the title.
         * @param aIndex The title index to delete.
         */
        void DeleteTitleL(TInt aIndex);

    	/**
    	 * Returns index of the currently selected title.
    	 * @return The index of the current title.
    	 */
        TInt CurrentTitleIndex();

    	/**
    	 * Return the title of the given index.
    	 * @param aIndex The index to return the title.
    	 * @return The title of the given index.
    	 */
        const TDesC& Title(TInt aIndex);

    	/**
    	 * Set the separator image for all titles.
    	 * @param aIcon The separator image. This must be mif icon.
    	 * @return KErrNone if successful, KErrArgument if the icon is not mif.
    	 */
        TInt SetSeparatorImageL(CGulIcon* aIcon);

        /**
         * Delete all titles.
         */
        void ClearTitles();

        /**
         * Append the SVG title to be shown.
         * @param aByteData The SVG data.
         */
        void AppendSvgTitleL(const TDesC8& aByteData);

        /**
         * Insert the SVG title to be shown.
         * @param aByteData The SVG data.
         */
        void InsertSvgTitleL(const TDesC8& aByteData, TInt aIndex);

        /**
         * Set callback interface.
         * @param aCallback The callback interface pointer.
         */
        void SetCallbackInterfaceL(
            XnNewstickerInterface::MXnNewstickerCallbackInterface* aCallback);

        /**
         * Start showing the titles.
         */
        void StartL();

        /**
         * Stop showing the titles.
         */
        void StopL();

        /**
         * Show SVG next.
         */
        void ShowSvgL();

        /**
         * Callback which will be called by the CPeriodic.
         * @param aPtr User defined pointer.
         * @return ETrue if the method should be called again.
         */
        static TInt PeriodicEventL(TAny * aPtr);

        /**
         * Draw background for the newsticker control area.
         * @param aRect The visivle area of newsticker component.
         * @param aGc Window graphics context.
         */
        void DrawBackgroundL(const TRect& aRect, CWindowGc& aGc) const;

        
        /**
        * Returns the bitmap context.
        * @return Bitmap context.
        */
        inline CBitmapContext* BufferGc() const { return iBufferGc; };
        
        
        /**
        * Returns the background bitmap
        * @return Background bitmap
        */
        inline const CFbsBitmap* BackgroundBitmap() const { return iBackgroundBitmap; };

        /**
        * Returns the state of the newsticker control.
        * @return Current state.
        */
        inline TState CurrentState() const { return iState; };

        /**
        * Returns the current scroll behaviour
        * @return current scroll behaviour
        */        
        inline TInt ScrollBehaviour() const { return iScrollBehaviour; };        
        
        /**
        * Reset periodic timer to set display time.
        */
        void SetTimerToDisplaytime();
        
        /**
        * Reset periodic timer to set scroll delay time.
        */
        void SetTimerToScrolltime();
        
    public: // from base classes

        //  From MXnNewstickerCallbackInterface
        /**
         * Called when the title has been shown and is now offscreen.
         * @param aTitleIndex The title that has been completely shown.
         */
        void TitleScrolled(TInt aTitleIndex);

        /**
        * See CCoeControl documentation
        */    	
        void SizeChanged();

        /**
        * See CCoeControl documentation
        */    	
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
        * See CCoeControl documentation
        */    	    	
        void FocusChanged(TDrawNow aDrawNow);

        /**
        * See CCoeControl documentation
        */
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent, TEventCode aType);

        /**
        * See CXnControlAdapter documentation
        */    	
    	void Draw(const TRect& aRect) const;

        /**
        * Enter power save mode. This is done when application goes to background or lights go off. 
        * Derived classes should override this function and stop animations, timers etc. when the function is called.
        */ 
        void DoEnterPowerSaveModeL(TModeEvent aEvent);

	   	/**
        * Exit power save mode. This is done when application comes to foreground or lights go on. 
        * Derived classes should override this function and restart animations, timers etc. when the function is called.
        */ 
        void DoExitPowerSaveModeL(TModeEvent aEvent);

        /**
        * From CXnControlAdapter Handles the property changes.
        * @since Series 60 3.1
        * @return void.
        */ 
        void DoHandlePropertyChangeL(CXnProperty* aProperty = NULL);

	    /**
        * Skin change notification.
        * See CXnControlAdapter documentation
        */        
        void SkinChanged();
        
        void ReportNewstickerEventL(const TDesC8& aEventName);
        
        /**
        * See CCoeControl documentation
        */
        void DrawNow() const;
        
        /**
        * If newsticker is not focusable, finds and draws focusable parent
        */
        TBool DrawFocusableParentL() const;

    private:

    	CXnNewstickerAdapter(CXnControlAdapter* aParent, CXnNodePluginIf& aNode);

    	void ConstructL();
    	
    	/**
    	* Sets text properties, such as color, font, etc.
    	*/
    	void SetTextPropertiesL();	

    	/**
    	* Sets newsticker timing properties.
    	*/
        void SetNewstickerPropertiesL();

    	/**
    	* Creates a new doublebuffer for newsticker.
    	*/        
        void UpdateDoubleBufferL();

    	/**
    	* Draws a reconstructed background for newsticker.
    	*/
        void UpdateBackgroundL() const;

    	/**
    	* Same as SizeChanged(), but this is an L function.
    	*/
        void SizeChangedL();

    	/**
    	* Checks if feed can be started
    	*/       
        TBool CheckDisplayL( CXnNodePluginIf& aNode );

        void RestartL();
    private:

        /**
        * The state of the newsticker control.
        */
        TState                      iState;

        /**
        * The timer to use for scrolling events.
        * Own.
        */
	    CPeriodic*                  iPeriodicTimer;

        /**
        * The callback interface which is used when a title has been shown.
        * Not own.
        */
        XnNewstickerInterface::MXnNewstickerCallbackInterface*  iCallback;

        /**
        * The control handling the text title showing
        * Own.
        */
        CXnNewstickerControl*       iControl;
        
        /**
        * The control handling the svg title showing
        * Own.
        */
        CXnNewstickerSvgControl*    iSvgControl;

        /**
         * Delay between loops in microseconds.
         */
	    TInt                        iDelay;

        /**
         * The interval between ticks.
         */	    
	    TInt                        iInterval;
        
        /**
         * The alternate interval between ticks.
         */	    
	    TInt                        iAlternateInterval;

        /**
         * The current interval between ticks.
         */	    
	    TInt                        iCurrentInterval;

        /**
         * Amount of loops executed
         */	    
        TInt                        iLoops;

        /**
        * The animation time in microseconds.
        */
        TInt                        iAnimationTime;

        /**
        * UI node
        * Not own.
        */
        CXnNodePluginIf&            iNode;

        /**
        * Font for control.
        * Own.
        */        
        CFont*                      iFont;
        
        /**
        * Whether the font needs to be released or not.
        */        
        TBool                       iReleaseFont;
                
        /**
        * Bitmap device. Used to draw on the drawing buffer
        * Own.
        */
    	CFbsBitmapDevice*           iBufferDevice;
	
        /**
        * Drawing buffer
        * Own.
        */
    	CFbsBitmap*                 iDrawingBuffer;
    	
        /**
        * Bitmap Graphic Context. Used to draw on the drawing buffer
        * Own.
        */
        CBitmapContext*             iBufferGc;
        
        /**
        * Pointer to background bitmap.
        * Own.
        */
        mutable CFbsBitmap*         iBackgroundBitmap;

        /**
        * For the display property (content is either shown or not)
        */
        TBool                       iDisplay;

        /**
        * For keeping track of the current powersave mode state
        */       
        TBool                       iPowerSaveMode;

        /**
        * Current scroll behaviour
        */               
        TInt                        iScrollBehaviour;
        
        /**
         * Restart animation after title has been updated
         */
        TBool                       iRestartAfterUpdate;
    };

#endif // XNNEWSTICKERADAPTER_H
            
// End of File
