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
* Description: 
*
*/


#ifndef CMCSPLUGINENGINE_H
#define CMCSPLUGINENGINE_H

// System includes
#include <e32base.h>
#include <mcsmenu.h>
#include <mcsmenuitem.h>
#include <mcsmenunotifier.h>
#include <msvapi.h>

// User includes
#include "carouselpluginwatcher.h"

// Forward declarations
class CGulIcon;
class CCarouselPluginData;
class CCarouselData;
class CCarouselPlugin;


/**
 * @ingroup group_mcsplugin
 *
 * MCSPluginEngine class
 *
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CCarouselPluginEngine ) : public CBase, 
    public MCarouselPluginWatcherObserver,
    public MMsvSessionObserver
    {
public:  
    // constructor and destructor

    /**
    * Part of the two phased constuction
    *
    * @param aPlugin
    * @param aInstanceUid
    * @return Pointer to the created CMCSPluginEngine object
    */
    static CCarouselPluginEngine* NewL( CCarouselPlugin& aPlugin, 
        const TDesC8& aInstanceUid );

    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CCarouselPluginEngine();

private:
    // constructors
    
    /**
     * C++ default constructor 
     */
    CCarouselPluginEngine( CCarouselPlugin& aPlugin, const TDesC8& aInstanceUid );

    /*
     * 2nd phase constructor
     */
    void ConstructL();
    
public:
    // new functions
    
    /**
     * Gets the menu data.
     * 
     * @param aIndex
     * @return TMCSData&
     */
    CCarouselData& MenuDataL( const TInt& aIndex );

    /** Gets the menu item count
     * 
     * @return TInt
     */
    TInt MenuItemCount();

    /**
     * Finds the menu item header for given filter.
     * 
     * @param aFilter
     * @return TMenuItem
     */
    TMenuItem FindMenuItemL( CMenuFilter& aFilter );

    /**
     * Retrieves the menu item object from MCS.
     * 
     * @param aMenuItem
     * @return CMenuItem*
     */
    CMenuItem* FetchMenuItemL( CCarouselData& aData);
    
    /**
     * Returns icon for given menu item and given attribute
     * 
     * @param aMenuItem
     * @param aAttr
     * @return CGulIcon*
     */
    CGulIcon* ItemIconL( CMenuItem* aMenuItem, const TDesC& aAttr );

    /**
     * Returns text for given menu item and given attribute
     * 
     * @param aMenuItem
     * @param aAttr
     */
    TPtrC ItemTextL( CMenuItem* aMenuItem, const TDesC& aAttr );

    /** 
     * Launches menu item
     * 
     * @param aIndex
     */
    void LaunchItemL( const TInt& aIndex );

    /**
    * Set backup/restore state
    *
    * @param aBackupRestore ETrue if backup/restore is ongoing 
    * @return void
    */
    void SetBackupRestore( TBool aBackupRestore );

    /**
     * ShowSettingsL
     */
    void ShowSettingsL();

private:
    // from MMCSPluginWatcherObserver
    
    /**
     * @see MMCSPluginWatcherObserver 
     */
    void HandleNotifyL();

private:    
    // from MMsvSessionObserver

    /**
     * @see MMsvSessionObserver
     */    
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1,
        TAny* aArg2, TAny* aArg3 );

private:
    // new functions

    /**
     * InitL
     */
    void InitL();
    
    /**
     * Tells the settings container to start observing 
     * for changes in mailbox db and changes in MCS.
     */
    void StartObservingL();

    /**
     * Tells the settings container to stop observing 
     * for changes in mailbox db.     
     */
    void StopObserving();

    /**
     * ConstructMenuItemForIconL
     * 
     * @param aPath
     * @param aMenuItem
     * @return TBool
     */
    TBool ConstructMenuItemForIconL( 
        const TDesC& aPath, CMenuItem& aMenuItem );

    /**
     * Creates bookmark specific MCS menu item.
     */
    CMenuItem* CreateBkmItemL( CCarouselData& aData );
    
    /**
     * Creates mailbox specific MCS menu item.
     */
    CMenuItem* CreateMailboxItemL( CCarouselData& aData );
    
    void LaunchFolderItemL( CCarouselData& aData );
    
    void LaunchBookmarkItemL( CCarouselData& aData );
    
    void LaunchMailboxItemL( CCarouselData& aData );
    
    void LaunchMCSItemL( CCarouselData& aData );

private:
    // data
    
    /** Plugin data, owned */
    CCarouselPluginData* iPluginData;
    /** MCS resource handle, owned */
    RMenu iMenu;
    /** MCS change notifier handle, owned */
    RMenuNotifier iNotifier;
    /** MCS asynchronous operation watcher, owned */
    CCarouselPluginWatcher* iWatcher;
    /** MCS change notifier watcher, owned */
    CCarouselPluginWatcher* iNotifyWatcher;
    /** MCS plugin, not owned */
    CCarouselPlugin& iPlugin;
    /** Reference to plugin owned instanceUid */
    const TDesC8& iInstanceUid;
    /** Flag Indicating that backup/restore is in progress */
    TBool iBackupRestore;
    /** Offset of resource file */
    TInt iResourceOffset;
    /** Message server session, owned */
    CMsvSession* iMsvSession;        
    /** "Undefined" menu item, owned */
    CMenuItem* iUndefinedItem;
    /** "Undefined" menu item header */
    TMenuItem iUndefinedItemHeader;    
    };

#endif // CMCSPLUGINENGINE_H

// End of file
