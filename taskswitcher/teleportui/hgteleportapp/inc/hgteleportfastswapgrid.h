/*
 * ============================================================================
 *  Name        : hgteleportfastswapgrid.h
 *  Part of     : Hg Teleport
 *  Description : Teleport Fast Swap area UI
 *  Version     : %version:  12 %
 *
 *  Copyright © 2009 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 *
 */

#ifndef HGTELEPORTFASTSWAPGRID_H_
#define HGTELEPORTFASTSWAPGRID_H_

#include <AknGrid.h>
#include <aknconsts.h>
#include "hgteleportdevicestate.h"

class CAknsFrameBackgroundControlContext;
class CHgTeleportFastSwapTimer;

/**
 * Observer for handling fast swap grid events
 */
class MHgTeleportFastSwapGridObserver
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


class MHgTeleportFastSwapTimerObserver
    {
public:
    /**
     * Called when timer is completed
     */
    virtual void TimerCompletedL( CHgTeleportFastSwapTimer* aSource ) = 0;
    };


/**
 * Avkon grid implementing custom item drawer
 */
class CHgTeleportFastSwapGrid: 
    public CAknGrid,
    public MHgDeviceStateObserver
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
    CHgTeleportFastSwapGrid();
    ~CHgTeleportFastSwapGrid();
    void ConstructL( const CCoeControl* aParent );
    
public: // From CCoeControl
    void HandlePointerEventL( const TPointerEvent &aPointerEvent );
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    void HandleResourceChange( TInt aType );
    void SizeChanged();
    
public: //From MHgDeviceStateObserver
    /**
     * Called when a change, to which the observer is registered,
     * has happened.
     */
    virtual void HandleDeviceStateChanged( TChangeType aChangeType );
    
public: // New functions

    /**
     * Sets fast swap grid observer
     */
    void SetFastSwapGridObserver( MHgTeleportFastSwapGridObserver* aObserver  );

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

private: // From CAknGrid
    virtual void CreateItemDrawerL();

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
    // Background context for grid
    CAknsFrameBackgroundControlContext* iBgContext;
    
    // Close icon handling
    RArray<TInt> iCloseItems;
    TInt iCloseIconHitIdx;
    MHgTeleportFastSwapGridObserver* iFastSwapGridObserver;

    // Highlight handling
    TFastSwapGridBehaviour iBehaviour;
    TBool iHighlightVisible;
    
    // Visible view rectangle (horizontal scrolling support)
    TRect iVisibleViewRect;
    };



/**
 * Custom item drawer for drawing grid items
 */
class CHgTeleportGridItemDrawer: public CFormattedCellListBoxItemDrawer
    {

public: // Constructor and destructor
    CHgTeleportGridItemDrawer( CHgTeleportFastSwapGrid* aGrid,
                               CFormattedCellListBoxData* aData );
    ~CHgTeleportGridItemDrawer();

public: // New functions

    /**
     * Sets close icon drawn for items that can be closed.
     * Ownership transferred.
     */
    void SetCloseIcon( CFbsBitmap* aBmp, CFbsBitmap* aMask );

    /**
     * Returns close icon coordinates for the given item rectangle
     */
    TRect GetCloseIconRect( const TRect& aItemRect ) const;
    
    /**
     * Sets screen offset for the item drawer to not draw items on the
     * grid edges
     */
    void SetEdgeOffset( TInt aLeftOffset, TInt aRightOffset );

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
    
private: // Data
    // Not owned
    CHgTeleportFastSwapGrid* iGrid;
    // Owned
    CFbsBitmap* iCloseIcon;
    CFbsBitmap* iCloseIconMask;
    
    TRect iScreenRect;
    TInt iLeftOffset;
    TInt iRightOffset;
    };



/**
 * Timer class for handling highlight bevaiour
 */
class CHgTeleportFastSwapTimer : public CTimer
    {
public:
    // Constructor
    CHgTeleportFastSwapTimer( MHgTeleportFastSwapTimerObserver& aObserver );
    // Destructor
    ~CHgTeleportFastSwapTimer();
    // 2nd phase constructor
    void ConstructL();
    
private: // From CTimer
    void RunL();
    
private: // Data
    MHgTeleportFastSwapTimerObserver* iObserver; // not own
    };

#endif /* HGTELEPORTFASTSWAPGRID_H_ */
