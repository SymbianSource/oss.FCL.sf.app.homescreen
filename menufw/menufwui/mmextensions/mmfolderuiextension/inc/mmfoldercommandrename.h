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
* Description:   Rename folder command
 *
*/




#ifndef MMFOLDERCOMMANDRENAME_H
#define MMFOLDERCOMMANDRENAME_H

#include <e32base.h>
#include "mmfoldercommand.h"

/**
 * CMmFolderCommandRename is a class inherits from CMmFolderCommand.
 * It's responsible for handling command renaming folder in MCS. Plugin supports
 * MMRenameFolderCommand command. It displays query, get name and update item
 * from MCS.
 *
 * @lib mmfolderuiextensionplugin.lib
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderCommandRename ) : public CMmFolderCommand
    {
public:
    
    /** 
 	 * Instantiates an object of this type.
     * @return Instance of the CMmFolderCommandRename class.
 	 */
 	static CMmFolderCommandRename* NewL();

 	/**
 	 * Virtual C++ Destructor.
 	 */
 	~CMmFolderCommandRename(); 

private:

    /**
     * Performs rename folder operation.
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
     * Gets new name of the folder.
     * @param aId Id of the item.
     * @param aFolderName Reference to buffer contains folder name.
     */ 
    void RenameFolderL( TInt aId, RBuf& aFolderName );
	
	/**
	 * Default C++ constructor.
	 */
	CMmFolderCommandRename();
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
     	
private:
    
    };

#endif /*MMFOLDERCOMMANDRENAME_H*/

// End of file
