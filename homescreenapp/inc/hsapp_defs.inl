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
 * Description: contains definition of constant strings
 *
 */

#ifndef HSAPPDEFS_INL_
#define HSAPPDEFS_INL_

/*!
\return widget library attribute name
*/
inline const QString widgetLibraryAttributeName()
{
    static const QString name("widget:library");
    return name;
}

/*!
\return widget uri attribute name
*/
inline const QString widgetUriAttributeName()
{
    static const QString name("widget:uri");
    return name;
}

/*!
\return application entry type name
*/
inline const QString applicationTypeName()
{
    static const QString name("application");
    return name;
}

/*!
\return url entry type name
*/
inline const QString urlTypeName()
{
    static const QString name("url");
    return name;
}

/*!
\return collection entry type name
*/
inline const QString collectionTypeName()
{
    static const QString name("collection");
    return name;
}

/*!
\return collection donwloaded entry type name
*/
inline const QString collectionDownloadedTypeName()
{
    static const QString name("collection::downloaded");
    return name;
}
/*!
\return menu collections entry type name
*/
inline const QString menuCollectionsTypeName()
{
    static const QString name("menucollections");
    return name;
}

/*!
\return widget entry type name
*/
inline const QString widgetTypeName()
{
    static const QString name("widget");
    return name;
}

/*!
\return identifier for open an item action
*/
inline const QString openActionIdentifier()
{
    static const QString name("open");
    return name;
}

/*!
\return identifier for remove an item action
*/
inline const QString removeActionIdentifier()
{
    static const QString name("remove");
    return name;
}

/*!
\return item id property key
*/
inline const QString itemIdKey()
{
    static const QString key("item_id");
    return key;
}

/*!
    \return collection id property key
*/
inline const QString collectionIdKey()
{
    static const QString key("collection_id");
    return key;
}

/*!
    \return collection sort order property key
*/
inline const QString collectionSortOrderKey()
{
    static const QString key("collection_sort_order");
    return key;
}

/*!
    \return application sort order property key
*/
inline const QString appSortOrderKey()
{
    static const QString key("app_sort_order");
    return key;
}

/*!
    \return entry type name property key
*/
inline const QString entryTypeNameKey()
{
    static const QString key("entry_type_name");
    return key;
}

/*!
    \return application uid entry key
*/
inline const QString applicationUidEntryKey()
{
    static const QString key("application:uid");
    return key;
}

/*!
    \return url entry key
*/
inline const QString urlEntryKey()
{
    static const QString name("url");
    return name;
}

/*!
    \return default collection icon id
*/
inline const QString defaultCollectionIconId()
{
    static const QString name("qtg_large_applications_user");
    return name;
}

/*!
    \return default bling icon id , used to mark unused downloaded apps
*/
inline const QString newIconId()
{
    static const QString name("qtg_large_super_new");
    return name;
}


/*!
    \return menu mode type
*/
inline const QString menuModeType()
{
    static const QString name("menu_mode_type");
    return name;
}

#endif
