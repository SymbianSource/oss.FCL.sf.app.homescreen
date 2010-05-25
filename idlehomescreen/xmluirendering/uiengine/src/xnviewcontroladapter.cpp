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
* Description:  View control adapter
*
*/

// System includes

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xnnodepluginif.h"
#include "xnnode.h"

#include "xndomdocument.h"
#include "xndomnode.h"
#include "xnproperty.h"

#include "xnviewcontroladapter.h"

// Constants
    
// ============================ LOCAL FUNCTIONS ================================
// -----------------------------------------------------------------------------
// ResetGrabbingL
// Removes recursively grabbing controls
// -----------------------------------------------------------------------------
//
static void ResetGrabbingL( CXnControlAdapter* aControl, 
    const TPointerEvent& aEvent ) 
    {       
    if ( aControl )
        {                              
        CCoeControl* grabber( aControl->GrabbingComponent() );
        
        if ( grabber )
            {                        
            grabber->IgnoreEventsUntilNextPointerUp();
            
            aControl->CCoeControl::HandlePointerEventL( aEvent );
            
            CXnControlAdapter* adapter = 
                dynamic_cast< CXnControlAdapter* >( grabber );
            
            ResetGrabbingL( adapter, aEvent );            
            }        
        }    
    }

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
    
    // By default all views are invisible during construction phase    
    CXnDomStringPool* sp( aNode.Node().DomNode()->StringPool() );
    
    CXnProperty* prop = CXnProperty::NewL(
        XnPropertyNames::style::common::KDisplay, 
        XnPropertyNames::style::common::display::KNone,
        CXnDomPropertyValue::EString, *sp );
    CleanupStack::PushL( prop );    
    
    aNode.SetPropertyWithoutNotificationL( prop );
    CleanupStack::Pop( prop );    
    }
    
// -----------------------------------------------------------------------------
// CXnViewControlAdapter::CXnViewControlAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnViewControlAdapter::CXnViewControlAdapter( CXnNodePluginIf& aNode ) 
    : iNode( aNode ), iAppUi( static_cast< CXnAppUiAdapter& >( *iAvkonAppUi ) )    
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
    
    event.iModifiers = 0;
    event.iPosition = TPoint();
    event.iParentPosition = TPoint();
    event.iType = TPointerEvent::EButton1Up;
    
    TRAP_IGNORE( ResetGrabbingL( this, event ) );        
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
