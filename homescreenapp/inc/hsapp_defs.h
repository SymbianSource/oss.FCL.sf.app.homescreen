/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: declaration of functions providing string constants
 *
 */

#ifndef HSAPP_DEFS_H_
#define HSAPP_DEFS_H_
#include <qstring.h>

inline const QString widgetLibraryAttributeName();
inline const QString widgetUriAttributeName();
inline const QString applicationTypeName();
inline const QString urlTypeName();
inline const QString collectionTypeName();
inline const QString collectionDownloadedTypeName();
inline const QString menuCollectionsTypeName();
inline const QString widgetTypeName();
inline const QString templatedApplicationTypeName();
inline const QString widgetParam();
inline const QString openActionIdentifier();
inline const QString removeActionIdentifier();
inline const QString itemIdKey();
inline const QString collectionIdKey();
inline const QString collectionSortOrderKey();
inline const QString appSortOrderKey();
inline const QString entryTypeNameKey();
inline const QString applicationUidEntryKey();
inline const QString urlEntryKey();
inline const QString defaultCollectionIconId();
inline const QString menuModeType();
inline const QString swTypeKey();
inline const QString javaSwType();
inline const QString packageTypeName();
inline const QString appSettingsPlugin();
inline const QString groupNameAttributeName();

// Sort attribute
enum HsSortAttribute {
    NoHsSortAttribute,
    AscendingNameHsSortAttribute,
    DescendingNameHsSortAttribute,
    LatestOnTopHsSortAttribute,
    OldestOnTopHsSortAttribute,
    CustomHsSortAttribute
};

enum HsMenuMode {
    NormalHsMenuMode,
    AddHsMenuMode
};

enum HsContextAction {
    AddToHomeScreenContextAction = 1,
    AddToCollectionContextAction,    
    UninstallContextAction, 
    AppSettingContextAction,
    RenameContextAction, 
    DeleteContextAction, 
    RemoveFromCollectionContextAction    
};


const char TS_DEVICE_DIALOG_URI[] = 
    "com.nokia.taskswitcher.tsdevicedialogplugin/1.0";

const char LIBRARY[] = "library";
const char URI[] = "uri";
const char PREFERENCES[] = "preferences";

const int softwareUpdateApplicationUid  = 0x2001FE2F;

#include "hsapp_defs.inl"

#endif
