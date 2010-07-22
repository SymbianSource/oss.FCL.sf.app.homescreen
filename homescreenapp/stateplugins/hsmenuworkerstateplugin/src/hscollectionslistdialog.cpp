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

#include <HbAction>
#include <HbLabel>
#include <QDebug>
#include <QStandardItemModel>

#include <hsmenuservice.h>
#include <hsmenuitemmodel.h>
#include "hscollectionslistdialog.h"

/*!
 \class HsCollectionsListDialog
 \ingroup group_hsworkerstateplugin
 \brief Hs List Dialog
 Hs List Dialog
 */

/*!
 \var HsCollectionsListDialog::mModel
 Model.
 */

/*!
 \var HsCollectionsListDialog::mItemId
 Selected item id.
 */

/*!
 \fn void HsCollectionsListDialog::makeConnect();
 Connects edit line signals to slots.
*/

/*!
 \fn void HsCollectionsListDialog::makeDisconnect();
 Disconnects edit line signals from slots.
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
    HbSelectionDialog(), mItemId(0)
{
    clearActions();
    HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"), this);
    addAction(cancelAction);

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

/*!
 Executes dialog.
 \retval Selected action.
 */
void HsCollectionsListDialog::open(QObject* receiver, const char* member)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    HbSelectionDialog::open(receiver, member);
}

/*!
    \reimp
    Disconnects signals and calls base class impl. which emits finished(HbAction*)
 */
void HsCollectionsListDialog::closeEvent ( QCloseEvent * event )
{
    qDebug("HsCollectionsListDialog::closeEvent");
    HbAction *closingAction = qobject_cast<HbAction *>(sender());

    if (closingAction != actions().value(0)) {
        QModelIndexList modlist = selectedModelIndexes();
        if (modlist.count()) {
            mItemId
            = mModel-> data(modlist[0], CaItemModel::IdRole).toInt();
        }
    }

    HbDialog::closeEvent(event); // emits finished(HbAction*)
}

/*!
 Creates standard item model.
 \param sortOrder sort order.
 \param collectionId id of collection to remove from model.
 \return QStandardItemModel - caller takes ownership.
 */
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

/*!
 Inserts "new collection" item into model.
 \param model a model.
 */
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
