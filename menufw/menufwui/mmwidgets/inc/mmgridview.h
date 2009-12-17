/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMmGridView declaration
*  Version     : %version: MM_24.1.7 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMGRIDVIEW_H
#define C_MMGRIDVIEW_H

#include <e32std.h>
#include <e32base.h>
#include <AknGridView.h> 

/**
 *  Multimedia Menu Grid View.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmGridView ) : public CAknGridView
    {
public:
    /**
     * Two-phased constructor.
     * 
     * @since S60 v3.0
     */
    static CMmGridView* NewL();
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 v3.0
     */
    static CMmGridView* NewLC();

    /**
     * Destructor.
     * 
     * @since S60 v3.0
     */
    virtual ~CMmGridView();
    
    /**
     * Draws the grid within the given clipping rectangle.
     *
     * @since S60 v3.0
     * @param aClipRect Clipping rectangle.
     */
	void Draw( const TRect* aClipRect ) const;
	
	/**
     * Draws item definied by index.
     *
     * @since S60 v3.0
     * @param aItemIndex Item index which should be draw.
     */
	void DrawItem(TInt aItemIndex) const;
	
	/**
     * Gets item position.
     * 
     * This fuction from @c CAknGridView is overridden so that it
     * returns 'mirrored' item positions when mirrored layout is
     * used. 
     * 
     * @since S60 v3.0
     * @param aItemIndex Item index.
     * @return Item position.
     */
    TPoint ItemPos( TInt aItemIndex ) const;
    
    /**
     * Converts an (x, y) pixel position to an item index.
     * 
     * This fuction from @c CAknGridView has been overridden in order
     * to make it work properly when mirrored layout is used.
     * 
     * @param aPosition Pixel position in the viewing rectangle.
     * @param aItemIndex Reference to the item index.
     * @return Whether there was an item at aPosition.
     */  
    TBool XYPosToItemIndex(TPoint aPosition, TInt& aItemIndex) const;
	
	/**
	 * Returns this view's gc.
	 * 
	 * @return pointer to gc
	 */
	CWindowGc* Gc();
	
    /**
     * @see CAknGridView::UpdateSelectionL
     * 
     * This method of CAknGridView is overriden to so that
     * correct effects are displayed when moving highlight with 
     * rocker key in mirrored layout.
     * Please note that it is necessary because of the hackish
     * solution used to draw items in mirrored layout.
     */
    void UpdateSelectionL( TSelectionMode aSelectionMode );

	/**
	 * @see CAknGridView::MoveCursorL
	 * 
	 * This method of CAknGridView is overriden to so that
	 * correct effects are displayed when moving highlight with 
	 * rocker key in mirrored layout.
     * Please note that it is necessary because of the hackish
     * solution used to draw items in mirrored layout.
	 */
    void MoveCursorL( TCursorMovement aCursorMovement,
            TSelectionMode aSelectionMode );

    /**
     * Gets the current widget vertical item offset.
     * 
     * @since S60 v5.0
     * @return The current widget vertical item offset.
     */
    TInt VerticalItemOffset() const;
    
	/**
	* This function sets item height in pixels.
	* @param aItemHeight New height in pixels for this view’s items.
	*/	
    void SetItemHeight(TInt aItemHeight);

private:
	
    /**
     * Default constructor.
     * 
     * @since S60 v3.0 
     */
    CMmGridView();

    /**
     * 2nd phase constructor.
     * 
     * @since S60 v3.0 
     */
    void ConstructL();
    
    /**
     * Draws the grid within the given clipping rectangle.
     *
     * @since S60 v3.0
     * @param aClipRect Clipping rectangle.
     */
    void DoDraw( const TRect* aClipRect ) const;
    
    /**
     * Gets item position.
     * 
     * AVKON implementation of ItemPos method does not always work properly.
     * It fails when:
     *   aItemIndex < TopItemIndex() - NumberOfColsInView()
     * Because of that problem this method was created. It is now used in
     * CMmGridView::ItemPos in place where base class (AVKON) implementation
     * was used previously.
     * 
     * @param aItemIndex Item index.
     * @return Item position.
     */
    TPoint CorrectItemPos( TInt aItemIndex ) const;

private:

    /**
     * Stores cursor movement flag during CMmGridView::MoveCursorL execution.
     * 
     * When CMmGridView::MoveCursorL is not executing this member variable is
     * always set to ECursorFirstItem. This is part of the hack that allows
     * for correct effects to be displayed when using rocker key in mirrored
     * layout.
     */
    TCursorMovement iLastCurMove;
    
    /**
     * Stores previosly highlighted item index during CMmGridView::MoveCursorL
     * execution.
     * 
     * When CMmGridView::MoveCursorL is not executing this member variable is
     * always set to KErrNotFound. This is part of the hack that allows
     * for correct effects to be displayed when using rocker key in mirrored
     * layout.
     */
    TInt iOldIndex;	
    };

#endif // C_MMGRIDVIEW_H
