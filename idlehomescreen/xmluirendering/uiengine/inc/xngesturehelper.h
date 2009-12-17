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
* Description:  Gesture helper implementation
*
*/


#ifndef _XNGESTUREHELPER_H_
#define _XNGESTUREHELPER_H_

// System includes
#include <e32base.h>

// Forward declarations
class CXnNode;
struct TPointerEvent;


/**
 * Swipe directions (left, right, none)
 */
enum TSwipeResult
    {
    ESwipeNone = 0,
    ESwipeLeft,
    ESwipeRight
    };

/**
 * XnGestureHelper namespace
 * Used for the whole gesture family - Gesture recognizer, gesture helper, 
 * Point array
 */
namespace XnGestureHelper
    {
    
    // Forward declarations
    class CHoldingTimer;
    class CXnGesture;
    
    // Constants
    const TReal32 KGestureMinSpeedX = 300;
    const TInt KGestureMinLengthX = 100;
    const TInt KGestureMaxDeltaY = 100;
    
    /**
     *  Implementation of CGestureHelper interface
     *
     *  @ingroup group_xnlayoutengine
     */
    NONSHARABLE_CLASS( CXnGestureHelper ) : public CBase
        {
    
        /** private implementation class */
        friend class CHoldingTimer;
    
    public:
        
        /** 2-phase constructor */
        static CXnGestureHelper* NewL( CXnNode& aNode );

        /** Destructor */
        ~CXnGestureHelper();

        /** 
         * See GestureHelper, SetHoldingEnabled function
         * 
         * @see CGestureHelper::SetHoldingEnabled 
         */
        void SetHoldingEnabled( TBool aEnabled );
        
        /** 
         * See Gesturehelper, IsHoldingEnabled function
         * 
         * @see CGestureHelper::IsHoldingEnabled 
         */
        TBool IsHoldingEnabled() const;

        /** 
         * Gets the owner
         * 
         * @return Owner
         */                
        CXnNode* Owner() const;
        
        /** 
         * Sets destination
         * 
         * @aParam Destination
         */        
        void SetDestination( CXnNode* aDestination );

        /** 
         * Gets destination
         * 
         * @return Destination
         */                         
        CXnNode* Destination() const;
        
        /** 
         * See GestureHelper Handlepointervent
         * 
         * @see CGestureHelper::HandlePointerEventL 
         */ 
        TSwipeResult HandlePointerEventL( const TPointerEvent& aEvent );
        
        /** Reset helper state */
        void Reset();

    private:
        
        /** Constructor */
        CXnGestureHelper( CXnNode& aNode );

        /** 
         * Returns if the helper is processing
         *         
         * @return ETrue if the helper has not started processing a gesture
         */
        inline TBool IsIdle() const; 
        
        /** 
         * Add a point to the sequence of points that make up the gesture 
         */
        inline void AddPointL( const TPointerEvent& aEvent );
        
        /** 
         * Add a point to the sequence of points that make up the gesture
         * 
         * @return                    
         */
        inline TInt AddPoint( const TPointerEvent& aEvent );
        
        /** 
         * Activates holding and notifies observer that holding has been 
         * started 
         */
        void StartHoldingL();
        
        /** 
         * Check if swipe if between defined values 
         * 
         * @return Swiping left/right/none 
         */
        TSwipeResult ValidSwipe();

    private:

        /** 
         * Gesture owner, Not owned.
         */        
        CXnNode& iOwner;        
        
        /** 
         * Gesture is the logical representation of a sequence of points 
         * arriving at certain times
         */ 
        CXnGesture* iGesture;

        /** 
         * holding is activated when user keeps pointer down in the same place 
         * for a longer period. the holding timer activates off when enough 
         * time has passed.
         */
        CHoldingTimer* iHoldingTimer;

        /** 
         * Gesture destination, Not owned.
         */        
        CXnNode* iDestination;        
        };
    } // GestureHelper

#endif // _XNGESTUREHELPER_H_
