/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for a box
*
*/

// System includes
#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xnnode.h"
#include "xndomnode.h"
#include "xnnodepluginif.h"
#include "xnproperty.h"
#include "xnviewadapter.h"
#include "xnbackgroundmanager.h"
#include "xnviewdata.h"
#include "xnviewmanager.h"
#include "xnviewcontroladapter.h"

// Constants
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnViewControlAdapter* CXnViewControlAdapter::NewL( CXnNodePluginIf& aNode )
    {
	CXnViewControlAdapter* self = new( ELeave ) CXnViewControlAdapter( aNode );

    CleanupStack::PushL( self );
    self->ConstructL( aNode );
    CleanupStack::Pop( self );

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::ConstructL( CXnNodePluginIf& aNode )
    {
    CreateWindowL();
    
    CXnControlAdapter::ConstructL( aNode );

    if( Window().SetTransparencyAlphaChannel() == KErrNone )      
        {       
        Window().SetBackgroundColor( ~0 );       
        }   
   
    Window().SetPointerGrab( EFalse );
    
    EnableDragEvents();
           
    ActivateL();
  
    SetComponentsToInheritVisibility( ETrue );
        
    iAppUi.UiStateListener().AddObserver( *this );
    }
    
// -----------------------------------------------------------------------------
// CXnViewControlAdapter::CXnViewControlAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnViewControlAdapter::CXnViewControlAdapter( CXnNodePluginIf& aNode ) 
    : iNode( aNode ), iAppUi( static_cast< CXnAppUiAdapter& >( *iAvkonAppUi ) ),
    iHitpoint( TPoint( -1,-1 ) )
    {    
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::~CXnViewControlAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnViewControlAdapter::~CXnViewControlAdapter()
    {
    iAppUi.UiStateListener().RemoveObserver( *this );      
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::MakeVisible
//
// -----------------------------------------------------------------------------
// 
void CXnViewControlAdapter::MakeVisible( TBool aVisible )
    {
    if ( aVisible == IsVisible() )
        {
        return;
        }
    
    if ( aVisible )
        {
        if ( !iAppUi.UiEngine().IsEditMode() )
            {
            Window().SetPointerGrab( ETrue );
            }
        }
    else
        {
        Window().SetPointerGrab( EFalse );
        
        ResetGrabbing();
        }
        
    CCoeControl::MakeVisible( aVisible );    
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::Draw
//
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::Draw( const TRect& aRect ) const
    {    
    SystemGc().Clear( aRect );        
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::HandlePointerEventL
//
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::HandlePointerEventL( 
    const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iHitpoint = aPointerEvent.iPosition;
        }
    
    iAppUi.UiEngine().DisableRenderUiLC();
            
    CXnControlAdapter::HandlePointerEventL( aPointerEvent );
    
    iAppUi.UiEngine().RenderUIL();
    
    CleanupStack::PopAndDestroy();    
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::ResetGrabbing()
// 
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::ResetGrabbing()
    {
    TPointerEvent event;
    event.iType = TPointerEvent::EButton1Up;
    
    TRAP_IGNORE( RemoveGrabbingControL( this, event ) );
    
    iHitpoint.SetXY( -1, -1 );
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::RemoveGrabbingControL()
// Removes recursively grabbing controls
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::RemoveGrabbingControL( const CCoeControl* aControl,
    const TPointerEvent& aEvent ) const
    {
    TInt count( aControl->CountComponentControls() );
    
    for( TInt i = 0; i < count; i++ )
        {
        CCoeControl* child( aControl->ComponentControl( i ) );
        
        if( child && child->Rect().Contains( iHitpoint ) )
            {
            child->CCoeControl::HandlePointerEventL( aEvent ); 
            RemoveGrabbingControL( child, aEvent );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::NotifyForegroundChanged()
// 
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::NotifyForegroundChanged( TForegroundStatus aStatus )
    {
    if ( aStatus == EBackground || aStatus == EPartialForeground )
        {
        ResetGrabbing();
        }
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::NotifyLightStatusChanged()
//  
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::NotifyLightStatusChanged( TBool /*aLightsOn*/ )
    {    
    }

// -----------------------------------------------------------------------------
// CXnViewControlAdapter::NotifyInCallStateChaged()
// 
// -----------------------------------------------------------------------------
//
void CXnViewControlAdapter::NotifyInCallStateChaged( TBool /*aInCall*/ )
    {    
    }

// End of file
