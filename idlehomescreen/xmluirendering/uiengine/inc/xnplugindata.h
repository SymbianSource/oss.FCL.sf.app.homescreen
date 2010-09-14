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
* Description:  Data class to hold widget info
*
*/


#ifndef _XNPLUGINDATA_H
#define _XNPLUGINDATA_H

// System includes
#include <e32base.h>
#include <babitflags.h>

// User includes

// Forward declarations
class CXnNode;
class CXnDomNode;
class CXnResource;
class CXnControlAdapter;
class CXnViewData;
class CXnODT;
class CXnViewManager;
class CXnPublisherData;

// Constants

// Class declaration

/**
 * Holds plugin data in UiEngine
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnPluginData ) : public CBase
    {
public:
    // Data types
    enum
        {
        EUseEmpty,
        EIsDispose,
        EIsOccupied,   
        EIsEmpty,
        EIsRemovable,
        EIsActive,             
        EIsInitial,
        EIsEditable
        };
        
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     * 
     * @param aParent Parent     
     */
    static CXnPluginData* NewL( CXnPluginData& aParent );

    /**
     * Two-phased constructor. Leaving on stack
     * 
     * @param aParent Parent
     */
    static CXnPluginData* NewLC( CXnPluginData& aParent );

    /**
     * Destructor
     */
    ~CXnPluginData();
       
protected:
    
    /**
     * C++ constructor
     * 
     * @param aParent
     */
    CXnPluginData( CXnPluginData& aParent );

    /**
     * C++ constructor
     *      
     * @param aManager 
     */    
    CXnPluginData( CXnViewManager& aManager );
                   
    /**
     * C++ constructor
     */
    CXnPluginData();

    /**
     * Leaving constructor
     */
    void ConstructL();

public:
    // New functions

    /**
     * Loads content to plugin
     * 
     * @return KErrNone if succesful, error code otherwise          
     */    
    virtual TInt Load();

    /**
     * Destroys content from plugin          
     */    
    virtual void Destroy();
    
    /**
     * Sets plugindata node
     * 
     * @param aNode Node to be set.
     */
    inline void SetNode( CXnDomNode* aNode );
    
    /**
     * Returns plugindata node
     * 
     * @return Dom Node
     */
    inline CXnDomNode* Node() const;

    /**
     * Sets Owner
     * 
     * @param Dom node owner
     */
    inline void SetOwner( CXnDomNode* aOwner );
    
    /**
     * Returns owner
     * 
     * @return Returns the owning dom node
     */
    inline CXnDomNode* Owner() const;

    /**
     * Returns parent
     * 
     * @return ViewData Parent
     */
    inline CXnPluginData* Parent() const;

    /**
     * Returns View Manager
     * 
     * @return View manager
     */
    inline CXnViewManager& ViewManager() const;
    
    /**
     * Returns state of the parent - active/ not active
     * 
     * @return ETrue - parent active 
     *         EFalse - parent not active
     */
    inline virtual TBool Active() const;

    /**
     * Sets new configuration ID
     * 
     * @param aConfigurationId Config ID
     */
    void SetConfigurationIdL( const TDesC8& aConfigurationId );
    
    /**
     * Returns Config. ID
     * 
     * @return Id
     */
    inline const TDesC8& ConfigurationId() const;
    
    /**
     * Sets new plugin ID
     * 
     * @param aPluginId Plugin ID
     */
    void SetPluginIdL( const TDesC8& aPluginId );
    
    /**
     * Returns plugin ID
     * 
     * @return Plugin ID
     */
    inline const TDesC8& PluginId() const;

    /**
     * Sets new plugin UID
     * 
     * @param aPluginUid Plugin UID
     */
    void SetPluginUidL( const TDesC8& aPluginUid );
    
    /**
     * Returns plugin UID
     * 
     * @return Plugin Uid
     */
    inline const TDesC8& PluginUid() const;

    /**
     * Sets new plugin name
     * 
     * @param aPluginName Plugin name
     */
    void SetPluginNameL( const TDesC8& aPluginName );
    
    /**
     * Returns plugin name
     * 
     * @return Plugin name
     */
    inline const TDesC8& PluginName() const;
    
    /**
     * Sets new plugin type
     * 
     * @param aPluginType Plugin type
     */
    void SetPluginTypeL( const TDesC8& aPluginType );
    
    /**
     * Returns plugin type
     * 
     * @return Plugin type
     */    
    inline const TDesC8& Type() const;

    /**
	 * void SetPublisherNameL( const TDesC8& aPublisherName )
	 * Sets the name of the publisher
	 *
	 * @param const TDesC& aPublisherName - Publisher name
	 */
	void SetPublisherNameL( const TDesC8& aPublisherName );
        
    /**
     * void SetPublisherNameL( const TDesC& aPublisherName )
     * Sets the name of the publisher
     *
     * @param const TDesC& aPublisherName - Publisher name
     */
    void SetPublisherNameL( const TDesC& aPublisherName );

    /**
     * inline const TDesC16& PublisherName()
     * Returns the publisher name
     *
     * @param
     * @return - *iPublisherName or KNullDesC16()       
     */
    inline const TDesC& PublisherName() const;

    /**
     * Sets new resources.     
     * 
     * @param aResources resource list
     */
    void SetResources( CArrayPtrSeg< CXnResource >* aResources );
    
    /**
     * Adds resources to the list
     * 
     * @param aList List are added to
     */
    virtual void ResourcesL( CArrayPtrSeg< CXnResource >& aList ) const;

    /**
     * Adds control from Node's control
     * 
     * @param aNode Node which control should be added
     */
    void SetControlL( CXnNode* aNode );
    
    /**
     * Returns list of controls
     * 
     * @param aList List where controls are added
     */
    virtual void ControlsL( RPointerArray< CXnControlAdapter >& aList ) const;

    /**
     * Sets content source
     * 
     * @param aNode Node to be added
     */
    void SetContentSourceNodeL( CXnNode* aNode );
    
    /**
     * Returns all content source nodes
     * 
     * @param aList List where they are returned
     */
    virtual void ContentSourceNodesL( RPointerArray< CXnNode >& aList ) const;

    /**
     * Returns all publishers
     * 
     * @param aList List publishers
     */
    virtual void PublishersL( RPointerArray< CXnPublisherData >& aList ) const;    
    
    /**
     * Adds appearance nodes
     * 
     * @param aNode Node to be added
     */
    void SetAppearanceNodeL( CXnNode* aNode );
    
    /**
     * Returns list of appearance nodes
     * 
     * @param aList List of apperance nodes
     */
    virtual void AppearanceNodesL( RPointerArray< CXnNode >& aList ) const;

    /**
     * Adds an initial focus node
     * 
     * @param aNode Node
     */
    void SetInitialFocusNodeL( CXnNode* aNode );
    
    /**
     * Returns list of focus nodes
     * 
     * @param aList List of focus nodes
     */
    virtual void InitialFocusNodesL( RPointerArray< CXnNode >& aList ) const;

    /**
     * Returns list of popup nodes
     * 
     * @param aList List of popup nodes
     */
    virtual void PopupNodesL( RPointerArray< CXnNode >& aList ) const;

    /**
     * Sets the plugin occupied     
     */
    inline void SetOccupied();
    
    /*
     * Checks whether occupied
     * 
     * @return ETrue when occupied EFalse when not
     */
    inline TBool Occupied() const;
        
    /*
     * Checks whether removable
     * 
     * @return ETrue when removable EFalse when not
     */    
    inline TBool Removable() const;
    
    /*
     * Checks whether editable
     * 
     * @return ETrue when editable EFalse when not
     */      
    inline TBool Editable() const;
    
    /*
     * Queries whether this plugin is holding "empty" widget
     * 
     * @return ETrue if empty, EFalse otherwise     
     */                
    TBool Empty() const;
    
    /*
     * Sets this plugin to hold "empty" widget
     * 
     * @param aPluginId PluginId for later usage     
     */            
    void SetEmptyL( const TDesC8& aPluginId );
    
    /**
     * Flushes plugin data         
     */
    void Flush();

    /**
     * Sets/Unsets the popup
     * @param aVisible sets the popup visible or invisible
     * @param aNode a pointer to the popup node
     */
    void SetIsDisplayingPopup( TBool aVisible, CXnNode* aNode );

    /**
     * Checks if the popup is displayed
     */
    TBool IsDisplayingPopup() const;
        
    /**
     * Returns list of plugindata
     * 
     * @return List of plugindata
     */
    inline RPointerArray< CXnPluginData >& PluginData() const;

    /**
     * Loads publishers
     * 
     * @param aReason Load reason
     */    
    void LoadPublishers( TInt aReason );

    /**
     * Destroy publishers
     * 
     * @param aReason Destroy reason
     */    
    void DestroyPublishers( TInt aReason );
            
    /**
     * Notifies a publisher is ready
     */        
    virtual void NotifyPublisherReadyL();        

    /**
     * Show content removed error note
     */
    void ShowContentRemovedError();

    /**
     * Show oom error note
     */    
    void ShowOutOfMemError();
    
    /**
     * Sets locking_status attribute (none/locked/permanent) 
     * 
     * @param aStatus "none" / "locked" / "permanent"
     */
    void SetLockingStatus( const TDesC8& aStatus );    
        
protected:
    // data
        
    /** Plugins data */
    mutable RPointerArray< CXnPluginData > iPluginsData;
    /** List of plugin resources, Owned */
    CArrayPtrSeg< CXnResource >* iResources;
    /** List of controls, Not owned */
    RPointerArray< CXnControlAdapter > iControls;
    /** List of publishers, Owned */
    RPointerArray< CXnPublisherData > iPublishers;
    /** List of appearance nodes, Not owned */
    RPointerArray< CXnNode > iAppearanceNodes;
    /** List of initial focus nodes, Not owned */
    RPointerArray< CXnNode > iInitialFocusNodes;   
    /** List of popup focus nodes, Not owned */
    RPointerArray< CXnNode > iPopupNodes;
    /** Parent, Not owned */
    CXnPluginData* iParent;
    /** View manager, Not owned */
    CXnViewManager& iManager;
    /** This plugin's node, Not owned */
    CXnDomNode* iNode;
    /** This plugin's owner, Not owned */
    CXnDomNode* iOwner;
    /** config ID, Owned */ 
    HBufC8* iConfigurationId;
    /** Plugin ID, Owned */
    HBufC8* iPluginId;
    /** Plugin UID, Owned */
    HBufC8* iPluginUid;
    /** Plugin name, Owned */
    HBufC8* iPluginName;    
    /** Plugin type, Owned */
    HBufC8* iPluginType;
    /** Publisher name, Owned */
    HBufC* iPublisherName;
    /** Flags to define this plugin's state */
    TBitFlags32 iFlags;
    };

// Inline functions
#include "xnplugindata.inl"

#endif // _XNPLUGINDATA_H
