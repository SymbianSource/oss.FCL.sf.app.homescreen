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

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param parent Owner.
     */
    HsAppLibraryState(QState *parent = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~HsAppLibraryState();

signals:

    /**
     * Signal emitted when leaving the state,
     *  i.e when the back softkey is pressed.
     *
     * @since S60 ?S60_version.
     */
    void toHomescreenState();

    /**
     * Signal emitted when entering the state
     *
     * @since S60 ?S60_version.
     */
    void initialize();

public slots:

    void clearToolbarLatch();

    /**
     * All apps stete entered.
     * @since S60 ?S60_version.
     */
    void allAppsStateEntered();

    /**
     * All collections state entered.
     * @since S60 ?S60_version.
     */
    void allCollectionsStateEntered();

private slots:

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


protected:

    void onEntry(QEvent *event);

private:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     */
    void construct();

    /**
     * Sets up Toolbar.
     *
     * @since S60 ?S60_version.
     */
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
