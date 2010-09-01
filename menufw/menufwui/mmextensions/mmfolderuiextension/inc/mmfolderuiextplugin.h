/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Folder UI extension plugin
 *
*/



#ifndef MMFOLDERUIEXTPLUGIN_H
#define MMFOLDERUIEXTPLUGIN_H

#include <e32base.h>
#include <liwcommon.h>
#include "mmextensionplugin.h"

class CMmPluginSharedResources;
class CMmFolderCommand;
class MMmActionRequest;

/**
 * CMmFolderUiExtPlugin is an Ecom plugin implementation responsible for
 * executing operations on folders from MCS. Plugin receives command from UI,
 * creates appropriate handler and starts command handling.
 * 
 * CMmFolderUiExtPlugin implements CMMExtensionPlugin interface. The most
 * important function is ExecuteActionL which receives parameters from
 * extension manager. One of those parameters is command name. Plugin is
 * supporting following commands:
 * - MMCreateNewFolderCommand,
 * - MMRenameFolderCommand,
 * - MMDeleteFolderCommand,
 * - MMDeleteAppCommand,
 * - MMDeleteSuiteCommand.
 * If function received other command it would return KErrNotSupported error
 * code.
 * 
 * UI extension plugin need to be declared and configured in xml file as part
 * of mmenu:action tag, which is inside mmenu:event tag. Any data that should
 * be passed to plugin have to be defined inside mmenu:data tag as mmenu:key
 * tags.
 *
 * @lib mmfolderuiextensionplugin.lib
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderUiExtPlugin ) : public CMMExtensionPlugin
	{
public:
   
    /** 
     * Instantiates an object of this type.
     * @return Instance of the CMmFolderUiExtPlugin class.
     * @param aActionRequest Interface enabling to issues request to App UI.
     */
    static CMmFolderUiExtPlugin* NewL( MMmActionRequest& aActionRequest );

 	/**
 	 * Virtual C++ Destructor.
 	 */
 	~CMmFolderUiExtPlugin();
    
    /**
     * Executes provided action.
     * Has to be implemented by each UI extension plugin.
     * @param aEventParamList Event parameters.
     * @param aCommand Command to be executed.
     * @return Error code.
     */
    TInt ExecuteActionL( const TDesC& aCommand,
    					 CLiwGenericParamList* aEventParamList );

protected:
	/**
	 * Default C++ constructor.
     * @param aActionRequest Interface enabling to issues request to App UI.
	 */
	CMmFolderUiExtPlugin( MMmActionRequest& aActionRequest );
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
	 
private:
    /**
     * Check wheather Menu is in foreground.
     * @return ETrue if Menu is in foreground, otherwise EFalse;
     */
    TBool IsMatrixInForegroundL();
    
private:

	/**
	 * Own.
	 * Pointer to the folder command.
	 */
    CMmFolderCommand* iCommand;
    
    /**
	 * Not Own.
	 * Reference to the App UI.
	 */
    MMmActionRequest& iActionRequest;
    
    /**
     * Own.
     * Shared resources used in plugins.
     * Created during first command execution.
     */
    CMmPluginSharedResources* iSharedResources;
    
    };

#endif /*MMFOLDERUIEXTPLUGIN_H*/

// End of file
