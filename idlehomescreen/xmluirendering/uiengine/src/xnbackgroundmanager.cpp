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
#include "xnoomsyshandler.h"
#include "xneffectmanager.h"

// SYSTEM INCLUDE FILES
#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>
#include <aknlistquerydialog.h> 
#include <xnuiengine.rsg>
#include <xnwallpaperview.rsg>
#include <AknSkinsInternalCRKeys.h>
#include <activeidle2domaincrkeys.h>
#include <AknsWallpaperUtils.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <bautils.h>
#include <utf.h>

#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <driveinfo.h>
#include <layoutmetadata.cdl.h>

using namespace hspswrapper;

// Constants
_LIT8( KSingle, "single" );
const TUid KDummyUid = { 0x0000000 };
const TInt KSkinGfxInnerRectShrink( 5 );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ShowInfoNoteL
// -----------------------------------------------------------------------------
//
void ShowInfoNoteL( TInt aResourceId )
    {
    HBufC* msg( StringLoader::LoadLC( aResourceId ) ); 

    CAknInformationNote* note = new ( ELeave ) CAknInformationNote;    
    note->ExecuteLD( *msg );
                   
    CleanupStack::PopAndDestroy( msg );                 
    }

// -----------------------------------------------------------------------------
// HandleErrorL
// -----------------------------------------------------------------------------
//
void HandleErrorL( TInt aErr )
    {
    TInt resourceId( NULL );
    if ( aErr == KErrTooBig || aErr == KErrNoMemory )
        {
        resourceId = R_QTN_HS_TOO_BIG_IMAGE_NOTE;
        }
    else if ( aErr == KErrCancel || aErr == KErrCouldNotConnect || 
        aErr == KErrCANoRights )
        {
        // Ignore these
        }
    else if ( aErr != KErrNone )
        {
        resourceId = R_QTN_HS_CORRUPTED_IMAGE_NOTE;
        }

    if( resourceId )
        {
        ShowInfoNoteL( resourceId );
        }
    }

// -----------------------------------------------------------------------------
// CreateBitmapFromColorL
// Creates a bitmap object with the given size and fill color
// -----------------------------------------------------------------------------
//
static CFbsBitmap* CreateBitmapFromColorL( TSize aSize, TRgb aColor )
    {
    CFbsBitmap* newBitmap = new ( ELeave ) CFbsBitmap;
    newBitmap->Create( aSize, EColor16M );
    CleanupStack::PushL( newBitmap );

    CFbsBitmapDevice* bitmapDev = CFbsBitmapDevice::NewL( newBitmap );
    CleanupStack::PushL( bitmapDev );

    CFbsBitGc* bc = NULL;
    User::LeaveIfError( bitmapDev->CreateContext( bc ) );
    CleanupStack::PushL( bc );

    bc->SetBrushColor( aColor );
    bc->Clear(); // area is filled with the brush color

    CleanupStack::PopAndDestroy( bc );
    CleanupStack::PopAndDestroy( bitmapDev );
    CleanupStack::Pop( newBitmap );
    return newBitmap;
    }
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnBackgroundManager::CXnBackgroundManager( CXnViewManager& aViewManager, CHspsWrapper& aWrapper )
    : iViewManager( aViewManager ), 
      iHspsWrapper( aWrapper ),
      iStoreWallpaper( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::ConstructL()
    {    
    // Connect to skin server.
    User::LeaveIfError( iSkinSrv.Connect( this ) );
          
    // Start listening for drive events.
    User::LeaveIfError( iFsSession.Connect() );

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
    
    // Start listening file server notifications.
    iDiskNotifier = CDiskNotifyHandler::NewL( *this, iFsSession );
    User::LeaveIfError( iDiskNotifier->NotifyDisk() );
    
    // Reads from cenrep wheteher page specific wallpaper is enabled or not
    CheckFeatureTypeL();   
    
    GfxTransEffect::Register( this, KGfxContextBgAppear );
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
    if ( GfxTransEffect::IsRegistered( this) )
        {
        GfxTransEffect::Deregister( this );
        }
        
    iSkinSrv.RemoveAllWallpapers();
    iSkinSrv.Close();
    delete iDiskNotifier;
    iFsSession.Close();
    delete iBgContext;
    delete iBgImage;
    delete iBgImagePath;    
    delete iSpBitmap;
    delete iSpMask;   
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::Draw
// -----------------------------------------------------------------------------
//    
void CXnBackgroundManager::Draw(const TRect& aRect) const
    {
    CFbsBitmap* wallpaper( NULL );
    
    if( iType == EPageSpecific )
        {
        CXnViewData& viewData( iViewManager.ActiveViewData() );
        wallpaper = viewData.WallpaperImage();
        }
    else if( iType == ECommon )
        {
        wallpaper = iBgImage;
        }

    // Draw bg image
    if( wallpaper )
        {   
        TSize bitmapSize = wallpaper->SizeInPixels();
        
        // If image is smaller that screen size it needs to be centralized
        if( iRect.Height() > bitmapSize.iHeight && 
            iRect.Width() > bitmapSize.iWidth )
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
        
        DrawStatusPaneMask();
        }
    
    // Skin bg is used by default
    else
        {  
        MAknsSkinInstance* skin( AknsUtils::SkinInstance() );     
        AknsDrawUtils::Background( skin, iBgContext, this, SystemGc(), aRect );
        }

    // Draw edit mode background highlight
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
    
    if ( iType == EPageSpecific )
        {
        TRAP_IGNORE( UpdateWallpapersL() );
        }
    else if ( iType == ECommon ) 
        {
        if( iBgImagePath )
            {
            delete iBgImage;
            iBgImage = NULL;
            TRAP_IGNORE( iBgImage = iSkinSrv.WallpaperImageL( *iBgImagePath ) );
            }
        }
    
    iBgContext->SetRect( iRect );
    
    TRAPD( err, UpdateStatuspaneMaskL() );
    if( err )
        {
        delete iSpBitmap;
        iSpBitmap = NULL;
        delete iSpMask;
        iSpMask = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::MakeVisible
// -----------------------------------------------------------------------------
// 
void CXnBackgroundManager::MakeVisible( TBool aVisible )
    {    
    CCoeControl::MakeVisible( aVisible );      
    
    if ( aVisible )
        {
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// Handle disk drive notifications.
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::HandleNotifyDisk( TInt /*aError*/, 
    const TDiskEvent& aEvent )              
    {
    if( aEvent.iType == MDiskNotifyHandlerCallback::EDiskStatusChanged || 
        aEvent.iType == MDiskNotifyHandlerCallback::EDiskAdded  ||  
        aEvent.iType == MDiskNotifyHandlerCallback::EDiskRemoved )
        {
        if( !( aEvent.iInfo.iDriveAtt & KDriveAttInternal ) ) 
            {        
            TBool diskRemoved( aEvent.iInfo.iType == EMediaNotPresent );
            
            if( diskRemoved )
                {
                TRAP_IGNORE( RemovableDiskRemovedL() );        
                }
            else
                {
                TRAP_IGNORE( RemovableDiskInsertedL() );        
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::ConstructWallpaper
// -----------------------------------------------------------------------------
//
TInt CXnBackgroundManager::ConstructWallpaper( const TDesC& aFileName, 
    CXnViewData& aViewData )
    {
    TRAP_IGNORE( aViewData.SetWallpaperImagePathL( aFileName ) );
    aViewData.SetWallpaperImage( NULL );

    TRAPD( err, iSkinSrv.AddWallpaperL( aFileName, iRect.Size() ) );
    if( err == KErrNone )
        {
        UpdateViewData( aFileName, aViewData );
        }  
    
    return err;
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
    query->PrepareLC( R_LISTQUERY_CHANGE_WALLPAPER );

    if ( !query->RunLD() )
        {
        // Query canceled
        return;
        }
           
    CXnAppUiAdapter& appui( iViewManager.AppUiAdapter() );
    
    if ( selectedIndex == 0 )
        {
        // Set wallpaper to default skin
        SetWallpaper( KNullDesC );
        
        appui.EffectManager()->BgAppearEffect( this, ETrue );        
        }
    else if ( selectedIndex == 1 )
        {
        if ( CXnOomSysHandler::HeapAvailable( CXnOomSysHandler::EMem6MB ) )
            {                                      
            appui.ActivateLocalViewL( KWallpaperViewUid, KDummyUid, KSingle );
            
            appui.EffectManager()->BeginFullscreenEffectL( KGfxContextOpenWallpaperView );            
            }
        else
            {
            // Potentially not enough memory
            iViewManager.OomSysHandler().HandlePotentialOomL();        
            }
        }        
    }
    
// ---------------------------------------------------------------------------
// CXnBackgroundManager::SetWallpaper
// ---------------------------------------------------------------------------
//
TBool CXnBackgroundManager::SetWallpaper( const TDesC& aFileName )
    {
    TInt err( KErrNone );
                         
    if ( iType == EPageSpecific )
        {
        TRAP( err, SetPageSpecificWallpaperL( aFileName ) );
        }
    else if ( iType == ECommon )
        {
        TRAP( err, SetCommonWallpaperL( aFileName ) );
        }
    
    if( err )
        {
        TRAP_IGNORE( HandleErrorL( err ) );
        return EFalse;
        }
        
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::DeleteWallpaper
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::DeleteWallpaper( CXnViewData& aViewData )
    {
    if( iType == EPageSpecific )
        {
        const TDesC& path = aViewData.WallpaperImagePath();
        if( path != KNullDesC )
            {
            RemoveWallpaperFromCache( path, &aViewData );
            TRAP_IGNORE( aViewData.SetWallpaperImagePathL( KNullDesC ) );
            aViewData.SetWallpaperImage( NULL );
            }
        }
    else if( iType == ECommon )
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

// -----------------------------------------------------------------------------
// CXnBackgroundManager::ChangeWallpaper
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::ChangeWallpaper( const CXnViewData& aOldView, 
    const CXnViewData& aNewView, TBool aDrawNow )
    {
    if( iType == EPageSpecific )
        {
        const TDesC& oldwp( aOldView.WallpaperImagePath() );
        const TDesC& newwp( aNewView.WallpaperImagePath() ); 
            
        if ( oldwp.Compare( newwp ) ) 
            {
            iStoreWallpaper = ETrue;                    
            
            if ( aDrawNow )
                {
                DrawNow();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::WallpaperType
// ---------------------------------------------------------------------------
//
CXnBackgroundManager::WppType CXnBackgroundManager::WallpaperType()
    {
    return iType;
    }
	
// ---------------------------------------------------------------------------
// CXnBackgroundManager::UpdateViewData
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::UpdateViewData( const TDesC& aFileName,
    CXnViewData& aViewData )
    {
    TRAP_IGNORE( aViewData.SetWallpaperImagePathL( aFileName ) );
    
    if( aFileName == KNullDesC )
        {
        aViewData.SetWallpaperImage( NULL );
        }
    else
        {
        CFbsBitmap* bitmap( NULL );
        TRAPD( err, bitmap = iSkinSrv.WallpaperImageL( aFileName ) );
        if( err == KErrNone && bitmap )
            {
            aViewData.SetWallpaperImage( bitmap ); // Ownership tranferred
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
    if( iType == EPageSpecific )
        {
        CXnViewData& viewData( iViewManager.ActiveViewData() );
        
        HBufC8* pUtf8String = CnvUtfConverter::ConvertFromUnicodeToUtf8L( viewData.WallpaperImagePath() );
        CleanupStack::PushL( pUtf8String );
        
        SetSettingPropertyL( viewData.PluginId(),
                            KWallpaper,
                            KPath,
                            *pUtf8String );
        
        CleanupStack::PopAndDestroy( pUtf8String );
        pUtf8String = NULL;
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
// CXnBackgroundManager::SkinContentChanged
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SkinContentChanged()
    {   
    TRAPD( err, UpdateStatuspaneMaskL() );
    if( err )
        {
        delete iSpBitmap;
        iSpBitmap = NULL;
        delete iSpMask;
        iSpMask = NULL;
        }
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

// -----------------------------------------------------------------------------
// CXnBackgroundManager::RemoveWallpaper
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::RemoveWallpaperFromCache( const TDesC& aFileName,
    CXnViewData* aViewData )
    {
    if( aFileName == KNullDesC )
        {
        return;
        }
    
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
// CXnBackgroundManager::RemovableDiskRemovedL
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::RemovableDiskRemovedL()
    {
    TInt drawingNeeded( EFalse );
    RFs& fs( CEikonEnv::Static()->FsSession() );

    if( iType == EPageSpecific )
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
            CFbsBitmap* bitmap = viewData->WallpaperImage();
            if( path != KNullDesC && bitmap )
                {
                if ( !BaflUtils::FileExists( fs, path ) )
                    {
                    RemoveWallpaperFromCache( path, viewData );
                    viewData->SetWallpaperImage( NULL );
                    if( viewData == &iViewManager.ActiveViewData() )
                        {
                        drawingNeeded = ETrue;
                        }
                    }
                }
            }
        }
    else
        {
        if( iBgImagePath && iBgImage )
            {
            if ( !BaflUtils::FileExists( fs, *iBgImagePath ) )
                {
                RemoveWallpaperFromCache( *iBgImagePath );
                delete iBgImage;
                iBgImage = NULL;
                drawingNeeded = ETrue;
                }
            }
        }

    if( drawingNeeded )
        {
        DrawNow();
        
        iIntUpdate++;
        TInt err = AknsWallpaperUtils::SetIdleWallpaper( KNullDesC , NULL );
        if( err )
            {
            iIntUpdate--;
            }  
        }    
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::RemovableDiskInsertedL
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::RemovableDiskInsertedL()
    {
    RFs& fs( CEikonEnv::Static()->FsSession() );

    if( iType == EPageSpecific )
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
                if ( BaflUtils::FileExists( fs, path ) )
                    {
                    TInt err = ConstructWallpaper( path, *viewData );
                    if( err == KErrNone && viewData == &iViewManager.ActiveViewData() )
                        {
                        drawingNeeded = ETrue;
                        }
                    }
                }
            }
        if( drawingNeeded )
            {
            DrawNow();
            iStoreWallpaper = ETrue;
            StoreWallpaperL();
            }    
        }
    else
        {
        if( iBgImagePath && !iBgImage )
            {
            if ( BaflUtils::FileExists( fs, *iBgImagePath ) )
                {
                TRAPD( err, SetCommonWallpaperL( *iBgImagePath, EFalse ) );
                if( err == KErrCANoRights )
                    {
                    ShowInfoNoteL( R_QTN_HS_DRM_PROTECTED_IMAGE_NOTE );
                    delete iBgImagePath;
                    iBgImagePath = NULL;
                    SaveWallpaperL();
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::CheckFeatureTypeL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::CheckFeatureTypeL()
    {
    iType = ECommon;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
    CleanupStack::PushL( repository );
    if ( repository )
        {
        // Get wallpaper handling type from cenrep
        TInt type;
        TInt err = repository->Get( KAIWallpaperChangeType, type );
        if ( err == KErrNone && type == 1)
            {
            iType = EPageSpecific;
            }
        else
            {
            iStoreWallpaper = EFalse;

            TFileName path;
            err = repository->Get( KAIWallpaperPath, path );
            if ( !err && path.Length())
                {
                TRAP_IGNORE( SetCommonWallpaperL( path, EFalse, EFalse ) );
                }
            }
        }
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// CXnBackgroundManager::SetPageSpecificWallpaperL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SetPageSpecificWallpaperL( const TDesC& aFileName )
    {
    CXnViewData& viewData( iViewManager.ActiveViewData() );

    TInt err( KErrNone );
    iIntUpdate++;

    if(  aFileName == KNullDesC )
        {
        err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, NULL );
        }
    else
        {
        // Wallpaper is also added into the cache if it is not there already.
        err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, CCoeEnv::Static(),
            R_QTN_HS_PROCESSING_NOTE, R_CHANGE_WALLPAPER_WAIT_DIALOG );    
        }
    if( err == KErrNone )
        {
        // Remove old wallpaper from the cache
        const TDesC& oldPath = viewData.WallpaperImagePath();
        RemoveWallpaperFromCache( oldPath );

        UpdateViewData( aFileName, viewData );

        SaveWallpaperL(); // to HSPS
        }
    else
        {
        iIntUpdate--;
        User::Leave( err );
        }
    }
        
// ---------------------------------------------------------------------------
// CXnBackgroundManager::SetCommonWallpaperL
// ---------------------------------------------------------------------------
//
void CXnBackgroundManager::SetCommonWallpaperL( const TDesC& aFileName, 
    TBool aSave, TBool aShowProgressBar )
    {
    TInt err( KErrNone );
    iIntUpdate++;

    if(  aFileName == KNullDesC || !aShowProgressBar )
        {
        err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, NULL );
        }
    else
        {
        // Wallpaper is also added into the cache if it is not there already.
        err = AknsWallpaperUtils::SetIdleWallpaper( aFileName, CCoeEnv::Static(),
            R_QTN_HS_PROCESSING_NOTE, R_CHANGE_WALLPAPER_WAIT_DIALOG );    
        }

    if( !err )
        {
        // Remove old from the cache
        if( iBgImagePath && iBgImagePath->Compare( aFileName ) )
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

            TRAPD( err, iBgImage = iSkinSrv.WallpaperImageL( aFileName ) );
            if( err )
                {
                delete iBgImage;
                iBgImage = NULL;
                delete iBgImagePath;
                iBgImagePath = NULL;
                User::Leave( err );
                }
            }    
        }
    else
        {
        iIntUpdate--;
        User::Leave( err );
        }
    
    // Update screen
    DrawNow();        

    // Save path to cenrep
    if( aSave )
        {
        SaveWallpaperL();
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
            TFileName wallpaper;            
            
            // WallpaperImage is image
            // Get wallpaper image path from cenrep
            if ( wallpaperType == 1 )
                {
                err = repository->Get( KPslnIdleBackgroundImagePath, wallpaper );
                if ( err != KErrNone )
                    {
                    return;
                    }
                }
                
            if( iType == EPageSpecific )
                {
                CXnViewData& viewData( iViewManager.ActiveViewData() );
                RemoveWallpaperFromCache( viewData.WallpaperImagePath() );
                viewData.SetWallpaperImagePathL( KNullDesC );
                viewData.SetWallpaperImage( NULL );
                if( wallpaperType == 1 )
                    {
                    UpdateViewData( wallpaper, viewData );
                    }
                } 
            else if( iType == ECommon )
                {
                if( iBgImagePath )
                    {
                    iSkinSrv.RemoveWallpaper( *iBgImagePath );
                    delete iBgImagePath;
                    iBgImagePath = NULL;
                    }
                delete iBgImage;
                iBgImage = NULL;
                if( wallpaperType == 1 )
                    {
                    iBgImagePath = wallpaper.AllocL();                
                    iBgImage = iSkinSrv.WallpaperImageL( wallpaper );
                    }
                }
            }

        SaveWallpaperL();
        DrawNow();
        }    
    CleanupStack::PopAndDestroy( repository );
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
// CXnBackgroundManager::DrawStatusPaneMask
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::DrawStatusPaneMask() const
    {
    if( iSpBitmap && iSpMask )
        {
        TSize bmpSize = iSpMask->SizeInPixels();
        TRect spRect( 0, 0, bmpSize.iWidth, bmpSize.iHeight );
        SystemGc().DrawBitmapMasked( spRect, iSpBitmap, spRect, iSpMask, ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::StoreWallpaperL
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::StoreWallpaperL()
    {
    if ( iStoreWallpaper )
        {   
        iStoreWallpaper = EFalse;

        CXnViewData& activeView = iViewManager.ActiveViewData();
        const TDesC& path( activeView.WallpaperImagePath() );
        RFs& fs( CEikonEnv::Static()->FsSession() );
        if ( path != KNullDesC && BaflUtils::FileExists( fs, path ) )
            {
            iIntUpdate++;
            TInt err( AknsWallpaperUtils::SetIdleWallpaper( path, NULL ) ); 
                  
            if( err )
                {
                iIntUpdate--;
                
                if ( err == KErrCANoRights )
                    {
                    ShowInfoNoteL( R_QTN_HS_DRM_PROTECTED_IMAGE_NOTE );

                    // Change wpp to default
                    UpdateViewData( KNullDesC, iViewManager.ActiveViewData() );
                    SaveWallpaperL();
                    DrawNow();
                    }            
                }        
            }
        else
            {
            iIntUpdate++;
            TInt err( AknsWallpaperUtils::SetIdleWallpaper( KNullDesC, NULL ) ); 
            if( err )
                {
                iIntUpdate--;
                }       
            }
        }       
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::UpdateStatuspaneMaskL
// -----------------------------------------------------------------------------
//
void CXnBackgroundManager::UpdateStatuspaneMaskL()
    {
    if ( iSpBitmap )
        {
        delete iSpBitmap;
        iSpBitmap = NULL;
        }
    if ( iSpMask )
        {
        delete iSpMask;
        iSpMask = NULL;
        }
    
    TRect spRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, spRect );
    
    MAknsSkinInstance* skinInstance( AknsUtils::SkinInstance() );

    // Get Homescreen status area mask color (EAknsCIQsnOtherColorsCG23)
    TRgb color;
    User::LeaveIfError( AknsUtils::GetCachedColor( skinInstance, color, 
        KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG23 ) );

    iSpBitmap = CreateBitmapFromColorL( spRect.Size(), color );
    
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        iSpMask = AknsUtils::CreateBitmapL( skinInstance,
            KAknsIIDQgnGrafBgLscTopMaskIcon );
        }
    else
        {
        iSpMask = AknsUtils::CreateBitmapL( skinInstance,
            KAknsIIDQgnGrafBgPrtTopMaskIcon );        
        }
    
    if ( iSpMask )
        {
        User::LeaveIfError( AknIconUtils::SetSize( 
            iSpMask, spRect.Size(), EAspectRatioNotPreserved ) );
        }
    }

//  End of File
