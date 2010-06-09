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
        static CXnNewstickerControl* NewL( CXnNewstickerAdapter* aAdapter );

        // Routed from MXnNewstickerInterface.

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
         * Update title in the title list.
         * @param aTitle The new title to add.
         * @param aIndex The place to add the title.
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
        TInt CurrentTitleIndex() const;

    	/**
    	 * Return the title of the given index.
    	 * @param aIndex The index to return the title.
    	 * @return The title of the given index.
    	 */
        const TDesC& Title(TInt aIndex) const;
		
		/**
		 * Returns last index from iTitleTexts
		 * Which is not empty string
		 */
        TInt LastIndexWithContent() const;
        
        /**
         * Delete all titles.
         */
        void ClearTitles();

        /**
         * Return the number of titles in the list.
         * @return The number of titles in the list.
         */
        TInt TitleCount() const;
        
        /** 
        * Destructor
        */
        virtual ~CXnNewstickerControl();

	    /**
        * Selects title to be shown
        */
        const TDesC& CurrentTitle() const;

        /**
        * Selects current title to display
        */
        TInt SelectTitle();
        
        /**
        * Selects next title to display
        */
        TBool SelectNextTitle();

        /**
        * Checks is there any visible titles in array
        */
        TBool IsVisibleTitles() const;
        
        /**
         * Sets scroll looping
         */
        void SetScrollLooping( TBool aLooping );

        /**
         * Peeks next title, return loop status in aEndOfLoop.         
         */
        TPtrC PeekNextTitle( TBool& aEndOfLoop ) const;
        
    private: 

        CXnNewstickerControl(CXnNewstickerAdapter* aAdapter);

        void ConstructL();
        
        /**
         * Get the index of the next title that has content
         * 
         * @param aStartSearch The index where to start search
         * @param aBackwards Search backwards
         * 
         * @return The next index that has content or -1 if nothing was found 
         */
        TInt GetNextTitleWithContent( TInt aStartSearch, TBool aBackwards = EFalse ) const;
        
    private:    // Data
        
        /**
        * The array for title texts
        * owned
        */
        RPointerArray<HBufC>        iTitleTexts;
        
        /**
        * The index of the title which is selected if user activates the
        * control.
        */
        TInt                        iCurrentTitleIndex;
        
        /**
         * Parent control adapter
         * Not own.
         */	            
        CXnNewstickerAdapter*       iAdapter;
        
        /**
        * is scroll looping activated
        */ 
        TBool                    iScrollLooping;
    };

#endif // XNNEWSTICKERCONTROL_H

// End of File
