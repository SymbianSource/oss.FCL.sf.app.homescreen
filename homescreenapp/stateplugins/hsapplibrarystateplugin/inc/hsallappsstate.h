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
 * Description: Menu All Applications state.
 *
 */

#ifndef HSALLAPPSSTATE_H
#define HSALLAPPSSTATE_H

#include <qstate.h>
#include <QModelIndex>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class QPointF;
class HsMenuView;
class HsMenuItemModel;
/**
 * @ingroup group_hsmenustateplugin
 * @brief All Applications State.
 *
 * Displays all applications present on the device in a sorted list.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsAllAppsState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:
    HsAllAppsState(HsMenuView &menuView, QState *parent = 0);
    ~HsAllAppsState();

public slots:
    void scrollToBeginning();
    void collectionsSortOrder(HsSortAttribute sortAttribute);

private slots:

    bool openTaskSwitcher();

    bool checkSoftwareUpdates();

    void listItemActivated(const QModelIndex &index);

    void addActivated(const QModelIndex &index);

    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);

    void addLongPressed(HbAbstractViewItem *item,
                        const QPointF &coords);

    void addToCollection();

    void openInstalledView();

    void ascendingMenuAction();

    void descendingMenuAction();

    void stateEntered();

    void addModeEntered();

    void normalModeEntered();

    void stateExited();

private:

    void construct();

    void setMenuOptions();

    void addToHomeScreen(const QModelIndex &index);

private:

    /**
     * Sort order
     */
    HsSortAttribute mSortAttribute;

    /**
     * Collections sort order
     */
    HsSortAttribute mCollectionsSortAttribute;

    /**
     * The View widget.
     */
    HsMenuView &mMenuView;

    /**
     * Item Model for the List.
     * Own.
     */
    HsMenuItemModel *mAllAppsModel;

    QModelIndex mBookmark;

};

#endif // HSALLAPPSSTATE_H