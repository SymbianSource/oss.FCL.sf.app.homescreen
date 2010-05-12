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
#include <eikapp.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <AknsWallpaperUtils.h>
#include <MGFetch.h>
#include <aknnotewrappers.h>
#include <StringLoader.h> 
#include <caf/caf.h>
#include <bautils.h>
#include <AknWaitDialog.h>
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
#include "xnspbgcleaner.h"

// Constants
_LIT( KResourceDrive, "z:" );
_LIT( KResourceFile, "xnwallpaperview.rsc" );

_LIT8( KMulti, "multi" );

const TInt KFileArrayGranularity( 6 );
const TInt KShortDelay = 1000;
const TInt KLongDelay = 1000 * 1000;

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
    iViewState = EIdle;
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
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL(); );
        }
    
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );    
    
    delete iContainer;
    delete iXnSpBgCleaner;
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
    
    // setup status pane layout
    sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
    // apply changes 
    sp->ApplyCurrentSettingsL();
    // disable transparancy
    sp->EnableTransparent( EFalse );
    
    // create background cleaner for sp
    if ( !iXnSpBgCleaner )
        {
        iXnSpBgCleaner = CXnSpBgCleaner::NewL();
        AppUi()->AddToStackL( *this, iXnSpBgCleaner );
        }

    // update sp
    iXnSpBgCleaner->DrawNow();
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
    
    iPreviousViewUid = aPrevViewId;
    iMultiple = ( (aCustomMessage == KMulti) ? ETrue : EFalse );

    iAppUi.EffectManager()->UiRendered();
    
    iWaitDialog = NULL;
    iViewState = EImageSelection;

    // Run image selection dialog asynchronously
    iTimer->Cancel();
    iTimer->Start( KShortDelay, KLongDelay, TCallBack( TimerCallback, this ) );
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoActivateL
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoDeactivate()
    {
    iTimer->Cancel(); // cancel timer
    
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL(); );
        }
    
    if ( iContainer )
        {
        iAppUi.RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    
    if ( iXnSpBgCleaner )
        {
        AppUi()->RemoveFromStack( iXnSpBgCleaner );
        delete iXnSpBgCleaner;
        iXnSpBgCleaner = NULL;
        }
    
    iViewState = EIdle;
    iAppUi.EffectManager()->UiRendered();
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::TimerCallback
// -----------------------------------------------------------------------------
//
TInt CXnWallpaperView::TimerCallback( TAny *aPtr )
    {       
    CXnWallpaperView* self = reinterpret_cast< CXnWallpaperView* >( aPtr );
    TRAPD( err, self->DoHandleCallBackL(); );    
    if ( KErrNone != err )
        {
        // activate default view in case of any error
        TVwsViewId defaultView;
        if ( self->iAppUi.GetDefaultViewId( defaultView ) != KErrNone )
            {
            // use default if we got wrong viewid as previous view
            defaultView.iAppUid = self->iAppUi.Application()->AppDllUid();
            defaultView.iViewUid = TUid::Uid( 1 );
            }        
        // try activating default view
        TRAP_IGNORE( self->iAppUi.ActivateViewL( defaultView ); );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoHandleCallBackL
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoHandleCallBackL()
    {
    iTimer->Cancel();
    
    if ( iViewState == EImageSelection )
        {
        CDesCArrayFlat* files = 
                new (ELeave) CDesCArrayFlat( KFileArrayGranularity );    
        CleanupStack::PushL( files );
    
        TInt err( KErrNone );
        TBool selected( EFalse );
        
        CXnBackgroundManager& bg( iAppUi.ViewAdapter().BgManager() );
        
        selected = MGFetch::RunL( *files, EImageFile, iMultiple );
        
        if ( selected && files->MdcaCount() > 0 )                 
            {
            // set wallpaper
            if( files->MdcaCount() == 1 )
                {
                TFileName fileName( files->MdcaPoint( 0 ) );
                RFs& fs = CEikonEnv::Static()->FsSession();
                if ( BaflUtils::FileExists( fs, fileName ) )
                    {
                    // if wallpaper adding will take more than 1,5 sec waitdlg will appear
                    iWaitDialog = new( ELeave ) CAknWaitDialog(
                            reinterpret_cast<CEikDialog**>( &iWaitDialog ), EFalse );
                    iWaitDialog->ExecuteLD( R_CHANGE_WALLPAPER_WAIT_DIALOG );
                    
                    // add wallpaper
                    err = bg.AddWallpaperL( fileName );
    
                    // notify waitdlg we're done.     
                    iWaitDialog->ProcessFinishedL();
                    // ProcessFinishedL() will NULL iWaitDialog
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( files );
        
        if( err == KErrTooBig || err == KErrNoMemory )
            {
            ShowErrorDialogL( R_QTN_HS_TOO_BIG_IMAGE_NOTE );
            }
        else if( err == KErrCancel || err == KErrCouldNotConnect )
            {
            // Ignore these.
            }
        else if( err != KErrNone )
            {
            ShowErrorDialogL( R_QTN_HS_CORRUPTED_IMAGE_NOTE );        
            }
        
        // restart timer to deactivate view. View activation fails if
        // any dialog was shown just before calling iAppUi.ActivateViewL
        // specially when theme effects are on.
        iViewState = EViewDeactivation;
        iTimer->Start( KLongDelay, KLongDelay, TCallBack( TimerCallback, this ) );
        }
    else if ( iViewState == EViewDeactivation )
        {        
        iAppUi.EffectManager()->BeginFullscreenEffectL( 
                                    KGfxContextCloseWallpaperView, 
                                    iAppUi.ViewManager().ActiveViewData() );
        iAppUi.ActivateViewL( iPreviousViewUid );
        }
    else
        {
        iViewState = EIdle;
        User::Leave( KErrUnknown );
        }
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::ShowErrorDialogL
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::ShowErrorDialogL( const TInt aResourceId )
    {
    //load message text
    HBufC* msg = StringLoader::LoadLC( aResourceId );
    //ensure that dialog will not disappear immediatelly - by const. param
    CAknErrorNote* dialog = new (ELeave) CAknErrorNote( true );
    CleanupStack::PushL( dialog );
    //show dialog to user and destroy it
    dialog->ExecuteLD( *msg );
    CleanupStack::Pop( dialog );
    CleanupStack::PopAndDestroy( msg );
    }


//  End of File
