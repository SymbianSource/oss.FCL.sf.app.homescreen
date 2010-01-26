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


#include "wmportalbutton.h"
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmmaincontainer.h"
#include "wmconfiguration.h"

// CONSTANTS
_LIT( KBrowserPrefix, "4 ");

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

    TAknsItemID frameId = KAknsIIDQgnHomeWmButton;
    TAknsItemID frameCenterId = KAknsIIDQgnHomeWmButtonCenter;
    TAknsItemID framePressedId = KAknsIIDQgnHomeWmButtonPressed;
    TAknsItemID framePressedCenterId = KAknsIIDQgnHomeWmButtonPressedCenter;

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
    
    // start image converter for the icon
    iImageConverter = CWmImageConverter::NewL( this );
    TSize iconsize( LayoutIconSize() );
    iImageConverter->HandleIconString(
            iconsize.iWidth, iconsize.iHeight, aIcon );
    // observe our own press events
    SetObserver( this );
    
    // ready to be drawn
    ActivateL();
    }

// ---------------------------------------------------------------------------
// Runs HTTP method: (starts browser or brongs browser to foreground)
// ---------------------------------------------------------------------------
//
void TryRunHttpL( const TDesC& aParam )
    {
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    // browser start parameters
    const TUid KOSSBrowserUidValue = { 0x10008D39 };
    HBufC* param = HBufC::NewLC( aParam.Length() + 
            KBrowserPrefix().Length() );
    
    param->Des().Copy( KBrowserPrefix );
    param->Des().Append( aParam );
    
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
        User::LeaveIfError(session.StartDocument(*param, KOSSBrowserUidValue, thread));
        }
    
    CleanupStack::PopAndDestroy( param );
    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------------------------
// Runs WIDGET method: (launches given widget with parameters)
// ---------------------------------------------------------------------------
//
void TryRunWidgetL( const TDesC& aBundleId, const TDesC& aParam )
    {
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    TApaAppInfo appInfo;
    TUid launchUid;
    
    // Get widget uid    
    RWidgetRegistryClientSession widgetSession;
    User::LeaveIfError( widgetSession.Connect() );    
    CleanupClosePushL( widgetSession );
    launchUid.iUid = widgetSession.GetWidgetUidL( aBundleId );
    CleanupStack::PopAndDestroy( &widgetSession );

    // prepare widget start params
    User::LeaveIfError( session.GetAppInfo( appInfo, launchUid ) );
    CApaCommandLine* commandLine = CApaCommandLine::NewLC();
    commandLine->SetExecutableNameL( appInfo.iFullName );
    HBufC8* buf8 = HBufC8::NewLC( aParam.Length() );
    buf8->Des().Copy( aParam );

    // do the launch
    commandLine->SetTailEndL( *buf8 );
    User::LeaveIfError( session.StartApp( *commandLine ) );
    
    CleanupStack::PopAndDestroy( buf8 );
    CleanupStack::PopAndDestroy( commandLine );
    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------------------------
// Runs APPLICATION method: (launches given application with parameters)
// ---------------------------------------------------------------------------
//
void TryRunApplicationL( const TDesC& /*aApplication*/, const TDesC& /*aParam*/ )
    {
    // This method has not been implemented
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Tries to open a portal with given method and parameters.
// this method may be called twice on a portal button, if a primary
// method fails.
// ---------------------------------------------------------------------------
//
void TryOpenPortalL(
        CWmConfiguration::TMethod aMethod, const TDesC& aService,
        const TDesC& aParam )
    {
    // open portal according to the method.
    if ( aMethod == CWmConfiguration::EHttp )
        { TryRunHttpL( aParam ); }
    else if ( aMethod == CWmConfiguration::EWidget )
        { TryRunWidgetL( aService, aParam ); }
    else if ( aMethod == CWmConfiguration::EApplication )
        { TryRunApplicationL( aService, aParam ); }
    else
        { User::Leave( KErrNotSupported ); }
    }

// ---------------------------------------------------------------------------
// Opens a portal. Called when user presses a portal button. tries the
// primary method, and if if fails, tries the secondary. If it fails,
// gives up.
// ---------------------------------------------------------------------------
//
void OpenPortalL(
        CWmConfiguration& aConfiguration, TInt aPortalIndex )
    {
    TRAPD( err,
        TryOpenPortalL(
            aConfiguration.PortalButtonPrimaryMethod( aPortalIndex ),
            aConfiguration.PortalButtonPrimaryService( aPortalIndex ),
            aConfiguration.PortalButtonPrimaryParams( aPortalIndex ) ); );
    if ( err != KErrNone )
        {
        // if secondary method fails, leave will be propagated.
        TryOpenPortalL(
            aConfiguration.PortalButtonSecondaryMethod( aPortalIndex ),
            aConfiguration.PortalButtonSecondaryService( aPortalIndex ),
            aConfiguration.PortalButtonSecondaryParams( aPortalIndex ) );
        }
    }

// ---------------------------------------------------------
// CWmPortalButton::ExecuteL
// ---------------------------------------------------------
//
void CWmPortalButton::ExecuteL()
    {
    OpenPortalL( iWmMainContainer->Configuration(), iPortalButtonIndex );
    }

// ---------------------------------------------------------
// CWmPortalButton::HandleControlEventL
// ---------------------------------------------------------
//
void CWmPortalButton::HandleControlEventL( CCoeControl* /*aControl*/,
        TCoeEvent aEventType )
    {
    // execute portal action when button pressed (short or long press)
    if ( aEventType == EEventStateChanged ||
        aEventType == ELongPressEndedEvent )
        {
        ExecuteL();
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
    imageLayout.LayoutRect( Rect(),
        AknLayoutScalable_Apps::wgtman_btn_pane_g1(
            landscape ? 1 : 0).LayoutLine() );
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
            AknIconUtils::SetSize( 
                    iButtonIcon, size, EAspectRatioPreserved );
            AknIconUtils::SetSize( 
                    iButtonIconMask, size, EAspectRatioPreserved );
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

 	TAknsItemID frameId = ( KAknsIIDQgnHomeWmButton );
    TAknsItemID frameCenterId = ( KAknsIIDQgnHomeWmButtonCenter );
 	
    if ( iButtonPressed )
    	{
        frameId = ( KAknsIIDQgnHomeWmButtonPressed );
        frameCenterId = ( KAknsIIDQgnHomeWmButtonPressedCenter );
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
        
        // draw image if one exists
        if ( iButtonIcon && iButtonIconMask )
            {
            TAknLayoutRect imageLayout;
            imageLayout.LayoutRect( rect,
                    AknLayoutScalable_Apps::wgtman_btn_pane_g1(
                            landscape ? 1 : 0).LayoutLine() );
            imageLayout.DrawImage( gc, iButtonIcon, iButtonIconMask );
            }
        
        // draw text if portrait        
        if ( !landscape )
            {
            TAknTextComponentLayout leftLayout;
            leftLayout = AknLayoutScalable_Apps::wgtman_btn_pane_t1( 
                                                landscape ? 1 : 0  );
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
