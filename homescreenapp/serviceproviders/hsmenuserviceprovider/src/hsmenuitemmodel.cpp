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
 * Description: hsmenuitemmodel.cpp
 *
 */

#include <hbnamespace.h>
#include <cauninstallnotifier.h>
#include <casoftwareregistry.h>

#include "hsmenuitemmodel.h"
#include "hsmenuserviceutils.h"
#include "hsiconsidleloader.h"

// Constants
const QSize smallIconSize(55, 55);

/*!
 Constructor
 \param query used to create model
 \param pointer to parent object
 */
HsMenuItemModel::HsMenuItemModel(const CaQuery &query, QObject *parent) :
    CaItemModel(query, parent),
    mIconsIdleLoader(NULL)
{
    setIconSize(smallIconSize);
    mComponentId = 0;
    mUninstallNotifier = 
        CaSoftwareRegistry::create()->createUninstallNotifier();
    connect(mUninstallNotifier, SIGNAL(progressChange(int, int)),
            this, SLOT(uninstallChange(int, int)));
}

/*!
 Destructor
 */
HsMenuItemModel::~HsMenuItemModel()
{
    if (!mIds.isEmpty()) {
        mIds.clear();
    }
}

/*!
 Sets sort order in the model
 */
void HsMenuItemModel::setSort(
    HsSortAttribute sortAttribute)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuItemModel::setSort");
    CaItemModel::setSort(HsMenuServiceUtils::sortBy(sortAttribute),
                         HsMenuServiceUtils::sortOrder(sortAttribute));
    HSMENUTEST_FUNC_EXIT(" HsMenuItemModel::setSort");
}

/*!
 Returns data of particular collection
 \param index
 \param role
 \retval
 */
QVariant HsMenuItemModel::data(const QModelIndex &index,
                               int role) const
{
    HSMENUTEST_FUNC_ENTRY("HsMenuItemModel::data");
    QVariant variant;

    if (role == Qt::DecorationRole && secondLineVisibility()
            && newIconNeeded(index)) {
        QList<QVariant> icons;

        icons << CaItemModel::data(index, role);
        icons << HbIcon(newIconId());

        variant = QVariant(icons);
    } else if (role == Hb::IndexFeedbackRole){
        QVariant display = CaItemModel::data(index, Qt::DisplayRole);
        if (display.type() == QVariant::String){
            variant = QVariant(display.toString().at(0).toUpper());;
        } else {
            variant = QVariant(display.toList().at(0).toString().at(0).toUpper());
        }
    } else {
        variant = CaItemModel::data(index, role);
    }
    HSMENUTEST_FUNC_EXIT("HsMenuItemModel::data");
    return variant;

}

/*!
 Checks if blink icon needed for entry
 \param entry entry representing an item
 \retval decision
 */
bool HsMenuItemModel::newIconNeeded(const QModelIndex &index) const
{
    HSMENUTEST_FUNC_ENTRY("HsMenuItemModel::newIconNeeded");
    bool result = false;
    if (root().isValid()
            && (!(entry(index)->flags() & UsedEntryFlag))
            && entry(root())->entryTypeName() == collectionDownloadedTypeName()) {
        result = true;
    }
    HSMENUTEST_FUNC_EXIT("HsMenuItemModel::newIconNeeded");
    return result;
}

/*!
 Updating uninstall progress with value for each item
  \param componentId Component Id of installed item
  \param valueOfProgress % value of current progress
 */
void HsMenuItemModel::uninstallChange(int componentId, int valueOfProgress)
{
    if (componentId!=mComponentId) {
        if (!mIds.isEmpty()) {
            mIds.clear();
        }
        // get items list with same componentID
        mIds = CaItemModel::getUninstallingEntriesIds(componentId);
        mComponentId = componentId;
    }
    // update each item with progress value
    foreach (int id, mIds) {
        CaItemModel::updateProgress(id, valueOfProgress);
    }
}


/*!
 Start preloading icons if idle
 \param entry entry representing an item
 \retval void
 */
void HsMenuItemModel::preloadIcons()
{
    mIconsIdleLoader = new HsIconsIdleLoader(this, this);
}
