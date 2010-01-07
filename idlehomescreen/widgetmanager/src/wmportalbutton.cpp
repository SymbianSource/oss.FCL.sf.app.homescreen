/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Description:
* Implementation of the portal button for WidgetManager
*
*/

// INCLUDE FILES
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknbutton.h>
#include <AknsDrawUtils.h>
#include <AknBidiTextUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <gulicon.h>
#include <avkon.mbg>
#include <widgetmanager.mbg>

#include "wmportalbutton.h"
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmmaincontainer.h"

// MEMBER FUNCTIONS

// ---------------------------------------------------------
// CWmPortalButton::NewL
// ---------------------------------------------------------
//
CWmPortalButton* CWmPortalButton::NewL( 
        const CCoeControl* aParent,
        const TDesC& aText,
        const TDesC& aUrl,
        TWmUiControlIds aButtonCtrlId )
	{
 	CWmPortalButton* self = new (ELeave) CWmPortalButton( 
                KAknButtonTextInsideFrame, aButtonCtrlId );
    CleanupStack::PushL( self );
    self->ConstructL( aParent,aText, aUrl );
    CleanupStack::Pop( self );
    return self;
	}

// ---------------------------------------------------------
// CWmPortalButton::~CWmPortalButton
// ---------------------------------------------------------
//
CWmPortalButton::~CWmPortalButton()
    {
    delete iText;
    delete iUrl;
    delete iImageConverter;
    }

// ---------------------------------------------------------
// CWmPortalButton::CWmPortalButton
// ---------------------------------------------------------
//
CWmPortalButton::CWmPortalButton( 
                        const TInt aFlags,
                        TWmUiControlIds aButtonCtrlId )
    :CAknButton( aFlags ),
    iButtonCtrlId( aButtonCtrlId )
    {
    }

// ---------------------------------------------------------
// CWmPortalButton::ConstructL
// ---------------------------------------------------------
//
void CWmPortalButton::ConstructL( 
        const CCoeControl* aParent,
        const TDesC& aText, const TDesC& aUrl )
    {
    if (  !aParent )
        {
        User::Leave( KErrArgument );
        }
    else if ( iButtonCtrlId != EOviPortal )
        {
        // operator button not supported until layout available.
        User::Leave( KErrNotSupported );
        }

    SetContainerWindowL( *aParent );
    
    // Obtain pointer to main container.
    iWmMainContainer = 
            static_cast <CWmMainContainer*>( 
                const_cast <CCoeControl*>( aParent ) );
    
    CGulIcon* icon = AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(), KAknsIIDQgnMenuOviStore,
            iWmMainContainer->WmPlugin().ResourceLoader().IconFilePath(),
            EMbmWidgetmanagerQgn_menu_ovistore,
            EMbmWidgetmanagerQgn_menu_ovistore_mask );
    
    CleanupStack::PushL( icon );
    CAknButton::ConstructL( icon, NULL, NULL, NULL,
                            aText, KNullDesC, 0 );
    CleanupStack::Pop( icon ); // ownership taken
    
    TAknsItemID frameId = ((iButtonCtrlId == EOviPortal) ? 
        KAknsIIDQgnHomeWmButton : KAknsIIDQsnFrButtonNormal);
    TAknsItemID frameCenterId = ((iButtonCtrlId == EOviPortal) ? 
        KAknsIIDQgnHomeWmButtonCenter : KAknsIIDQsnFrButtonCenterNormal );
    TAknsItemID framePressedId = ((iButtonCtrlId == EOviPortal) ? 
        KAknsIIDQgnHomeWmButtonPressed : KAknsIIDQsnFrButtonPressed );
    TAknsItemID framePressedCenterId = ((iButtonCtrlId == EOviPortal) ? 
        KAknsIIDQgnHomeWmButtonPressedCenter : KAknsIIDQsnFrButtonCenterPressed );

    iText = aText.AllocL();
    iUrl = aUrl.AllocL();

    SetFocusing( ETrue );
    SetBackgroundIds( frameId,
                      framePressedId,
                      KAknsIIDQsnFrButtonInactive,
                      framePressedId,
                      KAknsIIDQsnFrButtonInactive );
    SetFrameAndCenterIds( 
                        frameId,
						frameCenterId,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						KAknsIIDDefault,
						framePressedId,
						framePressedCenterId,
						KAknsIIDDefault,
						KAknsIIDDefault );
    
    // ready to be drawn
    ActivateL();
    }

// ---------------------------------------------------------
// CWmPortalButton::HandlePointerEventL
// ---------------------------------------------------------
//
void CWmPortalButton::HandlePointerEventL( 
        const TPointerEvent& aPointerEvent )
	{
	CAknButton::HandlePointerEventL( aPointerEvent);
	
	if ( AknLayoutUtils::PenEnabled() )
		{
        switch ( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
                {
                break;	
                }
            case TPointerEvent::EButton1Up:
                {
                if ( iWmMainContainer && 
                    iButtonCtrlId == EOviPortal )
                    {
                    iWmMainContainer->OpenOviPortalL();
                    }
                break;
                }
            default:
                break;
            }
       }	
	}
	
// ---------------------------------------------------------
// CWmPortalButton::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmPortalButton::OfferKeyEventL( 
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );    
    keyResponse = CAknButton::OfferKeyEventL( aKeyEvent, aType );
    
    if ( ( aType == EEventKey ) &&
       ( aKeyEvent.iScanCode == EStdKeyDevice3 ||
        aKeyEvent.iScanCode == EStdKeyEnter ) )
        {
        if ( iWmMainContainer && 
            iButtonCtrlId == EOviPortal )
            {
            iWmMainContainer->OpenOviPortalL();
            }

        keyResponse = EKeyWasConsumed;
        }

    return keyResponse;
    }

// ---------------------------------------------------------
// CWmPortalButton::SizeChanged
// ---------------------------------------------------------
//
void CWmPortalButton::SizeChanged()
    {    
    CAknButton::SizeChanged();

    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();
    TRect rect = Rect();
    TAknLayoutRect imageLayout;
    if ( iButtonCtrlId == EOviPortal )
        {
        imageLayout.LayoutRect( rect,
                AknLayoutScalable_Apps::wgtman_btn_pane_g1(
                                    landscape ? 1 : 0).LayoutLine() );
        }
    SetTextVerticalAlignment( CAknButton::ECenter );
    SetIconSize( imageLayout.Rect().Size() );
    SetTextAndIconAlignment( 
            landscape ? CAknButton::EIconOverText : CAknButton::EIconBeforeText );
    }

// ---------------------------------------------------------
// CWmPortalButton::NotifyCompletion
// ---------------------------------------------------------
//
void CWmPortalButton::NotifyCompletion( TInt aError )
    {
    if ( KErrNone != aError )
        {
        // no image available. Do nothing.
        }
    else
        {        
        CGulIcon* icon = NULL;
        TRAPD( err, icon = CGulIcon::NewL( 
                iImageConverter->Bitmap(), iImageConverter->Mask() ) );
        if ( KErrNone == err && icon )
            {
            // Ownership transfered
            State()->SetIcon( icon );
            DrawDeferred();            
            }
        }
    }

// ---------------------------------------------------------
// CWmPortalButton::Draw
// ---------------------------------------------------------
//
void CWmPortalButton::Draw( const TRect& /*aRect*/ ) const
    {
    TRect rect = Rect();

    TAknLayoutRect centerLayout;
    centerLayout.LayoutRect( rect,
        AknLayoutScalable_Avkon::toolbar_button_pane_g1().LayoutLine() );
    TRect innerRect = centerLayout.Rect();

    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

 	TAknsItemID frameId = ( ( iButtonCtrlId == EOviPortal) ? 
 	    KAknsIIDQgnHomeWmButton : KAknsIIDQsnFrButtonNormal );
    TAknsItemID frameCenterId = ( ( iButtonCtrlId == EOviPortal) ? 
        KAknsIIDQgnHomeWmButtonCenter : KAknsIIDQsnFrButtonCenterNormal );
 	
    if ( iButtonPressed )
    	{
        frameId = ( ( iButtonCtrlId == EOviPortal) ? 
            KAknsIIDQgnHomeWmButtonPressed : KAknsIIDQsnFrButtonPressed );
        frameCenterId = ( ( iButtonCtrlId == EOviPortal) ? 
            KAknsIIDQgnHomeWmButtonPressedCenter : KAknsIIDQsnFrButtonCenterPressed );
    	}
    else if ( IsDimmed() )
        {
    	frameId = KAknsIIDQsnFrButtonInactive;
    	frameCenterId = KAknsIIDQsnFrButtonCenterInactive;
        }
    else if ( IsFocused() )
        {
        frameId = KAknsIIDQsnFrList;
        frameCenterId = KAknsIIDQsnFrListCenter;
        }
    
    iBgContext->SetFrame( frameId );
    iBgContext->SetCenter( frameCenterId );
    iBgContext->SetFrameRects( rect, innerRect );

    if ( !AknsDrawUtils::Background( skin, iBgContext, NULL, 
                    gc, rect, KAknsDrawParamNoClearUnderImage ) )
        {
        gc.SetBrushColor( KRgbRed );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( rect );
        }
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	
    CAknButtonState* state = State();
    if ( state )
        {
        TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();
        
        const CGulIcon* icon = state->Icon();
        CFbsBitmap* bitmap = const_cast<CFbsBitmap*>(icon->Bitmap());
        CFbsBitmap* mask = const_cast<CFbsBitmap*>(icon->Mask());
        // draw image
        if ( state->Icon() && bitmap && mask )
            {
            TAknLayoutRect imageLayout;
            if ( iButtonCtrlId == EOviPortal )
                {
                imageLayout.LayoutRect( rect,
                        AknLayoutScalable_Apps::wgtman_btn_pane_g1(
                                landscape ? 1 : 0).LayoutLine() );
                }
            imageLayout.DrawImage( gc, bitmap, mask );
            }
        
        // draw text if portrait        
        if ( !landscape )
            {
            TAknTextComponentLayout leftLayout;
            if ( iButtonCtrlId == EOviPortal )
                {
                leftLayout = AknLayoutScalable_Apps::wgtman_btn_pane_t1( 
                                                    landscape ? 1 : 0  );
                }
            DrawText( gc, state->Text(), leftLayout, 1 );
            }
        }
    }

// ---------------------------------------------------------
// CWmPortalButton::DrawText
// ---------------------------------------------------------
//
void CWmPortalButton::DrawText( 
                        CWindowGc& aGc,
                        const TDesC& aText,
                        TAknTextComponentLayout& aLayout,
                        TInt aMargin ) const
    {
    TAknLayoutText layoutText;
    layoutText.LayoutText(Rect(), aLayout.LayoutLine() );
    
    const CFont* font = layoutText.Font();
    TRect textRect = layoutText.TextRect();
    
    aGc.UseFont( font );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    TRgb textColor( layoutText.Color() );

    if ( IsDimmed() )
        {
        const TInt KAlpha = 77;
        textColor = iEikonEnv->ControlColor( EColorButtonText, *this );        
        // try over-writing color from theme, ignore error.
        AknsUtils::GetCachedColor( 
                skin, textColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
        textColor.SetAlpha( KAlpha );
        }
    else if ( IsFocused() || iButtonPressed )
        {
        textColor = iEikonEnv->ControlColor( EColorButtonTextPressed, *this );
        // try over-writing color from theme, ignore error.
        AknsUtils::GetCachedColor( 
                skin, textColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 );
        }
    else if ( textColor.Internal() <= 0 )
        {
        textColor = iEikonEnv->ControlColor( EColorButtonText, *this );
        // try over-writing color from theme, ignore error.
        AknsUtils::GetCachedColor( 
                skin, textColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );        
        }
    aGc.SetPenColor( textColor);
    
    // Layout data not correct, add some space for text
    textRect.iBr.iX += aMargin;
    textRect.iTl.iX -= aMargin;
    
    // buffer for visually ordered text
    TBuf<255 + KAknBidiExtraSpacePerLine> visualText; 
    TInt clipWidth = textRect.Width();

    // bidi processing - using AknBidiTextUtils.
    AknBidiTextUtils::ConvertToVisualAndClip(
        aText,
        visualText,
        *font,
        clipWidth,
        clipWidth );

    TInt baselineOffset = 0;
    switch ( iVerticalAlignment )
        {
        case ETop:
            baselineOffset = font->AscentInPixels();
            break;

        case EBottom:
            baselineOffset = textRect.Height();
            break;

        default:  // centered
            baselineOffset = font->AscentInPixels() +
                           ( textRect.Height() - font->AscentInPixels() ) / 2;
        }
        
    aGc.DrawText( visualText, textRect, 
            baselineOffset, layoutText.Align() );    
    }

// End of file
