/*
 * ============================================================================
 *  Name        : hgteleportfastswaparea.cpp
 *  Part of     : Hg Teleport
 *  Description : Teleport Fast Swap area UI
 *  Version     : %version: sa1spcx1#74 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
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
#include <hgteleport.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlists.h>
#include <touchfeedback.h>
#include <akntransitionutils.h>
#include <akntranseffect.h>

#include "hgteleportfastswaparea.h"
#include "hgteleportapplogging.h"
#include "hgfswclient.h"
#include "hgteleportappui.h"
#include "hgteleportdatachangeobserver.h"
#include "hgteleporteventcontroler.h"

/** command ids for the fsw popup */
enum TPopupCommands
    {
    EFswCmdClose = 10000,
    EFswCmdCloseAll
    };

/** Number of closable applications, to show "close all" option. */
const TInt KHgMaxClosableApps = 2;

/** Interval until which no change in the fsw content is rendered
    after starting the closing of an application. */
const TInt KRefreshDelayAfterClose = 2; // seconds

/** Uid of Active Idle application. 
    Used when movind Ai to specified position.*/
const TUid KAiUid = TUid::Uid( 0x102750F0 );
    
/** Position of Ai in fsw grid.*/
const TInt KAiPosition = 0;

/** Default grid item to highlight.*/
const TInt KItemToHighlight = 3;

const TInt KAppKeyTypeShort = 1;
const TInt KAppKeyTypeLong = 2;

const TInt KLayoutItemCount = 3;

const TInt KRedrawTime = 250000; // 0.25 sec
const TInt KHighlighActivationTime = 100000; // 100 ms
const TInt KUpdateGridTime = 1000000; // 1 s

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::NewL
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapArea* CHgTeleportFastSwapArea::NewL( const TRect& aRect,
    CCoeControl& aParent, CHgTeleportDeviceState& aDeviceState, 
    CHgTeleportEventControler& aEventHandler )
    {
    CHgTeleportFastSwapArea* self = CHgTeleportFastSwapArea::NewLC(aRect,
            aParent, aDeviceState, aEventHandler);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::NewLC
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapArea* CHgTeleportFastSwapArea::NewLC( const TRect& aRect,
    CCoeControl& aParent, CHgTeleportDeviceState& aDeviceState,
    CHgTeleportEventControler& aEventHandler)
    {
    CHgTeleportFastSwapArea* self = new (ELeave) CHgTeleportFastSwapArea(
            aParent, aDeviceState, aEventHandler);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::CHgTeleportFastSwapArea
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapArea::CHgTeleportFastSwapArea(CCoeControl& aParent,
    CHgTeleportDeviceState& aDeviceState,
    CHgTeleportEventControler& aEventHandler) :
    iParent(aParent), iDeviceState(aDeviceState), iEvtHandler(aEventHandler),
    iPreviousNoOfItems(0)
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::~CHgTeleportFastSwapArea
// -----------------------------------------------------------------------------
//
CHgTeleportFastSwapArea::~CHgTeleportFastSwapArea()
    {
    iArray.ResetAndDestroy();
    delete iGrid;
    delete iFSClient;
    delete iPopup;
    delete iConfirmCloseQuery;
    delete iHighlightTimer;
    delete iRedrawTimer;
    delete iUpdateGridTimer;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::ConstructL( const TRect& aRect )
    {
    SetContainerWindowL( iParent );

    SetRect( aRect );

    // setup ganes grid
    ReCreateGridL();

    // create stylus popup instance
    PreCreatePopupL();

    // connect to fsw server
    iFSClient = CHgFswClient::NewL();
    
    iEvtHandler.ReInitPhysicsL(GridWorldSize(), ViewSize(), ETrue);
    
    // add self to device state observer
    iDeviceState.AddObserverL( *this, EDeviceType );
    
    iHighlightTimer = new (ELeave) CHgTeleportFastSwapTimer( *this );
    iHighlightTimer->ConstructL();
    
    iRedrawTimer = new (ELeave) CHgTeleportFastSwapTimer( *this );
    iRedrawTimer->ConstructL();

    iUpdateGridTimer = new (ELeave) CHgTeleportFastSwapTimer( *this );
    iUpdateGridTimer->ConstructL();
    
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ReCreateGridL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::ReCreateGridL()
    {
    MCoeControlObserver* obs = NULL;
    TBool wasHighlight = iDeviceState.DeviceType() == 
                             CHgTeleportDeviceState::EHybrid ? ETrue : EFalse;
    if ( iGrid )
        {
        obs = iGrid->Observer();
        iDeviceState.RemoveObserver(*iGrid);
        wasHighlight = iGrid->IsHighlightVisible();
        delete iGrid;
        iGrid = NULL;
        }
    
    iGrid = new( ELeave ) CHgTeleportFastSwapGrid;
    iGrid->ConstructL( this );
    iDeviceState.AddObserverL(*iGrid, MHgDeviceStateObserver::ESkin);
    
    AknListBoxLayouts::SetupStandardGrid( *iGrid );
    
    RArray<TAknLayoutRect> rects;
    CleanupClosePushL(rects);
    rects.ReserveL(KLayoutItemCount);
    GetFastSwapAreaRects(rects);
    TAknLayoutRect gridAppPane = rects[0];
    TAknLayoutRect gridItem = rects[1];
    TAknLayoutRect gridImage = rects[2];
    CleanupStack::PopAndDestroy(&rects);
    
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    iGrid->SetRect(gridAppPane.Rect());
    iGrid->SetVisibleViewRect(gridAppPane.Rect());
    TAknLayoutScalableParameterLimits gridParams = 
        AknLayoutScalable_Apps::cell_tport_appsw_pane_ParamLimits( variety );
    TPoint empty( ELayoutEmpty, ELayoutEmpty );
    
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
    AknListBoxLayouts::SetupFormGfxCell( *iGrid, iGrid->ItemDrawer(), 0,
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g1( variety ).LayoutLine(),
            empty, empty );
#else
    TRect imageRect;
    TRect textRect;
    TInt leftMargin = ( KFswItemWidth - KFswImageSize ) / 2;
    TInt topMargin = ( KFswItemHeight - KFswImageSize - KFswTextHeight ) / 3;
    textRect.iTl = TPoint( leftMargin, topMargin );
    textRect.iBr = TPoint( leftMargin + KFswImageSize, topMargin + KFswTextHeight );
    imageRect.iTl = TPoint( leftMargin, topMargin * 2 + KFswTextHeight );
    imageRect.iBr = TPoint( leftMargin + KFswImageSize, topMargin * 2 + KFswTextHeight + KFswImageSize );
    
    AknListBoxLayouts::SetupFormGfxCell( *iGrid, iGrid->ItemDrawer(), 0 /*Column index*/,
                                         imageRect.iTl.iX  /*Left pos*/, imageRect.iTl.iY /*Top pos*/,
                                         0 /*unused*/, 0 /*unused*/,
                                         imageRect.Width() /*Icon width*/,
                                         imageRect.Height() /*Icon height*/,
                                         imageRect.iTl /*Start pos*/,
                                         imageRect.iBr /*End pos*/ );
#endif

    // Setup text layout
    TRgb textColor;
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG9 );
    
    TAknLayoutText textLayout;
    textLayout.LayoutText(
            Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_t1( variety ).LayoutLine() );
    
    // Because textLayout.BaselineOffset() does not work (missing lib entry),
    // we need to calculate offset ourselves
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
    TInt baselineOffset = textLayout.TextRect().iBr.iY - textLayout.TextRect().iTl.iY;
    AknListBoxLayouts::SetupFormTextCell( *iGrid, iGrid->ItemDrawer(), 1 /*Column index*/,
                                          textLayout.Font() /*Font type*/,
                                          textColor.Color16() /*color*/,
                                          textLayout.TextRect().iTl.iX /*Left margin*/, 0 /*unused*/,
                                          baselineOffset /*Baseline*/, 0 /*Text width*/,
                                          textLayout.Align() /*Text alignment*/,
                                          TPoint(0,0) /*Start pos*/,
                                          TPoint(0,0) /*End pos*/);
#else    
    TInt baselineOffset = textRect.iBr.iY - textRect.iTl.iY;
    AknListBoxLayouts::SetupFormTextCell( *iGrid, iGrid->ItemDrawer(), 1 /*Column index*/,
                                          textLayout.Font() /*Font type*/,
                                          textColor.Color16() /*color*/,
                                          textRect.iTl.iX /*Left margin*/, 0 /*unused*/,
                                          baselineOffset /*Baseline*/, 0 /*Text width*/,
                                          CGraphicsContext::ECenter /*Text alignment*/,
                                          TPoint(0,0) /*Start pos*/,
                                          TPoint(0,0) /*End pos*/);
#endif
    
    // Setup grid observers
    if ( obs )
        {
        iGrid->SetObserver( obs );
        }
    iGrid->SetListBoxObserver(this);
    iGrid->SetFastSwapGridObserver(this);
    iGrid->SetContainerWindowL(*this);
    
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
    iMaxItemsOnScreen = Rect().Width() / gridItem.Rect().Width();
    iGridItemWidth = gridItem.Rect().Width();
#else
    iMaxItemsOnScreen = Rect().Width() / KFswItemWidth;
    iGridItemWidth = KFswItemWidth;
#endif
    
    iGrid->ItemDrawer()->ColumnData()->SetDrawBackground(EFalse);
    
    // Update state
    HandleDeviceStateChanged( EDeviceType );
    if( wasHighlight )
        {
        iGrid->ShowHighlight();
        }
    else
        {
        iGrid->HideHighlight();
        }
    
    // Make sure that there is an ActivateL call even when we are not
    // called from ConstructL. (in order to have the grid's parent ptr set properly)
    ActivateL();
    }


// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::GetFastSwapAreaRects
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::GetFastSwapAreaRects( RArray<TAknLayoutRect>& aRects )
    {
    TAknLayoutRect gridAppPane;
    TAknLayoutRect gridItem;
    TAknLayoutRect gridImage;
    
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    
    gridAppPane.LayoutRect( Rect(), 
            AknLayoutScalable_Apps::tport_appsw_pane( variety ) );
    aRects.Append(gridAppPane);
    
    gridItem.LayoutRect( gridAppPane.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane( variety, 0, 0 ) );
    aRects.Append(gridItem);
    
    gridImage.LayoutRect( gridItem.Rect(),
            AknLayoutScalable_Apps::cell_tport_appsw_pane_g1( variety ) ); 
    aRects.Append(gridImage);
    }


// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::Setup
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::Setup( MCoeControlObserver& aControlObserver )
    {
    iGrid->SetObserver( &aControlObserver );
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SizeChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::SizeChanged()
    {
    HGLOG_CONTEXT( CHgTeleportFastSwapArea::SizeChanged, HGLOG_LOCAL );
    HGLOG_IN();
    
    if ( iGrid )
        {
        // Grid needs to be recreated to proper reinitilize
        // data with new layout values
        TInt selIdx = SelectedIndex();
        TRAPD(err, 
              ReCreateGridL();
              iEvtHandler.ReInitPhysicsL(GridWorldSize(), ViewSize(), ETrue););
        if ( err != KErrNone )
            {
            HGLOG1( HGLOG_INFO, "ReCreateGridL leaves with %d", err );
            }
        HandleFswContentChanged();
        iGrid->SetCurrentDataIndex(selIdx);
        UpdateGrid(ETrue, EFalse);
        DrawDeferred();
        }
    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::Draw
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin,
           cc,
           this,
           gc,
           Rect() );
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SwitchToApp
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::SwitchToApp( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iArray.Count() )
        {
        TInt wgId = iArray[aIndex]->WgId();
        // Move other app to foreground and then move ourselves to background.
        // Order is important and cannot be reversed.
        iFSClient->SwitchToApp( wgId );
        // We do not want to come back to teleport if the activated app is closed.
        // Therefore teleport must be moved to background.
        CHgTeleportAppUi* appui =
            static_cast<CHgTeleportAppUi*>( iEikonEnv->AppUi() );
        appui->MoveAppToBackground( AknTransEffect::EApplicationStartRect );
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::TryCloseAppL
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::TryCloseAppL( TInt aIndex,
        TBool aSuppressRendering )
    {
    HGLOG_CONTEXT( TryCloseAppL, HGLOG_LOCAL );
    HGLOG2_IN( "%d %d", aIndex, aSuppressRendering );

    if ( aIndex >= 0 && aIndex < iArray.Count() && CanClose( aIndex ) )
        {
        TInt wgId = iArray[aIndex]->WgId();
        iFSClient->CloseApp( wgId );
        // The fsw content will change sooner or later
        // but the updated content (without the closed app) will not
        // come very fast. It looks better to the user if the item
        // in the grid is removed right here, right now.
        // If the app does not close for some reason then this is
        // not fully correct but the app will then reappear on the next
        // content-changed notification anyway.
        delete iArray[aIndex];
        iArray.Remove( aIndex );
        NotifyChange();
        if ( !aSuppressRendering )
            {
            RenderContentL();
            }
        // Update item selection on the screen if last item was deleted
        TInt newItemCount = GridItemCount();
        if ( aIndex == newItemCount )
            {
            newItemCount--;
            iGrid->SetCurrentDataIndex(newItemCount);
            }
        iTimeOfLastClose.HomeTime();
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::TryCloseAppWithQueryL
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::TryCloseAppWithQueryL( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iArray.Count()
            && CanClose( aIndex )
            && ConfirmCloseL( aIndex ) )
        {
        TryCloseAppL( aIndex );
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::TryCloseAllL
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::TryCloseAllL()
    {
    // note the direction of the loop, this is needed because
    // TryCloseAppL may modify the array
    TBool changed = EFalse;
    for ( TInt i = iArray.Count() - 1; i >= 0; --i )
        {
        if ( CanClose( i ) )
            {
            TryCloseAppL( i, ETrue );
            changed = ETrue;
            }
        }
    if ( changed )
        {
        RenderContentL();
        RestoreSelectedIndex();
        iGrid->DrawNow();
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::CanClose
// --------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapArea::CanClose( TInt aIndex ) const
    {
    CHgFswEntry* e = iArray[aIndex];
    return !e->AlwaysShown() && !e->SystemApp();
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::CanCloseOthers
// --------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapArea::CanCloseAll( TInt aSelectedItem ) const
    {
    TInt count( 0 );

    // Count number of closable applications and if number exceeds 2 finish
    // counting, because it is already enough to show the option.
    for ( TInt i = iArray.Count(); --i >= 0 && count < KHgMaxClosableApps; )
        {
        if ( CanClose( i ) )
            {
            count++;
            }
        }
    return ( count >= KHgMaxClosableApps ) ||
           ( count && !CanClose( aSelectedItem ) );
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleFswContentChanged
// From MHgFswObserver
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleFswContentChanged()
    {
    HGLOG_CONTEXT( HandleFswContentChanged, HGLOG_LOCAL );
    HGLOG_IN();

    TRAPD( err, HandleFswContentChangedL() );
    if ( err != KErrNone )
        {
        HGLOG1( HGLOG_INFO, "leave occured: %d", err );
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleFswContentChangedL
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleFswContentChangedL()
    {
    HGLOG_CONTEXT( HandleFswContentChangedL, HGLOG_LOCAL );
    HGLOG_IN();

    // If there was an app close operation started during the last
    // few seconds then stop, to prevent flickering.
    TTime now;
    now.HomeTime();
    TTimeIntervalSeconds iv;
    if ( now.SecondsFrom( iTimeOfLastClose, iv ) == KErrNone
            && iv.Int() <= KRefreshDelayAfterClose )
        {
        HGLOG1_OUT( "difference since last close is only %d sec, stop", iv.Int() );
        return;
        }

    // get current content from fastswap server
    iFSClient->GetContentL( iArray );
    SwapApplicationOrder( iArray );

#ifdef _DEBUG
    for ( TInt i = 0, ie = iArray.Count(); i != ie; ++i )
        {
        CHgFswEntry* e = iArray[i];
        const TDesC& name( e->AppName() );
        HGLOG4( HGLOG_INFO, "[%d]: %d %d %S", i, e->WgId(), e->AppUid(), &name );
        }
#endif

    // draw
    RenderContentL();

    // notify observer, if present
    NotifyChange();

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::RenderContentL
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::RenderContentL()
    {
    HGLOG_CONTEXT( RenderContentL, HGLOG_LOCAL );
    HGLOG_IN();

    _LIT(KSeparator, "\t");
    
    CArrayPtr<CGulIcon>* iconArray = new ( ELeave ) CAknIconArray( iArray.Count() );
    CleanupStack::PushL( iconArray );
    CDesCArrayFlat* textArray = new ( ELeave ) CDesCArrayFlat( iArray.Count() );
    CleanupStack::PushL( textArray );
    RArray<TInt> closeItemArray;
    CleanupClosePushL(closeItemArray);
    
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    RArray<TAknLayoutRect> rects;
    CleanupClosePushL(rects);
    rects.ReserveL(KLayoutItemCount);
    GetFastSwapAreaRects(rects);
    TAknLayoutRect gridItem = rects[1];
    CleanupStack::PopAndDestroy(&rects);
    if ( AknLayoutUtils::LayoutMirrored() )
        {
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
        iGrid->SetLayoutL( EFalse, EFalse, ETrue, iArray.Count(), 1, gridItem.Rect().Size() );
#else
        TSize itemSize( KFswItemWidth, KFswItemHeight );
        iGrid->SetLayoutL( EFalse, EFalse, ETrue, iArray.Count(), 1, itemSize );
#endif
        }
    else
        {
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
        iGrid->SetLayoutL( EFalse, ETrue, ETrue, iArray.Count(), 1, gridItem.Rect().Size() );
#else
        TSize itemSize( KFswItemWidth, KFswItemHeight );
        iGrid->SetLayoutL( EFalse, ETrue, ETrue, iArray.Count(), 1, itemSize );
#endif
        }
    
    for ( TInt i = 0, ie = iArray.Count(); i != ie; ++i )
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
        HGLOG2( HGLOG_INFO, "'%S' screenshot handle %d", &appName, h );
        TInt maskh = 0;
        CFbsBitmap* mask = 0;
        if ( !h )
            {
            h = iArray[i]->AppIconBitmapHandle();
            maskh = iArray[i]->AppIconMaskHandle();
            HGLOG1( HGLOG_INFO, "using appicon, handle = %d", h );
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
    
    // Cleanup
    CleanupStack::PopAndDestroy(&closeItemArray);
    CleanupStack::Pop(textArray);
    CleanupStack::Pop(iconArray);
    
    iGrid->ScrollBarFrame()->SetScrollBarVisibilityL(
                CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
    
    // refresh the items in the grid
    if(iPreviousNoOfItems < iArray.Count())
        {
        iGrid->HandleItemAdditionL();
        }
    else if(iPreviousNoOfItems > iArray.Count())
        {
        iGrid->HandleItemRemovalL();
        }
    iPreviousNoOfItems = iArray.Count();
    iEvtHandler.ReInitPhysicsL( GridWorldSize(), ViewSize(), ETrue );
    UpdateGrid( ETrue );
    
    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::CopyBitmapL
// Copy and scale.
// --------------------------------------------------------------------------
//
CFbsBitmap* CHgTeleportFastSwapArea::CopyBitmapL( TInt aFbsHandle, TSize aSize )
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
// CHgTeleportFastSwapArea::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CHgTeleportFastSwapArea::CountComponentControls() const
    {
    return 1;
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CHgTeleportFastSwapArea::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iGrid;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleSwitchToBackgroundEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleSwitchToBackgroundEvent()
    {
    // stop listening for changes in fsw content
    iFSClient->CancelSubscribe();
    // get rid of the close confirmation query if shown
    delete iConfirmCloseQuery; // this will cause ExecuteLD to return with 0
    iConfirmCloseQuery = 0;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleSwitchToForegroundEvent
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleSwitchToForegroundEvent()
    {
    HGLOG_CONTEXT( CHgTeleportFastSwapArea::HandleSwitchToForegroundEvent, HGLOG_LOCAL );
    HGLOG_IN();
    
    // get the current task list
    HandleFswContentChanged();
    // and then start listening for changes
    iFSClient->Subscribe( *this );
    
    if ( iDeviceState.DeviceType() == CHgTeleportDeviceState::EFullTouch )
        {
        iGrid->HideHighlight();
        }
    else
        {
        iGrid->ShowHighlight();
        }
    
    RestoreSelectedIndex();
    UpdateGrid(ETrue, EFalse);
    
    iRedrawTimer->Cancel();
    iRedrawTimer->After(KRedrawTime);
    
    // give feedback
    LaunchPopupFeedback();

    HGLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::FocusChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::FocusChanged( TDrawNow /*aDrawNow*/ )
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
    iGrid->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CHgTeleportFastSwapArea::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    iKeyEvent = ETrue;
    // handle the 'clear' key
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
        {
        TInt idx = SelectedIndex();
        if ( idx >= 0 )
            {
            TryCloseAppWithQueryL( idx );
            }
        return EKeyWasConsumed;
        }
    
    //do not forward the event until item is higlighted
    if( aKeyEvent.iScanCode == EStdKeyLeftArrow ||
        aKeyEvent.iScanCode == EStdKeyRightArrow )
        {
        if ( !iGrid->IsHighlightVisible() )
            {
            if ( aType == EEventKey )
                {
                ShowHighlight();
                iConsumeEvent = ETrue;
                }
            return EKeyWasConsumed;
            }
        else if(iConsumeEvent)
            {
            if (aType == EEventKeyUp)
                {
                return EKeyWasConsumed;
                }
            iConsumeEvent = EFalse;
            }
        }
    
    // pass the event to grid
    // do not pass down and up arrow key events
    if ( aKeyEvent.iScanCode != EStdKeyUpArrow &&
         aKeyEvent.iScanCode != EStdKeyDownArrow )
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
// CHgTeleportFastSwapArea::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    iKeyEvent = EFalse;
    if(aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        iTapEvent = aPointerEvent;
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ConfirmCloseL
// -----------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapArea::ConfirmCloseL( TInt aIndex )
    {
    HBufC* msg = StringLoader::LoadLC( R_TELEPORT_FSW_CONFIRM_CLOSE,
        iArray[aIndex]->AppName() );
    iConfirmCloseQuery = CAknQueryDialog::NewL(
        CAknQueryDialog::EConfirmationTone );
    iConfirmCloseQuery->SetPromptL( *msg );
    CleanupStack::PopAndDestroy( msg );
    TBool ok = iConfirmCloseQuery->ExecuteLD(
        R_HG_TELEPORT_FSW_CONFIRM_CLOSE_QUERY );
    iConfirmCloseQuery = 0;
    return ok;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SelectedIndex
// -----------------------------------------------------------------------------
//
TInt CHgTeleportFastSwapArea::SelectedIndex() const
    {
    return iGrid->CurrentDataIndex();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SaveSelectedIndex
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::SaveSelectedIndex()
    {
    iSavedSelectedIndex = SelectedIndex();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::RestoreSelectedIndex
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::RestoreSelectedIndex()
    {
    iSavedSelectedIndex = KErrNotFound;
    if ( GridItemCount() )
        {
        // highlight second recent item (that has index 2) if possible
        TInt highlightItem = 0;
        TInt count = GridItemCount();
        while( highlightItem < count 
            && highlightItem < KItemToHighlight )
            {
            ++highlightItem;
            }
        iSavedSelectedIndex = highlightItem - 1;//count from 0
        iGrid->SetCurrentItemIndex( iSavedSelectedIndex );
        TBool forceRedraw(ETrue);
        UpdateGrid(forceRedraw);
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ProcessCommandL
// Handle fsw popup commands
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::ProcessCommandL( TInt aCommandId )
    {
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
    iTapEvent.iType = TPointerEvent::EButton1Up;
    iGrid->HandlePointerEventL(iTapEvent);
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleCloseEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleCloseEventL(TInt aItemIdx)
    {
    if ( !iLongTap )
        {
        TryCloseAppWithQueryL( aItemIdx );
        }
    // Simulate long tap to prevent item activation
    iLongTap = ETrue;
    iTapEvent.iType = TPointerEvent::EButton1Up;
    iGrid->HandlePointerEventL(iTapEvent);
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleDeviceStateChanged
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleDeviceStateChanged( TChangeType aChangeType )
    {
    if ( aChangeType == EDeviceType )
        {
        CHgTeleportDeviceState::TDeviceType state = iDeviceState.DeviceType();
        switch ( state )
            {
            case CHgTeleportDeviceState::EHybrid:
                {
                iGrid->SetGridBehaviour( CHgTeleportFastSwapGrid::EHybrid );
                }
                break;
            case CHgTeleportDeviceState::EFullTouch:
                {
                iGrid->SetGridBehaviour( CHgTeleportFastSwapGrid::ETouchOnly );
                }
                break;
            default:
                break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::TimerCompletedL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::TimerCompletedL( CHgTeleportFastSwapTimer* aSource )
    {
    if(aSource == iHighlightTimer)
        {
        iTapEvent.iType = TPointerEvent::EButton1Up;
        iGrid->HandlePointerEventL(iTapEvent);
        }
    else if(aSource == iRedrawTimer)
        {
        DrawNow();
        }
    else if( aSource == iUpdateGridTimer )
        {
        UpdateGrid(ETrue, ETrue);
        }
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::PreCreatePopupL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::PreCreatePopupL()
    {
    if ( !iPopup )
        {
        iPopup = CAknStylusPopUpMenu::NewL( this, Rect().iTl );
        HBufC* text = StringLoader::LoadLC( R_TELEPORT_FSW_CLOSE );
        iPopup->AddMenuItemL( *text, EFswCmdClose );
        CleanupStack::PopAndDestroy( text );
        text = StringLoader::LoadLC( R_TELEPORT_FSW_CLOSE_ALL );
        iPopup->AddMenuItemL( *text, EFswCmdCloseAll );
        CleanupStack::PopAndDestroy( text );
        }
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ShowPopupL
// -----------------------------------------------------------------------------
//
TBool CHgTeleportFastSwapArea::ShowPopupL( TInt aIndex, const TPoint& aPoint )
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
// CHgTeleportFastSwapArea::Count
// -----------------------------------------------------------------------------
//
TInt CHgTeleportFastSwapArea::Count() const
    {
    return iArray.Count();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SetDataChangeObserver
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::SetDataChangeObserver(
        MHgTeleportDataChangeObserver* aDcObserver )
    {
    iDcObserver = aDcObserver;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::NotifyChange
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::NotifyChange()
    {
    if ( iDcObserver )
        {
        iDcObserver->DataChanged( this, Count() );
        }
    }
    
// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SwapApplicationOrder
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::SwapApplicationOrder( 
    RPointerArray<CHgFswEntry>& aArray )
    {
    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        if( aArray[i]->AppUid() == KAiUid )
            {
            CHgFswEntry* homescreenEntry(0);
            homescreenEntry = aArray[i];
            aArray.Remove(i);
            aArray.Insert(homescreenEntry, KAiPosition);
            break;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::PreferredImageSize
// -----------------------------------------------------------------------------
//
TSize CHgTeleportFastSwapArea::PreferredImageSize()
    {
#ifndef TASKSWITCHER_USE_CUSTOM_LAYOUT
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
#else
    return TSize( KFswImageSize, KFswImageSize );
#endif
    }
    
// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::GridItemCount
// -----------------------------------------------------------------------------
//
TInt CHgTeleportFastSwapArea::GridItemCount()
    {
    return iGrid->Model()->ItemTextArray()->MdcaCount();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    if ( aListBox == iGrid )
        {
        switch ( aEventType )
            {
            case EEventEnterKeyPressed:
            case EEventItemClicked:
                {
                if (!iLongTap)
                    {
                    SwitchToApp(SelectedIndex());
                    }
                }
                break;
            case EEventPenDownOnItem:
                {
                iGrid->HideHighlight();
                iGrid->ShowHighlight();
                if (!iLongTap )
                    {
                    iHighlightTimer->Cancel();
                    iHighlightTimer->After(KHighlighActivationTime);
                    }
                }
                break;
            default:
                break;
            }
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::CalculateSizePreserveRatio
// --------------------------------------------------------------------------
//
TSize CHgTeleportFastSwapArea::CalculateSizePreserveRatio(
        const TSize& aTargetAreaSize,
        const TSize& aSourceSize )
    {
    TSize sz;
    if ( aSourceSize.iWidth > aSourceSize.iHeight )
        {
        sz.iWidth = aTargetAreaSize.iWidth;
        TReal ratio = aSourceSize.iWidth / (TReal) aSourceSize.iHeight;
        sz.iHeight = sz.iWidth / ratio;
        }
    else
        {
        sz.iHeight = aTargetAreaSize.iHeight;
        TReal ratio = aSourceSize.iHeight / (TReal) aSourceSize.iWidth;
        sz.iWidth = sz.iHeight / ratio;
        }
    return sz;
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::SelectNextItem
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::SelectNextItem()
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
    iGrid->SetCurrentItemIndex(selectedItem);
    UpdateGrid(forceRedraw, animate);
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ShowHiglight
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::ShowHighlight()
    {
    iGrid->ShowHighlight();
    UpdateGrid(ETrue, EFalse);
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::CenterItem
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::CenterItem(TInt aRedrawDelay)
    {
    if( iMaxItemsOnScreen < GridItemCount() )
        {
        TInt visibleItem = ViewToVisibleItem( ViewPos() );
        if(iKeyEvent)
            {
            visibleItem = SelectedIndex();
            }
        iGrid->SetCurrentDataIndex( visibleItem );
        }

    iUpdateGridTimer->Cancel();
    iUpdateGridTimer->After(aRedrawDelay);
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::GridWorldSize
// --------------------------------------------------------------------------
//
TSize CHgTeleportFastSwapArea::GridWorldSize()
    {
    return TSize( GridItemCount() * iGridItemWidth, Rect().Height() );
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::UpdateGrid
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::UpdateGrid( TBool aForceRedraw, TBool aAnimate )
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
            MoveOffset(targetPoint);
            iEvtHandler.StopAnimation();
            }
        if ( aForceRedraw )
            {
            iGrid->DrawNow();
            }
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::HandleAppKey
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::HandleAppKey(TInt aType)
    {
    if( aType == KAppKeyTypeShort )
        {
        if(iGrid->IsHighlightVisible())
            {
            SelectNextItem();
            }
        else
            {
            ShowHighlight();
            }
        }
    else if( aType == KAppKeyTypeLong )
        {
        SwitchToApp( SelectedIndex() );
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::MoveOffset
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::MoveOffset(const TPoint& aPoint)
    {
    HGLOG_CONTEXT( CHgTeleportFastSwapArea::MoveOffset, HGLOG_LOCAL );
    HGLOG2_IN("Old position x: %d, y:%d", ViewPos().iX, ViewPos().iY);
    HGLOG2_IN("New position x: %d, y:%d", aPoint.iX, aPoint.iY);
    HGLOG_OUT();
    
    TInt currentXPos = aPoint.iX;
    currentXPos -= Rect().Width() / 2;
    TRect gridViewRect = Rect();
    gridViewRect.iTl.iX = -currentXPos;
    // Take edge offset into account
    gridViewRect.iTl.iX += Rect().iTl.iX;
    if(GridItemCount() <= iMaxItemsOnScreen)
        {
        // Center view
        gridViewRect.iTl.iX += ( Rect().Width() - GridItemCount() * iGridItemWidth ) / 2;
        }
    iGrid->SetRect( gridViewRect );
    DrawNow();
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::Tap
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::TapL(const TPoint& aPoint)
    {
    iLongTap = EFalse;
    if(iGrid->Rect().Contains(aPoint))
        {
        //provide tap pointer event to grid
        iGrid->HandlePointerEventL(iTapEvent);
        }
    else
        {
        //move task switcher to background
        iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::LongTap
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::LongTapL(const TPoint& aPoint)
    {
    iLongTap = ETrue;
    TInt index(KErrNotFound);
    if( iGrid->GridView()->XYPosToItemIndex(aPoint,index) )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // Calculate logical item index
            index = GridItemCount() - 1 - index;
            }
        SaveSelectedIndex();
        iGrid->HandlePointerEventL(iTapEvent);
        if ( !ShowPopupL(index, aPoint) )
            {
            TapL(aPoint);
            }
        }
    else
        {
        TapL(aPoint);
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportFastSwapArea::Drag
// --------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::Drag(
    const MAknTouchGestureFwDragEvent& /*aEvent*/)
    {
    iGrid->HideHighlight();
    CenterItem( KUpdateGridTime );
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ViewSize
// -----------------------------------------------------------------------------
//
TSize CHgTeleportFastSwapArea::ViewSize()
    {
    return TSize(Rect().Width(), Rect().Height());  
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::Stop
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::Stop()
    {    
    CenterItem( KUpdateGridTime );
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ViewSize
// -----------------------------------------------------------------------------
//
TPoint CHgTeleportFastSwapArea::ViewPos() const
    {
    TPoint retVal;
    retVal.iY = iGrid->Rect().iTl.iY + Rect().Height() / 2;
    retVal.iX = - (iGrid->Rect().iTl.iX - Rect().iTl.iX) + Rect().Width() / 2 ;
    TInt gridItemCount = iGrid->Model()->ItemTextArray()->MdcaCount();
    if( gridItemCount <= iMaxItemsOnScreen)
        {
        // View centered
        retVal.iX += ( Rect().Width() - gridItemCount * iGridItemWidth ) / 2;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::ItemPosition
// -----------------------------------------------------------------------------
//
TPoint CHgTeleportFastSwapArea::ItemViewPosition( TInt aItemIdx )
    {
    TPoint retVal = Rect().iTl;
    if ( aItemIdx == 0 )
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
            }
        else // normal layout
            {
            retVal.iX = iGridItemWidth * aItemIdx - screenMiddleItemOffset;
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
// CHgTeleportFastSwapArea::ViewToVisibleItem
// -----------------------------------------------------------------------------
//
TInt CHgTeleportFastSwapArea::ViewToVisibleItem( const TPoint aViewPos )
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
        TInt offsetCheck = GridWorldSize().iWidth;
        // View inside of grid world rect
        for ( TInt i = 0 ; i < GridItemCount(); i++ )
            {
            TInt offset = aViewPos.iX - ItemViewPosition( i ).iX;
            if ( Abs( offset ) <= offsetCheck )
                {
                offsetCheck = Abs( offset );
                retVal = i;
                }
            else
                {
                break;
                }
            }
        }
    return retVal;
    }


// -----------------------------------------------------------------------------
// CHgTeleportFastSwapArea::LaunchPopupFeedback
// -----------------------------------------------------------------------------
//
void CHgTeleportFastSwapArea::LaunchPopupFeedback()
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

// End of file
