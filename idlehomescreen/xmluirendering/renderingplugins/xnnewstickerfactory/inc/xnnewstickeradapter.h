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

// CLASS DECLARATION

/**
* @ingroup group_xnnewstickerfactory
* 
*  Newsticker adapter class implementation.
*
*  @lib xnnewstickerfactory.dll
*  @since Series 60 3.2
*/
class CXnNewstickerAdapter : public CXnControlAdapter    
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
         * Restarts current title
         */
        void Restart();
        
        /**
         * Start showing the titles.
         */
        void Start( TBool aSelectTitle = EFalse );

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

    public: // from base classes

        /**
        * See CCoeControl documentation
        */      
        void MakeVisible( TBool aVisible );

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
        void ReportNewstickerEvent( const TDesC8& aEventName, TInt aIndex );
        
        /**
        * Draws text
        * Text drawing function is selected by scrolling behaviour
        */
        void DrawText( const TDesC& aText );
        
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
        void DoScroll( TBool aEndOfLoop );
        
        /**
        *  Draws text directly to screen if scrollins is not needed
        */  
        void DrawStaticText( CWindowGc& aGc, const TDesC& aText, TBool aTruncate = ETrue ) const;
        
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
        * The control handling the text title showing
        * Own.
        */
        CXnNewstickerControl*       iControl;
	    
        /**
         * Alternate delay, i.e. display time
         */  
	    TInt                        iAlternateDelay;        

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
       * is marquee control redraw needed
       */ 
       TBool                    iRedraw;
    };

#endif // XNNEWSTICKERADAPTER_H
            
// End of File
