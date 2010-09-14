/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Background manager
*
*/

#ifndef CXNBACKGROUNDMANAGER_H
#define CXNBACKGROUNDMANAGER_H

// System includes
#include <coecntrl.h>
#include <AknsSrvClient.h>
#include <coeview.h>
#include <disknotifyhandler.h>

// CLASS DECLARATION
class CXnViewManager;
class CXnViewData;
class CAknsLayeredBackgroundControlContext;

namespace hspswrapper
    {
    class CHspsWrapper;
    }

using namespace hspswrapper;

/**
*  Background manager.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CXnBackgroundManager ) : public CCoeControl,
    public MAknsSkinChangeObserver, public MDiskNotifyHandlerCallback
    {
public:

   enum WppType
        {
        ENone,
        ECommon,        
        EPageSpecific 
        };
   
    /**
     * Two-phased constructor.
     * @param aWrapper HSPS wrapper
     * @return new instance of CXnWallpaperView.
     */
    static CXnBackgroundManager* NewL( CXnViewManager& aViewManager, CHspsWrapper& aWrapper );

    /**
     * Destructor.
     */
    ~CXnBackgroundManager();

    /**
     * Caches wallpaper image.
     * 
     * @since S60 5.0
     * @param aFileName WallpaperImage image path and filename to be cached
     * @param aViewData Page that owns the given wallpaper.
     * @return Error code.
     */            
    TInt ConstructWallpaper( const TDesC& aFileName, CXnViewData& aViewData );

    /**
     * Shows wallpaper change dialog
     * 
     * @since S60 5.0
     */
    void SetWallpaperL();

    /**
     * Changes wallpaper image of the current page.
     * 
     * @since S60 5.0
     * @param aFileName WallpaperImage image path and filename
     * @return Whether wallpaper is succesfully set or not.
     */
    TBool SetWallpaper( const TDesC& aFileName );

    /**
     * Deletes wallpaper from the given page as well as from the cache.
     * 
     * @since S60 5.0
     * @param aViewData Page where the wallpaper is to be destroyed.
     */            
    void DeleteWallpaper( CXnViewData& aViewData );

    /**
     * Handles wallpaper changing in page change
     * 
     * @since S60 5.0
     * @param aOldView Old view
     * @param aNewView New view 
     * @param aDrawNow ETrue if appearance should be updated
     */
    void ChangeWallpaper( const CXnViewData& aOldView, 
        const CXnViewData& aNewView, TBool aDrawNow );

    /**
     * Returns wallpaper type.
     *  
     * @since S60 5.0
     */
    CXnBackgroundManager::WppType WallpaperType();
            
    /** 
     * Store current wallpaper if needed by using
     * AknsWallpaperUtils::SetIdleWallpaper
     */
    void StoreWallpaperL();

public: // Functions from base classes    

    /**
     * @see CCoeControl
     */    
    void MakeVisible( TBool aVisible );

private: // Functions from base classes    

    /**
     * see CCoeControl 
     */
    void Draw(const TRect& aRect) const;  

    /**
     * see CCoeControl 
     */
    void SizeChanged();
    
    /**
     * see MAknsSkinChangeObserver 
     */
    void SkinContentChanged();
    
    /**
     * see MAknsSkinChangeObserver 
     */
    void SkinConfigurationChanged(
        const TAknsSkinStatusConfigurationChangeReason aReason );
    
    /**
     * see MAknsSkinChangeObserver 
     */
    void SkinPackageChanged(
        const TAknsSkinStatusPackageChangeReason aReason );

    /**
    * From MDiskNotifyHandlerCallback.
    */
    void HandleNotifyDisk( TInt aError, const TDiskEvent& aEvent );   

private:
    
    /**
     * C++ default constructor.
     */
    CXnBackgroundManager( CXnViewManager& aViewManager, CHspsWrapper& aWrapper );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Gets dublicate bitmap from AknsSrv cache and stores ot to view data.
     */
    void UpdateViewData( const TDesC& aFileName, CXnViewData& aViewData );

    /**
     * Stores current wallpaper to HSPS.
     */
    void SaveWallpaperL();
    TInt SetSettingPropertyL( const TDesC8& aPluginId, const TDesC8& aItemId,
        const TDesC8& aPropertyName, const TDesC8& aPropertyValue );

    /**
     * Goes through all the views and updates wallpapers to the correct size.
     */
    void UpdateWallpapersL();
    
    /**
     * Removes wallpaper from AknsSrv cache, 
     * if it is not used in other pages than the given (or active page).
     */
    void RemoveWallpaperFromCache( const TDesC& aFileName, 
        CXnViewData* aViewData = NULL );
    
    /**
     * Removes wallpapers that were loaded from MMC.
     */
    void RemovableDiskRemovedL();

    /**
     * Tries to restore wallpapers from MMC.
     */
    void RemovableDiskInsertedL();
    
    /**
     * Reads from cenrep wheteher page specific or common wallpaper is enabled 
     * and acts accordingly.
     */
    void CheckFeatureTypeL();
    
    /**
     * Set the given wallpaper to the active page.
     */
    void SetPageSpecificWallpaperL( const TDesC& aFileName );

    /**
     * Set the given wallpaper. 
     * aSave states whether to save change to HSPS or not.
     */
    void SetCommonWallpaperL( const TDesC& aFileName, TBool aSave = ETrue, 
        TBool aShowProgressBar = ETrue );

    /**
     * Called when wallpaper has been changed outside of Homescreen. 
     * aSave states whether to save change to HSPS or not.
     */
    void ReadWallpaperFromCenrepL();    

    /**
     * Helper functions for drawing
     */
    void DrawEditModeBackgroundSkin() const;    
    void DrawStatusPaneMask() const;    
    void UpdateStatuspaneMaskL();

private: // data

    /**
     * Provides needed services
     */
    CXnViewManager& iViewManager;
    
    /** 
     * Hsps wrapper 
     */
    CHspsWrapper& iHspsWrapper;
    
    /** 
     * skin server to detect when wallpaper has changed 
     * Own.
     */
    RAknsSrvSession iSkinSrv;

    /**
     * File server session.
     * Own.
     */
    RFs iFsSession;

    /**
     * Notifier for disk drive status changes.
     * Own.
     */
    CDiskNotifyHandler* iDiskNotifier;

    /** 
     * Background control context. 
     * Own. 
     */    
    CAknsLayeredBackgroundControlContext* iBgContext;
    
    /** 
     * Current view rect, which is basically the whole screen.
     */        
    TRect iRect;

    /** 
     * Stores wallpaper type. 
     */
    CXnBackgroundManager::WppType iType;
    
    /** 
     * Internal wallpaper update in progress
     */
    TInt iIntUpdate;

    /**
     * Status pane bitmap. Drawn on top of a wallpaper in 
     * the status pane area.
     * Own.
     */
    CFbsBitmap* iSpBitmap;

    /**
     * Pointer to status pane mask. Drawn on top of a wallpaper in 
     * the status pane area.
     * Own.
     */
    CFbsBitmap* iSpMask;
    
    /** 
     * Path of the wallpaper image, including filename. 
     * This is used only if same wallpaper 
     * is shared among all HS pages.
     * Own. 
     */  
    HBufC* iBgImagePath;    

    /** 
     * Pointer to wallpaper image. This is used only if same wallpaper 
     * is shared among all HS pages.
     * Own. 
     */   
    CFbsBitmap* iBgImage;

    /*
     * Flag to indicate wheter wallpaper information needs
     * to be stored to AknsWallpaperUtils
     */
    TBool iStoreWallpaper;
    
    /** 
     * Pointer to wallpaper image. 
     * Usually wallpaper is read from active view data, but in case of 
     * finger follow swipe, wallpaper is changed during the page switch. 
     * Therefore this is used only during finger follow. 
     * 
     * Not own. 
     */   
    mutable CFbsBitmap* iWallpaper;

    /*
     * Flag to indicate to use bitmap stored in iWallpaper 
     * instead of reading it from CXnViewData. 
     * This is used when page is being changed with finger follow swipe.
     */
    mutable TBool iImageChanged;
    };

#endif      // CXNBACKGROUNDMANAGER_H

// End of File
