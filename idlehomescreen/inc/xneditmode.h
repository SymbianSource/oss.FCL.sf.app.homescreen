/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Edit mode for widget drag and drop.
*
*/

#ifndef XNEDITMODE_H
#define XNEDITMODE_H

// System includes
#include <coecntrl.h>

// Forward declarations
class CXnUiEngine;
class CXnViewManager;
class CXnPluginData;
class CXnNode;
class CFbsBitGc;
class CFbsBitmap;
class CFbsBitmapDevice;
class CWindowToBitmapMappingGc;

// Class declaration

/**
 * Edit mode. Enables widget drag and drop.
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 5.1
 */
NONSHARABLE_CLASS( CXnEditMode ) : public CCoeControl  
    {
public:
    // Data types
    enum TEditState
        {
        ENone = 0x00,
        EWaitDrag = 0x01,        
        EShootContent = 0x02,
        EDragging = 0x04,
        EDragAndDrop = 0x08,
        EKeyMove = 0x10
        };

public:
    /**
     * Two-phased constructor.
     */
    static CXnEditMode* NewL( CXnUiEngine& aUiEngine );

    /**
     * Destructor.
     */
    ~CXnEditMode();

private:
    // from CCoeControl

    /**
     * @see CCoeControl
     */
    void Draw( const TRect& aRect ) const;

    /**
     * @see CCoeControl
     */    
    void MakeVisible( TBool aVisible );

    /**
     * @see CCoeControl
     */        
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * @see CCoeControl
     */        
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
            TEventCode aType );

    /**
     * @see CCoeControl
     */            
    void SizeChanged();
            
public:
    // New functions

    /**
     * Changes the state of edit mode.
     *
     * @since Series 60 5.0
     * @param aSet The state of edit mode.     
     */
    void SetEditModeL( CXnEditMode::TEditState aSet );

    /**
     * Gets the status of the edit mode.
     *
     * @since Series 60 5.0
     * @return Edit mode state.
     */
    CXnEditMode::TEditState EditState() const;
    
    /**
     * This is for to re-initializing the double buffer when the orientations
     * is changed
     *
     * @since Series 60 5.0
     */
    void HandleScreenDeviceChangedL();

    /**
     * Sets control rect.
     *
     * @since Series 60 5.0
     */
    void SetClientRect( TRect aRect );

    /**
     * Reset the dragging state and update screen
     *
     * @since Series 60 5.0
     */
    void StopDraggingL();

private:
    // new functions

    /**
     * Organizing starts using drag and drop.
     *
     * @since Series 60 5.0
     * @param aPlugin Plugin to drag.
     */
    void StartDragL( const CXnPluginData& aPlugin );
    
    /**
     * Reset the dragging state and deletes screenshot bitmaps.
     *
     * @since Series 60 5.0
     */
    void StopDragL();
    
    /**
     * Updates invalid parts of the screen. Calls internally DrawNow
     *
     * @since Series 60 5.0     
     */
    void UpdateScreen();
    
private:    
    // Constructors
    
    /**
     * C++ default constructor.
     */
    CXnEditMode( CXnUiEngine& aEngine );

    /**
     * 2nd phase constructor.
     */    
    void ConstructL();
    
private:
    // Data

    /**
     * UI engine
     * Not own.
     */
    CXnUiEngine& iUiEngine;

    /**
     * View manager
     * Not own.
     */
    CXnViewManager& iViewManager;
    
    /**
     * Screenshot of part of mainpane
     * Owned
     */    
    CFbsBitmap* iMainpane;

    /**
     * Screenshot of widget
     * Owned
     */
    CFbsBitmap* iWidget;
    
    /**
     * Mapping Gc
     * Owned
     */
    CWindowToBitmapMappingGc* iMapGc;
    
    /**
     * Bmp device
     * Owned
     */
    CFbsBitmapDevice* iBmpDevice;
    
    /**
     * Bmp Gc
     * Owned
     */
    CFbsBitGc* iBmpGc;          
    
    /**
     * A pointer to the node that is being dragged.
     * Not own.
     */
    CXnNode* iDraggingNode;

    /**
     * A pointer to the node that is underneath of the dragging node.
     * Not own.
     */
    CXnNode* iTargetNode;
    
    /**
     * State of edit mode
     */
    CXnEditMode::TEditState iState;
        
    /**
     * A point where dragging started from.
     */
    TPoint iStylusDownPos;

    /**
     * A point where drawing was done last time.
     */
    TPoint iPreviousPos;
               
    /**
     * Screenshot draw position
     */
    TPoint iDrawPos;
    
    /**
     * Last redraw rect
     */
    TRect iLastDrawRect;
    
    /**
     * Flag determine whether drag is occured
     */
    TBool iDragged;
    };

#endif
