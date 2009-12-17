/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content that is shown inside a stylus popup menu.
*
*/


#ifndef CAKNSTYLUSPOPUPMENUCONTENT_H
#define CAKNSTYLUSPOPUPMENUCONTENT_H

#include <coecntrl.h>

class CAknButton;

const TInt KAiStylusMenuMaxItems = 10;

/**
 *  Class providing content for the stylus popup menu.
 *
 *  @lib eikcoctl.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CAiStylusPopUpMenuContent ) : public CCoeControl
    {
public:

    static CAiStylusPopUpMenuContent* NewL();

    static CAiStylusPopUpMenuContent* NewLC();

    ~CAiStylusPopUpMenuContent();

    /**
     * Adds a new menu item.
     *
     * @param aItem Menu item to be added.
     * @param aCommandId Command ID of the menu item.
     * @param aObserver Item's observer.
     */
    void AddMenuItemL( const TDesC& aItem, const TInt aCommandId,
                       MCoeControlObserver& aObserver );

    /**
     * Removes a menu item.
     *
     * @param aIndex Index of the menu item.
     */
    void RemoveMenuItem( const TInt aIndex );
    
    /**
     * Hides / shows a menu item.
     *
     * @param aIndex Index of the menu item.
     * @param aDimmed ETrue to hide and EFalse to show
     */
    void SetItemDimmed( const TInt aIndex, const TBool aDimmed );
        
    /**
     * Method to get a command id of the specified menu item.
     *
     * @param aControl The menu item.
     * @return Command id.
     */
    TInt CommandId( CCoeControl* aControl ) const;

// from base class CCoeControl

    /**
     * Returns the minimum size of the control.
     *
     * @return Minimum size.
     */
     TSize MinimumSize();
    
    /**
     * Returns the number of component controls.
     *
     * @return Number of component controls.
     */
     TInt CountComponentControls() const;
    
    /**
     * Returns the specified component control.
     *
     * @return The specified control.
     */
     CCoeControl* ComponentControl( TInt aIndex ) const;
     
    /**
     * Handles changes to the control's resources.
     *
     * @aParam aType Type of resource change.
     */     
     void HandleResourceChange( TInt aType );

private:

    CAiStylusPopUpMenuContent();

    void ConstructL();
    
// from base class CCoeControl
    
    void SizeChanged();

private: // data

    /**
     * Array which holds pointers to menu items. Menu item's command id
     * is stored in iCommands array with the same index value .
     */
    RPointerArray<CAknButton> iItems;
    
    /**
     * Array which holds menu items' command IDs.
     */
    RArray<TInt> iCommands;
    
    /**
     * Array which holds menu items' visibility
     */ 
    TInt iVisibility[KAiStylusMenuMaxItems];
    
    };

#endif // CAKNSTYLUSPOPUPMENUCONTENT_H
