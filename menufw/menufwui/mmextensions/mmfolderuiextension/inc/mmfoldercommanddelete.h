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
* Description:  Delete folder command
*
 *
*/




#ifndef MMFOLDERCOMMANDDELETE_H
#define MMFOLDERCOMMANDDELETE_H

#include <e32base.h>
#include <AknProgressDialog.h> // for MProgressDialogCallback
#include "mmfoldercommand.h"

class CAknWaitDialog;

/**
 * Class representing remove folder operations.
 * @since S60 v5.0
 */

/**
 * CMmFolderCommandDelete is a class inherits from CMmFolderCommand.
 * It's responsible for handling command removing folder from MCS. Plugin
 * supports MMDeleteFolderCommand command. It displays confirmation dialog,
 * removes item from MCS and displays notification with operation summary.
 *
 * @lib mmfolderuiextensionplugin.lib
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderCommandDelete ):
    public CMmFolderCommand,
    public MProgressDialogCallback
    {
public:
    
    /** 
 	 * Instantiates an object of this type.
     * @return Instance of the CMmFolderCommandDelete class.
 	 */
 	static CMmFolderCommandDelete* NewL();

 	/**
 	 * Virtual C++ Destructor.
 	 */
 	~CMmFolderCommandDelete();    

protected:
    /**
     * Performs add folder operation.
     * @param aEventParamList Parameters for operations.
     */
    void ExecuteActionL( CLiwGenericParamList* aEventParamList );
    
    /**
     * Handles notifications caused by an asynchronous Execute*CmdL call
     * or an event.
     * @param aCmdId The service command associated to the event.
     * @param aEventId Occured event, see LiwCommon.hrh.
     * @param aEventParamList Event parameters, if any, as defined per
     *        each event.
     * @param aInParamList Input parameters, if any, given in the
     *        related HandleCommmandL.
     * @return Error code for the callback.
     */
    TInt HandleNotifyL( TInt aCmdId, TInt aEventId,
             CLiwGenericParamList& aEventParamList,
             const CLiwGenericParamList& aInParamList );

	/**
	 * Default C++ constructor.
	 */
	CMmFolderCommandDelete();
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
    
   	/**
	 * Displays confirmation dialog.
	 * @param aFolderName Name of the folder.
	 */
    TBool DeleteQueryL( const TDesC& aFolderName );
    
    /**
     * Moves focus before delete.
     */
    void MoveFocusBeforeDeleteL();
    
    /**
	 * Starts wait dialog with progress bar.
	 * @param aLabel Wait dialog content.
	 */
    void StartWaitDialogL( const TDesC& aLabel );
    
    /**
	 * Stops wait dialog with progress bar.
	 */
    void StopWaitDialogL();
    
    /**
	 * Removes item from mcs.
	 * @param aId Id in MCS.
	 * @param aFolderName Name of the folder.
	 */
    void DeleteItemFromMcsL( TInt aId, const TDesC& aFolderName );
    
    /**
     * Starts uninstallation process.
     * @param aId Id in MCS.
     */
    void UninstallL( TInt aId );
    
    /**
     * Gets application package UID.
     * @param aUid UID of the application.
     * @return Application package UID.
     */
    TUid GetSisPackageUidL( TUid aUid );
    
    /**
     * Checks wheather item is a midlet.
     * @param aUid UID of the item.
     * @return ETrue if item is a midlet, otherwise EFalse.
     */
    TBool IsMidletL( TUid aUid );
    

private:
    
    /**
     * Callback method from MProgressDialogCallback interface.
     * Gets called when a dialog is dismissed.
     * @param aButtonId Id of the pushed button.
     */
    void DialogDismissedL( TInt aButtonId );

protected:
    
    /**
     * Own.
     * Pointer to wait dialog.
     */ 
    CAknWaitDialog* iWaitDialog;
    };


/**
 *  Class representing remove suite operations.
 *  @since S60 v5.0
 */

/**
 * CMmFolderDeleteSuite is a class deriving from CMmFolderCommandDelete.
 * It's responsible for handling command removing a suite from MCS.
 *
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderDeleteSuite ) : public CMmFolderCommandDelete
    {
public:
    
 	/**
 	 * Virtual C++ Destructor.
 	 */
 	~CMmFolderDeleteSuite();
    
    /** 
 	 * Instantiates an object of this type.
     * @return Instance of the CMmFolderDeleteSuite class.
 	 */
 	static CMmFolderDeleteSuite* NewL();

private:
	
	/**
     * Performs add folder operation.
     * @param aEventParamList Parameters for operations.
     */ 
    void ExecuteActionL( CLiwGenericParamList* aEventParamList );

	/**
	 * Default C++ constructor.
	 */
	CMmFolderDeleteSuite();
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
    
    };


/**
 *  Class representing remove application operations.
 *  @since S60 v5.0
 */

/**
 * CMmFolderDeleteApp is a class deriving from CMmFolderCommandDelete.
 * It's responsible for handling command removing an application from MCS.
 *
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderDeleteApp ) : public CMmFolderCommandDelete
	{
public:
    
    /** 
 	 * Instantiates an object of this type.
     * @return Instance of the CMmFolderDeleteApp class.
 	 */
 	static CMmFolderDeleteApp* NewL();

 	/**
 	 * Virtual C++ Destructor.
 	 */
 	~CMmFolderDeleteApp();


private:     
	
	/**
     * Performs add folder operation.
     * @param aEventParamList Parameters for operations.
     */ 
    void ExecuteActionL( CLiwGenericParamList* aEventParamList );

	/**
	 * Default C++ constructor.
	 */
	CMmFolderDeleteApp();
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
    
    };


#endif /*MMFOLDERCOMMANDDELETE_H*/

// End of file
