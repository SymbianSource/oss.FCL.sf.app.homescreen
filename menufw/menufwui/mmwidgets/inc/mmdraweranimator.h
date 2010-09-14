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
* Description:  CMmDrawerAnimator declaration
*  Version     : %version: MM_17.1.11 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMDRAWERANIMATOR_H
#define C_MMDRAWERANIMATOR_H

#include <e32base.h>
#include <e32std.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#include <aknlistboxtfx.h>
#endif
class CMmListBoxItemDrawer;
class TMmFloatingItem;

/**
 * Zoom ratio when no zooming is performed.
 */
const TReal KNormalStateZoomRatio = 1.0f;

/**
 * Zoom ratio when zooming is performed.
 */
const TReal KZoomStateZoomRatio = 1.25f;

/**
 *  Used by Multimedia Menu to draw Animated items.
 *  Drawer animator is an active object, that forces refreshes
 *  while floating items are available in the associated CListBoxItemDrawer.
 *  When no items are available the drawer ceases to redraw the screen.
 *  Active object becomes active when a new animation is triggered.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v5.0
 *  @see CListBoxItemDrawer
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmDrawerAnimator ) : public CActive
    {

public:
    /**
     * Two-phased constructor.
     *
     * @since S60 v5.0
     * @param aDrawer Item drawer used to draw item.
     */
    static CMmDrawerAnimator* NewL( CMmListBoxItemDrawer& aDrawer );

    /**
     * Two-phased constructor.
     *
     * @since S60 v5.0
     * @param aDrawer Item drawer used to draw item.
     */
    static CMmDrawerAnimator* NewLC( CMmListBoxItemDrawer& aDrawer );

    /**
     * Destructor.
     *
     * @since S60 v5.0
     */
    virtual ~CMmDrawerAnimator();

    /**
     * Animates transition of the dragged item.
     * The animation starts in the current position of EDrag
     * floating item, and end in the original place of dragging
     * or destination place, if reorder occured.
     *
     * @since S60 v5.0
     */
    void AnimateDragItemTransitionL( );

    /**
     * Animates an item with index aItemFrom into position
     * indexed by aItemTo.
     *
     * @since S60 v5.0
     * @param aItemFrom Which item should be animated.
     * @param aItemTo Where to should the item be animated.
     */
    void AnimateItemSwapL( TInt aItemFrom, TInt aItemTo );

    /**
     * Animates zooming of an item.
     *
     * @since S60 v3.0
     * @param aItemIndex Index of item to be zoomed.
     * @param aZoomIn Is zooming in or out.
     */
    void AnimateItemZoomL( TInt aItemIndex, TBool aZoomIn );

    /**
     * Checks if system is ready for new animation.
     *
     * @since S60 v5.0
     * @return Is system ready so new animation can proceed.
     */
    TBool IsReadyForNewAnimation();

    /**
     * Cancel currently running or pending animations.
     *
     * @since S60 v5.0
     */
    void CancelAnimationsL();

    /**
     * Triggers animation.
     * This methods sets the Activeobject into active state
     * if floating items are available
     *
     * @since S60 v5.0
     * @return Error code
     */
    TInt Trigger();

    /**
     * Make the next animation redraw the whole screen;
     */
    void SetNextRedrawToWholeScreen();

    /**
     * Indicates that draweranimator is in garbage.
     * This is used when suite model has been destroyed so there isn't
     * anything to draw, therefore any animator attempts should be
     * dropped.
     */
    void PrepareForGarbage();

    /**
     * Calls move event.
     */
    void TriggerMoveItemL();

    /**
     * Animates transition of the dragged item.
     * The animation starts in the original place of dragging item,
     * and end in the EDrag floating item position, when item had been start drag.
     *
     * @since S60 v5.0
     * @param aDraggedIndex Draged item index to be animated.
     * @param aPoint End position of animated item.
     */
    void AnimateDragItemStartL( TInt aDraggedIndex, TPoint aPoint );

private:

    /**
     * Default constructor.
     *
     * @since S60 v3.0
     * @param aDrawer Object used to draw items in widgets.
     */
    CMmDrawerAnimator( CMmListBoxItemDrawer& aDrawer );

    /**
     * 2nd phase constructor.
     *
     * @since S60 v3.0
     */
    void ConstructL();

private: // From CActive

    /**
     * Draws next animation phase.
     *
     * @since S60 v3.0
     */
    void RunL();

    /**
     * Cancels animation timer. Stops animation.
     *
     * @since S60 v3.0
     */
    void DoCancel();

    /**
     * Handles error if error an error occured.
     *
     * @since S60 v3.0
     * @param aError Error code which occurred.
     * @return Error code for error handling.
     */
    TInt RunError( TInt aError );

    /**
     * Adjusts rect to non touch.
     *
     * @since S60 v3.0
     * @param aRefreshRect Refresh rect.
     * @return Adjusted rect.
     */
    TRect AdjustRefreshRectToNonTouch( const TRect& aRefreshRect );

private: // Data

    /**
     * Timer
     */
    RTimer iTimer;

    /**
     * Associated item drawer.
     */
    CMmListBoxItemDrawer& iDrawer;

    /**
     * Number of animation frames
     */
    TInt iAnimateFrames;

    /**
     * Delay between redrawing.
     */
    TTimeIntervalMicroSeconds iDelay;

    /**
     * Effects api.
     */
    MAknListBoxTfx *iTransTfx;

    /**
     * Effects api.
     */
    MAknListBoxTfxInternal *iTransTfxInternal;

    /**
     * Highlight noted when last redraw occured.
     */
    TInt iLastNotedHighlight;

    /**
     * Top item index noted when last redraw occured.
     */
    TInt iLastNotedTopItem;

    /**
     * Vertical offset noted when last redraw occured.
     */
    TInt iLastNotedVerticalOffset;

    /**
     * Holds the previous animation refresh rectangle.
     */
    TRect iPreviousRefreshRect;

    /**
     * Holds the number of frames to animate.
     */
    const TInt iUsualAnimationFramesCount;

    /**
     * Holds the previous animation refresh rectangle.
     */
    TBool iPreparedForGarbage;
    };

#endif // MMDRAWERANIMATOR_H
