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

// SYSTEM INCLUDE FILES
#include <aknlistquerydialog.h> 
#include <xnuiengine.rsg>
#include <aknskinsinternalcrkeys.h>
#include <activeidle2domaincrkeys.h>
#include <aknswallpaperutils.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>

#include <aknsutils.h>
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <aknslayeredbackgroundcontrolcontext.h>
#include <driveinfo.h>

using namespace hspswrapper;

_LIT8( KSingle, "single" );
const TUid KDummyUid = { 0x0000000 };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnBackgroundManager::CXnBackgroundManager( CXnViewManager& aViewManager, CHspsWrapper& aWrapper )
    : iViewManager( aViewManager ), 
      iHspsWrapper( aWrapper ),
      iTransparencyEnabled( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::ConstructL()
    {
    // Register for view activation & deactivation
    iViewManager.AppUiAdapter().AddViewActivationObserverL( this );
    
    CreateWindowL();

    iBgContext = CAknsLayeredBackgroundControlContext::NewL(
            KAknsIIDQsnBgScreenIdle, TRect(), ETrue, 1 );

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
    if( iTransparencyEnabled )
        {
        if( iFeatureSuppoted )
            {
            CXnViewData& viewData( iViewManager.ActiveViewData() );
            CFbsBitmap* wallpaper = viewData.WallpaperImage();
            if( wallpaper )
                {
                SystemGc().BitBlt( TPoint(0, 0), wallpaper ); 
                return;
                }
            }
        else if( iBgImage )
            {
            SystemGc().BitBlt( TPoint(0, 0), iBgImage ); 
            return;
            }    
        }
    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );     
    AknsDrawUtils::Background( skin, iBgContext, this, 
        SystemGc(), aRect );
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
void CXnBackgroundManager::AddWallpaperL( const TDesC& aFileName )
    {
    if( iFeatureSuppoted )
        {
        AddPageSpecificWallpaperL( aFileName );
        }
    else
        {
        AddCommonWallpaperL( aFileName );
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
        DrawNow();
        TInt err = AknsWallpaperUtils::SetIdleWallpaper( aNewView.WallpaperImagePath(), NULL );
        if( err == KErrNone )
            {
            iIntUpdate++;
            }
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
        DrawNow();
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

// ----------------------------------------------------------------------------
// CXnBackgroundManager::HandleViewActivation()
// ----------------------------------------------------------------------------
//
void CXnBackgroundManager::HandleViewActivation( 
    const TVwsViewId& aNewlyActivatedViewId, 
    const TVwsViewId& aViewIdToBeDeactivated )
    {
    TVwsViewId viewId( iViewManager.AppUiAdapter().View().ViewId() );
    if ( viewId == aNewlyActivatedViewId && !iTransparencyEnabled )
        {
        iTransparencyEnabled = ETrue;
        DrawNow();
        }
    else if ( viewId == aViewIdToBeDeactivated && iTransparencyEnabled )
        {
        iTransparencyEnabled = EFalse;
        DrawNow();
        }
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
void CXnBackgroundManager::RemoveWallpaperFromCache( const TDesC& aFileName )
    {
    CXnViewData& activeViewData( iViewManager.ActiveViewData() );
    CXnRootData& rootData = iViewManager.ActiveAppData();
    RPointerArray<CXnPluginData>& rootDataArr = rootData.PluginData();

    for( TInt i = 0; i < rootDataArr.Count(); i++ )
        {
        CXnViewData* viewData = static_cast<CXnViewData*>( rootDataArr[i] );
        if( viewData == &activeViewData )
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
            DrawNow();
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
void CXnBackgroundManager::AddPageSpecificWallpaperL( const TDesC& aFileName )
    {
    CXnViewData& viewData( iViewManager.ActiveViewData() );
    const TDesC& old = viewData.WallpaperImagePath();

    if( aFileName.Compare( old ) )
        {
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
                return;
                }
            }
        
        // WallpaperImage changed back to default. Update view data.
        else
            {
            viewData.SetWallpaperImagePathL( KNullDesC );
            viewData.SetWallpaperImage( NULL );            
            }

        // Update screen
        DrawNow();        

        TInt err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, NULL );
        if( err == KErrNone )
            {
            iIntUpdate++;        
            }                
        }
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::AddCommonWallpaperL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::AddCommonWallpaperL( const TDesC& aFileName, 
    TBool aSave )
    {
    // Remove old from the cache
    if( iBgImagePath )
        {
        iSkinSrv.RemoveWallpaper( aFileName );          
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
            return;
            }
        TRAP( err, iBgImage = iSkinSrv.WallpaperImageL( aFileName ) );
        if( err )
            {
            iSkinSrv.RemoveWallpaper( aFileName );
            delete iBgImage;
            iBgImage = NULL;
            return;
            }
        }
    
    // Update screen
    DrawNow();        

    // Save path to cenrep
    if( aSave )
        {
        SaveWallpaperL();
        }

    TInt err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, NULL );
    if( !err )
        {
        iIntUpdate++;        
        }                
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
            DrawNow();
            }

        CleanupStack::PopAndDestroy( repository );
        }    
    }

//  End of File
