/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Native Ui's content publisher plug-ins.
*
*/


#ifndef AINATIVEUIPLUGINS_H
#define AINATIVEUIPLUGINS_H

#include <aisystemuids.hrh>
#include "aipropertyextension.h"
#include "aiprofilepluginuids.hrh"

const TUid KDeviceStatusPluginUid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN };
_LIT( KDeviceStatusPluginName, "0x102750F8" );

const TUid KShortcutPluginUid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_SHORTCUTPLUGIN };
_LIT( KShortcutPluginName, "Shortcut" );

const TUid KProfilePluginUid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN };
_LIT( KProfilePluginName, "Profile" );

_LIT8( KNativeUiNamespace, "NativeUiNamespace" );

#endif // AINATIVEUIPLUGINS_H
