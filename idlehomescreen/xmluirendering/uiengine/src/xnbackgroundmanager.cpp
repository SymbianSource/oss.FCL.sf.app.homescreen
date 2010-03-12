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
* Description:  Background manager.
*
*/


// INCLUDE FILES
#include "xnbackgroundmanager.h"
#include "hspswrapper.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xnplugindefs.h"
#include "hspssapi.h"
#include "xnappuiadapter.h"
#include "xnwallpaperview.h"
#include "xnrootdata.h"
#include "xnuiengine.h"

// SYSTEM INCLUDE FILES
#include <aknlistquerydialog.h> 
#include <xnuiengine.rsg>
#include <AknSkinsInternalCRKeys.h>
#include <activeidle2domaincrkeys.h>
#include <AknsWallpaperUtils.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>

#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <driveinfo.h>

using namespace hspswrapper;

_LIT8( KSingle, "single" );
const TUid KDummyUid = { 0x0000000 };
const TInt KSkinGfxInnerRectShrink = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnBackgroundManager::CXnBackgroundManager( CXnViewManager& aViewManager, CHspsWrapper& aWrapper )
    : iViewManager( aViewManager ), 
      iHspsWrapper( aWrapper )
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::ConstructL()
    {    
    CreateWindowL();

    iRect = TRect();
    iBgContext = CAknsLayeredBackgroundControlContext::NewL(
            KAknsIIDQsnBgScreenIdle, iRect, ETrue, 1 );

    TRect bgRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, bgRect );
    SetRect( bgRect );

    Window().SetOrdinalPosition( -1 );

    MakeVisible( ETrue );
    ActivateL();
    iIntUpdate = 0;
    User::LeaveIfError( iSkinSrv.Connect( this ) );
    iSkinSrv.EnableSkinChangeNotify();  
    
    // Start listening for drive events.
    User::LeaveIfError( iFsSession.Connect() );
    
    // Start listening file server notifications.
    iDiskNotifier = CDiskNotifyHandler::NewL( *this, iFsSession );
    User::LeaveIfError( iDiskNotifier->NotifyDisk() );
    
    // Reads from cenrep wheteher page specific wallpaper is enabled or not
    CheckFeatureTypeL();   

    iTimer = CPeriodic::NewL( CActive::EPriorityIdle );
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnBackgroundManager* CXnBackgroundManager::NewL( CXnViewManager& aViewManager, 
    CHspsWrapper& aWrapper )
    {
    CXnBackgroundManager* self = new (ELeave) CXnBackgroundManager( aViewManager, 
        aWrapper );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CXnBackgroundManager::~CXnBackgroundManager()
    {
    delete iTimer;
    CleanCache();
    iSkinSrv.Close();
    delete iDiskNotifier;
    iFsSession.Close();
    delete iBgContext;
    delete iBgImage;
    delete iBgImagePath;
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::Draw
// -----------------------------------------------------------------------------
//    
void CXnBackgroundManager::Draw(const TRect& aRect) const
    {
    if( iFeatureSuppoted )
        {
        CXnViewData& viewData( iViewManager.ActiveViewData() );
        CFbsBitmap* wallpaper = viewData.WallpaperImage();
        if( wallpaper )
            {   
            TSize bitmapSize = wallpaper->SizeInPixels();
            
            if( iRect.Height() > bitmapSize.iHeight && iRect.Width() > bitmapSize.iWidth )
                {
                TInt width = bitmapSize.iWidth / 2;
                TInt height = bitmapSize.iHeight / 2;
            
                TPoint point = iRect.Center();
                point.SetXY( point.iX - width, point.iY - height );
                
                SystemGc().SetBrushColor( KRgbBlack );
                SystemGc().Clear( aRect );
                SystemGc().DrawBitmap( TRect( point, bitmapSize), wallpaper );
                }
            else
                {
                SystemGc().DrawBitmap( iRect, wallpaper );
                }

            if( iViewManager.UiEngine().IsEditMode() )
                {
                DrawEditModeBackgroundSkin();
                }
            return;
            }
        }
    else if( iBgImage )
        {
        TSize bitmapSize = iBgImage->SizeInPixels();
        
        if( iRect.Height() > bitmapSize.iHeight && iRect.Width() > bitmapSize.iWidth )
            {
            TInt width = bitmapSize.iWidth / 2;
            TInt height = bitmapSize.iHeight / 2;
        
            TPoint point = iRect.Center();
            point.SetXY( point.iX - width, point.iY - height );
            
            SystemGc().SetBrushColor( KRgbBlack );
            SystemGc().Clear( aRect );
            SystemGc().DrawBitmap( TRect( point, bitmapSize ), iBgImage );

            }
        else
            {
            SystemGc().DrawBitmap( iRect, iBgImage );
            }
        if( iViewManager.UiEngine().IsEditMode() )
            {
            DrawEditModeBackgroundSkin();
            }
        return;
        }    
    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );     
    AknsDrawUtils::Background( skin, iBgContext, this, SystemGc(), aRect );
    
    if( iViewManager.UiEngine().IsEditMode() )
        {
        DrawEditModeBackgroundSkin();
        }   
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::SizeChanged
// -----------------------------------------------------------------------------
// 
void CXnBackgroundManager::SizeChanged()
    {
    iRect = Rect();
    if( iFeatureSuppoted )
        {
        TRAP_IGNORE( UpdateWallpapersL() );
        }
    else
        {
        if( iBgImagePath )
            {
            delete iBgImage;
            iBgImage = NULL;
            TRAP_IGNORE( iBgImage = iSkinSrv.WallpaperImageL( *iBgImagePath ) );
            }
        }
    iBgContext->SetRect( iRect );
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::MakeVisible
// -----------------------------------------------------------------------------
// 
void CXnBackgroundManager::MakeVisible( TBool aVisible )
    {    
    CCoeControl::MakeVisible( aVisible );
    if ( aVisible && iScreenUpdateNeeded )
        {
        iScreenUpdateNeeded = EFalse;
        DrawNow();
        }
    }


// -----------------------------------------------------------------------------
// Handle disk drive notifications.
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::HandleNotifyDisk(
        TInt /*aError*/,
        const TDiskEvent& aEvent )
    {
    if( aEvent.iType == MDiskNotifyHandlerCallback::EDiskStatusChanged )
        {
        if( !( aEvent.iInfo.iDriveAtt & KDriveAttInternal ) ) 
            {        
            TBool diskRemoved( aEvent.iInfo.iType == EMediaNotPresent );
            if( diskRemoved )
                {
                // TODO:
                //TRAP_IGNORE( RemovableDiskRemovedL() );
                }
            else
                {
                TRAP_IGNORE( RemovableDiskInsertedL() );        
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::CacheWallpaperL
// -----------------------------------------------------------------------------
//
TInt CXnBackgroundManager::CacheWallpaperL( const TDesC& aFileName, CXnViewData& aViewData )
    {
    if( aFileName == KNullDesC )
        {
        return KErrArgument;
        }

    aViewData.SetWallpaperImagePathL( aFileName );
    aViewData.SetWallpaperImage( NULL );

    TBool err( KErrNone );
    TRAP( err, iSkinSrv.AddWallpaperL( aFileName, iRect.Size() ) );
    if( err == KErrNone )
        {    
        CFbsBitmap* bitmap( NULL );
        CleanupStack::PushL( bitmap );
        TRAP( err, bitmap = iSkinSrv.WallpaperImageL( aFileName ) );
        if( err == KErrNone && bitmap )
            {
            aViewData.SetWallpaperImage( bitmap ); // Ownership tranferred
            }        
        else
            {
            iSkinSrv.RemoveWallpaper( aFileName );
            }
        CleanupStack::Pop();
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::AddWallpaperL
// ---------------------------------------------------------------------------
//
TInt CXnBackgroundManager::AddWallpaperL( const TDesC& aFileName )
    {
    TInt retVal = KErrNone;
    if( iFeatureSuppoted )
        {
        retVal = AddPageSpecificWallpaperL( aFileName );
        }
    else
        {
        retVal = AddCommonWallpaperL( aFileName );
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::DeleteWallpaper
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::DeleteWallpaper( CXnViewData& aViewData )
    {
    if( iFeatureSuppoted )
        {
        const TDesC& path = aViewData.WallpaperImagePath();
        if( path != KNullDesC )
            {
            RemoveWallpaperFromCache( path, &aViewData );
            TRAP_IGNORE( aViewData.SetWallpaperImagePathL( KNullDesC ) );
            aViewData.SetWallpaperImage( NULL );
            }
        }
    else
        {
        if( iBgImagePath )
            {
            iSkinSrv.RemoveWallpaper( *iBgImagePath );          
            delete iBgImagePath;
            iBgImagePath = NULL;
            }
        delete iBgImage;
        iBgImage = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// CXnBackgroundManager::ActivatedL
// ---------------------------------------------------------------------------
//
TBool CXnBackgroundManager::ActivatedL()
    {
    return iFeatureSuppoted;
    }
    
// -----------------------------------------------------------------------------
// CXnBackgroundManager::WallpaperChanged
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::WallpaperChanged( CXnViewData& aOldView, CXnViewData& aNewView )
    {
    if( iFeatureSuppoted && 
        aOldView.WallpaperImagePath().Compare( aNewView.WallpaperImagePath() ) )
        {
        UpdateScreen();

        // Since AknsWallpaperUtils::SetIdleWallpaper() call is slow, it is called
        // asynchronously. In that way we can avoid it slowing down page switching.
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        iTimer->Start( 0, 1000, TCallBack( TimerCallback, this ) );
        }
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::SaveWallpaperL
// ---------------------------------------------------------------------------
// 
void CXnBackgroundManager::SaveWallpaperL()
    {
    // Save wallpaper to HSPS
    if( iFeatureSuppoted )
        {
        TBuf8<KMaxFileName> wallpaper8;
        CXnViewData& viewData( iViewManager.ActiveViewData() );
        wallpaper8.Copy( viewData.WallpaperImagePath() );    
        SetSettingPropertyL( viewData.PluginId(), KWallpaper,
                KPath, wallpaper8 );
        }
    else
        {
        CRepository* repository = CRepository::NewLC( TUid::Uid( KCRUidActiveIdleLV ) );
        if ( repository )
            {
            if( iBgImagePath )
                {
                User::LeaveIfError( repository->Set( KAIWallpaperPath, 
                    *iBgImagePath ) );            
                }
            else
                {
                User::LeaveIfError( repository->Set( KAIWallpaperPath, 
                    KNullDesC ) );            
                }
            }
        CleanupStack::PopAndDestroy( repository );
        }
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::SetSettingPropertyL
// ---------------------------------------------------------------------------
//
TInt CXnBackgroundManager::SetSettingPropertyL( const TDesC8& aPluginId,
    const TDesC8& aItemId, const TDesC8& aPropertyName, 
    const TDesC8& aPropertyValue )
    {
    if ( aPluginId == KNullDesC8() ||
         aItemId == KNullDesC8() ||
         aPropertyName == KNullDesC8() )
        {
        return KErrArgument;
        }        
    
    CHspsConfiguration* pluginConf(
            iHspsWrapper.GetPluginConfigurationL( aPluginId ) );
    if ( !pluginConf )
        {
        return KErrNotFound;
        }    
    CleanupStack::PushL( pluginConf );
        
    // Find.
    CPropertyMap* property = NULL;
    RPointerArray<CItemMap>& settings = pluginConf->Settings();           
    for( TInt i = 0; i < settings.Count(); i++ )
        {
        CItemMap* setting = settings[i];
        if ( !setting )
            {
            continue;
            }

        if ( setting->ItemId() == aItemId )
            {
            RPointerArray<CPropertyMap>& properties = setting->Properties();
            for( TInt j = 0; j < properties.Count(); j++ )
                {
                CPropertyMap* tmpProperty = properties[j];
                if ( !tmpProperty )
                    {
                    continue;
                    }
                
                if ( tmpProperty->Name() == aPropertyName )
                    {
                    property = tmpProperty;     
                    break;
                    }
                }            
            break;
            }
        }

    TInt ret = KErrNone;
    
    // Set.    
    if ( property )
        {
        property->SetValueL( aPropertyValue );
        ret = iHspsWrapper.SetPluginSettingsL( aPluginId, settings );        
        }
    else
        {
        ret = KErrNotFound;
        }
    
    CleanupStack::PopAndDestroy( pluginConf );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::SetWallpaperL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SetWallpaperL()
    {
    TInt selectedIndex( 0 );

    CAknListQueryDialog* query =
        new ( ELeave ) CAknListQueryDialog( &selectedIndex );
    CleanupStack::PushL( query );
    query->PrepareLC( R_LISTQUERY_CHANGE_WALLPAPER );

    if ( query->RunLD() )
        {
        if ( selectedIndex == 0 )
            {
            AddWallpaperL( KNullDesC );
            }
        else if ( selectedIndex == 1 )
            {
            iViewManager.AppUiAdapter().ActivateLocalViewL( KWallpaperViewUid, 
                KDummyUid, KSingle );                
            }
        }
    CleanupStack::Pop( query );
    }
    
// ---------------------------------------------------------------------------
// CXnBackgroundManager::SkinContentChanged
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SkinContentChanged()
    {    
    }
    
// ---------------------------------------------------------------------------
// CXnBackgroundManager::SkinConfigurationChanged
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SkinConfigurationChanged(
    const TAknsSkinStatusConfigurationChangeReason aReason )
    {
    if ( aReason == EAknsSkinStatusWallpaperChanged )
        {        
        if( iIntUpdate > 0 )
            {
            iIntUpdate--;
            }
        else
            {
            TRAP_IGNORE( ReadWallpaperFromCenrepL() );
            }
        }
    else if ( aReason == EAknsSkinStatusConfigurationDeployed )
        {
        UpdateScreen();
        }
    }
    
// ---------------------------------------------------------------------------
// CXnBackgroundManager::SkinPackageChanged
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SkinPackageChanged(
        const TAknsSkinStatusPackageChangeReason /*aReason*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::CleanCache
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::CleanCache()
    {
    iSkinSrv.RemoveAllWallpapers();
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::RemoveWallpaper
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::RemoveWallpaperFromCache( const TDesC& aFileName,
    CXnViewData* aViewData )
    {
    CXnViewData* currentViewData( aViewData );
    if( !currentViewData )
        {
        currentViewData = &iViewManager.ActiveViewData();
        }
    CXnRootData& rootData = iViewManager.ActiveAppData();
    RPointerArray<CXnPluginData>& rootDataArr = rootData.PluginData();

    for( TInt i = 0; i < rootDataArr.Count(); i++ )
        {
        CXnViewData* viewData = static_cast<CXnViewData*>( rootDataArr[i] );
        if( currentViewData == viewData )
            {
            continue;
            }
        else if( viewData->WallpaperImagePath() == aFileName )
            {
            // Some other view has same wallpaper image. 
            // This must not be removed from the cache.
            return;
            }
        }
    // Image is not needed anymore. Can be removed from the cache.
    iSkinSrv.RemoveWallpaper( aFileName );  
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::RemoveWallpaper
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::RemoveWallpaperL( CXnViewData& aViewData )
    {
    aViewData.SetWallpaperImagePathL( KNullDesC );
    aViewData.SetWallpaperImage( NULL );
    SetSettingPropertyL( aViewData.PluginId(), KWallpaper, KPath, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::UpdateWallpapersL
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::UpdateWallpapersL()
    {
    CXnRootData& rootData = iViewManager.ActiveAppData();
    if( !&rootData )
        {
        return;
        }
    RPointerArray<CXnPluginData>& rootDataArr = rootData.PluginData();

    for( TInt i = 0; i < rootDataArr.Count(); i++ )
        {
        CXnViewData* viewData = static_cast<CXnViewData*>( rootDataArr[i] );
        const TDesC& path = viewData->WallpaperImagePath();
        if( path != KNullDesC )
            {
            CFbsBitmap* bitmap = iSkinSrv.WallpaperImageL( path );
            if( bitmap )
                {
                viewData->SetWallpaperImage( bitmap );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::RemovableDiskInsertedL
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::RemovableDiskInsertedL()
    {
    if( iFeatureSuppoted )
        {
        CXnRootData& rootData = iViewManager.ActiveAppData();
        if( !&rootData )
            {
            return;
            }
        RPointerArray<CXnPluginData>& rootDataArr = rootData.PluginData();
        TInt drawingNeeded( EFalse );
        for( TInt i = 0; i < rootDataArr.Count(); i++ )
            {
            CXnViewData* viewData = static_cast<CXnViewData*>( rootDataArr[i] );
            const TDesC& path = viewData->WallpaperImagePath();
            CFbsBitmap* bitmap = viewData->WallpaperImage();
            if( path != KNullDesC && !bitmap )
                {
                TInt err = CacheWallpaperL( path, *viewData );
                if( err == KErrNone )
                    {
                    drawingNeeded = ETrue;
                    }
                }
            }
        if( drawingNeeded )
            {
            UpdateScreen();
            }    
        }
    else
        {
        if( iBgImagePath )
            {
            AddCommonWallpaperL( *iBgImagePath, EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::CheckFeatureTypeL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::CheckFeatureTypeL()
    {
    iFeatureSuppoted = EFalse;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
    CleanupStack::PushL( repository );
    if ( repository )
        {
        // Get wallpaper handling type from cenrep
        TInt type;
        TInt err = repository->Get( KAIWallpaperChangeType, type );
        if ( err == KErrNone && type == 1)
            {
            iFeatureSuppoted = ETrue;
            }
        else
            {
            TFileName path;
            err = repository->Get( KAIWallpaperPath, path );
            if ( !err && path.Length())
                {
                AddCommonWallpaperL( path, EFalse );
                }
            }
        }
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::AddPageSpecificWallpaperL
// ---------------------------------------------------------------------------
//
TInt CXnBackgroundManager::AddPageSpecificWallpaperL( const TDesC& aFileName )
    {
    TInt err = KErrNone;
    CXnViewData& viewData( iViewManager.ActiveViewData() );
    const TDesC& old = viewData.WallpaperImagePath();

    // Remove old from the cache
    if( old != KNullDesC )
        {
        RemoveWallpaperFromCache( old );
        }

    // Add new to the cache
    if( aFileName != KNullDesC )
        {
        if( CacheWallpaperL( aFileName, viewData ) == KErrNone )
            {
            SaveWallpaperL(); // to HSPS
            }
        else
            {
            // image is corrupted or format is not supported
            return KErrCACorruptContent;
            }
        }
    // WallpaperImage changed back to default. Update view data.
    else
        {
        viewData.SetWallpaperImagePathL( KNullDesC );
        viewData.SetWallpaperImage( NULL ); 
        SaveWallpaperL(); // to HSPS
        }

    // Update screen
    UpdateScreen();
    
    err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, NULL );
    if( err == KErrNone )
        {
        iIntUpdate++;
        }
        
    return err;
    }
        
// ---------------------------------------------------------------------------
// CXnBackgroundManager::AddCommonWallpaperL
// ---------------------------------------------------------------------------
//
TInt CXnBackgroundManager::AddCommonWallpaperL( const TDesC& aFileName, 
    TBool aSave )
    {
    TInt err = KErrNone;      
    // Remove old from the cache
    if( iBgImagePath )
        {
        iSkinSrv.RemoveWallpaper( *iBgImagePath );          
        delete iBgImagePath;
        iBgImagePath = NULL;
        }
    delete iBgImage;
    iBgImage = NULL;

    if( aFileName != KNullDesC )
        {
        iBgImagePath = aFileName.AllocL();
    
        TBool err( KErrNone );
        TRAP( err, iSkinSrv.AddWallpaperL( aFileName, iRect.Size() ) );
        if( err )
            {
            // image is corrupted or format is not supported
            return KErrCACorruptContent;
            }
        TRAP( err, iBgImage = iSkinSrv.WallpaperImageL( aFileName ) );
        if( err )
            {
            iSkinSrv.RemoveWallpaper( aFileName );
            delete iBgImage;
            iBgImage = NULL;
            // image is corrupted or format is not supported
            return KErrCACorruptContent;
            }
        }
    
    // Update screen
    UpdateScreen();        

    // Save path to cenrep
    if( aSave )
        {
        SaveWallpaperL();
        }

    err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, NULL );
    if( err == KErrNone )
        {
        iIntUpdate++;
        }

    return err;
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::ReadWallpaperFromCenrepL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::ReadWallpaperFromCenrepL()
    {
    CRepository* repository = CRepository::NewLC( KCRUidPersonalisation );
    if ( repository )
        {                
        // Get wallpaper type from cenrep
        TInt wallpaperType;
        TInt err = repository->Get( KPslnWallpaperType, wallpaperType );
        if ( err == KErrNone )
            {
            if ( wallpaperType == 0 )
                {
                if( iFeatureSuppoted )
                    {
                    CXnViewData& viewData( iViewManager.ActiveViewData() );
                    RemoveWallpaperFromCache( viewData.WallpaperImagePath() );
                    RemoveWallpaperL( viewData );                        
                    } 
                else
                    {
                    if( iBgImagePath )
                        {
                        iSkinSrv.RemoveWallpaper( *iBgImagePath );
                        delete iBgImagePath;
                        iBgImagePath = NULL;
                        }
                    delete iBgImage;
                    iBgImage = NULL;
                    SaveWallpaperL();
                    }
                }
            else if ( wallpaperType == 1 )
                {
                // WallpaperImage is image
                // Get wallpaper image path from cenrep and save it
                TFileName wallpaper;            
                err = repository->Get( KPslnIdleBackgroundImagePath, wallpaper );
                if ( err == KErrNone )
                    {
                    if( iFeatureSuppoted )
                        {
                        AddPageSpecificWallpaperL( wallpaper );
                        }
                    else
                        {
                        AddCommonWallpaperL( wallpaper, EFalse );
                        }
                    }                
                }
            UpdateScreen();
            }

        CleanupStack::PopAndDestroy( repository );
        }    
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::UpdateScreen
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::UpdateScreen()
    {
    if( !IsVisible() )
        {
        iScreenUpdateNeeded = ETrue;
        }
    else
        {
        DrawNow();  
        }
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::DrawEditModeBackgroundSkin
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::DrawEditModeBackgroundSkin() const
    { 
    TRect shrunkRect = iRect;

    shrunkRect.Shrink(
        KSkinGfxInnerRectShrink,
        KSkinGfxInnerRectShrink );
    
    AknsDrawUtils::DrawFrame( AknsUtils::SkinInstance(), SystemGc(), 
            iRect, shrunkRect, KAknsIIDQgnHomeEditBg, KAknsIIDDefault );
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::TimerCallback
// -----------------------------------------------------------------------------
//
TInt CXnBackgroundManager::TimerCallback(TAny *aPtr)
    {
    CXnBackgroundManager* bgManager = reinterpret_cast<CXnBackgroundManager*>( aPtr );    
    bgManager->iTimer->Cancel();
    
    TInt err = AknsWallpaperUtils::SetIdleWallpaper( bgManager->
        iViewManager.ActiveViewData().WallpaperImagePath(), NULL );
    if( err == KErrNone )
        {
        bgManager->iIntUpdate++;
        }   
    return EFalse;
    }


//  End of File
