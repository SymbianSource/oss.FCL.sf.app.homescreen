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
* Description:  Taskswitcher Fast Swap area UI
 *
*/


#include <gulicon.h>
#include <eikenv.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <AknIconArray.h>
#include <aknstyluspopupmenu.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <taskswitcher.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlists.h>
#include <touchfeedback.h>
#include <akntransitionutils.h>
#include <akntranseffect.h>
#include <aknlongtapanimation.h>

#include "tsfastswaparea.h"
#include "tsapplogging.h"
#include "tsfswclient.h"
#include "tsappui.h"
#include "tsdatachangeobserver.h"
#include "tseventcontroler.h"
#include "tsappview.h"

/** command ids for the fsw popup */
enum TPopupCommands
    {
    EFswCmdClose = 10000,
    EFswCmdCloseAll
    };

/** Number of closable applications, to show "close all" option. */
const TInt KTsMaxClosableApps = 2;

/** Default grid item to highlight.*/
const TInt KItemToHighlight = 2;

const TInt KAppKeyTypeShort = 1;
const TInt KAppKeyTypeLong = 2;

const TInt KLayoutItemCount = 4;

const TInt KRedrawTime = 250000; // 0.25 sec
const TInt KRedrawTimeForLayoutSwitch = 700000; // 0.7 sec
const TInt KHighlighActivationTime = 100000; // 100 ms
const TInt KUpdateGridTime = 0; // imediately
const TInt KOrientationSwitchTime = 1000000; // 1 sec
const TInt KLongTapAnimationInitTime = 150000; // 0.15 sec
const TInt KLongTapAnimationTimeout = 1000000; // 1 sec

const TInt KMaxGranularity = 4;

const TUid KTsMenuUid = { 0x101f4cd2 };
const TUid KTsHomescreenUid = { 0x102750f0 };

// -----------------------------------------------------------------------------
// CTsFastSwapArea::NewL
// -----------------------------------------------------------------------------
//
CTsFastSwapArea* CTsFastSwapArea::NewL( const TRect& aRect,
    CCoeControl& aParent, CTsDeviceState& aDeviceState, 
    CTsEventControler& aEventHandler )
    {
    CTsFastSwapArea* self = CTsFastSwapArea::NewLC(aRect,
            aParent, aDeviceState, aEventHandler);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::NewLC
// -----------------------------------------------------------------------------
//
CTsFastSwapArea* CTsFastSwapArea::NewLC( const TRect& aRect,
    CCoeControl& aParent, CTsDeviceState& aDeviceState,
    CTsEventControler& aEventHandler)
    {
    CTsFastSwapArea* self = new (ELeave) CTsFastSwapArea(
            aParent, aDeviceState, aEventHandler);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::CTsFastSwapArea
// -----------------------------------------------------------------------------
//
CTsFastSwapArea::CTsFastSwapArea(CCoeControl& aParent,
    CTsDeviceState& aDeviceState,
    CTsEventControler& aEventHandler) :
    iParent(aParent), iDeviceState(aDeviceState), iEvtHandler(aEventHandler),
    iIgnoreLayoutSwitch(EFalse), iWidgetClosingCount(0), iLongTapAnimationRunning(EFalse)
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::~CTsFastSwapArea
// -----------------------------------------------------------------------------
//
CTsFastSwapArea::~CTsFastSwapArea()
    {
    iArray.ResetAndDestroy();
    iIsClosing.Close();
    delete iGrid;
    delete iFSClient;
    delete iPopup;
    delete iHighlightTimer;
    delete iRedrawTimer;
    delete iUpdateGridTimer;
    delete iOrientationSignalTimer;
    delete iLongTapAnimation;
    delete iLongTapAnimationTimer;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::ConstructL( const TRect& aRect )
    {
    SetContainerWindowL( iParent );

    SetRect( aRect );

    // setup grid
    ReCreateGridL();

    // create stylus popup instance
    PreCreatePopupL();

    // connect to fsw server
    iFSClient = CTsFswClient::NewL();
    
    iEvtHandler.ReInitPhysicsL(GridWorldSize(), ViewSize(), ETrue);
    
    // add self to device state observer
    iDeviceState.AddObserverL( *this, EDeviceType );
    
    iHighlightTimer = new (ELeave) CTsFastSwapTimer( *this );
    iHighlightTimer->ConstructL();
    
    iRedrawTimer = new (ELeave) CTsFastSwapTimer( *this );
    iRedrawTimer->ConstructL();

    iUpdateGridTimer = new (ELeave) CTsFastSwapTimer( *this );
    iUpdateGridTimer->ConstructL();
    
    iOrientationSignalTimer = new (ELeave) CTsFastSwapTimer( *this ); 
    iOrientationSignalTimer->ConstructL();
    
    iLongTapAnimationTimer = new (ELeave) CTsFastSwapTimer( *this ); 
    iLongTapAnimationTimer->ConstructL();
    
    iActivateOnPointerRelease = TPoint();
    iHandlePointerCandidate = EFalse;
    
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ReCreateGridL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::ReCreateGridL()
    {
    MCoeControlObserver* obs = NULL;
    TBool wasHighlight = iDeviceState.DeviceType() == 
                             CTsDeviceState::EHybrid ? ETrue : EFalse;
    if ( iGrid )
        {
        obs = iGrid->Observer();
        iDeviceState.RemoveObserver(*iGrid);
        wasHighlight = iGrid->IsHighlightVisible();
        delete iGrid;
        iGrid = NULL;
        }
    
    iGrid = new( ELeave ) CTsFastSwapGrid;
    iGrid->ConstructL( this );
	iGrid->DisableSingleClick(ETrue);//enables highlight on pointer
    iDeviceState.AddObserverL(*iGrid, MTsDeviceStateObserver::ESkin);
    
    AknListBoxLayouts::SetupStandardGrid( *iGrid );
    
    // Setup layout
    LayoutGridL();
    
    if( wasHighlight )
        {
        iGrid->ShowHighlight();
        }
    else
        {
        iGrid->HideHighlight();
        }
    
    // Setup empty text
    HBufC* text = StringLoader::LoadLC( R_TS_FSW_NO_APPS );
    iGrid->SetEmptyGridTextL( *text );
    CleanupStack::PopAndDestroy( text );
    
    // Setup grid observers
    if ( obs )
        {
        iGrid->SetObserver( obs );
        }
    iGrid->SetListBoxObserver(this);
    iGrid->SetFastSwapGridObserver(this);
    iGrid->SetContainerWindowL(*this);
	
    // Make sure that there is an ActivateL call even when we are not
    // called from ConstructL. (in order to have the grid's parent ptr set properly)
    ActivateL();
    }


// --------------------------------------------------------------------------
// CTsFastSwapArea::LayoutGridL
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::LayoutGridL()
    {
    RArray<TAknLayoutRect> rects;
    CleanupClosePushL(rects);
    rects.ReserveL(KLayoutItemCount);
    GetFastSwapAreaRects(rects);
    TAknLayoutRect gridAppPane = rects[0];
    TAknLayoutRect gridItem = rects[1];
    TAknLayoutRect gridImage = rects[2];
    TAknLayoutRect gridNextItem = rects[3];
    CleanupStack::PopAndDestroy(&rects);
    
    iGrid->SetRect(gridAppPane.Rect());
    
    TInt variety;
    TBool disable = GetVariety(variety);
    if ( disable )
        {
        TRAP_IGNORE(static_cast<CTsAppUi*>(iEikonEnv->AppUi())->RequestPopUpL());
        }
    variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0; // double check to avoid layout panic
    
    TAknLayoutScalableParameterLimits gridParams = 
        AknLayoutScalable_Apps::cell_tport_appsw_pane_ParamLimits( variety );
    TPoint empty( ELayoutEmpty, ELayoutEmpty );
    
    // Setup bitmap layout
    AknListBoxLayouts::SetupFormGfxCell( *iGrid, iGrid->ItemDrawer(), 0,
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g1( variety ).LayoutLine(),
            empty, empty );

    // Setup text layout
    AknListBoxLayouts::SetupFormTextCell(*iGrid, iGrid->ItemDrawer(), 1,
            AknLayoutScalable_Apps::cell_tport_appsw_pane_t1( variety ).LayoutLine(),
            empty, empty);
    
    // Text colors
    TRgb textColor;
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG19 );
    TRgb highlightTextColor;
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), highlightTextColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG11 );
    CFormattedCellListBoxData::TColors colors;
    colors.iText = textColor;
    colors.iBack = iGrid->ItemDrawer()->BackColor();
    colors.iHighlightedText = highlightTextColor;
    colors.iHighlightedBack = iGrid->ItemDrawer()->HighlightedBackColor();
    iGrid->ItemDrawer()->FormattedCellData()->SetSubCellColorsL(1, colors);
    iGrid->SetStrokeColors(textColor, highlightTextColor);
    
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iGridItemGap = gridItem.Rect().iTl.iX - gridNextItem.Rect().iBr.iX;
        }
    else
        {
        iGridItemGap = gridNextItem.Rect().iTl.iX - gridItem.Rect().iBr.iX;
        }
    iMaxItemsOnScreen = Rect().Width() / gridItem.Rect().Width();
    if ( iMaxItemsOnScreen > 1 )
        {
        TInt widthWithGaps =
            (iMaxItemsOnScreen - 1) * iGridItemGap + iMaxItemsOnScreen * gridItem.Rect().Width();
        if ( widthWithGaps > Rect().Width() )
            {
            iMaxItemsOnScreen--;
            }
        }
    iGridItemWidth = gridItem.Rect().Width();
    
    // Update item drawer
    iGrid->UpdateItemDrawerLayoutDataL();
    
    // Update state
    HandleDeviceStateChanged( EDeviceType );
    
    if ( disable )
        {
        TRAP_IGNORE(static_cast<CTsAppUi*>(iEikonEnv->AppUi())->DisablePopUpL());
        }
    }


// --------------------------------------------------------------------------
// CTsFastSwapArea::LayoutGridView
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::LayoutGridViewL( TInt aItemCount )
    {
    RArray<TAknLayoutRect> rects;
    CleanupClosePushL(rects);
    rects.ReserveL(KLayoutItemCount);
    GetFastSwapAreaRects(rects);
    TAknLayoutRect gridItem = rects[1];
    CleanupStack::PopAndDestroy(&rects);
    if ( aItemCount )
        {
        iGrid->ItemDrawer()->ColumnData()->SetDrawBackground(EFalse);
        static_cast<CTsAppView*>(&iParent)->EnableDragEvents(ETrue);
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            iGrid->SetLayoutL( EFalse, EFalse, ETrue, aItemCount, 1, gridItem.Rect().Size(), iGridItemGap );
            }
        else
            {
            iGrid->SetLayoutL( EFalse, ETrue, ETrue, aItemCount, 1, gridItem.Rect().Size(), iGridItemGap );
            }
        }
    else
        {
        iGrid->ItemDrawer()->ColumnData()->SetDrawBackground(ETrue);
        static_cast<CTsAppView*>(&iParent)->EnableDragEvents(EFalse);
        }
    
    // Update scrollbar frame
    if( iGrid->ScrollBarFrame() )
        {
        iGrid->SetScrollBarFrame(NULL,CEikListBox::EOwnedExternally);
        }
    }


// --------------------------------------------------------------------------
// CTsFastSwapArea::GetFastSwapAreaRects
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::GetFastSwapAreaRects( RArray<TAknLayoutRect>& aRects )
    {
    TAknLayoutRect gridAppPane;
    TAknLayoutRect gridItem;
    TAknLayoutRect gridImage;
    TAknLayoutRect gridNextItem;
    
    TInt variety;
    TBool disable = GetVariety(variety);
    if ( disable )
        {
        TRAP_IGNORE(static_cast<CTsAppUi*>(iEikonEnv->AppUi())->RequestPopUpL());
        }
    variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0; // double check to avoid layout panic
    
    gridAppPane.LayoutRect( Rect(), 
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    aRects.Append(gridAppPane);
    
    gridItem.LayoutRect( gridAppPane.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane( variety, 0, 0 ) );
    aRects.Append(gridItem);
    
    gridImage.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g1( variety ) ); 
    aRects.Append(gridImage);
    gridNextItem.LayoutRect( gridAppPane.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_cp03( variety ) );
    aRects.Append(gridNextItem);
    
    if ( disable )
        {
        TRAP_IGNORE(static_cast<CTsAppUi*>(iEikonEnv->AppUi())->DisablePopUpL());
        }
    }


// --------------------------------------------------------------------------
// CTsFastSwapArea::Setup
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::Setup( MCoeControlObserver& aControlObserver )
    {
    iGrid->SetObserver( &aControlObserver );
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::SizeChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::SizeChanged()
    {
    TSLOG_CONTEXT( CTsFastSwapArea::SizeChanged, TSLOG_LOCAL );
    TSLOG_IN();
    
    if ( iGrid && !iIgnoreLayoutSwitch )
        {
        // Cancel ongoing pointer event
        iHandlePointerCandidate = EFalse;
        // Grid needs to be recreated to proper reinitilize
        // data with new layout values
        TInt selIdx = SelectedIndex();
        TRAPD(err,
              LayoutGridL();
              LayoutGridViewL( iArray.Count() );
              iEvtHandler.ReInitPhysicsL( GridWorldSize(), ViewSize(), ETrue );
              );
        
        if ( err != KErrNone )
            {
            TSLOG1( TSLOG_INFO, "LayoutGridL leaves with %d", err );
            }
        
        // Update grid view
        iGrid->SetCurrentDataIndex(selIdx);
        UpdateGrid(ETrue, EFalse);
        iGrid->DrawDeferred();
        
        // Order full redraw after switch
        if(iRedrawTimer)
            {
            iRedrawTimer->Cancel();
            iRedrawTimer->After(KRedrawTimeForLayoutSwitch);
            }
        }
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::Draw
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::Draw( const TRect& /*aRect*/ ) const
    {
//    CWindowGc& gc = SystemGc();
//    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
//    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
//    AknsDrawUtils::Background( skin,
//           cc,
//           this,
//           gc,
//           Rect() );
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::SwitchToApp
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::SwitchToApp( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iArray.Count() )
        {
        TInt wgId = iArray[aIndex]->WgId();
        TUid appUid = iArray[aIndex]->AppUid();
        SwitchToApp( wgId, appUid );
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::SwitchToApp
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::SwitchToApp( const TUid& aUid )
    {
    TApaTaskList taskList( iEikonEnv->WsSession() );
    TApaTask task = taskList.FindApp( aUid );
    TInt wgId = task.WgId();
    SwitchToApp( wgId, aUid );
    }


// --------------------------------------------------------------------------
// CTsFastSwapArea::SwitchToApp
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::SwitchToApp( TInt aWgId, const TUid& aUid )
    {
    CTsAppUi* appui =
        static_cast<CTsAppUi*>( iEikonEnv->AppUi() );
    TBool effectsEnabled = appui->EffectsEnabled();

    // Move other app to foreground
    if ( !effectsEnabled )
        {
        iFSClient->SwitchToApp( aWgId );
        }
    
    // We do not want to come back to ts if the activated app is closed.
    // Therefore ts must be moved to background. Ignore orientation updates, it
    // will be done after task switcher is sent to background
    iIgnoreLayoutSwitch = ETrue;
    appui->MoveAppToBackground( CTsAppUi::EActivationTransition, aUid, aWgId );
    iIgnoreLayoutSwitch = EFalse;
    
    // Move other app to foreground
    if ( effectsEnabled )
        {
        iFSClient->SwitchToApp( aWgId );
        }
    
    // Orientation update
    iPrevScreenOrientation = -1; // force orientation reinit
    iOrientationSignalTimer->Cancel();
    iOrientationSignalTimer->After(KOrientationSwitchTime);
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::TryCloseAppL
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::TryCloseAppL( TInt aIndex,
        TBool aSuppressRendering )
    {
    TSLOG_CONTEXT( TryCloseAppL, TSLOG_LOCAL );
    TSLOG2_IN( "%d %d", aIndex, aSuppressRendering );

    if ( aIndex >= 0 && aIndex < iArray.Count() && CanClose( aIndex ) )
        {
        TInt wgId = iArray[aIndex]->WgId();
        iFSClient->CloseApp( wgId );
        iIsClosing.Append(wgId);
        if ( iArray[aIndex]->Widget() )
            {
            iWidgetClosingCount++;
            }
        
        // Orientation update
        iPrevScreenOrientation = GetCurrentScreenOrientation();
        iOrientationSignalTimer->Cancel();
        iOrientationSignalTimer->After(KOrientationSwitchTime);
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::TryCloseAllL
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::TryCloseAllL()
    {
    for ( TInt i = iArray.Count() - 1; i >= 0; --i )
        {
        if ( CanClose( i ) )
            {
            TryCloseAppL( i, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::CanClose
// --------------------------------------------------------------------------
//
TBool CTsFastSwapArea::CanClose( TInt aIndex ) const
    {
    TBool canClose(EFalse);
    if ( aIndex >= 0 && aIndex < iArray.Count() )
        {
        CTsFswEntry* e = iArray[aIndex];
        canClose = !e->AlwaysShown() && !e->SystemApp();
        // Special cases: Menu
        canClose |= e->AppUid() == KTsMenuUid;
        }
    return canClose;
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::CanCloseOthers
// --------------------------------------------------------------------------
//
TBool CTsFastSwapArea::CanCloseAll( TInt aSelectedItem ) const
    {
    TInt count( 0 );

    // Count number of closable applications and if number exceeds 2 finish
    // counting, because it is already enough to show the option.
    for ( TInt i = iArray.Count(); --i >= 0 && count < KTsMaxClosableApps; )
        {
        if ( CanClose( i ) )
            {
            count++;
            }
        }
    return ( count >= KTsMaxClosableApps ) ||
           ( count && !CanClose( aSelectedItem ) );
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::HandleFswContentChanged
// From MTsFswObserver
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleFswContentChanged()
    {
    TSLOG_CONTEXT( HandleFswContentChanged, TSLOG_LOCAL );
    TSLOG_IN();

    TRAPD( err, HandleFswContentChangedL() );
    if ( err != KErrNone )
        {
        TSLOG1( TSLOG_INFO, "leave occured: %d", err );
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::HandleFswContentChangedL
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleFswContentChangedL()
    {
    TSLOG_CONTEXT( HandleFswContentChangedL, TSLOG_LOCAL );
    TSLOG_IN();

    // get current content from fastswap server
    iFSClient->GetContentL( iArray );

#ifdef _DEBUG
    for ( TInt i = 0, ie = iArray.Count(); i != ie; ++i )
        {
        CTsFswEntry* e = iArray[i];
        const TDesC& name( e->AppName() );
        TSLOG4( TSLOG_INFO, "[%d]: %d %d %S", i, e->WgId(), e->AppUid(), &name );
        }
#endif
    
    // Update closing widget count if necessary
    if ( iWidgetClosingCount )
        {
        TInt widgetCount(0);
        for ( TInt i = 0, ie = iArray.Count(); i != ie; ++i )
            {
            if ( iArray[i]->Widget() )
                {
                widgetCount++;
                }
            }
        iWidgetClosingCount = widgetCount;
        }

    // draw
    RenderContentL();

    // notify observer, if present
    NotifyChange();

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::RenderContentL
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::RenderContentL( TBool aSuppressAnimation )
    {
    TSLOG_CONTEXT( RenderContentL, TSLOG_LOCAL );
    TSLOG_IN();

    _LIT(KSeparator, "\t");
    
    CArrayPtr<CGulIcon>* iconArray = new ( ELeave ) CAknIconArray( KMaxGranularity );
    CleanupStack::PushL( iconArray );
    CDesCArrayFlat* textArray = new ( ELeave ) CDesCArrayFlat( KMaxGranularity );
    CleanupStack::PushL( textArray );
    RArray<TInt> closeItemArray;
    CleanupClosePushL(closeItemArray);
    RArray<TInt> strokeItemArray;
    CleanupClosePushL(strokeItemArray);
    
    // Update view based on number of items
    LayoutGridViewL( iArray.Count() );
    
    for ( TInt i = 0; i < iArray.Count(); ++i )
        {
        const TDesC& appName( iArray[i]->AppName() );
        const TInt formatLen = 3 + 2;
        RBuf formAppName;
        CleanupClosePushL(formAppName);
        formAppName.CreateL(appName.Length() + formatLen);
        formAppName.AppendNum(i);
        formAppName.Append(KSeparator);
        formAppName.Append(appName);
        textArray->AppendL(formAppName);
        CleanupStack::PopAndDestroy(&formAppName);
        TSize sz = PreferredImageSize();

        // take the screenshot or appicon+mask and make a copy and scale
        CFbsBitmap* bitmap = 0;
        TInt h = iArray[i]->ScreenshotHandle();
        TSLOG2( TSLOG_INFO, "'%S' screenshot handle %d", &appName, h );
        TInt maskh = 0;
        CFbsBitmap* mask = 0;
        if ( !h )
            {
            // No screenshot, take app icon
            h = iArray[i]->AppIconBitmapHandle();
            maskh = iArray[i]->AppIconMaskHandle();
            TSLOG1( TSLOG_INFO, "using appicon, handle = %d", h );
            }
        else
            {
            // Screenshot exists, mark it for stroke
            strokeItemArray.AppendL(i);
            }
        __ASSERT_DEBUG( h, User::Invariant() );
        bitmap = CopyBitmapL( h, sz );
        CleanupStack::PushL( bitmap );
        if ( maskh )
            {
            mask = CopyBitmapL( maskh, sz );
            }
        CleanupStack::PushL( mask );

        CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
        CleanupStack::PushL(icon);
        iconArray->AppendL(icon);
        CleanupStack::Pop( 3, bitmap ); // mask, bitmap, icon
        
        // Check if item can be closed
        if ( CanClose(i) && AknLayoutUtils::PenEnabled() )
            {
            closeItemArray.AppendL(i);
            }
        }
    // Setup grid
    iGrid->Model()->SetItemTextArray(textArray);
    CArrayPtr<CGulIcon>* oldIconArray =
        iGrid->ItemDrawer()->FormattedCellData()->IconArray();
    if(oldIconArray)
        {
        delete oldIconArray;
        oldIconArray = NULL;
        }
    iGrid->ItemDrawer()->FormattedCellData()->SetIconArrayL(iconArray);
    iGrid->SetCloseItemsL(closeItemArray);
    iGrid->SetStrokeItemsL(strokeItemArray);
    
    // Update scrollbar visibility
    if( iGrid->ScrollBarFrame() )
        {
        iGrid->SetScrollBarFrame(NULL,CEikListBox::EOwnedExternally);
        }
    
    // Cleanup
    CleanupStack::PopAndDestroy(&strokeItemArray);
    CleanupStack::PopAndDestroy(&closeItemArray);
    CleanupStack::Pop(textArray);
    CleanupStack::Pop(iconArray);
    
    // refresh the items in the grid
    iGrid->HandleItemAdditionL();
    iEvtHandler.ReInitPhysicsL( GridWorldSize(), ViewSize(), ETrue );
    if ( SelectedIndex() >= GridItemCount() && GridItemCount() )
        {
        iGrid->SetCurrentDataIndex( GridItemCount() - 1 );
        }
    UpdateGrid( ETrue, !aSuppressAnimation );
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::CopyBitmapL
// Copy and scale.
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFastSwapArea::CopyBitmapL( TInt aFbsHandle, TSize aSize )
    {
    CFbsBitmap* ret = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( ret );

    CFbsBitmap* bmp = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( bmp );
    User::LeaveIfError( bmp->Duplicate( aFbsHandle ) );

    // do not always use aSize, preserving the aspect ratio is quite
    // important when showing app icons instead of screenshots
    TSize sz = CalculateSizePreserveRatio( aSize, bmp->SizeInPixels() );
    User::LeaveIfError( ret->Create( sz, bmp->DisplayMode() ) );

    AknIconUtils::ScaleBitmapL( sz, ret, bmp );

    CleanupStack::PopAndDestroy( bmp );
    CleanupStack::Pop( ret );

    return ret;
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CTsFastSwapArea::CountComponentControls() const
    {
    return 1;
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CTsFastSwapArea::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iGrid;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::HandleSwitchToBackgroundEvent
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleSwitchToBackgroundEvent()
    {
    // stop listening for changes in fsw content
    iFSClient->CancelSubscribe();
    // Hide highlight
    if ( iGrid->GridBehaviour() == CTsFastSwapGrid::ETouchOnly )
        {
        iGrid->HideHighlight();
        }
    CancelLongTapAnimation();
    iGrid->MakeVisible(EFalse);
    iGrid->DrawNow();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::HandleSwitchToForegroundEvent
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleSwitchToForegroundEvent()
    {
    TSLOG_CONTEXT( CTsFastSwapArea::HandleSwitchToForegroundEvent, TSLOG_LOCAL );
    TSLOG_IN();
    
    iIsClosing.Reset();
    iWidgetClosingCount = 0;
    iHandlePointerCandidate = EFalse;
    
    CTsGridItemDrawer* itemDrawer =
        static_cast<CTsGridItemDrawer*>( iGrid->ItemDrawer() );
    itemDrawer->SetRedrawBackground(ETrue);
    
    // Update Layout
    CTsAppUi* appUi = static_cast<CTsAppUi*>(iEikonEnv->AppUi());
    if ( appUi && appUi->EffectsEnabled() )
        {
        TRAP_IGNORE( LayoutGridL() );
        }
    
    // Reset grid
    if ( iDeviceState.DeviceType() == CTsDeviceState::EFullTouch )
        {
        iGrid->HideHighlight();
        }
    else
        {
        iGrid->ShowHighlight();
        }
    
    // get the current task list
    HandleFswContentChanged();
    // and then start listening for changes
    iFSClient->Subscribe( *this );
    
    RestoreSelectedIndex();
    UpdateGrid(EFalse, EFalse);
    
    iRedrawTimer->Cancel();
    iRedrawTimer->After(KRedrawTime);
    
    // give feedback
    LaunchPopupFeedback();
    
    iPrevAppCount = iArray.Count();
    
    if ( !appUi->DelayedForegroundLaunched() )
        {
        iGrid->MakeVisible(ETrue);
        }

    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::FocusChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    if ( IsFocused() )
        {
        // if in non-touch mode then select (highlight) some item
        if ( !AknLayoutUtils::PenEnabled()
            && SelectedIndex() == KErrNotFound
            && GridItemCount() )
            {
            RestoreSelectedIndex();
            }
        }
    else
        {
        // store the currently selected index if there is one
        SaveSelectedIndex();
        }
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CTsFastSwapArea::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    CancelLongTapAnimation();
    
    iKeyEvent = ETrue;
    
    //do not forward the event until item is higlighted
    TKeyResponse response = ShowHighlightOnKeyEvent(aKeyEvent, aType);
    if( response == EKeyWasConsumed )
        {
        return EKeyWasConsumed;
        }
    
    // handle the 'clear' key
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
        {
        TInt idx = SelectedIndex();
        if ( idx >= 0 )
            {
            TryCloseAppL( idx );
            }
        return EKeyWasConsumed;
        }
    
    // pass the event to grid
    // do not pass down and up arrow key events
    if ( aKeyEvent.iScanCode != EStdKeyUpArrow &&
         aKeyEvent.iScanCode != EStdKeyDownArrow &&
         aKeyEvent.iScanCode != EStdKeyApplication0 )
        {
        TBool animate(ETrue);
        TBool redraw(EFalse);
        TInt prevItem = SelectedIndex();
        TKeyResponse response = iGrid->OfferKeyEventL( aKeyEvent, aType );
        if ( prevItem != SelectedIndex() && // item changed
             ( ( prevItem == 0 &&
                 SelectedIndex() == GridItemCount() - 1 &&
                 GridItemCount() > iMaxItemsOnScreen + 1 ) || // loop from first to last item
               ( prevItem == GridItemCount() - 1 &&
                 SelectedIndex() == 0 &&
                 GridItemCount() > iMaxItemsOnScreen + 1) // loop from last to first item
              ) // loop
            )
            {
            // Loop occured
            animate = EFalse;
            }
        if ( prevItem != SelectedIndex() )
            {
            redraw = ETrue;
            iGrid->ShowHighlight();
            }
        UpdateGrid( redraw, animate );
        return response;
        }
    
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::CatchKeyEvent
// -----------------------------------------------------------------------------
//
TKeyResponse CTsFastSwapArea::ShowHighlightOnKeyEvent(
		const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TKeyResponse retVal(EKeyWasNotConsumed);
	
	if (aKeyEvent.iScanCode == EStdKeyLeftArrow ||
		aKeyEvent.iScanCode == EStdKeyRightArrow ||
		aKeyEvent.iScanCode == EStdKeyDevice3 ||
        aKeyEvent.iScanCode == EStdKeyBackspace ||
        aKeyEvent.iScanCode == EStdKeyEnter )
	    {
		if (!iGrid->IsHighlightVisible())
			{
			if (aType == EEventKey)
				{
				iGrid->ShowHighlight();
				iConsumeEvent = ETrue;
				}
			retVal = EKeyWasConsumed;
			}
		else if (iConsumeEvent)
			{
			if (aType == EEventKeyUp)
				{
				retVal = EKeyWasConsumed;
				}
			else
				{
				iConsumeEvent = EFalse;
				}
			}
		}
	return retVal;
	}


// -----------------------------------------------------------------------------
// CTsFastSwapArea::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    iKeyEvent = EFalse;
    if(aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        iHandlePointerCandidate = ETrue;
        iTapEvent = aPointerEvent;
        iGrid->EnableAknEventHandling(EFalse);
        iGrid->HandlePointerEventL(aPointerEvent);
        iGrid->EnableAknEventHandling(ETrue);
        // Check if long tap animation should be launched
        if ( LongTapAnimForPos(aPointerEvent.iParentPosition) )
            {
            iLongTapAnimationTimer->Cancel();
            iLongTapAnimationTimer->After(KLongTapAnimationInitTime);
            }
        }
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        CancelLongTapAnimation( EFalse );
        if( iActivateOnPointerRelease != TPoint() )
            {
            iHandlePointerCandidate = ETrue;
            TapL(iActivateOnPointerRelease);
            iActivateOnPointerRelease = TPoint();
            }
        }
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::SelectedIndex
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapArea::SelectedIndex() const
    {
    return iGrid->CurrentDataIndex();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::SaveSelectedIndex
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::SaveSelectedIndex()
    {
    iSavedSelectedIndex = SelectedIndex();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::RestoreSelectedIndex
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::RestoreSelectedIndex()
    {
    iSavedSelectedIndex = KErrNotFound;
    if ( GridItemCount() )
        {
        // highlight second recent item (that has index 1) if possible
        TInt highlightItem = 0;
        TInt count = GridItemCount();
        while( highlightItem < count 
            && highlightItem < KItemToHighlight )
            {
            ++highlightItem;
            }
        iSavedSelectedIndex = highlightItem - 1;//count from 0
        iGrid->SetCurrentDataIndex( iSavedSelectedIndex );
        }
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ProcessCommandL
// Handle fsw popup commands
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::ProcessCommandL( TInt aCommandId )
    {
    static_cast<CTsAppUi*>(iEikonEnv->AppUi())->DisablePopUpL();
    switch ( aCommandId )
        {
        case EFswCmdClose:
            TryCloseAppL( iAppIndexForPopup );
            break;

        case EFswCmdCloseAll:
            TryCloseAllL();
            break;

        case KErrCancel:
            // popup dismisses automatically when tapping outside or when switching to other app
            break;
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::HandleCloseEventL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleCloseEventL(TInt aItemIdx)
    {
    TryCloseAppL( aItemIdx );
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::HandleDeviceStateChanged
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleDeviceStateChanged( TChangeType aChangeType )
    {
    if ( aChangeType == EDeviceType )
        {
        CTsDeviceState::TDeviceType state = iDeviceState.DeviceType();
        switch ( state )
            {
            case CTsDeviceState::EHybrid:
                {
                iGrid->SetGridBehaviour( CTsFastSwapGrid::EHybrid );
                }
                break;
            case CTsDeviceState::EFullTouch:
                {
                iGrid->SetGridBehaviour( CTsFastSwapGrid::ETouchOnly );
                }
                break;
            default:
                break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::TimerCompletedL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::TimerCompletedL( CTsFastSwapTimer* aSource )
    {
    if(aSource == iHighlightTimer)
        {
        iTapEvent.iType = TPointerEvent::EButton1Up;
        iGrid->HandlePointerEventL(iTapEvent);
        }
    else if(aSource == iRedrawTimer)
        {
        iGrid->MakeVisible(ETrue);
        static_cast<CTsAppView*>(&iParent)->OrderFullWindowRedraw();
        }
    else if( aSource == iUpdateGridTimer )
        {
        UpdateGrid(ETrue, ETrue);
        }
    else if ( aSource == iOrientationSignalTimer )
        {
        TInt currentOrientation = GetCurrentScreenOrientation();
        if ( currentOrientation != iPrevScreenOrientation )
            {
            // Order layout change
            static_cast<CAknAppUi*>(iCoeEnv->AppUi())->HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
            iRedrawTimer->Cancel();
            iRedrawTimer->After(KRedrawTime);
            }
        }
    else if ( aSource == iLongTapAnimationTimer )
        {
        if ( iLongTapAnimationRunning )
            {
            CancelLongTapAnimation();
            }
        else
            {
            static_cast<CTsAppUi*>(iEikonEnv->AppUi())->RequestPopUpL();
            if ( iLongTapAnimation )
                {
                delete iLongTapAnimation;
                iLongTapAnimation = NULL;
                }
            iLongTapAnimation = CAknLongTapAnimation::NewL(EFalse);
            iLongTapAnimation->SetParent( this );
            iLongTapAnimation->ShowAnimationL( iTapEvent.iParentPosition.iX,
                                               iTapEvent.iParentPosition.iY );
            iLongTapAnimationRunning = ETrue;
            iLongTapAnimationTimer->Cancel();
            iLongTapAnimationTimer->After(KLongTapAnimationTimeout);
            }
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::PreCreatePopupL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::PreCreatePopupL()
    {
    if ( !iPopup )
        {
        iPopup = CAknStylusPopUpMenu::NewL( this, Rect().iTl );
        HBufC* text = StringLoader::LoadLC( R_TS_FSW_CLOSE );
        iPopup->AddMenuItemL( *text, EFswCmdClose );
        CleanupStack::PopAndDestroy( text );
        text = StringLoader::LoadLC( R_TS_FSW_CLOSE_ALL );
        iPopup->AddMenuItemL( *text, EFswCmdCloseAll );
        CleanupStack::PopAndDestroy( text );
        }
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ShowPopupL
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapArea::ShowPopupL( TInt aIndex, const TPoint& aPoint )
    {
    TBool showPopUp(EFalse);
    TBool showPopupItem;
    // hide 'close' if app cannot be closed
    showPopupItem = CanClose( aIndex );
    iPopup->SetItemDimmed( EFswCmdClose, !showPopupItem );
    showPopUp = showPopUp || showPopupItem;
    // hide 'close all' if there are no applications to close.
    showPopupItem = CanCloseAll( aIndex );
    iPopup->SetItemDimmed( EFswCmdCloseAll, !showPopupItem );
    showPopUp = showPopUp || showPopupItem;

    if(showPopUp)
        {
        static_cast<CTsAppUi*>(iEikonEnv->AppUi())->RequestPopUpL();
        // give feedback
        LaunchPopupFeedback();
        // save index for later use & show popup
        iAppIndexForPopup = aIndex;
        iPopup->SetPosition( aPoint, CAknStylusPopUpMenu::EPositionTypeLeftBottom );
        iPopup->ShowMenu();
        }
    return showPopUp;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::Count
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapArea::Count() const
    {
    return iArray.Count();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::SetDataChangeObserver
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::SetDataChangeObserver(
        MTsDataChangeObserver* aDcObserver )
    {
    iDcObserver = aDcObserver;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::NotifyChange
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::NotifyChange()
    {
    if ( iDcObserver )
        {
        iDcObserver->DataChanged( this, Count() );
        }
    }
    
// -----------------------------------------------------------------------------
// CTsFastSwapArea::PreferredImageSize
// -----------------------------------------------------------------------------
//
TSize CTsFastSwapArea::PreferredImageSize()
    {
    TAknLayoutRect gridImage;
    TRAP_IGNORE(
        RArray<TAknLayoutRect> rects;
        CleanupClosePushL(rects);
        rects.ReserveL(KLayoutItemCount);
        GetFastSwapAreaRects(rects);
        gridImage = rects[2];
        CleanupStack::PopAndDestroy(&rects);
        );
    return gridImage.Rect().Size();
    }
    
// -----------------------------------------------------------------------------
// CTsFastSwapArea::GridItemCount
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapArea::GridItemCount()
    {
    return iGrid->Model()->ItemTextArray()->MdcaCount();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    if ( aListBox == iGrid )
        {
        switch ( aEventType )
            {
            case EEventEnterKeyPressed:
            case EEventItemClicked:
            case EEventItemSingleClicked:
                {
                SwitchToApp(SelectedIndex());
                }
                break;
            case EEventPenDownOnItem:
                {
                iGrid->ShowHighlight();
                }
                break;
            default:
                break;
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::CalculateSizePreserveRatio
// --------------------------------------------------------------------------
//
TSize CTsFastSwapArea::CalculateSizePreserveRatio(
        const TSize& aTargetAreaSize,
        const TSize& aSourceSize )
    {
    const TReal 
      resizedAspectRatio(aTargetAreaSize.iWidth/(TReal)aTargetAreaSize.iHeight);
    const TReal 
      orginalAspectRatio(aSourceSize.iWidth/(TReal)aSourceSize.iHeight);
    //this condition avoid empty margins ( bigger output ). to realy fit area change it
    const TReal scaleFactor = 
        ( orginalAspectRatio > resizedAspectRatio ) ? 
        (aTargetAreaSize.iHeight /(TReal)aSourceSize.iHeight) ://scale by height
        (aTargetAreaSize.iWidth /(TReal)aSourceSize.iWidth) ;//scale by width
    return TSize(aSourceSize.iWidth * scaleFactor, aSourceSize.iHeight * scaleFactor);
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::SelectNextItem
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::SelectNextItem()
    {
    iKeyEvent = ETrue;
    TBool forceRedraw(ETrue);
    TBool animate(ETrue);
    TInt selectedItem = SelectedIndex();
    selectedItem++;
    if ( selectedItem == GridItemCount() )
        {
        // Last item is selected, move to first one
        selectedItem = 0;
        animate = EFalse;
        }
    iGrid->SetCurrentDataIndex(selectedItem);
    UpdateGrid(forceRedraw, animate);
    }


// --------------------------------------------------------------------------
// CTsFastSwapArea::CenterItem
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::CenterItem(TInt aRedrawDelay)
    {
    if( iMaxItemsOnScreen < GridItemCount() )
        {
        TInt visibleItem = ViewToVisibleItem( ViewPos() );
        if(iKeyEvent)
            {
            visibleItem = SelectedIndex();
            }
        if(visibleItem != SelectedIndex())
            {
            iGrid->SetCurrentDataIndex( visibleItem );
            iParent.DrawDeferred();
            iGrid->DrawDeferred();
            }
        }

    // Check if view is outside of grid world
    TPoint absViewPos = ViewPos();
    absViewPos.iX -= Rect().Width() / 2;
    if( !iEvtHandler.IsPhysicsRunning() &&
        ( absViewPos.iX < 0 ||( absViewPos.iX + Rect().Width() > GridWorldSize().iWidth && GridItemCount() ) )
      )
        {
        // View is outside of grid world - update view
        iUpdateGridTimer->Cancel();
        iUpdateGridTimer->After(aRedrawDelay);
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::GridWorldSize
// --------------------------------------------------------------------------
//
TSize CTsFastSwapArea::GridWorldSize()
    {
    return TSize( GridItemCount() * iGridItemWidth + (GridItemCount() - 1) * iGridItemGap, Rect().Height() );
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::UpdateGrid
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::UpdateGrid( TBool aForceRedraw, TBool aAnimate )
    {
    TPoint targetPoint = ItemViewPosition( SelectedIndex() );
    if ( aForceRedraw || targetPoint.iX != ViewPos().iX )
        {
        if ( aAnimate )
            {
            iEvtHandler.Animate( targetPoint );
            }
        else
            {
            MoveOffset(targetPoint, ETrue);
            iEvtHandler.StopAnimation();
            }
        if ( aForceRedraw )
            {
            iParent.DrawDeferred();
            iGrid->DrawNow();
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::HandleAppKey
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::HandleAppKey(TInt aType)
    {
    if ( iArray.Count() )
        {
        if( aType == KAppKeyTypeShort )
            {
            //SwitchToApp( KTsHomescreenUid );
            TRAP_IGNORE( iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit) );
            }
        else if( aType == KAppKeyTypeLong )
            {
            // Dismiss task switcher
            TRAP_IGNORE( iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit) );
            }
        }
    else
        {
        TRAP_IGNORE( iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit) );
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::MoveOffset
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::MoveOffset(const TPoint& aPoint, TBool aDrawNow)
    {
    if( iHandlePointerCandidate )
        {
		//pointer was pressed and it's being waiting for handling
        return;
        }

    //postpone center item request in case of being moved
    if(iUpdateGridTimer->IsActive())
    	{
    	iUpdateGridTimer->Cancel();
		iUpdateGridTimer->After(KUpdateGridTime);
    	}
    
    if ( aDrawNow )
        {
        TInt currentXPos = aPoint.iX;
        currentXPos -= Rect().Width() / 2;
        TRect gridViewRect = Rect();
        gridViewRect.iTl.iX = -currentXPos;
        // Take edge offset into account
        gridViewRect.iTl.iX += Rect().iTl.iX;
        if(GridItemCount() && GridItemCount() <= iMaxItemsOnScreen)
            {
            // Center view
            gridViewRect.iTl.iX += ( Rect().Width() - GridItemCount() * iGridItemWidth ) / 2;
            }
        //iParent.DrawDeferred();
        iGrid->DrawDeferred();
        iGrid->SetRect( gridViewRect );
        iLogicalViewPosOffset = 0;
        }
    else
        {
        // Update logical view position
        iLogicalViewPosOffset = aPoint.iX - ViewPos().iX;
        }
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::Tap
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::TapL(const TPoint& aPoint)
    {
    CancelLongTapAnimation();
    
    if(!iHandlePointerCandidate)
        {
        return;
        }
    
    if(Rect().Contains(aPoint) && iArray.Count())
        {
        //provide tap pointer event to grid
        iGrid->HandlePointerEventL(iTapEvent);
        iHighlightTimer->Cancel();
        iHighlightTimer->After(KHighlighActivationTime);
        }
    else
        {
        //move task switcher to background
        iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
        }
	iHandlePointerCandidate = EFalse;
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::LongTap
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::LongTapL(const TPoint& aPoint)
    {
    CancelLongTapAnimation();
    
    if(!iHandlePointerCandidate)
        {
        return;
        }
    
    TInt index(KErrNotFound);
    if( iGrid->GridView()->XYPosToItemIndex(aPoint,index) && iArray.Count() )
        {
        iGrid->SetCurrentItemIndex(index);
        SaveSelectedIndex();
        if ( !ShowPopupL(iSavedSelectedIndex, aPoint) )
            {
            iActivateOnPointerRelease = aPoint;
            }
        iGrid->ShowHighlight();
        DrawNow();
        }
	iHandlePointerCandidate = EFalse;
    }

// --------------------------------------------------------------------------
// CTsFastSwapArea::DragL
// --------------------------------------------------------------------------
//
void CTsFastSwapArea::DragL(
    const MAknTouchGestureFwDragEvent& aEvent)
    {
    CancelLongTapAnimation();
    // Reset activation point
    iActivateOnPointerRelease = TPoint();
	iHandlePointerCandidate = EFalse;
	if( aEvent.State() == EAknTouchGestureFwStop)
		{
		CenterItem( KUpdateGridTime );
		}

    iGrid->HideHighlight();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ViewSize
// -----------------------------------------------------------------------------
//
TSize CTsFastSwapArea::ViewSize()
    {
    return TSize(Rect().Width(), Rect().Height());  
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::Stop
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::Stop()
    {    
    CenterItem( KUpdateGridTime );
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ViewSize
// -----------------------------------------------------------------------------
//
TPoint CTsFastSwapArea::ViewPos() const
    {
    TPoint retVal;
    retVal.iY = iGrid->Rect().iTl.iY + Rect().Height() / 2;
    retVal.iX = - (iGrid->Rect().iTl.iX - Rect().iTl.iX) + Rect().Width() / 2 ;
    TInt gridItemCount = iGrid->Model()->ItemTextArray()->MdcaCount();
    if(gridItemCount && gridItemCount <= iMaxItemsOnScreen)
        {
        // View centered
        retVal.iX += ( Rect().Width() - gridItemCount * iGridItemWidth ) / 2;
        }
    retVal.iX += iLogicalViewPosOffset;
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ItemPosition
// -----------------------------------------------------------------------------
//
TPoint CTsFastSwapArea::ItemViewPosition( TInt aItemIdx )
    {
    TPoint retVal = Rect().iTl;
    if ( aItemIdx < 0 )
        {
        // No items
        retVal.iX = 0;
        }
    else if ( aItemIdx == 0 )
        {
        // First item
        if( AknLayoutUtils::LayoutMirrored() )
            {
            if ( GridItemCount() > iMaxItemsOnScreen )
                {
                retVal.iX = GridWorldSize().iWidth - Rect().Width();
                }
            else
                {
                retVal.iX = 0;
                }
            }
        else // normal layout
            {
            retVal.iX = 0;
            }
        }
    else if ( aItemIdx == GridItemCount() - 1 )
        {
        // Last item selected
        if( AknLayoutUtils::LayoutMirrored() )
            {
            retVal.iX = 0;
            }
        else // normal layout
            {
            if ( GridItemCount() > iMaxItemsOnScreen )
                {
                retVal.iX = GridWorldSize().iWidth - Rect().Width();
                }
            else
                {
                retVal.iX = 0;
                }
            }
        }
    else
        {
        // Middle item
        TInt screenMiddleItemOffset = ( Rect().Width() - iGridItemWidth ) / 2;
        if( AknLayoutUtils::LayoutMirrored() )
            {
            retVal.iX = iGridItemWidth * ( GridItemCount() - 1 - aItemIdx ) - screenMiddleItemOffset;
            retVal.iX += ( GridItemCount() - 1 - aItemIdx ) * iGridItemGap;
            }
        else // normal layout
            {
            retVal.iX = iGridItemWidth * aItemIdx - screenMiddleItemOffset;
            retVal.iX += iGridItemGap * aItemIdx;
            }
        if ( retVal.iX < 0 )
            {
            retVal.iX = 0;
            }
        else if ( retVal.iX + Rect().Width() > GridWorldSize().iWidth )
            {
            retVal.iX = GridWorldSize().iWidth - Rect().Width();
            }
        }
    
    // Return middle of the view rectangle
    retVal.iX += Rect().Width() / 2;
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::ViewToVisibleItem
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapArea::ViewToVisibleItem( const TPoint aViewPos )
    {
    TInt retVal(0);
    TPoint absViewPos = aViewPos;
    absViewPos.iX -= Rect().Width() / 2;
    if ( absViewPos.iX < 0 )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // View crossed left border of grid world rect, last item selected
            retVal = GridItemCount() - 1;
            }
        else // normal layout
            {
            // View crossed left border of grid world rect, first item selected
            retVal = 0;
            }
        }
    else if ( absViewPos.iX + Rect().Width() > GridWorldSize().iWidth )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // View crossed right border of grid world rect, first item selected
            retVal = 0;
            }
        else // normal layout
            {
            // View crossed right border of grid world rect, last item selected
            retVal = GridItemCount() - 1;
            }
        }
    else
        {
        TInt retItemPosX(0);
        TInt offsetCheck = GridWorldSize().iWidth;
        // View inside of grid world rect
        for ( TInt i = 0 ; i < GridItemCount(); i++ )
            {
            TInt itemPosX = ItemViewPosition( i ).iX;
            TInt offset = aViewPos.iX - itemPosX;
            if ( Abs( offset ) <= offsetCheck )
                {
                offsetCheck = Abs( offset );
                retVal = i;
                retItemPosX = itemPosX;
                }
            else
                {
                break;
                }
            }
        // Check if item is fully visible. If not
        // return next one if possible
        if ( retItemPosX - iGridItemWidth / 2 < absViewPos.iX &&
             retVal + 1 < GridItemCount() )
            {
            retVal++;
            }
        }
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::LaunchPopupFeedback
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::LaunchPopupFeedback()
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            TTouchLogicalFeedback fbLogicalType = ETouchFeedbackPopUp;
            if ( CAknTransitionUtils::TransitionsEnabled(
                 AknTransEffect::EComponentTransitionsOff ) )
                {
                fbLogicalType = ETouchFeedbackIncreasingPopUp;
                }
            feedback->InstantFeedback( this,
                                       fbLogicalType,
                                       ETouchFeedbackVibra,
                                       TPointerEvent() );
            }
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::GetCurrentScreenOrientation
// -----------------------------------------------------------------------------
//
TInt CTsFastSwapArea::GetCurrentScreenOrientation()
    {
    TPixelsAndRotation availableRect;
    iEikonEnv->ScreenDevice()->GetDefaultScreenSizeAndRotation(availableRect);
    return availableRect.iPixelSize.iWidth > availableRect.iPixelSize.iHeight;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::GetVariety
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapArea::GetVariety( TInt& aVariety )
    {
    aVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TBool foreground = static_cast<CTsAppUi*>(iEikonEnv->AppUi())->IsForeground();
    if ( foreground )
        {
        TInt screenOrientation = GetCurrentScreenOrientation();
        if ( aVariety != screenOrientation )
            {
            aVariety = screenOrientation;
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::IsAppClosing
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapArea::IsAppClosing( TInt aWgId )
    {
    TBool retVal(EFalse);
    if ( iIsClosing.Count() )
        {
        TInt idx = iIsClosing.Find(aWgId);
        retVal = idx != KErrNotFound;
        if ( retVal )
            {
            iIsClosing.Remove(idx);
            }
        else if ( iWidgetClosingCount )
            {
            retVal = ETrue;
            iWidgetClosingCount--;
            }
        }
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::WgOnTaskList
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapArea::WgOnTaskList( TInt aWgId )
    {
    TBool retVal(EFalse);
    TInt appCount = iArray.Count();
    
    TApaTaskList taskList( iEikonEnv->WsSession() );
    TApaTask task = taskList.FindApp( KTsHomescreenUid );
    TInt homescrWgId = task.WgId();
    
    if ( iPrevAppCount != appCount )
        {
        for ( TInt i = 0; i < iArray.Count(); i++ )
            {
            TInt wgId = iArray[i]->WgId();
            if ( wgId == aWgId ||
                 homescrWgId == aWgId )
                {
                retVal = ETrue;
                }
            }
        }
    else if ( aWgId == homescrWgId )
        {
        retVal = ETrue;
        }
    iPrevAppCount = appCount;
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTsFastSwapArea::CancelLongTapAnimation
// -----------------------------------------------------------------------------
//
void CTsFastSwapArea::CancelLongTapAnimation(TBool aDisablePopup)
    {
    iLongTapAnimationRunning = EFalse;
    iLongTapAnimationTimer->Cancel();
    if ( iLongTapAnimation )
        {
        iLongTapAnimation->HideAnimation();
        delete iLongTapAnimation;
        iLongTapAnimation = NULL;
        }
    if( aDisablePopup )
        {
        TRAP_IGNORE( 
        static_cast<CTsAppUi*>(iEikonEnv->AppUi())->DisablePopUpL() );
        }
    }


// -----------------------------------------------------------------------------
// CTsFastSwapArea::LongTapAnimForPos
// -----------------------------------------------------------------------------
//
TBool CTsFastSwapArea::LongTapAnimForPos( const TPoint& aHitPoint )
    {
    if ( Rect().Contains(aHitPoint) )
        {
        TInt itemIdx;
        TBool isItemHit = iGrid->GridView()->XYPosToItemIndex( aHitPoint, itemIdx );
        if ( isItemHit && ( CanClose( itemIdx ) || CanCloseAll( itemIdx ) ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// End of file
