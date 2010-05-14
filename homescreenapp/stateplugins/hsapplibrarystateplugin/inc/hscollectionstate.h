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
#include <QPointer>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"
#include "hsmenuview.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class QModelIndex;
class HsMenuItemModel;
class HsMenuModeWrapper;
class HsMenuViewBuilder;

class HsCollectionState: public QState
{
    Q_OBJECT
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
public:
    HsCollectionState(HsMenuViewBuilder &menuView,
                      HsMenuModeWrapper &menuMode,
                      QState *parent = 0);
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
    void listItemLongPressed(HbAbstractViewItem *item, const QPointF &coords);
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
	void contextMenuAction(HbAction *action);
private:
    void construct();
    void constructMenu(bool isDynamic);
    void makeConnect();
    void makeDisconnect();
    void addElementToHomeScreen(const QModelIndex &index);

private:
    HsSortAttribute mSortAttribute;
    HsSortAttribute mCollectionsSortAttribute;
    int mCollectionId;
    QString mCollectionType;
    HsMenuView mMenuView;
    HsMenuModeWrapper &mMenuMode;
    HbAction *const mSecondarySoftkeyAction;
    HsMenuItemModel *mCollectionModel;
    HbMenu *mOptions;
	QModelIndex mContextModelIndex;
	QPointer<HbMenu> mContextMenu;

};

#endif // HSCOLLECTIONSTATE_H
