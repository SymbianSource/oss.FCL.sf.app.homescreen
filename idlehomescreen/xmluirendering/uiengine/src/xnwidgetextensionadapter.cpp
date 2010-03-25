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
*/


#include <e32base.h>
#include <e32const.h>
#include <coecntrl.h>
#include <coemain.h>

#include <AknUtils.h>

#include "xnwidgetextensionadapter.h"
#include "xncontroladapter.h"

#include "xncomponentnodeimpl.h"
#include "xncomponent.h"

#include "xnuiengine.h"
#include "xnhittest.h"

#include "xnnode.h"
#include "xnnodepluginif.h"
#include "xnviewnodeimpl.h"
#include "xnnodepluginif.h"
#include "xndomdocument.h"
#include "xnproperty.h"
#include "xnodt.h"  
#include "xntype.h"
#include "xndomnode.h"
#include "xndomstringpool.h"

#include "xnappuiadapter.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xnplugindata.h"


_LIT8( KPopUpText, "popup" );
_LIT8( KPositionHint, "_s60-position-hint" );
_LIT8( KWidgetNodeName, "widget" );
_LIT8( KParentIdName, "parentid" );
_LIT8( KDisplay, "display" );
_LIT8( KNone, "none" );
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CXnWidgetExtensionAdapter* CXnWidgetExtensionAdapter::NewL( 
    CXnNodePluginIf& aNode )
    {
    CXnWidgetExtensionAdapter* self = 
        new ( ELeave ) CXnWidgetExtensionAdapter( aNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::~CXnWidgetExtensionAdapter
// Destructor.
// -----------------------------------------------------------------------------
//
CXnWidgetExtensionAdapter::~CXnWidgetExtensionAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::CXnWidgetExtensionAdapter
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnWidgetExtensionAdapter::CXnWidgetExtensionAdapter( CXnNodePluginIf& aNode )
    : iNode( aNode ), iPositionHint( ENone )
    {
    }

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::ConstructL
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//    
void CXnWidgetExtensionAdapter::ConstructL()
    {
    CreateWindowL();
    Window().SetRequiredDisplayMode( EColor16MA );

    // this adapter handles both widgetextension and popup nodes
    // we have to decide which one of them is the recent one    
 
    CXnType* typeInfo = iNode.Node().Type();
    const TDesC8& type = typeInfo->Type();

    if ( ( type != KPopUpText ) &&
         ( Window().SetTransparencyAlphaChannel() == KErrNone ) )
        {
        Window().SetBackgroundColor( ~0 );     
        }
    iUiEngine = iNode.Node().UiEngine();
    CXnControlAdapter::ConstructL( iNode );
    EnableDragEvents();   

    iAppUiAdapter = static_cast< CXnAppUiAdapter* >( iAvkonAppUi );

    }

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::MakeVisible
// 
// -----------------------------------------------------------------------------
// 
void CXnWidgetExtensionAdapter::MakeVisible( TBool aVisible )
    {
   
    TBool visible( IsVisible() ? ETrue : EFalse );
    if ( visible == aVisible )
        {
        return;
        }

    SetPointerCapture( aVisible );
    
    CXnPluginData& plugin( 
            iAppUiAdapter->ViewManager().ActiveViewData().Plugin( &iNode.Node() ) );

    plugin.SetIsDisplayingPopup( aVisible, &iNode.Node() );
    
    CXnType* typeInfo = iNode.Node().Type();
    const TDesC8& type = typeInfo->Type();

    if ( type != KPopUpText )
        {
        DrawableWindow()->FadeBehind( aVisible );
        }
    
    if ( aVisible && type == KPopUpText )
        {        
        // read position-hint property and set-up its variable
        CXnProperty* positionHintProp = NULL;
        TRAP_IGNORE( positionHintProp = iNode.Node().GetPropertyL( KPositionHint ) );
                   
        if ( positionHintProp )
            {
            const TDesC8& displayHintVal = positionHintProp->StringValue();
                    
            if ( displayHintVal == XnPropertyNames::tooltip::positionhint::KAboveLeft )
                {
                iPositionHint = EAboveLeft;
                }
            else if ( displayHintVal == XnPropertyNames::tooltip::positionhint::KAboveRight )
                {
                iPositionHint = EAboveRight;
                }
            else if ( displayHintVal == XnPropertyNames::tooltip::positionhint::KBelowLeft )
                {
                iPositionHint = EBelowLeft;
                }
            else if ( displayHintVal == XnPropertyNames::tooltip::positionhint::KBelowRight )
                {
                iPositionHint = EBelowRight;
                }
            else if ( displayHintVal == XnPropertyNames::tooltip::positionhint::KRight )
                {
                iPositionHint = ERight;
                }
            else if ( displayHintVal == XnPropertyNames::tooltip::positionhint::KLeft )
                {
                iPositionHint = ELeft;
                }
            else 
                {
                 // if the value if of unknown type, use default one
                 if ( AknLayoutUtils::LayoutMirrored() )
                     {
                     iPositionHint = EAboveRight;
                     }
                     else
                     {
                     iPositionHint = EAboveLeft; 
                     }
                 }
        
            if ( iPositionHint != ENone )
                {    
                // the popup is going visible and position-hind is available
                // calculate its position
                CalculatePosition();
                }
            }        
        }

    CCoeControl::MakeVisible( aVisible );            
    }

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::HandlePointerEventL
// 
// -----------------------------------------------------------------------------
//    
void CXnWidgetExtensionAdapter::HandlePointerEventL( 
    const TPointerEvent& aPointerEvent )
    {    
    
    CXnType* typeInfo = iNode.Node().Type();
    const TDesC8& type = typeInfo->Type();
    
    // in case of popup, we have to make sure that 
    // it will be closed after tapping outside of the
    // area of itself and its parent
    if ( type == KPopUpText )
        {
        
        // check if the tap was inside of popup
        TRect popupRect = this->Rect();
        popupRect.Move(this->Position() );
        TBool isInPopupWindow = popupRect.Contains(
                aPointerEvent.iParentPosition );

        if ( !isInPopupWindow )
            {
            
            // if tap was outside of window, check if tap was 
            // inside of the parrent
            CXnProperty* parentIdProp = iNode.Node().GetPropertyL(
                    KParentIdName );
            
            if ( parentIdProp )
                {                
                const TDesC8& parentIdVal = parentIdProp->StringValue();
                CXnNode* parentN(iUiEngine->FindNodeByIdL( parentIdVal,
                        iNode.Node().Namespace() ) );
                
                if ( parentN )
                    {
                    TRect clientRect =
                            static_cast<CEikAppUi&> ( *iAppUiAdapter ).ClientRect();
                    TRect parentRect = parentN->Rect();
                    parentRect.Move( clientRect.iTl );
                    
                    if ( !parentRect.Contains( aPointerEvent.iParentPosition ) )
                        {
                        // tap was neither in popup nor in its parent -
                        // we can close it
                        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                            {
                            CXnDomStringPool* sp =
                                iNode.Node().DomNode()->StringPool();
                            CXnProperty* prop = CXnProperty::NewL( KDisplay, KNone,
                            CXnDomPropertyValue::EString, *sp );
                            CleanupStack::PushL( prop );
                            iNode.Node().SetPropertyL( prop );
                            CleanupStack::Pop( prop );
                            return;
                            }
                        }
                    else
                        {
                        // tap was made inside of popup parent
                        // we pass the event to it after
                        // recalculating the taping point
                        TPointerEvent newPointerEvent;
						// Copy is not yet defined in S60v5, do it manually
                        //newPointerEvent.Copy( aPointerEvent );
						newPointerEvent.iType=aPointerEvent.iType;
						// Clear the advanced pointer flag EModifierAdvancedPointerEvent = 0x10000000
						newPointerEvent.iModifiers=(aPointerEvent.iModifiers&~0x10000000);
						// not needed, will be overwritten below
						// newPointerEvent.iPosition=aPointerEvent.iPosition;
						newPointerEvent.iParentPosition=aPointerEvent.iParentPosition;
						// end copy
						
                        newPointerEvent.iPosition = TPoint(
                                 aPointerEvent.iParentPosition - clientRect.iTl );
                        parentN->Control()->HandlePointerEventL( newPointerEvent );
                        return;
                        }
                    }
                }
            }
        }
    
    CXnControlAdapter::HandlePointerEventL( aPointerEvent );
    
    }

// -----------------------------------------------------------------------------
// CXnWidgetExtensionAdapter::Draw
// 
// -----------------------------------------------------------------------------
//    
void CXnWidgetExtensionAdapter::Draw( const TRect& aRect ) const
    {
        CXnControlAdapter::Draw( aRect );
    }

// -----------------------------------------------------------------------------
// CalculatePosition
// This is used only for popup element with position hint
// -----------------------------------------------------------------------------
void CXnWidgetExtensionAdapter::CalculatePosition() 
    {

    // widget's rectangle
    TRect controlRect;

    // get popup's window size
    TRect popupRect = this->Rect();

    TRect clientRect = static_cast<CEikAppUi&>( *iAppUiAdapter ).ClientRect();

    // get entire screen except control pane
    TRect contentRect( 0, 0, clientRect.iBr.iX, clientRect.iBr.iY ); 

    // resulting rectangle
    TRect rect;

    TPoint offset( clientRect.iTl );

    // parent widget's rectangle ( first predecesscor which is "widget" )
    CXnNode* parent = iNode.Node().Parent();

    while ( parent )
        {
        const TDesC8& type( parent->DomNode()->Name() );
        if ( type == KWidgetNodeName )
            {
            break;
            }
        parent = parent->Parent();
        }
    
    // if predecesscor widget was not found, use parent's rectangle
    if ( parent == NULL )
        { 
        controlRect = iNode.Node().Parent()->Rect();
        }
    else
        {
        controlRect = parent->BorderRect();
        }
    
    // calculate available space for placing the popup
    TInt spaceAbove = controlRect.iTl.iY + offset.iY;
    TInt spaceBelow = contentRect.iBr.iY - controlRect.iBr.iY - offset.iY;
    TInt spaceLeft = controlRect.iTl.iX + offset.iX;
    TInt spaceRight = contentRect.iBr.iX - controlRect.iBr.iX - offset.iX;

    switch ( iPositionHint )
        {
        
        case EAboveLeft:
            
            // if this position does not fit the screen,
            // and if below left is more suitable, use it
            if ( spaceAbove < popupRect.Height() && spaceBelow > spaceAbove )
                {
                rect = TRect( TPoint( controlRect.iTl.iX, controlRect.iBr.iY ), 
                              TPoint( controlRect.iTl.iX + popupRect.Width(), controlRect.iBr.iY + popupRect.Height() ) );
                }
            else
                {
                // use the above-left position
                rect = TRect( TPoint( controlRect.iTl.iX, controlRect.iTl.iY - popupRect.Height() ), 
                              TPoint( controlRect.iTl.iX + popupRect.Width(), controlRect.iTl.iY ) );
                
                }
            break;
            
        case EAboveRight:
            
            // if this position does not fit the screen,
            // and if below right is more suitable, use it
            if ( spaceAbove < popupRect.Height() && spaceBelow > spaceAbove )
                {
                rect = TRect( TPoint( controlRect.iBr.iX - popupRect.Width(), controlRect.iBr.iY ), 
                              TPoint( controlRect.iBr.iX, controlRect.iBr.iY + popupRect.Height() ) );
                }
            else
                {
                // use the above-right position
                rect = TRect( TPoint( controlRect.iBr.iX - popupRect.Width(), controlRect.iTl.iY - popupRect.Height() ), 
                              TPoint( controlRect.iBr.iX,  controlRect.iTl.iY ) );
                }
            break;
            
        case EBelowLeft:
            
            // if this position does not fit the screen,
            // and if above left is more suitable, use it
            if ( spaceBelow < popupRect.Height() && spaceBelow < spaceAbove )
                {
                rect = TRect( TPoint( controlRect.iTl.iX, controlRect.iTl.iY - popupRect.Height() ), 
                              TPoint( controlRect.iTl.iX + popupRect.Width(), controlRect.iTl.iY ) );
                }
            else
                {
                // use the below-left position
                rect = TRect( TPoint( controlRect.iTl.iX, controlRect.iBr.iY ), 
                              TPoint( controlRect.iTl.iX + popupRect.Width(), controlRect.iBr.iY + popupRect.Height() ) );
                }
            break;

        case EBelowRight:

              // if this position does not fit the screen,
              // and if above right is more suitable, use it
              if ( spaceBelow < popupRect.Height() && spaceBelow < spaceAbove )
                  {
                  rect = TRect( TPoint( controlRect.iBr.iX - popupRect.Width(), controlRect.iTl.iY - popupRect.Height() ), 
                                TPoint( controlRect.iBr.iX,  controlRect.iTl.iY ) );
                  }
              else
                  {
                  // use the below-right position
                  rect = TRect( TPoint( controlRect.iBr.iX - popupRect.Width(), controlRect.iBr.iY ), 
                                TPoint( controlRect.iBr.iX, controlRect.iBr.iY + popupRect.Height() ) );
                  }
            break;

        case ERight:

            // if this position does not fit the screen,
            // and if left or above-left is more suitable, use it
            if ( spaceRight < popupRect.Width() )
                {
                // use the left position if the space is big enough
                if ( spaceLeft >= popupRect.Width() )
                    {
                    // use left position
                    rect = TRect( TPoint( controlRect.iTl.iX - popupRect.Width(), controlRect.iTl.iY ), 
                                  TPoint( controlRect.iTl.iX, controlRect.iTl.iY + popupRect.Height() ) );
                    }
                else if ( spaceAbove >= popupRect.Height() )
                    {
                    // use the above-right position
                    rect = TRect( TPoint( controlRect.iBr.iX - popupRect.Width(), controlRect.iTl.iY - popupRect.Height() ), 
                                  TPoint( controlRect.iBr.iX,  controlRect.iTl.iY ) );  
                    }
                else
                    {
                    // use the below-right position
                    rect = TRect( TPoint( controlRect.iBr.iX - popupRect.Width(), controlRect.iBr.iY ), 
                                  TPoint( controlRect.iBr.iX, controlRect.iBr.iY + popupRect.Height() ) );
                    }
                }
            else
                {
                // use the right position
                rect = TRect( TPoint( controlRect.iBr.iX, controlRect.iTl.iY ), 
                              TPoint( controlRect.iBr.iX + popupRect.Width(), controlRect.iTl.iY + popupRect.Height() ) );
                }
                
            break;
            
        case ELeft:
            
            // if this position does not fit the screen,
            // and if right is more suitable, use it
            if ( spaceLeft < popupRect.Width() )
                {
                // use the right position, if it the space is big enough
                if ( spaceRight >= popupRect.Width() )
                    {    
                    rect = TRect( TPoint( controlRect.iBr.iX, controlRect.iTl.iY ), 
                                  TPoint( controlRect.iBr.iX + popupRect.Width(), controlRect.iTl.iY + popupRect.Height() ) );
                    }
                else if ( spaceAbove >= popupRect.Height() )
                    {
                    // use the above-left position
                    rect = TRect( TPoint( controlRect.iTl.iX, controlRect.iTl.iY - popupRect.Height() ), 
                                  TPoint( controlRect.iTl.iX + popupRect.Width(), controlRect.iTl.iY ) );
                    }
                else
                    {
                    // use the below-left position
                    rect = TRect( TPoint( controlRect.iTl.iX, controlRect.iBr.iY ), 
                                  TPoint( controlRect.iTl.iX + popupRect.Width(), controlRect.iBr.iY + popupRect.Height() ) );
                    }
                }
            else
                {
                // use the left position  
                rect = TRect( TPoint( controlRect.iTl.iX - popupRect.Width(), controlRect.iTl.iY ), 
                              TPoint( controlRect.iTl.iX, controlRect.iTl.iY + popupRect.Height() ) );
                }
            break;
        default:
            break;
        }

    rect.Move( offset );

    // if the popup rectangle exceeds the borders of content rectangle, move it.
    if ( rect.iTl.iY < contentRect.iTl.iY )
        {
        rect.Move( 0, contentRect.iTl.iY - rect.iTl.iY );
        }
    if ( rect.iTl.iX < contentRect.iTl.iX )
        {
        rect.Move( contentRect.iTl.iX - rect.iTl.iX, 0 );
        }
    if ( rect.iBr.iY > contentRect.iBr.iY )
        {
        rect.Move( 0, contentRect.iBr.iY - rect.iBr.iY );
        }
    if ( rect.iBr.iX > contentRect.iBr.iX )
        {
        rect.Move( contentRect.iBr.iX - rect.iBr.iX, 0 );
        }
    this->SetRect( rect );
    }

//  End of File  
