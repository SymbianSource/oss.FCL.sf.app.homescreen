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

    HsCollectionState(HsMenuView &menuView, QState *parent = 0);

    ~HsCollectionState();

public slots:

    void collectionsSortOrder(HsSortAttribute sortAttribute);

protected:

    void onEntry(QEvent *event);

signals:

    void sortOrderChanged(HsSortAttribute sortAttribute);

private slots:

    bool openTaskSwitcher();

    void listItemActivated(const QModelIndex &index);

    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);

    void addAppsAction();

    void addCollectionShortcutToHomeScreenAction();

    void renameAction();

    void deleteAction();

    void backSteppingAction();

    void updateLabel();

    void stateEntered();

    void stateExited();

    void latestOnTopMenuAction();

    void oldestOnTopMenuAction();

private:

    void construct();

    /**
     * Set up Menu.
     *
     * @since S60 ?S60_version.
     * @param isDynamic Decides which options should be added to menu.
     */
    void constructMenu(bool isDynamic);

    void makeConnect();

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
     */
    HsMenuView &mMenuView;

    /**
     * Secondary Softkey action.
     * Backstepping functionality.
     * Own.
     */
    HbAction *mSecondarySoftkeyAction;

    /**
     * Old navigation icon.
     * Not own.
     */
    HbAction *mOldNavigationAction;
   

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
