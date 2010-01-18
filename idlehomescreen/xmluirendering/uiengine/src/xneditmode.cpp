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
* Description:  Edit mode for widget drag and drop.            
*
*/

// System includes
#include <AknUtils.h>
#include <gulgcmap.h>
#include <akntitle.h>
#include <barsread.h>
#include <xnuiengine.rsg>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK

// User includes
#include "xnappuiadapter.h"
#include "xnviewmanager.h"
#include "xnviewadapter.h"
#include "xnviewdata.h"
#include "xnnodeimpl.h"
#include "xndomnode.h"
#include "xnnode.h"
#include "xnproperty.h"
#include "xnuiengine.h"
#include "xncontroladapter.h"
#include "xnpopupcontroladapter.h"
#include "xnfocuscontrol.h"
#include "xneditor.h"
#include "xntype.h"
#include "xnmenu.h"

#include "xneditmode.h"

// Constants
_LIT8( KMenu, "menu" );

// ============================ LOCAL FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CopyBitmap
// Copies a data from source bitmap to target bitmap.
// -----------------------------------------------------------------------------
//
static TInt CopyBitmap( CFbsBitmap& aTarget, const CFbsBitmap& aSource,        
    TPoint aSourcePoint )
    {
    TSize targetSize( aTarget.SizeInPixels() );
    TSize sourceSize( aSource.SizeInPixels() );
    
    TInt lineLength( targetSize.iWidth );
    TInt maxSourceLineLength( sourceSize.iWidth - aSourcePoint.iX );
    
    if ( lineLength > maxSourceLineLength )
        {
        lineLength = maxSourceLineLength;
        }
    
    TInt rowCount( targetSize.iHeight );
    TInt maxSourceRowCount( sourceSize.iHeight - aSourcePoint.iY );
    
    if ( rowCount > maxSourceRowCount )
        {
        rowCount = maxSourceRowCount;
        }

    // Get bitmap display mode
    TDisplayMode displayMode( aSource.DisplayMode() );

    // Create buffer for a scan line
    HBufC8* scanLine = HBufC8::New( aSource.ScanLineLength( 
        lineLength, displayMode ) );
           
    if ( !scanLine )
        {
        return KErrNoMemory;
        }
     
    TPtr8 scanPtr( scanLine->Des() );

    // Copy all rows to destination bitmap
    for ( TInt row = 0; row < rowCount; row++ )
        {
        aSource.GetScanLine( scanPtr,
            TPoint( aSourcePoint.iX, aSourcePoint.iY + row ),
            lineLength, displayMode );
        
        aTarget.SetScanLine( scanPtr, row );
        }

    delete scanLine;
    
    return KErrNone;
    }        

// -----------------------------------------------------------------------------
// SetVisibilityL
// Sets node either visible or hidden
// -----------------------------------------------------------------------------
//
static void SetVisibilityL( CXnNode& aNode, const TDesC8& aVisibility )
    {
    CXnUiEngine* engine( aNode.UiEngine() );
    
    CXnDomStringPool* sp( aNode.DomNode()->StringPool() );
    
    CXnProperty* prop = CXnProperty::NewL(
            XnPropertyNames::style::common::KVisibility,
            aVisibility,
            CXnDomPropertyValue::EString, *sp );
    CleanupStack::PushL( prop );
    
    aNode.SetPropertyL( prop );
    
    CleanupStack::Pop( prop );
    }

// -----------------------------------------------------------------------------
// ResolveMenuIdL
// Resolves <menu> element and its id based on aMenuBar <menubar> element 
// -----------------------------------------------------------------------------
//
static HBufC* ResolveMenuIdL( CXnNode& aMenuBar )
    {       
    HBufC* retval( NULL );
    
    RPointerArray< CXnNode >& children( aMenuBar.Children() );
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNode* child( children[i] );
        
        if ( child->Type()->Type() == KMenu )
            {
            CXnProperty* prop( 
                child->GetPropertyL( XnPropertyNames::common::KId ) );
            
            if ( prop )
                {
                // Ownership is passed to the caller
                retval = prop->StringValueL();                
                }
            
            break;
            }
        }
    
    return retval;
    }

#ifdef RD_TACTILE_FEEDBACK
// -----------------------------------------------------------------------------
// Feedback
// Gives instant touch feedback
// -----------------------------------------------------------------------------
//
static void Feedback( TTouchLogicalFeedback aType )
    {
    MTouchFeedback* fb( MTouchFeedback::Instance() );

    if ( fb )
        {
        fb->InstantFeedback( aType );
        }                        
    }
#endif // RD_TACTILE_FEEDBACK

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnEditMode::CXnEditMode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnEditMode::CXnEditMode( CXnUiEngine& aUiEngine )
    : iUiEngine( aUiEngine ), 
      iViewManager( *iUiEngine.ViewManager() ),
      iState( CXnEditMode::ENone ), 
      iStylusDownPos( TPoint::EUninitialized ),
      iPreviousPos( TPoint::EUninitialized ),
      iDrawPos( TPoint::EUninitialized ), 
      iLastDrawRect( TRect::EUninitialized )     
    {
    }

// -----------------------------------------------------------------------------
// CXnEditMode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnEditMode::ConstructL()
    {             
    CreateWindowL();
    
    ActivateL();            
            
    Window().SetRequiredDisplayMode( EColor16MA );
    
    if ( Window().SetTransparencyAlphaChannel() == KErrNone )
        {    
        Window().SetBackgroundColor( ~0 );
        }
                  
    EnableDragEvents();

    CCoeControl::MakeVisible( EFalse );
    
    TRect rect;
    
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    
    iMainpane = new ( ELeave ) CFbsBitmap;
    User::LeaveIfError( iMainpane->Create( TSize(), EColor16MA ) );
    
    iWidget = new ( ELeave ) CFbsBitmap;
    User::LeaveIfError( iWidget->Create( TSize(), EColor16MA ) );
                  
    iBmpDevice = CFbsBitmapDevice::NewL( iMainpane );
              
    iBmpGc = CFbsBitGc::NewL();
        
    iBmpGc->Activate( iBmpDevice );
            
    iMapGc = CWindowToBitmapMappingGc::NewL(  
        *static_cast< CWsScreenDevice* >( SystemGc().Device() ), *iBmpGc );               
    
    SetRect( rect );    
    }

// -----------------------------------------------------------------------------
// CXnEditMode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnEditMode* CXnEditMode::NewL( CXnUiEngine& aEngine )
    {
    CXnEditMode* self = new ( ELeave ) CXnEditMode( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnEditMode::~CXnEditMode
// Destructor
// -----------------------------------------------------------------------------
//
CXnEditMode::~CXnEditMode()
    {    
    TRAP_IGNORE( StopDragL() );
    
    delete iMapGc;
    delete iBmpGc;
    delete iBmpDevice;
    delete iMainpane;
    delete iWidget;       
    }

// -----------------------------------------------------------------------------
// CXnEditMode::UpdateScreen
// Updates invalid parts of the screen. All drawing must be done from here
// -----------------------------------------------------------------------------
//
void CXnEditMode::UpdateScreen()
    {
    if ( iState == CXnEditMode::EDragging )
        {
        if ( !iWidget || iDrawPos == TPoint::EUninitialized )
            {
            return;
            }

        TRect drawRect( iDrawPos, iWidget->SizeInPixels() );
               
        if ( iLastDrawRect != TRect::EUninitialized )
            {
            drawRect.BoundingRect( iLastDrawRect );
            }

        iLastDrawRect = drawRect;
        
        DrawNow( drawRect );          
        }
    else 
        {
        if ( iLastDrawRect != TRect::EUninitialized )
            {
            DrawNow( iLastDrawRect );
            }
        
        iDrawPos.SetXY( 0, 0 );
        iLastDrawRect = TRect::EUninitialized;        
        }
    }

// -----------------------------------------------------------------------------
// CXnEditMode::Draw
// 
// -----------------------------------------------------------------------------
//
void CXnEditMode::Draw( const TRect& aRect ) const
    {      
    CWindowGc& gc( SystemGc() );

    gc.SetClippingRect( Rect() );

    gc.Clear( aRect );
    
    if ( iWidget && iState == CXnEditMode::EDragging )
        {              
        gc.BitBlt( iDrawPos, iWidget );
        }
    
    gc.Reset();
    }

// -----------------------------------------------------------------------------
// CXnEditMode::MakeVisible
// 
// -----------------------------------------------------------------------------
//
void CXnEditMode::MakeVisible( TBool aVisible )
    {        
    TBool visible( IsVisible() ? ETrue : EFalse );
    
    if ( aVisible == visible )
        {
        return;
        }
        
    CCoeControl::MakeVisible( aVisible );

    CXnAppUiAdapter& appui( iUiEngine.AppUiAdapter() );
    
    // Remove focus
    appui.HideFocus();
    
    CCoeControl& bg( appui.ViewAdapter().BgControl() );
    
    if ( aVisible )
        {
        bg.DrawableWindow()->SetPointerGrab( EFalse );
        
        Window().SetOrdinalPosition( 0 );
        Window().SetPointerGrab( ETrue );
        Window().ClaimPointerGrab();               
        }
    else
        {
        Window().SetPointerGrab( EFalse );
        
        bg.DrawableWindow()->SetPointerGrab( ETrue );               
        }
    }

// -----------------------------------------------------------------------------
// CXnEditMode::HandlePointerEventL
//
// -----------------------------------------------------------------------------
//
void CXnEditMode::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( !AknLayoutUtils::PenEnabled() )         
        {
        return;
        }
    
    if ( aPointerEvent.iType != TPointerEvent::EButton1Down &&
        iStylusDownPos == TPoint::EUninitialized )
        {
        return;
        }
    
    // For performance reasons, discard the event if delta between previous
    // event is within 'KOffset'.
    TInt KOffset( 5 );
    TRect rect(
        aPointerEvent.iPosition.iX - KOffset,
        aPointerEvent.iPosition.iY - KOffset,
        aPointerEvent.iPosition.iX + KOffset,
        aPointerEvent.iPosition.iY + KOffset );
        
    if ( aPointerEvent.iType == TPointerEvent::EDrag && 
        rect.Contains( iPreviousPos ) )
        {   
        return;                 
        }
    
    RPointerArray< CXnPluginData >& plugins( 
        iViewManager.ActiveViewData().PluginData() );
            
    CXnNode* node( NULL );
    
    for ( TInt i = 0; i < plugins.Count(); i++ )
        {   
        CXnPluginData* data( plugins[i] );
        
        CXnNode* plugin( data->Owner()->LayoutNode() );
        
        CXnControlAdapter* ctrl( plugin->Control() );

        if ( ctrl->IsVisible() && 
            ctrl->Rect().Contains( aPointerEvent.iPosition ) )        
            {                      
            node = plugin;
            break;
            }
        }
    
    if ( !node )
        {
        if ( iTargetNode )
            {
            iTargetNode->UnsetStateL( XnPropertyNames::style::common::KFocus );
            }
        
        iTargetNode = NULL;
        }
               
    if ( node && node != iTargetNode )
        {
        if ( !node->IsStateSet( XnPropertyNames::style::common::KEdit ) )
            {
            // Not in edit state
            node = NULL;
            }
        else
            {
            iTargetNode = node;

            iUiEngine.DisableRenderUiLC();
            
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                iUiEngine.AppUiAdapter().ShowFocus();
                
                node->SetStateL( XnPropertyNames::style::common::KFocus );
#ifdef RD_TACTILE_FEEDBACK                
                Feedback( ETouchFeedbackBasic );
#endif
                }
            else if ( aPointerEvent.iType == TPointerEvent::EDrag )                     
               {
               if ( iState == CXnEditMode::EDragging )
                   {
                   node->SetStateL( XnPropertyNames::style::common::KFocus );               
#ifdef RD_TACTILE_FEEDBACK               
                   Feedback( ETouchFeedbackSensitive );
#endif                                   
                   }
               else if ( iDraggingNode && 
                   !iDraggingNode->MarginRect().Contains( aPointerEvent.iPosition ) )
                   {
                   iDraggingNode->UnsetStateL( 
                       XnPropertyNames::style::common::KFocus );
                   
                   iDraggingNode = NULL;
                   }                             
               }     
            
            CleanupStack::PopAndDestroy();
            }
        }
        
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {        
        iDragged = EFalse;
               
        if ( node )
            {
            CXnPluginData& plugin( iUiEngine.ViewManager()->
                    ActiveViewData().Plugin( node ) );
            
            if ( plugin.Occupied() )
                {
                StartDragL( *node );
                                  
                iDrawPos = iDraggingNode->BorderRect().iTl;
                
                iStylusDownPos = iPreviousPos = aPointerEvent.iPosition;
                }  
            else
                {
                iDraggingNode = node;
                
                iStylusDownPos = iPreviousPos = aPointerEvent.iPosition;
                }
            }
        }
    else if ( aPointerEvent.iType == TPointerEvent::EDrag )
        {
        const TInt KDragTreshold( 30 );
        
        TInt dx( Abs( aPointerEvent.iPosition.iX - iStylusDownPos.iX ) );
        TInt dy( Abs( aPointerEvent.iPosition.iY - iStylusDownPos.iY ) );
        
        TBool dragged( iDragged );
        
        if ( dx > KDragTreshold || dy > KDragTreshold )
            {
            iDragged = ETrue;
            
            if ( !dragged && iDragged && iDraggingNode && 
                iState == CXnEditMode::EDragging )
                {
                // Hide the node which will be dragged from layout tree 
                SetVisibilityL( *iDraggingNode,
                    XnPropertyNames::style::common::visibility::KHidden );
                
                iUiEngine.RenderUIL();                
                }
            }
        
        if ( iDragged && iWidget && iDraggingNode && iState == CXnEditMode::EDragging )
            {                        
            // Resolve draw position
            TPoint dp( iPreviousPos - aPointerEvent.iPosition );
                                
            iDrawPos -= dp;
                    
            // Update previous position
            iPreviousPos = aPointerEvent.iPosition;
            
            UpdateScreen();
            }
        }
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
#ifdef RD_TACTILE_FEEDBACK                
        Feedback( ETouchFeedbackBasic );
#endif
        // Cancel
        if ( !iTargetNode || !iDraggingNode ||                 
             iDraggingNode == iTargetNode ||
             !iTargetNode->MarginRect().Contains( aPointerEvent.iPosition ) )
            {                                
            CXnNode* node( iDraggingNode );
            
            iUiEngine.DisableRenderUiLC();
            
            StopDragL();
            
            iUiEngine.RenderUIL();
            
            CleanupStack::PopAndDestroy(); // DisableRenderUiLC
            
            UpdateScreen();
                        
            if ( node && !iDragged )
                {
                CXnPluginData& plugin( iUiEngine.ViewManager()->
                        ActiveViewData().Plugin( node ) );

                if ( plugin.Occupied() )
                    {
                    CXnNode* popup( iUiEngine.StylusPopupNode() );
                    
                    if ( popup )
                        {
                        CXnPopupControlAdapter* control =
                            static_cast< CXnPopupControlAdapter* >(
                                    popup->Control() );
                       
                        if ( control )
                            {
                            control->TryDisplayingStylusPopupL( *node );
                            }
                        }
                    }
                else
                    {
                    // empty slot, start add widget query
                    iUiEngine.Editor()->SetTargetPlugin( node );
                    iUiEngine.Editor()->AddWidgetL();
                    }
                }
            }            
        else // Reorder
            {
            if ( iDraggingNode && iTargetNode )
                {
                iUiEngine.DisableRenderUiLC();
                
                CXnNode* parent1( iDraggingNode->Parent() );
                CXnNode* parent2( iTargetNode->Parent() );
                
                if ( parent1 == parent2 )
                    {
                    parent1->ReorderNodesL( iDraggingNode, iTargetNode );
                    
                    // Update plugin positions to HSPS
                    iUiEngine.Editor()->ReorderWidgetsL( 
                            iUiEngine.Plugins() );                    
                    }
                                               
                StopDragL();
                
                iUiEngine.RenderUIL();
                
                CleanupStack::PopAndDestroy(); // DisableRenderUiLC
                
                UpdateScreen();                    
                }
            }
        
        iUiEngine.AppUiAdapter().HideFocus();
        }    
    }

// -----------------------------------------------------------------------------
// CXnEditMode::OfferKeyEventL
//
// -----------------------------------------------------------------------------
//
TKeyResponse CXnEditMode::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {    
    if ( IsVisible() && aType == EEventKey )
        {
        CXnNode* focused( iUiEngine.FocusedNode() );
        
        if ( focused && ( aKeyEvent.iScanCode == EStdKeyDevice3 ||
            aKeyEvent.iScanCode == EStdKeyEnter ) )
            {
            CXnPluginData& plugin( iUiEngine.ViewManager()->
                    ActiveViewData().Plugin( focused ) );
            
            if ( plugin.Occupied() )
                {
                // Open context menu
                CXnNode* menubar( iUiEngine.MenuBarNode() );
                
                if ( menubar )
                    {
                    HBufC* id( ResolveMenuIdL( *menubar ) );
                    CleanupStack::PushL( id );
                    
                    if ( id )
                        {
                        XnMenuInterface::MXnMenuInterface* menuIf( NULL );
                        
                        XnComponentInterface::MakeInterfaceL( 
                            menuIf, menubar->AppIfL() );
                        
                        if ( menuIf )
                            {
                            menuIf->TryDisplayingMenuBarL( *id );
                            iUiEngine.Editor()->SetTargetPlugin( focused );
                            }                        
                        }
                    
                    CleanupStack::PopAndDestroy( id );
                    }                                               
                }
            else
                {
                // empty slot, start add widget query
                iUiEngine.Editor()->SetTargetPlugin( focused );
                iUiEngine.Editor()->AddWidgetL();                
                } 
            
            return EKeyWasConsumed;
            }
        }
    
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CXnEditMode::SizeChanged
//
// -----------------------------------------------------------------------------
//
void CXnEditMode::SizeChanged()
    {
    TSize size( Rect().Size() );
    
    iBmpDevice->Resize( size );
    
    // Needs to be called if the device is resized
    iBmpGc->Resized();
    
    iMainpane->Resize( size );    
    }

// -----------------------------------------------------------------------------
// CXnEditMode::StartDragL()
//
// -----------------------------------------------------------------------------
//
void CXnEditMode::StartDragL( CXnNode& aNode )
    {  
    CXnControlAdapter* control( aNode.Control() );
    
    TRect rect( control->Rect() );
    
    // Clear first with alpha 
    TRgb rgb( TRgb::Color16MA( 0 ) );
    rgb.SetAlpha( 64 );
        
    iMapGc->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    
    iMapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iMapGc->SetBrushColor( rgb );
    
    iMapGc->Clear( rect );
                         
    iState = CXnEditMode::EShootContent;
        
    CWindowGc* gc( control->CustomGc() );
            
    control->SetCustomGc( iMapGc );
    
    control->DrawNow( rect );
        
    control->SetCustomGc( gc );
           
    if ( iWidget->SizeInPixels() != rect.Size() )
        {
        iWidget->Resize( rect.Size() );
        }
    
    User::LeaveIfError( CopyBitmap( *iWidget, *iMainpane, rect.iTl ) );

    iState = CXnEditMode::EDragging;
        
    iDraggingNode = &aNode;       
    }

// -----------------------------------------------------------------------------
// CXnEditMode::StopDragL()
// -----------------------------------------------------------------------------
//
void CXnEditMode::StopDragL()
    {
    if ( iDraggingNode && iState == CXnEditMode::EDragging )
        {
        SetVisibilityL( *iDraggingNode,
            XnPropertyNames::style::common::visibility::KVisible );
                        
        iDraggingNode->SetStateL( XnPropertyNames::style::common::KFocus );
        iDraggingNode->Parent()->SetDirtyL( XnDirtyLevel::ELayoutAndRender );
        }

    iDraggingNode = NULL;
    iTargetNode = NULL;

    iPreviousPos.SetXY( 0, 0 );
    iStylusDownPos.SetXY( 0, 0 );

    iState = CXnEditMode::EDragAndDrop;
    }

// -----------------------------------------------------------------------------
// CXnEditMode::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//
void CXnEditMode::HandleScreenDeviceChangedL()
    {
    if ( iState != CXnEditMode::ENone )
        {
        if ( iState == CXnEditMode::EDragging )
            {
            IgnoreEventsUntilNextPointerUp();
            }
        
        StopDragL();               
        }

    SetRect( iUiEngine.ClientRect() );
                       
    UpdateScreen();
    }

// -----------------------------------------------------------------------------
// CXnEditMode::SetEditModeL()
// -----------------------------------------------------------------------------
//
void CXnEditMode::SetEditModeL( CXnEditMode::TEditState aState )
    {    
    // Enter to edit mode
    if ( aState == CXnEditMode::EDragAndDrop )
        {               
        iState = aState;
        
        iUiEngine.AppUiAdapter().HandleEnterEditModeL( ETrue );
        SetStatusPaneTitleL();
        
        MakeVisible( ETrue );               
        }

    // Exit from edit mode
    else if ( aState == CXnEditMode::ENone )
        {                
        iDraggingNode = NULL;
        iTargetNode = NULL;
        
        iState = aState;
                       
        iUiEngine.AppUiAdapter().HandleEnterEditModeL( EFalse );
        
        MakeVisible( EFalse );
        }   
    }

// -----------------------------------------------------------------------------
// CXnEditMode::EditState()
//
// -----------------------------------------------------------------------------
//
CXnEditMode::TEditState CXnEditMode::EditState() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CXnEditMode::SetStatusPaneTitleL()
// -----------------------------------------------------------------------------
//
void CXnEditMode::SetStatusPaneTitleL()
    {
    TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
    CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
        iUiEngine.AppUiAdapter().StatusPane()->PaneCapabilities( titlePaneUid );
    if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
        {
        CAknTitlePane* title = static_cast< CAknTitlePane* >( 
            iUiEngine.AppUiAdapter().StatusPane()->ControlL( titlePaneUid ) );
        TResourceReader reader;
        CEikonEnv::Static()->CreateResourceReaderLC(
                reader, R_QTN_HS_TITLE_EDITMODE );
        title->SetFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader internal state
        }
    }

// End of file
