/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Carousel widget for Symbian Homescreen
 *
 */

#include <ecom/implementationproxy.h>
#include <aknphysics.h>
#include <gulicon.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <xnexteventhandler.h>
#include "carouselwidget.h"

const TSize KIconSize(70,70);
const TInt KItemPadding( 6 );

_LIT( KLaunchByIndex, "LaunchByIndex(");

template< class PtrT > inline PtrT* UnpackPtr( 
    const TDesC8& aBuf )
    {
    TAny* result( NULL );
    
    if ( aBuf.Size() == sizeof( TAny* ) )
        {
        // Effectively writes aBuf contents to result
        TPckg< TAny* >( result ).Copy( aBuf ); 
        }
        
    return static_cast< PtrT* >( result );
    }
// ============================ MEMBER FUNCTIONS ===============================
CCarouselItem::CCarouselItem( CFbsBitmap* aBitmap, CFbsBitmap* aMask)
    :iBitmap( aBitmap ), iMask( aMask )
    {
    }

CCarouselItem::~CCarouselItem()
    {
    delete iBitmap;
    delete iMask;
    delete iText;
    }

void CCarouselItem::SetTextL( const TDesC& aText )
    {
    delete iText;
    iText = NULL;
    iText = aText.AllocL();    
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CCarouselWidget* CCarouselWidget::NewL()
    {
    CCarouselWidget* self = new( ELeave ) CCarouselWidget();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// C++ destructor
// -----------------------------------------------------------------------------
//
CCarouselWidget::~CCarouselWidget()
    {
    delete iPhysics;
    iStripeItems.ResetAndDestroy();
    //delete iCover;
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCarouselWidget::ConstructL()
    {
    
    }

// -----------------------------------------------------------------------------
// Handles key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CCarouselWidget::OfferKeyEventL( const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/ )
    {
    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// Set window for this control
// -----------------------------------------------------------------------------
//
void CCarouselWidget::SetContainerWindowL( const CCoeControl &aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCarouselWidget::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if( !iPhysics )
        {
        return;
        }
    TPoint stylusPos( aPointerEvent.iPosition );   
    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
            iPhysics->StopPhysics();
            iPhysics->ResetFriction();
            iStartPosition = stylusPos;
            iStartTime.HomeTime();
            iStylusPosition = stylusPos;
            iDrawHighlight = ETrue;
            }
            break; 
            
        case TPointerEvent::EButton1Up:
            {
            iDrawHighlight = EFalse;
            TInt distance = iStartPosition.iX - stylusPos.iX;
            if( Abs(distance) <= iPhysics->DragThreshold() )
                {
                LaunchItemL( stylusPos );
                }
            else
                {
                TPoint drag( distance, 0 );
                iPhysics->StartPhysics( drag, iStartTime );
                }            }
            break; 
        case TPointerEvent::EDrag:
        case TPointerEvent::EMove:
            {
            TPoint dragTh(  iStartPosition - stylusPos );
            if( Abs(dragTh.iX) > iPhysics->DragThreshold() || 
                Abs(dragTh.iY) > iPhysics->DragThreshold())
                {
                iDrawHighlight = EFalse;
                }
            TInt deltaX( iStylusPosition.iX - stylusPos.iX );
            iStylusPosition = stylusPos;
            TPoint deltaPoint( deltaX, 0 );
            iPhysics->RegisterPanningPosition( deltaPoint );
            }
            break;
        default:                
            break;                
        }

    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// Returns the number of component controls
// -----------------------------------------------------------------------------
//
TInt CCarouselWidget::CountComponentControls() const
    {
    return 0;
    }    

// -----------------------------------------------------------------------------
// Returns the specified control
// -----------------------------------------------------------------------------
//
CCoeControl* CCarouselWidget::ComponentControl( TInt /*aIndex*/ ) const
    {
      return NULL;
    }

// -----------------------------------------------------------------------------
// Skin change notification.
// -----------------------------------------------------------------------------
//
void CCarouselWidget::SkinChanged()
    {
    }

// -----------------------------------------------------------------------------
// CCarouselWidget::EnterPowerSaveModeL
//
// -----------------------------------------------------------------------------
//
void CCarouselWidget::EnterPowerSaveModeL()
    {
    }

// -----------------------------------------------------------------------------
// CCarouselWidget::ExitPowerSaveModeL
//
// -----------------------------------------------------------------------------
//
void CCarouselWidget::ExitPowerSaveModeL()
    {
    }

// -----------------------------------------------------------------------------
// Called if focus changes
// -----------------------------------------------------------------------------
//
void CCarouselWidget::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    }

// -----------------------------------------------------------------------------
// Called if position or size changes
// -----------------------------------------------------------------------------
//
void CCarouselWidget::SizeChanged()
    {
    iViewPort = Rect();
    TRAP_IGNORE( InitPhysicEngineL() );
    DrawNow();
    }

// -----------------------------------------------------------------------------
// Sets the external event handler interface.
// -----------------------------------------------------------------------------
//
void CCarouselWidget::SetEventHandler( MXnExtEventHandler* aEventHandler )
    {
    iEventHandler = aEventHandler;
    }

// -----------------------------------------------------------------------------
// Data stream from the publisher
// -----------------------------------------------------------------------------
//
void CCarouselWidget::SetDataL( const TDesC8& aData, const TDesC& aType, TInt aIndex )
    {
    /// Unpack the data stream, works because publisher is in same process.
    CGulIcon* icon( UnpackPtr<CGulIcon>( aData ) );
    CleanupStack::PushL( icon );
    AknIconUtils::SetSize( icon->Bitmap(), KIconSize);
    
    CCarouselItem* item = new ( ELeave ) CCarouselItem(/* *iEditor,*/ icon->Bitmap(), icon->Mask() );
    CleanupStack::PopAndDestroy( icon );
    
    if( aType == _L("Appstripe/widget"))
        {
        if( iStripeItems.Count() <= aIndex )
            {
            iStripeItems.AppendL( item );
            InitPhysicEngineL();
            }
        else
            {
            CCarouselItem* citem = iStripeItems[ aIndex ];
            delete citem;
            iStripeItems.Remove( aIndex );
            iStripeItems.Insert( item, aIndex );
            }
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// Draws the carousel component
// -----------------------------------------------------------------------------
//
void CCarouselWidget::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc( SystemGc() );
    gc.SetClippingRect( iViewPort );    
    TRect bitmapRect( TPoint(0,0), KIconSize );
    TPoint point( iViewPort.iTl );
    point.iX -= iViewPosition.iX;
    
    TInt count = iStripeItems.Count();
    for( TInt i = 0; i < count; i++ )
        {
        CFbsBitmap* bitmap = iStripeItems[i]->iBitmap;
        CFbsBitmap* mask = iStripeItems[i]->iMask;

        TRect drawRect( point, KIconSize );
        if( iDrawHighlight && drawRect.Contains( iStylusPosition ))
            {
            TRect innerRect( drawRect );
            innerRect.Shrink( 5, 5 );
            
            MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
            AknsDrawUtils::DrawFrame( skin, gc, drawRect, innerRect,
                KAknsIIDQsnFrHomePressed, KAknsIIDDefault );
            }
        if( iViewPort.Contains( drawRect.iBr )||
            iViewPort.Contains( drawRect.iTl ))
            {
            gc.BitBltMasked( point, bitmap, bitmapRect, mask, EFalse);
            }
        point.iX += KIconSize.iWidth + KItemPadding;
        }
    }

// ---------------------------------------------------------------------------
// ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CCarouselWidget::ViewPositionChanged( 
    const TPoint& aNewPosition, TBool aDrawNow, TUint /*aFlags*/ )
    {
    iCurrentPosition = aNewPosition;
    iViewPosition.iX = aNewPosition.iX - iViewPort.Width()/2;
    if( aDrawNow )
        {
        DrawNow( iViewPort );
        }
    }

// ---------------------------------------------------------------------------
// PhysicEmulationEnded
// ---------------------------------------------------------------------------
//    
void CCarouselWidget::PhysicEmulationEnded()
    {
    }

// ---------------------------------------------------------------------------
// ViewPosition
// ---------------------------------------------------------------------------
//    
TPoint CCarouselWidget::ViewPosition() const
    {
    return iCurrentPosition;
    }

// ---------------------------------------------------------------------------
// InitPhysicEngineL
// ---------------------------------------------------------------------------
//    
void CCarouselWidget::InitPhysicEngineL()
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
    TInt totalWidth( iStripeItems.Count() * (KIconSize.iWidth + KItemPadding ));
    TSize totalSize( totalWidth, viewPortSize.iHeight );
    iPhysics->InitPhysicsL( totalSize, viewPortSize, ETrue );
    iCurrentPosition = TPoint( viewPortSize.iWidth / 2, viewPortSize.iHeight / 2 );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCarouselWidget::ItemIndex( TPoint& aPoint )
    {
    TPoint itemStartPoint( iViewPort.iTl );
    itemStartPoint.iX -= iViewPosition.iX;
    
    TRect dropRect( itemStartPoint, KIconSize );
    dropRect.Resize( KItemPadding, 0 );
    TInt index( 0 );
    do
        {
        if( dropRect.Contains( aPoint ))
            {
            return index;
            }
        dropRect.Move( KIconSize.iWidth+KItemPadding, 0);
        index++;
        }
    while( index < iStripeItems.Count() );
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// Launches the item, which is in aPosition
// -----------------------------------------------------------------------------
//
void CCarouselWidget::LaunchItemL( TPoint& aPosition )
    {
    TInt index = ItemIndex( aPosition );
    // carousel plugin expects indexes to start from 1
    index++;
    TInt len( KLaunchByIndex().Length()+3 );
    TBuf<17> event;
    event.Append( KLaunchByIndex );
    event.AppendNum( index );
    event.Append( _L(")"));
    iEventHandler->HandleEventL( event, _L8("Appstripe"));
    }

// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CCarouselWidget::CCarouselWidget()
    {
    }

// ============================ PUBLIC FUNCTIONS ===============================
const TImplementationProxy KImplementationTable[] =
    {
#ifdef __EABI__
    IMPLEMENTATION_PROXY_ENTRY( 0xEabba433, CCarouselWidget::NewL )
#else
    { { 0xEabba433 }, CCarouselWidget::NewL }
#endif
    };

// -----------------------------------------------------------------------------
// Returns the list of implementations provided by the plugin
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
    return KImplementationTable;
    }

