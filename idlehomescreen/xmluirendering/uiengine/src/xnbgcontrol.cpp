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

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xneditmode.h"

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
    :   iHitpoint( TPoint( -1,-1 ) )
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
    
    static_cast< CXnAppUiAdapter* >( iAvkonAppUi )
            ->UiStateListener().AddObserver( *this );
    
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
    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            iHitpoint = aPointerEvent.iPosition;
            break;
            
        case TPointerEvent::EButton1Up:
            break;
            
        default:
            break;
        }
    
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

// -----------------------------------------------------------------------------
// CXnBgControl::ResetGrabbingL()
// Service for removing grabbing controls
// -----------------------------------------------------------------------------
//
void CXnBgControl::ResetGrabbingL()
    {
    TPointerEvent event;
    event.iType = TPointerEvent::EButton1Up;
    
    RemoveGrabbingControL( this, event );
    
    iHitpoint.SetXY( -1, -1 );
    }

// -----------------------------------------------------------------------------
// CXnBgControl::PrepareDestroy()
// Prepares control for destroying
// -----------------------------------------------------------------------------
//
void CXnBgControl::PrepareDestroy()
    {
    static_cast< CXnAppUiAdapter* >( iAvkonAppUi )
            ->UiStateListener().RemoveObserver( *this );
    }

// -----------------------------------------------------------------------------
// CXnBgControl::RemoveGrabbingControL()
// Removes recursively grabbing controls
// -----------------------------------------------------------------------------
//
void CXnBgControl::RemoveGrabbingControL( const CCoeControl* aControl,
        const TPointerEvent& aEvent ) const
    {
    TInt count = aControl->CountComponentControls();
    
    for( TInt i = 0; i < count; i++ )
        {
        CCoeControl* child = aControl->ComponentControl( i );
        
        if( child && child->Rect().Contains( iHitpoint ) )
            {
            child->CCoeControl::HandlePointerEventL( aEvent ); 
            RemoveGrabbingControL( child, aEvent );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnBgControl::NotifyForegroundChanged()
// Notifies foreground changes.
// -----------------------------------------------------------------------------
//
void CXnBgControl::NotifyForegroundChanged( TForegroundStatus aStatus )
    {
    switch( aStatus )
        {
        case EForeground:
            break;
            
        case EUnknown:
        case EBackground:
        case EPartialForeground:  
        default:
            TRAP_IGNORE( ResetGrabbingL(); )
            break;
        }
    }

// -----------------------------------------------------------------------------
// CXnBgControl::NotifyLightStatusChanged()
// Notifies primary display light status is changed.
// -----------------------------------------------------------------------------
//
void CXnBgControl::NotifyLightStatusChanged( TBool /*aLightsOn*/ )
    {
    
    }

// -----------------------------------------------------------------------------
// CXnBgControl::NotifyInCallStateChaged()
// Notifies in-call state is changed.
// -----------------------------------------------------------------------------
//
void CXnBgControl::NotifyInCallStateChaged( TBool /*aInCall*/ )
    {
    
    }

// End of file

