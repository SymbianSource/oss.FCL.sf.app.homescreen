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


#ifndef _XNVIEWDATA_H
#define _XNVIEWDATA_H

// User inlcudes
#include "xnplugindata.h"

// Forward declarations
class CXnViewManager;
class CFbsBitmap;

// Constants

// Class declaration

/**
 * Holds view data in UiEngine
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnViewData ) : public CXnPluginData
    {
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnViewData* NewL( CXnPluginData& aParent );
    static CXnViewData* NewLC( CXnPluginData& aParent );

    ~CXnViewData();

private:
    // private constructors
    
    CXnViewData( CXnPluginData& aParent );
    void ConstructL();

public:
    // From CXnPluginData
    
    /**
     * @see CXnPluginData           
     */    
    void LoadL();

    /**
     * @see CXnPluginData           
     */    
    void Destroy();
            
    /**
     * @see CXnPluginData           
     */    
    void ResourcesL( CArrayPtrSeg< CXnResource >& aList ) const;
    
    /**
     * @see CXnPluginData           
     */    
    void ControlsL( RPointerArray< CXnControlAdapter >& aList ) const;
    
    /**
     * @see CXnPluginData           
     */    
    void ContentSourceNodesL( RPointerArray< CXnNode >& aList ) const;
    
    /**
     * @see CXnPluginData           
     */    
    void InitialFocusNodesL( RPointerArray< CXnNode >& aList ) const;
    
    /**
     * @see CXnPluginData           
     */    
    void AppearanceNodesL( RPointerArray< CXnNode >& aList ) const;
    
    /*
     * @see CXnPluginData
     */        
    void LoadDataPluginsL();    

    /*
     * @see CXnPluginData
     */            
    void DataPluginsLoadCompletedL( TInt aStatus );
    
    /*
     * @see CXnPluginData
     */    
    void DestroyDataPluginsL();
    
    /*
     * @see CXnPluginData
     */
    TBool DataPluginsLoaded() const;
    
public:   
    // New functions
    
    /**
     * Sets viewdata to active/ not active
     * 
     * @param aActive ETrue active, EFalse not active
     */
    void SetActiveL( TBool aActive );
    
    /**
     * Returns if active or not
     * 
     * @return ETrue active, EFalse otherwise
     */
    inline TBool Active() const;

    /**
     * Sets viewdata to be initial         
     */    
    inline void SetInitial();

    /**
     * Returns if initial or not
     * 
     * @return ETrue initial, EFalse otherwise
     */
    inline TBool Initial() const;
    
    /**
     * Sets focused node
     * 
     * @param aNode Focused node
     */
    inline void SetFocusedNode( CXnNode* aNode );
    
    /**
     * Returns focused node
     * 
     * @return focused node
     */
    inline CXnNode* FocusedNode() const;

    /**
     * Sets this view data to use empty widget
     * 
     * @param aUse ETrue use, EFalse don't use
     */    
    inline void SetUseEmptyWidget( TBool aUse );

    /**
     * Returns whether empty widget is in use in this view
     * 
     * @return ETrue if empty widget is used, EFalse otherwise
     */        
    inline TBool UseEmptyWidget() const;
        
    /**
     * Finds and reurns plugin/view data for node
     * 
     * @param aNode Node
     * @return Plugin data
     */
    CXnPluginData& Plugin( CXnNode* aNode );

    /**
     * Finds and reurns plugin/view data for namespace
     * 
     * @param aNamespace Namespace
     * @return Plugin data
     */
    CXnPluginData* Plugin( const TDesC8& aNamespace );
    
    /**
     * Gets view data's plugin nodes
     * 
     * @param aList Data returned in list
     */    
    void PluginNodesL( RPointerArray< CXnNode >& aList ) const;

    /**
     * return view data's view node
     * 
     * @return View node
     */
    CXnNode* ViewNode() const;

    /**
     * Set wallpaper image. Ovnership is transferred.
     * 
     * @param aBitmap Background image file name.
     */
    void SetWallpaperImage( CFbsBitmap* aBitmap );

    /**
     * Get background image. Ovnership is NOT transferred. 
     * 
     * @return Pointer to background image. 
     *         Returns NULL if image has not been set.
     */
    CFbsBitmap* WallpaperImage() const;

    /**
     * Set path of the wallpaper image.
     * 
     * @param aFileName Image path, including file name.
     */
    void SetWallpaperImagePathL( const TDesC& aFileName );
    
    /**
     * Get the path of wallpaper image.
     * 
     * @return Image path, including file name. 
     *         Returns KNullCDes if wallpaper has not been set.
     */
    const TDesC& WallpaperImagePath() const;

    /**
     * Sets view's locking_status attribute (locked/none) to determine if view
     * is prevented from removing/deleting or not
     * 
     * @param aLockingStatusString attr. locking_status ("locked"/"none")
     */
    void SetLockingStatus( const TDesC8& aLockingStatusString );
    
private:
    // data                              
    
    /** Focused Node, Not owned */
    CXnNode* iFocusedNode;

    /** Pointer to wallpaper image, Owned */    
    CFbsBitmap* iBgImage;
    
    /** Path of the wallpaper image, including filename */    
    HBufC* iBgImagePath;
    };

#include "xnviewdata.inl"

#endif // _XNVIEWDATA_H
