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
 * Description: Hs list dialog
 *
 */

#include <hbaction.h>
#include <hblistdialog.h>
#include <hblabel.h>
#include <hsmenuservice.h>
#include <QDebug>
#include <QStandardItemModel>

#include <hsmenuitemmodel.h>
#include "hscollectionslistdialog.h"

/*!
 \class HsCollectionsListDialog
 \ingroup group_hsworkerstateplugin
 \brief Hs List Dialog
 Hs List Dialog
 */

/*!
 Constructor
 \param sortOrder sort order.
 \param collectionId collectionId id of collection
 to remove from list.
 \retval void
 */
HsCollectionsListDialog::HsCollectionsListDialog(HsSortAttribute sortOrder,
        int collectionId) :
    HbListDialog(), mItemId(0)
{
    setPrimaryAction(NULL);
    setHeadingWidget(new HbLabel(hbTrId("txt_applib_title_add_to")));
    // it must be single selection, although it shows checkboxes -
    // it is Orbit defect and will be fixed in next release
    setSelectionMode(HbAbstractItemView::SingleSelection);
    mModel = standartItemModel(sortOrder, collectionId);
    setModel(mModel);
}

/*!
 Destructor
 */
HsCollectionsListDialog::~HsCollectionsListDialog()
{
    delete mModel;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP // Reason: Modal inputdialog exec
#endif //COVERAGE_MEASUREMENT

/*!
 Executes dialog.
 \retval Selected action.
 */
HbAction *HsCollectionsListDialog::exec()
{
    HbAction *action = HbListDialog::exec();
    if (action != secondaryAction()) {
        QModelIndexList modlist = selectedModelIndexes();
        if (modlist.count()) {
            mItemId
            = mModel-> data(modlist[0], CaItemModel::IdRole).toInt();
        }
    }
    return action;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP // Reason: Modal inputdialog exec
#endif //COVERAGE_MEASUREMENT
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
QStandardItemModel *HsCollectionsListDialog::standartItemModel(
    HsSortAttribute sortOrder, int collectionId)
{
    HSMENUTEST_FUNC_ENTRY("HsListDialog::standartItemModel");
    QScopedPointer<HsMenuItemModel> caModel(
        HsMenuService::getAllCollectionsModel(sortOrder));
    caModel->setSecondLineVisibility(false);
    QStandardItemModel *model = new QStandardItemModel(this);
    QList<QStandardItem *> items;
    for (int row = 0; row < caModel->rowCount(); row++) {
        uint flags = caModel->data(caModel->index(row, 0),
                                   CaItemModel::FlagsRole).value<EntryFlags> ();
        int itemId = caModel->data(caModel->index(row, 0),
                                   CaItemModel::IdRole).toInt();
        if ((flags & RemovableEntryFlag) && (itemId != collectionId)) {
            QStandardItem *standardItem = new QStandardItem();
            standardItem->setData(caModel->data(caModel->index(row, 0),
                                                CaItemModel::IdRole), CaItemModel::IdRole);
            standardItem->setData(caModel->data(caModel->index(row, 0),
                                                Qt::DisplayRole), Qt::DisplayRole);
            standardItem->setData(caModel->data(caModel->index(row, 0),
                                                Qt::DecorationRole), Qt::DecorationRole);
            items << standardItem;
        }
    }
    model->insertColumn(0, items);

    insertNewCollectionItem(model);
    HSMENUTEST_FUNC_EXIT("HsListDialog::standartItemModel");
    return model;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionsListDialog::insertNewCollectionItem(
    QStandardItemModel *model)
{
    HSMENUTEST_FUNC_ENTRY("HsListDialog::insertNewCollectionItem");
    QStandardItem *standardItem = new QStandardItem();
    standardItem->setData(0, CaItemModel::IdRole);
    standardItem->setData(hbTrId("txt_applib_list_new_collection"),
                          Qt::DisplayRole);

    standardItem->setData(HbIcon(defaultCollectionIconId()),
                          Qt::DecorationRole);

    QList<QStandardItem *> items;
    items << standardItem;
    model->insertRow(0, items);
    HSMENUTEST_FUNC_EXIT("HsListDialog::insertNewCollectionItem");
}

/*!
 Gets selected item id.
 \retval Selected item id.
 */
int HsCollectionsListDialog::getItemId()
{
    return mItemId;
}
