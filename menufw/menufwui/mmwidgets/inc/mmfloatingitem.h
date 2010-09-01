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
* Description:  floating item
*
*/


#ifndef T_MMFLOATINGITEM_H
#define T_MMFLOATINGITEM_H

#include <e32std.h>
#include <e32base.h>
#include "mmwidgetsconstants.h"


/**
 * Floating item.
 * 
 * This class describes a floating item element. Floating item is an extension
 * to the way grid and list widgets are drawn. Since widgets are very restrictive,
 * as to the place where na item is drawn (since in both grid and list each item
 * has a pretty well defined place) there had to be a way to draw items off the
 * standard place. Together with CMMDrawerAnimator, these classes support
 * drawing of dragged item as well as other off place items such as animation 
 * frames.
 * Floating items as such support the idea of animation, as each of the items
 * are aware of the animation step, and may modify its own position, whenever
 * requested. However a timer in this manner is needed, and this role is 
 * performed by CMMDrawerAnimator, which is aware of the redraw speed, and may
 * control the state of all instances of TMMFloatingItem.
 * It should be noted that both TMMFloatingItem and CMMDrawerAnimator doesn't 
 * handle item's drawing on it's own. These classes just control the logical 
 * state of floating items. All floating items are associated with the 
 * CMMListBoxItemDrawer in an array, and drawn whenever a Draw operation is 
 * performed on the widget, or any particular item, that is in any way overlapped
 * by a floating item.
 * 
 * @see CMMDrawerAnimator
 * @lib mmwidgets
 * @since S60 v5.0
 * @ingroup group_mmwidgets
 */

class CListBoxView;

class TMmFloatingItem
    {
    
public:
    /**
     * Constructor.
     * 
     * If a non-null value for the aView parameter is supplied, then item position
     * will be relative, so when the view is scrolled the floating item will move
     * just like all the non-floating items.
     * If aView is NULL, then item position will be absolute, meaning that it will
     * not change when the view is scrolled. 
     *
     * @since S60 ?S60_version
     * @param aDrawnIndex Index of the item which is going to 
     *                        be draw off place.
     * @param aStartPosition Position of the item (always expressed in screen
     *                       coordinates, even when creating an item with relative
     *                       position).
     * @param aType Type of the floating item.
     * @param aAnimationFrames Total number of animation frames to perform.
     * @param aView The view on which items are drawn (optional - affects
     *        scrolling behavior).
     */
    TMmFloatingItem( TInt aDrawnIndex, TPoint aStartPosition,
            TMmFloatingItemType aType, TInt aAnimationFrames,
            CListBoxView* aView = NULL );
    
    /**
     * Sets the flag of manual delete. If such flag is set
     * the floating item will not perfrom self removal. Normally
     * floating items remove themselves fram the drawing buffer whenever 
     * the animation is finished. This is used for example for
     * dragged items, since they are not animated, and they will be
     * deleted manual when the uses lets go of the drag.
     *
     * @since S60 5.0
     * @param aManual Flag of manual deletion.
     */
    void SetManualDelete( TBool aManual );
    
    /**
     * Performs a step of the animation, according to 
     * earlier provided information.
     *
     * @since S60 5.0
     */
    TBool MakeStep();
       
    /**
     * Set the difference in the location of the floating item
     * from the originating position of the item, to the resulting
     * position of the animation
     *
     * @since S60 5.0
     * @param aDiffetenceVector Vector of displacement.
     */
    void SetPositionStep( TPoint aDiffetenceVector  );
    
    /**
     * Set the size of the step that modifies the size of the
     * item with every step of the animation.
     *
     * @since S60 5.0
     * @param aStartSize Initial size of the item.
     * @param aFinalSize Final size of the item.
     */
    void SetSizeStep( TReal aStartSize, TReal aFinalSize );   
    
    /**
     * Gets the current zoom ratio. The ratio might by
     * time specific, since floating items support animations
     * changing size of the item.
     *
     * @since S60 5.0
     * @return Zooming ratio.
     */
    TReal GetCurrentZoomRatio() const;
    
    /**
     * Gets the type of the floating item.
     *
     * @since S60 5.0
     * @return Type of the floating item.
     */    
    TMmFloatingItemType GetFloatingItemType() const;

    /**
     * Gets the item index of the item being drawn.
     *
     * @since S60 5.0
     * @return Item of the item drawn.
     */    
    TInt GetDrawnItemIndex() const;
    
    /**
     * Gets the current item position.
     * 
     * The position returned is always expressed in screen coordinates,
     * even if this item's position is relative.
     *
     * @since S60 5.0
     * @return Position of the item.
     */    
    TPoint GetItemPosition() const;

    /**
     * Marks the item as invalid. Item will be deleted 
     * from the cache when an attempt to draw the item
     * occurs.
     *
     * @since S60 5.0
     */    
    void InvalidateFloatingItem();

    /**
     * Tests if the item is still valid.
     *
     * @since S60 5.0
     */    
    TBool IsFloatingItemValid() const;
    
    /**
     * Zooming status is returned, meaning if the item is still 
     * in the state of zoming in, zooming out, or in normal size
     * 
     * @since S60 5.0
     * @return Negative value is returned if the item is zooming out.
     *         Positive value is returned if item is zooming in.
     *         Zero is returned if the item is static in size.
     */
    TInt GetZoomingStatus();
    
    /**
     * Checks if the item has the manual deletion
     * flag setup.
     *
     * @since S60 5.0
     * @return The status of manual deletion flag.
     */    
    TBool IsManualDelete();
    
    /**
     * Calculates consequent steps
     *
     * @since S60 5.0
     * @param aVector Displacement vector.
     */    
    void CalculateSteps(TPoint aVector);
    
private:
    
    /**
     * Index of the item, which is being animated. 
     */  
    TInt iDrawnItemIndex;
    
    /**
     * Current item position.
     * 
     * This can be either an absolute position (expressed in screen coordinates) or
     * a relative position depending on whether iView member variable is NULL.
     * In the latter case, the position is relative to the top-left corner of the
     * first item in the view.
     */  
    TPoint iItemPosition;
    
    /**
     * Dispalcement of the item, per animation step.
     */  
    TFixedArray<TPoint, MmEffects::KMaximumAnimationFramesCount> iPositionStep;
    
    /**
     * Size change, per animation step.
     */  
    TReal iSizeStep;
    
    /**
     * Counter of animation frames.
     */  
    TInt iFrameCounter;
    
    /**
     * Total number of animation frames to perform.
     */  
    TInt iFrames;
    
    /**
     * Manual deletion flag.
     */  
    TBool iManualDelete;
    
    /**
     * Current scaling ratio.
     */  
    TReal iZoomRatio;
    
    /**
     * Type of the floating item. There is a need for distinction
     * between different animations types.
     */  
    TMmFloatingItemType iType; 
    
    /**
     * The view on which items are drawn.
     * 
     * When iView is NULL, then iItemPosition contains absolute item position.
     * Otherwise iItemPostion contains item position relative to the top-left
     * corner of the first item in the view. 
     */
    CListBoxView* iView;
       
    };
    
#endif // T_MMFLOATINGITEM_H
