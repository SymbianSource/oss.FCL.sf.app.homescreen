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
* Description:  Text scrolling functionality.
*
*/


#ifndef XNNEWSTICKERCONTROL_H
#define XNNEWSTICKERCONTROL_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CXnNewstickerAdapter;
class CFont;

// CLASS DECLARATION

/**
* @ingroup group_xnnewstickerfactory
* 
*  Newsticker control for XUIKON text scrolling functionality.
*
*  @lib xn3newstickerfactory.dll
*  @since Series 60 3.2
*/
class CXnNewstickerControl : public CBase
    {
    public:
	
        /**
         * Two-phased constructor.
         * @param aAdapter Parent control
         * @param aLoops Max number of loops to be executed
         * @param aScrollAmount The amount of pixels scrolled per iteration
         * @param aScrollDelay The interval between iterations
         * @param aStartDelay The delay before the animation starts
         * @return Pointer to this.
         */
        static CXnNewstickerControl* NewL(CXnNewstickerAdapter* aAdapter);
        
        /** 
        * Sets newsticker scroll amount.
        * @param aScrollAmount A scroll amount.
        * @return void
        */
        void SetScrollAmount(TInt aScrollAmount);

        /** 
        * Sets the rect of the visible area.
        * @param aContentRect The visible rect.
        * @return void
        */        
        void SetNewstickerRect(TRect& aContentRect);

        /** 
        * Checks whether a western layout is used.
        * @return Returns true if wesern layout is in use.
        */
        TBool IsWestern();

        // Routed from MXnNewstickerInterface.

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
         * Update title in the title list.
         * @param aTitle The new title to add.
         * @param aIndex The place to add the title.
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
         * Return the current svg title.
         * @return The current svg title.
         */
        const TDesC8& CurrentSvgTitle();

        /**
         * Return the number of titles in the list.
         * @return The number of titles in the list.
         */
        TInt TitleCount() const;

	    /**
        * Move to the next title.
        */
        void MoveToNextL();

	    /**
        * Move to the previous title.
        */
        void MoveToPrevL();

        /**
         * Move to first title
         */
        void MoveToFirstL();

        /**
         * Move to last title
         */
        void MoveToLastL();
	    
        /**
        * Move to the previous title.
        */
        void MoveToCurrent();

        /**
         * Sets font for bitmap graphic context.
         * @param aFont Font family.
         */
        void SetFont(CFont* aFont);

        /**
         * Sets text color for bitmap graphic context.
         * @param aColor Color in RGB format.
         */
        void SetTextColor(TRgb aColor);

        /**
         * Sets the underline style for all subsequently drawn text.
         * @param aStyle Underline style, which can be either on of off.
         */
        void SetTextUnderlineStyle(TFontUnderline aStyle);

        /**
         * Sets the strikethrough style for all subsequently drawn text.
         * @param aStyle Strikethrough style, which can be either on of off.
         */
        void SetTextStrikethroughStyle(TFontStrikethrough aStyle);
        
        /** 
        * Destructor
        */
        virtual ~CXnNewstickerControl();

        /**
         * Creates background bitmap
         */	
        void CreateBufferBitmapL();

	    /**
        * Advance text and issue a callback to parent
        */
        void DoScrollL();

	    /**
        * Blit visible stuff to d-buffer. Drawing direction is left-to-right.
        */
        void PrepareToDrawLtrL();

	    /**
        * Blit visible stuff to d-buffer. Drawing direction is right-to-left.
        */
        void PrepareToDrawRtlL();

	    /**
        * Blit double buffer to gc.
        */
        void Draw();
        
        /**
         * Blit double buffer to gc. Draws only the last title truncated.
         */
        void DrawStatic();

	    /**
        * Sets iFirstdrawingOffset as a start position.
        */
        void SetBeginningState();

        /**
        * Return the font
        * @return font
        */
        inline const CFont* Font() { return iFont; };

        /**
        * Return the text color.
        * @return Text color.
        */
        inline const TRgb TextColor() { return iTextColor; };

        /**
        * Return font underline flag.
        * @return Font underline flag.
        */
        inline const TFontUnderline TextUnderlineStyle() { return iUnderlining; };

        /**
        * Return the font strikethrough flag.
        * @return Font strikethrough flag.
        */
        inline const TFontStrikethrough TextStrikethroughStyle() { return iStrikethrough; };
        
        /**
         * Set scroll looping property
         */
        void SetScrollLooping( TBool aLoop );
        
        /**
         * Return scroll looping property
         * @return TBool
         */                
        TBool ScrollLooping();
        
        /**
         * @return ETrue if all the text fits on the drawing rect. In that case no scrolling and 
         * text alignment should be used
         */ 
        TBool TextFitInNewstickerRect();
        
        /**
         * @param aAlignment ( ELayoutAlignCenter, ELayoutAlignLeft or ELayoutAlignRight )
         * ELayoutAlignLeft default in western, ELayoutAlignRight in AH.
         */
        void SetTextAlignment(TInt aAlignment);
        
        /**
         * @return ETrue if the current text fits on
         * the drawing rect (when scrolling behaviour is scroll-alternate)
         * EFalse otherwise. 
         * If this is ETrue then no scrolling and text alignment should be used
         */ 
        TBool CalculateCurrentTextFitInNewstickerRect();

    private: 

        CXnNewstickerControl(CXnNewstickerAdapter* aAdapter);

        void ConstructL();

        /**
        * Calculate the indexes and offset after new title has been inserted.
        * @param aIndex The inserted index.
        */
        void CountIndexAfterInsertL(TInt aIndex);
        
        /**
         * @return ETrue if all the text fits on the drawing rect (when scrolling behaviour is slide)
         * EFalse otherwise. 
         * If this is ETrue then no scrolling and text alignment should be used
         */ 
        TBool CalculateTextFitInNewstickerRect();
        
        /**
         * @return text alignment from properties if text fits to the rect or scroll behaviour is alternate.
         * otherwise returns default alignment value (left in western, right in AH).  
         */
        CGraphicsContext::TTextAlign TextAlignment();
        /**
         * Get the index of the next title that has content
         * 
         * @param aStartSearch The index where to start search
         * @param aBackwards Search backwards
         * 
         * @return The next index that has content or -1 if nothing was found 
         */
        TInt GetNextTitleWithContent( TInt aStartSearch, TBool aBackwards = EFalse );

        TBool IsVisibleTitles();
        
    private:    // Data

        /**
        *  Helper class containing the title text and its length in pixels.
        *
        *  @lib xnnewstickerfactory.dll
        *  @since Series 60 3.2
        */
        class CTitleData : public CBase
            {
            public:

                /** 
                * Destructor
                */
                virtual ~CTitleData();

                /**
                * Two-phased constructor.
                * @param aTitle The title text.
                * @param aLenghtInPixels The lenght of title text in pixels.
                * @return Pointer to this.
                */
                static CTitleData* NewL(const TDesC& aTitle, TInt aLenghtInPixels);

                /**
                * Two-phased constructor.
                * @param aTitle The title text.
                * @param aLenghtInPixels The lenght of title text in pixels.
                * @return Pointer to this.
                */
                static CTitleData* NewLC(const TDesC& aTitle, TInt aLenghtInPixels);

                /**
                * Two-phased constructor.
                * @param aByteData The SVG title.
                * @param aLenghtInPixels The lenght of title text in pixels.
                * @return Pointer to this.
                */
                static CTitleData* NewL(const TDesC8& aByteData);

                /**
                * Two-phased constructor.
                * @param aByteData The SVG title.
                * @param aLenghtInPixels The lenght of title text in pixels.
                * @return Pointer to this.
                */
                static CTitleData* NewLC(const TDesC8& aByteData);

                /** 
                * Return the title text.
                * @return The title text.
                */
                const TDesC& TitleText();
                
                /**
                 * Set new title text
                 */
                void SetTitleTextL(const TDesC& aTitle);
                
                /** 
                * Return the title text lenght in pixels.
                * @return The title text lenght in pixels.
                */
                TInt TitleTextLengthInPixels();

                /** 
                * Return the title text lenght in pixels.
                * @param aLenghtInPixels The lenght of title text in pixels.
                */
                void SetTitleTextLengthInPixels(TInt aLenghtInPixels);

                /** 
                * ETrue if the title is svg, EFalse if it is text.
                * @return ETrue if the title is svg, EFalse if it is text.
                */
                TBool IsSvgTitle();

                /**
                * Return the svg title data.
                * @return The svg title data.
                */
                const TDesC8& SvgTitleData();            

            private:

                CTitleData(TInt aLenghtInPixels = 0);

                void ConstructL(const TDesC& aTitle);

                void ConstructL(const TDesC8& aByteData);

            private:    //  Data

                /**
                * ETrue if the title is SVG, EFalse if it is text.
                */
                TBool   iTitleIsSvg;

                /**
                * The title text. Owned.
                */
                HBufC*  iText;

                /**
                * The svg title. Owned.
                */
                HBufC8*  iData;

                /**
                * The title text length in pixels.
                */
                TInt    iTextLenghtInPixels;
            };

        /**
        * List of titles to show.
        */
        RPointerArray<CTitleData>   iTitles;

        /**
        * The index of the title which is drawn first.
        */
        TInt                        iFirstDrawingTitleIndex;

        /**
        * The number of pixels of the first title and separator image which
        * have already been shown. In other words, the number of the pixels
        * that are not visible in the screen. The iFirstDrawingTitleIndex
        * is updated when:
        * iFirstDrawingOffset > iTextLenghtInPixels + iSeparatorImageWidth
        */
        TInt                        iFirstDrawingOffset;

        /**
        * The width of the.separator image.
        */
        TInt                        iImageWidthInPixels;
        
        /**
        * The index of the title which is selected if user activates the
        * control.
        */
        TInt                        iCurrentTitleIndex;

        /**
        * The separator image. 
        * Owned.
        */
        CGulIcon*                   iSeparatorimage;

        /**
        * The separator image including the gap between image and text. 
        * Owned.
        */
        TInt                        iSeparatorImageWidth;

        /**
         * Font
         * Not own.
         */
    	CFont* 	                    iFont;

        /**
         * Baseline for the text.
         */
        TInt                        iTextBaseline;
        
        /**
         *  Color for the text
         */
        TRgb                        iTextColor;

        /**
         *  Whether to use underlining for the text
         */
        TFontUnderline              iUnderlining;

        /**
         *  Whether to use strikethrough for the text
         */
        TFontStrikethrough          iStrikethrough;

        /**
         * A rect in which the scrolling text is shown.
         */
        TRect                       iContentRect;

        /**
         * Amount of pixels the text moves in a tick.
         */	    
	    TInt                        iSpeed;
	    
	    /**
	     * Whether the scrolling texts will loop or not.
	     * If false, the scrolling will end at the last index.
	     */
	    TBool                       iScrollLooping;

        /**
         * Whether the text direction is LTR or RTL.
         */	    
        TBool                       iIsWestern;
        
        /**
         * Parent control adapter
         * Not own.
         */	            
        CXnNewstickerAdapter*       iAdapter;
        
        /**
         * ETrue if there is only one title, it fits in the rect and 
         * scroll behaviour is slide
         */
        TBool                       iTextFitInNewstickerRect;
        /**
         * Text alignment from the properties
         */
        TInt                        iTextAlignment;        
    };

#endif // XNNEWSTICKERCONTROL_H

// End of File
