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

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param containerView Container view.
     * @param parent Owner.
     */
    HsAllAppsState(HsMenuView &menuView, QState *parent = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    ~HsAllAppsState();

public slots:
    /**
     * Populates all applications
     *
     * @since S60 ?S60_version.
     */
    void scrollToBeginning();

    /**
     * Sets collections sort order
     *
     * @since S60 ?S60_version.
     * @param sortOrder sort order.
     */
    void collectionsSortOrder(HsSortAttribute sortAttribute);

private slots:

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
     * Slot called when application is adding to collection.
     *
     * @since S60 ?S60_version.
     */
    void addToCollection();

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
     * Slot invoked when add mode entered.
     *
     * @since S60 ?S60_version.
     */
    void addModeEntered();

    /**
     * Slot invoked when normal mode entered.
     *
     * @since S60 ?S60_version.
     */
    void normalModeEntered();

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
     * Own.
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
