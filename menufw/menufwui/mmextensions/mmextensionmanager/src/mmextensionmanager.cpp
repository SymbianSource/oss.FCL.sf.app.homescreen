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
* Description:   Menu Framework Extension Manager
*
*/

#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL

#include "mmextensionplugin.h"
#include "mmextensionmanager.h"
#include "menudebug.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMExtensionManager::CMMExtensionManager( MMmActionRequest& aActionRequest ):
    iActionRequest( aActionRequest )
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMExtensionManager::~CMMExtensionManager()
	{
	THashMapIter< TInt32, CMMExtensionPlugin* > iter( iPluginMap);	
    for (CMMExtensionPlugin* const* ptr =
                                ( CMMExtensionPlugin* const* ) iter.NextValue();
        ptr; ptr = ( CMMExtensionPlugin* const* ) iter.NextValue())
        {
            delete (CMMExtensionPlugin*)(*ptr);
        }    
	iPluginMap.Close ();
	
	
    //delete iPluginNameMap
    /*THashMapIter< HBufC*, TInt32 > iter1( iPluginNameMap );
    while ( HBufC*const* ptrHbuf = iter1.NextKey() )
        {		
        delete *ptrHbuf;
        }  
    iPluginNameMap.Close();*/
	
    delete iEcomObserver;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMExtensionManager* CMMExtensionManager::NewLC( MMmActionRequest& aActionRequest )
	{
	CMMExtensionManager* self = new (ELeave) CMMExtensionManager( aActionRequest );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CMMExtensionManager* CMMExtensionManager::NewL( 
                                                   MMmActionRequest& aActionRequest )
	{
	CMMExtensionManager* self = CMMExtensionManager::NewLC( aActionRequest );
	CleanupStack::Pop (); // self;
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMMExtensionManager::ConstructL()
	{
    iEcomObserver = CMMEcomObserver::NewL( this );
	LoadPluginsL();
	}

// ---------------------------------------------------------------------------
// CASpaPluginManager::LoadPluginsL
// Load plugins implementations
// ---------------------------------------------------------------------------
//    
void CMMExtensionManager::LoadPluginsL()
    {	
	DEBUG16(("\t_MM_: CMMExtensionManager::LoadPluginsL()") );
	
	// Read info about all implementations into infoArray
	RImplInfoPtrArray infoArray;
	
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	CleanupResetAndDestroyPushL( infoArray );
	CMMExtensionPlugin::ListAllImplementationsL( infoArray );
	
	// Loop through each info for each implementation
	// and create and use each in turn
	CMMExtensionPlugin* plug;
	for ( TInt i = 0; i < infoArray.Count(); i++ )
		{
		// Slice off first sub-section in the data section
	    TUid current_plugin = infoArray[i]->ImplementationUid();
		plug = CMMExtensionPlugin::NewL( current_plugin , &iActionRequest );
		CleanupStack::PushL( plug );
		TInt32 key = current_plugin.iUid;
	    iPluginMap.InsertL( key, plug );
	    CleanupStack::Pop( plug );
		plug = NULL;
		}
    CleanupStack::PopAndDestroy( &infoArray );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMMExtensionManager::ExecuteActionL( 
	    const TUid aUid,
        const TDesC& aCommand,
	    CLiwGenericParamList* aEventParamList )
	{
    TInt result = KErrNotFound;
	if( aUid.iUid != KNullUidValue )
	    {
	    CMMExtensionPlugin* pluginInstance = GetImplementationL( aUid );
	    if( pluginInstance ) 
	        {
	        result = pluginInstance->ExecuteActionL(
	                aCommand, aEventParamList );
	        }
	    }
	else
	    {
	    THashMapIter<TInt32, CMMExtensionPlugin*> plugins( iPluginMap );
	    CMMExtensionPlugin* const* plugin = NULL;
	    for( plugin = plugins.NextValue(); plugin;
	            plugin = plugins.NextValue() )
	        {
	        (*plugin)->ExecuteActionL( aCommand, aEventParamList );
	        }
	    result = KErrNone;
	    }
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMMExtensionPlugin* CMMExtensionManager::GetImplementationL(
    const TUid aUid )
    {
    CMMExtensionPlugin* ret = NULL;
    CMMExtensionPlugin **imp = NULL;

    TInt pluginId ( aUid.iUid );
    
    imp = iPluginMap.Find( pluginId );
    if( imp )
        {
        ret = *imp;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMMExtensionManager::EcomChangedL()
    {
    THashMapIter< TInt32, CMMExtensionPlugin* > iter( iPluginMap );
    for (CMMExtensionPlugin* const* ptr =
                                ( CMMExtensionPlugin* const* ) iter.NextValue();
        ptr; ptr = ( CMMExtensionPlugin* const* ) iter.NextValue())
        {
            delete (CMMExtensionPlugin*)(*ptr);
        }
    iPluginMap.Close();
    LoadPluginsL();
    }

// End of file
