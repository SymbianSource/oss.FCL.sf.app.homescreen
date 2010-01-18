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

#include <widgetmanagerview.rsg>
#include "widgetmanager.hrh"
#include "wmdetailsdlg.h"


// CONSTANTS


// MEMBER FUNCTIONS

// ---------------------------------------------------------
// FullScreenRect
// rectangle representing the screen
// ---------------------------------------------------------
//
TRect FullScreenRect()
    {
    TRect screen;
    CCoeEnv* env = CCoeEnv::Static();
    if ( env )
        {
        CWsScreenDevice* screenDevice = env->ScreenDevice();
        if ( screenDevice )
            screen.SetRect( TPoint(0,0), screenDevice->SizeInPixels() );
        }
    return screen;
    }

// ---------------------------------------------------------
// CWmDetailsDlg::NewL
// ---------------------------------------------------------
//
CWmDetailsDlg* CWmDetailsDlg::NewL(
			const TDesC& aName,
	        const TDesC& aDescription,
	        TBool  aCanBeAdded,
            const CFbsBitmap* aLogoBmp,
            const CFbsBitmap* aLogoMask,
            CAknsBasicBackgroundControlContext* aBgContext )
    {
    CWmDetailsDlg* self = new ( ELeave ) CWmDetailsDlg( aCanBeAdded );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aDescription, aLogoBmp, aLogoMask, aBgContext );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmDetailsDlg::CWmDetailsDlg
// ---------------------------------------------------------
//
CWmDetailsDlg::CWmDetailsDlg( TBool  aCanBeAdded )
    : CAknDialog(),
    iCanBeAdded( aCanBeAdded )
    {
    iBorder = TGulBorder::ERaisedControl;
    }

// ---------------------------------------------------------
// CWmDetailsDlg::~CWmDetailsDlg
// ---------------------------------------------------------
//
CWmDetailsDlg::~CWmDetailsDlg()
    {
    iEikonEnv->RemoveFromStack( iRtEditor );
    iEikonEnv->RemoveFromStack( this );
    delete iRtEditor;
    delete iName;
    delete iDescription;
    delete iLogoBmp;
    delete iLogoMask;	
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
            const CFbsBitmap* aLogoMask,
            CAknsBasicBackgroundControlContext* aBgContext )
    {
    if ( !aLogoBmp )
        {
        User::Leave( KErrArgument );
        }
 
	CAknDialog::ConstructL( R_AVKON_DIALOG_EMPTY_MENUBAR );
    
	iBgContext = aBgContext;
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

    iRtEditor->SetSkinBackgroundControlContextL( iBgContext );
    
    CEikScrollBarFrame* scrollBarFrame = iRtEditor->CreateScrollBarFrameL();
    scrollBarFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                             CEikScrollBarFrame::EOff );
    
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
    const TInt KMinTxtLength = 100;
    
    richText->Reset();
    if ( iDescription->Des().Length() <= KMinTxtLength )
        {     
        richText->InsertL( richText->DocumentLength(),
                CEditableText::ELineBreak );
        }
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
    if ( Layout_Meta_Data::IsMirrored() )
        {
        paraFormat.iHorizontalAlignment = CParaFormat::ERightAlign;
        }
    else
        {
        paraFormat.iHorizontalAlignment = CParaFormat::ELeftAlign;
        }
    charFormat.iFontPresentation.iTextColor = color;
    paraFormatMask.SetAttrib(EAttAlignment);
    charFormatMask.SetAttrib(EAttFontTypeface);
    charFormatMask.SetAttrib(EAttFontHeight);
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
    return iRtEditor ? 1 : 0;
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
// CWmDetailsDlg::WmDetailsDialogRect
// -----------------------------------------------------------------------------
//
TRect CWmDetailsDlg::WmDetailsDialogRect()
    {
    TRect mainPane, dlgRect;    
    TPoint topLeft;
    TInt desiredW, desiredH;    
    AknLayoutUtils::LayoutMetricsRect( 
            AknLayoutUtils::EApplicationWindow, mainPane );

    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        desiredW = ( mainPane.Width() - (mainPane.Width()/4) );
        desiredH = ( mainPane.Height() - (mainPane.Height()/5) );        
        topLeft.iX = ( ( mainPane.Width() - desiredW ) /2);
        topLeft.iY = ( ( mainPane.Height() - desiredH ) /2);
        dlgRect.SetRect( topLeft, TSize( desiredW, desiredH ) );
        }
    else
        {
        desiredH = ( mainPane.Height() - (mainPane.Height()/4) );
        desiredW = ( mainPane.Width() - (mainPane.Width()/5) );
        topLeft.iX = ( ( mainPane.Width() - desiredW ) /2);
        topLeft.iY = ( ( mainPane.Height() - desiredH ) /2);
        dlgRect.SetRect( topLeft, TSize( desiredW, desiredH ) );        
        }
    return dlgRect;
    }

// -----------------------------------------------------------------------------
// CWmDetailsDlg::SetSizeAndPosition
// -----------------------------------------------------------------------------
//
void CWmDetailsDlg::SetSizeAndPosition( const TSize& /*aSize*/ )
    {    
    SetRect( WmDetailsDialogRect() );
    }

// -----------------------------------------------------------------------------
// CWmDetailsDlg::SizeChanged
// -----------------------------------------------------------------------------
//
void CWmDetailsDlg::SizeChanged()
    {
    CAknDialog::SizeChanged();
    TRect rect = Rect();
    if ( iBgContext ) { iBgContext->SetRect( FullScreenRect() ); }
    if ( iRtEditor ) 
        {
        const TInt offSet = 5;
        TRect cbaRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EControlPane, cbaRect );
        
        TInt logoH = iLogoBmp->SizeInPixels().iHeight;
        
        TInt height, width;
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {        
            height = ( (rect.Height() - logoH) - (cbaRect.Height() * 2) - offSet );
            }
        else
            {
            height = ( (rect.Height() - logoH) - (cbaRect.Height() * 1.5) - offSet );
            }
 
        width = ( rect.Width() - (CEikScrollBar::DefaultScrollBarBreadth() * 4 ) - offSet );
        TPoint point = TPoint( rect.iTl.iX + offSet, rect.iTl.iY + logoH + offSet );
        TSize size = TSize( width, height );
        iRtEditor->SetRect( TRect( point, size ) );
        TRAP_IGNORE( 
            iRtEditor->SetSkinBackgroundControlContextL( iBgContext );
            InsertAndFormatContentL(); );
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
    const TInt offSet = 5;

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

    // to fix scrollbar background issue in richtext editor
    if ( iRtEditor ) 
        {
        MAknsControlContext* ccRte = AknsDrawUtils::ControlContext( iRtEditor );
        AknsDrawUtils::Background( skin, ccRte, iRtEditor, gc, iRtEditor->Rect() );
        }
    
    // draw logo
    TRect bmpRect( TPoint(0,0), TSize( iLogoBmp->SizeInPixels() ) );
    TPoint point = TPoint( innerRect.iTl.iX + offSet,
                           innerRect.iTl.iY + offSet );

    if ( iLogoBmp && iLogoMask )
        {
        gc.BitBltMasked( point, iLogoBmp, 
                         bmpRect, iLogoMask, ETrue );
        }
    else
        {
        gc.BitBlt( point, iLogoBmp, bmpRect );
        }

    // draw name
    point.iX += (bmpRect.Width() + offSet);
    
    AknsUtils::GetCachedColor( 
                    skin, color, 
                    KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );

    // DRAW TEXT
    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont );
    gc.UseFont( font );
    gc.SetPenColor( color );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    point.iY += (bmpRect.Height() - (font->HeightInPixels() /2 ) );
    CGraphicsContext::TDrawTextParam param;
    
    HBufC* buf = iName->Des().Alloc();
    if ( buf )
        {
        TPtr bufPtr = buf->Des();
        TextUtils::ClipToFit( bufPtr, *font, (innerRect.Width() - point.iX ) );
        gc.DrawText( bufPtr ,point, param );
        delete buf;
        }
    else
        {
        gc.DrawText( *iName ,point, param );
        }
    
    gc.DiscardFont();
    
    // draw the rounded rectangle as border
    const TInt KFrameRoundRadius = 3;
    const TInt KBorderWidth = 2;
    gc.SetPenSize( TSize( KBorderWidth, KBorderWidth ) );    
    gc.DrawRoundRect( innerRect, TSize( KFrameRoundRadius, KFrameRoundRadius ) );
    
    // draw shadow
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetBrushColor( color );
    gc.SetPenColor( color );
    TRect rightShadowArea = Rect();
    rightShadowArea.iTl.iX = rightShadowArea.iBr.iX - offSet;
    rightShadowArea.iTl.iY += offSet/2;
    gc.DrawRect(rightShadowArea);
    TRect bottomShadowArea = Rect();
    bottomShadowArea.iTl.iX += offSet/2;
    bottomShadowArea.iTl.iY += bottomShadowArea.iBr.iY - offSet;
    gc.DrawRect( bottomShadowArea );
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    }

// End of File

