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
 * Description: Menu Installed state.
 *
 */

#ifndef HSINSTALLEDAPPSSTATE_H
#define HSINSTALLEDAPPSSTATE_H

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
 * Displays all installed packages present on the device.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsInstalledAppsState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsInstalledAppsState(HsMenuView &menuView, QState *parent = 0);

    ~HsInstalledAppsState();

private slots:

    bool openTaskSwitcher();

    void listItemActivated(const QModelIndex &index);

    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);

    void backAction();

    void stateEntered();

    void stateExited();

private:

    void construct();

    void setMenuOptions();

private:

    /**
     * The View widget.
     * Own.
     */
    HsMenuView &mMenuView;

    /**
     * Item Model for the List.
     * Own.
     */
    HsMenuItemModel *mInstalledAppsModel;

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

};

#endif // HSINSTALLEDAPPSSTATE_H
