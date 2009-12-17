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
* Description:   menu item presentation model
*
*/


#ifndef HNMENUITEMMODEL_H_
#define HNMENUITEMMODEL_H_

#include <e32base.h>

class CHnMenuItemModel;

/**
 *  Menu item model iterator.
 *
 *  Used to iterate on menu items structure. Provides basic
 *  iterator interface.
 *  
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
class MHnMenuItemModelIterator
    {
public:

    /**
     * Checks whether there is at least one more element.
     *
     * @since S60 5.0
     * @return ETrue if there is a next item, otherwise EFalse.
     */
    virtual TBool HasNext() = 0;

    /**
     * Gets the next item and move the cursor to the next item.
     *
     * @since S60 5.0
     * @return A pointer to the element or NULL if there are no more elements.
     */
    virtual CHnMenuItemModel* GetNext() = 0;

    /**
     * Checks whether there is at least one more element (item specific).
     *
     * @since S60 5.0
     * @return ETrue if there is a next item, otherwise EFalse.
     */
    virtual TBool HasNextSpecific() = 0;
    
    /**
     * Gets the next menu item ( specific for the item 
     * - e.g. Applications folder item )
     * and move the cursor to the next menu item.
     *
     * @since S60 5.0
     * @return A pointer to the element or NULL if there are no more elements.
     */
    virtual CHnMenuItemModel* GetNextSpecific() = 0;
    
    /**
     * Resets the iterator.
     *
     * @since S60 5.0
     */
    virtual void Reset() = 0;
    
    };

/**
 *  Menu item model class.
 *
 *  Represents menu item. Contains all data needed to display and 
 *  use menu item properly. Implemented as tree structure.
 *  
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnMenuItemModel ):
    public CBase,
    public MHnMenuItemModelIterator
    {
public:
        
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aName Name for menu item
     */
    IMPORT_C static CHnMenuItemModel* NewL( const TDesC& aName );
        
    /**
     * Virtual Destructor.
     * 
     * @since S60 5.0
     */
    ~CHnMenuItemModel();
    
    /**
     * Gets menu item model.
     * 
     * @since S60 5.0
     * @return Menu item structure
     */
    IMPORT_C MHnMenuItemModelIterator* GetMenuStructure();
    
    /**
     * @see MHnMenuItemModelIterator.
     */
    IMPORT_C virtual TBool HasNext();
    
    /**
     * @see MHnMenuItemModelIterator.
     */
    IMPORT_C virtual CHnMenuItemModel* GetNext();

    /**
     * @see MHnMenuItemModelIterator.
     */
    IMPORT_C virtual TBool HasNextSpecific();
    
    /**
     * @see MHnMenuItemModelIterator.
     */
    IMPORT_C virtual CHnMenuItemModel* GetNextSpecific();
    
    /**
     * @see MHnMenuItemModelIterator.
     */
    IMPORT_C virtual void Reset();
    
    /**
     * Gets name.
     *
     * @since S60 5.0
     * @return Name of the menu.
     */
    IMPORT_C const TDesC& NameL();
    
    /**
     * Gets command id.
     *
     * @since S60 5.0
     * @return Command id.
     */
    IMPORT_C TInt Command();
        
    /**
     * Sets command id.
     *
     * @since S60 5.0
     * @param aCommandId Command id from data model.
     */
    IMPORT_C void SetCommand( TInt aCommandId );
    
    /**
     * Gets menu item position.
     *
     * @since S60 5.0
     * @return Position of item in menu.
     */
    IMPORT_C TInt Position();
    
    /**
     * Sets menu item position.
     *
     * @since S60 5.0
     * @param aPosition Position of item in menu.
     */
    IMPORT_C void SetPosition( TInt aPosition );
    
    /**
     * Sets menu item specific flag.
     *
     * @since S60 5.0
     * @param aItemSpecific Is menu element item specific.
     */
    IMPORT_C void SetItemSpecific( TBool aItemSpecific );

    /**
     * Appends child item.
     *
     * @since S60 5.0
     * @param aMenuModel Menu item model.
     */
    IMPORT_C void AppendChildMenuL( CHnMenuItemModel* aMenuModel );
    
    /**
     * Tells if menu item is item specific.
     * 
     * @since S60 5.0
     * @return ETrue if menu item is item specific.
     */
    IMPORT_C TBool IsItemSpecific();
    
private:
    
    /**
     * Default constructor.
     * 
     * @since S60 5.0
     */
    CHnMenuItemModel();
    
    /**
     * Second phase constructor.
     * 
     * @since S60 5.0
     * @param aName Name for menu item 
     */
    void ConstructL( const TDesC& aName );
    
private: // data
        
    /**
     * Name value cache.
     */
    RBuf iName;
        
    /**
     * Command Id. 
     */
    TInt iCommand;
        
    /**
     * Position.
     */
    TInt iPosition;
    
    /**
     * Next child.
     */
    TInt iNextMenu;
        
    /**
     * Children pointer array.
     */
    RPointerArray< CHnMenuItemModel > iChildren;
    
    /**
     * Is menu item item specific.
     */
    TBool iItemSpecific;
    };

#endif // HNMENUITEMMODEL_H_
