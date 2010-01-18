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
* Description:  Background appearance drawer
*
*/

// System includes
#include <aknappui.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <gdi.h>

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xneditmode.h"
#include "xnviewadapter.h"
#include "xnfocuscontrol.h"

#include "xnbgcontrol.h"

// Constants

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnBgControl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnBgControl* CXnBgControl::NewL()
    {
    CXnBgControl* self = CXnBgControl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnBgControl::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnBgControl* CXnBgControl::NewLC()
    {
    CXnBgControl* self = new ( ELeave ) CXnBgControl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnBgControl::~CXnBgControl()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnBgControl::~CXnBgControl()
    {  
    }

// -----------------------------------------------------------------------------
// CXnBgControl::CXnBgControl()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnBgControl::CXnBgControl()
    {  
    }

// -----------------------------------------------------------------------------
// CXnBgControl::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnBgControl::ConstructL()
    {   
    CreateWindowL();

    TRgb backgroundColour = KRgbWhite;
    if( KErrNone == Window().SetTransparencyAlphaChannel() )      
        {       
        backgroundColour.SetAlpha( 0 );       
        }   
    Window().SetBackgroundColor( backgroundColour );
    
    EnableDragEvents();
    
    Window().SetPointerGrab( ETrue );
    
    
    ActivateL();

    MakeVisible( ETrue );
    
    SetComponentsToInheritVisibility( ETrue );
    }

// -----------------------------------------------------------------------------
// CXnBgControl::CountComponentControls()
// 
// -----------------------------------------------------------------------------
//
TInt CXnBgControl::CountComponentControls() const
    {
    if ( iControl )
        {
        return 1;
        }
    
    return 0;
    }

// -----------------------------------------------------------------------------
// CXnBgControl::ComponentControl()
// 
// -----------------------------------------------------------------------------
//
CCoeControl* CXnBgControl::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iControl;
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnBgControl::SizeChanged()
// 
// -----------------------------------------------------------------------------
//
void CXnBgControl::SizeChanged()
    {
    }

// -----------------------------------------------------------------------------
// CXnBgControl::Draw()
// 
// -----------------------------------------------------------------------------
//
void CXnBgControl::Draw( const TRect& aRect ) const
    {    
    CXnAppUiAdapter* appui( 
        static_cast< CXnAppUiAdapter* >( iAvkonAppUi ) );

    TInt state( appui->UiEngine().EditMode()->EditState() );
    
    CWindowGc& gc( SystemGc() );
    
    if ( state == CXnEditMode::EShootContent )
        {
        // No background needed for dragging widget screenshot        
        }
    else
        {        
        gc.Clear( aRect );        
        }          
    }

// -----------------------------------------------------------------------------
// CXnBgControl::SetCompoundControl()
// 
// -----------------------------------------------------------------------------
//
void CXnBgControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    CXnAppUiAdapter* appui( static_cast< CXnAppUiAdapter* >( iAvkonAppUi ) );
    
    appui->UiEngine().DisableRenderUiLC();
            
    CCoeControl::HandlePointerEventL( aPointerEvent );
    
    appui->UiEngine().RenderUIL();
    
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnBgControl::SetCompoundControl()
// 
// -----------------------------------------------------------------------------
//
void CXnBgControl::SetCompoundControl( CCoeControl* aControl )
    {
    if ( iControl == aControl )
        {
        return;
        }
    
    if ( iControl )
        {
        // Remove parents    
        iControl->SetParent( NULL );
        iControl->SetMopParent( NULL );
        
        iControl->MakeVisible( EFalse );
        }
    
    if ( aControl )
        {        
        // Set self as parent
        aControl->SetParent( this );
        aControl->SetMopParent( this );
        
        aControl->MakeVisible( ETrue );        
        }
    
    iControl = aControl;        
    }

// End of file

