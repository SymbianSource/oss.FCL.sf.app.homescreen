/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for a menu
*
*/

// System includes
#include <utf.h>
#include <aknappui.h>
#include <e32base.h>
#include <coecntrl.h>
#include <eikbtgpc.h>
#include <eikcba.h>
#include <barsread.h>
#include <eiksoftkeyimage.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>

#include <AknStatuspaneUtils.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <eikspane.h>
#include <AknSgcc.h>
#include <aknview.h>

// User includes
#include "xnnodepluginif.h"
#include "xntype.h"
#include "xnproperty.h"

#include "xndompropertyvalue.h"
#include "xndomproperty.h"
#include "xndomlist.h"

#include "xnuienginepluginif.h"
#include "xncomponent.h"

#include "xnappuiadapter.h"
#include "xnbitmap.h"
#include "xnmenuadapter.h"
#include "xncomponentnodeimpl.h"
#include "c_xnutils.h"

#undef _AVKON_CBA_LSC

_LIT8( KWidgetSpecific, "Menu/WidgetSpecific" );
_LIT8( KViewSpecific, "Menu/ViewSpecific" );
_LIT8( KWidgetsHidden, "Menu/WidgetsHidden" );
_LIT8( KWidgetsShown, "Menu/WidgetsShown" );
_LIT8( KEditMode, "Menu/EditMode" );
_LIT8( KAlwaysShown, "Menu/AlwaysShown" );
_LIT8( KHsShowHelp, "hs_show_help" );

const TInt KMenuCommandFirst         = 6000;
const TInt KMenuCommandLast          = 6199;
const TInt KCBACommandFirst          = 6200;
const TInt KCBACommandSecond         = 6201;
const TInt KCBACommandMiddle         = 6202; 

const TInt KXnMenuArrayGranularity   = 6;
const TInt KWideScreenWidth          = 640;

_LIT( KMskIconDialer, "#dialer");

using namespace XnPropertyNames;

static CXnNodePluginIf* FindChildL(CXnNodePluginIf& aNode, 
    const TDesC8& aType, const TDesC8& aTarget = KNullDesC8);
	
class CXnSoftkeyItem: public CBase
    { 
public:           
    CXnSoftkeyItem() 
    : iNode( NULL ),         
      iPosition( 0 ), 
      iCommandId( 0 ),      
      iLabel( NULL ),
      iBitmap( NULL ),
      iMask( NULL ),
      iImageOn( EFalse ),  
      iInvertMask( EFalse ),
      iPreserveAspectRatio( EFalse ),            
      iUpdateAppearance( ETrue ),
      iOwnBitmaps( ETrue ),
      iDefault( EFalse )
#if 0 // MSK icon change
	  ,
      iId( KAknsIIDQsnIconColors ),
      iBmpFile( NULL ),
      iBmp( 0 ),
      iBmpM( 0 )
#endif // MSK icon change
        {
        }  
                              
    ~CXnSoftkeyItem()
        {
        Reset();            
        }
        
    void Reset()
        {
        delete iLabel;
        iLabel = NULL;        
        if ( iOwnBitmaps )
            {
            delete iBitmap;
            iBitmap = NULL;
            delete iMask;            
            iMask = NULL;            
            }
        else
            {
            iBitmap = NULL;
            iMask = NULL;
            }
        
        iImageOn = EFalse;        
        iInvertMask = EFalse;
        iPreserveAspectRatio = EFalse;
#if 0 // MSK icon change
        delete iBmpFile;
        iBmpFile = NULL;
#endif // MSK icon change
        }        
        
    CXnNodePluginIf* iNode;
    TInt             iPosition;
    TInt             iCommandId;    
    HBufC*           iLabel;
    CFbsBitmap*      iBitmap;
    CFbsBitmap*      iMask;    
    TBool            iImageOn;
    TBool            iInvertMask;
    TBool            iPreserveAspectRatio;    
    TBool            iUpdateAppearance;
    TBool            iOwnBitmaps;
    TBool            iDefault;
#if 0 // MSK icon change
    TAknsItemID      iId;
    HBufC*           iBmpFile;
    TInt32           iBmp;
    TInt32           iBmpM;
#endif // MSK icon change
    };


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
// IsNodeVisibleL
// 
// -----------------------------------------------------------------------------
//
static TBool IsNodeVisibleL(CXnNodePluginIf& aNode)
    {          
    CXnProperty* propVisibility = aNode.VisibilityL();
    
    if( propVisibility )
        {
        const TDesC8& visibility = propVisibility->StringValue();
    
        if( visibility == XnPropertyNames::style::common::visibility::KHidden )
            {
            return EFalse;
            }    
        }
        
    CXnProperty* propDisplay = aNode.DisplayL();
    
    if( propDisplay )
        {
        const TDesC8& display = propDisplay->StringValue();
        
        if( display == XnPropertyNames::style::common::display::KNone )
            {
            return EFalse;
            }
        }
        
    CXnProperty* propDisabled = aNode.GetPropertyL( XnPropertyNames::common::KDisabled );
    
    if( propDisabled )
        {
        const TDesC8& disabled = propDisabled->StringValue();
        
        if( disabled == XnPropertyNames::KTrue )
            {
            return EFalse;
            }
        }
        
    return ETrue;
    }

#ifdef _AVKON_CBA_LSC
// -----------------------------------------------------------------------------
// 
// Calculate touch sensitive softkey area for Top button (landscape)
// -----------------------------------------------------------------------------         
static TRect SoftkeyRectTopTouch()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    TAknLayoutRect layoutRect;

    TAknWindowComponentLayout rightAreaLayout(
            AknLayoutScalable_Avkon::area_side_right_pane( 0 ) );


    layoutRect.LayoutRect( screen, DoCompose( rightAreaLayout,
            AknLayoutScalable_Avkon::sctrl_sk_top_pane() ).LayoutLine() );
    TRect topSKRect( layoutRect.Rect() );
    
    layoutRect.LayoutRect( topSKRect, 
                    AknLayoutScalable_Avkon::aid_touch_sctrl_top().LayoutLine() );
    return layoutRect.Rect();
    }

// -----------------------------------------------------------------------------
// 
// Calculate touch sensitive softkey area for right button (portrait)
// -----------------------------------------------------------------------------         
static TRect SoftkeyRectRightTouch()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    TAknLayoutRect layoutRect;

    TAknWindowComponentLayout bottomAreaLayout(
            AknLayoutScalable_Avkon::area_bottom_pane( 0 ) );


    layoutRect.LayoutRect( screen, DoCompose( bottomAreaLayout,
            AknLayoutScalable_Avkon::control_bg_pane() ).LayoutLine() );
    TRect rightSKRect( layoutRect.Rect() );
    layoutRect.LayoutRect(
    		rightSKRect,
            AknLayoutScalable_Avkon::aid_touch_ctrl_right().LayoutLine() );
    return layoutRect.Rect();
    }

// -----------------------------------------------------------------------------
// 
// Calculate touch sensitive softkey area for left button (portrait)
// -----------------------------------------------------------------------------         
static TRect SoftkeyRectLeftTouch()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    TAknLayoutRect layoutRect;

    TAknWindowComponentLayout bottomAreaLayout(
            AknLayoutScalable_Avkon::area_bottom_pane( 0 ) );


    layoutRect.LayoutRect( screen, DoCompose( bottomAreaLayout,
            AknLayoutScalable_Avkon::control_bg_pane() ).LayoutLine() );
    TRect leftSKRect( layoutRect.Rect() );
    layoutRect.LayoutRect(
    		leftSKRect,
            AknLayoutScalable_Avkon::aid_touch_ctrl_left().LayoutLine() );
 
    return layoutRect.Rect();

    }

// -----------------------------------------------------------------------------
// 
// Calculate touch sensitive softkey area for Bottom button (landscape)
// -----------------------------------------------------------------------------         
static TRect SoftkeyRectBottomTouch()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    
    TAknLayoutRect layoutRect;
        
    TAknWindowComponentLayout rightAreaLayout( AknLayoutScalable_Avkon::area_side_right_pane( 0 ) );
        
    layoutRect.LayoutRect( screen, DoCompose( rightAreaLayout,
                    AknLayoutScalable_Avkon::sctrl_sk_bottom_pane() ).LayoutLine() );
    TRect bottomSKRect( layoutRect.Rect() );
            
    layoutRect.LayoutRect( bottomSKRect,
                    AknLayoutScalable_Avkon::aid_touch_sctrl_bottom().LayoutLine() );
    return layoutRect.Rect();
    }
#endif
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
            case CEikButtonGroupContainer::ELeftSoftkeyPosition:
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
            case CEikButtonGroupContainer::ERightSoftkeyPosition:
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
            case CEikButtonGroupContainer::ELeftSoftkeyPosition:
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
            case CEikButtonGroupContainer::ERightSoftkeyPosition:
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
            case CEikButtonGroupContainer::EMiddleSoftkeyPosition:
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
// Get softkey image rect
// -----------------------------------------------------------------------------
static TRect SoftkeyImageRect(CEikButtonGroupContainer& aContainer, TInt aPosition)
    {
    TRect ret;
#ifndef _AVKON_CBA_LSC    
    TAknLayoutRect softkeyImg;
    CEikCba* cba( static_cast< CEikCba* >( aContainer.ButtonGroup() ) );
    TRect parentRect( cba->Rect() );
    
    if(Layout_Meta_Data::IsLandscapeOrientation())
        {
        if(aPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition)
            {
            TAknWindowComponentLayout wcl = AknLayoutScalable_Avkon::control_pane_g1(2);
            softkeyImg.LayoutRect(parentRect,wcl.LayoutLine() ); 
            }
        else if(aPosition == CEikButtonGroupContainer::ERightSoftkeyPosition)
            {
            TAknWindowComponentLayout wcl = AknLayoutScalable_Avkon::control_pane_g2(2);
            softkeyImg.LayoutRect(parentRect,wcl.LayoutLine() );     
            }
        }
    else
        {
        if(aPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition)
            {
            TAknWindowComponentLayout wcl = AknLayoutScalable_Avkon::control_pane_g1(4);
            softkeyImg.LayoutRect(parentRect,wcl.LayoutLine() );  
            }
        else if(aPosition == CEikButtonGroupContainer::ERightSoftkeyPosition)
            {
            TAknWindowComponentLayout wcl = AknLayoutScalable_Avkon::control_pane_g2(4);
            softkeyImg.LayoutRect(parentRect,wcl.LayoutLine() );  
       
            }
        }
    ret = softkeyImg.Rect();
#else
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        if(aPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition)
            {
            ret = SoftkeyRectTopTouch();
            }
        else if(aPosition == CEikButtonGroupContainer::ERightSoftkeyPosition)
            {
            ret = SoftkeyRectBottomTouch();
            }
        }
    else // portrait
        {
        if(aPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition)
           {
           ret = SoftkeyRectLeftTouch();
           }
       else if(aPosition == CEikButtonGroupContainer::ERightSoftkeyPosition)
           {
           ret = SoftkeyRectRightTouch();
           }
        }
#endif    
    return ret; 
    }
    
// -----------------------------------------------------------------------------
// CopyBitmapLC
// Copy bitmap
// -----------------------------------------------------------------------------         
static CFbsBitmap* CopyBitmapL( const CFbsBitmap* aBitmap ) 
    {   
    CFbsBitmap* copy = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( copy );
    
    copy->Create( aBitmap->SizeInPixels(), aBitmap->DisplayMode() );

    CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( copy );
    CleanupStack::PushL( dev );
    
    CFbsBitGc* gc = CFbsBitGc::NewL();
    CleanupStack::PushL( gc );
    
    gc->Activate( dev );
    gc->BitBlt( TPoint( 0, 0 ), aBitmap );

    CleanupStack::PopAndDestroy( 2 ); // gc, dev
    
    CleanupStack::Pop( copy );
    
    return copy;
    }
       
// -----------------------------------------------------------------------------
// MenuTypeL
// This is used to deactivate "active applications" menu item using the theme
// Get the "menutype" property from menubar and check if it has value
// "menuoptions" (default) or "menuoptionsnotaskswapper" and return
// correct menu type.
// -----------------------------------------------------------------------------     
static CEikMenuBar::TMenuType MenuTypeL( CXnNodePluginIf& aMenubarNode )
    {
    CEikMenuBar::TMenuType menuType( CEikMenuBar::EMenuOptions );
    
    CXnProperty* property( aMenubarNode.GetPropertyL( XnPropertyNames::menu::KMenuType ) );
                                        
    if( property )
        {                
        if( property->StringValue() == XnPropertyNames::menu::menutype::KMenuOptionsNoTaskSwapper )
            {
            menuType = CEikMenuBar::EMenuOptionsNoTaskSwapper;
            }
        }

    return menuType;
    }
    
// -----------------------------------------------------------------------------
// FindChildL
// 
// -----------------------------------------------------------------------------
static CXnNodePluginIf* FindChildL( CXnNodePluginIf& aNode, 
    const TDesC8& aType, const TDesC8& aTarget )    
    {
    RPointerArray<CXnNodePluginIf> children( aNode.ChildrenL() );
    CleanupClosePushL( children );
    
    CXnNodePluginIf* retval( NULL );

    for( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNodePluginIf* child( children[i] );
        
        if( child->Type()->Type() == aType )
            {
            if( aTarget == KNullDesC8 )
                {
                retval = child;
                break;
                }
            else
                {
                CXnProperty* target( child->GetPropertyL( KXnTarget ) );
                
                if( target && target->StringValue() == aTarget )
                    {
                    retval = child;
                    break;
                    }       
                
                CXnProperty* source( child->GetPropertyL( KXnSource ) );
                                               
                if( source && source->StringValue() == aTarget )
                    {
                    retval = child;
                    break;
                    }                               
                }
            }
        }
    
    CleanupStack::PopAndDestroy( &children );
    
    return retval;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnMenuAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnMenuAdapter* CXnMenuAdapter::NewL( CXnControlAdapter* aParent, CXnNodePluginIf& aNode )
    {
	CXnMenuAdapter* self = new( ELeave ) CXnMenuAdapter;

    CleanupStack::PushL( self );
    self->ConstructL( aParent, aNode );
    CleanupStack::Pop( self );

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::ConstructL( CXnControlAdapter* /*aParent*/, CXnNodePluginIf& aNode )
    {
    iRootNode = &aNode;      
    
    CXnControlAdapter::ConstructL( aNode );        
    
    iUiEngine = iRootNode->UiEngineL();
    
    CAknAppUi* appui( iAvkonAppUi );
    
    iPreviousMenuPaneId = appui->StatusPane()->CurrentLayoutResId();
    
    CEikMenuBar* current( iEikonEnv->AppUiFactory()->SwapMenuBar( NULL ) );    
    appui->RemoveFromStack( current );                
	
    delete current;
	current = NULL;
		   
    InitSoftkeyItemsL();
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::CXnMenuAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnMenuAdapter::CXnMenuAdapter(): 
    iMenuItems( KXnMenuArrayGranularity ), 
    iIdCounter( KMenuCommandFirst )
    {
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::~CXnMenuAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnMenuAdapter::~CXnMenuAdapter()
    {
    StopDisplayingMenu();

    if ( iAvkonAppUi )
        {
        iAvkonAppUi->RemoveFromStack( iMenuBar );
        }

    if ( iMenuBar )
        {
        delete iMenuBar;
        }

    iSoftkeyItems.ResetAndDestroy();
    iMenuItems.Reset();
    }

// -----------------------------------------------------------------------------
// Create scaled bitmap from source bitmap.
// -----------------------------------------------------------------------------
//
void CreateScaledBitmapL(
    const TRect& aRect,
    CFbsBitmap*& aTrgBitmap, 
    CFbsBitmap* aSrcBitmap,
    TBool aPreserveAspectRatio )    
    {
    TRect destRect = aRect;
        
    if( aPreserveAspectRatio )
    	{
	    // Calculate the bitmap image dimensions so that it uses maximum space 
	    // of the given rectangle and maintains aspect ratio.
    	TInt srcHeight = aSrcBitmap->SizeInPixels().iHeight;
    	TInt srcWidth = aSrcBitmap->SizeInPixels().iWidth;
    	TReal scaleRatio( 1 ); //no scale as defaul

		// If any dimension is 0, then we do not bother to scale
    	if( aRect.Width() > 0 && aRect.Height() > 0 )
    		{
	    	TReal xRatio = ( (TReal )srcWidth / ( TReal )aRect.Width() );
	    	TReal yRatio = ( ( TReal )srcHeight / ( TReal )aRect.Height() );
	    	
	    	//Find out appropriate scaling factor
	   		xRatio > yRatio ? ( scaleRatio = xRatio ) : ( scaleRatio = yRatio );
    		}
    	
    	// Scale the size for target bitmap
    	destRect.SetHeight( srcHeight / scaleRatio );
		destRect.SetWidth( srcWidth / scaleRatio );
    	}

    aTrgBitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( aTrgBitmap );
    
    // It is allowed to create zero height or width bitmap.    
    TInt err( aTrgBitmap->Create( destRect.Size(), aSrcBitmap->DisplayMode() ) );

    if( err == KErrNone )
        {
		CXnUtils::ScaleBitmapL( destRect, aTrgBitmap, aSrcBitmap );
        }

    // we do not own the bitmap so just Pop.
    CleanupStack::Pop( aTrgBitmap );
    }    

// -----------------------------------------------------------------------------
// CXnMenuAdapter::PrepareSoftkeyImageL
// Make a copy of originals and scale them to right size.
// Finally place the scaled icon to right position of drawing area.
// -----------------------------------------------------------------------------         
CEikImage* CXnMenuAdapter::PrepareSoftkeyImageL( const CXnSoftkeyItem& aItem, const TSize& aSize )
    {                         
    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );
    
    // Start from bitmap
    CFbsBitmap* source( aItem.iBitmap );    
        
    TInt count( 0 );
           
    // Go through bitmap and mask
    for( TInt i = 0; i < 2; i++ )
        {
        if( !source )
            {
            source = aItem.iMask;
            continue;
            }
            
        CFbsBitmap* scaled( NULL );            

        TScaleMode mode( aItem.iPreserveAspectRatio ? EAspectRatioPreserved : EAspectRatioNotPreserved );
            
        if( AknIconUtils::IsMifIcon( source ) )
            {                       
            AknIconUtils::SetSize( source, aSize, mode );
            
            scaled = CopyBitmapL( source );
            }
        else
            {                            
            CreateScaledBitmapL( TRect( aSize ), scaled, source, mode == EAspectRatioPreserved );        
            }

        CleanupStack::PushL( scaled );                
        
        if( !aItem.iPreserveAspectRatio )                        
            {                          
            count++;
                                 
            if( i == 0 )
                {                
                bitmap = scaled;                                
                }
            else
                {
                mask = scaled;
                }                                                                         
            }
        else
            {                        
            CFbsBitmap* target = new ( ELeave ) CFbsBitmap;
            CleanupStack::PushL( target );

            // Target is size of whole draw area
            User::LeaveIfError( target->Create( aSize, source->DisplayMode() ) );
        	
    	    CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( target );
    	    CleanupStack::PushL( dev );
    	    
    	    CFbsBitGc* gc = CFbsBitGc::NewL();
    	    CleanupStack::PushL( gc );
    	    
    	    gc->Activate( dev );
    	    
    	    gc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    	    gc->SetPenStyle( CGraphicsContext::ENullPen );
    	    
    	    if( aItem.iInvertMask )
    	        {
        	    // White, with full transparent alpha channel
        	    gc->SetBrushColor( TRgb( 0x00FFFFFF, 0 ) );	       
    	        }
            else
                {
        	    // Black, with full transparent alpha channel
        	    gc->SetBrushColor( TRgb( 0x00000000, 0 ) );
                }	        
    	    
    	    // Make whole target area transparent
    	    gc->DrawRect( TRect( aSize ) );
    	   
    	   	TPoint iconPos;
    	   	
    	   	switch( AknLayoutUtils::CbaLocation() )
    	   	    {	   	    
    	   	    case AknLayoutUtils::EAknCbaLocationBottom:
    	   	        // Rsk must be moved to right, lsk is in correct place by default	   	        
    	   	        if ( aItem.iPosition != CEikButtonGroupContainer::ELeftSoftkeyPosition )
    	   	            {
    	   	            iconPos.iX = aSize.iWidth - scaled->SizeInPixels().iWidth;
    	   	            }
    	   	        break;	   	    	   	   
    	   	    case AknLayoutUtils::EAknCbaLocationRight:
    	   	        // Rsk and lsk must be moved to right	   	       
    	   	        iconPos.iX = aSize.iWidth - scaled->SizeInPixels().iWidth;
    	   	        break;
    	   	    case AknLayoutUtils::EAknCbaLocationLeft:
    	   	        // Already in correct position
    	   	    default:    	   	        
    	   	        break;    
    	   	    }
    	   	
    	   	// Align horizontally to center
            iconPos.iY = iconPos.iX ? 0 : ( ( aSize.iHeight - scaled->SizeInPixels().iHeight ) / 2 );
    	   		   	
    	   	gc->SetBrushStyle( CGraphicsContext::ENullBrush );
    	   			   	
    	   	// Blit image to correct position
    	   	gc->BitBlt( iconPos, scaled );    	

    		CleanupStack::PopAndDestroy( 2, dev );    		    		    		    		
    		
    		CleanupStack::Pop( target );
    		
    		CleanupStack::PopAndDestroy( scaled );
    		
    		CleanupStack::PushL( target );
            count++;                		    		
            
    		if( i == 0 )
    		    {
    		    bitmap = target;    		        		    
    		    }
            else
                {
                mask = target;
                }    		        		      
                
            
        	}                  
        
        // Go with mask next
        source = aItem.iMask;
        }    
        
    // Takes ownership of scaled bitmap and mask
    CEikImage* image = new ( ELeave ) CEikImage;
    
    image->SetPictureOwnedExternally( EFalse );
    image->SetPicture( bitmap, mask );        
                       
    CleanupStack::Pop( count ); 
    
    return image;
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetSoftkeyAppearanceL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetSoftkeyAppearanceL( CXnSoftkeyItem& aItem )
    {
#if 0 // MSK icon change
    if( IsNodeVisibleL( *aItem.iNode) )
        {
        if( aItem.iImageOn )
            {
            ChangeSoftkeyImageL( aItem );
            }
        else if( aItem.iLabel )// change label
            {
            TBool left( EFalse );
            if ( aItem.iPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition )
                {
                left = ETrue;
                EikSoftkeyImage::SetLabel( iContainer,left );
                }
            else if ( aItem.iPosition == CEikButtonGroupContainer::ERightSoftkeyPosition )
                {
                left = EFalse;
                EikSoftkeyImage::SetLabel( iContainer,left );
                }

            iContainer->SetCommandL( aItem.iPosition, aItem.iCommandId, *aItem.iLabel );
            }
        }
    }
#else // MSK icon change
    if( IsNodeVisibleL( *aItem.iNode) )
        {
        if( aItem.iPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition 
            || aItem.iPosition == CEikButtonGroupContainer::ERightSoftkeyPosition)
            {
            if(aItem.iImageOn)
                {
                ChangeSoftkeyImageL(aItem);
                }
            else if(aItem.iLabel)// change label
                {
                TBool left(EFalse);
                if(aItem.iPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition)
                    {
                    left = ETrue;
                    }
                EikSoftkeyImage::SetLabel( iContainer,left );
                iContainer->SetCommandL( aItem.iPosition, aItem.iCommandId, *aItem.iLabel );
                
                }
                                   
            }
        else if(aItem.iPosition == CEikButtonGroupContainer::EMiddleSoftkeyPosition)   
            {
            if( aItem.iLabel )
                {
                // Msk will have predefined image based on command id                                                            
                if( *aItem.iLabel == menu::KMSKDotImage16 )
                    {
                    aItem.iCommandId = KCBACommandFirst;
                    }                                           
                else if(*aItem.iLabel == KMskIconDialer)
                    {
#ifndef _AVKON_CBA_LSC                    
                    aItem.iCommandId = EAknSoftkeyDialler;
#endif                    
                    }
                else
                    {
                    aItem.iCommandId = KCBACommandMiddle;
                    }
                iContainer->SetCommandL( aItem.iPosition, aItem.iCommandId, *aItem.iLabel );
                }
            }
        }
    }
#endif // MSK icon change

// -----------------------------------------------------------------------------
// CXnMenuAdapter::InitSoftkeyItemsL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::InitSoftkeyItemsL()
    {                
    // No layout calulations needed
    iRootNode->SetLayoutCapable( EFalse );       
    
    // create softkeyitems to hold data
    
    RPointerArray< CXnNodePluginIf > children( iRootNode->ChildrenL() );
    CleanupClosePushL( children );
    CXnSoftkeyItem* item(0);
    for(TInt i=0;i<children.Count();++i)
        {
        if(children[i]->Type()->Type() == XnPropertyNames::softkey::KNodeName)
            {
            item = AppendSoftkeyItemL(children[i]);
            if(item)
                {
                item->iDefault = ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy( &children );
    
   }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::InitSoftkeyItemL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::InitSoftkeyItemImageL( CXnSoftkeyItem& aItem)
    {
    CXnNodePluginIf* node( aItem.iNode );
    
    // No layout calulations needed
    node->SetLayoutCapable( EFalse );                                                                 

#if 0 // MSK icon change
    if ( aItem.iPosition == CEikButtonGroupContainer::EMiddleSoftkeyPosition )
        {
        aItem.iImageOn = ETrue;
        return;
        }
#endif // MSK icon change

    CXnProperty* pathProp( node->PathL() );
    if( pathProp )
        {     
        HBufC* bitmapPath( NULL );                                  
        HBufC* maskPath( NULL );
        
        bitmapPath = pathProp->StringValueL();
        CleanupStack::PushL( bitmapPath );
        TInt count( 1 );
        
        CXnProperty* maskProp( node->MaskPathL() );
                                        
        if( maskProp )
            {
            maskPath = maskProp->StringValueL();                                
            CleanupStack::PushL( maskPath );
            count++;
            }
                    
        CFbsBitmap* bitmap( NULL );
        CFbsBitmap* mask( NULL );
                                                                    
        // Call base class to get bitmap and mask                        
        GetBitmapAndMask( bitmapPath ? *bitmapPath : KNullDesC(), 
                          maskPath ? *maskPath : KNullDesC(), 
                          bitmap, mask );                            
                    
        aItem.iImageOn = ETrue;
        aItem.iBitmap = bitmap;
        aItem.iMask = mask;
        
        CXnProperty* maskInvertedProp( node->GetPropertyL( XnPropertyNames::image::KMaskInverted ) );
        
        if( maskInvertedProp )
            {                        
            if( maskInvertedProp->StringValue() == XnPropertyNames::KTrue )
                {
                aItem.iInvertMask = ETrue;
                }
            }
        
        CXnProperty* aspectRatioProp( node->GetPropertyL( XnPropertyNames::image::KS60AspectRatio ) );
        
        if( aspectRatioProp )
            {
            if( aspectRatioProp->StringValue() == XnPropertyNames::image::KPreserve )
                {
                aItem.iPreserveAspectRatio = ETrue;
                }
            }
                
        CleanupStack::PopAndDestroy( count ); // path and/or mask
        }    
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::IsMskAvailable
// 
// -----------------------------------------------------------------------------
//
TBool CXnMenuAdapter::IsMskAvailable()
    {
    if( AknLayoutUtils::MSKEnabled() && Layout_Meta_Data::IsMSKEnabled() )
        {     
        return ETrue;
        }
    
    return EFalse;                
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::AddMenuItemL
// 
// -----------------------------------------------------------------------------
//
TInt CXnMenuAdapter::AddMenuItemL( TInt aParentIndex, 
    CXnNodePluginIf* aNode, RPointerArray< const TDesC8 >& aGroups )
    {
    if ( !aNode )
        {
        return 0;
        }
    
    const TDesC8& name( aNode->Type()->Type() );
    
    TInt retval( 0 );
    
    TBool showByGroup( EFalse );
    
    const TDesC8* id( NULL );
    
    CXnProperty* prop( aNode->IdL() );
    
    if( prop )
        {            
        id = &prop->StringValue();
        }
    else
        {
        // If menuitem doesn't have id, group doesn't affect to it
        showByGroup = ETrue;
        }
    
    if( name == KXnMenuItem || 
        name == KXnDynMenuItem || 
        name == KXnWidgetMenuItem )
        {               
        const TDesC8& ns( aNode->Namespace() );
                       
        for( TInt i = 0; !showByGroup && i < aGroups.Count(); i++ )
            {                       
            const TDesC8& groupId( *(aGroups)[i] );
        
            CXnNodePluginIf* group( iUiEngine->FindNodeByIdL( groupId, ns ) );
            
            if( group )
                {
                RPointerArray< CXnNodePluginIf > 
                    children( group->ChildrenL() );
                
                CleanupClosePushL( children );
                
                for( TInt j = 0; j < children.Count(); j++ )
                    {
                    CXnProperty* prop( children[j]->GetPropertyL( 
                            XnPropertyNames::action::KName ) );
                                        
                    if( prop )
                        {
                        const TDesC8& name( prop->StringValue() );
                            
                        if( name == *id )
                            {
                            // The menuitem belongs to defined group
                            showByGroup = ETrue;
                            break;                            
                            }
                        }
                    }
                
                CleanupStack::PopAndDestroy( &children );                
                }            
            }
        
        if( !showByGroup || !IsNodeVisibleL( *aNode ) )
            {
            // Menuitem is invisible  
            return retval;
            }
        }
               
    TXnMenuItem item;

    item.iAutomaticSP = ETrue;
        
    item.iParentIndex = aParentIndex;
    item.iData.iFlags = 0;

    CXnNodePluginIf* menuItem( NULL );
    
    if( name == KXnMenuItem || name == KXnDynMenuItem )
        {
        menuItem = aNode;
        }
    else if( name == KXnWidgetMenuItem )
        {
        TRAP_IGNORE( menuItem = FindWidgetMenuItemL( aNode ) );
        }
    
    if( menuItem )
        {
        const TDesC8& type( menuItem->Type()->Type() );
        
        // check first if label comes from view
        CXnProperty* label( aNode->LabelL() );
        
        if ( !label )
            {
            // get label from widget
            label = menuItem->LabelL();
            }
        
        if( label )
            {
            HBufC* itemLabel = label->StringValueL();
            item.iData.iText.Copy( *itemLabel );
            delete itemLabel;
            }
        else
            {
            item.iData.iText.Copy( KNullDesC );
            }
    
        TBool okToAdd( ETrue );
        
        if( type == KXnDynMenuItem )
            {
            okToAdd = EFalse;
            
            if( iObserver )
                {
                okToAdd = iObserver->DynInitMenuItemL( menuItem->AppIfL() );
                }            
            }
        
        if( okToAdd )
            {
            if ( id && *id == KHsShowHelp )
                {
                // Help command id must be EAknCmdHelp to get editing input options
                // shown correctly  
                item.iData.iCommandId = EAknCmdHelp;
                }
            else
                {
                item.iData.iCommandId = iIdCounter++;
                }
            
            item.iData.iCascadeId = 0;
            item.iNode = menuItem;
            item.iIsCascade = EFalse;
            iMenuItems.AppendL( item );
            
            retval = 1;
            }               
        }
    
    if( name == KXnMenu )
        {
        // main level menu or submenu (cascade)               
        item.iData.iCommandId = iIdCounter++;

        if( aParentIndex == -1 )
            {
            item.iData.iCascadeId = R_AVKON_MENUPANE_EMPTY;
            }
        else
            {
            item.iData.iCascadeId = iIdCounter++;
            }
        
        item.iNode = aNode;
        item.iIsCascade = ETrue;        
        iMenuItems.AppendL( item );

        TInt currentIndex( iMenuItems.Count() - 1 );
        
        RPointerArray< CXnNodePluginIf > children( aNode->ChildrenL() );
        CleanupClosePushL( children );

        TInt count( 0 );
        
        for( TInt i = 0; i < children.Count(); i++ )
            {
            count += AddMenuItemL( currentIndex, children[i], aGroups );
            }
        
        if( aParentIndex != -1 && count == 0 )
            {
            // Empty submenu, don't show it            
            iMenuItems.Delete( currentIndex );
            // Reverse counter as well
            iIdCounter -= 2;
            }
        
        CleanupStack::PopAndDestroy( &children );
        
        retval = count;
        }

    return retval;
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::PopulateMenuL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::PopulateMenuL()
    {
    iMenuItems.Reset();

    iIdCounter = KMenuCommandFirst;
       
    CXnNodePluginIf* focused( iUiEngine->FocusedNodeL() );
    
    const TDesC8& ns( iRootNode->Namespace() );
    
    TBool widgetSpecific( EFalse );
    
    for( ; focused ; focused = focused->ParentL() )
        {
        if( focused->AppIfL().InternalDomNodeType() == XnPropertyNames::KPlugin )
            {
            widgetSpecific = ETrue;
            break;
            }
        }

    TBool widgetsShown( iUiEngine->WidgetsVisible() );
    
    RPointerArray< const TDesC8 > groups;
    CleanupClosePushL( groups );
               
    if( iUiEngine->EditMode() )
        {
        groups.AppendL( &KEditMode );
        }
    else
        {
        groups.AppendL( widgetSpecific ? &KWidgetSpecific : &KViewSpecific );
        groups.AppendL( widgetsShown ? &KWidgetsShown : &KWidgetsHidden );        
        }
    
    groups.AppendL( &KAlwaysShown );
         
    // Recursively add menuitems
    AddMenuItemL( -1, iMenuBarNode, groups );
    
    CleanupStack::PopAndDestroy( &groups );
    }
  
// -----------------------------------------------------------------------------
// CXnMenuAdapter::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CXnMenuAdapter::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {    
    TKeyResponse resp( EKeyWasNotConsumed );
    
    if( IsMenuFocused() && aKeyEvent.iScanCode == EStdKeyNo )
        {
        StopDisplayingMenu();            
        
        iKeyEventNode = NULL;
        
        return resp;
        }        

    const TDesC8* pos( &KNullDesC8 );
    
    if( aKeyEvent.iScanCode == EStdKeyDevice0 )
        {
        pos = &XnPropertyNames::softkey::type::KLeft;
        }
    else if( aKeyEvent.iScanCode == EStdKeyDevice1 )
        {
        pos = &XnPropertyNames::softkey::type::KRight;
        }
    else if( aKeyEvent.iScanCode == EStdKeyDevice3 )
        {
        pos = &XnPropertyNames::softkey::type::KMiddle;
        }            
    
    if( aType == EEventKeyDown )
        {                                  
        iKeyEventNode = FindSoftkeyNodeL( *pos );           
        }
            
    if( aKeyEvent.iScanCode == EStdKeyDevice0 || 
        aKeyEvent.iScanCode == EStdKeyDevice1 || 
        aKeyEvent.iScanCode == EStdKeyDevice3 )
        {
        CXnNodePluginIf* node(  FindSoftkeyNodeL( *pos ) );
        
        if( node && node == iKeyEventNode )
            {
            // Let base class handle the event           
            resp = CXnControlAdapter::OfferKeyEventL( aKeyEvent, aType );                                        
            }            
        }
        
    if( aType == EEventKeyUp )
        {
        iKeyEventNode = NULL;
        }
        
    return resp;
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {          
    CXnNodePluginIf* node( FindSoftkeyItemByPosition( aPointerEvent.iParentPosition ) );
    
    if( aPointerEvent.iType  == TPointerEvent::EButton1Down )
        {
        // The sofkey node which received the button down event
        iPointerEventNode = node;        
        }
        
    if( node && node == iPointerEventNode )
        {
        iUiEngine->DisableRenderUiLC();
        
        // Handle event if location is in the grabbing node's area    
        CXnControlAdapter::HandlePointerEventL( aPointerEvent );
        
        CleanupStack::PopAndDestroy();
        }
            
    if( aPointerEvent.iType  == TPointerEvent::EButton1Up )
        {
        // Reset grabbing node after the up event
        iPointerEventNode = NULL;
        }    
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::DoEnterPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
void CXnMenuAdapter::DoEnterPowerSaveModeL(TModeEvent /*aEvent*/)
    {
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::DoExitPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
void CXnMenuAdapter::DoExitPowerSaveModeL(TModeEvent /*aEvent*/)
    {
    } 
       
// -----------------------------------------------------------------------------
// CXnMenuAdapter::RestoreMenuL
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::RestoreMenuL( CCoeControl* aMenuControl, TInt aResourceId,        
    TMenuType aType )
    {
    // let all menu pane resources propogate to default implementation,
    // otherwise panic when trying to display menu
    if ( aType == MEikMenuObserver::EMenuPane )
        {
        if ( !( KMenuCommandFirst <= aResourceId && 
                aResourceId <= KMenuCommandLast ) 
            && aResourceId != R_AVKON_MENUPANE_EMPTY  )
            {
            MEikMenuObserver::RestoreMenuL( aMenuControl,aResourceId, aType );
            }
        else
            {
            AddItemsToMenuPaneL( ( CEikMenuPane* )aMenuControl, aResourceId );            
            }
        }  
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::DynInitMenuPaneL( TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_AVKON_MENUPANE_EMPTY )
        {
        RestoreMenuL( aMenuPane, aResourceId, MEikMenuObserver::EMenuPane );
        }    
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetEmphasis
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetEmphasis( CCoeControl* aMenuControl, 
    TBool aEmphasis )
    {
    if ( iMenuBar && aMenuControl == iMenuBar )
        {   
        CXnAppUiAdapter* appui( 
            static_cast< CXnAppUiAdapter* >( iAvkonAppUi ) );

        if ( aEmphasis && !iMenuShown )
            {
            iMenuShown = ETrue;

            TRAP_IGNORE( appui->AddToStackL( appui->View(),
                                             iMenuBar,
                                             ECoeStackPriorityMenu ) );

            iMenuBar->MakeVisible( ETrue );
            }
        else if ( !aEmphasis && iMenuShown )
            {
            iMenuShown = EFalse;
         
            appui->RemoveFromStack( iMenuBar );
            
            iMenuBar->MakeVisible( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::ProcessCommandL( TInt aCommand )
    {     
    if ( ( KMenuCommandFirst <= aCommand && aCommand <= KMenuCommandLast ) || 
        aCommand == EAknCmdHelp )
        {
        // find item for this command
        for ( TInt i = 0; i< iMenuItems.Count(); i++ )
            {
            if ( iMenuItems[i].iData.iCommandId == aCommand )
                {
                CXnNodePluginIf* node( iMenuItems[i].iNode );
                
                if ( node )
                    {
                    StopDisplayingMenu();
                    
                    iUiEngine->DisableRenderUiLC();
                    
                    node->SetStateL( XnPropertyNames::style::common::KActive );
                    
                    CleanupStack::PopAndDestroy();                 
                    
                    break;
                    }                                    
                }
            }        
        }
    else
        {
        StopDisplayingMenu();

        if( aCommand == EEikCmdEditPaste )
            {
            // Paste event for AknCcpuSupport
            TKeyEvent pasteEvent = 
                { EKeyF18, EEikCmdEditPaste, EModifierCtrl, 1 };  
            
            iCoeEnv->SimulateKeyEventL( pasteEvent, EEventKey );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetContainerL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetContainerL( CEikButtonGroupContainer& aContainer )
    {
    TBool updateNeeded( EFalse );
    
    TInt statusPaneId( iAvkonAppUi->StatusPane()->CurrentLayoutResId() );
    
    if(iPreviousMenuPaneId != statusPaneId)
        {
        updateNeeded = ETrue;
        iPreviousMenuPaneId = statusPaneId;
        }
    
 	if((&aContainer) && (iContainer != &aContainer || updateNeeded))
        {
        iUpdateLskAppearance = ETrue;
        iUpdateMskAppearance = ETrue;
        iUpdateRskAppearance = ETrue;
        }
 	
	iContainer = &aContainer;
	
	UpdateSoftkeyAppearancesL();
	    
	if (iContainer)
	    {
        iContainer->DrawNow();        
	    }
    }

//--------------------------------------------------------------
// CXnMenuAdapter::IsMenuFocused
// ------------------------------------------------------------        
TBool CXnMenuAdapter::IsMenuFocused()
    {
    return iMenuShown;
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::TryDisplayingMenuL
// Displays options menu if it is defined for the softkey item
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::TryDisplayingMenuL( const TDesC& aMenuNodeId )
    {
    HBufC8* id = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aMenuNodeId );
    CleanupStack::PushL( id );
    CXnNodePluginIf* node( iUiEngine->FindNodeByIdL( *id, iRootNode->Namespace() ) );
    CleanupStack::PopAndDestroy( id );
    
    if( node && node->Type()->Type() == KXnMenu )
        {
        TryDisplayingMenuL();
        }
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::TryDisplayingMenuL
// Displays options menu if it is defined for the softkey item
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::TryDisplayingMenuL()
    {
    StopDisplayingMenu();
    
    CreateMenuBarL();    
    
    if ( iMenuBar )
        {    
        CEikMenuBarTitle* title = new( ELeave ) CEikMenuBarTitle;
        CleanupStack::PushL( title );
            
        title->iData.iMenuPaneResourceId = R_AVKON_MENUPANE_EMPTY;    
        title->iTitleFlags = 0;
        
        // this cannot be done earlyer, than this callback
        iMenuBar->TitleArray()->AddTitleL( title );
        CleanupStack::Pop( title );
        
        iMenuBar->TryDisplayMenuBarL();
        }    
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::StopDisplayingMenu
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::StopDisplayingMenu()
    {
    if( iMenuShown && iMenuBar )
        {
        iMenuBar->StopDisplayingMenuBar();
        }
    }

// ---------------------------------------------------------------------------
// CXnMenuAdapter::CreateMenuBarL
// ---------------------------------------------------------------------------
//
void CXnMenuAdapter::CreateMenuBarL()
    {
    if ( iMenuBar )
        {
        // All ready created
        return;
        }
    
    // Create
    RPointerArray< CXnNodePluginIf > children( iRootNode->ChildrenL() );
    CleanupClosePushL( children );
      
    for( TInt i = 0; i < children.Count(); i++ )
        {       
        CXnNodePluginIf* node( children[i] );
        
        TPtrC8 type( node->Type()->Type() );
                                                       
        // Currently only one menubar is allowed,
        // but it can be assigned to any of the CBAs
        // To change this, we have to create and maintain a list of menubars 
        if( type == KXnMenu )
            {
            iMenuBarNode = node;
            
            iMenuBar = new ( ELeave ) CEikMenuBar;
            iMenuBar->ConstructL( this );
            iMenuBar->SetMenuType( MenuTypeL( *iRootNode ) );
                        
            // All done
            break;
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

//--------------------------------------------------------------
// CXnMenuAdapter::HandleResourceChange
// Handles a change to the control's resources of type aType
// ------------------------------------------------------------    
void CXnMenuAdapter::HandleResourceChange( TInt aType )
    {	
    if( iMenuBar )
        {
        iMenuBar->HandleResourceChange( aType );
        }   	
    }
   
// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetSoftKeyImageL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetSoftKeyImageL(
    CFbsBitmap* aBitmap, 
    CFbsBitmap* aMask, 
    XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos, 
    CXnNodeAppIf* aNode,
    TBool aPreserveAspectRatio, TBool aInvertMask,
    TBool aTransferOwnership)
    {        
    if( ( !aBitmap && !aMask ) || aPos == XnMenuInterface::MXnMenuInterface::ECenter )
        {
        // No images present and image can be set only to lsk or rsk         
        User::Leave( KErrArgument );
        }

    CXnSoftkeyItem* softkey(NULL);
    
    if( aPos == XnMenuInterface::MXnMenuInterface::ELeft)
        {
        if( aNode )
            {
            softkey = SoftkeyItemL( aNode );
            }
        else
            {
            softkey = SoftkeyItemL(CEikButtonGroupContainer::ELeftSoftkeyPosition);
            }
        iUpdateLskAppearance = ETrue;
        }
    else if(aPos == XnMenuInterface::MXnMenuInterface::ERight)
        {
        if( aNode )
            {
            softkey = SoftkeyItemL( aNode );
            }
        else
            {
            softkey = SoftkeyItemL(CEikButtonGroupContainer::ERightSoftkeyPosition);
            }
        iUpdateRskAppearance = ETrue;
        }
    
    if(softkey)
        {
        softkey->Reset();
        softkey->iImageOn = ETrue;
                                                                    
        // Save new icon data
        softkey->iBitmap = aBitmap;
        softkey->iMask = aMask;
        softkey->iPreserveAspectRatio = aPreserveAspectRatio;
        softkey->iInvertMask = aInvertMask;
        softkey->iOwnBitmaps = aTransferOwnership;
        // Image set, mark node as dirty
        softkey->iNode->SetDirtyL();
                
        }
    }

#if 0 // MSK icon change
// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetSoftKeyImageL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetSoftKeyImageL( const TAknsItemID& aId,
    const TDesC& aBmpFile,
    const TInt32 aBmp,
    const TInt32 aBmpM,
    XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos, 
    TBool /*aEnable*/ )
    {
    CXnSoftkeyItem* softkey( NULL );

    switch( aPos )
        {
        case XnMenuInterface::MXnMenuInterface::ECenter:
            {
            softkey = SoftkeyItemL( CEikButtonGroupContainer::EMiddleSoftkeyPosition );           
            iUpdateMskAppearance = ETrue;
            break;
            }
        // Currently only MSK icon updating supported here
        case XnMenuInterface::MXnMenuInterface::ELeft:
        case XnMenuInterface::MXnMenuInterface::ERight:
        default:
            {
            break;
            }
        }

    if( softkey )
        {
        softkey->Reset();
        softkey->iImageOn = ETrue;

        // Save new icon data
        softkey->iBitmap = NULL;
        softkey->iMask = NULL;
        softkey->iPreserveAspectRatio = EFalse;
        softkey->iInvertMask = EFalse;
        softkey->iOwnBitmaps = ETrue;
        delete softkey->iBmpFile;
        softkey->iBmpFile = NULL;
        softkey->iBmpFile = aBmpFile.AllocL();
        softkey->iId.Set( aId );
        softkey->iBmp = aBmp;
        softkey->iBmpM = aBmpM;

        // Image set, mark node as dirty
        softkey->iNode->SetDirtyL();
        }
    }
#endif // MSK icon change

// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetSoftKeyTextL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetSoftKeyTextL( const TDesC& aText, 
    XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos )
    {       
    CXnSoftkeyItem* softkey(NULL);
    
    switch( aPos )
        {
        case XnMenuInterface::MXnMenuInterface::ELeft:
            softkey = SoftkeyItemL(CEikButtonGroupContainer::ELeftSoftkeyPosition);
            iUpdateLskAppearance = ETrue;
            break;
        case XnMenuInterface::MXnMenuInterface::ERight:
            softkey = SoftkeyItemL(CEikButtonGroupContainer::ERightSoftkeyPosition);
            iUpdateRskAppearance = ETrue;
            break;
        case XnMenuInterface::MXnMenuInterface::ECenter:  
            softkey = SoftkeyItemL(CEikButtonGroupContainer::EMiddleSoftkeyPosition);           
            iUpdateMskAppearance = ETrue;
            break;
        default:     
            break;
        }
    if(softkey)
        {
        softkey->Reset();
                                              
        softkey->iImageOn = EFalse;    
                                              
        softkey->iLabel = aText.AllocL();
        // Text set, mark parent as dirty
        softkey->iNode->SetDirtyL();
       }
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetSoftKeyL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::SetSoftKeyL( CXnNodePluginIf* aSoftkeyNode, 
    XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos )
    {

    CXnSoftkeyItem* item = AppendSoftkeyItemL(aSoftkeyNode);
    switch(aPos)
        {
        case XnMenuInterface::MXnMenuInterface::ELeft:
            if(item)
                {
                iVisibleLSK = item;
                }
            else
                {
                iVisibleLSK = NULL;                
                }
            iUpdateLskAppearance = ETrue;
            break;
        case XnMenuInterface::MXnMenuInterface::ECenter:
            if(item)
                {
                iVisibleMSK = item;
                }
            else
                {
                iVisibleMSK = NULL;                
                }
            iUpdateMskAppearance = ETrue;
            break;
        case XnMenuInterface::MXnMenuInterface::ERight:
            if(item)
                {
                iVisibleRSK = item;
                }
            else
                {
                iVisibleRSK = NULL;                
                }
            iUpdateRskAppearance = ETrue;
            break;
        }
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::SoftKeyL
// 
// -----------------------------------------------------------------------------
//
CXnNodePluginIf* CXnMenuAdapter::SoftKeyL( XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos )
    {
    CXnNodePluginIf* node(NULL);
    CXnSoftkeyItem* item = SoftkeyItemL(aPos);
    if(item)
        {
        node = item->iNode;
        }
    return node;
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::HandleScreenDeviceChangedL
// 
// -----------------------------------------------------------------------------
//
void CXnMenuAdapter::HandleScreenDeviceChangedL()
    {
    CXnSoftkeyItem* left = 
        SoftkeyItemL(CEikButtonGroupContainer::ELeftSoftkeyPosition);
    
    CXnSoftkeyItem* middle = 
            SoftkeyItemL(CEikButtonGroupContainer::EMiddleSoftkeyPosition);
    CXnSoftkeyItem* right = 
            SoftkeyItemL(CEikButtonGroupContainer::ERightSoftkeyPosition);
            
    if(left && left->iImageOn)
        {
        iUpdateLskAppearance = ETrue;
        left->iNode->SetDirtyL();
        }
    if(right && right->iImageOn)
        {
        iUpdateRskAppearance = ETrue;
        right->iNode->SetDirtyL();
        }
    if(middle && middle->iImageOn)
        {
        iUpdateMskAppearance = ETrue;
        middle->iNode->SetDirtyL();
        }
    CXnControlAdapter::HandleScreenDeviceChangedL();
    } 
      
// -----------------------------------------------------------------------------
// CXnMenuAdapter::HandleMenuPropertyChangeL
// Change softkey appearance
// -----------------------------------------------------------------------------
//    
void CXnMenuAdapter::HandleMenuPropertyChangeL(CXnNodePluginIf* aNode, CXnProperty* aProperty)
    {
    if ( !aNode )
        {
        return;
        }
           
    const TDesC8& type( aNode->Type()->Type() ); 
    
    if ( type != softkey::KNodeName)
        {
        return;
        }
            
    // Find target softkey
    CXnSoftkeyItem* softkey( SoftkeyItemL( aNode ) );
        
    if ( !softkey )
        {
        return;
        }
    
    SetUpdateAppearanceL( softkey );  
    
    const TDesC8* name( &KNullDesC8() );
    
    if ( aProperty )
        {
        name = &aProperty->Property()->Name();
        }
    
    if ( *name == menu::KLabel )
       {             
       softkey->Reset();
       softkey->iImageOn = EFalse;    
       softkey->iLabel = aProperty->StringValueL();              
       }
    else if ( *name == style::common::KDisplay )
        {        
        if ( aProperty->StringValue() == XnPropertyNames::style::common::display::KNone )
            {
            if ( softkey->iPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition ) 
                {
                EikSoftkeyImage::SetLabel( iContainer,ETrue );
                iContainer->SetCommandL( CEikButtonGroupContainer::ELeftSoftkeyPosition,KCBACommandFirst, KNullDesC );                
                }
            else if ( softkey->iPosition == CEikButtonGroupContainer::ERightSoftkeyPosition )
                {
                EikSoftkeyImage::SetLabel( iContainer,EFalse );
                iContainer->SetCommandL( CEikButtonGroupContainer::ERightSoftkeyPosition,KCBACommandSecond, KNullDesC );                
                }
            else if ( softkey->iPosition == CEikButtonGroupContainer::EMiddleSoftkeyPosition )
                {
                iContainer->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition,KCBACommandMiddle, KNullDesC );                
                }
            }
        }
    }    

// -----------------------------------------------------------------------------
// CXnMenuAdapter::SetObserver
// 
// -----------------------------------------------------------------------------
//    
void CXnMenuAdapter::SetObserver( XnMenuInterface::MXnMenuObserver& aObserver )   
    {
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::RemoveObserver
// 
// -----------------------------------------------------------------------------
//    
void CXnMenuAdapter::RemoveObserver()   
    {
    iObserver = NULL;
    }

// ---------------------------------------------------------------------------
// CXnMenuAdapter::FindSoftKeyNodeByPosition
// ---------------------------------------------------------------------------
//
TBool CXnMenuAdapter::FindSoftKeyNodeByPosition( 
    const TPoint& aPosition, 
    CXnNodePluginIf*& aNode )
    {
    CXnNodePluginIf* ret = FindSoftkeyItemByPosition(aPosition);
    aNode = ret;
    return (ret?ETrue:EFalse);
    }
// ---------------------------------------------------------------------------
// CXnMenuAdapter::ButtonTouchableRectByPosition
// Gets a CBA button rectangle. Touchable are is larger than controls rect
// ---------------------------------------------------------------------------
//
TRect CXnMenuAdapter::ButtonTouchableRectByPosition( TInt aPosition)
    {
    TRect rect( 0, 0, 0, 0 );
    
    TRect button1Rect( 0, 0, 0, 0 );
    TRect button2Rect( 0, 0, 0, 0 );
    TRect buttonMSKRect( 0, 0, 0, 0 );
     
    // In touch layouts the button areas are read from the
    // layout data because they are larger than the actual
    // control size.
#ifndef _AVKON_CBA_LSC
    CCoeControl* container = iContainer->ButtonGroup()->AsControl();
    TRect containerRect(container->PositionRelativeToScreen(),container->Size());
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        containerRect,
        AknLayoutScalable_Avkon::control_pane_g6( 0 ).LayoutLine() );
    button1Rect = layoutRect.Rect();
    
    layoutRect.LayoutRect(
        containerRect,
        AknLayoutScalable_Avkon::control_pane_g8( 0 ).LayoutLine() );
    button2Rect = layoutRect.Rect();

    layoutRect.LayoutRect(
        containerRect,
        AknLayoutScalable_Avkon::control_pane_g7( 0 ).LayoutLine() );
    buttonMSKRect = layoutRect.Rect();
#else           
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        button1Rect = SoftkeyRectTopTouch();
        button2Rect= SoftkeyRectBottomTouch();
        
        }
    else // portrait
        {
        button1Rect = SoftkeyRectLeftTouch();
        button2Rect = SoftkeyRectRightTouch();
        }
#endif
    
    switch ( aPosition )
        {
        case CEikButtonGroupContainer::ELeftSoftkeyPosition:
            {
            rect = button1Rect;
            break;
            }

        case CEikButtonGroupContainer::ERightSoftkeyPosition:
            {
            rect = button2Rect;
            break;
            }

        case CEikButtonGroupContainer::EMiddleSoftkeyPosition:
            {
            rect = buttonMSKRect;
            break;
            }

        default:
            {
            break;
            }
        }
        
        
    return rect;
    }

// ---------------------------------------------------------------------------
// CXnMenuAdapter::FindSoftkeyNodeL
// ---------------------------------------------------------------------------
//
CXnNodePluginIf* CXnMenuAdapter::FindSoftkeyNodeL(const TDesC8& aNodeType)
    {
    CXnNodePluginIf* ret(0);
    
    RPointerArray< CXnNodePluginIf > children( iRootNode->ChildrenL() );
    CleanupClosePushL( children );
    for(TInt i=0;i<children.Count();++i)
        {
        if(children[i]->Type()->Type() == XnPropertyNames::softkey::KNodeName)
            {
            if( SoftkeyPlaceL(*children[i]) == aNodeType 
                && IsNodeVisibleL( *children[i]) )
                {
                ret = children[i];
                break;
                }
            
            }
        }
    CleanupStack::PopAndDestroy( &children );
    return ret;
    }

// ---------------------------------------------------------------------------
// CXnMenuAdapter::UpdateSoftkeyAppearancesL
// ---------------------------------------------------------------------------
//
void CXnMenuAdapter::UpdateSoftkeyAppearancesL()
    {
    
    if(iUpdateLskAppearance)
        {
        CXnSoftkeyItem* left = 
            SoftkeyItemL(CEikButtonGroupContainer::ELeftSoftkeyPosition);
        if( left )
            {
            SetSoftkeyAppearanceL(*left);
            }
        iUpdateLskAppearance = EFalse;
        }
    if(iUpdateRskAppearance)
        {
        CXnSoftkeyItem* right = 
            SoftkeyItemL(CEikButtonGroupContainer::ERightSoftkeyPosition);
        if(right)
            {
            SetSoftkeyAppearanceL(*right);            
            }
        iUpdateRskAppearance = EFalse;
        }
    if(iUpdateMskAppearance)
        {
        CXnSoftkeyItem* middle = 
            SoftkeyItemL(CEikButtonGroupContainer::EMiddleSoftkeyPosition);
        if(middle)
            {
            SetSoftkeyAppearanceL(*middle);                
            }
        iUpdateMskAppearance = EFalse;
        }
    }
// ---------------------------------------------------------------------------
// CXnMenuAdapter::ChangeSoftkeyImageL
// ---------------------------------------------------------------------------
//
void CXnMenuAdapter::ChangeSoftkeyImageL(const CXnSoftkeyItem& aSoftkey)
    {
#if 0 // MSK icon change
    if ( aSoftkey.iPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition || 
            aSoftkey.iPosition == CEikButtonGroupContainer::ERightSoftkeyPosition )
        {
#endif // MSK icon change
        TSize size;
        if(AknLayoutUtils::PenEnabled())
            {
            size = SoftkeyImageRect(*iContainer,aSoftkey.iPosition).Size();
            }
        else
            {
            size = SoftkeyRectL( *iContainer,aSoftkey.iPosition, aSoftkey.iImageOn ).Size();
            }

        CEikImage* image( PrepareSoftkeyImageL( aSoftkey, size ) );
        TBool left(ETrue);

        if(aSoftkey.iPosition != CEikButtonGroupContainer::ELeftSoftkeyPosition)
            {
            left = EFalse;
            }
        EikSoftkeyImage::SetImage( iContainer, *image, left );                        
#if 0 // MSK icon change
        }
    else if ( aSoftkey.iBmpFile )
        {
        CEikCba* cba = static_cast< CEikCba* >( iContainer->ButtonGroup() );
        // use iAvkonAppUi
        CAknAppUi* appUi = static_cast<CAknAppUi*>( CEikonEnv::Static()->AppUi() );
        cba->UpdateMSKIconL(
                aSoftkey.iId,
                aSoftkey.iBmpFile->Des(),
                aSoftkey.iBmp,
                aSoftkey.iBmpM,
                ETrue );
        cba->DrawNow();
        }
#endif // MSK icon change
    }

// ---------------------------------------------------------------------------
// CXnMenuAdapter::AddItemsToMenuPaneL
// ---------------------------------------------------------------------------
//
void CXnMenuAdapter::AddItemsToMenuPaneL(CEikMenuPane* aMenuPane, TInt aResourceId)
    {   
   TInt parentIndexForNotification( 0 );
   
   for ( TInt i = 0; i< iMenuItems.Count(); i++ )
       {
       // find items belong to this pane
       TInt parent( iMenuItems[i].iParentIndex );
       
       if ( parent >= 0 )
           {
           TInt cid( iMenuItems[parent].iData.iCascadeId );
           
           if ( cid == aResourceId )
               {
               if ( parent > 0 )
                   {
                   parentIndexForNotification = parent;
                   break;
                   }
               }
           }
       }
   
   TBool selectionEnabled( EFalse );
   TBool isRadio( EFalse );            
   
   if ( parentIndexForNotification != 0 )
       {
       CXnNodePluginIf* node( iMenuItems[parentIndexForNotification].iNode );
       
       CXnProperty* property( node->GetPropertyL( _L8( "selectiontype" ) ) );
       
       if ( property && property->StringValue() == _L8( "multi" ) )
           {
           selectionEnabled = ETrue;
           }
       else if ( property && property->StringValue() == _L8( "single" ) )
           {
           selectionEnabled = ETrue;
           isRadio = ETrue;
           }
           
       node->SetStateL( XnPropertyNames::style::common::KActive );
       node->UnsetStateL( XnPropertyNames::style::common::KActive );
       }
       
   PopulateMenuL();        
   
   for ( TInt i = 0; i< iMenuItems.Count(); i++ )
       {
       // find items belong to this pane
       TInt parent( iMenuItems[i].iParentIndex );
       
       if ( parent >= 0 )
           {
           TInt cid( iMenuItems[parent].iData.iCascadeId );
           
           if ( cid == aResourceId )
               {
               if ( selectionEnabled )
                   {
                   if ( !isRadio )
                       {
                       iMenuItems[i].iData.iFlags |= 
                           EEikMenuItemCheckBox;
                       }
                   else
                       {
                       iMenuItems[i].iData.iFlags |= 
                           EEikMenuItemRadioStart|
                           EEikMenuItemRadioMiddle|
                           EEikMenuItemRadioEnd;
                       }
                   }
                   
               aMenuPane->AddMenuItemL( iMenuItems[i].iData );
               
               if ( selectionEnabled )
                   {
                   CXnProperty* property( 
                       iMenuItems[i].iNode->GetPropertyL( XnPropertyNames::menu::KChecked ) );                           
                   
                   if ( property && property->StringValue() == XnPropertyNames::KTrue )
                       {
                       aMenuPane->SetItemButtonState(
                           iMenuItems[i].iData.iCommandId, EEikMenuItemSymbolOn );                                         
                       }
                   }
               }
           }
       } // for end      
    }

// ---------------------------------------------------------------------------
// CXnMenuAdapter::AppendSoftkeyItemL
// ---------------------------------------------------------------------------
//
CXnSoftkeyItem* CXnMenuAdapter::AppendSoftkeyItemL(CXnNodePluginIf* aNode)
    {
    if(!aNode)
        {
        return NULL;
        }
    CXnProperty* property = 
        aNode->GetPropertyL( XnPropertyNames::softkey::KTypeAttribute );     
    if(!property)
        {
        return NULL;
        }
    const TDesC8& skType = property->StringValue();
    if( skType != XnPropertyNames::softkey::type::KRight &&
        skType != XnPropertyNames::softkey::type::KLeft &&
        skType != XnPropertyNames::softkey::type::KMiddle )
        {
        return NULL;
        }
    for( TInt i=0; i<iSoftkeyItems.Count(); i++ )
        {
        if( iSoftkeyItems[i]->iNode == aNode )
            {
            return iSoftkeyItems[i];
            }
        }        
    CXnSoftkeyItem* softkey(0);
    softkey = new (ELeave) CXnSoftkeyItem;
    CleanupStack::PushL(softkey);
    
    if ( skType == XnPropertyNames::softkey::type::KRight )
       {
       softkey->iPosition = CEikButtonGroupContainer::ERightSoftkeyPosition;
       softkey->iCommandId = KCBACommandSecond;
       }
    else if ( skType == XnPropertyNames::softkey::type::KLeft )
       {
       softkey->iPosition = CEikButtonGroupContainer::ELeftSoftkeyPosition;
       softkey->iCommandId = KCBACommandFirst;
       }
    else 
        {
        softkey->iPosition = CEikButtonGroupContainer::EMiddleSoftkeyPosition;
        softkey->iCommandId = KCBACommandMiddle;
        }

    softkey->iNode = aNode;
    
    CXnProperty* labelProp( aNode->LabelL() );
    CXnProperty* targetProp( aNode->GetPropertyL(KXnTarget) );

    if(labelProp)
       {
       softkey->iImageOn = EFalse;
       softkey->iLabel = labelProp->StringValueL();
       }
    else if(targetProp)
        {
        softkey->iImageOn = EFalse;
        softkey->iLabel = FindSoftkeyLabelL(aNode);        
        }
#if 0 // MSK icon change
    else
       {
       InitSoftkeyItemImageL( *softkey );
       }
#else // MSK icon change
    else if(softkey->iPosition != CEikButtonGroupContainer::EMiddleSoftkeyPosition)
       { // we can't set own image to middle softkey
       InitSoftkeyItemImageL( *softkey );
       }
#endif // MSK icon change
    softkey->iUpdateAppearance = ETrue;
    iSoftkeyItems.AppendL(softkey);
    CleanupStack::Pop(softkey);
    return softkey;
    }
// ---------------------------------------------------------------------------
// CXnMenuAdapter::SoftkeyPlaceL
// ---------------------------------------------------------------------------
//
const TDesC8& CXnMenuAdapter::SoftkeyPlaceL(CXnNodePluginIf& aNode)
    {
    CXnProperty* property = 
        aNode.GetPropertyL( XnPropertyNames::softkey::KTypeAttribute );                           
                      
    if( property)
        {
        return property->StringValue();
        }
    return KNullDesC8;                  
    
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::FindSoftkeyItemByPosition
// 
// -----------------------------------------------------------------------------
CXnNodePluginIf* CXnMenuAdapter::FindSoftkeyItemByPosition( const TPoint& aPosition )
    {
    CXnNodePluginIf* softkey(0);            
    TRAP_IGNORE( softkey = DoFindSoftkeyItemByPositionL( aPosition ) );    
    return softkey;
    }
    
// -----------------------------------------------------------------------------
// CXnMenuAdapter::DoFindSoftkeyItemByPositionL
// 
// -----------------------------------------------------------------------------
CXnNodePluginIf* CXnMenuAdapter::DoFindSoftkeyItemByPositionL( const TPoint& aPosition )
    {            
    TRect lsk( ButtonTouchableRectByPosition(CEikButtonGroupContainer::ELeftSoftkeyPosition) );
    TRect rsk( ButtonTouchableRectByPosition(CEikButtonGroupContainer::ERightSoftkeyPosition) );
    TRect msk( ButtonTouchableRectByPosition(CEikButtonGroupContainer::EMiddleSoftkeyPosition) );
    
    CXnNodePluginIf* softkey(NULL);
    CXnSoftkeyItem* item(NULL);
    if ( lsk.Contains(aPosition) )
        {
        item = SoftkeyItemL(CEikButtonGroupContainer::ELeftSoftkeyPosition);
        }
    else if(msk.Contains(aPosition))
        {
        item = SoftkeyItemL(CEikButtonGroupContainer::EMiddleSoftkeyPosition);
        }
    else if( rsk.Contains(aPosition) )
        {
        item = SoftkeyItemL(CEikButtonGroupContainer::ERightSoftkeyPosition);
        }
    if(item)
        {
        softkey = item->iNode;
        }        
    return softkey;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CXnSoftkeyItem* CXnMenuAdapter::SoftkeyItemL(TInt aPosition)
    {
    CXnSoftkeyItem* softkeyItem(0);
    if(iVisibleLSK && aPosition == CEikButtonGroupContainer::ELeftSoftkeyPosition)
        {
        return iVisibleLSK;
        }
    else if(iVisibleRSK && aPosition == CEikButtonGroupContainer::ERightSoftkeyPosition)
        {
        return iVisibleRSK;
        }
    else if(iVisibleMSK && aPosition == CEikButtonGroupContainer::EMiddleSoftkeyPosition)
        {
        return iVisibleMSK;
        }

    // return default
    for(TInt i=0;i<iSoftkeyItems.Count();++i)
        {
        softkeyItem = iSoftkeyItems[i];
        if(softkeyItem->iPosition == aPosition &&
           softkeyItem->iDefault && 
           IsNodeVisibleL(*softkeyItem->iNode))
            {
            return softkeyItem; 
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CXnSoftkeyItem* CXnMenuAdapter::SoftkeyItemL(CXnNodePluginIf* aNode)
    {
    CXnSoftkeyItem* softkeyItem(0);
    for(TInt i=0;i<iSoftkeyItems.Count();++i)
       {
       softkeyItem = iSoftkeyItems[i];
       if(softkeyItem->iNode == aNode)
           {
           return softkeyItem; 
           }
       }
    return NULL;
    }

CXnSoftkeyItem* CXnMenuAdapter::SoftkeyItemL(CXnNodeAppIf* aNode)
    {
    CXnSoftkeyItem* softkeyItem(0);
    for(TInt i=0;i<iSoftkeyItems.Count();++i)
       {
       softkeyItem = iSoftkeyItems[i];
       if(&(softkeyItem->iNode->AppIfL()) == aNode)
           {
           return softkeyItem; 
           }
       }
    return NULL;
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CXnMenuAdapter::SetUpdateAppearanceL(CXnSoftkeyItem* aItem)
    {
    if ( aItem && IsNodeVisibleL( *aItem->iNode ) )
        {
        aItem->iNode->SetDirtyL();
                
        switch ( aItem->iPosition )
            {
            case CEikButtonGroupContainer::ELeftSoftkeyPosition:
                iUpdateLskAppearance = ETrue;
                iVisibleLSK = NULL;
                break;
            case CEikButtonGroupContainer::EMiddleSoftkeyPosition:
                iUpdateMskAppearance = ETrue;
                iVisibleMSK = NULL;
                break;
            case CEikButtonGroupContainer::ERightSoftkeyPosition:
                iUpdateRskAppearance = ETrue;
                iVisibleRSK = NULL;
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::FindWidgetMenuItemL
// 
// -----------------------------------------------------------------------------
CXnNodePluginIf* CXnMenuAdapter::FindWidgetMenuItemL( CXnNodePluginIf* aNode )
    {    
    CXnNodePluginIf* focusedNode( iUiEngine->FocusedNodeL() );

    CXnNodePluginIf* menuItem( NULL );
    
    if ( focusedNode && aNode )
        {
        // Get <plugin> element
        CXnNodePluginIf& pluginNode( iUiEngine->PluginNodeL( focusedNode ) );
        
        RPointerArray< CXnNodePluginIf > children( pluginNode.ChildrenL() );
        CleanupClosePushL( children );
        
        CXnNodePluginIf* widgetExtNode( NULL );
        
        if( children.Count() > 0 )
            {
            // children[0] must be <widget> element
            widgetExtNode = FindChildL( *children[0], KXnMenuExtension );
            }
                                                 
        if( !widgetExtNode )
            {
            widgetExtNode = focusedNode;
            }
        
        if ( widgetExtNode )
            {
            CXnProperty* prop( aNode->GetPropertyL( KXnSource ) );
            
            if ( prop )
                {
                const TDesC8& source( prop->StringValue() );
                
                // Try to find <menuitem>
                menuItem = FindChildL( *widgetExtNode, 
                                       KXnMenuItem, 
                                       source );
                
                if ( !menuItem )
                    {
                    // Try to find <dynmenuitem>
                    menuItem = FindChildL( *widgetExtNode, 
                                           KXnDynMenuItem, 
                                           source );
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( &children );
        }
    
    if( menuItem && IsNodeVisibleL( *menuItem ) )
        {
        return menuItem;    
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnMenuAdapter::FindSoftkeyLabelL
// 
// -----------------------------------------------------------------------------
HBufC* CXnMenuAdapter::FindSoftkeyLabelL( CXnNodePluginIf* aNode )
    {
    if( !aNode )
        {
        return NULL;
        }
    
    CXnProperty* targerProp( aNode->GetPropertyL( KXnTarget ) );
    
    if ( targerProp && iRootNode )
        {
        CXnNodePluginIf* widgetSoftkeyNode( 
                FindChildL( *iRootNode,               
                            KXnWidgetSoftkey, 
                            targerProp->StringValue() ) ); 
                          
        if ( widgetSoftkeyNode )
            {
            CXnProperty* label( widgetSoftkeyNode->LabelL() );
            
            if( label )
                {
                return label->StringValueL();
                }
            }
        }
    
    return NULL;;
    }

//end of file
