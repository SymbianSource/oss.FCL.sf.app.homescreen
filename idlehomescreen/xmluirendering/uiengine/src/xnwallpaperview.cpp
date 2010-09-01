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
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <MGFetch.h> 
#include <bautils.h>
#include <data_caging_path_literals.hrh>

// User includes
#include <xnwallpaperview.rsg>
#include "xnwallpaperview.h"
#include "xnwallpapercontainer.h"
#include "xnuiengine.h"
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xnviewmanager.h"
#include "xneffectmanager.h"
#include "xnspbgcleaner.h"

// Constants
_LIT( KResourceDrive, "z:" );
_LIT( KResourceFile, "xnwallpaperview.rsc" );

_LIT8( KMulti, "multi" );
_LIT8( KSetWallpaper, "setwallpaper" );

const TInt KFileArrayGranularity( 6 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
//
// -----------------------------------------------------------------------------
//
CXnWallpaperView::CXnWallpaperView( CXnUiEngine& aEngine ) 
    : iEngine( aEngine ), iAppUi( iEngine.AppUiAdapter() )    
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
//
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
           
    TCallBack cb( HandleCallback, this );
    
    iAsyncCb = new ( ELeave ) CAsyncCallBack( cb, CActive::EPriorityHigh );
    
    iFiles = new ( ELeave ) CDesCArrayFlat( KFileArrayGranularity );                
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
//
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
//
// -----------------------------------------------------------------------------
//
CXnWallpaperView::~CXnWallpaperView()
    {        
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );    
    
    if ( iFiles )
        {
        iFiles->Reset();
        }
    
    delete iFiles;    
    delete iContainer;
    delete iSpBgCleaner;
    delete iAsyncCb;
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::Id
//
// -----------------------------------------------------------------------------
//
TUid CXnWallpaperView::Id() const
    {
    return KWallpaperViewUid;
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoActivateL
//
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,           
    TUid /*aCustomMessageId*/, const TDesC8& aCustomMessage )            
    {
    // switch layout 
    CEikStatusPane* sp( iAppUi.StatusPane() );
    
    // setup status pane layout
    sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
    // apply changes 
    sp->ApplyCurrentSettingsL();
    // disable transparancy
    sp->EnableTransparent( EFalse );
    
    // create background cleaner for sp
    if ( !iSpBgCleaner )
        {
        iSpBgCleaner = CXnSpBgCleaner::NewL();
        AppUi()->AddToStackL( *this, iSpBgCleaner );
        }

    // update sp
    iSpBgCleaner->DrawNow();
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
      
    iAppUi.EffectManager()->EndFullscreenEffect( KGfxContextOpenWallpaperView );
    
    iMultiple = ( aCustomMessage == KMulti ) ? ETrue : EFalse;
           
    // Run image selection dialog asynchronously    
    iAsyncCb->CallBack();
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoDeactivate
//
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
    
    if ( iSpBgCleaner )
        {
        AppUi()->RemoveFromStack( iSpBgCleaner );
        delete iSpBgCleaner;
        iSpBgCleaner = NULL;
        }
    
    iAppUi.EffectManager()->EndFullscreenEffect( KGfxContextCloseWallpaperView );
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::SelectedWallpaper
//
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::SelectedWallpaper( TFileName& aFileName ) const
    {
    aFileName.Copy( iFileName );       
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::HandleCallback
//
// -----------------------------------------------------------------------------
//
TInt CXnWallpaperView::HandleCallback( TAny* aAny )
    {       
    CXnWallpaperView* self = static_cast< CXnWallpaperView* >( aAny );
          
    TRAP_IGNORE( self->DoHandleCallbackL() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoHandleCallbackL
//
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoHandleCallbackL()
    {    
    // Reset 
    iFileName = KNullDesC();    
    iFiles->Reset();
        
    TBool selected( EFalse );
                   
    TRAP_IGNORE( selected = 
        MGFetch::RunL( *iFiles, EImageFile, iMultiple ) ); 
            
    if ( selected && iFiles->MdcaCount() > 0 )                 
        {
        // Reset
        selected = EFalse;
                
        if( iFiles->MdcaCount() == 1 )
            {
            TPtrC fileName( iFiles->MdcaPoint( 0 ) );
                            
            RFs& fs( CEikonEnv::Static()->FsSession() );
            
            if ( BaflUtils::FileExists( fs, fileName ) )
                {
                // Accept the selected file
                selected = ETrue;
                iFileName.Copy( fileName );
                }
            }
        
        iFiles->Reset();
        }       
    
    // Default view
    TVwsViewId xmlViewId( iAppUi.View().ViewId() );
    
    const TDesC8& param( selected ? KSetWallpaper() : KNullDesC8() );
    
    iAppUi.EffectManager()->BeginFullscreenEffectL( KGfxContextCloseWallpaperView );
    
    iAppUi.ActivateLocalViewL( xmlViewId.iViewUid, TUid(), param );                         
    }

//  End of File
