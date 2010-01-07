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
#include <aknlayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout.lag>
#include <widgetmanagerview.rsg>
#include <widgetmanager.mbg>
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmlistbox.h"
#include "wmwidgetdata.h"

// CONSTANTS

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
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::CreateIconL(
            skin,
            KAknsIIDQgnMenuHswidget,
            iDefaultLogoImage,
            iDefaultLogoImageMask,
            iWmPlugin.ResourceLoader().IconFilePath(),
            EMbmWidgetmanagerQgn_menu_hswidget,
            EMbmWidgetmanagerQgn_menu_hswidget_mask
            );
	
    AknsUtils::CreateColorIconL(
            skin,
            KAknsIIDQgnIndiWmAdd,
            KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG6,
            iAddWidgetBtnImage,
            iAddWidgetBtnMask,
            iWmPlugin.ResourceLoader().IconFilePath(),
            EMbmWidgetmanagerAdd_widget_button,
            EMbmWidgetmanagerAdd_widget_button_mask,
            KRgbBlack );

    AknsUtils::CreateColorIconL(
            skin,
            KAknsIIDQgnIndiWmAdd,
            KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG10,
            iAddWidgetBtnHighlightImage,
            iAddWidgetBtnHighlightMask,
            iWmPlugin.ResourceLoader().IconFilePath(),
            EMbmWidgetmanagerAdd_widget_button,
            EMbmWidgetmanagerAdd_widget_button_mask,
            KRgbWhite );
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
    delete iAddWidgetBtnHighlightImage;
    delete iAddWidgetBtnHighlightMask;
	}

// ---------------------------------------------------------
// CWmListItemDrawer::ResizeDefaultBitmaps
// ---------------------------------------------------------
//
void CWmListItemDrawer::ResizeDefaultBitmaps()
    {
	TAknWindowLineLayout logoPane = 
	          AknLayoutScalable_Apps::listrow_wgtman_pane_g1().LayoutLine();
    TAknWindowLineLayout addPane = 
              AknLayoutScalable_Apps::listrow_wgtman_pane_g2().LayoutLine();
    TAknLayoutRect layoutRect;
    TRect rect = TRect( ItemCellSize() );
    layoutRect.LayoutRect( rect, logoPane );
    
    // RESIZE DEFAULT LOGO BITMAPS ACCORDING TO LAYOUT
    TSize size = layoutRect.Rect().Size();
    AknIconUtils::SetSize( 
        iDefaultLogoImage, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( 
        iDefaultLogoImageMask, size, EAspectRatioPreserved );
    
    // RESIZE ADD BTN BITMAPS ACCORDING TO LAYOUT
    layoutRect.LayoutRect( rect, addPane );
    size = layoutRect.Rect().Size();
    AknIconUtils::SetSize( 
        iAddWidgetBtnImage, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( 
        iAddWidgetBtnMask, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( 
        iAddWidgetBtnHighlightImage, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( 
        iAddWidgetBtnHighlightMask, size, EAspectRatioPreserved );
    }
	
// ---------------------------------------------------------
// CWmListItemDrawer::DrawItem
// ---------------------------------------------------------
//
void CWmListItemDrawer::DrawItem( TInt aItemIndex, TPoint aItemRectPos, 
                            TBool aItemIsSelected, TBool aItemIsCurrent, 
                            TBool aViewIsEmphasized, TBool aViewIsDimmed ) const
    {
    TSize cellSize = ItemCellSize();
    
    CWmWidgetData& wData = iListBox->WidgetData( aItemIndex );
    // Get graphics context which is used for drawing.
    CWindowGc& gc = *Gc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TInt selectedIndex = iListBox->CurrentItemIndex();
    TBool isInFocus = ( selectedIndex == aItemIndex );
    TBool listFocused = iListBox->IsFocused();
    TRect itemRect = TRect( aItemRectPos, cellSize );

    if ( isInFocus && listFocused )
        {
        // force baseclass to draw highlight and animation for this item
        CFormattedCellListBoxItemDrawer::DrawItemText( 
                aItemIndex, itemRect,
                aItemIsCurrent, aViewIsEmphasized,
                ( isInFocus || aItemIsSelected ) );
		
		if ( iListBox->PressedDown() )
			{
			TRect innerRect( itemRect );
    	    const TInt highlightOffset = 3;
	        innerRect.Shrink( highlightOffset, highlightOffset );
			AknsDrawUtils::DrawFrame( skin,
                                      gc,
                                      itemRect,
                                      innerRect,
                                      KAknsIIDQsnFrListPressed,
                                      KAknsIIDQsnFrListCenterPressed );
			}
        }
    else
        {
        // to prevent item staying highlighted when list isn't focused.
        CFormattedCellListBoxItemDrawer::DrawEmptyItem( 
                                aItemIndex, aItemRectPos, aViewIsDimmed );
        }

    
    // DRAW LOGO
    CFbsBitmap* bitmap = const_cast<CFbsBitmap*>(wData.LogoImage());
    CFbsBitmap* mask = const_cast<CFbsBitmap*>(wData.LogoImageMask());
    TAknLayoutRect logoLayout;
    logoLayout.LayoutRect( itemRect,
             AknLayoutScalable_Apps::listrow_wgtman_pane_g1().LayoutLine() );

    if ( !bitmap )
        {
        logoLayout.DrawImage( gc, iDefaultLogoImage, iDefaultLogoImageMask );
        }
    else
        {
        logoLayout.DrawImage( gc, bitmap, mask );
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

    TAknTextLineLayout titleTextLayout = 
              AknLayoutScalable_Apps::listrow_wgtman_pane_t1().LayoutLine();
    TAknTextLineLayout descTextLayout =
            AknLayoutScalable_Apps::listrow_wgtman_pane_t2().LayoutLine();

    TAknLayoutText textLayout;
    textLayout.LayoutText( itemRect, titleTextLayout );
    textLayout.DrawText( gc, wData.Name(), ETrue, textColor );
    
    if ( wData.HsContentInfo().CanBeAdded() )
        {
        // DRAW ADD BUTTON
        TAknLayoutRect addButtonLayout;
        addButtonLayout.LayoutRect( itemRect,
                AknLayoutScalable_Apps::listrow_wgtman_pane_g2().LayoutLine() );
        if ( isInFocus && listFocused )
            {
            addButtonLayout.DrawImage( gc,                    
                    iAddWidgetBtnHighlightImage, iAddWidgetBtnHighlightMask );
            }
        else
            {
            addButtonLayout.DrawImage( gc, 
                    iAddWidgetBtnImage, iAddWidgetBtnMask );
            }        
        }

    gc.SetPenSize(TSize(1,1));

    // DRAW DESCRIPTION TEXT
    textLayout.LayoutText( itemRect, descTextLayout );
    textLayout.DrawText( gc, wData.Description(), ETrue, textColor );
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
// CWmListItemDrawer::HandleSkinChanged
// ---------------------------------------------------------
//
void CWmListItemDrawer::HandleSkinChanged()
    {
    CFbsBitmap* addWidgetBtnImage( NULL );
    CFbsBitmap* addWidgetBtnMask( NULL );
    CFbsBitmap* addWidgetBtnHighlightImage( NULL );
    CFbsBitmap* addWidgetBtnHighlightMask( NULL );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    TRAPD( err, 
        AknsUtils::CreateColorIconLC(
                skin,
                KAknsIIDQgnIndiWmAdd,
                KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6,
                addWidgetBtnImage,
                addWidgetBtnMask,
                iWmPlugin.ResourceLoader().IconFilePath(),
                EMbmWidgetmanagerAdd_widget_button,
                EMbmWidgetmanagerAdd_widget_button_mask,
                KRgbBlack );
        
        AknsUtils::CreateColorIconLC(
                skin,
                KAknsIIDQgnIndiWmAdd,
                KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG10,
                addWidgetBtnHighlightImage,
                addWidgetBtnHighlightMask,
                iWmPlugin.ResourceLoader().IconFilePath(),
                EMbmWidgetmanagerAdd_widget_button,
                EMbmWidgetmanagerAdd_widget_button_mask,
                KRgbWhite );
        
        CleanupStack::Pop( 4 ); // pop icons
        );
    
    if ( KErrNone == err )
        {
        // delete old icons
        delete iAddWidgetBtnImage;
        iAddWidgetBtnImage = NULL;
        delete iAddWidgetBtnMask;
        iAddWidgetBtnMask = NULL;
        delete iAddWidgetBtnHighlightImage;
        iAddWidgetBtnHighlightImage = NULL;
        delete iAddWidgetBtnHighlightMask;
        iAddWidgetBtnHighlightMask = NULL;
        
        // transfer ownership
        iAddWidgetBtnImage = addWidgetBtnImage;
        addWidgetBtnImage = NULL;
        iAddWidgetBtnMask = addWidgetBtnMask;
        addWidgetBtnMask = NULL;
        iAddWidgetBtnHighlightImage = addWidgetBtnHighlightImage;
        addWidgetBtnHighlightImage = NULL;
        iAddWidgetBtnHighlightMask = addWidgetBtnHighlightMask;
        addWidgetBtnHighlightMask = NULL;
        
        // resize new created icons
        ResizeDefaultBitmaps();
        }
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

    // set empty-text to null
    View()->SetListEmptyTextL( KNullDesC );

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
    HandleItemAdditionL();
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
// CWmListBox::HandleSkinChanged
// ---------------------------------------------------------
//
void CWmListBox::HandleSkinChanged()
    {
    CWmListItemDrawer* itemDrawer = 
                        static_cast <CWmListItemDrawer*>( iItemDrawer );
    if ( itemDrawer )
        {
        itemDrawer->HandleSkinChanged();
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
    else
        {
        // redraw everything
        Parent()->DrawDeferred();
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
    CAknDouble2LargeStyleListBox::SizeChanged();
    
    CWmListItemDrawer* itemDrawer = 
                    static_cast <CWmListItemDrawer*>( iItemDrawer );
    if ( itemDrawer )
        {
        return itemDrawer->ResizeDefaultBitmaps();
        }
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
    TAknLayoutRect rowLayoutRect;
    rowLayoutRect.LayoutRect( Rect(),
            AknLayoutScalable_Apps::listrow_wgtman_pane().LayoutLine() );
    TAknLayoutRect logoLayoutRect;
    logoLayoutRect.LayoutRect( rowLayoutRect.Rect(),
            AknLayoutScalable_Apps::listrow_wgtman_pane_g1().LayoutLine() );
    return logoLayoutRect.Rect().Size();
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

