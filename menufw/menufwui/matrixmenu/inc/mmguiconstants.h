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
* Description:  Constants for the matrixmenu
*
*/


#ifndef MMGUICONSTANTS_H
#define MMGUICONSTANTS_H

#include "mmguiuid.hrh"

// UID for the application,
// this should correspond to the uid defined in the mmp file
const TUid 	KUidMatrixMenuApp = { _MATRIX_UID3 };

// fixed menu items position multiplier
const TInt KMenuPosMultiplier( 100 );
// supported number of buttons in toolbar
const TInt KNumOfButtonsInToolbar( 3 );

// used for exiting Matrix
const TInt KPhoneAppUid( 0x100058B3 );

// EMHU-7KW9WM 
const TInt KVideoCallAppUid( 0x101F8681 );  
// used for initializing Matrix
_LIT8( KParamSuiteName8,                 "suite_name" );
_LIT ( KParamFolderSuite,                "foldersuite" );
_LIT8( KParamFolderId8,                  "folder_id" );
_LIT8( KParamRemoveLocked8, 			 "remove_locked" );
const TInt32 KParamFolderId( 1 );
_LIT8( KParamMoveLocked8,                "move_locked" );
_LIT ( KParamFalse,                      "false" );

#endif // MMGUICONSTANTS_H
