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
* Description: Holds plugins shared resources
* Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
*
*/
#ifndef CMMPLUGINSHAREDRESOURCES_H_
#define CMMPLUGINSHAREDRESOURCES_H_

class CMmPluginSharedResources : public CBase
	{
public:
	
    /** 
 	 * Instantiates an object of this type.
     * @return Instance of the CMmPluginSharedResources class.
 	 */
 	static CMmPluginSharedResources* NewL();
	
	/**
	 * Second phase Symbian constructor.
	 */
	void ConstructL();
	
 	/**
 	 * C++ Destructor.
 	 */
	~CMmPluginSharedResources();
	
	/**
	 * Getter for resource file
	 * @return String file path. 
	 */
	const TDesC& GetLanguageFilePath() { return *iLanguageFilePath; }
	
	/**
	 * Getter for R_MENU_EDIT_DEFAULT_FOLDER_NAME
	 * @return String loaded from resource. 
	 */
	const TDesC& GetDefaultFolderName() { return *iDefFolderName; }
	
	/**
	 * Getter for R_MENU_EDIT_FOLDER_NAME 
	 * @return String loaded from resource. 
	 */
	const TDesC& GetEditFolderName() { return *iEditFolderName; }
	
	/**
	 * Getter for R_MENU_EDIT_FLDR_BAD_FILE_NAME
	 * @return String loaded from resource. 
	 */
	const TDesC& GetMsgBadFileName() { return *iMsgBadFileName; }
	
	/**
	 * Getter for R_MENU_EDIT_FLDR_ILLEGAL_CHARACTERS
	 * @return String loaded from resource. 
	 */
	const TDesC& GetMsgIllegalCharacters() { return *iMsgBadChar; }
	
	/**
	 * Getter for R_MENU_EDIT_ROOT_FOLDER
	 * @return String loaded from resource. 
	 */
	const TDesC& GetRootFolder() { return *iRootFolder; }
	
	/**
	 * Getter for R_MENU_EDIT_FLDR_DELETING_WAIT_NOTE
	 * @return String loaded from resource. 
	 */
	const TDesC& GetMsgFolderDeleting() { return *iDelWaitNote; }
	
private:
    /**
     * Resources. Own.
     */
    HBufC* iLanguageFilePath;
    HBufC* iDefFolderName;
    HBufC* iEditFolderName;
    HBufC* iMsgBadFileName;
    HBufC* iMsgBadChar;
    HBufC* iRootFolder;
	HBufC* iDelWaitNote;
    
	/**
	 * Resource offset.
	 */
    TInt iResourceOffset;
	};

#endif /* CMMPLUGINSHAREDRESOURCES_H_ */
