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
* Description:
* Implementation of the widget details dlg for WidgetManager
*
*/

// INCLUDE FILES
#include <gdi.h>
#include <eikrted.h> // for CRichTextEditor
#include <txtrich.h> // for CRichText
#include <AknUtils.h> // for AknUtils
#include <AknsUtils.h> // MAknsSkinInstance
#include <AknsDrawUtils.h> // AknsDrawUtils
#include <applayout.cdl.h> //layout
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknMarqueeControl.h>
#include <widgetmanagerview.rsg>
#include "widgetmanager.hrh"
#include "wmdetailsdlg.h"


// CONSTANTS


// MEMBER FUNCTIONS

// ---------------------------------------------------------
// CWmDetailsDlg::NewL
// ---------------------------------------------------------
//
CWmDetailsDlg* CWmDetailsDlg::NewL(
			const TDesC& aName,
	        const TDesC& aDescription,
	        TBool  aCanBeAdded,
            const CFbsBitmap* aLogoBmp,
            const CFbsBitmap* aLogoMask )
    {
    CWmDetailsDlg* self = new ( ELeave ) CWmDetailsDlg( aCanBeAdded );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aDescription, aLogoBmp, aLogoMask );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmDetailsDlg::CWmDetailsDlg
// ---------------------------------------------------------
//
CWmDetailsDlg::CWmDetailsDlg( TBool  aCanBeAdded )
    : CAknDialog(),
    iCanBeAdded( aCanBeAdded ),
    iNeedToScroll( EFalse )
    {
    iBorder = TGulBorder::ERaisedControl;    
    }

// ---------------------------------------------------------
// CWmDetailsDlg::~CWmDetailsDlg
// ---------------------------------------------------------
//
CWmDetailsDlg::~CWmDetailsDlg()
    {
    StopMarquee();
    iEikonEnv->RemoveFromStack( iRtEditor );
    iEikonEnv->RemoveFromStack( this );
    delete iRtEditor;
    delete iMarquee;
    delete iName;
    delete iDescription;
    delete iLogoBmp;
    delete iLogoMask;
    delete iBgContext;
    }

// -----------------------------------------------------------------------------
// CWmDetailsDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
TInt CWmDetailsDlg::ExecuteLD()
    {
    if ( iCanBeAdded )
        {
        return CAknDialog::ExecuteLD( R_WM_DETAILS_ADD_DIALOG );
        }
    else
        {
        return CAknDialog::ExecuteLD( R_WM_DETAILS_ONLY_DIALOG );
        }
    }

// ---------------------------------------------------------
// CWmDetailsDlg::ConstructL
// ---------------------------------------------------------
//
void CWmDetailsDlg::ConstructL(
			const TDesC& aName,
	        const TDesC& aDescription,
            const CFbsBitmap* aLogoBmp,
            const CFbsBitmap* aLogoMask )
    {
    if ( !aLogoBmp )
        {
        User::Leave( KErrArgument );
        }
 
	CAknDialog::ConstructL( R_AVKON_DIALOG_EMPTY_MENUBAR );
    
	iBgContext = CAknsBasicBackgroundControlContext::NewL( 
	        KAknsIIDQsnFrPopupCenter, TRect(0,0,1,1), EFalse);

	iName = aName.AllocL();
	iDescription = aDescription.AllocL();

	// create bitmap and duplicate handle
	iLogoBmp = new ( ELeave ) CFbsBitmap;
    TSize newSize = TSize( aLogoBmp->SizeInPixels().iWidth ,
                           aLogoBmp->SizeInPixels().iHeight );
    User::LeaveIfError( iLogoBmp->Create( newSize, aLogoBmp->DisplayMode() ) );    
    User::LeaveIfError( iLogoBmp->Duplicate( aLogoBmp->Handle() ) );
	
    if ( aLogoMask )
        {
        iLogoMask = new ( ELeave ) CFbsBitmap;
        newSize = TSize( aLogoMask->SizeInPixels().iWidth ,
                         aLogoMask->SizeInPixels().iHeight );
        User::LeaveIfError( iLogoMask->Create( newSize, aLogoMask->DisplayMode() ) );    
        User::LeaveIfError( iLogoMask->Duplicate( aLogoMask->Handle() ) );
        }
    
    iEikonEnv->AddWindowShadow( static_cast<CCoeControl*>(this) );
    }

// -----------------------------------------------------------------------------
// CWmDetailsDlg::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CWmDetailsDlg::OkToExitL( TInt aButtonId )
	{
    if ( aButtonId == ECbaAddToHs ||
		aButtonId == EAknSoftkeyClose )
        {
        // Close dlg
        return ETrue;
        }
    return EFalse;
	}

// -----------------------------------------------------------------------------
// CWmDetailsDlg::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CWmDetailsDlg::PreLayoutDynInitL()
    {
    iRtEditor = new(ELeave) CEikRichTextEditor();
    iRtEditor->ConstructL( this, 
                        0, 
                        0, 
                        CEikEdwin::EReadOnly | 
                        CEikEdwin::ENoAutoSelection | 
                        CEikEdwin::EAvkonDisableCursor |
                        CEikEdwin::EDisplayOnly );
    iRtEditor->SetContainerWindowL(*this);
    iRtEditor->SetAknEditorFlags( 
                    EAknEditorFlagEnableScrollBars | 
                    EEikEdwinAvkonDisableCursor |
                    EEikEdwinDisplayOnly |
                    EEikEdwinReadOnly );
    iRtEditor->EnableKineticScrollingL( ETrue );

    CEikScrollBarFrame* scrollBarFrame = iRtEditor->CreateScrollBarFrameL();
    scrollBarFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                             CEikScrollBarFrame::EAuto );
    
    iMarquee = CAknMarqueeControl::NewL();
    TCallBack callback( RedrawCallback, this );
    iMarquee->SetRedrawCallBack( callback );
    iMarquee->SetLoops( 1 );
    iMarquee->SetContainerWindowL( *this );
    
    InsertAndFormatContentL();
    iRtEditor->UpdateScrollBarsL();
    }

//------------------------------------------------------------------------------
// CWmDetailsDlg::InsertAndFormatContentL
//------------------------------------------------------------------------------
//
void CWmDetailsDlg::InsertAndFormatContentL()
    {
    CRichText* richText = iRtEditor->RichText();
    
    richText->Reset();
    richText->InsertL( richText->DocumentLength(), *iDescription );

    // change the color of the text according to the skin color
    TRgb color = KRgbBlack;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors, 
                                                    EAknsCIQsnTextColorsCG6 );

    // Format text 
    iRtEditor->SetSelectionL( 0, richText->DocumentLength() );

    // Get a logical font to base my font on:
    const CFont* logicalFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    // Extract font information
    TFontSpec fontspec = logicalFont->FontSpecInTwips();
    TCharFormat charFormat(fontspec.iTypeface.iName, fontspec.iHeight);
    TCharFormatMask charFormatMask;
    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
    paraFormat.iHorizontalAlignment = CParaFormat::ELeftAlign;
    charFormat.iFontPresentation.iTextColor = color;
    paraFormatMask.SetAttrib(EAttAlignment);
    charFormatMask.SetAttrib(EAttFontTypeface);
    charFormatMask.SetAttrib(EAttFontHeight);
    paraFormatMask.SetAttrib(EAttLineSpacing);
    charFormatMask.SetAttrib(EAttColor);
    iRtEditor->ApplyParaFormatL(&paraFormat, paraFormatMask);
    iRtEditor->ApplyCharFormatL(charFormat, charFormatMask);
    iRtEditor->ClearSelectionL();
    }

//------------------------------------------------------------------------------
// CWmDetailsDlg::CountComponentControls
//------------------------------------------------------------------------------
//
TInt CWmDetailsDlg::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iRtEditor )
        count++;
    if ( iMarquee )
        count++;
    return count;
    }

//------------------------------------------------------------------------------
// CWmDetailsDlg::ComponentControl
//------------------------------------------------------------------------------
//
CCoeControl* CWmDetailsDlg::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iRtEditor;
        case 1:
            return iMarquee;
        default:
            return NULL;
        }
    }

//------------------------------------------------------------------------------
// CWmDetailsDlg::ActivateL
//------------------------------------------------------------------------------
//
void CWmDetailsDlg::ActivateL()
	{
    CAknDialog::ActivateL();
	iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityDialog );
    iEikonEnv->EikAppUi()->AddToStackL( iRtEditor );    
    iRtEditor->SetFocus( ETrue );
    }

// -----------------------------------------------------------------------------
// CWmDetailsDlg::SetSizeAndPosition
// -----------------------------------------------------------------------------
//
void CWmDetailsDlg::SetSizeAndPosition( const TSize& /*aSize*/ )
    {    
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EApplicationWindow, mainPane );
    TAknWindowLineLayout dlgWindow = AknLayoutScalable_Apps
               ::popup_wgtman_window().LayoutLine();
    AknLayoutUtils::LayoutControl( this, mainPane, dlgWindow );
    }

// -----------------------------------------------------------------------------
// CWmDetailsDlg::SizeChanged
// -----------------------------------------------------------------------------
//
void CWmDetailsDlg::SizeChanged()
    {
    CAknDialog::SizeChanged();
    TRect rect = Rect();
    if ( iBgContext ) { iBgContext->SetRect( rect ); }
    if ( iRtEditor ) 
        {
        TAknLayoutRect layoutRect;
        TAknWindowLineLayout contentPane = AknLayoutScalable_Apps
                   ::wgtman_content_pane().LayoutLine();        
        TAknWindowLineLayout rtePane = AknLayoutScalable_Apps
                   ::wgtman_list_pane().LayoutLine();
        layoutRect.LayoutRect( rect, contentPane );
        AknLayoutUtils::LayoutControl( iRtEditor, layoutRect.Rect(), rtePane );
        CEikScrollBarFrame* sbFrame = iRtEditor->ScrollBarFrame();
        if ( sbFrame && sbFrame->VScrollBarVisibility() != CEikScrollBarFrame::EOff )
            {
            CEikScrollBar* scrollBar = sbFrame->VerticalScrollBar();
            TAknWindowLineLayout scrollPane = AknLayoutScalable_Apps
                           ::scroll_pane_cp036().LayoutLine();
            AknLayoutUtils::LayoutControl( scrollBar, layoutRect.Rect(), scrollPane );
            }
        TRAP_IGNORE( 
            iRtEditor->SetSkinBackgroundControlContextL( iBgContext );
            InsertAndFormatContentL(); );

        StartMarquee();
        }
    }

// ----------------------------------------------------------------------------
// CWmDetailsDlg::SupplyMopObject
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CWmDetailsDlg::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CAknDialog::MopSupplyObject( aId );
    }
	
// ----------------------------------------------------------------------------
// CWmDetailsDlg::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CWmDetailsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType )
	{
	if (aType == EEventKey)
		{
		switch (aKeyEvent.iCode)
			{			
			case EKeyEscape:
				{
				// framework calls this when dialog must shut down
				return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        		}
			case EKeyDownArrow:
                {
                if ( iRtEditor )
                    {
                    iRtEditor->MoveCursorL(TCursorPosition::EFPageDown, EFalse);
                    return EKeyWasConsumed;
                    }
                }
            case EKeyUpArrow:
                {
                if ( iRtEditor )
                    {
                    iRtEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
                    return EKeyWasConsumed;
                    }
                }				
            default:
				{
				if ( iRtEditor )
					{
					return iRtEditor->OfferKeyEventL( aKeyEvent, aType );
					}
			    break;
				}
			}
		}
	return CAknDialog::OfferKeyEventL( aKeyEvent, aType);
	}

// -----------------------------------------------------------------------------
// CWmDetailsDlg::Draw
// -----------------------------------------------------------------------------
//
void CWmDetailsDlg::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    TRect rect = Rect();
    TRect innerRect( rect );
    TRgb color = KRgbBlack;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    // draw background
    if ( !AknsDrawUtils::Background( skin, cc, this, gc, innerRect ) )
        {
        gc.SetPenStyle( CGraphicsContext::ENullPen );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
        gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( innerRect );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
        }

    // draw logo
    TAknLayoutRect layoutRect;
    TAknWindowLineLayout headingPane = AknLayoutScalable_Apps
               ::wgtman_heading_pane().LayoutLine();        
    layoutRect.LayoutRect( rect, headingPane );

    if( iLogoBmp && iLogoMask )
        {
        TAknLayoutRect logoLayout;
        logoLayout.LayoutRect( layoutRect.Rect(),AknLayoutScalable_Apps
                ::wgtman_heading_pane_g1().LayoutLine() );        
        logoLayout.DrawImage( gc, iLogoBmp, iLogoMask );
        }
    
    AknsUtils::GetCachedColor( 
                    skin, color, 
                    KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );

    // DRAW TEXT
    TAknTextLineLayout titleTextLayout = 
              AknLayoutScalable_Apps::wgtman_heading_pane_t1().LayoutLine();
    TAknLayoutText textLayoutTitle;
    textLayoutTitle.LayoutText( layoutRect.Rect(), titleTextLayout );
    
    const CFont* font = textLayoutTitle.Font();
    if ( !font ) 
        {
        font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont ); 
        }

    gc.UseFont( font );
    gc.SetPenColor( color );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    
    TBool truncate( ETrue );

    // check if name needs scrolling
    if ( textLayoutTitle.Font()->TextWidthInPixels( *iName ) > 
        textLayoutTitle.TextRect().Width() && IsFocused() )
        {
        const_cast<CWmDetailsDlg&>(*this).iNeedToScroll = ETrue;
        const_cast<CWmDetailsDlg&>(*this).StartMarquee();
        truncate = iMarquee->DrawText(
                        gc, textLayoutTitle.TextRect(), *iName,               
                        (textLayoutTitle.TextRect().Height() - font->FontMaxDescent()),
                        CGraphicsContext::ELeft, *font );
        }

    if ( truncate )
        {
        const_cast<CWmDetailsDlg&>(*this).StopMarquee();
        textLayoutTitle.DrawText( gc, *iName, ETrue, color );
        }

    gc.DiscardFont();

    // draw the rounded rectangle as border
    const TInt KFrameRoundRadius = 3;
    const TInt KBorderWidth = 2;
    gc.SetPenSize( TSize( KBorderWidth, KBorderWidth ) );    
    gc.DrawRoundRect( innerRect, TSize( KFrameRoundRadius, KFrameRoundRadius ) );
    }

// ---------------------------------------------------------
// CWmDetailsDlg::RedrawCallback
// ---------------------------------------------------------
//
TInt CWmDetailsDlg::RedrawCallback( TAny* aPtr )
    {
    CWmDetailsDlg* self = 
        static_cast<CWmDetailsDlg*>( aPtr );
    if ( !self->iNeedToScroll )
        {
        self->StopMarquee();    
        }
    self->DrawDeferred();
    return static_cast<TBool>( ETrue );
    }

// ---------------------------------------------------------
// CWmDetailsDlg::StartMarquee
// ---------------------------------------------------------
//
void CWmDetailsDlg::StartMarquee()
    {
    if ( iMarquee && iNeedToScroll &&
        !iMarquee->IsMarqueeOn() )
        {
        iMarquee->Reset();
        iMarquee->EnableMarquee( iNeedToScroll );
        iMarquee->Start();
        }
    }

// ---------------------------------------------------------
// CWmDetailsDlg::StopMarquee
// ---------------------------------------------------------
//
void CWmDetailsDlg::StopMarquee()
    {
    if ( iMarquee && iMarquee->IsMarqueeOn() )
        {
        iMarquee->EnableMarquee( EFalse );
        iMarquee->Stop();        
        }
    }

// ---------------------------------------------------------
// CWmDetailsDlg::FocusChanged
// ---------------------------------------------------------
//
void CWmDetailsDlg::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    if ( IsFocused() )
        {
        StartMarquee();
        }
    else
        {
        StopMarquee();
        }
    }


// End of File

