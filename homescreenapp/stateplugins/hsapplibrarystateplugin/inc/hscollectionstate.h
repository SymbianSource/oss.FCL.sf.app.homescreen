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
 * Description: Menu collection state.
 *
 */

#ifndef HSCOLLECTIONSTATE_H
#define HSCOLLECTIONSTATE_H

#include <qstate.h>
#include <qabstractitemmodel.h>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class QModelIndex;
class HsMenuView;
class HsMenuItemModel;

/**
 * @ingroup group_hsmenustateplugin
 * @brief Collection State.
 *
 * Displays a concrete collection.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsCollectionState: public QState
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
    HsCollectionState(HsMenuView &menuView, QState *parent = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    ~HsCollectionState();

public slots:

    /**
     * Sets collections sort order
     *
     * @since S60 ?S60_version.
     * @param sortOrder sort order.
     */
    void collectionsSortOrder(HsSortAttribute sortAttribute);

protected:

    void onEntry(QEvent *event);

signals:

    void sortOrderChanged(HsSortAttribute sortAttribute);

private slots:

    /**
     * Slot connected to List widget.
     *
     * @since S60 ?S60_version.
     * @param index Model index of the activated item.
     */
    void listItemActivated(const QModelIndex &index);

    /**
     * Slot connected to List widget.
     * Called when item long pressed.
     *
     * @since S60 ?S60_version.
     * @param item View item.
     * @param coords Press point coordinates.
     */
    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);


    /**
     * Menu add applications action slot
     *
     * @since S60 ?S60_version.
     */
    void addAppsAction();

    /**
     * Menu add shortcut action slot
     *
     * @since S60 ?S60_version.
     */
    void addCollectionShortcutToHomeScreenAction();

    /**
     * Menu rename action slot
     *
     * @since S60 ?S60_version.
     */
    void renameAction();

    /**
     * Menu delete action slot
     *
     * @since S60 ?S60_version.
     */
    void deleteAction();

    /**
     * Menu softkey back action slot
     *
     * @since S60 ?S60_version.
     */
    void backSteppingAction();

    /**
     * Updates label
     *
     * @since S60 ?S60_version.
     */
    void updateLabel();


    /**
     * Slot invoked when a state is entered.
     *
     * @since S60 ?S60_version.
     */
    void stateEntered();

    /**
     * Slot invoked when a state is exited.
     *
     * @since S60 ?S60_version.
     */
    void stateExited();

    void latestOnTopMenuAction();

    void oldestOnTopMenuAction();

private:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     */
    void construct();

    /**
     * Set up Menu.
     *
     * @since S60 ?S60_version.
     * @param isDynamic Decides which options should be added to menu.
     */
    void constructMenu(bool isDynamic);

    /**
     * Perform all signal connections.
     *
     * @since S60 ?S60_version.
     */
    void makeConnect();

    /**
     * Perform all signal disconnections.
     *
     * @since S60 ?S60_version.
     */
    void makeDisconnect();

    void addElementToHomeScreen(const QModelIndex &index);

private:

    /**
     * Sort order
     */
    HsSortAttribute mSortAttribute;

    /**
     * All collections sort order
     */
    HsSortAttribute mCollectionsSortAttribute;

    /**
     * The id of the current collection.
     */
    int mCollectionId;

    /**
     * The type of the current collection.
     */
    QString mCollectionType;

    /**
     * The List View widget.
     * Own.
     */
    HsMenuView &mMenuView;

    /**
     * Secondary Softkey action.
     * Backstepping functionality.
     * Own.
     */
    HbAction *mSecondarySoftkeyAction;

    /**
     * Item Model for the List.
     * Own.
     */
    HsMenuItemModel *mCollectionModel;

    /**
     * Options menu.
     * Own.
     */
    HbMenu *mOptions;

};

#endif // HSCOLLECTIONSTATE_H
