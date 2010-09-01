/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   item presentation model
*
*/


#ifndef HNITEMMODEL_H_
#define HNITEMMODEL_H_

#include <e32base.h>
#include <e32hashtab.h>

class CHnMenuItemModel;
class MHnMenuItemModelIterator;
class CHnActionModel;
class CHnToolbarModel;
class CHnEventHandler;
class CHnAttributeBase;
class CLiwGenericParamList;
class CHnButtonModel;

/** 
 * Item flag. Used to indicate if item is move locked,
 * remove locked or runs.
 */
enum TItemFlag
    {
    EItemFlagDefault = 0,
    EItemFlagRemoveLocked = 1,
    EItemFlagMoveLocked = 2,
    EItemFlagRunning = 4,
    EItemFlagDrmExpired = 8
    };

/** 
 * Service item type. Type of the item in service. 
 */
enum TMcsItemType
    {
    EItemTypeUnknown = 0,
    EItemTypeFolder,
    EItemTypeSuite,
    EItemTypeApplication,
    EItemTypeParentFolder,
    };

/**
 *  Item presentation model class.
 * 
 *  Instance of this class represents item visible 
 *  on the screen. A single CHnMdItem may lead to 
 *  creation of multiple CHnItemModel instances.
 *
 *  @see CHnMdItem
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnItemModel ) : public CBase
    {
    
public:
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnItemModel* NewL();
    
    /**
     * Virtual Destructor.
     * 
     * @since S60 5.0
     */
    ~CHnItemModel();
    
    /**
     * Sets menu item model.
     * 
     * @since S60 5.0
     * @param aMenu visual model for menu item, transfers ownership.
     */
    IMPORT_C  void SetMenuItemModel(  CHnMenuItemModel* aMenu );
    
    /**
     * Gets menu item model.
     * 
     * @since S60 5.0
     * @return menu item structure
     */
    MHnMenuItemModelIterator* GetMenuStructure();
    
    /**
     * Gets actions models.
     * 
     * @param aId Event id
     * @return Array of actions models corresponding to event id
     */
    IMPORT_C CArrayPtr<CHnActionModel>* GetActions( TInt aId );
    
    /**
     * Gets actions models.
     * 
     * @return List of all actions for every event.
     */
    IMPORT_C RHashMap< TInt, CArrayPtr<CHnActionModel>* >& GetActions();
    
    /**
     * Gets toolbar model.
     * 
     * @since S60 5.0
     * @return Toolbar model for item, NULL if not exist
     */
    IMPORT_C const CHnToolbarModel* GetToolbarModel() const;
    
    /**
     * Sets toolbar model.
     * 
     * @since S60 5.0
     * @param aToolbar Toolbar model for item, transfers ownership
     */
    IMPORT_C void SetToolbarModel( CHnToolbarModel* aToolbar );
    
    /**
     * event handling
     * 
     * @since S60 5.0
     * @param aEventHandler An instance of event handler.
     * @param aEventId Event id.
     * @param aEventParameters Parameters of that event
     * @return Error code
     */
    TInt OfferHnEventL( CHnEventHandler & aEventHandler, 
            const TInt aEventId, CLiwGenericParamList* aEventParameters );

    /**
     * Sets attribute.
     * 
     * @since S60 5.0
     * @param aAttr Attribute name
     */
    IMPORT_C void SetAttributeL( CHnAttributeBase* aAttr );
 
    /**
     * Gets attribute.
     * 
     * @since S60 5.0
     * @param aAttribute Attribute name.
     * @return Attribute instance. 
     */
    IMPORT_C CHnAttributeBase* GetAttribute( const TDesC8 & aAttribute );
    
    /**
     * Clears attributes.
     * 
     * @since S60 5.0
     */
    IMPORT_C void ClearAttributesL();

    /**
     * Gets template name.
     * 
     * @since S60 5.0
     * @return Template name
     */
    IMPORT_C const TDesC8& GetTemplate();
    
    /**
     * Sets middle soft key.
     * 
     * @since S60 5.0
     * @param aMsk Msk model for item.
     */
    IMPORT_C void SetMiddleSoftKey( CHnButtonModel* aMsk );
    
    /**
     * Gets middle soft key.
     * 
     * @since S60 5.0
     * @return Msk model for item.
     */
    IMPORT_C CHnButtonModel* GetMiddleSoftKey();
    
    /**
     * Sets template.
     * 
     * @since S60 5.0
     * @param aTemplate Template name.
     */
    IMPORT_C void SetTemplateL( const TDesC8& aTemplate );
    
    /**
     * Sets item's flag.
     * 
     * @since S60 5.0
     * @param aFlag A flag to be set.
     */
    IMPORT_C void SetFlag( TInt aFlag );
    
    /**
     * Removes all flags of the item.
     * 
     * @since S60 5.0
     */
    IMPORT_C void ClearFlags();
    
    /**
     * Returns true if deleting the item is locked.
     * 
     * @since S60 5.0
     * @return True if deleting the item is forbidden, otherwise - false
     */
    IMPORT_C TBool IsDeleteLocked();
    
    /**
     * Returns true if moving the item is locked.
     * 
     * @since S60 5.0
     * @return True if moving the item is forbidden, otherwise - false
     */
    IMPORT_C TBool IsMoveLocked();
    
    /**
     * Returns true if the item is running.
     * 
     * @since S60 5.0
     * @return True if the item is running, otherwise - false.
     */
    IMPORT_C TBool IsRunning();
    
    /**
     * Returns true if the item drm rights are expired.
     * 
     * @since S60 5.0
     * @return True if the item drm rights are expired, otherwise - false.
     */
    IMPORT_C TBool IsDrmExpired();
    
    /**
     * Sets type of the item.
     * 
     * @since S60 5.0
     * @param aType type
     */
    IMPORT_C void SetType( const TDesC8& aType );
    
    /**
     * Sets custom id.
     * 
     * @since S60 5.0
     * @param aCustomId Custom id.
     */
    IMPORT_C void SetCustomId( TInt64 aCustomId );
    
    /**
     * Returns custom id.
     * 
     * @since S60 5.0
     * @return Custom id.
     */
    IMPORT_C TInt64 CustomId();
    
    /**
     * Returns the type of the item.
     * 
     * @since S60 5.0
     * @return Item type.
     */
    IMPORT_C TMcsItemType GetItemType();
    
    /**
     * Returns the uid of the item.
     * 
     * @since S60 5.0
     * @return Item uid.
     */
    IMPORT_C TUid GetItemUid();
    
    /**
     * Sets uid of the item.
     * 
     * @since S60 5.0
     * @param Item uid.
     */
    IMPORT_C void SetItemUidL( const TDesC8& aUid );
    
    /**
     * Deletes all action models.
     * 
     * @since S60 5.0
     */
    IMPORT_C void DeleteAllActionModels();
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    void RemoveLiwObjects();
    
private:
    
    /**
     * Default constructor.
     */
    CHnItemModel();
            
    /**
     * Second phase constructor.
     */
    void ConstructL();
    
private: // data

    /**
     * Template.
     */
    RBuf8 iTemplate;
    
    /**
     * Menu item structure
     * Own
     */
    CHnMenuItemModel* iMenuModel;
    
    /**
     * Model for toolbar
     * Own
     */
    CHnToolbarModel* iToolbarModel;
    
    /**
     * Actions corresponding to events
     * event id <=> array of actions models
     */
    RHashMap< TInt, CArrayPtr<CHnActionModel>* > iActions; 
    
    /**
     * Visual item model (Aakash)
     */
    TBool iEditable;

    /**
     * Attributes.
     */
    RPointerArray<CHnAttributeBase> iAttrs;
    
    /**
     * Flags.
     */
    TInt iFlags;
    
    /**
     * Type of an item.
     */
    TMcsItemType iType;
    
    /**
     * Uid of the item.
     */
    TUid iUid;
    
    /**
     * Custom Id.
     */
    TInt64 iCustomId;
    
    /**
     * Middel soft key model
     * Own
     */
    CHnButtonModel* iMskModel;
    
    };

#endif // HNITEMMODEL_H_
