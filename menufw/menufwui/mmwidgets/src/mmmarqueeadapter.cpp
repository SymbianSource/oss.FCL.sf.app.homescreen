/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>
#include <eikfrlb.h>
#include <AknBidiTextUtils.h>
#include <gdi.h>
#include <AknLayoutFont.h>

#include "mmlistbox.h"
#include "mmmarqueeadapter.h"
#include "mmwidgetsconstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter::CMmMarqueeAdapter ()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter::~CMmMarqueeAdapter ()
	{
	iMarqueeElements.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter* CMmMarqueeAdapter::NewLC ()
	{
	CMmMarqueeAdapter* self = new (ELeave)CMmMarqueeAdapter();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter* CMmMarqueeAdapter::NewL ()
	{
	CMmMarqueeAdapter* self = CMmMarqueeAdapter::NewLC ();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::ConstructL ()
	{
	iMarqueeEnabled = ETrue;
	iCurrentMarqueeItemIndex = -1;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::AddMarqueeElementL(
	    const TRect& aRect,
	    const TDesC& aText,
	    TAknLogicalFontId aFont,
	    TRgb  aColor,
		CGraphicsContext::TTextAlign aAlign,
		TInt aIndex,
		TInt aBaselineOffset,
		TInt aCurrentlyDrawnItemIndex
	    )
	{
	if( !iMarqueeEnabled )
		{
		return;
		}
	
	if ( aCurrentlyDrawnItemIndex != iCurrentMarqueeItemIndex )
		{
		StopMarqueeDrawing();
		iCurrentMarqueeItemIndex = aCurrentlyDrawnItemIndex;
		}

	CMmMarqueeElement* element = NULL;
	TInt elementIndex(0);
	for (TInt i = 0; i < iMarqueeElements.Count() ; i++)
		{
		if (iMarqueeElements[i]->iSubcellIndex == aIndex)
			{
			element = iMarqueeElements[i];
			elementIndex = i;
			break;
			}
		}

	if ( element )
		{
		const TDesC& previousText = *(element->iText);
		TBool textChanged = previousText.Compare( aText );
	
		if ( textChanged )
			{
			CAknMarqueeControl * control = element->iMarqueeControl;
			control->Stop();
			control->Reset();
			iMarqueeElements.Remove(elementIndex );
			delete element;
			}
		}
	
	if( SubcellMarqueeElementExists( aIndex ) )
		{
		return;
		}
		
	CMmMarqueeElement* newElement = 
		CMmMarqueeElement::NewL( aRect, aAlign, aText, aFont, aColor, aIndex, aBaselineOffset );
	CleanupStack::PushL( newElement );
	newElement->SetupMarqueeControlL( MmMarqueeConstants::KLoops,
			MmMarqueeConstants::KScrollAmount, MmMarqueeConstants::KScrollDelay);
    TCallBack callBack(CMmMarqueeAdapter::RedrawEvent, iListbox);
    if ( newElement->iMarqueeControl ) 
    	{
        newElement->iMarqueeControl->SetRedrawCallBack(callBack);
    	}
    iMarqueeElements.AppendL( newElement );
    CleanupStack::Pop( newElement );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::StopMarqueeDrawing(TInt aIndex)
	{
    for (TInt elementIndex = iMarqueeElements.Count() - 1;
            elementIndex >= 0; elementIndex--)
        {
        if ((aIndex < 0) ||
                (iMarqueeElements[elementIndex]->iSubcellIndex == aIndex))
            {
            CMmMarqueeElement* element = iMarqueeElements[elementIndex];
            CAknMarqueeControl * control = element->iMarqueeControl;
            control->Stop();
            control->Reset();
            iMarqueeElements.Remove(elementIndex);
            delete element;
            if (iMarqueeElements.Count() == 0 || (aIndex >= 0))
                {
                iCurrentMarqueeItemIndex = -1;
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::EnableMarquee( TBool aEnable )
	{
	iMarqueeEnabled = aEnable;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::DrawMarqueeL(CWindowGc & aGc )
	{
	if ( iCurrentMarqueeItemIndex != iListbox->View()->CurrentItemIndex() )
		{
		StopMarqueeDrawing();
		return;
		}
	else if ( iMarqueeEnabled )
		{
		for (TInt i = 0; i < iMarqueeElements.Count() ; i++)
			{
			CMmMarqueeElement* element = iMarqueeElements[i];
			CAknMarqueeControl* control = element->iMarqueeControl;
			control->EnableMarquee( ETrue );
			
//			Setup graphical context ( font, colors ) for marquee
			const CFont* font = AknLayoutUtils::FontFromId( element->iFont );
            TInt baseLineOffset = CAknLayoutFont::AsCAknLayoutFontOrNull( font )->TextPaneTopToBaseline();
            aGc.UseFont(font);
            aGc.SetPenColor( element->iColor );
            aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
            
//			Let marquee know if it needs to do bidi conversion
            control->UseLogicalToVisualConversion( ETrue );
            
            TRect subcellRectOnScreen = element->iElementRect;
            TPoint currentItemPositionOnScreen 
				= iListbox->View()->ItemPos( iListbox->View()->CurrentItemIndex() );
            subcellRectOnScreen.Move( currentItemPositionOnScreen.iX , currentItemPositionOnScreen.iY );
            if ( control->DrawText(aGc, subcellRectOnScreen, *(element->iText),
            		baseLineOffset, element->iAlign, *font) )
            	
                {
//              All the loops have been executed, the text needs to be truncated.
                TBuf< MmMarqueeConstants::KClippingBufLength > clipbuf 
					= (element->iText)->Left( MmMarqueeConstants::KTextTrimmingThreshold );
//              Get the clipping buffer and draw it
            	TInt maxClipWidth = element->iElementRect.Width();
                AknBidiTextUtils::ConvertToVisualAndClipL( clipbuf, *font,
                		element->iElementRect.Width(), maxClipWidth );
                aGc.DrawText(clipbuf, subcellRectOnScreen, baseLineOffset , element->iAlign, 0);
                }
//          Clean the font cache in graphical context after marquee drawing
            aGc.DiscardFont();
			}
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
TInt CMmMarqueeAdapter::RedrawEvent(TAny* aControl )
    {
    if ( !((CCoeControl*)aControl)->IsVisible() )
        {
        return EFalse;
        }
    
    CMmListBox* listBox = 
            (CMmListBox*)aControl;
    if (listBox->CurrentItemIndex() >= 0 
    		&& listBox->CurrentItemIndex() <= listBox->View()->BottomItemIndex()
    		&& listBox->CurrentItemIndex() >= listBox->View()->TopItemIndex()
    		)
        {
        listBox->SetMarqueeDrawing( ETrue ); //redraw the item without using marquee in the last loop
        listBox->View()->DrawItem( listBox->CurrentItemIndex() );
        listBox->SetMarqueeDrawing(EFalse);
        }
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::SetControl( CEikListBox * aControl )
	{
	iListbox = aControl;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
TBool CMmMarqueeAdapter::SubcellMarqueeElementExists( TInt aIndex )
	{
	TBool ret = EFalse;
	for (TInt i = 0; i < iMarqueeElements.Count() ; i++)
		{
		if (iMarqueeElements[i]->iSubcellIndex == aIndex)
			{
			ret = ETrue;
			}
		}
	return ret;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::RemoveSubcellMarqueeElement(TInt aSubcellIndex)
	{
	for (TInt i = 0; i < iMarqueeElements.Count() ; i++)
		{
		if (iMarqueeElements[i]->iSubcellIndex == aSubcellIndex)
			{
			iMarqueeElements.Remove(i);
			break;
			}
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
TBool CMmMarqueeAdapter::IsMarqueeEnabled() const
    {
    return iMarqueeEnabled;
    }

//class CMmMarqueeElement

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter::CMmMarqueeElement::CMmMarqueeElement (
		TRect aElementRect,
		CGraphicsContext::TTextAlign aAlign,
		TAknLogicalFontId aFont,
		TRgb aColor, TInt aIndex,
		TInt aBaselineOffset ):iElementRect(aElementRect),
		iAlign(aAlign),
		iFont(aFont),iColor(aColor),
		iSubcellIndex(aIndex),
		iBaselineOffset(aBaselineOffset)
	{

	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter::CMmMarqueeElement::~CMmMarqueeElement ()
	{
	delete iMarqueeControl;
	delete iText;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmMarqueeAdapter::CMmMarqueeElement* CMmMarqueeAdapter::CMmMarqueeElement::NewL (
		TRect aElementRect, CGraphicsContext::TTextAlign aAlign,
		const TDesC& aText, TAknLogicalFontId aFont, TRgb aColor,TInt aIndex,
		TInt aBaselineOffset )
	{
	CMmMarqueeAdapter::CMmMarqueeElement* self 
		= new (ELeave)CMmMarqueeAdapter::CMmMarqueeElement( aElementRect, aAlign,
				aFont, aColor, aIndex, aBaselineOffset );
	CleanupStack::PushL( self ); 
	self->ConstructL( aText );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 		
void CMmMarqueeAdapter::CMmMarqueeElement::ConstructL ( const TDesC& aText )
	{
	iText = aText.AllocL();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmMarqueeAdapter::CMmMarqueeElement::SetupMarqueeControlL( 
		TInt aLoops, 
		TInt aScrollAmount, 
		TInt aScrollDelay )
	{
	iMarqueeControl = CAknMarqueeControl::NewL( aLoops, aScrollAmount, aScrollDelay );
	iMarqueeControl->SetLoops(aLoops);
	iLoops = aLoops;
	iScrollAmount = aScrollAmount;
	iScrollDelay = aScrollDelay;
	}
// End of file

		


