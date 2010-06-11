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
#include <QPointer>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"
#include "hsmenuview.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class QPointF;
class HsMenuViewBuilder;
class HsMenuItemModel;
class HsMainWindow;

class HsInstalledAppsState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsInstalledAppsState(HsMenuViewBuilder &menuViewBuilder,
                         HsMainWindow &mainWindow,
                         QState *parent = 0);

    ~HsInstalledAppsState();

private slots:

    bool openTaskSwitcher();

    void listItemActivated(const QModelIndex &index);

    void listItemLongPressed(HbAbstractViewItem *item,
                             const QPointF &coords);

    void backAction();

    void stateEntered();

    void stateExited();

    void setEmptyLabelVisibility(bool visibility);

    void contextMenuAction(HbAction *action);

private:

    void construct();

    void setMenuOptions();

private:
    /**
     * The View widget.
     * Own.
     */
    HsMenuView mMenuView;

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
    HbAction *const mSecondarySoftkeyAction;
    
    HsMainWindow &mMainWindow;

    QModelIndex mContextModelIndex;
    QPointer<HbMenu> mContextMenu;
};

#endif // HSINSTALLEDAPPSSTATE_H
