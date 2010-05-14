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

#ifndef APPSCHECKLIST_H
#define APPSCHECKLIST_H

#include <QModelIndexList>
#include <hsmenuservice.h>
#include "hbmainwindow.h"

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

//forward declarations
class HbView;
class HbAction;
class HbListView;
class QStandardItemModel;
class HsMenuItemModel;

class HsAppsCheckList: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAppsCheckList();

    virtual ~HsAppsCheckList();

    void cleanUp();

    void setSortOrder(HsSortAttribute sortAttribute);

    void showAppsCheckboxList(HsSortAttribute sortAttribute);

signals:

    void commit(const QList<int> &appList);

protected slots:

    void selectApplicationsDone();

private:

    void constructControls();

    HbMainWindow *mainWindow() const;

    QList<int> getSortedItemsList(const QModelIndexList &modelIndexList);

private:

    HbView *mView;

    HbView *mPreviousView;

    HbAction *mActionConfirm;

    HbListView *mListView;

    HsMenuItemModel *mModel;

    HsSortAttribute mSortAttribute;

    QObjectList mLoadedObjects;

};

#endif /* APPSCHECKLIST_H */
