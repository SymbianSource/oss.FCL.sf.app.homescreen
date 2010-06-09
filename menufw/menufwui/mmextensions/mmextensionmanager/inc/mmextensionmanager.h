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
* Description:   Menu Framewrok extension manager
*
*/


#ifndef C_MM_EXTENSION_MANAGER_H
#define C_MM_EXTENSION_MANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h>

class CMMExtensionPlugin;
class CLiwGenericParamList;
class MMmActionRequest;

/**
 * CMMExtensionManager is a class derived from MHNEcomInstallNotifierCallback.
 * It is responsible for loading, keeping and managing all ECom plugin
 * implementations. Additionaly, CMMExtensionManager receives command from UI
 * and decides which plugin have to handle the event.
 *
 * @since S60 5.0
 * @ingroup group_mmextensions
 */
NONSHARABLE_CLASS( CMMExtensionManager ): public CBase
	{
public: // Constructors and destructor

	/**
	 * Standard C++ Destructor.
	 */
	~CMMExtensionManager();

	/**
	 * Two-phased constructor.
     * @return Instance of the CMMExtensionManager class.
     * @param aActionRequest Interface enabling to issues request to App UI.
	 */
	IMPORT_C static CMMExtensionManager* NewL( MMmActionRequest& aActionRequest );

	/**
	 * Two-phased constructor.
     * @return Instance of the CMMExtensionManager class.
     * @param aActionRequest Interface enabling to issues request to App UI.
	 */
	static CMMExtensionManager* NewLC( MMmActionRequest& aActionRequest );
	
public:

	/** Pure interface method.
	* Representative of a method provided on the interface by 
	* the interface definer.
	* @param aUid UID of the ECom plugin's.
	* @param aCommand Command which has to be executed on the plugin.
	*                 Request to the Service.
	* @param aEventParamList List of the event parameters needed to
	*                        execute action. Extention Manager is its owner.
	* @return Number representing error code.
	*/
	virtual TInt ExecuteActionL( 
		const TUid aUid,
        const TDesC& aCommand,
        CLiwGenericParamList* aEventParamList );

private:

	/**
	 * Constructor for performing 1st stage construction.
	 * @param aActionRequest Interface enabling to issues request to App UI.
	 */
	CMMExtensionManager( MMmActionRequest& aActionRequest );
	
	/**
	 * EPOC default constructor for performing 2nd stage construction.
	 *
	 */
	void ConstructL();
	
	/**
	 * Loads MultimediaMenu plugins implementations.
	 */
	void LoadPluginsL( );
	
	/**
	 * Gets ECom plugin implementation.
	 * @param aUid Id of the ECom plugin.
	 * @return ECom extension plugin.
	 */
	CMMExtensionPlugin* GetImplementationL( const TUid aUid );
	
private:
	
	/**
	 * Pointer to plugin map.
	 * Own.
	 */
	RHashMap< TInt32, CMMExtensionPlugin*> iPluginMap;
	
    /**
	 * Not Own.
	 * Reference to the App UI.
	 */	
	MMmActionRequest& iActionRequest;

};

#endif // C_MM_EXTENSION_MANAGER_H

// End of file
