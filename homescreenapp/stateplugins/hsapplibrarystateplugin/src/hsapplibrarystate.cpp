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
#include <qsignaltransition.h>
#include <qhistorystate.h>

#include <hbmainwindow.h>
#include <hbview.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbinstance.h>
#include <hblistview.h>
#include <hbsearchpanel.h>
#include <hsmenueventtransition.h>

#include "hstest_global.h"
#include "hsapplibrarystate.h"
#include "hsallappsstate.h"
#include "hsallcollectionsstate.h"
#include "hscollectionstate.h"
#include "hsinstalledappsstate.h"
#include "hsovistorehandler.h"
#include "hsmenuview.h"
#include "hsmenumodetransition.h"

/*!
 Constructor.
 \param parent Owner.
 */
HsAppLibraryState::HsAppLibraryState(QState *parent) :
    QState(parent), mSecondarySoftkeyAction(0), mAllAppsState(0),
    mHistoryTransaction(0), mAllCollectionsState(0), mCollectionState(0)
{
    construct();
}

/*!
 Destructor.
 */
HsAppLibraryState::~HsAppLibraryState()
{
    HbMainWindow *const hbW =
        HbInstance::instance()->allMainWindows().value(0);

    if (hbW) {
        mMenuView.view()->setNavigationAction(NULL);
        hbW->removeView(mMenuView.view());
    }
}

/*!
 Constructs contained objects.
 */
void HsAppLibraryState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::construct");
    setObjectName("homescreen.nokia.com/state/applibrarystate");

    mSecondarySoftkeyAction = new HbAction(Hb::BackAction, this);
    connect(mSecondarySoftkeyAction, SIGNAL(triggered()),
            SIGNAL(toHomescreenState()));

    mAllAppsState = new HsAllAppsState(mMenuView, this);
    connect(this, SIGNAL(entered()),mAllAppsState, SLOT(scrollToBeginning()));

    mAllCollectionsState = new HsAllCollectionsState(mMenuView, this);
    connect(this, SIGNAL(entered()),
            mAllCollectionsState, SLOT(scrollToBeginning()));

    QState *initialState = new QState(this);
    setInitialState(initialState);

    mHistoryTransaction =  new HsMenuModeTransition(
        mMenuView, NormalHsMenuMode, mAllAppsState);
    initialState->addTransition(mHistoryTransaction);
    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuView, AddHsMenuMode, mAllAppsState));

    mCollectionState = new HsCollectionState(mMenuView, this);
    connect(mCollectionState, SIGNAL(entered()),SLOT(clearToolbarLatch()));

    HsMenuEventTransition *eventTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenCollection,
                                  mAllCollectionsState, mCollectionState);
    mAllCollectionsState->addTransition(eventTransition);

    HsMenuEventTransition *collectionDeletedTransition =
        new HsMenuEventTransition(HsMenuEvent::CollectionDeleted,
                                  mCollectionState, mAllCollectionsState);
    mCollectionState->addTransition(collectionDeletedTransition);

    //transition for returning from collection
    HsMenuEventTransition *collectionToAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenApplicationLibrary,
                                  mCollectionState, mAllCollectionsState);
    mCollectionState->addTransition(collectionToAppLibTransition);

    HsInstalledAppsState *installedAppsState = new HsInstalledAppsState(
        mMenuView, this);

    HsMenuEventTransition *installedToAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenApplicationLibrary,
                                  installedAppsState, mAllAppsState);
    installedAppsState->addTransition(installedToAppLibTransition);

    HsMenuEventTransition *allViewToInstalledTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenInstalledView,
                                  mAllAppsState, installedAppsState);
    mAllAppsState->addTransition(allViewToInstalledTransition);

    constructToolbar();

    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    connect(mAllCollectionsState, SIGNAL(sortOrderChanged(HsSortAttribute)),
            mAllAppsState, SLOT(collectionsSortOrder(HsSortAttribute)));
    connect(mAllCollectionsState, SIGNAL(sortOrderChanged(HsSortAttribute)),
            mCollectionState, SLOT(collectionsSortOrder(HsSortAttribute)));

    connect(mAllAppsState, SIGNAL(entered()),
            this, SLOT(allAppsStateEntered()));
    connect(mAllCollectionsState, SIGNAL(entered()),
            this, SLOT(allCollectionsStateEntered()));

    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::construct");
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsAppLibraryState::onEntry(QEvent *event)
{
    qDebug("HsCollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::onEntry");
    QState::onEntry(event);

    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();
        mMenuView.setHsMenuMode(
            static_cast<HsMenuMode>(data.value(menuModeType()).toInt()));
    } else {
        mMenuView.setHsMenuMode(NormalHsMenuMode);
    }

    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::onEntry");
}

/*!
 Slot invoked when a state is entered.
 */
void HsAppLibraryState::stateEntered()
{
    HSTEST_FUNC_ENTRY("AppLibraryState::stateEntered");
    HbMainWindow *hbW = HbInstance::instance()->allMainWindows().value(0);
    if (!hbW->views().contains(mMenuView.view())) {
        hbW->addView(mMenuView.view());
        mMenuView.view()->setNavigationAction(mSecondarySoftkeyAction);
    }
    hbW->setCurrentView(mMenuView.view());
    HSTEST_FUNC_EXIT("AppLibraryState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsAppLibraryState::stateExited()
{
    HSTEST_FUNC_ENTRY("AppLibraryState::stateExited");
    HSTEST_FUNC_EXIT("AppLibraryState::stateExited");
}

/*!
 Sets up Toolbar.
 */
void HsAppLibraryState::constructToolbar()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::constructToolbar");

    HsOviStoreHandler *const oviStoreHandler = new HsOviStoreHandler(this);

    HbAction *const oviStoreAction(mMenuView.oviStoreAction());

    oviStoreAction->setIcon(HbIcon(oviStoreHandler->icon()));
    connect(mMenuView.oviStoreAction(), SIGNAL(triggered()),
            oviStoreHandler, SLOT(action()));

    HbAction *const allCollectionsAction(mMenuView.allCollectionsAction());

    mAllAppsState->addTransition(allCollectionsAction, SIGNAL(triggered()),
                                 mAllCollectionsState);
    mCollectionState->addTransition(allCollectionsAction,
                                    SIGNAL(triggered()), mAllCollectionsState);

    HbAction *const allAppsAction(mMenuView.allAppsAction());

    mAllCollectionsState->addTransition(allAppsAction,
                                        SIGNAL(triggered()), mAllAppsState);
    mCollectionState->addTransition(allAppsAction, SIGNAL(triggered()),
                                    mAllAppsState);

    mAllAppsState->assignProperty(
        allAppsAction, "checked", true);
    mAllCollectionsState->assignProperty(
        allCollectionsAction, "checked", true);
    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::constructToolbar");
}

/*!
 Clear toolbar latch
*/
void HsAppLibraryState::clearToolbarLatch()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::clearToolbarLatch");
    QAction *const checkedAction =
        mMenuView.toolBarActionGroup()->checkedAction();

    if (checkedAction != NULL) {
        checkedAction->setChecked(false);
    }
    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::clearToolbarLatch");
}

/*!
 All apps stete entered.
 */
void HsAppLibraryState::allAppsStateEntered()
{
    if (mMenuView.getHsMenuMode() == NormalHsMenuMode) {
        mHistoryTransaction->setTargetState(mAllAppsState);
    }
}

/*!
 All collections state entered.
 */
void HsAppLibraryState::allCollectionsStateEntered()
{
    if (mMenuView.getHsMenuMode() == NormalHsMenuMode) {
        mHistoryTransaction->setTargetState(mAllCollectionsState);
    }
}
