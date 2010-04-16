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

    /**
     * Slot connected trrigger action of secondary soft key of check list box.
     * It is called when done button is selected.
     *
     * @since S60 ?S60_version.
     */
    void selectApplicationsDone();

private:

    /**
     * Construct contrlos.
     */
    void constructControls();

    /**
     * Returns pointer to tha main window.
     *
     * @since S60 ?S60_version.
     * @return Pointer to the main window.
     */
    HbMainWindow *mainWindow() const;

    /**
     * Returns list with selected items ids.
     *
     * @param modelIndexList list with selected model indexes.
     * @return list with items ids.
     */
    QList<int> getSortedItemsList(const QModelIndexList &modelIndexList);

private:

    /**
     * View. Owned.
     */
    HbView *mView;

    /**
     * Previous view. Not owned.
     */
    HbView *mPreviousView;

    /**
     * Confirm action. Owned.
     */
    HbAction *mActionConfirm;

    /**
     * List view. Owned.
     */
    HbListView *mListView;

    /**
     * Standard Item Model for the List.
     * Own.
     */
    HsMenuItemModel *mModel;

    /**
     * Sort order for applications.
     */
    HsSortAttribute mSortAttribute;

	/**
     * List of objects loaded by document loader. Content owned.
     */
    QObjectList mLoadedObjects;

};

#endif /* APPSCHECKLIST_H */
