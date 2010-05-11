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
* Description:  Focus appearance drawer
*
*/

// System includes
#include <AknsDrawUtils.h>
#include <AknsUtils.h>
#include <AknsItemID.h>

// User includes
#include "xncontroladapter.h"
#include "xnproperty.h"
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xneditmode.h"
#include "xnnode.h"
#include "xninactivitymonitor.h"

#include "xnfocuscontrol.h"

// Constants
const TInt KSkinGfxInnerRectShrink( 5 );

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnFocusControl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnFocusControl* CXnFocusControl::NewL( CXnAppUiAdapter& aAppUiAdapter )
    {
    CXnFocusControl* self = CXnFocusControl::NewLC( aAppUiAdapter );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnFocusControl* CXnFocusControl::NewLC( CXnAppUiAdapter& aAppUiAdapter )
    {
    CXnFocusControl* self = new ( ELeave ) CXnFocusControl( aAppUiAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::~CXnFocusControl()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnFocusControl::~CXnFocusControl()
    {
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::CXnFocusControl()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnFocusControl::CXnFocusControl( CXnAppUiAdapter& aAppUiAdapter )
    : iAppUiAdapter( aAppUiAdapter )
    {    
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnFocusControl::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::MakeVisible()
// 
// -----------------------------------------------------------------------------
//
void CXnFocusControl::MakeVisible( TBool aVisible )
    {
    TRAP_IGNORE( DoMakeVisibleL( aVisible ) );
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::DoMakeVisibleL()
// 
// -----------------------------------------------------------------------------
//
void CXnFocusControl::DoMakeVisibleL( TBool aVisible )
    {
    if ( aVisible != iVisible )
        {
        if ( !aVisible )
            {
            iVisible = aVisible;
            
            iRefused = EFalse;
            
            CXnNode* node( iAppUiAdapter.UiEngine().FocusedNode() ); 
                                   
            if ( node )
                {
                node->HideTooltipsL();
                                    
                CXnControlAdapter* control( node->Control() );
                
                if  ( control && control->RefusesFocusLoss() )
                    {
                    // Need to keep drawing focus appearance
                    iRefused = ETrue;
                    
                    node->UnsetStateL( 
                        XnPropertyNames::style::common::KPressedDown );
                    }
                else
                    {
                    node->UnsetStateL(  
                        XnPropertyNames::style::common::KFocus );                    
                    }
                                
                iAppUiAdapter.UiEngine().RenderUIL();                
                }    
            }
        else
            {
            iRefused = EFalse;
            
            iVisible = aVisible;
            }              
        }        
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::IsVisible()
// 
// -----------------------------------------------------------------------------
//
TBool CXnFocusControl::IsVisible() const
    {
    return iVisible;
    }

// -----------------------------------------------------------------------------
// CXnFocusControl::Draw()
// 
// -----------------------------------------------------------------------------
//
void CXnFocusControl::Draw( const TRect& aRect, CWindowGc& aGc ) const
    {
    TInt state( iAppUiAdapter.UiEngine().EditMode()->EditState() );
            
    // Don't draw focus appearance when shooting dragging widget in edit mode
    if ( state == CXnEditMode::EShootContent )
        {
        return;
        }
    
    if ( IsVisible() || iRefused ) 
        {                                       
        CXnNode* node( iAppUiAdapter.UiEngine().FocusedNode() );
        
        if ( node )
            {            
            CXnProperty* prop( NULL );
            
            TRAP_IGNORE( prop = node->GetPropertyL( 
                XnPropertyNames::common::KFocusAppearance ) );
                                            
            if ( prop && prop->StringValue() == XnPropertyNames::KNone )
                {
                // Current element refuses to draw focus appearance
                return;
                }
            
            TRect innerRect( aRect );            
            
            innerRect.Shrink( 
                KSkinGfxInnerRectShrink, KSkinGfxInnerRectShrink );
            
            MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
                 
            if ( node->IsStateSet( XnPropertyNames::style::common::KPressedDown ) )
                {
                AknsDrawUtils::DrawFrame( skin, aGc, aRect, innerRect,
                    KAknsIIDQsnFrHomePressed, KAknsIIDDefault );                
                }            
            
            if ( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
                {
                AknsDrawUtils::DrawFrame( skin, aGc, aRect, innerRect, 
                    KAknsIIDQsnFrHome, KAknsIIDDefault );                
                }            
            }       
        }
    }

// End of file

