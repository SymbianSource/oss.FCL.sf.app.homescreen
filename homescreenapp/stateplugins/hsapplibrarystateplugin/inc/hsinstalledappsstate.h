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

#include <QState>

#include "hsbaseviewstate.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class QPointF;
class HsMenuViewBuilder;
class HsMenuItemModel;
class HsMainWindow;

class HsInstalledAppsState: public HsBaseViewState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsInstalledAppsState(HsMenuViewBuilder &menuViewBuilder,
                         HsMainWindow &mainWindow,
                         QState *parent = 0);

    ~HsInstalledAppsState();

private slots:

    void openInstallationLog();
    void stateEntered();
    void stateExited();
    void setEmptyLabelVisibility(bool visibility);
    void latestOnTopMenuAction();
    void oldestOnTopMenuAction();
    
private:

    void construct();
    void setMenuOptions();
    void setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags);
    Hs::HsSortAttribute mSortAttribute;
};

#endif // HSINSTALLEDAPPSSTATE_H
