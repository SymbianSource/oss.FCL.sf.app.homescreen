/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Taskswitcher Fast Swap area UI
 *
*/


#ifndef TSFASTSWAPGRID_H_
#define TSFASTSWAPGRID_H_

#include <AknGrid.h>
#include <aknconsts.h>
#include "tsdevicestate.h"

class CAknsFrameBackgroundControlContext;
class CTsFastSwapTimer;

/**
 * Observer for handling fast swap grid events
 */
class MTsFastSwapGridObserver
    {
public:
    /**
     * Called when close icon is tapped on the grid item.
     * 
     * @param  aItemIdx  index of the the item for which close icon
     *                   has been tapped
     */
    virtual void HandleCloseEventL( TInt aItemIdx ) = 0;
    };


class MTsFastSwapTimerObserver
    {
public:
    /**
     * Called when timer is completed
     */
    virtual void TimerCompletedL( CTsFastSwapTimer* aSource ) = 0;
    };


/**
 * Avkon grid implementing custom item drawer
 */
class CTsFastSwapGrid: 
    public CAknGrid,
    public MTsDeviceStateObserver,
    public MTsFastSwapTimerObserver
    {
public: // enums
    enum TFastSwapGridBehaviour
        {
        // Highlight always visible
        EHybrid,
        
        // Touch only highlight handling
        ETouchOnly
        };
    
public: // Constructor and destructor
    CTsFastSwapGrid();
    ~CTsFastSwapGrid();
    void ConstructL( const CCoeControl* aParent );
    
public: // From CCoeControl
    void HandlePointerEventL( const TPointerEvent &aPointerEvent );
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    void HandleResourceChange( TInt aType );
    void SizeChanged();
    
public: //From MTsDeviceStateObserver
    /**
     * Called when a change, to which the observer is registered,
     * has happened.
     */
    virtual void HandleDeviceStateChanged( TChangeType aChangeType );
    
public: // New functions

    /**
     * Sets fast swap grid observer
     */
    void SetFastSwapGridObserver( MTsFastSwapGridObserver* aObserver  );

    /**
     * Set indexes of items which can be closed.
     * Copies item index array.
     */
    void SetCloseItemsL( RArray<TInt>& aItemIndex );

    /**
     * Checks if the item of given index is closable
     */
    TBool CanCloseItem( TInt aItemIndex );

    /**
     * Checks if the given item's close icon has been tapped.
     */
    TBool IsItemCloseHit( TInt aItemIndex );

    /**
     * Resets index of the item that has been close icon hit
     */
    void ResetCloseHit();
    
    /**
     * Sets grid behaviour
     * 
     * @param  aBehaviour  behaviour to be set
     */
    void SetGridBehaviour( TFastSwapGridBehaviour aBehaviour );
    
    /**
     * Getter for grid behaviour
     * 
     * @return  girds behaviour
     */
    TFastSwapGridBehaviour GridBehaviour();
    
    /**
     * Shows highlight. In touch only behaviour highlight
     * will be shown only for ammount of time.
     */
    void ShowHighlight();
    
    /**
     * Hides highlight.
     */
    void HideHighlight();
    
    /**
     * Checks if the highlight is currently visible.
     * 
     * @return  ETrue  if highlight is visible
     *          EFalse if highlight is not visible
     */
    TBool IsHighlightVisible();
    
    /**
     * Sets visible view, items outside of view will not be drawn
     */
    void SetVisibleViewRect( const TRect aRect );
    
    /**
     * Returns visible view. Items outside of visible rectangle are not drawn.
     */
    TRect VisibleViewRect();
    
    /**
     * Launch tactile ETouchFeedbackSensitive feedback.
     */
    void LaunchTactileFeedback();
    
    /**
     * Set tactile feedback support.
     * 
     * @param aSupport new support value 
     */
    void SetTactileFeedbackSupport(TBool aSupport);
    
    /**
     * Remove item from iFullyVisibleItems and iPartialVisibleItems arrays 
     * 
     * @param   aItem  idem index 
     * @return  ETrue  if item was removed
     *          EFalse if item was not found
     */
    TBool RemoveFromVisibleItems(TInt aItem) const;

    /**
     * Add item to iFullyVisibleItems array 
     * 
     * @param   aItem  idem index 
     * @return  ETrue  if item was added
     *          EFalse if item was not added because it was there before
     */
    TBool AddToFullyVisibleItems(TInt aItem) const;

    /**
     * Remove item from iPartialVisibleItems array 
     * 
     * @param   aItem  idem index 
     * @return  ETrue  if item was removed
     *          EFalse if item was not removed because it was not found
     */
    TBool MoveToPartialVisibleItems(TInt aItem) const;

private: // From CAknGrid
    virtual void CreateItemDrawerL();

private: // From MTsFastSwapTimerObserver
    void TimerCompletedL( CTsFastSwapTimer* aSource );
    
private: // New functions
    
    /**
     * Loads close icon bitmap and mask
     */
    void LoadCloseIcon();
    
    /**
     * Redraws grid and parent controls
     */
    void Redraw();
    
private: // Data
    
    // Grid's parent
    const CCoeControl* iParent;
    
    // Background context for grid
    CAknsFrameBackgroundControlContext* iBgContext;
    
    // Close icon handling
    RArray<TInt> iCloseItems;
    TInt iCloseIconHitIdx;
    MTsFastSwapGridObserver* iFastSwapGridObserver;
    CTsFastSwapTimer* iCloseIconRedrawTimer;

    // Highlight handling
    TFastSwapGridBehaviour iBehaviour;
    TBool iHighlightVisible;
    
    // Visible view rectangle (horizontal scrolling support)
    TRect iVisibleViewRect;
    
    // Feedback support
    TBool iTactileFeedbackSupport;
    CTsFastSwapTimer* iFeedbackTimer;
    
    mutable RArray<TInt> iFullyVisibleItems;
    mutable RArray<TInt> iPartialVisibleItems;
    };



/**
 * Custom item drawer for drawing grid items
 */
class CTsGridItemDrawer: public CFormattedCellListBoxItemDrawer
    {

public: // Constructor and destructor
    CTsGridItemDrawer( CTsFastSwapGrid* aGrid,
                               CFormattedCellListBoxData* aData );
    ~CTsGridItemDrawer();

public: // New functions

    /**
     * Sets close icon drawn for items that can be closed.
     * Ownership transferred.
     */
    void SetCloseIcon( CFbsBitmap* aBmp, CFbsBitmap* aMask );

    /**
     * Initializes close icon rectangles.
     * 
     * @param  aButtonRect  rectangle for the button, coordinates must be relative to the grid item
     * @param  aIconRect    rectangle for the icon, coordinates must be relative to the grid item
     */
    void SetCloseIconRect( const TRect& aButtonRect, const TRect& aIconRect );
    
    /**
     * Returns current close icon coordinates for the given item rectangle
     * 
     * @param  aItemRect  rectangle of the item for which close button coordinates
     *                    are to be returned
     */
    TRect GetCloseButtonRect( const TRect& aItemRect ) const;
    
    /**
     * Sets screen offset for the item drawer to not draw items on the
     * grid edges
     */
    void SetEdgeOffset( TInt aLeftOffset, TInt aRightOffset );
    
    /**
     * Enables/Disables item drawer background redrawing
     * 
     * @param  aEnable  if set to ETrue, background will be redrawn by item drawer
     *                  if set to EFalse, background will not be redrawn
     */
    void SetRedrawBackground( TBool aEnable );

private: // From CFormattedCellListBoxItemDrawer
    void DrawActualItem( TInt aItemIndex, const TRect& aActualItemRect,
                         TBool aItemIsCurrent, TBool aViewIsEmphasized,
                         TBool aViewIsDimmed, TBool aItemIsSelected ) const;
private: // New functions
    /**
     * Checks if the item rectangle is visible in the view.
     * 
     * @param   aItemRect  rectangle of the item to be checked 
     * @return  ETrue      if rectangle is fully or partially visible
     *          EFalse     if rectangle is not visible
     */
    TBool IsItemRectVisible( const TRect& aItemRect ) const;
    
    /**
     * Checks if the aRectContained rectancle is contained by aRectContainig 
     * 
     * @param   aRectContained  rectangle that is suppose to be contained 
     * @param   aRectContainig  rectangle that is suppose to be containing 
     * @return  ETrue      if rectangle is fully contained 
     *          EFalse     if rectangle is not fully contained
     */
    TBool IsRectContained(const TRect& aRectContained,
            const TRect& aRectContainig) const;

    /**
     * Calculates current inner button rectangle for close icon
     */
    TRect CalculateInnerButtonRect( const TRect& aOuterRect ) const; 
    
private: // Data
    // Not owned
    CTsFastSwapGrid* iGrid;
    // Owned
    CFbsBitmap* iCloseIcon;
    CFbsBitmap* iCloseIconMask;
    
    TRect iScreenRect;
    TInt iLeftOffset;
    TInt iRightOffset;
    
    // Layout data
    TRect iCloseIconRect;
    TRect iCloseButtonRect;
	
    TBool iRedrawBackground;
    };



/**
 * Timer class for handling highlight bevaiour
 */
class CTsFastSwapTimer : public CTimer
    {
public:
    // Constructor
    CTsFastSwapTimer( MTsFastSwapTimerObserver& aObserver );
    // Destructor
    ~CTsFastSwapTimer();
    // 2nd phase constructor
    void ConstructL();
    
private: // From CTimer
    void RunL();
    
private: // Data
    MTsFastSwapTimerObserver* iObserver; // not own
    };

#endif /* TSFASTSWAPGRID_H_ */
