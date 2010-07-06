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
#include <QPointer>

#include "hsbaseviewstate.h"
#include "hsmenumodewrapper.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbMenu;
class HsMenuViewBuilder;
class HbAbstractViewItem;
class HbAction;
class HbMainWindow;
class HsMenuItemModel;
class HsMenuView;
class HsMainWindow;

class HsAllCollectionsState: public HsBaseViewState
{
    Q_OBJECT
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
public:
    HsAllCollectionsState(HsMenuViewBuilder &menuViewBuilder,
                          HsMenuModeWrapper &menuMode,
                          HsMainWindow &mainWindow,
                          QState *parent = 0);
    ~HsAllCollectionsState();
protected:
signals:
    void sortOrderChanged(HsSortAttribute sortAttribute);
    void toAppLibraryState();

public slots:
    void scrollToBeginning();
private slots:
    bool openTaskSwitcher();
    void listItemActivated(const QModelIndex &index);
    void addActivated(const QModelIndex &index);
    void listItemLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    void addLongPressed(HbAbstractViewItem *item, const QPointF &coords);
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
    void contextMenuAction(HbAction *action);
private:
    void construct();
    void setMenuOptions();
private:
    HbAction *mSecondarySoftkeyAction;
    HsSortAttribute mSortAttribute;
    HsMenuView mMenuView;
    HsMenuModeWrapper &mMenuMode;
    HsMenuItemModel *mAllCollectionsModel;
    HsMainWindow &mMainWindow;
    QModelIndex mContextModelIndex;
    QPointer<HbMenu> mContextMenu;
};

#endif // HSALLCOLLECTIONSSTATE_H
