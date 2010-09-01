/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Native ui controller view.
*
*/


#include    <AknsDrawUtils.h>
#include    <AknsUtils.h>
#include    <AknUtils.h>
#include    "appui.h"
#include    "ainativeuiview.h"

using namespace AiNativeUiController;

CAiNativeUiView::CAiNativeUiView( CAppUi* aAppUi )
 :  iAppUi( aAppUi )
    {
    }

void CAiNativeUiView::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );
    ActivateL();
    }

CAiNativeUiView* CAiNativeUiView::NewL(
    const TRect& aRect,
     CAppUi* aAppUi )
    {
    CAiNativeUiView* self =
        new (ELeave) CAiNativeUiView( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop(self);
    return self;
    }

CAiNativeUiView::~CAiNativeUiView()
    {
    }

void CAiNativeUiView::SizeChanged()
    {
    if( iControl )
        {
        iControl->SetRect( Rect() );
        }
    }

TInt CAiNativeUiView::CountComponentControls() const
    {
    if( iControl )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

CCoeControl* CAiNativeUiView::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iControl;
    }

TKeyResponse CAiNativeUiView::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if( iControl )
        {
        return iControl->OfferKeyEventL( aKeyEvent, aType );
        }
    return EKeyWasNotConsumed;
    }

void CAiNativeUiView::Draw( const TRect& aRect ) const
    {
    TRect rect = Rect();
    if ( !rect.iTl.iY )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
        CWindowGc& gc = SystemGc();

        if ( !AknsDrawUtils::Background( skin, cc, this, gc, aRect ) )
            {
            gc.SetPenStyle( CGraphicsContext::ENullPen );
            gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.DrawRect( aRect );
            }
        }
    }

TVwsViewId CAiNativeUiView::ViewId() const
    {
    return TVwsViewId( TUid::Uid( 0x102750F0 ), TUid::Uid( 0x102750F0 ) );
    }

void CAiNativeUiView::ViewActivatedL( const TVwsViewId& /*aPrevViewId*/,
                             TUid /*aCustomMessageId*/,
                             const TDesC8& /*aCustomMessage*/ )
    {
    }

void CAiNativeUiView::ViewDeactivated()
    {
    }

void CAiNativeUiView::SetControl( CCoeControl* aControl )
    {
    iControl = aControl;
    }

//  End of File.
