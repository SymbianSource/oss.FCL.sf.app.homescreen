/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Text scrolling functionality.
*
*/


// INCLUDE FILES
#include <AknUtils.h>
#include <gulicon.h>
#include <AknsDrawUtils.h>
#include <AknBidiTextUtils.h>

#include "xnnewstickercontrol.h"
#include "xnnewstickeradapter.h"
#include "xnproperty.h"

// CONSTANTS
/**
* for the empty space in pixels between text and image.
*/
const TInt KGap = 10;

   
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::CTitleData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CTitleData::CTitleData(TInt aLenghtInPixels) :
    iTitleIsSvg(EFalse), iTextLenghtInPixels(aLenghtInPixels)
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CTitleData::~CTitleData()
    {
    delete iText;
    delete iData;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::CTitleData::ConstructL(const TDesC& aTitle)
    {
    delete iText;
    iText = NULL;
    iText = aTitle.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::CTitleData::ConstructL(const TDesC8& aByteData)
    {
    iTitleIsSvg = ETrue;
    iData = aByteData.AllocL();
    iText = KNullDesC().AllocL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CTitleData* CXnNewstickerControl::CTitleData::NewL(
                const TDesC& aTitle, TInt aLenghtInPixels)
    {
    CXnNewstickerControl::CTitleData* self =
            CXnNewstickerControl::CTitleData::NewLC(aTitle, aLenghtInPixels);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CTitleData* CXnNewstickerControl::CTitleData::NewLC(
                const TDesC& aTitle, TInt aLenghtInPixels)
    {
    CXnNewstickerControl::CTitleData* self =
            new(ELeave) CXnNewstickerControl::CTitleData(aLenghtInPixels);
    CleanupStack::PushL(self);
    self->ConstructL(aTitle);
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CTitleData* CXnNewstickerControl::CTitleData::NewL(
                const TDesC8& aByteData)
    {
    CXnNewstickerControl::CTitleData* self =
            CXnNewstickerControl::CTitleData::NewLC(aByteData);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CTitleData* CXnNewstickerControl::CTitleData::NewLC(
                const TDesC8& aByteData)
    {
    CXnNewstickerControl::CTitleData* self =
            new(ELeave) CXnNewstickerControl::CTitleData();
    CleanupStack::PushL(self);
    self->ConstructL(aByteData);
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::TitleText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CXnNewstickerControl::CTitleData::TitleText()
    {
    return *iText;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::SetTitleText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::CTitleData::SetTitleTextL(const TDesC& aTitle)
    {
    HBufC* newTitle = aTitle.AllocL();
    delete iText;
    iText = newTitle;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::TitleTextLengthInPixels
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerControl::CTitleData::TitleTextLengthInPixels()
    {
    return iTextLenghtInPixels;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::SetTitleTextLengthInPixels
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::CTitleData::SetTitleTextLengthInPixels(TInt aLenghtInPixels)
    {
    iTextLenghtInPixels = aLenghtInPixels;    
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::IsSvgTitle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::CTitleData::IsSvgTitle()
    {
    return iTitleIsSvg;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CTitleData::SvgTitleData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CXnNewstickerControl::CTitleData::SvgTitleData()
    {
    return *iData;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CXnNewstickerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CXnNewstickerControl(CXnNewstickerAdapter* aAdapter) :     
    iFirstDrawingTitleIndex(0),
    iFirstDrawingOffset(0),
    iImageWidthInPixels(0),
    iCurrentTitleIndex(-1),
    iSeparatorimage(NULL),
    iSeparatorImageWidth(0),
    iFont(NULL),
    iTextBaseline(0),
    iTextColor(KRgbBlack),
    iUnderlining(EUnderlineOff),
    iStrikethrough(EStrikethroughOff),
    iIsWestern(ETrue),
    iAdapter(aAdapter),
    iTextAlignment(ELayoutAlignLeft)
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::ConstructL()
    {
    if(AknLayoutUtils::LayoutMirrored())
        {
    	iIsWestern = EFalse;
        iTextAlignment = ELayoutAlignRight;
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl* CXnNewstickerControl::NewL(CXnNewstickerAdapter* aAdapter)
    {
    CXnNewstickerControl* self = new(ELeave)CXnNewstickerControl(aAdapter);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();    
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::~CXnNewstickerControl()
// Destructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::~CXnNewstickerControl()
    {
    iTitles.ResetAndDestroy();
    delete iSeparatorimage;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetScrollAmount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::SetScrollAmount(TInt aScrollAmount)
    {
    iSpeed = aScrollAmount;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetNewstickerRect
// Set visible rect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::SetNewstickerRect(TRect& aContentRect)
    {
    if(iContentRect != aContentRect)
        {
        iContentRect = aContentRect;
        iTextBaseline = iContentRect.Height() / 2 + iFont->AscentInPixels() / 2;
        SetBeginningState();
        for(TInt i = 0; i < iTitles.Count(); i++)
            {
            if(!iTitles[i]->IsSvgTitle())
                {
                TInt textWidth = iFont->TextWidthInPixels(
                    iTitles[i]->TitleText());
                iTitles[i]->SetTitleTextLengthInPixels(textWidth);
                }
            }
        CalculateTextFitInNewstickerRect();        
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::IsWestern
// Returns ETrue if western layout is used, otherwise EFalse is returned.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::IsWestern()
    {
    return iIsWestern;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::AppendTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::AppendTitleL(const TDesC& aTitle)
    {
    TInt textWidth = iFont->TextWidthInPixels(aTitle);
    CTitleData* title = CTitleData::NewLC(aTitle, textWidth);
    iTitles.AppendL(title);
    CleanupStack::Pop(title);
    
    CalculateTextFitInNewstickerRect();    

    // if this is the first item
    if(iTitles.Count() == 1) 
        {
        iCurrentTitleIndex = 0;
        iAdapter->StartL();    
        }
    }

// ---------------------------------------------------------
// CXnNewstickerControl::InsertTitleL
// ---------------------------------------------------------
//
void CXnNewstickerControl::InsertTitleL(const TDesC& aTitle, TInt aIndex)
    {
    TInt textWidth = iFont->TextWidthInPixels(aTitle);
    CTitleData* title = CTitleData::NewLC(aTitle, textWidth);
    iTitles.InsertL(title, aIndex);
    CleanupStack::Pop(title);

    
    CountIndexAfterInsertL(aIndex);
    CalculateTextFitInNewstickerRect();    
    }

// ---------------------------------------------------------
// CXnNewstickerControl::UpdateTitleL
// ---------------------------------------------------------
//
void CXnNewstickerControl::UpdateTitleL(const TDesC& aTitle, TInt aIndex)
    {
    if ( aIndex >= 0 && aIndex < iTitles.Count() )
        {
        CTitleData* titleData = iTitles[aIndex];    
        TInt textWidth = iFont->TextWidthInPixels(aTitle);
        titleData->SetTitleTextL(aTitle);
        titleData->SetTitleTextLengthInPixels(textWidth);
        CalculateTextFitInNewstickerRect();
        }
    else
        {
        AppendTitleL(aTitle);
        }
    }

// ---------------------------------------------------------
// CXnNewstickerControl::DeleteTitle
// ---------------------------------------------------------
//
void CXnNewstickerControl::DeleteTitleL(TInt aIndex)
    {
    TInt count = iTitles.Count();
    if (aIndex >= 0 && aIndex < count)
        {
        // If the last item will be deleted
        if(count == 1)
            {
            iAdapter->StopL();
            iFirstDrawingTitleIndex = GetNextTitleWithContent( 0 );
            SetBeginningState();
            iCurrentTitleIndex = -1;
            }
        if (aIndex == iFirstDrawingTitleIndex)
            {
            // If this happens to be the last item in the list
            if(aIndex == count - 1)
                {
                iFirstDrawingTitleIndex = GetNextTitleWithContent( 0 );
                }
            SetBeginningState();
            iCurrentTitleIndex = iFirstDrawingTitleIndex;
            }
            
        //  Check if the title was deleted before the current title
        else if (aIndex < iFirstDrawingTitleIndex)
            {
            iCurrentTitleIndex--;
            iFirstDrawingTitleIndex = GetNextTitleWithContent( iFirstDrawingTitleIndex-1, ETrue);
            }
        UpdateTitleL(KNullDesC, aIndex);
        CalculateTextFitInNewstickerRect();        
        }
    }

// ---------------------------------------------------------
// CXnNewstickerControl::CurrentTitleIndex
// ---------------------------------------------------------
//
TInt CXnNewstickerControl::CurrentTitleIndex()
    {
    return iCurrentTitleIndex;
    }

// ---------------------------------------------------------
// CXnNewstickerControl::Title
// ---------------------------------------------------------
//
const TDesC& CXnNewstickerControl::Title(TInt aIndex)
    {
    if (aIndex < 0 || aIndex > iTitles.Count())
        {
        return KNullDesC;
        }

    if (iTitles[aIndex]->IsSvgTitle())
        {
        return KNullDesC;
        }

    return iTitles[aIndex]->TitleText();
    }

// ---------------------------------------------------------
// CXnNewstickerControl::SetSeparatorImageL
// ---------------------------------------------------------
//
TInt CXnNewstickerControl::SetSeparatorImageL(CGulIcon* aIcon)
    {
    CFbsBitmap* bitmap = aIcon->Bitmap();
    if(AknIconUtils::IsMifIcon(bitmap))
        {
        delete iSeparatorimage;
        iSeparatorimage = aIcon;
        TInt maxHeight = iContentRect.Height();
        TSize size = TSize(maxHeight, maxHeight);        
        AknIconUtils::SetSize(bitmap, size, EAspectRatioPreservedAndUnusedSpaceRemoved);
        iSeparatorImageWidth = bitmap->SizeInPixels().iWidth + KGap;
        return KErrNone;
        }
    else
        {
        iSeparatorimage = NULL;
        return KErrNotSupported;
        }
    }

// ---------------------------------------------------------
// CXnNewstickerControl::ClearTitles
// ---------------------------------------------------------
//
void CXnNewstickerControl::ClearTitles()
    {
    TRAP_IGNORE(iAdapter->StopL());
    iFirstDrawingTitleIndex = 0;
    SetBeginningState();
    iCurrentTitleIndex = -1;
    // Don't delete just clear the contents
    for( TInt i = 0; i < iTitles.Count(); ++i )
        {
        TRAP_IGNORE( UpdateTitleL( KNullDesC, i ) );
        }
    iTextFitInNewstickerRect = EFalse;
    }

// ---------------------------------------------------------
// CXnNewstickerControl::AppendSvgTitleL
// ---------------------------------------------------------
//
void CXnNewstickerControl::AppendSvgTitleL(const TDesC8& aByteData)
    {
    CTitleData* title = CTitleData::NewLC(aByteData);
    iTitles.AppendL(title);
    CleanupStack::Pop(title);
    
    // if this is the first item
    if(iTitles.Count() == 1) 
        {
        iCurrentTitleIndex = 0;
        iAdapter->StartL();
        }
    }

// ---------------------------------------------------------
// CXnNewstickerControl::InsertSvgTitleL
// ---------------------------------------------------------
//
void CXnNewstickerControl::InsertSvgTitleL(const TDesC8& aByteData, TInt aIndex)
    {
    CTitleData* title = CTitleData::NewLC(aByteData);
    iTitles.InsertL(title, aIndex);
    CleanupStack::Pop(title);
    CountIndexAfterInsertL(aIndex);
    }

// ---------------------------------------------------------
// CXnNewstickerControl::CurrentSvgTitle
// ---------------------------------------------------------
//
const TDesC8& CXnNewstickerControl::CurrentSvgTitle()
    {
    CTitleData* title = iTitles[iFirstDrawingTitleIndex];
    //  Is it SVG title
    if (title->IsSvgTitle())
        {
        //  Yes, advance the title index
        iFirstDrawingTitleIndex++;
        if (iFirstDrawingTitleIndex >= iTitles.Count())
            {
            iFirstDrawingTitleIndex = 0;
            }

        //  Return the data
        return title->SvgTitleData();
        }

    //  Not svg title, return empty desc
    return KNullDesC8;
    }

// ---------------------------------------------------------
// CXnNewstickerControl::TitleCount
// ---------------------------------------------------------
//
TInt CXnNewstickerControl::TitleCount() const
    {
    return iTitles.Count();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::MoveToNext
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::MoveToNextL()
    {
    //  If there are no titles, don't do anything.
    if (!IsVisibleTitles())
        {
        return ;
        }
    // Skip notification of the first title
    if ( iCurrentTitleIndex != GetNextTitleWithContent( 0 ) )
        {
        iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleScrolled);
        }
    // Don't report the last TitleToScroll in case the scroll ended
    if ( iAdapter->CurrentState() != CXnNewstickerAdapter::EScrollEnded )
        {    
        iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleToScroll);
        }
    iAdapter->TitleScrolled(iFirstDrawingTitleIndex);
    if (iAdapter->CurrentState() == CXnNewstickerAdapter::EAnimation)
        {
        iCurrentTitleIndex = GetNextTitleWithContent( iCurrentTitleIndex + 1 );
        iFirstDrawingTitleIndex = iCurrentTitleIndex;
        }
    else
        {
        iCurrentTitleIndex = GetNextTitleWithContent( iFirstDrawingTitleIndex + 1);
        iFirstDrawingTitleIndex = iCurrentTitleIndex;
        }

    //  Check that we are still in range
    if (iFirstDrawingTitleIndex >= iTitles.Count() || iFirstDrawingTitleIndex < 0)
        {
        iFirstDrawingTitleIndex = GetNextTitleWithContent( 0 );
        iCurrentTitleIndex = iFirstDrawingTitleIndex;
        }

    SetBeginningState();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::MoveToFirstL
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::MoveToFirstL()
    {
    //  If there are no titles, don't do anything.
    if (!IsVisibleTitles())
        {
        return ;
        }
    // Something already scrolling, report scroll done
    if ( iAdapter->CurrentState() == CXnNewstickerAdapter::EText )
        {
        iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleScrolled);
        iAdapter->TitleScrolled(iFirstDrawingTitleIndex);
        }
       
    TInt start = 0; // iIsWestern ? 0 : (iTitles.Count() - 1);
    
    iCurrentTitleIndex = GetNextTitleWithContent( start );
    iFirstDrawingTitleIndex = iCurrentTitleIndex;
    iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleToScroll);
    SetBeginningState();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::MoveToLastL
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::MoveToLastL()
    {
    //  If there are no titles, don't do anything.
    if (!IsVisibleTitles())
        {
        return ;
        }
    // Something already scrolling, report scroll done
    if ( iAdapter->CurrentState() == CXnNewstickerAdapter::EText )
        {
        iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleScrolled);
        iAdapter->TitleScrolled(iFirstDrawingTitleIndex);
        }
    
    TInt start = iTitles.Count() - 1; // iIsWestern ? (iTitles.Count() - 1) : 0;
    
    iCurrentTitleIndex = GetNextTitleWithContent( start, ETrue );
    iFirstDrawingTitleIndex = iCurrentTitleIndex;
    iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleToScroll);
    SetBeginningState();
    }
// -----------------------------------------------------------------------------
// CXnNewstickerControl::MoveToPrev
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::MoveToPrevL()
    {
    //  If there are no titles, don't do anything.
    if (!IsVisibleTitles())
        {
        return ;
        }
    
    iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleScrolled);

    if (iAdapter->CurrentState() == CXnNewstickerAdapter::EAnimation)
        {
        iCurrentTitleIndex = GetNextTitleWithContent( iCurrentTitleIndex - 1, ETrue );        
        iFirstDrawingTitleIndex = iCurrentTitleIndex;
        }
    else
        {
        iFirstDrawingTitleIndex = GetNextTitleWithContent( iFirstDrawingTitleIndex - 1, ETrue );
        iCurrentTitleIndex = iFirstDrawingTitleIndex;
        }
    //  Check that we are still in range
    if (iFirstDrawingTitleIndex >= iTitles.Count() || iFirstDrawingTitleIndex < 0)
        {
        iFirstDrawingTitleIndex = GetNextTitleWithContent( iTitles.Count() - 1, ETrue );
        iCurrentTitleIndex = iFirstDrawingTitleIndex;
        }
    
    iAdapter->ReportNewstickerEventL(XnPropertyNames::action::trigger::name::KTitleToScroll);
    SetBeginningState();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::MoveToCurrent
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::MoveToCurrent()
    {
    //  If there are no titles, don't do anything.
    if (!IsVisibleTitles())
        {
        return ;
        }

    iFirstDrawingTitleIndex = iCurrentTitleIndex;
    SetBeginningState();    
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CountIndexAfterInsert
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::CountIndexAfterInsertL(TInt aIndex)
    {
    //  Check if the new title was inserted before the first drawing title
    if (aIndex <= iFirstDrawingTitleIndex)
        {
        iCurrentTitleIndex++;
        iFirstDrawingTitleIndex++;
        }
        
    // if this is the first item
    if(iTitles.Count() == 1 && GetNextTitleWithContent( 0 ) == 0) 
        {
        iCurrentTitleIndex = 0;
        iFirstDrawingTitleIndex = 0;
        iAdapter->StartL();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetFont
// -----------------------------------------------------------------------------
void CXnNewstickerControl::SetFont(CFont* aFont)
    {
    if(iFont != aFont)
        {
        for(TInt i = 0; i < iTitles.Count(); i++)
            {
            if(!iTitles[i]->IsSvgTitle())
                {
                TInt textWidth = aFont->TextWidthInPixels(
                    iTitles[i]->TitleText());
                iTitles[i]->SetTitleTextLengthInPixels(textWidth);
                }
            }
        iFont = aFont;
        iTextBaseline = iContentRect.Height() / 2 + iFont->AscentInPixels() / 2;
        CBitmapContext* gc = iAdapter->BufferGc();
        if(gc)
            {
            gc->UseFont(aFont);
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetTextColor
// -----------------------------------------------------------------------------
void CXnNewstickerControl::SetTextColor(TRgb aColor)
    {
    iTextColor = aColor;
    CBitmapContext* gc = iAdapter->BufferGc();
    if(gc)
        {
        gc->SetPenColor(iTextColor);
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetTextUnderlineStyle
// -----------------------------------------------------------------------------
void CXnNewstickerControl::SetTextUnderlineStyle(TFontUnderline aStyle)
    {
    iUnderlining = aStyle;
    CBitmapContext* gc = iAdapter->BufferGc();
    if(gc)
        {
        gc->SetUnderlineStyle(iUnderlining);        
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetTextUnderlineStyle
// -----------------------------------------------------------------------------
void CXnNewstickerControl::SetTextStrikethroughStyle(TFontStrikethrough aStyle)
    {
    iStrikethrough = aStyle;
    CBitmapContext* gc = iAdapter->BufferGc();
    if(gc)
        {
        gc->SetStrikethroughStyle(iStrikethrough);        
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::PrepareToDrawL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::PrepareToDrawLtrL()
    {
    CBitmapContext* gc = iAdapter->BufferGc();
    const CFbsBitmap* background = iAdapter->BackgroundBitmap();
    if(!gc || !background || !IsVisibleTitles() )
        {
        return;
        }
    iFirstDrawingTitleIndex = GetNextTitleWithContent( iFirstDrawingTitleIndex );
    if ( iFirstDrawingTitleIndex < 0 )
        {
        return;
        }
    if (iTitles[iFirstDrawingTitleIndex]->IsSvgTitle())
        {
        //  Show SVG title
        iAdapter->ShowSvgL();
        
    	// Just in case, draw background to d-buffer
        gc->BitBlt(TPoint(0,0), background);
        return;
        }

    CGraphicsContext::TTextAlign alignment = TextAlignment();
    if ( iTextFitInNewstickerRect || 
         iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate && iTitles.Count() == 1 )
        {
        iAdapter->StopL();
        SetBeginningState();
        }
    
	TInt drawingPos(-iFirstDrawingOffset);

	// At first, draw background
    gc->BitBlt(TPoint(0,0), background);
    TInt textLeft = iTitles[iFirstDrawingTitleIndex]->TitleTextLengthInPixels() - 
        iFirstDrawingOffset;
    TInt drawingTitleIndex = iFirstDrawingTitleIndex;
    TInt drawingOffset(0);

	// Main drawing loop. This loop runs as long as drawing area is not filled
    // if Scroll behaviour is slide, then short text drawn only once.
    FOREVER
        {
        TRect clipRect(TPoint(drawingPos,0), TPoint(iContentRect.Width(), iContentRect.Height()));
        if ( iTextFitInNewstickerRect )
            {
            clipRect.SetWidth( clipRect.Width()-drawingPos);
            }
        // if the rest text fits into visible area
     	if (textLeft <= iContentRect.Width() - drawingOffset)
     	    {
            gc->DrawText(iTitles[drawingTitleIndex]->TitleText(), 
                   clipRect,
                   iTextBaseline,
                   alignment);

     	    TInt drawnLength = iTitles[drawingTitleIndex]->TitleTextLengthInPixels() + KGap;
            drawingPos += drawnLength;
     	    drawingOffset = drawingPos;
            
            // Update title index, if needed
     	    drawingTitleIndex++;
     	    if(drawingTitleIndex >= iTitles.Count())
     	        {
     	        drawingTitleIndex = 0;
     	        if ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::ESlide || iTextFitInNewstickerRect )
     	            {
     	            // what a dirty code!
     	            break;
     	            }
     	        }
            if(iSeparatorimage)
                {
                textLeft = 0;
                }
            else
                {
                if (iTitles[drawingTitleIndex]->IsSvgTitle())
                    {
                    return;
                    }
                else
                    {
                    textLeft = iTitles[drawingTitleIndex]->TitleTextLengthInPixels(); 
                    }
                }
     	    }
        else 
            {
            // Clip the text if needed in alternate mode
            if  ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate )
                {
                HBufC* title = iTitles[drawingTitleIndex]->TitleText().AllocLC();
                TPtr titlePtr = title->Des();
                TInt maxLength = iContentRect.Width();
                AknBidiTextUtils::ConvertToVisualAndClipL(
                        titlePtr, *iFont, maxLength, maxLength );                              
                gc->DrawText( titlePtr, clipRect, iTextBaseline, alignment );
                CleanupStack::PopAndDestroy( title );
                }
            else
                {
                gc->DrawText(iTitles[drawingTitleIndex]->TitleText(), 
                        clipRect,
                        iTextBaseline,
                        alignment);
            
                }            
     	    break;
            }
            
        if(iSeparatorimage)
     	    {
            TInt imageLeft = iSeparatorImageWidth - KGap;
            
            // if the rest of the image fits into visible area
 	        if (imageLeft <= iContentRect.Width() - drawingOffset)
 	            {
 	            gc->BitBltMasked(TPoint(drawingPos, 0), iSeparatorimage->Bitmap(), 
 	                TRect(TPoint(0,0), 
 	                iSeparatorimage->Bitmap()->SizeInPixels()), iSeparatorimage->Mask(), ETrue); 
                drawingPos += iSeparatorImageWidth;
     	        drawingOffset = drawingPos;
                textLeft = iTitles[drawingTitleIndex]->TitleTextLengthInPixels();
                if (iTitles[drawingTitleIndex]->IsSvgTitle())
                    {
                    return;
                    }
 	            }
            else 
                {
 	            gc->BitBltMasked(TPoint(drawingPos, 0), iSeparatorimage->Bitmap(), 
 	                TRect(TPoint(0,0), iSeparatorimage->Bitmap()->SizeInPixels()), 
 	                iSeparatorimage->Mask(), ETrue); 
                break;
                }
     	    }

        if( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate || 
			iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate )
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::PrepareToDrawRtlL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::PrepareToDrawRtlL()
    {
    CBitmapContext* gc = iAdapter->BufferGc();
    const CFbsBitmap* background = iAdapter->BackgroundBitmap();
    if(!gc || !background || !IsVisibleTitles() )
        {
        return;
        }
    if (iTitles[iFirstDrawingTitleIndex]->IsSvgTitle())
        {
        //  Show SVG title
        iAdapter->ShowSvgL();
        
    	// Just in case, draw background to d-buffer
        gc->BitBlt(TPoint(0,0), background);
        return;
        }

    CGraphicsContext::TTextAlign alignment = TextAlignment();
    if ( iTextFitInNewstickerRect || 
         iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate && iTitles.Count() == 1 )
        {
        SetBeginningState();
        iAdapter->StopL();
        }
    
	// At first, draw background
    gc->BitBlt(TPoint(0,0), background);
    
    TInt textLeft = iFirstDrawingOffset - 
        iTitles[iFirstDrawingTitleIndex]->TitleTextLengthInPixels();
    if(textLeft >= iContentRect.Width() )
        {
        textLeft = 0;
        }
    TInt drawingTitleIndex = iFirstDrawingTitleIndex;
    TInt drawingOffset(iContentRect.iBr.iX);
    
    TRect clipRect(TPoint(0,0), TPoint(iFirstDrawingOffset, iContentRect.Height()));
    TInt clipWidth = clipRect.Width();
	// Main drawing loop. This loop runs as long as drawing area is not filled
    FOREVER
        {
     	// if the rest text fits into visible area
     	if (textLeft <= drawingOffset)
     	    {
     	    HBufC* title = iTitles[drawingTitleIndex]->TitleText().AllocLC();
     	    TPtr titlePtr = title->Des();

     	    // convert to visual, do not clip
     	    TInt maxLength = iTitles[drawingTitleIndex]->TitleTextLengthInPixels();
     	    AknBidiTextUtils::ConvertToVisualAndClipL( titlePtr, *iFont,
     	                                               maxLength, maxLength );
   
            gc->DrawText( titlePtr, clipRect, iTextBaseline, alignment );
            CleanupStack::PopAndDestroy( title );

     	    TInt drawnLength = iTitles[drawingTitleIndex]->TitleTextLengthInPixels() + KGap;
            clipWidth -= drawnLength;
            clipRect.SetWidth(clipWidth);
            drawingOffset = clipRect.iBr.iX;

            // Update title index
     	    drawingTitleIndex++;
     	    if(drawingTitleIndex >= iTitles.Count())
     	        {
     	        drawingTitleIndex = 0;
                if ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::ESlide || iTextFitInNewstickerRect)
                    {
                    // what a dirty code!
                    break;
                    }
     	        }
            if(iSeparatorimage)
                {
                textLeft = 0xfff;
                }
            else
                {
                if (iTitles[drawingTitleIndex]->IsSvgTitle())
                    {
                    return;
                    }
                else
                    {
                    textLeft = iTitles[drawingTitleIndex]->TitleTextLengthInPixels(); 
                    }
                }
     	    }
        else 
            {
            // Clip the text if needed in alternate mode
            if  ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate )
                {
                HBufC* title = iTitles[drawingTitleIndex]->TitleText().AllocLC();
                TPtr titlePtr = title->Des();
                TInt maxLength = iContentRect.Width();
                AknBidiTextUtils::ConvertToVisualAndClipL(
                        titlePtr, *iFont, maxLength, maxLength );                              
                gc->DrawText( titlePtr, clipRect, iTextBaseline, alignment );
                CleanupStack::PopAndDestroy( title );
                }
            else
                {
                gc->DrawText(iTitles[drawingTitleIndex]->TitleText(), clipRect, 
                        iTextBaseline, alignment);
                }
     	    break;
            }

        if(iSeparatorimage)
     	    {
            TInt imageLeft = iSeparatorImageWidth - KGap;
            
            // if the rest of the image fits into visible area
 	        if (imageLeft <= drawingOffset)
 	            {
 	            gc->BitBltMasked(TPoint(clipRect.iBr.iX - imageLeft, 0), 
 	                iSeparatorimage->Bitmap(), TRect(TPoint(0,0), 
 	                iSeparatorimage->Bitmap()->SizeInPixels()), 
 	                iSeparatorimage->Mask(), ETrue);                
                clipWidth -= iSeparatorImageWidth;
                clipRect.SetWidth(clipWidth);
                drawingOffset = clipRect.iBr.iX;                
                textLeft = iTitles[drawingTitleIndex]->TitleTextLengthInPixels();
                if (iTitles[drawingTitleIndex]->IsSvgTitle())
                    {
                    return;
                    }
 	            }
            else 
                {
 	            gc->BitBltMasked(TPoint(clipRect.iBr.iX - imageLeft, 0), 
 	                iSeparatorimage->Bitmap(), TRect(TPoint(0,0), 
 	                iSeparatorimage->Bitmap()->SizeInPixels()), 
 	                iSeparatorimage->Mask(), ETrue); 
                break;
                }
     	    }
     	    
        if( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate || 
			iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate )     	    
            {
            break;
            }     	    
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::DoScrollL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::DoScrollL()
    {
    //  If there are no titles, don't do anything.
    if( !IsVisibleTitles() || iFirstDrawingTitleIndex < 0 )
        {
        return;
        }
    
    iAdapter->DrawNow();
    
    if( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScroll ||
        iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::ESlide ||
		( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate &&
		  !CalculateCurrentTextFitInNewstickerRect() ) )
        {    
        iAdapter->SetTimerToScrolltime();             
        iFirstDrawingOffset += iSpeed; // advance text
        TInt titleLen = iTitles[iFirstDrawingTitleIndex]->TitleTextLengthInPixels() + 
            iSeparatorImageWidth;

        // if 100% (or more) has been drawn update...
        if ((iIsWestern && iFirstDrawingOffset >= titleLen) ||
            (!iIsWestern && iFirstDrawingOffset - titleLen >= iContentRect.Width()))
            {
            iAdapter->TitleScrolled(iFirstDrawingTitleIndex);
            iFirstDrawingTitleIndex = GetNextTitleWithContent( iFirstDrawingTitleIndex + 1);
            if(iFirstDrawingTitleIndex >= iTitles.Count())
     	        {
     	        iFirstDrawingTitleIndex = GetNextTitleWithContent( 0 );
                if (iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::ESlide )
                    {
                    if ( iIsWestern )
                        {
                        iFirstDrawingOffset = -(iContentRect.iBr.iX-iContentRect.iTl.iX-KGap);
                        }
                    else
                        {
                        iFirstDrawingOffset = 0;
                        }
                    }
                else
                    {
                    iFirstDrawingOffset -= titleLen + KGap;
                    }
     	        }
            else
                {
                iFirstDrawingOffset -= titleLen + KGap;
                }
            iCurrentTitleIndex = iFirstDrawingTitleIndex;

            if( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate &&
		        CalculateCurrentTextFitInNewstickerRect() )
                {
                MoveToNextL();
                }
            }
        // but if less than 100% and more than 75% has been drawn, update only current title index.
        else if (((iIsWestern && iFirstDrawingOffset >= titleLen * 0.75) || 
                  (!iIsWestern && iFirstDrawingOffset - titleLen >= iContentRect.iBr.iX * 0.75)) &&
                   iFirstDrawingTitleIndex == iCurrentTitleIndex)
            {
            iCurrentTitleIndex++;
     	    if(iCurrentTitleIndex >= iTitles.Count())
     	        {
     	        iCurrentTitleIndex = 0;
     	        }
     	    iCurrentTitleIndex = GetNextTitleWithContent( iCurrentTitleIndex );

            if( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate &&
		        CalculateCurrentTextFitInNewstickerRect() )
                {
                MoveToNextL();
                }
            }
        }
    else if( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate &&
		     CalculateCurrentTextFitInNewstickerRect() )
        {
        iAdapter->SetTimerToDisplaytime();
        MoveToNextL();
        }
    else
        {
        MoveToNextL();
        }        
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::Draw
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::Draw()
    {
    if( IsVisibleTitles() > 0 ) 
        {
        if( iIsWestern )
            {
            TRAP_IGNORE( PrepareToDrawLtrL() );
            }
        else
            {
            TRAP_IGNORE( PrepareToDrawRtlL() );
            }
        }    
    else
        {
        CBitmapContext* gc = iAdapter->BufferGc();
        const CFbsBitmap* background = iAdapter->BackgroundBitmap();
        if ( gc && background )
            {            
            gc->BitBlt(TPoint(0,0), background);
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::DrawStatic
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::DrawStatic()
    {
    // draw to buffer gc
    CBitmapContext* gc = iAdapter->BufferGc();
    const CFbsBitmap* background = iAdapter->BackgroundBitmap();
    if( !gc || !background || !IsVisibleTitles() )
        {
        return;
        }
    gc->BitBlt( TPoint(0,0), background );
    
    // Get the last title
    TInt index = GetNextTitleWithContent( iTitles.Count() - 1, ETrue );
    const TDesC& title = Title( index );
    
    // truncate text if needed
    HBufC* visualText = HBufC::New(title.Length() + KAknBidiExtraSpacePerLine);
    if (visualText)
        {
        TPtr visualTextPtr(visualText->Des());
        TInt maxLength = iContentRect.Width();
        AknBidiTextUtils::ConvertToVisualAndClip(
                     title, visualTextPtr, *iFont, maxLength, maxLength );
            
        TRect blitRect( 0, 0, iContentRect.Width(), iContentRect.Height() );
        gc->DrawText( *visualText, blitRect, iTextBaseline, TextAlignment() );
        delete visualText;
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetBeginningState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::SetBeginningState()
    {
    iFirstDrawingOffset = 0;
    if(!iIsWestern)
        {
        iFirstDrawingOffset = iContentRect.Width();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetScrollLooping
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::SetScrollLooping( TBool aLoop )
    {
    iScrollLooping = aLoop;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::ScrollLooping
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::ScrollLooping()
    {
    return iScrollLooping;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::TextFitInNewstickerRect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::TextFitInNewstickerRect()
    {
    return iTextFitInNewstickerRect; 
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CalculateTextFitInNewstickerRect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::CalculateTextFitInNewstickerRect()
    {
    if ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::ESlide &&
         iTitles.Count() == 1 )
        {
        const TInt count = iTitles.Count();
        TInt textWidth=0;
        const TInt rectWidth = iContentRect.Width();
        for (TInt i=0; i<count && textWidth<rectWidth;i++)
            {
            textWidth += iTitles[i]->TitleTextLengthInPixels();
            }
        iTextFitInNewstickerRect = (textWidth<rectWidth);
        }
    else
        {
        iTextFitInNewstickerRect = EFalse;
        }
    return iTextFitInNewstickerRect;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CalculateCurrentTextFitInNewstickerRect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::CalculateCurrentTextFitInNewstickerRect()
    {
    if ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate )
        {
        const TInt count = iTitles.Count();
        TInt textWidth=0;
        const TInt rectWidth = iContentRect.Width();
        if( iCurrentTitleIndex < count )
            {
            textWidth = iTitles[iCurrentTitleIndex]->TitleTextLengthInPixels();
            }
        return ( textWidth < rectWidth );
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetTextAlignment
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::SetTextAlignment(TInt aAlignment)
    {
    iTextAlignment = aAlignment;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::TextAlignment
// if title fits to the drawing rect and scrolling behaviour is ESlide then 
// use alignment value 
// if scroll behaviour is alternate then use alignment value
// otherwise return ELeft in western and ERight on AH
// -----------------------------------------------------------------------------
//
CGraphicsContext::TTextAlign CXnNewstickerControl::TextAlignment()
    {
    CGraphicsContext::TTextAlign alignment = iIsWestern ? CGraphicsContext::ELeft : CGraphicsContext::ERight;
    
    if ( iTextFitInNewstickerRect || iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EAlternate ||
         ( iAdapter->ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate && 
           CalculateCurrentTextFitInNewstickerRect() ) )
        {
        switch( iTextAlignment )
            {
            default:
            case ELayoutAlignLeft:
                alignment = CGraphicsContext::ELeft;
                break;
            case ELayoutAlignRight:
                alignment = CGraphicsContext::ERight;
                break;
            case ELayoutAlignCenter:
                alignment = CGraphicsContext::ECenter;
                break;
            }
        }
    return alignment;
    }

TInt CXnNewstickerControl::GetNextTitleWithContent( TInt aStartSearch, TBool aBackwards )
    {
    TInt dir = 1;
    if ( aBackwards )
        {
        dir = -1;
        }
    for( TInt i = aStartSearch; i < iTitles.Count() && i >= 0; i += dir )
        {
        if ( iTitles[i]->TitleTextLengthInPixels() != 0 )
            {
            return i;
            }
        }
    return aStartSearch;
    }

TBool CXnNewstickerControl::IsVisibleTitles()
    {
    for( TInt i = 0; i < iTitles.Count() ; ++i )
        {
        if ( iTitles[i]->TitleTextLengthInPixels() != 0 )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


//  End of File  
