/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants for the matrixmenu
*  Version     : %version: 22 % << Don't touch! Updated by Synergy at check-out.
*
*/



#ifndef MMMARQUEEADAPTER_H
#define MMMARQUEEADAPTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AknMarqueeControl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <eiklbx.h>

// CLASS DECLARATION
/**
 *  Used by Multimedia Menu to display marquee 
 * 
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmMarqueeAdapter ) : public CBase
	{


private: 
	/**
 	*  Used to keep currently drawn marquee element
 	*  @code
 	*  @endcode
 	*  @lib mmwidgets
 	*  @since S60 v3.0
 	*  @ingroup group_mmwidgets
 	*/
    NONSHARABLE_CLASS( CMmMarqueeElement ) : public CBase
		{
		
		friend class CMmMarqueeAdapter;
		
		// No implementation required
		public:
		
	    /**
	     * Constructor
	     * 
	     * @since S60 v3.0
	     * @param aElementRect The rect to draw marquee
	     * @param aAlign Align of text
	     * @param aFont Font
	     * @param aColor Color
	     * @param aIndex Item subcell index
	     * @param aBaselineOffset Baseline offset
	     */
		CMmMarqueeElement (
				TRect aElementRect,
				CGraphicsContext::TTextAlign aAlign,
				TAknLogicalFontId aFont,
				TRgb aColor, 
				TInt aIndex,
				TInt aBaselineOffset );

	    /**
	     * Destructor
	     * 
	     * @since S60 v3.0
	     */
		~CMmMarqueeElement ();
	
	    /**
	     * Two-phased constructor.
	     * 
	     * @since S60 v3.0	     
	     * @param aElementRect The rect to draw marquee.
	     * @param aAlign Align of text.
	     * @param aText Text do be drawn.
	     * @param aFontId Font's id.
	     * @param aColor Color.
	     * @param aIndex Item subcell index.
	     * @param aBaselineOffset Baseline offset.
	     */
		static CMmMarqueeElement* NewL (
				TRect aElementRect,
				CGraphicsContext::TTextAlign aAlign,
				const TDesC& aText,
				TAknLogicalFontId aFontId,
				TRgb aColor, 
				TInt aIndex,
				TInt aBaselineOffset );

	    
	    /**
	     * Setup marquee scroll data
	     * 
	     * @since S60 v3.0
	     * @param aLoops Number of loops to be drawn
	     * @param aScrollAmount Scroll amount 
	     * @param aScrollDelay Scroll delay
	     */
		void SetupMarqueeControlL(
				TInt aLoops,
				TInt aScrollAmount,
				TInt aScrollDelay );
	
		
	private:
    
	    /**
	     * ConstructL construct marquee element
	     * 
	     * @since S60 v3.0
	     * @param aText Text for element
	     */
		void ConstructL ( const TDesC& aText );
		
	private:
    
		    /**
		     * Element rectangle ( according to item rect )
		     */
			TRect iElementRect;
	        
	        /**
	         * Align of text
	         */
			CGraphicsContext::TTextAlign iAlign;
	        
	        /**
	         * LCT layouts map
	         * Own
	         */
			HBufC* iText; 
	        
	        /**
	         * Font 
	         */
			TAknLogicalFontId iFont;
	        
	        /**
	         * Color
	         */
			TRgb iColor;
	        
	        /**
	         * Loops to be drawn
	         */
			TInt iLoops; // default 1
	        
	        /**
	         * Scroll amount
	         */
			TInt iScrollAmount; // default 6
	        
	        /**
	         * Scroll delay
	         */
			TInt iScrollDelay; // default 1000000
	        
	        /**
	         * Marquee control
	         * Own
	         */
			CAknMarqueeControl* iMarqueeControl;
	        
	        /**
	         * Subcell index of marquee element
	         */
			TInt iSubcellIndex;
	        
	        /**
	         * Baseline offset
	         */
			TInt iBaselineOffset;
				};
public:
	// Constructors and destructor
	
	/**
	 * Destructor.
	 */
	~CMmMarqueeAdapter ();
	
	/**
	 * Two-phased constructor.
	 */
	static CMmMarqueeAdapter* NewL ();
	
	/**
	 * Two-phased constructor.
	 */
	static CMmMarqueeAdapter* NewLC ();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CMmMarqueeAdapter ();
	
	/**
	 * Default symbian 2nd stage constructor.
	 */
	void ConstructL ();


public:
	
	/**
	 * Adds marquee element to be drawn
	 * 
	 * @since S60 v3.0
	 * @param aRect Rectangle where marquee is drawn
	 * @param aText Marquee text
	 * @param aFont Font
	 * @param aColor Color
	 * @param aAlign Align of text
	 * @param aIndex Subcell index
	 * @param aBaselineOffset Baseline offset
	 * @param aCurrentlyDrawnItemIndex Currently drawn item index
	 */
	void AddMarqueeElementL (const TRect& aRect, const TDesC& aText,
		TAknLogicalFontId aFont, TRgb aColor,
		CGraphicsContext::TTextAlign aAlign, TInt aIndex, TInt aBaselineOffset, TInt aCurrentlyDrawnItemIndex);
	
	/**
	 * Enables marquee
	 * 
	 * @since S60 v3.0
	 * @param aEnable Enable/disable marquee
	 */
	void EnableMarquee( TBool aEnable );
	
	/**
	 * Draws marquee text. is called every time marque is scrolled
	 * 
	 * @since S60 v3.0
	 * @param aGc Window graphics context.
	 */
	void DrawMarqueeL(CWindowGc & aGc);
	
	/**
	 * Stops marquee drawing 
	 * 
	 * @since S60 v3.0
	 */
	void StopMarqueeDrawing(TInt aIndex = -1);
		
	/**
	 * Set listbox where marquee is drawn
	 * 
	 * @since S60 v3.0
	 * @param aControl Listbox
	 */
	void SetControl( CEikListBox * aControl );
	
	/**
	 * Reveales if subcell marquee element already exists
	 * 
	 * @since S60 v3.0
	 * @param aIndex Item subcell index
	 * @return Does element exist in marquee adapter marquee elements array
	 */
	TBool SubcellMarqueeElementExists( TInt aIndex );
	
	/**
	 * Removes marquee elements from marquee adapter
	 * 
	 * @since S60 v3.0
	 * @param aSubcellIndex Item subcell index
	 */
	void RemoveSubcellMarqueeElement(TInt aSubcellIndex);
	
	/**
	 * Checks if marquee is enabled.
	 * 
	 * @return ETrue if marquee enabled, EFalse otherwise. 
	 */
    TBool IsMarqueeEnabled() const;

private:

	/**
	 * Called when marquee scroll increments
	 * 
	 * @since S60 v3.0
	 * @param aControl Listbox where marquee is drawn 
	 * @return Was marqueee drawing successful
	 */
	static TInt RedrawEvent(TAny* aControl);

private: //members

	/**
	 * Array of marquee element
	 */
	RPointerArray< CMmMarqueeElement > iMarqueeElements;

    /**
     * Is marquee enabled
     */
	TBool iMarqueeEnabled;
    
    /**
     * Listbox where marquee is drawn 
     * Not own
     */
	CEikListBox* iListbox;
    
    /**
     * Currently drawn item index
     */
	TInt iCurrentMarqueeItemIndex;
	};
	
#endif // MMMARQUEEADAPTER_H
