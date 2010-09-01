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
* Description:   Add folder command
 *
*/



#ifndef MMFOLDERCOMMANDADD_H
#define MMFOLDERCOMMANDADD_H

#include <e32base.h>
#include "mmfoldercommand.h"


/**
 * Class representing add folder operations
 * @since S60 v5.0
 */

/**
 * CMmFolderCommandAdd is a class inherits from CMmFolderCommand.
 * It's responsible for handling command adding folder to MCS. Plugin supports
 * MMCreateNewFolderCommand command. It displays query, get name and executes
 * MCS query creating new folder. After successfully folder creation it send
 * request to HN engine to set focus on added folder.
 *
 * @lib mmfolderuiextensionplugin.lib
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderCommandAdd ) : public CMmFolderCommand
    {
public:
    /** 
 	 * Instantiates an object of this type.
     * @return Instance of the CMmFolderCommandAdd class.
 	 */
 	static CMmFolderCommandAdd* NewL();

 	/**
 	 * Virtual C++ Destructor.
 	 */
 	~CMmFolderCommandAdd();    

private:
    
    /**
     * Performs add folder operation.
     * @param aEventParamList List of the event parameters.
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
     * Handle adding folder, caalled just after the GetList
     */
    void ExecuteAddFolderL();
     
    /**
     * Creates folder in MCS.
     * @param aFolder Folder name.
     */
    void CreateFolderL( const TDesC& aFolder );
	
    /**
	 * Default C++ constructor.
	 */
	CMmFolderCommandAdd();
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
    
   	
private:
    
    /**
     * Id of the parent folder.
     */
    TInt32 iParent;
    
    };

#endif /*MMFOLDERCOMMANDADD_H*/

// End of file
