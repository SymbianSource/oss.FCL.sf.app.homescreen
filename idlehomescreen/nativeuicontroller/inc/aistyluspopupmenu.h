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
* Description:  Interface for controlling stylus specific popup menu.
*
*/


#ifndef CAKNSTYLUSPOPUPMENU_H
#define CAKNSTYLUSPOPUPMENU_H


#include <e32base.h>
#include <coecobs.h>

class CAknPreviewPopUpController;
class CAiStylusPopUpMenuContent;
class MEikMenuObserver;

/**
 *  Stylus specific popup menu
 *
 *  A floating popup menu which is meant for stylus usage only.
 *  The menu appears near to the tapping position. When an item is 
 *  tapped the menu calls its observer's (iMenuObserver) HandleCommandL.
 *  Typically the observer is application's AppUi. It is then the
 *  observer's responsibility to handle the command appropriately.
 *
 *  @lib eikcoctl.lib
 *  @since S60 v4.0
 */
NONSHARABLE_CLASS( CAiStylusPopUpMenu ) : public CBase, 
                                           public MCoeControlObserver
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aObserver Menu observer (not owned) that handles 
     *                  stylus popup events.
     * @param aPoint The position where stylus was tapped
     * @param aPopup Preview popup from which stylus popup
     *               menu was launched, can be NULL.
     */
    static CAiStylusPopUpMenu* NewL( MEikMenuObserver* aObserver, 
                                         const TPoint& aPoint,
                                         CAknPreviewPopUpController* aPopup );
    
    /**
     * Two-phased constructor
     *
     * @param aObserver Menu observer (not owned) that handles 
     *                  stylus popup events.
     * @param aPoint The position where stylus was tapped
     */
    static CAiStylusPopUpMenu* NewL( MEikMenuObserver* aObserver, 
                                         const TPoint& aPoint );

    /**
     * Two-phased constructor
     *
     * @param aObserver Menu observer (not owned) that handles 
     *                  stylus popup events.
     * @param aPoint The position where stylus was tapped
     * @param aPopup Preview popup from which stylus popup
     *               menu was launched, can be NULL.
     */
    static CAiStylusPopUpMenu* NewLC( MEikMenuObserver* aObserver, 
                                         const TPoint& aPoint,
                                         CAknPreviewPopUpController* aPopup );
    
    ~CAiStylusPopUpMenu();
    
    /**
     * Adds a menu item. If the menu contains maximum amount of
     * items, the function will do nothing. Inserted item will be
     * placed last.
     *
     * @param aItem The menu item to be added.
     * @param aCommandId Item's command id.
     */
    void AddMenuItemL( const TDesC& aItem, const TInt aCommandId );
    
    /**
     * Removes a menu item.
     *
     * @param aCommandId Command id of the item to be removed.
     */
    void RemoveMenuItem( const TInt aCommandId );

    /**
     * Hides / shows a menu item.
     *
     * @param aCommandId Command id of the item to be hidden / shown.
     * @param aDimmed ETrue to hide item and EFalse to show item
     */
    void SetItemDimmed( const TInt aCommandId, const TBool aDimmed );

    /**
     * Shows the stylus popup menu. If it's already visible,
     * nothing happens.
     */
    void ShowMenu();
        
    /**
     * Sets the position of the stylus popup menu. 
     * Position will be adjusted if the menu doesn't fit on the screen
     * properly.
     *
     * @param aPoint Position of the stylus popup menu's top-left corner.
     */
    void SetPosition( const TPoint& aPoint );    
    
// from base class CCoeControl

    /**
     * Constructs the menu from resource.
     *
     * @param aReader Resource reader.
     */
    void ConstructFromResourceL( TResourceReader& aReader );

// from base class MCoeControlObserver
    
    /**
     * Method to handle events that occur when user selects a menu item.
     *
     * @param aControl The control that sent the event.
	 * @param aEventType The event type.
     */
    void HandleControlEventL( CCoeControl* aControl,
                                       TCoeEvent aEventType );

private:

    /**
     * Default constructor
     *
     * @param aObserver Menu observer (not owned) that handles 
     *                  stylus popup events.
     * @param aPoint The position where stylus was tapped
     * @param aPopup Preview popup from which stylus popup
     *               menu was launched, can be NULL.
     */
    CAiStylusPopUpMenu( MEikMenuObserver* aObserver, const TPoint& aPoint,
                         CAknPreviewPopUpController* aPopup );

    void ConstructL();

private: // data

    /**
     * Screen relative position where stylus was tapped.
     */
    TPoint iPosition;
    
    /**
     * Controller for the actual popup.
     * Own.
     */
    CAknPreviewPopUpController* iController;
    
    /**
     * Popup's content.
     * Own.
     */
    CAiStylusPopUpMenuContent* iContent;
    
    /**
     * Observer which will be used to forward selected menu command id.
     * Not own.
     */
    MEikMenuObserver* iMenuObserver;
    
    /**
     * Preview popup which launches stylus popup menu.
     * If NULL, menu wasn't launched from a preview popup.
     * If not NULL, stylus popup menu will close also the preview popup
     * when an option is selected in the stylus popup menu.
     * Not own.
     */
    CAknPreviewPopUpController* iPreviewPopup;
    
    };


#endif // CAKNSTYLUSPOPUPMENU_H
