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
#include "xneditor.h"

// SYSTEM INCLUDE FILES
#include <aknappui.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <AknsWallpaperUtils.h>
#include <MGFetch.h>

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
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage )
    {
    iAvkonAppUi->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
    iAvkonAppUi->StatusPane()->DrawNow();
    if ( !iContainer )
        {
        iContainer = CXnWallpaperContainer::NewL();
        iAvkonAppUi->AddToStackL( *this,  iContainer );
        iContainer->ActivateL();
        iContainer->DrawNow();
        }
    
    CDesCArrayFlat* files = 
        new (ELeave) CDesCArrayFlat( KFileArrayGranularity );
    CleanupStack::PushL( files );
    TBool selected = EFalse;
    
    TBool multiple=EFalse;
    if( aCustomMessage == KMulti )
        {
        multiple = ETrue;
        }
    TRAPD( err, selected = MGFetch::RunL( *files, EImageFile, multiple ) );
    if ( err == KErrNone &&
         selected &&
         files->MdcaCount() > 0 )
        {
        // set wallpaper.
        if( files->MdcaCount() == 1 )
            {
            AknsWallpaperUtils::SetIdleWallpaper(
                    files->MdcaPoint( 0 ),
                    NULL );
            }
        //set slideshow
        else
            {
            AknsWallpaperUtils::SetSlidesetWallpaper(
                    *files,
                    NULL );
            }
        }
    
    CleanupStack::PopAndDestroy( files );
    
    iAvkonAppUi->ActivateViewL( aPrevViewId );
    
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

//  End of File
