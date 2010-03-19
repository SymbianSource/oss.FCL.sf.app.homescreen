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

#include "hsapp_defs.inl"

#endif
