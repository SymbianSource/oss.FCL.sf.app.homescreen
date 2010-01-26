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
#include <aknssrvclient.h>
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
    public MAknsSkinChangeObserver, public MCoeViewActivationObserver,
    public MDiskNotifyHandlerCallback
    {
public:

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
     * @return Error code.
     */            
    TInt CacheWallpaperL( const TDesC& aFileName, CXnViewData& aViewData );

    /**
     * Changes wallpaper image of the current page.
     * 
     * @since S60 5.0
     * @param aFileName WallpaperImage image path and filename
     */            
    void AddWallpaperL( const TDesC& aFileName );

    /**
     * Checks whether page specific wallpaper feature is activated or not.
     *  
     * @since S60 5.0
     */
    TBool ActivatedL();

    /**
     * Shows wallpaper change dialog
     * 
     * @since S60 5.0
     */
    void SetWallpaperL();

    /**
     * Handles wallpaper changing in page change
     * 
     * @since S60 5.0
     * @param aOldView         Old view
     * @param aNewView         New view 
     */
    void WallpaperChanged( CXnViewData& aOldView, CXnViewData& aNewView );
   
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
     * @see MCoeViewActivationObserver
     */
    void HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
        const TVwsViewId& aViewIdToBeDeactivated );

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

    void SaveWallpaperL();
    TInt SetSettingPropertyL( const TDesC8& aPluginId, const TDesC8& aItemId,
        const TDesC8& aPropertyName, const TDesC8& aPropertyValue );
    void UpdateWallpapersL();
    void CleanCache();
    void RemoveWallpaperL( CXnViewData& aViewData );
    void RemoveWallpaperFromCache( const TDesC& aFileName );
    void RemovableDiskInsertedL();
    void CheckFeatureTypeL();
    void AddPageSpecificWallpaperL( const TDesC& aFileName );
    void AddCommonWallpaperL( const TDesC& aFileName, TBool aSave = ETrue );
    void ReadWallpaperFromCenrepL();

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
     * States whether page specific wallpaper is supported or
     * same wallpaper is shown in all pages.
     */
    TBool iFeatureSuppoted;

    /** 
     * Internal wallpaper update in progress
     */
    TInt iIntUpdate;
        
    /** 
     * States whether transparent CBA and status pane is used. 
     * Homescreen uses transparency, but e.g. Widget catalog does not.
     */
    TBool iTransparencyEnabled;

    /** 
     * Pointer to wallpaper image. This is used only if same wallpaper 
     * is shared among all HS pages.
     * Own. 
     */   
    CFbsBitmap* iBgImage;
    
    /** 
     * Path of the wallpaper image, including filename. 
     * This is used only if same wallpaper 
     * is shared among all HS pages.
     * Own. 
     */  
    HBufC* iBgImagePath;

    };

#endif      // CXNBACKGROUNDMANAGER_H

// End of File
