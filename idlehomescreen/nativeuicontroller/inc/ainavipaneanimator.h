/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Navi pane animator.
*
*/


#ifndef C_AINAVIPANEANIMATOR_H
#define C_AINAVIPANEANIMATOR_H

#include <e32base.h>
#include "ainativeuimodel.h"

namespace AiNativeUiController
{

class CAiStatusPanel;
class MAiNaviPaneAnimatorCallback;

/**
 *  Navi pane animator.
 *
 *  This class handles the navi pane animation when XML Idle gets the focus.
 *  It owns an array that is used for storing all data that is shown in form of
 *  soft indicators. It animates all items in the array in one second intervals. 
 *
 *  @since S60 3.2
 */
class CAiNaviPaneAnimator : public CBase
    {

public:

    static CAiNaviPaneAnimator* NewL( CAiStatusPanel& aStatusPane,
                AiNativeUiModel::TRenderingPriorities& aPriorities, 
                MAiNaviPaneAnimatorCallback& aCallback );

    virtual ~CAiNaviPaneAnimator();
    
    /**
     * Adds content item to the animator array.
     * @param aContentId Content item which value needs to be updated.
                            If item with same aContentId already exists in 
                            the array, it's value is updated. If not, new
                            item is created.
     * @param aText New value for aContentId.
     */
    void AddItemL( TInt aContentId, const TDesC16& aText );

    /**
     * Removes content item with given content id.
     * @param aContentId Content item which needs to be removed.
     */
    void RemoveItem( TInt aContentId );
     
    /**
     * Animates the content of the array.
     */
    void AnimateL();
     
    /**
     * Cancels ongoing animation (if there is one).
     */
    void CancelAnimationL();
    
    /**
     * Called by CPeriodic when timer expires.
     */
    static TInt AnimCallback( TAny* aPtr );     

    /**
     * Draws the text to navi pane.
     * This method is called by timer. It checks if there still are items left
     * in the animation array. If yes, it draws the current item's value. 
     * If not,it cancels the timer.
     */
    TBool DrawTextL();

    /**
     * Sorts iAnimArray in priority order defined by TRenderingPriorities.
     */
    void SortAnimArrayL();
    
    /**
     * Is ongoing animation
     */
    TBool IsActive();


private:

    CAiNaviPaneAnimator( CAiStatusPanel& aStatusPane,
                AiNativeUiModel::TRenderingPriorities& aPriorities, 
                MAiNaviPaneAnimatorCallback& aCallback );

    void ConstructL();
    
    /**
     * Helper class for animation array items.
     * Can be defined as an inner class since not used outside parent class.
     */
    class CAiNameValuePair : public CBase
        {
        public:
        
            static CAiNameValuePair* NewLC( const TInt aName, 
                                            const TDesC& aValue );

            ~CAiNameValuePair();

            TInt Name() const;

            const TDesC16& Value() const;

            void SetValueL( const TDesC& aValue );
            
        private:
        
            void ConstructL( const TInt aName, const TDesC& aValue );
        
        private:    // data
        
            /// Name of the item in name/value pair
            TInt iName;
            
            /// Value or the item in name/value pair
            HBufC* iValue;
        };        


private: // data

    /**
     * Used for doing the animation.
     * Own.
     */
    CPeriodic* iPeriodic;
    
    /// Status pane handler.
    CAiStatusPanel& iStatusPane;
    
    /// Navi pane rendering priorities
    AiNativeUiModel::TRenderingPriorities& iPriorities;
    
    /// Reference for informing about completed animation.
    MAiNaviPaneAnimatorCallback& iCallback;

    /// Array for holding data to be animated.
    RPointerArray<CAiNameValuePair> iAnimArray;
    
    /// Currently ongoing iteration of the animator array.
    TInt iCurrentItem;

    };
    
} // namespace AiNativeUiController

#endif // C_AINAVIPANEANIMATOR_H
