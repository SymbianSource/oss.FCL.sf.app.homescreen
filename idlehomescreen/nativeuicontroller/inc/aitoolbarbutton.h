/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Touch Toolbar button
*
*/


#ifndef C_AITOOLBARBUTTON_H
#define C_AITOOLBARBUTTON_H

#include <e32std.h>
class CGulIcon;
class CAknButton;

namespace AiNativeUiController
{

class CAiToolbarButton : public CBase
    {
    public: // constructors
       
        virtual ~CAiToolbarButton();
        
        static CAiToolbarButton* NewL(); 
        
        /**
         * Returns a button constructed using
         * the information available to button
         * Ownership transferred
         */
        CAknButton* ButtonL();
        
        /**
         * Returns the shortcut id           
         */
        TInt ShortcutId();
        
        /**
         * Set the shortcut id 
         */
        void SetShortcutId( TInt aId );


        /**
         * Return the tooltip
         */                     
        TDesC& Tooltip();
             
        CGulIcon* Icon( TBool aTransferOwnership = EFalse );
        
        /**
         * Sets tooltip. Creates own copy of the descriptor
         */
        void SetTooltipL(const TDesC& aTooltip);
        
        /**
         * Sets icon. Ownership moved if aTransferOwnership is set
         *
         * @param aIcon The actual icon
         * @param aTransferOwnership ETrue in case we take ownership, EFalse
         *   if caller wants to keep the ownership
         */
        void SetIcon( CGulIcon *aIcon, TBool aTransferOwnership = EFalse );
        
        /**
         * Sets the icon ownership to us or to the caller
         * 
         * @param aValue ETrue if ownership moved to us. 
         *               EFalse if caller wishes to keep it
         */
        void SetIconExternallyOwned( TBool aValue );

        
    private:
    
            
        CAiToolbarButton();
        
        void ConstructL();
        
    private:
        //Data
        
        /**
         * Own shortcut id
         */
        TInt iShortcutId;
           /** 
         * Is icon externally ow internally owned
         */
        TBool iIconNotOwned;
        
        /**
         * The icon. Depending on the flag above, owned or not
         */
        CGulIcon *iIcon;
        
        /**
         * Owned
         */
        HBufC *iTooltip;     

 

        
    };
    
}  // namespace AiNativeUiController

#endif //C_AITOOLBARBUTTON_H

// End of File.
