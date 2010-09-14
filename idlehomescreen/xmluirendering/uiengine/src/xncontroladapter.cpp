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
* Description:  Xuikon control adapter source file
*
*/


// System includes
#include <AknUtils.h>

// User includes
#include "xncontroladapter.h"
#include "xncontroladapterimpl.h"
#include "xnproperty.h"
#include "xnnodepluginif.h"
#include "xnnode.h"
#include "xnuiengine.h"
#include "xncomponent.h"
#include "xncomponentnodeimpl.h"
#include "xntype.h"

#include "xnviewnodeimpl.h"
#include "xneditmode.h"

// Constants
const TInt KLongTapStartDelay( 500000 ); // 0.5s
const TInt KLongTapTimeDelay( 1500000 ); // 1.5s

// -----------------------------------------------------------------------------
// CreateLongTapDetectorL
// Checks wheter longtap detector is needed for this node
// -----------------------------------------------------------------------------
//
static TBool CreateLongTapDetectorL( CXnNode& aNode )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( aNode.Type()->Type() == KMenuBarNode )
            {
            RPointerArray< CXnNode >& children( aNode.Children() );

            for ( TInt i = 0; i < children.Count(); i++ )
                {
                CXnProperty* prop( children[i]->GetPropertyL(
                    XnPropertyNames::common::KLongTap ) );

                if ( prop && prop->StringValue() == XnPropertyNames::KTrue )
                    {
                    return ETrue;
                    }
                }
            }
        else
            {
            CXnProperty* prop( aNode.GetPropertyL(
                XnPropertyNames::common::KLongTap ) );

            if ( prop && prop->StringValue() == XnPropertyNames::KTrue )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnControlAdapter::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnControlAdapter* CXnControlAdapter::NewL( CXnNodePluginIf& aNode )
    {
    CXnControlAdapter* self = new ( ELeave ) CXnControlAdapter;
    CleanupStack::PushL( self );
    self->ConstructL( aNode );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::~CXnControlAdapter
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnControlAdapter::~CXnControlAdapter()
    {
    delete iImpl;
    delete iLongTapDetector;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::CXnControlAdapter
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnControlAdapter::CXnControlAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::ConstructL
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::ConstructL( CXnNodePluginIf& aNode )
    {    
    iImpl = CXnControlAdapterImpl::NewL( aNode, *this, SystemGc() );

    if ( CreateLongTapDetectorL( aNode.Node() ) )
        {        
        iLongTapDetector = CAknLongTapDetector::NewL( this );
                       
        iLongTapDetector->SetTimeDelayBeforeAnimation( KLongTapStartDelay );
        iLongTapDetector->SetLongTapDelay( KLongTapTimeDelay );
        iCurrentLongTapStartDelay = KLongTapStartDelay;
        iCurrentLongTapTimeDelay = KLongTapTimeDelay;
        }    
    
    CCoeControl::MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetComponent
// Sets component object to adapter.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetComponent( CXnComponent* aComponent )
    {
    iImpl->SetComponent( aComponent );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::Component
// Returns component attached to this adapter
// -----------------------------------------------------------------------------
//
CXnComponent* CXnControlAdapter::Component() const
    {
    return const_cast< CXnComponent* >( iImpl->Component() );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::Component
// Gets component object from adapter.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponent* CXnControlAdapter::Component()
    {
    return iImpl->Component();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::OfferKeyEventL
// Handles key events.
// -----------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CXnControlAdapter::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return iImpl->OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::HandleControlEventL
// Handles control events.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::HandleControlEventL(
    CCoeControl* /*aControl*/,
    TCoeEvent /*aEventType*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::HandleLongTapEventL
// Handles the long tap events.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::HandleLongTapEventL(
    const TPoint& aPenEventLocation,
    const TPoint& aPenEventScreenLocation )
    {
    CXnNode* node( &Component()->Node()->Node() );

    CXnUiEngine* engine( node->UiEngine() );

    engine->DisableRenderUiLC();

    iImpl->HandleLongTapEventL( aPenEventLocation, aPenEventScreenLocation );

    CleanupStack::PopAndDestroy(); // DisableRenderUiLC;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::EnableLongTapAnimation
// Handles the long tap events.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::EnableLongTapAnimation( const TBool aAnimation )
    {
    if ( iLongTapDetector )
        {
        iLongTapDetector->EnableLongTapAnimation( aAnimation );
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::HandlePointerEventL
// Handle pointer events
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    TBool consumed( EFalse );
    
    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
        case TPointerEvent::EButton1Up:
        case TPointerEvent::EDrag:        
            consumed = iImpl->HandlePointerEventL( aPointerEvent );
            break;
        default:
            break;
        }

    if ( !consumed )
        {
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }    
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::Draw
// Draws the control
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::Draw( const TRect& aRect ) const
    {
    iImpl->Draw( aRect );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::Draw
// Draws the control
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::Draw( const TRect& aRect, CWindowGc& aGc ) const
    {
    iImpl->Draw( aRect, aGc );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::ComponentControl
// Return a child control by index
// -----------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CXnControlAdapter::ComponentControl( TInt aIndex ) const
    {
    return iImpl->ComponentControl( aIndex );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::CountComponentControls
// Return count of children
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXnControlAdapter::CountComponentControls() const
    {
    return iImpl->CountComponentControls();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetVisible
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetVisible( TBool aVisible )
    {
    MakeVisible( aVisible );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::AppendChildL
// -----------------------------------------------------------------------------
//
void CXnControlAdapter::AppendChildL(CXnControlAdapter& aChild, CXnNode& aNode)
    {
    iImpl->AppendChildL(aChild, aNode);
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::IsDrawingAllowed
// Checks whether drawing is allowed
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnControlAdapter::IsDrawingAllowed() const
    {
    return iImpl->IsDrawingAllowed();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::ContentBitmaps
// Gets content bitmaps
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::ContentBitmaps(
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask )
    {
    iImpl->ContentBitmaps( aBitmap, aMask );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetContentBitmaps
// Sets content bitmaps
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetContentBitmaps(
    CFbsBitmap* aBitmap,
    CFbsBitmap* aMask )
    {
    iImpl->SetContentBitmaps( aBitmap, aMask );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetContentBitmaps
// Sets content bitmaps
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetContentBitmaps(
    const TDesC& aBitmapUrl,
    const TDesC& aMaskUrl )
    {
    TRAP_IGNORE( iImpl->SetContentBitmapsL( aBitmapUrl, aMaskUrl ) );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::DrawContentImage
// Draws content image
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::DrawContentImage() const
    {
    iImpl->DrawContentImage();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SizeChanged
// Control size change notification
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SizeChanged()
    {
    iImpl->SizeChanged();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SkinChanged
// Skin change notification
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SkinChanged()
    {
    TRAP_IGNORE( Component()->Node()->SetDirtyL() );

    iImpl->SkinChanged();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::FocusChanged
// Focus change notification
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    TRAP_IGNORE( iImpl->FocusChangedL( IsFocused() ) );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::LoadBitmap
// Load a bitmap from the server. Ownership is transferred.
// -----------------------------------------------------------------------------
//
EXPORT_C CFbsBitmap* CXnControlAdapter::LoadBitmap( const TDesC& aBitmapUrl )
    {
    return iImpl->LoadBitmap( aBitmapUrl );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::GetBitmapAndMask
// Load a bitmap and mask from the server. Ownership is transferred.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::GetBitmapAndMask(
    const TDesC& aBitmapUrl,
    const TDesC& aMaskUrl,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask )
    {
    iImpl->GetBitmapAndMask( aBitmapUrl, aMaskUrl, aBitmap, aMask );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::EnterPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::EnterPowerSaveModeL( TModeEvent aEvent )
    {
    iImpl->StopHighlightAnimation();
    DoEnterPowerSaveModeL( aEvent );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::DoEnterPowerSaveModeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::DoEnterPowerSaveModeL( TModeEvent /* aEvent */ )
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::ExitPowerSaveModeL
// Exit power save mode
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::ExitPowerSaveModeL( TModeEvent aEvent )
    {
    iImpl->StartHighlightAnimation();
    DoExitPowerSaveModeL( aEvent );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::DoExitPowerSaveModeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::DoExitPowerSaveModeL( TModeEvent /*aEvent*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetLocalUiZoomL
// Set local UI zoom level
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetLocalUiZoomL( TAknUiZoom /*aZoom*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::HandleScreenDeviceChangedL()
    {
    CXnNode& node( Component()->Node()->Node() );

    if ( CreateLongTapDetectorL( node ) )
        {
        if ( !iLongTapDetector )
            {
            iLongTapDetector = CAknLongTapDetector::NewL( this );
            }
        }
    else
        {
        delete iLongTapDetector;
        iLongTapDetector = NULL;
        }

    Component()->Node()->SetDirtyL();

    iImpl->HandleScreenDeviceChangedL();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::HandlePropertyChangeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::HandlePropertyChangeL( CXnProperty* aProperty )
    {
    // Check component focus.
    CXnComponent* component( iImpl->Component() );

    if ( component )
        {
        CXnNodePluginIf* node( component->Node() );

        if ( node )
            {
            if ( node->IsFocusedState() )
                {
                iImpl->StartHighlightAnimation();
                }
            else
                {
                iImpl->StopHighlightAnimation();
                }
            }
        }

    if ( iImpl->UpdateBackgroundImageL( aProperty) ==
         CXnControlAdapterImpl::EPropertyChangeNotConsumed )
        {
        DoHandlePropertyChangeL( aProperty );
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {
    iImpl->DoHandlePropertyChangeL( aProperty );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::MeasureAdaptiveContentL
// Measures the content dimensions
// -----------------------------------------------------------------------------
//
EXPORT_C TSize CXnControlAdapter::MeasureAdaptiveContentL(
    const TSize& aAvailableSize )
    {
    return iImpl->MeasureAdaptiveContentL( aAvailableSize );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::ResetStylusCounter
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::ResetStylusCounter()
    {    
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetBlank
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetBlank( TBool aBlank )
    {
    iImpl->SetBlank( aBlank );
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::LongTapDetector
// -----------------------------------------------------------------------------
//
EXPORT_C CAknLongTapDetector* CXnControlAdapter::LongTapDetector() const
    {
    return iLongTapDetector;
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::RemoveChildAdapters
// -----------------------------------------------------------------------------
//
void CXnControlAdapter::RemoveChildAdapters()
    {
    iImpl->RemoveChildAdapters();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
RPointerArray<CXnControlAdapter>& CXnControlAdapter::ChildAdapters()
    {
    return iImpl->ChildAdapters();
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetLongTapDelay
// -----------------------------------------------------------------------------
//
void CXnControlAdapter::SetLongTapDelays( const TInt aStartDelay,
                                          const TInt aLongTapDelay )
    {
    if ( iCurrentLongTapTimeDelay != aLongTapDelay)
        {
        iLongTapDetector->SetLongTapDelay( aLongTapDelay );
        iCurrentLongTapTimeDelay = aLongTapDelay;
        }
    if ( iCurrentLongTapStartDelay != aStartDelay )
        {
        iLongTapDetector->SetTimeDelayBeforeAnimation( aStartDelay );
        iCurrentLongTapStartDelay = aStartDelay;
        }
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::SetDataL
// Empty default implemenatation for setting the data stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnControlAdapter::SetDataL( const TDesC8& /*aData*/, const TDesC& /*aType*/, TInt /*aIndex*/ )
    {
    
    }

// -----------------------------------------------------------------------------
// CXnControlAdapter::IsWidgetGestureDest
// -----------------------------------------------------------------------------
//
TBool CXnControlAdapter::IsWidgetGestureDest()
    {
    return iImpl->IsWidgetGestureDest();
    }

// End of file
