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
* Description:  Handles publishing to title pane.
*
*/


#include <eikbtgpc.h>
#include <aknappui.h>
#include <StringLoader.h>
#include <AiNativeUi.rsg>
#include <gulicon.h>
#include <AknIconUtils.h>
#include <AknsConstants.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknStatuspaneUtils.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>
#include <eiksoftkeyimage.h>
#include <AknSgcc.h>
#include <aiscutplugindomaincrkeys.h>

#include "ainativeui.hrh"
#include "aisoftkeyrenderer.h"
#include "ainativeuiplugins.h"
#include "aiscutdefs.h"


using namespace AiNativeUiController;

// Index for left softkey; defined in avkon
const TInt KNativeUiLeftSoftkeyId = 0;

// Index for right softkey; defined in avkon
const TInt KNativeUiRightSoftkeyId = 2;

const TInt KControlArrayCBAButton1Posn        =0;
const TInt KControlArrayCBAButton2Posn        =2;
const TInt KControlArrayCBAButtonMiddlePosn   =3;

const TInt KWideScreenWidth          = 640;


inline TAknWindowComponentLayout DoCompose( TAknWindowComponentLayout aLine1,
    TAknWindowComponentLayout aLine2 )
    {
    return TAknWindowComponentLayout::Compose( aLine1, aLine2 );
    }

inline TAknTextComponentLayout DoComposeText( TAknWindowComponentLayout aLine1,
    TAknTextComponentLayout aLine2 )
    {
    return TAknWindowComponentLayout::ComposeText( aLine1, aLine2 );
    }


// -----------------------------------------------------------------------------
//
// Calculate (whole) softkey area for Bottom button (landscape)
// -----------------------------------------------------------------------------
static TBool HDLayoutActive()
    {
    TInt resourceId = 0;
    CEikStatusPaneBase* statusPane = CEikStatusPaneBase::Current();

    if ( statusPane ) // Return the resource ID of app statuspane if it exists.
        {
        resourceId = statusPane->CurrentLayoutResId();
        }
    else // If this app does not have statuspane, then we ask the layout from AknCapServer.
        {
        resourceId = CAknSgcClient::CurrentStatuspaneResource();
        }

    TBool retVal = EFalse;

    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        // Can be widescreen only in landscape orientation.
        retVal = ( resourceId == R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL ||
                   resourceId == R_AVKON_WIDESCREEN_PANE_LAYOUT_IDLE  ||
                   resourceId == R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT ||
                   resourceId == R_AVKON_WIDESCREEN_PANE_LAYOUT_IDLE_FLAT );
        }

    return retVal;
    }


// -----------------------------------------------------------------------------
//
// Calculate softkey image graphics area for Bottom button (landscape)
// -----------------------------------------------------------------------------
static TRect SoftkeyRectBottomGraphics()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    TAknWindowComponentLayout rightAreaLayout(
        AknLayoutScalable_Avkon::area_side_right_pane( 0 ) );

    TAknWindowComponentLayout bottomSKLayout(
        DoCompose( rightAreaLayout,
                   AknLayoutScalable_Avkon::sctrl_sk_bottom_pane() ) );

    // Calculate softkey rects.
    // Left (bottom in landscape) softkey layout.
    TAknLayoutRect leftSoftkeyLayoutRect;
    leftSoftkeyLayoutRect.LayoutRect( screen,
                                      bottomSKLayout.LayoutLine() );
    TRect leftSoftKeyButtonRect( leftSoftkeyLayoutRect.Rect() );

    // Calculate softkey image layout.
    // Left (bottom in landscape) softkey layout.
    TAknWindowLineLayout leftSoftkeyImageLayout(
        DoCompose(
            bottomSKLayout,
            AknLayoutScalable_Avkon::sctrl_sk_bottom_pane_g1() ).LayoutLine() );


    TAknLayoutRect qgn_graf_sk_left;
    qgn_graf_sk_left.LayoutRect( screen, leftSoftkeyImageLayout );

    return qgn_graf_sk_left.Rect();
    }


// -----------------------------------------------------------------------------
//
// Calculate softkey image graphics area for Top button (landscape)
// -----------------------------------------------------------------------------
static TRect SoftkeyRectTopGraphics()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    TAknWindowComponentLayout rightAreaLayout(
        AknLayoutScalable_Avkon::area_side_right_pane( 0 ) );

    TAknWindowComponentLayout topSKLayout(
        DoCompose( rightAreaLayout,
                   AknLayoutScalable_Avkon::sctrl_sk_top_pane() ) );

    // Calculate softkey rects.
    // Right (top in landscape) softkey layout.
    TAknLayoutRect rightSoftkeyLayoutRect;
    rightSoftkeyLayoutRect.LayoutRect( screen,
                                       topSKLayout.LayoutLine() );
    TRect rightSoftKeyButtonRect( rightSoftkeyLayoutRect.Rect() );

    // Left (bottom in landscape) softkey layout.
    // Right (top in landscape) softkey layout.
    TAknWindowLineLayout rightSoftkeyImageLayout(
        DoCompose(
            topSKLayout,
            AknLayoutScalable_Avkon::sctrl_sk_top_pane_g1() ).LayoutLine() );

    TAknLayoutRect qgn_graf_sk_right;
    qgn_graf_sk_right.LayoutRect( screen, rightSoftkeyImageLayout );
    return qgn_graf_sk_right.Rect();
    }


// -----------------------------------------------------------------------------
// SoftkeyRectL
// Calculate soft key rect
// -----------------------------------------------------------------------------
static TRect SoftkeyRectL( CEikButtonGroupContainer& aContainer, TInt aPos, TBool aImageOn )
    {
    TRect rect;

    if( AknStatuspaneUtils::StaconPaneActive() )
        {
        TInt variety( 0 );

        if( AknStatuspaneUtils::StaconSoftKeysLeft() )
            {
            variety = 1;
            }

        TRect parentRect( iAvkonAppUi->ApplicationRect() );

        switch( aPos )
            {
            case KControlArrayCBAButton1Posn:
                {
                TAknWindowComponentLayout layout0( AknLayoutScalable_Avkon::area_bottom_pane( 2 ) );
                TAknWindowComponentLayout layout1( AknLayoutScalable_Avkon::stacon_bottom_pane() );

                // If clock is shown in stacon, cba area is smaller.
                TInt bottomCbaVariety( variety );

                if( AknStatuspaneUtils::ExtendedStaconPaneActive() )
                    {
                    bottomCbaVariety += 2;
                    }

                TAknWindowComponentLayout layout2( AknLayoutScalable_Avkon::control_bottom_pane_stacon( bottomCbaVariety ) );

                if( aImageOn )
                    {
                    // Icon layout
                    TAknWindowComponentLayout iconLayout( AknLayoutScalable_Avkon::control_bottom_pane_stacon_g1() );

                    TAknWindowComponentLayout lskIconLayout( DoCompose( layout0,
                        DoCompose( layout1, DoCompose( layout2, iconLayout ) ) ) );

                    TAknWindowLineLayout lskIcon( lskIconLayout.LayoutLine() );

                    TAknLayoutRect qgn_icon_lsk;

                    qgn_icon_lsk.LayoutRect( parentRect, lskIcon );

                    rect = qgn_icon_lsk.Rect();
                    }
                else
                    {
                    // Text layout
                    TAknTextComponentLayout textLayout( AknLayoutScalable_Avkon::control_bottom_pane_stacon_t1( variety ) );

                    TAknTextComponentLayout lskTextLayout( DoComposeText( layout0,
                        DoComposeText( layout1, DoComposeText( layout2, textLayout ) ) ) );

                    TAknTextLineLayout lskText( lskTextLayout.LayoutLine() );

                    TAknLayoutText qgn_text_lsk;

                    qgn_text_lsk.LayoutText( parentRect, lskText );

                    rect = qgn_text_lsk.TextRect();
                    }
                }
                break;
            case KControlArrayCBAButton2Posn:
                {
                TAknWindowComponentLayout layout0( AknLayoutScalable_Avkon::area_top_pane( 2 ) );
                TAknWindowComponentLayout layout1( AknLayoutScalable_Avkon::stacon_top_pane() );

                TInt topCbaVariety( variety );

                if( AknStatuspaneUtils::ExtendedStaconPaneActive() )
                    {
                    topCbaVariety += 4;
                    }

                TAknWindowComponentLayout layout2( AknLayoutScalable_Avkon::control_top_pane_stacon( topCbaVariety ) );

                if( aImageOn )
                    {
                    // Icon layout
                    TAknWindowComponentLayout iconLayout( AknLayoutScalable_Avkon::control_top_pane_stacon_g1() );

                    TAknWindowComponentLayout rskIconLayout( DoCompose( layout0,
                        DoCompose( layout1, DoCompose( layout2, iconLayout ) ) ) );

                    TAknWindowLineLayout rskIcon( rskIconLayout.LayoutLine() );

                    TAknLayoutRect qgn_icon_rsk;

                    qgn_icon_rsk.LayoutRect( parentRect, rskIcon );

                    rect = qgn_icon_rsk.Rect();
                    }
                else
                    {
                    // Text layout
                    TAknTextComponentLayout textLayout( AknLayoutScalable_Avkon::control_top_pane_stacon_t1( variety ) );

                    TAknTextComponentLayout rskTextLayout( DoComposeText( layout0,
                        DoComposeText( layout1, DoComposeText( layout2, textLayout ) ) ) );

                    TAknTextLineLayout rskText( rskTextLayout.LayoutLine() );

                    TAknLayoutText qgn_text_rsk;

                    qgn_text_rsk.LayoutText( parentRect, rskText );

                    rect = qgn_text_rsk.TextRect();
                    }
                }
                break;
            default:
                User::Leave( KErrNotSupported );
                break;

            }
        }
    else
        {
        TInt textVariety( 0 );
        TInt graphVariety( 0 );

        TBool mskEnabled( AknLayoutUtils::MSKEnabled() && Layout_Meta_Data::IsMSKEnabled() );

        if ( mskEnabled )
            {
            textVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 2 : 3;
            graphVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 2 : 4;
            }
        else
            {
            textVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 2 : 0;
            graphVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 2 : 4;
            }

        CEikCba* cba( static_cast< CEikCba* >( aContainer.ButtonGroup() ) );

        TRect parentRect( cba->Rect() );

        switch( aPos )
            {
            case KControlArrayCBAButton1Posn:
                {
                if( aImageOn )
                    {
                    if ( HDLayoutActive() &&
                         AKN_LAYOUT_WINDOW_screen.iW == KWideScreenWidth )
                        {
                        rect = SoftkeyRectBottomGraphics();
                        }
                     else
                        {

                        TAknLayoutRect qgn_icon_lsk;

                        qgn_icon_lsk.LayoutRect( parentRect,
                            AknLayoutScalable_Avkon::control_pane_g1( graphVariety ).LayoutLine() );

                        rect = qgn_icon_lsk.Rect();
                        }
                    }
                else
                    {
                    TAknLayoutText qgn_text_lsk;

                    qgn_text_lsk.LayoutText( parentRect,
                        AknLayoutScalable_Avkon::control_pane_t1( textVariety ).LayoutLine() );

                    rect = qgn_text_lsk.TextRect();
                    }
                }
                break;
            case KControlArrayCBAButton2Posn:
                {
                if( aImageOn )
                    {
                    if ( HDLayoutActive() &&
                         AKN_LAYOUT_WINDOW_screen.iW == KWideScreenWidth )
                        {
                        rect = SoftkeyRectTopGraphics();
                        }
                     else
                        {
                        TAknLayoutRect qgn_icon_rsk;

                        qgn_icon_rsk.LayoutRect( parentRect,
                            AknLayoutScalable_Avkon::control_pane_g2( graphVariety ).LayoutLine() );

                        rect = qgn_icon_rsk.Rect();
                        }
                    }
                else
                    {
                    TAknLayoutText qgn_text_rsk;

                    qgn_text_rsk.LayoutText( parentRect,
                        AknLayoutScalable_Avkon::control_pane_t2( textVariety ).LayoutLine() );

                    rect = qgn_text_rsk.TextRect();
                    }
                }
                break;
            case KControlArrayCBAButtonMiddlePosn:
                {
                if( !mskEnabled || aImageOn )
                    {
                    // No msk, or asking image size for msk
                    User::Leave( KErrNotSupported );
                    }
                else
                    {
                    TAknLayoutText qgn_text_msk;

                    qgn_text_msk.LayoutText( parentRect,
                        AknLayoutScalable_Avkon::control_pane_t3( textVariety ).LayoutLine() );

                    rect = qgn_text_msk.TextRect();
                    }
                }
                break;
            default:
                User::Leave( KErrNotFound );
                break;
            }
        }

    return rect;
    }


// -----------------------------------------------------------------------------
// SoftkeySizeL
// Calculate soft key size
// -----------------------------------------------------------------------------
static TSize SoftkeySizeL( CEikButtonGroupContainer& aContainer, TInt aPos, TBool aImageOn )
    {
    TSize size( SoftkeyRectL( aContainer, aPos, aImageOn ).Size() );

    return size;
    }


void CAiSoftKeyRenderer::ConstructL()
    {
    // load default soft key labels from resource
    CreateDefaultSoftKeysL();
    }


CAiSoftKeyRenderer* CAiSoftKeyRenderer::NewLC()
    {
    CAiSoftKeyRenderer* self = new( ELeave ) CAiSoftKeyRenderer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CAiSoftKeyRenderer::~CAiSoftKeyRenderer()
    {
    delete iText;

    if ( iCba )
        {
        CEikCba* cba =
            static_cast< CEikCba* >( iCba->ButtonGroup() );
        iAppUi->RemoveFromStack( cba );
        }
    delete iCba;
    }


CAiSoftKeyRenderer::CAiSoftKeyRenderer()
    {
    iAppUi = iAvkonAppUi;
    }




void CAiSoftKeyRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                        TInt aContent,
                                        const TDesC16& aText,
                                        TInt aIndex )
    {
    if( aPlugin.PublisherInfoL()->iUid == KShortcutPluginUid )
        {
        if( aContent == KAiScutContent[EAiScutContentShortcutShortCaption].id )
            {
            HBufC* temp = aText.AllocL();
            delete iText;
            iText = temp;

            UpdateSoftKeyL( aIndex, NULL );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }


void CAiSoftKeyRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                        TInt aContent,
                                        TInt aResource,
                                        TInt aIndex )
    {
    if( aPlugin.PublisherInfoL()->iUid == KShortcutPluginUid )
        {
        if( aContent == KAiScutContent[EAiScutContentShortcutShortCaption].id )
            {
            TInt internalId = KErrNotFound;
            switch( aResource )
                {
                case EAiScutResourceBackCaption:
                    {
                    internalId = R_NATIVEUI_SK_BACK;
                    break;
                    }
                case EAiScutResourceNewMsgShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_NEWMSG;
                    break;
                    }
                case EAiScutResourceNewEmailShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_EMAIL;
                    break;
                    }
                case EAiScutResourceNewSyncMLMailShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_SYNCMLMAIL;
                    break;
                    }
                case EAiScutResourceNewPostcardShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_MMSPOSTCARD;
                    break;
                    }
                case EAiScutResourceSelectMsgTypeShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_MSGTYPE;
                    break;
                    }
                case EAiScutResourceNewAudioMsgShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_AUDIOMSG;
                    break;
                    }
                case EAiScutResourceChangeThemeShortCaption:
                    {
                    internalId = R_NATIVEUI_SK_CHANGETHEME;
                    break;
                    }
                default:
                    {
                    User::Leave( KErrNotFound );
                    }
                }

            HBufC* temp = StringLoader::LoadL( internalId );
            delete iText;
            iText = temp;

            UpdateSoftKeyL( aIndex, NULL );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }


void CAiSoftKeyRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                        TInt aContent,
                                        const TDesC8& aBuf,
                                        TInt aIndex )
    {
    if( aPlugin.PublisherInfoL()->iUid == KShortcutPluginUid )
        {
        if( aContent == KAiScutContent[EAiScutContentShortcutSkIcon].id )
            {
            CGulIcon* icon = NULL;
            TPckg<CGulIcon*>( icon ).Copy( aBuf );
            // icon ownership is transferred
            UpdateSoftKeyL( aIndex, icon );
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }


void CAiSoftKeyRenderer::CreateDefaultSoftKeysL()
    {
    // If we already have cba, then do nothing.
    if ( iCba )
        {
        return;
        }

    // Otherwise create brand new cba.
    iCba = CEikButtonGroupContainer::NewL(
        CEikButtonGroupContainer::ECba,
        CEikButtonGroupContainer::EHorizontal,
        iAppUi,
        R_NATIVEUI_SOFTKEYS_DEFAULT,
        0 ); // Does not add to stack

    iCba->SetBoundingRect( iAppUi->ApplicationRect() );

    CCoeControl* cba = iCba->ButtonGroup()->AsControl();

    static_cast<CEikCba*>(cba)->SetSkinBackgroundId(
        KAknsIIDQsnBgAreaControlIdle );

    cba->MakeVisible( ETrue );

    iAppUi->RemoveFromStack( cba ); // Well, it is not in stack
    iAppUi->AddToStackL( cba,
                    KNativeUiCbaPriority,
                    ECoeStackFlagRefusesFocus | ECoeStackFlagRefusesAllKeys );
    }


void CAiSoftKeyRenderer::UpdateSoftKeyL( TInt aKey, CGulIcon* aIcon )
    {
    if( !iCba )
        {
        iCba = iAppUi->Cba();
        }

    // remove the locked flag if any
    aKey &= KScutBitMaskLocked;

    TInt buttonPosImage;
    TInt buttonPosText;
    TInt buttonCmd;

    if( aKey == KLeftSoftkeyId )
        {
        buttonPosImage = KControlArrayCBAButton1Posn;
        buttonPosText = KNativeUiLeftSoftkeyId;
        buttonCmd = ENativeUiSoftkeyLeft;
        }
    else if(aKey == KRightSoftkeyId)
        {
        buttonPosImage = KControlArrayCBAButton2Posn;
        buttonPosText = KNativeUiRightSoftkeyId;
        buttonCmd = ENativeUiSoftkeyRight;
        }
    else
        {
        delete aIcon;
        return;
        }

    if( aIcon )
        {
        CleanupStack::PushL( aIcon );

        TSize size( SoftkeySizeL( *iCba, buttonPosImage, ETrue ) );

        AknIconUtils::SetSize( aIcon->Bitmap(), size, EAspectRatioPreserved );
        AknIconUtils::SetSize( aIcon->Mask(), size, EAspectRatioPreserved );

        CEikImage* image = new ( ELeave ) CEikImage;
        image->SetPictureOwnedExternally( EFalse );
        // Bitmap and mask ownerships are transferred
        image->SetPicture( aIcon->Bitmap(), aIcon->Mask() );
        aIcon->SetBitmapsOwnedExternally( ETrue );

        // Image ownership is transferred
        EikSoftkeyImage::SetImage( iCba, *image, aKey == KLeftSoftkeyId );

        CleanupStack::PopAndDestroy( aIcon );
        }
    else
        {
        EikSoftkeyImage::SetLabel( iCba, aKey == KLeftSoftkeyId );
        iCba->SetCommandL( buttonPosText,
                            buttonCmd,
                            *iText );
        }

    iCba->DrawDeferred();
    }
