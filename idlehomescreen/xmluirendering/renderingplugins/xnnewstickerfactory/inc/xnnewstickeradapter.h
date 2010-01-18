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
class CAknMarqueeControl;

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
          
        enum TScrollBehaviour
            {
            EScroll,
            EAlternate,
            EScrollAlternate,
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
        void AppendTitleL( const TDesC& aTitle );

        /**
         * Insert new title to the end of the title list.
         * @param aTitle The new title to add.
         * @param aIndex The place to add the title.
         */
        void InsertTitleL( const TDesC& aTitle, TInt aIndex );

        /**
         * Update title text.
         * @param aTitle The new title to add.
         * @param aIndex The place to update the title.
         */
        void UpdateTitleL( const TDesC& aTitle, TInt aIndex );

        /**
         * Delete the title.
         * @param aIndex The title index to delete.
         */
        void DeleteTitleL( TInt aIndex );

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
        const TDesC& Title( TInt aIndex );

        /**
         * Delete all titles.
         */
        void ClearTitles();

        /**
         * Set callback interface.
         * @param aCallback The callback interface pointer.
         */
        void SetCallbackInterfaceL(
            XnNewstickerInterface::MXnNewstickerCallbackInterface* aCallback );

        /**
         * Start showing the titles.
         */
        void Start();

        /**
         * Stop showing the titles.
         */
        void Stop();
        
        /**
         * Stops alternate counter.
         */
        void StopAlternateCounter();
        
        /**
         * Resets and unenables narquee control.
         */
        void StopMarquee();
        
        /**
         * Starts alternate counter.
         */
        void StartAlternateCounter();

        /**
         * Callback which will be called by the CPeriodic.
         * @param aPtr User defined pointer.
         * @return ETrue if the method should be called again.
         */
        static TInt PeriodicEventL( TAny * aPtr );
     
        void MakeVisible( TBool aVisible );
        
    public: // from base classes

        //  From MXnNewstickerCallbackInterface
        /**
         * Called when the title has been shown and is now offscreen.
         * @param aTitleIndex The title that has been completely shown.
         */
        void TitleScrolled( TInt aTitleIndex );

        /**
        * See CCoeControl documentation
        */    	
        void SizeChanged();

        /**
        * See CXnControlAdapter documentation
        */    	
    	void Draw( const TRect& aRect ) const;

        /**
        * Enter power save mode. This is done when application goes to background or lights go off. 
        * Derived classes should override this function and stop animations, timers etc. when the function is called.
        */ 
        void DoEnterPowerSaveModeL( TModeEvent aEvent );

	   	/**
        * Exit power save mode. This is done when application comes to foreground or lights go on. 
        * Derived classes should override this function and restart animations, timers etc. when the function is called.
        */ 
        void DoExitPowerSaveModeL( TModeEvent aEvent );

        /**
        * From CXnControlAdapter Handles the property changes.
        * @since Series 60 3.1
        * @return void.
        */ 
        void DoHandlePropertyChangeL( CXnProperty* aProperty = NULL );

	    /**
        * Skin change notification.
        * See CXnControlAdapter documentation
        */        
        void SkinChanged();
        
        /**
        * Reports newsticker event
        * 
        */   
        void ReportNewstickerEvent( const TDesC8& aEventName );
        
        /**
        * Draws text
        * Text drawing function is selected by scrolling behaviour
        */
        void DrawText( const TDesC& aText, const TRect& aRect  );
        
        /**
        * ETrue if marquee control redrawing is needed. EFalse if truncated text is needed
        */
        TBool Redraw() { return iRedraw; }


    private:

    	CXnNewstickerAdapter( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );

    	void ConstructL( CXnControlAdapter* aParent );
    	
    	/**
    	* Sets text properties, such as color, font, etc.
    	*/
    	void SetTextPropertiesL();	

    	/**
    	* Sets newsticker timing properties.
    	*/
        void SetNewstickerPropertiesL();
        
        /**
         * From CoeControl,CountComponentControls.
         */
         TInt CountComponentControls() const;

        /**
         * From CCoeControl,ComponentControl.
         */
         CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**
        * Callback function for marquee control
        */    
        static TInt RedrawCallback( TAny* aPtr );
        
        /**
        *  FScrolls alternative text. Function is called by periodic timer
        */    
        void DoScroll();
        
        /**
        *  Draws text directly to screen if scrollins is not needed
        */  
        void DrawStaticText( CWindowGc& aGc, const TDesC& aText ) const;
        
        /**
        *  Draws scrolling text to screen via marquee control
        */ 
        TBool DrawMarqueeText( CWindowGc& aGc, const TDesC& aText ) const;               
        
    private:

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
         * The delay before first event
         */  
	    TInt                        iAlternateDelay;
        
        /**
         * The alternate interval between ticks.
         */	    
	    TInt                        iAlternateInterval;

        /**
        * UI node
        * Not own.
        */
        CXnNodePluginIf&            iNode;

        /**
        * Font for control.
        * Not own.
        */        
        CFont*                      iFont;
        
        /**
        * Whether the font needs to be released or not.
        */        
        TInt                       iReleaseFont;

        /**
        * For keeping track of the current powersave mode state
        */       
        TBool                       iPowerSaveMode;

        /**
        * Current scroll behaviour
        */               
        TScrollBehaviour            iScrollBehaviour;
        
        /**
         * Restart animation after title has been updated
         */
        TBool                       iRestartAfterUpdate;
        
        /**
        * marquee control for scrolling
		* own
        */ 
       CAknMarqueeControl*         iMarqueeControl;
       
       /**
       * Current text color
       */ 
       TRgb                        iTextColor;
       
       /**
       * Current text alignment
       */ 
       TInt                        iTextAlignment;   
       
       /**
       * Current baseline
       */ 
       TInt                        iTextBaseline;
       
       /**
       * Current underlining behaviour
       */ 
       TFontUnderline              iUnderlining;       
       
       /**
       * Current strikethrough behaviour
       */ 
       TFontStrikethrough          iStrikethrough;
       
       /**
       * is scroll looping activated
       */ 
       TBool                    iScrollLooping;
       
       /**
       * is marquee control redraw needed
       */ 
       TBool                    iRedraw;
        
        
        
    };

#endif // XNNEWSTICKERADAPTER_H
            
// End of File
