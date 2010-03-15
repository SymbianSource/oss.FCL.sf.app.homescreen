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
* Description:  Wallpaper view.
*
*/

// System includes
#include <aknappui.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <AknsWallpaperUtils.h>
#include <MGFetch.h>
#include <aknnotewrappers.h>
#include <StringLoader.h> 
#include <caf/caf.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

// User includes
#include <xnwallpaperview.rsg>
#include "xnwallpaperview.h"
#include "xnwallpapercontainer.h"
#include "xnuiengine.h"
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xnbackgroundmanager.h"
#include "xneffectmanager.h"
#include "xnviewmanager.h"

// Constants
_LIT( KResourceDrive, "z:" );
_LIT( KResourceFile, "xnwallpaperview.rsc" );

_LIT8( KMulti, "multi" );

const TInt KFileArrayGranularity( 6 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperView::CXnWallpaperView( CXnUiEngine& aEngine ) 
    : iEngine( aEngine ), iAppUi( iEngine.AppUiAdapter() )    
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::ConstructL()
    {
    TFileName resFile;
    resFile.Append( KResourceDrive );
    resFile.Append( KDC_APP_RESOURCE_DIR );
    resFile.Append( KResourceFile );
    
    CCoeEnv* env( CCoeEnv::Static() );

    BaflUtils::NearestLanguageFile( env->FsSession(), resFile );

    iResourceOffset = env->AddResourceFileL( resFile );
    
    BaseConstructL( R_WALLPAPER_VIEW );
           
    iTimer = CPeriodic::NewL( CActive::EPriorityIdle );
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperView* CXnWallpaperView::NewL( CXnUiEngine& aEngine )
    {
    CXnWallpaperView* self = new (ELeave) CXnWallpaperView( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperView::~CXnWallpaperView()
    {    
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );    
    
    delete iContainer;    
    delete iTimer;
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::Id
// -----------------------------------------------------------------------------
//
TUid CXnWallpaperView::Id() const
    {
    return KWallpaperViewUid;
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoActivateL
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoActivateL( const TVwsViewId& aPrevViewId,           
    TUid /*aCustomMessageId*/, const TDesC8& aCustomMessage )            
    {
    // switch layout 
    CEikStatusPane* sp( iAppUi.StatusPane() );
    
    sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
    sp->ApplyCurrentSettingsL();
            
    // disable transparancy
    if ( sp->IsTransparent() )
        {
        sp->EnableTransparent( EFalse );
        }
    
    sp->DrawNow();
    
    // update cba
    CEikButtonGroupContainer* bgc( Cba() );
    CEikCba* cba = static_cast< CEikCba* >( bgc->ButtonGroup() );

    if ( cba ) 
        {       
        bgc->SetBoundingRect( TRect() );
        cba->DrawNow();
        }
    
    if ( !iContainer )
        {
        iContainer = CXnWallpaperContainer::NewL();
        iAppUi.AddToStackL( *this, iContainer );
        iContainer->ActivateL();
        iContainer->DrawNow();
        }
    
    iData.iAppUid = aPrevViewId.iAppUid;
    iData.iViewUid = aPrevViewId.iViewUid; 
    iData.iMultiple = EFalse;
        
    if ( aCustomMessage == KMulti )
        {
        iData.iMultiple = ETrue;
        }

    // Run image selection dialog asynchronously
    iTimer->Cancel();
    iTimer->Start( 0, 1000, TCallBack( TimerCallbackL, this ) );
    
    iAppUi.EffectManager()->UiRendered();
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoActivateL
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoDeactivate()
    {
    if ( iContainer )
        {
        iAppUi.RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    
    iAppUi.EffectManager()->UiRendered();
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::TimerCallback
// -----------------------------------------------------------------------------
//
TInt CXnWallpaperView::TimerCallbackL( TAny *aPtr )
    {       
    CXnWallpaperView* self = reinterpret_cast< CXnWallpaperView* >( aPtr );
    self->iTimer->Cancel();
    
    CDesCArrayFlat* files = 
        new (ELeave) CDesCArrayFlat( KFileArrayGranularity );    
    CleanupStack::PushL( files );

    TInt err( KErrNone );
    TBool selected( EFalse );
        
    TXnWallpaperViewData& data( self->iData );
    
    CXnBackgroundManager& bg( self->iAppUi.ViewAdapter().BgManager() );
    
    TRAPD( fetch, selected = MGFetch::RunL( *files, EImageFile, data.iMultiple ) );
    
    if ( fetch == KErrNone && selected && files->MdcaCount() > 0 )                 
        {
        // set wallpaper
        if( files->MdcaCount() == 1 )
            {                        
            err = bg.AddWallpaperL( files->MdcaPoint( 0 ) );
            }
        }
    
    CleanupStack::PopAndDestroy( files );

    if ( err == KErrCACorruptContent )
        {
        //load message text
        HBufC* msg = StringLoader::LoadLC( R_QTN_HS_CORRUPTED_IMAGE_NOTE );
        //ensure that dialog will not disappear immediatelly - by const. param
        CAknErrorNote* dialog = new (ELeave) CAknErrorNote( true );
        CleanupStack::PushL( dialog );
        //show dialog to user and destroy it
        dialog->ExecuteLD( *msg );
        CleanupStack::Pop( dialog );
        CleanupStack::PopAndDestroy( msg );
        }
    
    self->iAppUi.EffectManager()->BeginFullscreenEffectL( 
        KGfxContextCloseWallpaperView, self->iAppUi.ViewManager().ActiveViewData() );
    
    self->iAppUi.ActivateViewL( TVwsViewId( data.iAppUid, data.iViewUid ) );

    return KErrNone;
    }

//  End of File
