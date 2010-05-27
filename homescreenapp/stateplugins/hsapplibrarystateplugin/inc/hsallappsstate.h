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
#include <QPointer>

#include "hsmenustates_global.h"
#include "hsmenumodewrapper.h"
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
class HsMenuModeWrapper;
class HsMainWindow;

class HsAllAppsState: public QState
{
    Q_OBJECT
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
public:
    HsAllAppsState(HsMenuViewBuilder &menuViewBuilder,
                   HsMenuModeWrapper &menuMode,
                   HsMainWindow &mainWindow,
                   QState *parent = 0);
    ~HsAllAppsState();
signals:
    void toAppLibraryState();

public slots:
    void scrollToBeginning();
    void collectionsSortOrder(HsSortAttribute sortAttribute);
private slots:
    bool openTaskSwitcher();
    bool checkSoftwareUpdates();
    void listItemActivated(const QModelIndex &index);
    void addActivated(const QModelIndex &index);
    void listItemLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    void addLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    void addToCollection();
    void openInstalledView();
    void ascendingMenuAction();
    void descendingMenuAction();
    void stateEntered();
    void addModeEntered();
    void normalModeEntered();
    void stateExited();
    void contextMenuAction(HbAction *action);
private:
    void construct();
    void setMenuOptions();
    void addToHomeScreen(const QModelIndex &index);
private:
    HbAction *mSecondarySoftkeyAction;
    HsSortAttribute mSortAttribute;
    HsSortAttribute mCollectionsSortAttribute;
    HsMenuView mMenuView;
    HsMenuModeWrapper &mMenuMode;
    HsMenuItemModel *mAllAppsModel;
    HsMainWindow &mMainWindow;
    QModelIndex mContextModelIndex;
    QPointer<HbMenu> mContextMenu;
};

#endif // HSALLAPPSSTATE_H
