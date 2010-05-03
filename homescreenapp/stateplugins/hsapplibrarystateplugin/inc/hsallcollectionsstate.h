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
 * Description: Menu All Collections state.
 *
 */

#ifndef HSALLCOLLECTIONSSTATE_H
#define HSALLCOLLECTIONSSTATE_H

#include <qstate.h>
#include <QModelIndex>
#include <QPointF>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbMenu;
class HsMenuView;
class HbAbstractViewItem;
class HbAction;
class HbMainWindow;
class HsMenuItemModel;
class HsMenuView;

/**
 * @ingroup group_hsmenustateplugin
 * @brief All Collections State.
 *
 * Displays all collections present on the device.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsAllCollectionsState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAllCollectionsState(HsMenuView &menuView, QState *parent = 0);

    ~HsAllCollectionsState();

signals:

    void sortOrderChanged(HsSortAttribute sortAttribute);

public slots:

    void scrollToBeginning();

private slots:

    bool openTaskSwitcher();

    void listItemActivated(const QModelIndex &index);

    void addActivated(const QModelIndex &index);

    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);

    void addLongPressed(HbAbstractViewItem *item,
                        const QPointF &coords);

    void createNewCollection();

    void createArrangeCollection();

    void customMenuAction();

    void ascendingMenuAction();

    void descendingMenuAction();

    void stateEntered();

    void addModeEntered();

    void normalModeEntered();

    void normalModeExited();

    void stateExited();

private:

    void construct();

    void setMenuOptions();

private:

    /**
     * Sort order
     */
    HsSortAttribute mSortAttribute;

    /**
     * The List View widget.
     */
    HsMenuView &mMenuView;

    /**
     * Item Model for the List.
     * Own.
     */
    HsMenuItemModel *mAllCollectionsModel;

    QModelIndex mBookmark;
};

#endif // HSALLCOLLECTIONSSTATE_H
