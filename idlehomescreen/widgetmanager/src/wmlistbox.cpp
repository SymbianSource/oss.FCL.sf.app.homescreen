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
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <aknlayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout.lag>
#include <AknIconArray.h>
#include <gulicon.h>
#include <widgetmanagerview.rsg>
#include <widgetmanager.mbg>

#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmlistbox.h"
#include "wmwidgetdata.h"
#include "wmwidgetorderdata.h"

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
    
    // This is temporary fix for ou1cimx1#228810
    // Can be removed when avkon provides real fix for this error.
    // Currently forever loop in CFormattedCellListBoxData::DrawFormattedSimple
    // never exits if there is no iconarray and name contains tab+digit which  
    // confuses listbox e.g. considering name as icon index 
    CArrayPtr<CGulIcon>* dummyArray = new( ELeave ) CAknIconArray(2);
    CleanupStack::PushL(dummyArray);
    
    CFbsBitmap* dummyBmp = new(ELeave) CFbsBitmap;
    CleanupStack::PushL( dummyBmp );
    User::LeaveIfError( dummyBmp->Create( TSize(0,0), EColor256 ) );
    CGulIcon* dummyIcon = CGulIcon::NewL( dummyBmp );
    CleanupStack::Pop( dummyBmp ); //ownership transfered
    CleanupStack::PushL( dummyIcon );

    CFbsBitmap* dummyBmp2 = new(ELeave) CFbsBitmap;
    CleanupStack::PushL( dummyBmp2 );
    User::LeaveIfError( dummyBmp2->Create( TSize(0,0), EColor256 ) );
    CGulIcon* dummyIcon2 = CGulIcon::NewL( dummyBmp2 );
    CleanupStack::Pop( dummyBmp2 ); //ownership transfered
    CleanupStack::PushL( dummyIcon2 );
    
    dummyArray->AppendL(dummyIcon);
    dummyArray->AppendL(dummyIcon2);
    ColumnData()->SetIconArray(dummyArray);
    CleanupStack::Pop( dummyIcon2 );
    CleanupStack::Pop( dummyIcon );
    CleanupStack::Pop( dummyArray );
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
	}

// ---------------------------------------------------------
// CWmListItemDrawer::ResizeDefaultBitmaps
// ---------------------------------------------------------
//
void CWmListItemDrawer::ResizeDefaultBitmaps()
    {
	TAknWindowLineLayout logoPane = 
	          AknLayoutScalable_Apps::listrow_wgtman_pane_g1().LayoutLine();

    TAknLayoutRect layoutRect;
    TRect rect = TRect( ItemCellSize() );
    layoutRect.LayoutRect( rect, logoPane );
    
    // RESIZE DEFAULT LOGO BITMAPS ACCORDING TO LAYOUT
    TSize size = layoutRect.Rect().Size();
    AknIconUtils::SetSize( 
        iDefaultLogoImage, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( 
        iDefaultLogoImageMask, size, EAspectRatioPreserved );
    }
	

// ---------------------------------------------------------
// CWmListItemDrawer::UpdateItemHeight
// ---------------------------------------------------------
//
void CWmListItemDrawer::UpdateItemHeight()
    {
    // sets item height according to layout
    TAknWindowLineLayout listPane = AknLayoutScalable_Apps
        ::list_wgtman_pane().LayoutLine();
    TAknLayoutRect listPaneRect;
    listPaneRect.LayoutRect( iListBox->Rect(), listPane );
    TAknWindowLineLayout listRowPane = AknLayoutScalable_Apps
        ::listrow_wgtman_pane().LayoutLine();
    TAknLayoutRect listRowPaneRect;
    listRowPaneRect.LayoutRect( listPaneRect.Rect(), listRowPane );
    iListBox->View()->SetItemHeight( listRowPaneRect.Rect().Height() );
    iListBox->View()->ItemDrawer()->SetItemCellSize( listRowPaneRect.Rect().Size() );
    }

// ---------------------------------------------------------
// CWmListItemDrawer::DrawItem
// ---------------------------------------------------------
//
void CWmListItemDrawer::DrawItem( TInt aItemIndex, TPoint aItemRectPos, 
                            TBool /*aItemIsSelected*/, TBool aItemIsCurrent, 
                            TBool /*aViewIsEmphasized*/, TBool aViewIsDimmed ) const
    {
    TSize cellSize = ItemCellSize();
    
    CWmWidgetData& wData = iListBox->WidgetData( aItemIndex );
    // Get graphics context which is used for drawing.
    CWindowGc& gc = *Gc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TBool highlightEnabled = !( iListBox->ItemDrawer()->Flags() & 
            CListItemDrawer::ESingleClickDisabledHighlight );
    TBool listFocused = ((iListBox->IsFocused() && !aViewIsDimmed) ? ETrue : EFalse);
    TRect itemRect = TRect( aItemRectPos, cellSize );
    
    CFormattedCellListBoxItemDrawer::DrawEmptyItem( 
                            aItemIndex, aItemRectPos, aViewIsDimmed );
    
    if ( aItemIsCurrent && listFocused && highlightEnabled )
        {
        TRect innerRect( itemRect );
        const TInt highlightOffset = 2;
        innerRect.Shrink( highlightOffset, highlightOffset );
		
        AknsDrawUtils::DrawFrame( skin,
                                  gc,
                                  itemRect,
                                  innerRect,
                                  KAknsIIDQsnFrList,
                                  KAknsIIDQsnFrListCenter );
        }
    
    // DRAW LOGO
    CFbsBitmap* bitmap = const_cast<CFbsBitmap*>(wData.LogoImage());
    CFbsBitmap* mask = const_cast<CFbsBitmap*>(wData.LogoImageMask());
    TAknLayoutRect logoLayout;
    logoLayout.LayoutRect( itemRect,
             AknLayoutScalable_Apps::listrow_wgtman_pane_g1().LayoutLine() );

    if ( !bitmap && !wData.IsPrepairingLogo() )
        {
        logoLayout.DrawImage( gc, iDefaultLogoImage, iDefaultLogoImageMask );
        }
    else if( bitmap && mask )
        {
        logoLayout.DrawImage( gc, bitmap, mask );
        }

    // DRAW NAME
    TRgb textColor;
    TAknsQsnTextColorsIndex index =
        ( aItemIsCurrent && listFocused && highlightEnabled )? 
                EAknsCIQsnTextColorsCG10 : EAknsCIQsnTextColorsCG6;

    AknsUtils::GetCachedColor( 
                    skin, textColor, KAknsIIDQsnTextColors, index );

    TAknTextLineLayout titleTextLayout;
    if ( !wData.IsUninstalling() )
        {
        // centered 1 row layout 
        titleTextLayout = 
              AknLayoutScalable_Apps::listrow_wgtman_pane_t1(1).LayoutLine();
        }
    else
        {
        // 2 row layout for uninstalling
        titleTextLayout = 
                AknLayoutScalable_Apps::listrow_wgtman_pane_t1(0).LayoutLine();
        }

    TAknLayoutText textLayoutTitle;
    textLayoutTitle.LayoutText( itemRect, titleTextLayout );
    textLayoutTitle.DrawText( gc, wData.Name(), ETrue, textColor );
    
    if ( wData.IsUninstalling() )
        {
        // DRAW UNINSTALL ANIMATION
        TAknLayoutRect animationLayout;
        animationLayout.LayoutRect( itemRect,
             AknLayoutScalable_Apps::wait_bar_pane_cp09().LayoutLine() );
        TSize size = animationLayout.Rect().Size();
        CFbsBitmap* animBitmap = 
                const_cast<CFbsBitmap*>(wData.AnimationBitmap( size ));
        CFbsBitmap* animMask = 
                const_cast<CFbsBitmap*>(wData.AnimationMask( size ));
        if ( animBitmap && animMask )
            {
            animationLayout.DrawImage( gc, animBitmap, animMask );
            }
        }
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
    ResizeDefaultBitmaps();
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
	iLogoSize = TSize( 0, 0);
    }

// ---------------------------------------------------------
// CWmListBox::~CWmListBox
// ---------------------------------------------------------
//
CWmListBox::~CWmListBox()
    {
    iVisibleWidgetArray.ResetAndDestroy();
    iOrderDataArray.ResetAndDestroy();
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
    Model()->SetItemTextArray( &iVisibleWidgetArray );
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
    return (index>=0 ? iVisibleWidgetArray[index] : NULL);
    }

// ---------------------------------------------------------
// CWmListBox::WidgetData
// ---------------------------------------------------------
//
CWmWidgetData& CWmListBox::WidgetData( TInt aItemIndex )
    {
    return *iVisibleWidgetArray[ RealIndex( aItemIndex ) ];
    }

// ---------------------------------------------------------
// CWmListBox::AddWidgetDataL
// ---------------------------------------------------------
//
void CWmListBox::AddWidgetDataL( CWmWidgetData* aWidgetData,
        TBool aRedraw )
    {
    if ( aWidgetData )
        {
        aWidgetData->SetObserver( this );
        iVisibleWidgetArray.InsertInOrderAllowRepeatsL( aWidgetData,
            SortOrder(EStoredOrder) );
        if ( aRedraw ) { HandleItemAdditionL(); }
        }
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
    CWmWidgetData* data = iVisibleWidgetArray[realIndex];
    iVisibleWidgetArray.Remove( realIndex );
    // reorganise
    TRAP_IGNORE(
        AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
            this, realIndex, current ) );

    // Remove item from order array
    for ( TInt i = 0; i < iOrderDataArray.Count(); i++ )
        {
        CWmWidgetOrderData* order = iOrderDataArray[i];
        if ( order->EqualsTo( data->Uid(), data->PublisherId() ) )
            {
            iOrderDataArray.Remove( i );
            delete order;
            order = NULL;
            break;
            }
        }
    
    // delete now
    delete data;
    data = NULL;
    }

// ---------------------------------------------------------
// CWmListBox::RedrawItem
// ---------------------------------------------------------
//
void CWmListBox::RedrawItem( TInt aItemIndex )
    {
    if ( aItemIndex >= 0 &&
        aItemIndex < Model()->NumberOfItems() )
        {
        View()->DrawItem( aItemIndex );
        }
    }

// ---------------------------------------------------------
// CWmListBox::SetSortOrderL
// ---------------------------------------------------------
//
void CWmListBox::DoSortToVisibleArray( TSortOrder aOrder )
    {
    // now sort the existing data
    iVisibleWidgetArray.Sort( SortOrder( aOrder ) );
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
    iLogoSize = TSize( 0, 0);
	iLogoSize = LogoSize();
    for ( TInt i=0; i<iVisibleWidgetArray.Count(); i++)
        {
        iVisibleWidgetArray[i]->ReCreateLogo( iLogoSize );
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
        TInt index = iVisibleWidgetArray.Find( aWidgetData );
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
        itemDrawer->UpdateItemHeight();
        itemDrawer->ResizeDefaultBitmaps();
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
// CWmListBox::LogoSize
// ---------------------------------------------------------
//
TSize CWmListBox::LogoSize()
    {
    TSize size( iLogoSize );    
    if ( size.iWidth == 0 || size.iHeight == 0 )
        {
        TAknLayoutRect rowLayoutRect;
        rowLayoutRect.LayoutRect( Rect(),
                AknLayoutScalable_Apps::listrow_wgtman_pane().LayoutLine() );
        TAknLayoutRect logoLayoutRect;
        logoLayoutRect.LayoutRect( rowLayoutRect.Rect(),
                AknLayoutScalable_Apps::listrow_wgtman_pane_g1().LayoutLine() );
        size = logoLayoutRect.Rect().Size();
        iLogoSize = size;
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

// ---------------------------------------------------------
// CWmListBox::SortOrderToOrderData
// ---------------------------------------------------------
//
TLinearOrder<CWmWidgetOrderData> CWmListBox::SortOrderToOrderData( TSortOrder aOrder )
    {
    if ( aOrder == EStoredOrder )
        {
        return TLinearOrder<CWmWidgetOrderData>(
                CWmWidgetOrderData::CompareByPersistentWidgetOrder );
        }
    else
        {
        return TLinearOrder<CWmWidgetOrderData>(
                CWmWidgetOrderData::CompareByName );
        }
    }

// ---------------------------------------------------------
// CWmListBox::SetSortOrderToOrderDataL
// ---------------------------------------------------------
//
void CWmListBox::DoSortToOrderData( TSortOrder aOrder )
    {
    iOrderDataArray.Sort( SortOrderToOrderData( aOrder ) );
    }

// ---------------------------------------------------------
// CWmListBox::AddWidgetDataL
// ---------------------------------------------------------
//
void CWmListBox::AddOrderDataL( CWmWidgetOrderData* aOrderData )
    {
    if ( aOrderData )
        {
        iOrderDataArray.InsertInOrderAllowRepeatsL( aOrderData,
                SortOrderToOrderData( EStoredOrder ) );
        }
    }

// ---------------------------------------------------------
// CWmListBox::OrderData
// ---------------------------------------------------------
//
CWmWidgetOrderData* CWmListBox::OrderData( TInt aItemIndex )
    {
    return iOrderDataArray[ aItemIndex ];
    }
// End of File

