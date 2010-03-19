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
 * Description:
 *
 */

#ifndef HS_LIST_DIALOG_H
#define HS_LIST_DIALOG_H

#include <hblistdialog.h>
#include <hsmenuservice.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)
class QStandardItemModel;

class HsCollectionsListDialog: public HbListDialog
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsCollectionsListDialog(HsSortAttribute sortOrder,
                            int collectionId = 0);

    virtual ~HsCollectionsListDialog();

    HbAction *exec();

    int getItemId();

private:

    /**
     * Connects edit line signals to slots.
     */
    void makeConnect();

    /**
     * Disconnects edit line signals from slots.
     */
    void makeDisconnect();

    /**
     * Creates standard item model.
     *
     * @since S60 ?S60_version.
     * @param sortOrder sort order.
     * @param collectionId id of collection to remove from model.
     * @return QStandardItemModel - caller takes ownership.
     */
    QStandardItemModel *standartItemModel(HsSortAttribute sortOrder,
                                          int collectionId);

    /**
     * Inserts "new collection" item into model.
     *
     * @since S60 ?S60_version.
     * @param model a model.
     */
    void insertNewCollectionItem(QStandardItemModel *model);

private:

    /**
     * Model
     */
    QStandardItemModel *mModel;

    /**
     * Selected item id
     */
    int mItemId;
};

#endif /* HS_LIST_DIALOG_H */
