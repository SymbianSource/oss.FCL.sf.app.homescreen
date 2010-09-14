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
* Description:
*  Version     : %version: MM_12.1.10 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef MMLISTBOXVIEW_H
#define MMLISTBOXVIEW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <eikfrlb.h>

// CLASS DECLARATION

/**
 *  Multimedia Menu Listbox view.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmListBoxView ) : public CFormattedCellListBoxView
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CMmListBoxView();

    /**
     * Two-phased constructor.
     */
    static CMmListBoxView* NewL();

    /**
     * Two-phased constructor.
     */
    static CMmListBoxView* NewLC();

  /**
   * Calculate the bottom item index.
   *
   * @since S60 v3.0
   */
    void CalcBottomItemIndex();

  /**
   * Draw the view.
   *
   * @since S60 v3.0
   * @param clipRect Clipping rectangle.
   */
    void Draw(const TRect* clipRect) const;

  /**
   * Draws the listbox within the given clipping rect.
   *
   * @since S60 v3.0
   * @param clipRect Clipping rectangle.
   */
    void DoDraw(const TRect* aClipRect) const;

  /**
   * Draw item in view. It is possible that it
   * calls drawing of more than one item.
   *
   * @since S60 v3.0
   * @param aItemIndex Item index.
   */
    void DrawItem(TInt aItemIndex) const;

  /**
   * Draw a single item in view.
   *
   * @since S60 v3.0
   * @param aItemIndex Item index.
   */
    void DrawSingleItem (TInt aItemIndex) const;

  /**
   * Sets aItemIndex to the index of the item whose bounding box contains aPosition.
   *
   * @since S60 v3.0
   * @param aPosition Item position.
   * @param aItemIndex Item index.
   * @return Was item found.
   */
    TBool XYPosToItemIndex(TPoint aPosition, TInt& aItemIndex) const;

  /**
   * Gets item position.
   *
   * @since S60 v3.0
   * @param aItemIndex Item index.
   * @return Item position.
   */
    TPoint ItemPos(TInt aItemIndex) const;

  /**
   * Sets item height.
   *
   * @since S60 v3.0
   * @param aItemHeight Item height.
   */
    void SetItemHeight(TInt aItemHeight);

  /**
   * Calculates new top item index so item is visible.
   *
   * @since S60 v3.0
   * @param aItemIndex Item to be visible.
   * @return New top item index.
   */
    TInt CalcNewTopItemIndexSoItemIsVisible(TInt aItemIndex) const;

  /**
   * Draw background between rects.
   *
   * @since S60 v3.0
   * @param aUsedPortionOfViewRect Inner rect.
   * @param aSmallerViewRect Outer rect.
   */
    void RedrawBackground( TRect aUsedPortionOfViewRect, TRect aSmallerViewRect) const;

  /**
   * Draws background in list which is not covered by vivible items.
   *
   * @since S60 v3.0
   */
    void RedrawBackground() const;

  /**
   * Gets total height of items within range.
   *
   * @since S60 v3.0
   * @param aStartIndex Start item index.
   * @param aEndIndex End item index.
   * @return Total height of items starting from aStartIndex, ending with aEndIndex.
   */
    TInt GetTotalHeight( TInt aStartIndex, TInt aEndIndex ) const;

    /**
     * Gets the current widget vertical item offset.
     *
     * @since S60 v5.0
     * @return The current widget vertical item offset.
     */
    TInt VerticalItemOffset() const;

    /**
     * Overridden to allow for disabling this method in certain situations.
     *
     * @param aItemIndex Item index.
     */
    TBool ScrollToMakeItemVisible(TInt aItemIndex);

    /**
     * Disables ScrollToMakeItemVisible.
     * When disabled, ScrollToMakeItemVisible does nothing.
     *
     * @param aDisable Self-explanatory.
     */
    void DisableScrollToItem( TBool aDisable );

    /**
     * Gets number of items in model.
     *
     * @return
     */
    TInt ModelItemsCount();
    
    /**
     * Sets a proper flag if the list is brought to foreground.
     *
     *  @param aCommingFromBackground indicates whether the list is brought to foreground.
     */
    void SetListCommingFromBackground(TBool aCommingFromBackground);

private:

    /**
     * Constructor for performing 1st stage construction.
     */
    CMmListBoxView();

    /**
     * Default symbian 2nd stage constructor.
     */
    void ConstructL();

  /**
   * Updates average item height.
   *
   * @since S60 v3.0
   */
     void UpdateAverageItemHeight();

  /**
   * Gets last item index depending on height.
   *
   * @since S60 v3.0
   * @param aStartIndex Start item index.
   * @param aHeight Height of area.
   * @return Last item index.
   */
    TInt GetLastIndexInHeight( const TInt aStartIndex, TInt aHeight ) const;

  /**
   * Sets the previously drawn current item index.
   *
   * @since S60 v3.0
   * @param aIndex Current Item index.
   */
   void SetPreviouslyDrawnCurrentItemIndex( TBool aIndex );
    
   /**
    * Gets the flag indicating if the list is being brought to foreground.
    * @return Last item index.
    */
   TBool GetListCommingFromBackground() const;
    

private:

  /**
   * iCurrentItemIndex
   */
  TInt iPreviouslyDrawnCurrentItemIndex;

  /**
   * When ETrue, disables ScrollToMakeItemVisible method.
   */
  TBool iScrollToItemDisabled;
  
  /**
   * When ETrue the list is being brought from background
   */
  TBool iListCommingFromBackground;
    };

#endif // MMLISTBOXVIEW_H
