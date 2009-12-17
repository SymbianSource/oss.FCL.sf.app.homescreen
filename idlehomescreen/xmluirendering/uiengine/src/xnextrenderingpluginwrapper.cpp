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
* Description:  External rendering plugin wrapper implementation
*
*/

// System include files

// User include files
#include "xnextrenderingpluginwrapper.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::NewL()
// ----------------------------------------------------------------------------
//
CXnExtRenderingPluginWrapper* CXnExtRenderingPluginWrapper::NewL( 
    CXnNodePluginIf& aNode, CXnExtRenderingPluginAdapter& aAdapter )
    {
    CXnExtRenderingPluginWrapper* self = new ( ELeave ) CXnExtRenderingPluginWrapper( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL( aNode );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::ConstructL()
// ----------------------------------------------------------------------------
//
void CXnExtRenderingPluginWrapper::ConstructL( CXnNodePluginIf& aNode )
    {
    iNode = &aNode;
    CXnControlAdapter::ConstructL( aNode );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::CXnExtRenderingPluginWrapper()
// ----------------------------------------------------------------------------
//
CXnExtRenderingPluginWrapper::CXnExtRenderingPluginWrapper( CXnExtRenderingPluginAdapter& aAdapter )
    {
    iAdapter = &aAdapter;
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::~CXnExtRenderingPluginWrapper()
// ----------------------------------------------------------------------------
//
CXnExtRenderingPluginWrapper::~CXnExtRenderingPluginWrapper()
    {
    delete iAdapter;
    }    

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::CountComponentControls()
// ----------------------------------------------------------------------------
//
TInt CXnExtRenderingPluginWrapper::CountComponentControls() const
    {
    return 1;
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::ComponentControl()
// ----------------------------------------------------------------------------
//
CCoeControl* CXnExtRenderingPluginWrapper::ComponentControl( TInt aIndex ) const
    {
    if( aIndex == 0 )
        {
        return iAdapter;
        }
    else
        {
        return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::OfferKeyEventL()
// ----------------------------------------------------------------------------
//
TKeyResponse CXnExtRenderingPluginWrapper::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    return iAdapter->OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::SizeChanged()
// ----------------------------------------------------------------------------
//    
void CXnExtRenderingPluginWrapper::SizeChanged()
    {
    iAdapter->SetRect( iNode->Rect() );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::SkinChanged()
// ----------------------------------------------------------------------------
//    
void CXnExtRenderingPluginWrapper::SkinChanged()
    {
    CXnControlAdapter::SkinChanged();
    iAdapter->SkinChanged();
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::SetContainerWindowL()
// ----------------------------------------------------------------------------
//   
void CXnExtRenderingPluginWrapper::SetContainerWindowL(const CCoeControl& aContainer)
    {
    CXnControlAdapter::SetContainerWindowL( aContainer );
    iAdapter->SetContainerWindowL( *this );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::MakeVisible()
// ----------------------------------------------------------------------------
//
void CXnExtRenderingPluginWrapper::MakeVisible( TBool aVisible )
    {
    CXnControlAdapter::MakeVisible( aVisible );
    iAdapter->MakeVisible( aVisible );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::FocusChanged()
// ----------------------------------------------------------------------------
//
void CXnExtRenderingPluginWrapper::FocusChanged( TDrawNow aDrawNow )
    {
    CXnControlAdapter::FocusChanged( aDrawNow );    
    iAdapter->FocusChanged( aDrawNow );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::HandleScreenDeviceChangedL()
// ----------------------------------------------------------------------------
//
void CXnExtRenderingPluginWrapper::HandleScreenDeviceChangedL()
    {
    CXnControlAdapter::HandleScreenDeviceChangedL();
    iAdapter->HandleResourceChange( KEikDynamicLayoutVariantSwitch );
    }

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::DoEnterPowerSaveModeL()
// ----------------------------------------------------------------------------
//
void CXnExtRenderingPluginWrapper::DoEnterPowerSaveModeL( TModeEvent /*aEvent*/ )
	{
	iAdapter->EnterPowerSaveModeL();
	}
	
// ----------------------------------------------------------------------------
// CXnExtRenderingPluginWrapper::DoExitPowerSaveModeL()
// ----------------------------------------------------------------------------
//
void CXnExtRenderingPluginWrapper::DoExitPowerSaveModeL( TModeEvent /*aEvent*/ )
	{
	iAdapter->ExitPowerSaveModeL();
	}
	
// End of file
