/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions for Menu Framewrok extension plugins
 *
*/



// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline CMMExtensionPlugin::CMMExtensionPlugin()
	{
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline CMMExtensionPlugin::~CMMExtensionPlugin()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation( iDtor_ID_Key );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline CMMExtensionPlugin* CMMExtensionPlugin::NewL( TUid aUid , TAny* aParams )
	{
	return REINTERPRET_CAST( CMMExtensionPlugin*, 
		REComSession::CreateImplementationL( aUid, 
		   _FOFF( CMMExtensionPlugin, iDtor_ID_Key ) , aParams ) ) ;
	}   

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void CMMExtensionPlugin::ListAllImplementationsL(
	RImplInfoPtrArray& aImplInfoArray)
	{
	REComSession::ListImplementationsL( TUid::Uid( KCMMExtensionInterfaceUid ), 
		aImplInfoArray );
	}

// End of File
