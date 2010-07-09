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

#include <QState>
#include "hsbaseviewstate.h"
#include "hsmenumodewrapper.h"


HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAbstractViewItem;
class QPointF;
class HsMenuViewBuilder;
class HsMenuItemModel;
class HsMenuModeWrapper;
class HsMainWindow;

class HsAllAppsState: public HsBaseViewState
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

private slots:
    void addActivated(const QModelIndex &index);
    void addLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    void addToCollection();
    void openInstalledView();
    void ascendingMenuAction();
    void descendingMenuAction();
    void normalModeEntered();
    void stateExited();
    void contextMenuAction(HbAction *action);
private:
    void construct();
    void setMenuOptions();
    void addToHomeScreen(const QModelIndex &index);
    void setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags);

private:
    HsSortAttribute mSortAttribute;
};

#endif // HSALLAPPSSTATE_H
