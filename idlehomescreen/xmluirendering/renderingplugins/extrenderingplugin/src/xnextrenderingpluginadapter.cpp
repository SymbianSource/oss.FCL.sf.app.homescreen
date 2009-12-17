/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  External rendering plugin adapter base class implementation
*
*/

// System include files
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <xnextrenderingpluginadapter.h>

// Local constants

// TODO: Content controller ECom interface UID
const TUid KInterfaceUidContentController = { 0x20026F51 };

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CXnExtRenderingPluginAdapter* CXnExtRenderingPluginAdapter::NewL( TUid aImplUid )
    {
    TAny* ptr = REComSession::CreateImplementationL( aImplUid,
        _FOFF( CXnExtRenderingPluginAdapter, iDestructKey ) );

    CXnExtRenderingPluginAdapter* adapter =
        reinterpret_cast< CXnExtRenderingPluginAdapter* >( ptr );
        
    if( adapter )
        {
        adapter->iImplUid = aImplUid;
        }
    
    return adapter;
    }
    
// -----------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::ImplUid()
// -----------------------------------------------------------------------
//
EXPORT_C TUid CXnExtRenderingPluginAdapter::ImplUid() const
    {
    return iImplUid;
    }
    
// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::~CXnExtRenderingPluginAdapter()
// ----------------------------------------------------------------------------
//
EXPORT_C CXnExtRenderingPluginAdapter::~CXnExtRenderingPluginAdapter()
    {
    REComSession::DestroyedImplementation( iDestructKey );
    }
  
// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::EnterPowerSaveModeL()
// ----------------------------------------------------------------------------
//
EXPORT_C void CXnExtRenderingPluginAdapter::EnterPowerSaveModeL()
	{
	}    
	
// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::ExitPowerSaveModeL()
// ----------------------------------------------------------------------------
//
EXPORT_C void CXnExtRenderingPluginAdapter::ExitPowerSaveModeL()
	{
	}
	
// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::SkinChanged()
// ----------------------------------------------------------------------------
//
EXPORT_C void CXnExtRenderingPluginAdapter::SkinChanged()
	{
	}  	

// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::FocusChanged()
// ----------------------------------------------------------------------------
//
EXPORT_C void CXnExtRenderingPluginAdapter::FocusChanged( TDrawNow /*aDrawNow*/ )
	{
	}
	
// ----------------------------------------------------------------------------
// CXnExtRenderingPluginAdapter::SizeChanged()
// ----------------------------------------------------------------------------
//
EXPORT_C void CXnExtRenderingPluginAdapter::SizeChanged()
	{
	}


// End of file
