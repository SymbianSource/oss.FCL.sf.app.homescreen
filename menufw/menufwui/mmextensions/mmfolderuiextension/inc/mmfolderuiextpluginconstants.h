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
* Description:  Contatns used by folder UI extension plugin
 *
*/


#ifndef MMFOLDERUIEXTPLUGINCONTANTS_H
#define MMFOLDERUIEXTPLUGINCONTANTS_H

/**
 * Constants used in add folder UI extension plugin.
 * For XML configuration as well.
 */

// implemantation UID
const TInt32 KCFolderUiExtPluginUid = { 0x2001CB79 };

// command names, also used in XML configuration
_LIT( KAddFolderCmd,    "MMCreateNewFolderCommand" );
_LIT( KRenameFolderCmd, "MMRenameFolderCommand" );
_LIT( KDeleteFolderCmd, "MMDeleteFolderCommand" );
_LIT( KDeleteAppCmd,    "MMDeleteAppCommand" );
_LIT( KDeleteSuiteCmd,  "MMDeleteSuiteCommand" );
_LIT( KMoveToFolderCmd, "MMMoveToFolderCommand" );


#endif /*MMFOLDERUIEXTPLUGINCONTANTS_H*/
