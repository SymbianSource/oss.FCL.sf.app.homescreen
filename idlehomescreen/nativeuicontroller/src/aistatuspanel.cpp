/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles of drawing text on navi and title panes.
*
*/


#include <aknappui.h>
#include <aknnavide.h>
#include <aknnavi.h>
#include <aknnavilabel.h>
#include <akntitle.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <avkon.rsg>
#include <AknStatuspaneUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknLayout2ScalableDef.h>
#include <e32property.h>
#include <avkondomainpskeys.h>
#include <telinformationpskeys.h>
#include <AknNaviDecoratorObserver.h>
#include <AknTitlePaneObserver.h>
#include "aistatuspanel.h"
#include "debug.h"

using namespace AiNativeUiController;

// Read capability: ReadDeviceData.
_LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
// Write capability: WriteDeviceData.
_LIT_SECURITY_POLICY_C1( KWritePolicy, ECapabilityWriteDeviceData );


void CAiStatusPanel::ConstructL()
    {
    // Display info
    RProperty::Define(
        KPSUidTelInformation,
        KTelDisplayInfo,
        RProperty::EByteArray,
        KReadPolicy,
        KWritePolicy );

    // RenderTitlePaneL() can be called without the bitmap set to valid handle
    iBitmapHandle = KErrNotFound;
    // Get pointer to status pane
    iStatusPane = static_cast<CAknAppUi*>( CEikonEnv::Static()->
        EikAppUi() )->StatusPane();

    // If status pane can't be initialized, let framework handle the problem
    if( !iStatusPane )
        {
        User::Leave( KErrNotFound );
        }

    //Get pointer to title pane
    iTitlePane = ( CAknTitlePane* )iStatusPane->ControlL(
        TUid::Uid( EEikStatusPaneUidTitle ) );

    // If title pane can't be initialized, let framework handle the problem
    if( !iTitlePane )
        {
        User::Leave( KErrNotFound );
        }

    //Get pointer to navi pane
    iNaviPane = (CAknNavigationControlContainer *)iStatusPane->
        ControlL(TUid::Uid(EEikStatusPaneUidNavi));

    // If navi pane can't be initialized, let framework handle the problem
    if( !iNaviPane )
        {
        User::Leave( KErrNotFound );
        }

    // Construct navi pane
    //iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC() );
    //iNaviPane->PushL( *iNaviDecorator );

#ifdef _DEBUG
    _LIT(titlepanetext, "Operator");
    SetTitlePaneTextL(titlepanetext);
#endif
    }


CAiStatusPanel* CAiStatusPanel::NewL()
    {
    CAiStatusPanel* self = new( ELeave ) CAiStatusPanel;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



CAiStatusPanel::~CAiStatusPanel()
    {
    delete iTitlePaneText;
    delete iNaviPaneText;
    delete iNaviDecorator;
    }


CAiStatusPanel::CAiStatusPanel()
    {
    }


void CAiStatusPanel::RenderTextOnNaviPaneL()
    {
    if( iNaviPaneText )
        {
        if (!iNaviDecorator)
            {
            iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC() );

            iNaviDecorator->SetNaviDecoratorObserver( iNaviDecoratorObserver );

            iNaviPane->PushL( *iNaviDecorator );
            }
        CAknNaviLabel* naviLabel =
            static_cast< CAknNaviLabel* >( iNaviDecorator->DecoratedControl() );
        if( naviLabel )
            {
            naviLabel->SetTextL( *iNaviPaneText );
            iNaviDecorator->DrawDeferred();
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    }

void CAiStatusPanel::RenderTextOnTitlePaneL()
    {
    __PRINTS("XAI: Rendering text to title pane");
    if( !iTitlePaneText )
        {
        __PRINTS("XAI: Rendering text to title pane - invalid text, aborting");
        return;
        }

    TTelTitleDisplay displayText;
    displayText.iLogoHandle = 0;
    displayText.iLogoMaskHandle = 0;
    TBool scroll = !IsKeyLockEnabled();
    iTitlePane->SetText( iTitlePaneText, scroll );
    displayText.iDisplayTag.Copy(
    iTitlePaneText->Left(displayText.iDisplayTag.MaxLength()) );

    // SetText took ownership
    iTitlePaneText = NULL;

    UpdatePhoneDisplayInfo( displayText );
    __PRINTS("XAI: Rendering text to title pane - done");
    }


void CAiStatusPanel::RenderBitmapOnTitlePaneL()
    {
    __PRINTS("XAI: Rendering bitmap to title pane");
    if( iBitmapHandle == KErrNotFound )
        {
        __PRINTS("XAI: Rendering bitmap to title pane - invalid bitmap handle, aborting");
        return;
        }

    TSize size;
    TRect rect;
    TBool hasTitlePane = AknLayoutUtils::LayoutMetricsRect(
                                            AknLayoutUtils::ETitlePane, rect );
    if ( !hasTitlePane )
        {
        // no title pane, size is unknown.
        User::Leave( KErrNotReady );
        }
    User::LeaveIfError( GetTitlePaneSize( size ) );

    // CAknIcon takes ownership of bitmaps.
    CFbsBitmap* dupMain = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( dupMain );
    User::LeaveIfError(
        dupMain->Duplicate( iBitmapHandle ) );

    CFbsBitmap* dupMask = NULL;
    if ( iMaskHandle != KErrNotFound )
        {
        dupMask = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( dupMask );
        User::LeaveIfError(
            dupMask->Duplicate( iMaskHandle ) );
        }

    //bitmapIcon is not pushed to cleanupstack, ownership
    //is transferred later
    CAknIcon* bitmapIcon = CAknIcon::NewL();
    bitmapIcon->SetMask( dupMask ); // ownership transferred
    if ( iMaskHandle != KErrNotFound )
        {
        CleanupStack::Pop( dupMask );
        }

    bitmapIcon->SetBitmap( dupMain ); // ownership transferred
    CleanupStack::Pop( dupMain );

    //ownership of bitmapIcon is transferred
    CAknIcon* scalableIcon = AknIconUtils::CreateIconL( bitmapIcon );
    CleanupStack::PushL( scalableIcon );

    AknIconUtils::SetSize( scalableIcon->Bitmap(), size,
                           EAspectRatioPreservedAndUnusedSpaceRemoved );
    AknIconUtils::SetSize( scalableIcon->Mask(), size,
                           EAspectRatioPreservedAndUnusedSpaceRemoved );

    // Title pane takes ownership of icons.
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Duplicate( scalableIcon->Bitmap()->Handle() ) );

    // Check if icon has mask
    CFbsBitmap* bitmapMask = NULL;
    if ( scalableIcon->Mask() )
        {
        bitmapMask = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( bitmapMask );
        User::LeaveIfError(
            bitmapMask->Duplicate( scalableIcon->Mask()->Handle() ) );
        }

    // Set the icon in title pane...
    iTitlePane->SetPicture( bitmap, bitmapMask );

    const TInt bitmapHandle = bitmap ? bitmap->Handle() : 0;
    const TInt bitmapMaskHandle = bitmapMask ? bitmapMask->Handle() : 0;

    // ... and inform TSY to use it
    TTelTitleDisplay displayText;
    displayText.iDisplayTag.Zero();
    displayText.iLogoHandle = bitmapHandle;
    displayText.iLogoMaskHandle = bitmapMaskHandle;

    UpdatePhoneDisplayInfo( displayText );

    if ( bitmapMask )
        {
        CleanupStack::Pop( bitmapMask );
        }
    CleanupStack::Pop( bitmap );
    CleanupStack::PopAndDestroy( scalableIcon );


    // Assume invalid bitmaps. Client needs to set the handles again
    // Otherwise invalid handles could be hold here and when drawing
    // undetermined stuff could occurr
    iBitmapHandle = KErrNotFound;
    iMaskHandle = KErrNotFound;
    __PRINTS("XAI: Rendering bitmap to title pane - done");
    }

TInt CAiStatusPanel::GetTitlePaneSize( TSize& aSize )
    {
    TRect titlePaneRect;
    TBool hasTitlePane = AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::ETitlePane, titlePaneRect );

    if ( !hasTitlePane )
        {
        return KErrNotFound;
        }

    TAknLayoutRect oplogoLayout;

    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        // stacon
        oplogoLayout.LayoutRect( titlePaneRect,
            AknLayoutScalable_Avkon::title_pane_stacon_g2(0).LayoutLine() );
        }
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        // flat
        oplogoLayout.LayoutRect( titlePaneRect,
            AknLayoutScalable_Avkon::title_pane_stacon_g2(0).LayoutLine() );
        }
    else
        {
        // usual idle
        oplogoLayout.LayoutRect( titlePaneRect,
            AknLayout::Title_pane_elements_Line_1() );
        }

    aSize = oplogoLayout.Rect().Size();

    return KErrNone;
    }


void CAiStatusPanel::UpdatePhoneDisplayInfo( const TTelTitleDisplay& aDisplayInfo )
    {
    TBool changed = EFalse;
    TBool indicatorChanged =
        aDisplayInfo.iDisplayTag != iDisplayInfo.iDisplayTag ||
        aDisplayInfo.iLogoHandle != iDisplayInfo.iLogoHandle ||
        aDisplayInfo.iLogoMaskHandle != iDisplayInfo.iLogoMaskHandle;

    if ( indicatorChanged )
        {
        changed = ETrue;
        iDisplayInfo.iDisplayTag = aDisplayInfo.iDisplayTag;
        iDisplayInfo.iLogoHandle = aDisplayInfo.iLogoHandle;
        iDisplayInfo.iLogoMaskHandle = aDisplayInfo.iLogoMaskHandle;
        }

    if ( changed )
        {
        TTelTitleDisplay::TDisplayInfoPckg pckg( iDisplayInfo );
        RProperty::Set(
            KPSUidTelInformation,
            KTelDisplayInfo,
            pckg );
        }
    }


void CAiStatusPanel::SetNaviPaneTextL( const TDesC& aText )
    {
    delete iNaviPaneText;
    iNaviPaneText = NULL;
    if ( !AknStatuspaneUtils::FlatLayoutActive() )
        {
        HBufC* temp = aText.AllocL();
        iNaviPaneText = temp;
        }
    }



void CAiStatusPanel::SetTitlePaneTextL( const TDesC& aText )
    {
    HBufC* temp = aText.AllocL();
    delete iTitlePaneText;
    iTitlePaneText = temp;
    }


void CAiStatusPanel::SetTitlePaneBitmapL( TInt aBitmapHandle, TInt aMaskHandle )
    {
    iBitmapHandle = aBitmapHandle;
    iMaskHandle = aMaskHandle;
    }


void CAiStatusPanel::RenderTitlePaneL()
    {
    RenderTextOnTitlePaneL();
    RenderBitmapOnTitlePaneL();
    }


void CAiStatusPanel::RenderNaviPaneL()
    {
    RenderTextOnNaviPaneL();
    }

void CAiStatusPanel::StopTitlePaneScrollingL()
    {
    //stop scrolling
    if( iTitlePane->Text() )
        {
        iTitlePane->SetTextL( *iTitlePane->Text(), EFalse );
        }
    }

void CAiStatusPanel::ScrollTitlePaneTextL()
    {
// start scrolling
    if( iTitlePane->Text() )
        {
        iTitlePane->SetTextL( *iTitlePane->Text(), ETrue );
        }
    }

void CAiStatusPanel::SetNaviDecoratorObserver( MAknNaviDecoratorObserver* aObserver )
    {
    iNaviDecoratorObserver = aObserver;
    }

void CAiStatusPanel::SetTitlePaneObserver( MAknTitlePaneObserver* aObserver )
    {
    iTitlePane->SetTitlePaneObserver( aObserver );
    }

TBool CAiStatusPanel::IsKeyLockEnabled()
    {
    TInt value;
    TInt err = RProperty::Get(KPSUidAvkonDomain, KAknKeyguardStatus, value);
    if ( err != KErrNone )
        return EFalse;
    switch( value )
        {
        case EKeyguardLocked:
        case EKeyguardAutolockEmulation:
            return ETrue;
        case EKeyguardNotActive:
        default:
            return EFalse;
        }
    }

