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
* Implementation of the widget listbox for WidgetManager
*
*/

// INCLUDE FILES
#include <coemain.h>
#include <StringLoader.h>
#include <aknstyluspopupmenu.h> 
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>

#include <widgetmanagerview.rsg>
#include <widgetmanager.mbg>
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmlistbox.h"
#include "wmwidgetdata.h"

// CONSTANTS
const TInt KFrameRoundRadius = 4;
const TInt KFocusFrameWidth = 2;
const TInt KLeftMarginBig = 5;
const TInt KRightMargin = 1;
const TInt KTopTextMargin = 2;
const TInt KRightTextMargin = 3;
const TInt KRightMarginBig = 5;
const TInt KTopMargin = 1;
const TInt KTopMarginBig = 5;
const TInt KBottomMargin = 1;
const TInt KBottomMarginBig = 5;
const TInt KLogoWidth = 55;
const TInt KLogoHeight = 55;

// MEMBER FUNCTIONS

// ---------------------------------------------------------
// CWmListItemDrawer::NewL
// ---------------------------------------------------------
//
CWmListItemDrawer* CWmListItemDrawer::NewL(
        CWmPlugin& aWmPlugin,
        MTextListBoxModel* aTextListBoxModel,  
        const CFont* aFont,  
        CFormattedCellListBoxData* aFormattedCellData,
        CWmListBox* aListBox  )
    {
    CWmListItemDrawer* self = new ( ELeave ) CWmListItemDrawer(
            aWmPlugin,
            aTextListBoxModel,
            aFont,
            aFormattedCellData,
            aListBox );
    CleanupStack::PushL(self);   
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CWmListItemDrawer::CWmListItemDrawer
// ---------------------------------------------------------
//
CWmListItemDrawer::CWmListItemDrawer(
        CWmPlugin& aWmPlugin,
        MTextListBoxModel* aTextListBoxModel,  
        const CFont* aFont,  
        CFormattedCellListBoxData* aFormattedCellData,
        CWmListBox* aListBox )
    : CFormattedCellListBoxItemDrawer( aTextListBoxModel,aFont,aFormattedCellData ),
    iWmPlugin( aWmPlugin )
	{    
    iCellData = aFormattedCellData;
    iListBox = aListBox;
    iFont = aFont;
	}

// ---------------------------------------------------------
// CWmListItemDrawer::ConstructL
// ---------------------------------------------------------
//
void CWmListItemDrawer::ConstructL()
    {
    SetSkinEnabledL( ETrue );
    
    AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnMenuHswidget,
            iDefaultLogoImage,
            iDefaultLogoImageMask,
            iWmPlugin.ResourceLoader().IconFilePath(),
            EMbmWidgetmanagerQgn_menu_hswidget,
            EMbmWidgetmanagerQgn_menu_hswidget_mask
            );
	
    AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnIndiWmAdd,
            iAddWidgetBtnImage,
            iAddWidgetBtnMask,
            iWmPlugin.ResourceLoader().IconFilePath(),
            EMbmWidgetmanagerAdd_widget_button,
            EMbmWidgetmanagerAdd_widget_button_mask
            );	
    }

// ---------------------------------------------------------
// CWmListItemDrawer::~CWmListItemDrawer
// ---------------------------------------------------------
//
CWmListItemDrawer::~CWmListItemDrawer()
	{
    iCellData = NULL;
    iListBox = NULL;

    // dispose icons
    delete iDefaultLogoImage;
    delete iDefaultLogoImageMask;
    delete iAddWidgetBtnImage;
    delete iAddWidgetBtnMask;
	}

// ---------------------------------------------------------
// CWmListItemDrawer::CalculateRectangles
// ---------------------------------------------------------
//
void CWmListItemDrawer::CalculateRectangles( const TSize& aSize )
    {
	iCurrentSize = aSize;
	
    // LOGO RECT
    TSize logoSize = iListBox->LogoSize();
    TInt val = ( aSize.iHeight - KBottomMarginBig * 4 );
	TSize newSize( val, val );
    if ( logoSize != newSize ||
        logoSize.iHeight >= aSize.iHeight )
        {        
        logoSize = newSize;
        }
    iRelativeLogoRect.SetRect(
            TPoint( KLeftMarginBig, KTopMarginBig ),
            TSize( logoSize ) );

    // ADD BUTTON RECT
    TInt btnWidth = ( aSize.iWidth / 7 );
    TInt btnHeight = ( aSize.iHeight / 3 );
    TPoint btnLocation(
            ( aSize.iWidth - KRightMarginBig - btnWidth ),
            ( aSize.iHeight / 2  - btnHeight / 2 ) );
    iRelativeAddButtonRect.SetRect( btnLocation, TSize( btnWidth, btnHeight ) );

    // TEXT RECT
    iRelativeTitleRect.SetRect(
            KLeftMarginBig + logoSize.iWidth + KRightMarginBig,
            KTopMargin,
            aSize.iWidth - KRightMargin - btnWidth - KRightMargin,
            aSize.iHeight - KBottomMargin - KBottomMargin );

    // RESIZE BITMAPS
    AknIconUtils::SetSize( 
        iDefaultLogoImage, iRelativeLogoRect.Size(), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( 
        iDefaultLogoImageMask, iRelativeLogoRect.Size(), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( 
        iAddWidgetBtnImage, iRelativeAddButtonRect.Size(), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( 
        iAddWidgetBtnMask, iRelativeAddButtonRect.Size(), EAspectRatioNotPreserved );
    }

// ---------------------------------------------------------
// CWmListItemDrawer::DrawFrame
// ---------------------------------------------------------
//
void CWmListItemDrawer::DrawFrame( 
							const TRect& aOutterRect,
							const TRect& aInnerRect,
							const TAknsItemID& aID,
							const TAknsItemID& aCenterID ) const
	{	
    CWindowGc& gc = *Gc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::DrawFrame( skin, gc, aOutterRect, aInnerRect, 
                              aID, aCenterID );
	}

// ---------------------------------------------------------
// CWmListItemDrawer::DrawItemRect
// ---------------------------------------------------------
//
void CWmListItemDrawer::DrawItemRect( 
							const TRect& aActualItemRect ) const
    {
	CWindowGc& gc = *Gc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();    
    MAknsControlContext *cc = AknsDrawUtils::ControlContext( iListBox ); 
    AknsDrawUtils::Background( skin, cc, iListBox, gc, aActualItemRect );
    }
	
// ---------------------------------------------------------
// CWmListItemDrawer::DrawItem
// ---------------------------------------------------------
//
void CWmListItemDrawer::DrawItem( TInt aItemIndex, TPoint aItemRectPos, 
                            TBool /*aItemIsSelected*/, TBool /*aItemIsCurrent*/, 
                            TBool /*aViewIsEmphasized*/, TBool /*aViewIsDimmed*/ ) const
    {
    TInt w = ItemWidthInPixels( aItemIndex );
    TInt h = iListBox->ItemHeight();
    CWmWidgetData& wData = iListBox->WidgetData( aItemIndex );
    if ( iCurrentSize != TSize(w,h) )
        {
        // re-calculate all rects
        // note: members are manipulated here, although this method is const!
        const_cast<CWmListItemDrawer&>(*this).CalculateRectangles( TSize(w,h) );
        wData.SetLogoSize( iRelativeLogoRect.Size() );
        }
            
    TInt selectedIndex = iListBox->CurrentItemIndex();
    TBool isInFocus = ( selectedIndex == aItemIndex );
    TBool listFocused = iListBox->IsFocused();
    TRect itemRect = TRect( aItemRectPos, TSize( w, h ) );
    TRect fullRect = TRect( aItemRectPos, 
            TSize( iListBox->Rect().Width(), h ) );
    
	DrawItemRect( fullRect );

    TRect logoRect( iRelativeLogoRect );
    logoRect.Move( aItemRectPos );
    TRect addBtnRect( iRelativeAddButtonRect );
    addBtnRect.Move( aItemRectPos );
    TRect textRect( iRelativeTitleRect );
    textRect.Move( aItemRectPos );
    
    if ( isInFocus && listFocused )
        {
        TRect innerRect( itemRect );
        const TInt highlightOffset = 5;
        innerRect.Shrink( highlightOffset, highlightOffset );		
		DrawFrame( itemRect, innerRect, 
				iListBox->PressedDown() ? 
				KAknsIIDQsnFrListPressed : KAknsIIDQsnFrList,
				KAknsIIDQsnFrListCenter );
        }
    
    // Get graphics context which is used for drawing.
    CWindowGc& gc = *Gc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    // DRAW LOGO
    CFbsBitmap* bitmap = const_cast<CFbsBitmap*>(wData.LogoImage());
    CFbsBitmap* mask = const_cast<CFbsBitmap*>(wData.LogoImageMask());
    if ( !bitmap )
        {
        TRect bmpRect( TPoint(0,0), TSize( iDefaultLogoImage->SizeInPixels() ) );
        gc.BitBltMasked( logoRect.iTl, iDefaultLogoImage, 
                         bmpRect, iDefaultLogoImageMask, ETrue );        
        }
    else
        {
        TRect bmpRect( TPoint(0,0), TSize( bitmap->SizeInPixels() ) );        
        if ( bitmap && mask )
            {
            gc.BitBltMasked( logoRect.iTl, bitmap, 
                                bmpRect, mask, ETrue );            
            }
        else
            {
            gc.BitBlt( logoRect.iTl, bitmap, bmpRect );
            }
        }

    TRgb textColor;
    TAknsQsnTextColorsIndex index = ( isInFocus && listFocused )? 
                EAknsCIQsnTextColorsCG10 : EAknsCIQsnTextColorsCG6;

    AknsUtils::GetCachedColor( 
                    skin, textColor, KAknsIIDQsnTextColors, index );

    // DRAW TEXT
    gc.UseFont( iFont );
    gc.SetPenColor( textColor );
    gc.SetPenSize( TSize(2,2) );
    
    gc.DrawText( wData.Name(), textRect, 
           ( KTopTextMargin + iFont->FontMaxHeight() ), 
           CGraphicsContext::ELeft, KRightTextMargin );

    if ( wData.HsContentInfo().CanBeAdded() )
        {
        // DRAW ADD BUTTON
        gc.BitBltMasked( addBtnRect.iTl, iAddWidgetBtnImage,
                        TRect( TPoint(0,0), iAddWidgetBtnImage->SizeInPixels() ),
                        iAddWidgetBtnMask, ETrue );

        // DRAW BUTTON FOCUS
        if ( isInFocus && listFocused )
            {
            // draw focused frame around button if item is selected
            gc.SetPenStyle( CGraphicsContext::ESolidPen );
            gc.SetPenSize( TSize( KFocusFrameWidth, KFocusFrameWidth ) );
            //Draw the rounded rectangle
            gc.DrawRoundRect( addBtnRect, TSize( KFrameRoundRadius, KFrameRoundRadius ) );
            gc.SetBrushStyle( CGraphicsContext::ENullBrush );
            gc.SetPenStyle( CGraphicsContext::ENullPen );
            }
        }

    gc.SetPenSize(TSize(1,1));

    // DRAW DESCRIPTION TEXT
    HBufC* buf = wData.Description().Alloc();
    if ( buf )
        {
        TPtr bufPtr = buf->Des();
        bufPtr.Copy( wData.Description() );    
        TextUtils::ClipToFit( bufPtr, *iFont, textRect.Width() );
        gc.DrawText( bufPtr, textRect,
                    ( iFont->HeightInPixels() + textRect.Height()/2 ),
                    CGraphicsContext::ELeft, KRightTextMargin );    
        delete buf;
        }

    gc.DiscardFont();
    }

// ---------------------------------------------------------
// CWmListItemDrawer::DefaultLogoImage()
// ---------------------------------------------------------
//
const CFbsBitmap* CWmListItemDrawer::DefaultLogoImage()
    {
    return iDefaultLogoImage;
    }

// ---------------------------------------------------------
// CWmListItemDrawer::DefaultLogoMask()
// ---------------------------------------------------------
//
const CFbsBitmap* CWmListItemDrawer::DefaultLogoMask()
    {
    return iDefaultLogoImageMask;
    }

// ---------------------------------------------------------
// CWmListBox::NewL
// ---------------------------------------------------------
//
CWmListBox* CWmListBox::NewL(
        CWmPlugin& aWmPlugin,
        const TRect& aRect,
        const CCoeControl* aParent,
        TInt aFlags )
    {
    CWmListBox* self = new ( ELeave ) CWmListBox( aWmPlugin);
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aFlags );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmListBox::CWmListBox
// ---------------------------------------------------------
//
CWmListBox::CWmListBox( CWmPlugin& aWmPlugin ):
    CAknDouble2LargeStyleListBox(),
    iWmPlugin( aWmPlugin )
    {
    iFindPaneIsVisible = EFalse;
	iPressedDown = EFalse;
    }

// ---------------------------------------------------------
// CWmListBox::~CWmListBox
// ---------------------------------------------------------
//
CWmListBox::~CWmListBox()
    {
    iWidgetDatas.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CWmListBox::ConstructL
// ---------------------------------------------------------
//
void CWmListBox::ConstructL(
        const TRect& aRect,
        const CCoeControl* aParent,
        TInt aFlags )
    {
    CEikFormattedCellListBox::ConstructL( aParent, aFlags );
    SetContainerWindowL( *aParent );    

    // set model point to the widgets array
    Model()->SetItemTextArray( &iWidgetDatas );
    Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    SetRect( aRect );
    }

// ---------------------------------------------------------
// CWmListBox::WidgetData
// ---------------------------------------------------------
//
CWmWidgetData* CWmListBox::WidgetData()
    {
    TInt index = CurrentListBoxItemIndex();
    return (index>=0 ? iWidgetDatas[index] : NULL);
    }

// ---------------------------------------------------------
// CWmListBox::WidgetData
// ---------------------------------------------------------
//
CWmWidgetData& CWmListBox::WidgetData( TInt aItemIndex )
    {
    return *iWidgetDatas[ RealIndex( aItemIndex ) ];
    }

// ---------------------------------------------------------
// CWmListBox::AddWidgetDataL
// ---------------------------------------------------------
//
void CWmListBox::AddWidgetDataL( CWmWidgetData* aWidgetData )
    {
    aWidgetData->SetObserver( this );
    aWidgetData->SetLogoSize( LogoSize() );
    iWidgetDatas.InsertInOrderAllowRepeatsL( aWidgetData,
            SortOrder(EStoredOrder) );
    }

// ---------------------------------------------------------
// CWmListBox::RemoveWidgetData
// ---------------------------------------------------------
//
void CWmListBox::RemoveWidgetData( TInt aItemIndex )
    {
    TInt realIndex = RealIndex( aItemIndex );
    TBool current = ( aItemIndex == CurrentItemIndex() );
    // remove widget data
    CWmWidgetData* data = iWidgetDatas[realIndex];
    iWidgetDatas.Remove( realIndex );
    delete data;

    // reorganise
    TRAP_IGNORE(
        AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
            this, realIndex, current ) );
    }

// ---------------------------------------------------------
// CWmListBox::RedrawItem
// ---------------------------------------------------------
//
void CWmListBox::RedrawItem( TInt aItemIndex )
    {
    View()->DrawItem( aItemIndex );
    }

// ---------------------------------------------------------
// CWmListBox::SetSortOrderL
// ---------------------------------------------------------
//
void CWmListBox::SetSortOrderL( TSortOrder aOrder )
    {
    // now sort the existing data
    iWidgetDatas.Sort( SortOrder( aOrder ) );
    DrawNow();
    }

// ---------------------------------------------------------
// CWmListBox::CreateItemDrawerL
// ---------------------------------------------------------
//
void CWmListBox::CreateItemDrawerL()
    {
    CFormattedCellListBoxData* cellData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer = CWmListItemDrawer::NewL(
            iWmPlugin,
            Model(),
            iEikonEnv->DenseFont(),
            cellData,
            this );
    CleanupStack::Pop(); // cellData
    }

// ---------------------------------------------------------
// CWmListBox::HandleLayoutChanged
// ---------------------------------------------------------
//
void CWmListBox::HandleLayoutChanged()
    {
	TSize ls = LogoSize();
    for ( TInt i=0; i<iWidgetDatas.Count(); i++)
        {
        iWidgetDatas[i]->ReCreateLogo( ls );
        }
    }

// ---------------------------------------------------------
// CWmListBox::HandleWidgetDataChanged
// ---------------------------------------------------------
//
void CWmListBox::HandleWidgetDataChanged( CWmWidgetData* aWidgetData )
    {
    if ( !iFindPaneIsVisible )
        {
        // spontaneous change in the model. Cause table to redraw
        TInt index = iWidgetDatas.Find( aWidgetData );
        if ( index >= 0 )
            {
            // redraw item.
            RedrawItem( RealIndex( index ) );			
            }
        }
    }

// ---------------------------------------------------------
// CWmListBox::SortOrder
// ---------------------------------------------------------
//
TLinearOrder<CWmWidgetData> CWmListBox::SortOrder( TSortOrder aOrder )
    {
    if ( aOrder == EStoredOrder )
        {
        return TLinearOrder<CWmWidgetData>(
                CWmWidgetData::CompareByPersistentWidgetOrder );
        }
    else
        {
        return TLinearOrder<CWmWidgetData>(
                CWmWidgetData::CompareByName );
        }
    }

// ---------------------------------------------------------
// CWmListBox::CurrentListBoxItemIndex
// ---------------------------------------------------------
//
TInt CWmListBox::CurrentListBoxItemIndex()
    {
    TInt index( KErrNotFound );
    if ( Model()->NumberOfItems() > 0 )
        {
        index = CurrentItemIndex();
        }
    return RealIndex( index );
    }

// ---------------------------------------------------------
// CWmListBox::RealIndex
// ---------------------------------------------------------
//
TInt CWmListBox::RealIndex( TInt aIndex )
    {
    TInt realIndex = aIndex;
    if ( iFindPaneIsVisible && aIndex >= 0 )
        {
        realIndex = static_cast<CAknFilteredTextListBoxModel*>(Model())->Filter()
                ->FilteredItemIndex( aIndex );
        }
    return realIndex;    
    }

// ---------------------------------------------------------
// CWmListBox::SizeChanged
// ---------------------------------------------------------
//
void CWmListBox::SizeChanged()
    {
    CAknsListBoxBackgroundControlContext* context =     
            static_cast<CAknsListBoxBackgroundControlContext*>(
                    ItemDrawer()->FormattedCellData()->
                        SkinBackgroundContext() );
    if ( context ) context->SetRect( Rect() );
    CAknDouble2LargeStyleListBox::SizeChanged();
    }

// ---------------------------------------------------------
// CWmListBox::Draw
// ---------------------------------------------------------
//
void CWmListBox::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc(); 
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, Rect() );
    CAknDouble2LargeStyleListBox::Draw( aRect );
    }

// ---------------------------------------------------------
// CWmListBox::HandlePointerEventL
// ---------------------------------------------------------
//
void CWmListBox::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
	// Check if pointer is down ( needed for drawing correct hightlight frame )
	if ( Rect().Contains( aPointerEvent.iPosition ) )
		{
		if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
			{
			iPressedDown = ETrue;
			RedrawItem( CurrentListBoxItemIndex() );
			}
		else if( aPointerEvent.iType == TPointerEvent::EButton1Up )
			{
			iPressedDown = EFalse;
			RedrawItem( CurrentListBoxItemIndex() );
			}
		}

	CAknDouble2LargeStyleListBox::HandlePointerEventL( aPointerEvent );	
    }

// ---------------------------------------------------------
// CWmListBox::LogoSize
// ---------------------------------------------------------
//
TSize CWmListBox::LogoSize()
    {
    TSize size( KLogoWidth,KLogoHeight );
    TInt val = ( ItemHeight() - KBottomMarginBig * 4 );
    if ( val > 0 )
        {
        size.SetSize( val, val );     
        }
    return size;
    }

// ---------------------------------------------------------
// CWmListBox::DefaultLogo
// ---------------------------------------------------------
//
const CFbsBitmap* CWmListBox::DefaultLogo()
    {
    CWmListItemDrawer* itemDrawer = 
                    static_cast <CWmListItemDrawer*>( iItemDrawer );
    if ( itemDrawer )
        {
        return itemDrawer->DefaultLogoImage();
        }
    return NULL;
    }

// ---------------------------------------------------------
// CWmListBox::DefaultMask
// ---------------------------------------------------------
//
const CFbsBitmap* CWmListBox::DefaultMask()
    {
    CWmListItemDrawer* itemDrawer = 
                    static_cast <CWmListItemDrawer*>( iItemDrawer );
    if ( itemDrawer )
        {
        return itemDrawer->DefaultLogoMask();
        }
    return NULL;
    }

// End of File

