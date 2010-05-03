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
 * Description: Menu Application Library state.
 *
 */

#ifndef HSAPPLIBRARYSTATE_H
#define HSAPPLIBRARYSTATE_H

#include <QState>
#include <hbdocumentloader.h>
#include "hsmenustates_global.h"
#include "hsmenuview.h"
#include "hsapp_defs.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbAction;
class HbView;
class HbMainWindow;
class QActionGroup;

class HsAllAppsState;
class HsAllCollectionsState;
class HsCollectionState;
class HsMenuModeTransition;

/**
 * @ingroup group_hsmenustateplugin
 * @brief Application Library State.
 *
 * Parent state for Application Library functionality
 *  (browsing applications and collections)
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsAppLibraryState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAppLibraryState(QState *parent = 0);

    virtual ~HsAppLibraryState();

signals:

    void toHomescreenState();

    void initialize();

public slots:

    void clearToolbarLatch();

    void allAppsStateEntered();

    void allCollectionsStateEntered();

private slots:

    void stateEntered();

    void stateExited();


protected:

    void onEntry(QEvent *event);

private:

    void construct();

    void constructToolbar();

private:

    /**
     * Secondary Softkey action.
     * Usually backstepping functionality.
     * Own.
     */
    HbAction *mSecondarySoftkeyAction;

    /**
     * All Applications state.
     * Own.
     */
    HsAllAppsState *mAllAppsState;

    /**
     * Transition to remember last visited child state in Application Library.
     * Own.
     */
    HsMenuModeTransition *mHistoryTransaction;

    /**
     * All Collections state.
     * Own.
     */
    HsAllCollectionsState *mAllCollectionsState;

    /**
     * Collection state.
     * Own.
     */
    HsCollectionState *mCollectionState;

    HsMenuView mMenuView;
};

#endif //HSAPPLIBRARYSTATE_H