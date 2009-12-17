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
*/

//  System includes
#include <e32base.h>
#include <e32const.h>
#include <coecntrl.h>
#include <coemain.h>
#include <aknphysics.h>

// User includes
#include "xncontroladapter.h"
#include "xnuiengine.h"
#include "xnnode.h"
#include "xnnodepluginif.h"
#include "xnproperty.h"

#include "xnscrollablecontroladapter.h"

// Constants
const TInt KOffset( 25 );

_LIT8(KViewportHeight, "viewport-height");
_LIT8(KViewportWidth, "viewport-width");
_LIT8(KViewportTop, "viewport-top");
_LIT8(KViewportLeft, "viewport-left");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CXnScrollableControlAdapter* CXnScrollableControlAdapter::NewL( 
    CXnNodePluginIf& aNode )
    {
    CXnScrollableControlAdapter* self = 
        new ( ELeave ) CXnScrollableControlAdapter( aNode );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::~CXnScrollableControlAdapter
// Destructor.
// -----------------------------------------------------------------------------
//
CXnScrollableControlAdapter::~CXnScrollableControlAdapter()
    {
    delete iPhysics;
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::CXnScrollableControlAdapter
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnScrollableControlAdapter::CXnScrollableControlAdapter( CXnNodePluginIf& aNode )
    : iNode( aNode ), iStartPosition( 0, 0), iPreviousPosition( 0, 0 ), 
    iCurrentPosition( 0, 0 ), iLayoutChanged( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::ConstructL
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::ConstructL()
    {
    CXnControlAdapter::ConstructL( iNode );
    SetControlContext( this );
    SetHitTest( this );
    iUiEngine = iNode.Node().UiEngine();
    
    iNode.Node().SetScrollableControl( this );
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::HandlePointerEventL
// 
// -----------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::HandlePointerEventL( 
    const TPointerEvent& aPointerEvent )
    {
    if( !iPhysics )
        {
        return;
        }
    TPoint stylusPos = aPointerEvent.iPosition;   
    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
            iPhysics->StopPhysics();
            iPhysics->ResetFriction();
            iStartPosition = stylusPos;
            iStartTime.HomeTime();
            iPreviousPosition = iCurrentPosition;
            iStylusPosition = stylusPos;
            }
            break;            
        case TPointerEvent::EButton1Up:
            {
            TInt distance = iStartPosition.iY - stylusPos.iY;
            TPoint drag( 0, distance );
            iPhysics->StartPhysics( drag, iStartTime );
            }
            break; 
        case TPointerEvent::EDrag:        
            {
            TPoint distanceFromStart( iStartPosition - stylusPos );

            if( Abs( distanceFromStart.iY ) > KOffset )                 
                {
                CXnNode* focused( iUiEngine->FocusedNode() );
                
                if ( focused )
                    {
                    // Remove pressed down
                    focused->UnsetStateL( 
                        XnPropertyNames::style::common::KPressedDown );
                    focused->HideTooltipsL();                                
                    }
                }
            
            TInt deltaY( iStylusPosition.iY - stylusPos.iY );
            iStylusPosition = stylusPos;
            TPoint deltaPoint( 0, deltaY );
            iPhysics->RegisterPanningPosition( deltaPoint );
            }
            break;
        default:                
            break;                
        }
 
    CXnControlAdapter::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::Draw
// 
// -----------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::Draw( const TRect& aRect ) const
    {
    CXnControlAdapter::Draw( aRect );    
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::SizeChanged
// 
// -----------------------------------------------------------------------------
void CXnScrollableControlAdapter::SizeChanged()
    {
    TRAP_IGNORE(         
        ReadPropertiesL();
        InitPhysicEngineL(); ); 
        
    CXnControlAdapter::SizeChanged();
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::MakeVisible
// 
// -----------------------------------------------------------------------------
// 
void CXnScrollableControlAdapter::MakeVisible( TBool aVisible )
    {
    TBool visible( IsVisible() ? ETrue : EFalse );
    
    if ( visible == aVisible )
        {
        return;
        }
    
    if( aVisible )
        {
        TRAP_IGNORE( InitPhysicEngineL() );
        }
    
    CCoeControl::MakeVisible( aVisible );            
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::HitRegionContains
// 
// -----------------------------------------------------------------------------
// 
TBool CXnScrollableControlAdapter::HitRegionContains( const TPoint& aPoint, 
    const CCoeControl& /*aControl*/) const
    {
    return iViewPort.Contains( aPoint );
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::HandleScreenDeviceChangedL
// 
// -----------------------------------------------------------------------------
void CXnScrollableControlAdapter::HandleScreenDeviceChangedL()
    {
    // The new layout has not been calculated yet. 
    // Therefore we need to read new propertues and initialise engine when LayoutChagedL() is called next time.
    iLayoutChanged = ETrue;
    CXnControlAdapter::HandleScreenDeviceChangedL();
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::DeltaPosition
// 
// -----------------------------------------------------------------------------
const TPoint CXnScrollableControlAdapter::DeltaPosition() const
    {
    return iStartViewPosition - iCurrentPosition;
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::ResetState
// 
// -----------------------------------------------------------------------------
void CXnScrollableControlAdapter::ResetState()
    {
    iStartViewPosition = TPoint( 0, 0 );
    iCurrentPosition = TPoint( 0, 0 );
    iPreviousPosition = TPoint( 0, 0 );
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::ShowItem
// 
// -----------------------------------------------------------------------------
void CXnScrollableControlAdapter::ShowItem( CXnNode& aNode )
    {
    TRect rect = aNode.MarginRect();
    if( !iViewPort.Contains( rect.iTl ) || !iViewPort.Contains( rect.iBr ) )
        {
        TInt delta( 0 );
        if( rect.iTl.iY < iViewPort.iTl.iY )
            {
            delta = rect.iTl.iY - iViewPort.iTl.iY;
            }
        else if( rect.iBr.iY > iViewPort.iBr.iY )
            {
            delta = rect.iBr.iY - iViewPort.iBr.iY;
            }
        TPoint newPosition = iPreviousPosition + TPoint( 0, delta );
        ViewPositionChanged( newPosition, ETrue, 0 );
        }
    }

// -----------------------------------------------------------------------------
// CXnScrollableControlAdapter::LayoutChangedL
// 
// -----------------------------------------------------------------------------
void CXnScrollableControlAdapter::LayoutChangedL()
    {
    if( iLayoutChanged )
        {
        iLayoutChanged = EFalse;
        ReadPropertiesL();
        InitPhysicEngineL();
        }
    }

// ---------------------------------------------------------------------------
// CXnScrollableControlAdapter::ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CXnScrollableControlAdapter::ViewPositionChanged( 
    const TPoint& aNewPosition, TBool aDrawNow, TUint /*aFlags*/ )
    {
    TPoint diff = TPoint( 0, aNewPosition.iY - iPreviousPosition.iY );
    iPreviousPosition = aNewPosition;
    iCurrentPosition += diff;
    TPoint tmpPos = iNode.Control()->Position();
    tmpPos -= diff;
    iNode.Control()->SetPosition( tmpPos );
    if( aDrawNow )
        {
        DrawNow( iViewPort );    
        }
    }
    
// ---------------------------------------------------------------------------
// CXnScrollableControlAdapter::PhysicEmulationEnded
// ---------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::PhysicEmulationEnded()
    {
    }

// ---------------------------------------------------------------------------
// CXnScrollableControlAdapter::ViewPosition
// ---------------------------------------------------------------------------
//    
TPoint CXnScrollableControlAdapter::ViewPosition() const
    {
    return iCurrentPosition;
    }

// ---------------------------------------------------------------------------
// CXnScrollableControlAdapter::ResetContext
// ---------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::ResetContext( CWindowGc& aGc ) const
    {
    aGc.Reset();
    aGc.SetClippingRect( iViewPort );
    }

// ---------------------------------------------------------------------------
// CXnScrollableControlAdapter::InitPhysicEngineL
// ---------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::InitPhysicEngineL()
    {
    // Init physic engine
    if ( !iPhysics && CAknPhysics::FeatureEnabled() )
        {
        iPhysics = CAknPhysics::NewL( *this, this );
        }
    if( !iPhysics )
        {
        return;
        }
    TSize viewPortSize = iViewPort.Size();
    TSize totalSize( iNode.MarginRect().Size() );
    iPhysics->InitPhysicsL( totalSize, viewPortSize, EFalse );
    iCurrentPosition = TPoint( 0, viewPortSize.iHeight / 2 );  
    iPreviousPosition = iStartViewPosition =  iCurrentPosition;
    }

// ---------------------------------------------------------------------------
// CXnScrollableControlAdapter::ReadPropertiesL
// ---------------------------------------------------------------------------
//    
void CXnScrollableControlAdapter::ReadPropertiesL()
    {
    TInt width( 0 );
    TInt height( 0 );
    TInt top( 0 );
    TInt left( 0 );

    // Width
    CXnProperty* prop = iNode.GetPropertyL( KViewportWidth );
    if( prop )
        {
        width = iUiEngine->HorizontalPixelValueL(
            prop, iNode.MarginRect().Width() );
        }
    if( width == 0 )
        {
        width = iNode.MarginRect().Width();
        }

    // Height
    prop = iNode.GetPropertyL( KViewportHeight );
    if( prop )
        {
        height = iUiEngine->VerticalPixelValueL(
            prop, iNode.MarginRect().Height() );
        }
    if( height == 0 )
        {
        height = iNode.MarginRect().Height();
        }

    // Top
    prop = iNode.GetPropertyL( KViewportTop );
    if( prop )
        {
        top = iUiEngine->VerticalPixelValueL(
            prop, iNode.MarginRect().Height() );
        }

    // Left
    prop = iNode.GetPropertyL( KViewportLeft );
    if( prop )
        {
        left = iUiEngine->HorizontalPixelValueL(
            prop, iNode.MarginRect().Width() );
        }

    iViewPort = TRect( iNode.MarginRect().iTl, TSize( width, height ) );
    iViewPort.Move( left, top );
    }

//  End of File  
