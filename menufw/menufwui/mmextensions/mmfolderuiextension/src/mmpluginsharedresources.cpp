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
* Contributors: Holds plugins shared resources
*
* Description: 
* Version     : %version: 3 % << Don't touch! Updated by Synergy at check-out.
*
*/
#include <StringLoader.h>
#include "mmpluginsharedresources.h"
#include "hnutils.h"

#include <mmfolderuiextplugin.rsg>

_LIT( KResourceFile, "mmfolderuiextplugin.rsc" );

// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmPluginSharedResources* CMmPluginSharedResources::NewL()
	{
	CMmPluginSharedResources* self = new( ELeave ) CMmPluginSharedResources();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmPluginSharedResources::ConstructL()
	{
	// Init resources
    iLanguageFilePath = HnUtils::LocateNearestLanguageFileLC( KResourceFile );
    CleanupStack::Pop(iLanguageFilePath);
	iResourceOffset = CCoeEnv::Static()->AddResourceFileL( 
			iLanguageFilePath->Des() );
	
    iDefFolderName = StringLoader::LoadLC( R_MENU_EDIT_DEFAULT_FOLDER_NAME );
    CleanupStack::Pop(iDefFolderName);
    iEditFolderName = StringLoader::LoadLC( R_MENU_EDIT_FOLDER_NAME );
    CleanupStack::Pop(iEditFolderName);
    iMsgBadFileName = StringLoader::LoadLC( R_MENU_EDIT_FLDR_BAD_FILE_NAME );
    CleanupStack::Pop(iMsgBadFileName);
    iMsgBadChar = StringLoader::LoadLC(R_MENU_EDIT_FLDR_ILLEGAL_CHARACTERS);
    CleanupStack::Pop(iMsgBadChar);  
    iRootFolder = StringLoader::LoadLC( R_MENU_EDIT_ROOT_FOLDER );
    CleanupStack::Pop(iRootFolder);
	iDelWaitNote = StringLoader::LoadLC(R_MENU_EDIT_FLDR_DELETING_WAIT_NOTE);
	CleanupStack::Pop(iDelWaitNote);
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmPluginSharedResources::~CMmPluginSharedResources()
	{
	delete iDelWaitNote;
	delete iRootFolder;
	delete iMsgBadChar;
	delete iMsgBadFileName;
	delete iEditFolderName;
	delete iDefFolderName;
	delete iLanguageFilePath;
	CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
	}
