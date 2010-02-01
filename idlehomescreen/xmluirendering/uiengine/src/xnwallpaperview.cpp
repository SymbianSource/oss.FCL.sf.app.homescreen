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


// INCLUDE FILES
#include "xnwallpaperview.h"
#include "xnwallpapercontainer.h"
#include "xnuiengine.h"
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xnbackgroundmanager.h"
#include <xnuiengine.rsg>

// SYSTEM INCLUDE FILES
#include <aknappui.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <AknsWallpaperUtils.h>
#include <MGFetch.h>
#include <aknnotewrappers.h>
#include <StringLoader.h> 
#include <caf/caf.h>

_LIT8( KMulti, "multi" );
const TInt KFileArrayGranularity = 6;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnWallpaperView::CXnWallpaperView( CXnUiEngine& aEngine ) :
    iEngine( aEngine )
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::ConstructL()
    {
    BaseConstructL();
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
    if ( iContainer )
        {
        delete iContainer;
        iContainer = NULL;
        }
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
void CXnWallpaperView::DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid /*aCustomMessageId*/,
            const TDesC8& aCustomMessage )
    {
    iAvkonAppUi->StatusPane()->SwitchLayoutL(
            R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
    iAvkonAppUi->StatusPane()->DrawNow();
    if ( !iContainer )
        {
        iContainer = CXnWallpaperContainer::NewL();
        iAvkonAppUi->AddToStackL( *this,  iContainer );
        iContainer->ActivateL();
        iContainer->DrawNow();
        }
    
    iData.iAppUid = aPrevViewId.iAppUid;
    iData.iViewUid = aPrevViewId.iViewUid; 
    iData.iMultiple = EFalse;
    iData.iTimer = iTimer;
    
    if( aCustomMessage == KMulti )
        {
        iData.iMultiple = ETrue;
        }

    // Run image selection dialog asynchronously
    if ( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }
    iTimer->Start( 0, 1000, TCallBack( TimerCallbackL, &iData ) );
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::DoActivateL
// -----------------------------------------------------------------------------
//
void CXnWallpaperView::DoDeactivate()
    {
    if ( iContainer )
        {
        iAvkonAppUi->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CXnWallpaperView::TimerCallback
// -----------------------------------------------------------------------------
//
TInt CXnWallpaperView::TimerCallbackL(TAny *aPtr)
    {
    TInt errAddWallpaper = KErrNone;
    
    TXnWallpaperViewData* data = reinterpret_cast<TXnWallpaperViewData*>( aPtr );
    data->iTimer->Cancel();
    
    CDesCArrayFlat* files = 
        new (ELeave) CDesCArrayFlat( KFileArrayGranularity );
    CleanupStack::PushL( files );
    TBool selected = EFalse;

    TRAPD( err, selected = MGFetch::RunL( *files, EImageFile, data->iMultiple ) );
    if ( err == KErrNone &&
         selected &&
         files->MdcaCount() > 0 )
        {
        // set wallpaper.
        if( files->MdcaCount() == 1 )
            {
            CXnAppUiAdapter* appui = static_cast< CXnAppUiAdapter* >( iAvkonAppUi );
            CXnBackgroundManager& bgManager = appui->ViewAdapter().BgManager();
            errAddWallpaper = bgManager.AddWallpaperL( files->MdcaPoint( 0 ) );
            }
        }
    CleanupStack::PopAndDestroy( files );

    if( errAddWallpaper == KErrCACorruptContent )
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
    
    iAvkonAppUi->ActivateViewL( TVwsViewId( data->iAppUid, data->iViewUid ) );

    return EFalse;
    }


//  End of File
