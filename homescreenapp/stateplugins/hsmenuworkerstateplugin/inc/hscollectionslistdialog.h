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

#include <HbSelectionDialog>
#include <hsmenuservice.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)
class QStandardItemModel;

class HsCollectionsListDialog: public HbSelectionDialog
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsCollectionsListDialog(HsSortAttribute sortOrder,
                            int collectionId = 0);

    virtual ~HsCollectionsListDialog();

    void open(QObject* receiver, const char* member );

    int getItemId();

protected:
    void closeEvent( QCloseEvent * event );

private:

    void makeConnect();

    void makeDisconnect();

    QStandardItemModel *standartItemModel(HsSortAttribute sortOrder,
                                          int collectionId);

    void insertNewCollectionItem(QStandardItemModel *model);

private:

    QStandardItemModel *mModel;

    int mItemId;
};

#endif /* HS_LIST_DIALOG_H */
