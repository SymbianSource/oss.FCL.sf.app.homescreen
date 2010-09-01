/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Scrollable control adapter
*
*/

#ifndef __XNSCROLLABLECONTROLADAPTER_H__
#define __XNSCROLLABLECONTROLADAPTER_H__

//  System includes
#include <e32base.h>
#include <coeccntx.h>
#include <aknphysicsobserveriface.h>

// User includes
#include "xncontroladapter.h"

// Forward declarations
class CXnNodePluginIf;
class CXnUiEngine;
class CXnNode;
class CAknPhysics;

// Class declaration

/**
*  @ingroup group_xnlayoutengine
*  Scrollable Control adapter interface class
*  @lib xnlayoutengine.lib
*  @since Series 60 5.0
*/
NONSHARABLE_CLASS( CXnScrollableControlAdapter ) : public CXnControlAdapter, 
    public MAknPhysicsObserver, public MCoeControlContext, public MCoeControlHitTest
	{
	public: // Constructors and destructor
		/**
        * Two-phased constructor.
        */
		static CXnScrollableControlAdapter* NewL( CXnNodePluginIf& aNode );
		
        /**
        * Destructor.
        */
		~CXnScrollableControlAdapter();
         
    private: // Functions from base classes    

        /**
        * see CCoeControl 
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
        * see CCoeControl 
        */
        void Draw(const TRect& aRect) const;        
        
        /**
        * see CCoeControl 
        */
        void SizeChanged();

        /**
        * @see CCoeControl documentation        
        */         
        void MakeVisible( TBool aVisible );

        /**
        * @see MCoeControlHitTest documentation        
        */ 
        TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& aControl) const;

        /**
        * @see CXnControlAdapter documentation        
        */ 
        void HandleScreenDeviceChangedL();

    public: // New functions
        /**
         * Returns Called when view position changes.
         *
         * @param aNewPosition New view position.
         */
        const TPoint DeltaPosition() const;

        /**
         * Reset state.
         *
         */
        void ResetState();
        
        /**
         * Scroll box so that the given node is shown.
         *
         */
        void ShowItem( CXnNode& aNode );

        /**
         * Layout has changed somewhere. Act if needed.
         *
         */
        void LayoutChangedL();
        
    public: // Functions from base classes  
        /**
         * From MAknPhysicsObserver.
         * 
         * Physics emulation has moved the view.
         * 
         * When this method is called client should update its visual and logical
         * states to reflect the changes caused by view move. Note that if the
         * parameter @c aDrawNow is @c EFalse client is not allowed to redraw the
         * display but should only update its logical state.
         * Physics engine will ensure that the last frame is always drawn.
         *
         * @since S60 v5.0
         *
         * @param  aNewPosition  The new position of the view.
         * @param  aDrawNow      @c ETrue if client should redraw the screen,
         *                       otherwise @c EFalse.
         * @param  aFlags        Additional flags, not used currently.
         */
        void ViewPositionChanged( const TPoint& aNewPosition, 
            TBool aDrawNow, TUint aFlags );

        /**
         * From MAknPhysicsObserver.
         * Called when emulation ended.
         */
        void PhysicEmulationEnded();
    
        /**
         * From MAknPhysicsObserver.
         * Returns the observer view position.
         *
         * @return Physics observer view position.
         */
        TPoint ViewPosition() const;

        /**
        * see MCoeControlContext 
        */
        void ResetContext( CWindowGc& aGc ) const;

    private: // Constructors
		/**
        * C++ default constructor.
        */
	    CXnScrollableControlAdapter( CXnNodePluginIf& aNode );

	   	/**
        * Two-phased constructor.        
        */ 
	    void ConstructL();	   
	    
    private: // New functions
        void ReadPropertiesL();
        void InitPhysicEngineL();

    private:    // Data
        /**
         * UiEngine. 
         * Not owned.
         */
        CXnUiEngine*        iUiEngine; 	

        /**
         * Layout node. 
         */
        CXnNodePluginIf&    iNode;
        
        /**
         * Physics. 
         * Own.
         */
        CAknPhysics*        iPhysics;

        /**
         * Pointer down start time.
         */
        TTime               iStartTime;

        /**
         * Start (pointer down) position.
         */
        TPoint              iStartPosition;
        
        /**
         * Position at previous pointer event.
         */
        TPoint              iPreviousPosition;
        
        /**
         * Current view position.
         */
        TPoint              iCurrentPosition;

        /**
         * View position at the beginning.
         */
        TPoint              iStartViewPosition;

        /**
         * Current pointer position.
         */   
        TPoint              iStylusPosition;

        /**
         * Visible area, i.e. area of the scollable control that is to be drawn.
         */          
        TRect               iViewPort;  

        /**
         * True, if layout has been changed and engine needs to be reset.
         */          
        TBool               iLayoutChanged;

        /**
         * Direction
         */                  
        TInt                iDirection;
	};

#endif // __XNSCROLLABLECONTROLADAPTER_H__

// End of File
