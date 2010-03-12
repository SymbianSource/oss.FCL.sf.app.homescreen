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
#include <apgcli.h>
#include <apgtask.h>
#include <widgetregistryclient.h> // widgetreqistry
#include <bacline.h>
#include <escapeutils.h> 

#include "wmportalbutton.h"
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmmaincontainer.h"
#include "wmconfiguration.h"
#include "wmprocessmonitor.h"

// CONSTANTS
_LIT( KUrlPrefix, "4 ");


const TUid KOSSBrowserUidValue = { 0x10008D39 };
// MEMBER FUNCTIONS

// ---------------------------------------------------------
// CWmPortalButton::NewL
// ---------------------------------------------------------
//
CWmPortalButton* CWmPortalButton::NewL( 
        const CCoeControl* aParent,
        TInt aPortalButtonIndex )
	{
 	CWmPortalButton* self = new (ELeave) CWmPortalButton( 
                KAknButtonTextInsideFrame, aPortalButtonIndex );

 	CleanupStack::PushL( self );
    
    CWmMainContainer* mainContainer =
        static_cast <CWmMainContainer*>( 
            const_cast <CCoeControl*>( aParent ) );

    self->ConstructL( mainContainer,
            mainContainer->Configuration().PortalButtonText( aPortalButtonIndex ),
            mainContainer->Configuration().PortalButtonIcon( aPortalButtonIndex ) );
    
    CleanupStack::Pop( self );
    return self;
	}

// ---------------------------------------------------------
// CWmPortalButton::~CWmPortalButton
// ---------------------------------------------------------
//
CWmPortalButton::~CWmPortalButton()
    {    
    delete iButtonIcon;
    delete iButtonIconMask;
    
    // if MAknIconFileProvider was used to create image from icon string
    // then it'll try accessing imageconverter after bitmap deletion
    // for de-reference open file count, so it should be deleted last.
    delete iImageConverter;
    delete iProcessMonitor;
    }

// ---------------------------------------------------------
// CWmPortalButton::CWmPortalButton
// ---------------------------------------------------------
//
CWmPortalButton::CWmPortalButton( 
                        const TInt aFlags,
                        TInt aPortalButtonIndex )
    :CAknButton( aFlags ),
    iPortalButtonIndex( aPortalButtonIndex )
    {
    iButtonIcon = NULL;
    iButtonIconMask = NULL;
	iProcessMonitor = NULL;
    }

// ---------------------------------------------------------
// CWmPortalButton::ConstructL
// ---------------------------------------------------------
//
void CWmPortalButton::ConstructL( 
        CWmMainContainer* aParent,
        const TDesC& aText,
        const TDesC& aIcon )
    {
    if (  !aParent )
        {
        User::Leave( KErrArgument );
        }
    SetContainerWindowL( *aParent );
    
    // Obtain pointer to main container.
    iWmMainContainer = aParent;
    
    // construct the button
    CAknButton::ConstructL( NULL, NULL, NULL, NULL, aText, KNullDesC, 0 );

    SetFrameAndCenterIds( 
            KAknsIIDQsnFrButtonNormal,
            KAknsIIDQsnFrButtonCenterNormal,
            KAknsIIDQsnFrButtonPressed,
            KAknsIIDQsnFrButtonCenterPressed,
            KAknsIIDQsnFrButtonInactive,
            KAknsIIDQsnFrButtonCenterInactive,
            KAknsIIDQsnFrButtonPressed,
            KAknsIIDQsnFrButtonCenterPressed,
            KAknsIIDQsnFrButtonInactive,
            KAknsIIDQsnFrButtonCenterInactive );
    
    // start image converter for the icon
    iImageConverter = CWmImageConverter::NewL( this );
    TSize iconsize( LayoutIconSize() );
    iImageConverter->HandleIconString(
            iconsize.iWidth, iconsize.iHeight, aIcon );
    // observe our own press events
    SetObserver( this );
    
    SetFocusing( ETrue );
    SetRequestExit( ETrue ); // notification request for button up event
    
    // ready to be drawn
    ActivateL();
    }

// ---------------------------------------------------------
// CWmPortalButton::ExecuteL
// ---------------------------------------------------------
//
void CWmPortalButton::ExecuteL()
    {
    if ( iPortalButtonIndex == 0 )
        {
        if ( !iProcessMonitor )
            {
            iProcessMonitor = CWmProcessMonitor::NewL();
            }
		if ( !iProcessMonitor->IsActive() )
            {
            RunOviL( iWmMainContainer->Configuration() );
            }
        }
    else if ( iPortalButtonIndex == 1 )
        {
        RunOperatorL( iWmMainContainer->Configuration() );
        } 
    else
        {
        // Not supported
        User::Leave( KErrGeneral );
        }
    }
   
// ---------------------------------------------------------
// CWmPortalButton::RunOviL
// ---------------------------------------------------------
//
void CWmPortalButton::RunOviL( CWmConfiguration& aConf )
    {
    // param is: channel=homescreenwidgets 
    // laucher uid: 0x2002D07F
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    
    //get app info
    TApaAppInfo appInfo;
    TUid launchUid;
    launchUid = aConf.PortalButtonClientUid( iPortalButtonIndex );

    TInt err = session.GetAppInfo( appInfo, launchUid );
    if ( err != KErrNone )
        {
        //This is temp until we have laucher in SDK
        StartBrowserL( aConf );
        }
    else
        {
        // Form parameter
        HBufC* param = HBufC::NewLC( aConf.PortalButtonClientParam( iPortalButtonIndex ).Length() );
        param->Des().Copy( aConf.PortalButtonClientParam( iPortalButtonIndex ) );
        
        // do the launch
        RProcess process;
        User::LeaveIfError( process.Create( appInfo.iFullName, *param ) );
    
        iProcessMonitor->Monitor( process );
        process.Resume();
    
        CleanupStack::PopAndDestroy( param );
        }
    
    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------
// CWmPortalButton::StartBrowserL
// ---------------------------------------------------------
//
void CWmPortalButton::StartBrowserL( CWmConfiguration& aConf )
    {
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    HBufC* param = NULL;
    if ( iPortalButtonIndex == 1 )
        {
        // browser start parameter
        param = HBufC::NewLC( 
                aConf.PortalButtonBrowserUrl( iPortalButtonIndex ).Length() + 
                        KUrlPrefix().Length() );
        
        param->Des().Copy( KUrlPrefix );
        param->Des().Append( aConf.PortalButtonBrowserUrl( iPortalButtonIndex ) );
        }
    else
        {
        // becouse launcher knows url we need to have it temp here
        // until laucher is available
        _LIT( KTempUrl, "4 https://store.ovi.com/applications/");
        param = HBufC::NewLC( KTempUrl().Length() );
        param->Des().Copy( KTempUrl );
        }
    
    TUid id( KOSSBrowserUidValue );
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( id );
    if( task.Exists() )
        {
        task.BringToForeground();
        HBufC8* param8 = HBufC8::NewLC(param->Length());
        param8->Des().Append(*param);
        task.SendMessage(TUid::Uid(0), *param8); // UID not used
        CleanupStack::PopAndDestroy(param8);
        }
    else
        {
        if( !session.Handle() )
            {
            User::LeaveIfError( session.Connect() );
            }
        TThreadId thread;
        User::LeaveIfError(
                session.StartDocument(*param, KOSSBrowserUidValue, thread));
        }
    
    CleanupStack::PopAndDestroy( param );
    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------
// CWmPortalButton::RunOperatorL
// ---------------------------------------------------------
//
void CWmPortalButton::RunOperatorL( CWmConfiguration& aConf )
    {
    // Current info is that this will be a widget
    // meanwhile we just start browser
    StartBrowserL( aConf );
    }

// ---------------------------------------------------------
// CWmPortalButton::HandleControlEventL
// ---------------------------------------------------------
//
void CWmPortalButton::HandleControlEventL( CCoeControl* aControl,
        TCoeEvent aEventType )
    {
    if ( aControl == this )
        {
        if ( aEventType == EEventStateChanged ||
            aEventType == ELongPressEndedEvent ||
            aEventType == EEventRequestCancel )
            {
            DrawDeferred();
            }
        // execute portal action when button pressed (short or long press)
        if ( aEventType == EEventRequestExit )
            {
            SetFocus( EFalse );
            DrawNow();
            ExecuteL();
            }
        }
    }

// ---------------------------------------------------------
// CWmPortalButton::HandlePointerEventL
// ---------------------------------------------------------
//
void CWmPortalButton::HandlePointerEventL( 
        const TPointerEvent& aPointerEvent )
    {
    CAknButton::HandlePointerEventL( aPointerEvent );
    
    // remove focus from button if button is released outside rect 
    if ( aPointerEvent.iType == TPointerEvent::EDrag )
        {
        TBool wasFoucused( IsFocused() );
        SetFocus( Rect().Contains( aPointerEvent.iPosition ) );
        if ( wasFoucused != IsFocused() ){ DrawNow(); }
        }
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        SetFocus( EFalse ); // remove focus when button released.
        }
    }


// ---------------------------------------------------------
// CWmPortalButton::LayoutIconSize
// ---------------------------------------------------------
//
TSize CWmPortalButton::LayoutIconSize() const
    {
    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();
    TAknLayoutRect imageLayout;
    if ( iWmMainContainer->Configuration().PortalButtonCount() == 1 )
        {
        imageLayout.LayoutRect( Rect(), AknLayoutScalable_Apps::
                wgtman_btn_pane_g1( landscape ? 1 : 0).LayoutLine() );
        }
    else
        {
        TInt variety = (landscape ? 5 : 4);
        imageLayout.LayoutRect( Rect(), AknLayoutScalable_Apps
                ::wgtman_btn_pane_g1( variety ).LayoutLine() );
        }
    
    return imageLayout.Rect().Size();
    }

// ---------------------------------------------------------
// CWmPortalButton::SizeChanged
// ---------------------------------------------------------
//
void CWmPortalButton::SizeChanged()
    {    
    CAknButton::SizeChanged();

    SetTextVerticalAlignment( CAknButton::ECenter );
    
    // resize icon
    if ( iButtonIcon && iButtonIconMask )
        {
        TSize size = LayoutIconSize();
        AknIconUtils::SetSize( 
            iButtonIcon, size, EAspectRatioPreserved );
        AknIconUtils::SetSize( 
            iButtonIconMask, size, EAspectRatioPreserved );
        }
        
    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();
    SetTextAndIconAlignment( 
            landscape ? CAknButton::EIconOverText : CAknButton::EIconBeforeText );
    }

// ---------------------------------------------------------
// CWmPortalButton::NotifyCompletion
// ---------------------------------------------------------
//
void CWmPortalButton::NotifyCompletion( TInt aError )
    {
    if ( KErrNone == aError )
        {
        // take ownership of icon
        delete iButtonIcon;
        iButtonIcon = NULL;
        iButtonIcon = iImageConverter->Bitmap();
        delete iButtonIconMask;
        iButtonIconMask = NULL;
        iButtonIconMask = iImageConverter->Mask();
        if ( iButtonIcon && iButtonIconMask )
            {
            TSize size = LayoutIconSize();
            if ( iButtonIcon->SizeInPixels() != size )
                {
                AknIconUtils::SetSize( 
                    iButtonIcon, size, EAspectRatioPreserved );
                }
            if ( iButtonIconMask->SizeInPixels() != size )
                {
                AknIconUtils::SetSize( 
                    iButtonIconMask, size, EAspectRatioPreserved );
                }
            DrawDeferred();
            }
        }
    else
        {
        // no image available. Do nothing.
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

 	TAknsItemID frameId = ( KAknsIIDQsnFrButtonNormal );
    TAknsItemID frameCenterId = ( KAknsIIDQsnFrButtonCenterNormal );
 	
    if ( iButtonPressed )
    	{
        frameId = ( KAknsIIDQsnFrButtonPressed );
        frameCenterId = ( KAknsIIDQsnFrButtonCenterPressed );
    	}
    else if ( IsDimmed() )
        {
    	frameId = KAknsIIDQsnFrButtonInactive;
    	frameCenterId = KAknsIIDQsnFrButtonCenterInactive;
        }
    else if ( IsFocused() )
        {
        frameId = KAknsIIDQsnFrButtonHighlight;
        frameCenterId = KAknsIIDQsnFrButtonHighlightCenter;
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
        
        // draw image if one exists
        if ( iButtonIcon && iButtonIconMask )
            {
            TAknLayoutRect imageLayout;
            if ( iWmMainContainer->Configuration().PortalButtonCount() == 1 )
                {
                imageLayout.LayoutRect( rect, AknLayoutScalable_Apps::
                        wgtman_btn_pane_g1( landscape ? 3 : 0).LayoutLine() );
                }
            else
                {
                TInt variety = (landscape ? 7 : 4);
                imageLayout.LayoutRect( rect, AknLayoutScalable_Apps
                        ::wgtman_btn_pane_g1( variety ).LayoutLine() );
                }
            imageLayout.DrawImage( gc, iButtonIcon, iButtonIconMask );
            }
        
        // draw text if portrait        
        if ( !landscape )
            {
            TAknTextComponentLayout leftLayout;
            TInt variety = 
                ((iWmMainContainer->Configuration().PortalButtonCount()== 2)? 4:0 );
            leftLayout = AknLayoutScalable_Apps::wgtman_btn_pane_t1( variety  );
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
    else
        {
        // default for button
        AknsUtils::GetCachedColor( 
                skin, textColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG80 ); 
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
