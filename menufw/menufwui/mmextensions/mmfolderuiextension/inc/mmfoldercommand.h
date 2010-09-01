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
* Description:   Interface of the folder command
 *
*/



#ifndef MMFOLDERCOMMAND_H
#define MMFOLDERCOMMAND_H

#include <e32base.h>
#include <liwcommon.h>
#include "mmdialogmanager.h"
#include "mcsmenusapi.h"

class CMmPluginSharedResources;
class CHnMdBaseKey;
class CHnServiceHandler;
class CAknQueryDialog;
class MMmActionRequest;
class MMmDialogManager;

/**
 * Base class for folder operations
 * @since S60 v5.0
 */

/**
 * CMmFolderCommand is a class which all command classes derived from.
 * It has a several common functions responsible for creating appropriate
 * command instance, displaying dialogs, controlling command execution,
 * getting data from resources, displaying dialogs and executing MCS queries.
 * CMmFolderCommand is using only as a base class in inheritance.
 *
 * @lib mmfolderuiextensionplugin.lib
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmFolderCommand ) : public CBase, 
                                        public MMmDialogManager,
                                        public MLiwNotifyCallback
    {

protected:
    
    /**
     * Current state of folder name validation.
     */
    enum TValidation
        {
        EValidationOk = 0,
        EValidationInvalid = 1,
        EValidationNotAllowed = 2
        };

public:
   
    /**
     * Factory Method which returns concrete class
     * representing folder operation.
     * @param aCommand Descriptor contains required command.
     * @return If aCommand is supported, operations instance is returned.
     */
    static CMmFolderCommand* FactoryL( const TDesC& aCommand );
    
    /**
     * Returns TInt value using the given path.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @return TInt value or KErrNotFound.
     */
    TInt GetIntL( const CLiwGenericParamList& aParam, const TDesC8& aPath );
    
    /**
     * Returns string value using the given path.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @return String value or KNullDesC.
     */
    TPtrC GetStringL( const CLiwGenericParamList& aParam, const TDesC8& aPath );
    
    /**
     * Returns bool value using the given path.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @return TBool value.
     */
    TBool GetBoolL( const CLiwGenericParamList& aParam, const TDesC8& aPath );
    
    /**
     * Performs operation on folder.
     * @param aEventParamList List of parameters for operations.
     */
    virtual void ExecuteActionL( CLiwGenericParamList* aEventParamList ) = 0;
    
    /**
     * Setter for App UI.
     * @param aActionRequest Interface enabling to issues request to App UI.
     */ 	
 	void SetRequestInterface( MMmActionRequest* aActionRequest );
 	
    /**
     * Setter for shared resources.
     * @param aActionRequest Interface enabling to issues request to App UI.
     */ 	
 	void SetSharedResources( CMmPluginSharedResources* aResources ); 
 	
    /**
     * Setter for dialog.
     * @param aDialog Dialog instance.
     * @return Result of the dialog.
     */
    void SetDialogL( CAknQueryDialog* aDialog );
    
    /**
     * Removes active dialog.
     */
    void RemoveDialogL();
 	
 	/**
 	 * Virtual C++ Destructor.
 	 */
    virtual ~CMmFolderCommand();
    
protected:

	/**
	 * Base Plugin constructor
	 */ 
	void BaseConstructL();
	
    /**
    * Gets folder name.
    * @param aFolderName Name of the folder.
    * @return Modal dialog displaying result.
    */ 
    TBool DoChangeNameL( RBuf& aFolderName, TInt aDefaultNameLength,
        RBuf& aNumberInBracket, CLiwGenericParamList& aFoldersNames, 
        TLiwVariant& aVar, TInt aNumberToAppend = 0 );

    /**
     * Gets folder name.
     * @param aFolderName Name of the folder.
     * @return Modal dialog displaying result.
     */ 
    void ChangeNameIfAlreadyExistsL( RBuf& aFolderName,
        CLiwGenericParamList* aFoldersNames, TInt aNumberToAppend = 0 );
    
    /**
     * Gets list of the folders names.
     * @param aFolderName Name of the folder.
     */
    void GetFolderListL( TInt aItemId );
    
    /**
     * Gets list of the items names.
     * @param aFolderName Name of the folder.
     */
    void GetListL( TInt aItemId );
    
    /**
     * Gets folder name.
     * @param aFolderName Name of the folder.
     * @return Modal dialog displaying result.
     */ 
    TBool GetFolderNameL( RBuf& aFolderName, 
            CLiwGenericParamList* aFoldersNames = 0 );
    
    /**
     * Displays notification contains reason of the deletion cancel.
     * @param aFolderName Name of the folder.
     */
    void DisplayErrorInfoL( const TDesC& aFolderName );
    
    /**
     * Executes query on MCS service.
     * @param aQueryName Query name.
     * @param aInputData Input data.
     * @param aFilter Query filter.
     * @param aCallback Interface handling query execution.
     */ 
    void MCSQueryExecutionL( const TDesC8& aQueryName,
                             const CHnMdBaseKey* aInputData,
                             const CHnMdBaseKey* aFilter,
                             MLiwNotifyCallback* aCallback );
    
    /**
     * Add new key to existing key.
     * @param aInDataKey An existing key.
     * @param aKeyName Name of the new key.
     * @param aKeyType Type of the new key.
     * @param aKeyValue Value of the new key.
     */
    void AddSubKeyL( CHnMdBaseKey* aInDataKey, 
                     const TDesC8& aKeyName, 
                     const TDesC8& aKeyType, 
                     const TDesC8& aKeyValue );
    
    /**
     * Add new key to existing key.
     * @param aInDataKey An existing key.
     * @param aKeyName Name of the new key.
     * @param aKeyType Type of the new key.
     * @param aKeyValue Value of the new key.
     */
    void AddSubKeyL( CHnMdBaseKey* aInDataKey, 
                     const TDesC8& aKeyName, 
                     const TDesC8& aKeyType, 
                     const TDesC& aKeyValue );
    
    /**
     * Checks if folder name is valid and consists of allowed characters
     * @param aFolderName Folder name to check
     * @return Validation result.
     */
    TInt FolderNameValidationL(RBuf& aFolderName,
            CLiwGenericParamList* aFoldersNames = 0);
    
    /**
     * Removes illegal character
     * @param aFolderName Folder name to check
     * @return Validation result.
     */
    TInt IllegalCharacterRemoveL(RBuf& aFolderName,
            CLiwGenericParamList* aFoldersNames = 0);
    
    /**
     * Checks if folder name is valid
     * @param aFolderName Folder name to check
     * @return Validation result.
     */
    TInt CheckValidFileNameL( const TDes& aFolderName );
    
    /**
     * Checks if folder name consists of allowed characters
     * @param aFolderName Folder name to check
     * @return Validation result.
     */
    TInt CheckAllowedCharsL( const TDes& aFolderName );
    
    /**
     * Displays notification contains reason of the add folder/rename folder name  cancel.
     * @param aResourceId The numeric ID of the resource string to be read.
     */
    void DisplayNameErrorInfoL( const TDesC& aMsg );
    
private:
	
    /**
     * Prepare GetList input data for specified folder
     * @param aItemId id of folder.
     */
	void PrepareGetListInputL(TInt aItemId);
	
	/**
	 * Prepare GetList filter for folders
	 */
	void PrepareGetListFolderFilterL();
	
    /**
     * Prepare GetList filter for folders
     */
    void PrepareGetListFilterL();
	
	/**
	 * Insert Liw map to Liw list
     * @param aList
     * @param aKey
     * @param aMap
	 */
	void InsertToLiwListL(CLiwGenericParamList* aList, 
			const TDesC8& aKey, const CLiwDefaultMap* aMap); 
	
	/**
	 * Externalize GetList input list
	 */
    void ExternalizeInputListL();
    
    /**
     * Internalize GetList output list
     */
    void InternalizeOutputListL();
	
    
protected:
    
    /**
     * Own.
     * Intance of the service handler.
     */
    CHnServiceHandler* iServiceHandler;
    
    
    /**
	 * Not Own.
	 * Pointer to the App UI.
	 */
    MMmActionRequest* iActionRequest;
    
    /**
     * Own.
     * Pointer to active dialog.
     */
    CAknQueryDialog* iDialog;
    
    /**
     * Flag indicating wheather dialog can be displayed.
     */
    TBool iLockDialog;
    
    /*
     * MCS sesion.
     * Own.
     * */
    RMenuSapi iMcs;
    
    /**
     * GetList input and output parameter lists.
     * Own.
     */
    CLiwGenericParamList* iGetListInParam;
    CLiwGenericParamList* iGetListOutParam;
    
    /**
     * GetList serialized input and output parameter lists.
     * Own.
     */
    RBuf8 iSerializedInputList;
    RBuf8 iSerializedOutputList;
    
    /**
     * Pointer to shared resources class.
     * Not owned.
     */
    CMmPluginSharedResources* iSharedResources;
    
    };

#endif /*MMFOLDERCOMMAND_H*/

// End of file
