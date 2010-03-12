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
    TInt Load();

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

    /**
     * @see CXnPluginData           
     */    
    void PopupNodesL( RPointerArray< CXnNode >& aList ) const;
            
public:   
    // New functions
    
    /**
     * Sets viewdata to active/ not active
     * 
     * @param aActive ETrue active, EFalse not active    
     */
    void SetActive( TBool aActive );
    
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
    CXnPluginData* Plugin( CXnNode* aNode );

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
     * Destroys all publishers in this view
     * 
     * @param aReason Destroy reason
     */
    void DestroyPublishers( TInt aReason );
    
private:
    // new functions
    
    void LoadPublishers();    
           
    static TInt DoLoadPublishersL( TAny* aAny );
    
    void DoDestroyPublishersL( TInt aReason );
    
private:
    // data                              
    
    /** Pointer to wallpaper image, Owned */    
    CFbsBitmap* iBgImage;    
    /** Path of the wallpaper image, including filename */    
    HBufC* iBgImagePath;            
    /** Publisher load index */
    TInt iLoadIndex;    
    /** Flag to indicate whether content removed error should be shown */
    TBool iShowContentRemoved;    
    };

#include "xnviewdata.inl"

#endif // _XNVIEWDATA_H
