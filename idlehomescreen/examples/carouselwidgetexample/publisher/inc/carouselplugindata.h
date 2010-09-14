/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in data class
*
*/


#ifndef CMCSPLUGINDATA_H
#define CMCSPLUGINDATA_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include <mhomescreensettingsobserver.h>
#include <hspluginsettings.h>
#include <propertymap.h>
#include <mcsmenu.h>

class TMenuItem;
class CCarouselPluginEngine;

/**
 *  @ingroup group_mcsplugin
 *
 *  Stores the MCS Menu Items and keeps track whether
 *  item needs to be published or not.
 *
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CCarouselData ) : public CBase
    {
public:
    CCarouselData();
    
    ~CCarouselData();
    
    /**
     * SetMenuItem
     * 
     * @param aMenuItem
     */
    void SetMenuItem( TMenuItem& aMenuItem );

    /**
     * MenuItem
     * 
     * @return TMenuItem
     */
    TMenuItem& MenuItem();
    
    /**
     * Name of the item.
     */
    TDesC& Name();
    
    /**
     * Set name of the item,
     */
    void SetNameL( const TDesC& aName );

    /**
     * Value of the item. Used for bookmark url.
     */
    TDesC& Value();
    
    /*
     * Set value of the item.
     */   
    void SetValueL( const TDesC& aValue );
    
    /**
     * SetDirty
     * 
     * @param aDirty
     * @return TBool
     */
    void SetDirty( TBool aDirty );

    /**
     * IsDirty
     * 
     * @return TBool
     */
    TBool IsDirty() const;

private:
    /**
     * iMenuItem
     */
    TMenuItem iMenuItem;
    
    /**
     * Item name, own
     */
    HBufC* iName;
    
    /**
     * Item value, own
     */
    HBufC* iValue;

    /**
     * iDirty
     */
    TBool iDirty;
    };

/**
 *  @ingroup group_mcsplugin
 *
 *  Plug-in data class
 *
 *  @since
 */
NONSHARABLE_CLASS( CCarouselPluginData ) : public CBase,
    public HSPluginSettingsIf::MHomeScreenSettingsObserver
    {

public:

    /**
    * Part of the two phased constuction
    *
    * @param aEngine
    * @param aInstanceUid
    * @return Pointer to the created CMCSPluginData object
    */
    static CCarouselPluginData* NewL( CCarouselPluginEngine& aEngine,
        const TDesC8& aInstanceUid );

    /**
    * Constructor
    *
    * @param aEngine
    * @param aInstanceUid
    * @return none
    */
    CCarouselPluginData( CCarouselPluginEngine& aEngine, const TDesC8& aInstanceUid );

    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CCarouselPluginData();

    /**
     * DataItem
     * 
     * @param aIndex
     * @return TMCSData&
     */
    CCarouselData& DataItemL( TInt aIndex );

    /**
     * Saves 'Undefined' menu item into settings when mailbox is deleted
     * 
     * @param aIndex
     * @param aMenuItem
     */
    void SaveUndefinedItemL( const TInt& aIndex );

    /**
     * DataCount
     * 
     * @return TInt
     */
    TInt DataCount(){ return iData.Count();};

    /**
     * Gets the instance specific settings from HSPS and creates data items
     */
    void UpdateDataL();

    /**
     * Removes data item from data list and saves new setting into HSPS
     */
    void RemoveDataL( TInt aId );
    
    // From MHomeScreenSettingsObserver
    /**
     * SettingsChangedL
     * 
     * @param aEvent
     * @param aPluginName
     * @param aPluginUid
     * @param aPluginId
     */
    void SettingsChangedL( const TDesC8& aEvent,  const TDesC8& aPluginName,
                           const TDesC8& aPluginUid, const TDesC8& aPluginId );

private:

    /**
    * Part of the two phased construction
    *
    * @param void
    * @return void
    */
    void ConstructL();

    /**
     * GetMenuDataL
     * @param aProperties
     * @return TMenuItem
     */
    CCarouselData* GetMenuDataL(
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties );
    
    /**
     * Get bookmark data item
     * @param aUid, used for bookmark uid
     * @param aView, used for bookmark url
     * @param aParam, used for bookmark name
     * @param aData, is filled with appropriate values
     */
    void GetBkmDataL( const TDesC8& aUid, const TDesC8& aView, const TDesC8& aParam, CCarouselData& aData );

    /**
     * Get folder data item
     * @param aParam, is used for folder id (in MCS)
     * @param aData, is filled with appropriate values
     */    
    void GetFolderData( const TDesC8& aParam, CCarouselData& aData );

    /**
     * Get mailbox data item
     * @param aUid, uid of the mailbox in messaging application
     * @param aParam, name of the mailbox
     * @param aData, is filled with appropriate values
     */
    void GetMailboxDataL( const TDesC8& aUid, const TDesC8& aParam,  CCarouselData& aData );
    
    /**
     * Get MCS data item
     * @param aProperties, Properties are used to filter correct item from MCS.
     * @param aData, is filled with appropriate values
     */
    void GetMCSDataL(
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties,  CCarouselData& aData );

private: // data

    // Menu items, which are defined in settings
    // Own
    RPointerArray<CCarouselData> iData;

    // Plugin settings. NOT OWNED!
    HSPluginSettingsIf::CHomescreenSettings* iPluginSettings;

    // Reference to MCS plug-in engine
    CCarouselPluginEngine& iEngine;

    // Reference to instance uid of HSPS widget
    const TDesC8& iInstanceUid;

    // MCS resource handle, owned 
    RMenu iMenu;

    };

#endif // CMCSPLUGINDATA_H
