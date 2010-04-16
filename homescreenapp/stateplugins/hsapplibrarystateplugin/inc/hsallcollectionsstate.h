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

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param containerView Container view.
     * @param parent Owner.
     */
    HsAllCollectionsState(HsMenuView &menuView, QState *parent = 0);


    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    ~HsAllCollectionsState();

signals:

    /**
     * Signal emitted when sort order is changed.
     *
     * @since S60 ?S60_version.
     * @param sortOrder new sort order.
     */
    void sortOrderChanged(HsSortAttribute sortAttribute);

public slots:
    /**
     * Populates all folders
     *
     * @since S60 ?S60_version.
     */
    void scrollToBeginning();

private slots:

    bool openTaskSwitcher();

    /**
     * Slot connected to List widget in normal mode.
     *
     * @since S60 ?S60_version.
     * @param index Model index of the activated item.
     */
    void listItemActivated(const QModelIndex &index);

    /**
     * Slot connected to List widget in add mode.
     *
     * @since S60 ?S60_version.
     * @param index Model index of the activated item.
     */
    void addActivated(const QModelIndex &index);

    /**
     * Slot connected to List widget in normal mode.
     * Called when item long pressed.
     *
     * @since S60 ?S60_version.
     * @param item View item.
     * @param coords Press point coordinates.
     */
    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);

    /**
     * Slot connected to List widget in add mode.
     * Called when item long pressed.
     *
     * @since S60 ?S60_version.
     * @param item View item.
     * @param coords Press point coordinates.
     */
    void addLongPressed(HbAbstractViewItem *item,
                        const QPointF &coords);

    /**
     * Slot connected to constructMenu.
     *
     * @since S60 ?S60_version.
     */
    void createNewCollection();

    /**
     * Slot connected to constructMenu.
     *
     * @since S60 ?S60_version.
     */
    void createArrangeCollection();

    /**
     * Menu custom sort action slot.
     *
     * @since S60 ?S60_version.
     */
    void customMenuAction();

    /**
     * Menu ascending sort action slot.
     *
     * @since S60 ?S60_version.
     */
    void ascendingMenuAction();

    /**
     * Menu descending sort action slot.
     *
     * @since S60 ?S60_version.
     */
    void descendingMenuAction();

    /**
     * Slot invoked when a state is entered.
     *
     * @since S60 ?S60_version.
     */
    void stateEntered();

    /**
     * Slot invoked when add mode is entered.
     *
     * @since S60 ?S60_version.
     */
    void addModeEntered();

    /**
     * Slot invoked when normal mode is entered.
     *
     * @since S60 ?S60_version.
     */
    void normalModeEntered();

    /**
     * Slot invoked when normal mode is exited.
     *
     * @since S60 ?S60_version.
     */
    void normalModeExited();

    /**
     * Slot invoked when a state is exited.
     *
     * @since S60 ?S60_version.
     */
    void stateExited();

private:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     */
    void construct();
    /**
     * Creates and installs menu options for the view
     *
     * @since S60 ?S60_version.
     */
    void setMenuOptions();

private:

    /**
     * Sort order
     */
    HsSortAttribute mSortAttribute;

    /**
     * The List View widget.
     * Own.
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
