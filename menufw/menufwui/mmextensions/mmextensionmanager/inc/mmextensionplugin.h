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
* Description: Menu Framework extension plugin
*
*/


#ifndef C_MM_EXTENSION_PLUGIN_H_
#define C_MM_EXTENSION_PLUGIN_H_

#include <e32base.h>
#include <ecom/ecom.h>
#include "mmextensionplugindef.h"

class CLiwGenericParamList;

/**
 * CMMExtensionPlugin is a class representing ECom plugin.
 * Class has virtual function responsible for executing
 * command. It can also returns array of all implementations.
 * Class is deriving by all ECom plugin implementations.
 *
 * @since S60 5.0
 * @ingroup group_mmextensions
 */
NONSHARABLE_CLASS( CMMExtensionPlugin ) : public CBase
	{
public:
 	/** 
 	* Instantiates an object of this type.
    * @param aUid ECom plugin's UID.
    * @param aParams The plugin's parameters.
    * @return Instance of the CMMExtensionPlugin class.
    */
 	static CMMExtensionPlugin* NewL( TUid aUid ,  TAny* aParams );

 	/**
 	 * Virtual C++ Destructor.
 	 */
    virtual ~CMMExtensionPlugin();
    
	/** 
	* Request a list of all available implementations which 
	* satisfy this given interface.
    * @param aImplInfoArray ECom plugin implementations array.
    */
	static void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );

    /**
     * Executes provided action.
     * @param aCommand Command which has to be executed on the plugin.
     *                 Request to the Service.
     * @param aEventParamList Pointer to a list of the event parameters. 
     *                        Its ownership is held in extension manager,
     *                        so if one wishes to use it in asynchronous operations
     *                        then he needs to keep a copy of the data.
     * @return Error code.
     */
    virtual TInt ExecuteActionL( 
        const TDesC& aCommand,
        CLiwGenericParamList* aEventParamList ) = 0;

protected:
	/**
	 * Default constructor.
	 */
	inline CMMExtensionPlugin();

private:
	/**
	 * Unique instance identifier key.
	 */
	TUid iDtor_ID_Key;
	};
	
#include "mmextensionplugin.inl"

#endif /* C_MMEXTENSIONPLUGIN_H_ */

// End of file    
