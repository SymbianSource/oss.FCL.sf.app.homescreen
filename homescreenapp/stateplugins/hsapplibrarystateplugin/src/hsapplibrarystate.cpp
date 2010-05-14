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
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbaction.h>
#include <HbToolBar>
#include <HbIcon>
#include <hbinstance.h>
#include <hblistview.h>
#include <hbsearchpanel.h>
#include <HbToolBarExtension>
#include <HbStyleLoader>
#include <hsmenueventtransition.h>

#include "hstest_global.h"
#include "hsapplibrarystate.h"
#include "hsallappsstate.h"
#include "hsallcollectionsstate.h"
#include "hscollectionstate.h"
#include "hsinstalledappsstate.h"
#include "hsoperatorhandler.h"
#include "hsmenuview.h"
#include "hsmenumodetransition.h"

/*!
 \class HsAppLibraryState
 \ingroup group_hsmenustateplugin
 \brief Application Library State.
 Parent state for Application Library functionality
 (browsing applications and collections)
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsAppLibraryState::mSecondarySoftkeyAction
 Secondary Softkey action.
 Usually backstepping functionality.
 Own.
 */

/*!
 \var HsAppLibraryState::mAllAppsState
 All Applications state.
 Own.
 */

/*!
 \var HsAppLibraryState::mHistoryTransaction
 Transition to remember last visited child state in Application Library.
 Own.
 */

/*!
 \var HsAppLibraryState::mAllCollectionsState
 All Collections state.
 Own.
 */

/*!
 \var HsAppLibraryState::mCollectionState
 Collection state.
 Own.
 */

/*!
 \fn void HsAppLibraryState::toHomescreenState();
 Signal emitted when leaving the state,
 i.e when the back softkey is pressed.
 */

/*!
 \fn void HsAppLibraryState::initialize();
 Signal emitted when entering the state
 */

/*!
 Constructor.
 \param parent Owner.
 */
HsAppLibraryState::HsAppLibraryState(QState *parent) :
    QState(parent), mAllAppsState(0),
    mHistoryTransaction(0), mAllCollectionsState(0), mCollectionState(0)
{
    construct();
}

/*!
 Destructor.
 */
HsAppLibraryState::~HsAppLibraryState()
{
    delete mAllAppsState;
    delete mAllCollectionsState;
    delete mCollectionState;
    delete mInstalledAppsState;

}

/*!
 Constructs contained objects.
 */
void HsAppLibraryState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::construct");
    setObjectName("homescreen.nokia.com/state/applibrarystate");

    mAllAppsState = new HsAllAppsState(mMenuViewBuilder, mMenuMode, this);
    connect(this, SIGNAL(entered()),mAllAppsState, SLOT(scrollToBeginning()));

    mAllCollectionsState = new HsAllCollectionsState(mMenuViewBuilder, mMenuMode, this);
    connect(this, SIGNAL(entered()),
            mAllCollectionsState, SLOT(scrollToBeginning()));

    QState *initialState = new QState(this);
    setInitialState(initialState);

    mHistoryTransaction =  new HsMenuModeTransition(
        mMenuMode, NormalHsMenuMode, mAllAppsState);

    initialState->addTransition(mHistoryTransaction);
    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuMode, AddHsMenuMode, mAllAppsState));

    mCollectionState = new HsCollectionState(mMenuViewBuilder,
            mMenuMode,
            this);

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

    mInstalledAppsState = new HsInstalledAppsState(
        mMenuViewBuilder, this);

    HsMenuEventTransition *installedToAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenApplicationLibrary,
                                  mInstalledAppsState, mAllAppsState);
    mInstalledAppsState->addTransition(installedToAppLibTransition);

    HsMenuEventTransition *allViewToInstalledTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenInstalledView,
                                  mAllAppsState, mInstalledAppsState);
    mAllAppsState->addTransition(allViewToInstalledTransition);

    constructToolbar();

    connect(mAllCollectionsState, SIGNAL(sortOrderChanged(HsSortAttribute)),
            mAllAppsState, SLOT(collectionsSortOrder(HsSortAttribute)));
    connect(mAllCollectionsState, SIGNAL(sortOrderChanged(HsSortAttribute)),
            mCollectionState, SLOT(collectionsSortOrder(HsSortAttribute)));

    connect(mAllAppsState, SIGNAL(entered()),
            this, SLOT(allAppsStateEntered()));

    connect(mAllAppsState, SIGNAL(toAppLibraryState()),
            this, SIGNAL(toHomescreenState()));
    connect(mAllCollectionsState, SIGNAL(toAppLibraryState()),
            this, SIGNAL(toHomescreenState()));

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
        mMenuMode.setHsMenuMode(
            static_cast<HsMenuMode>(data.value(menuModeType()).toInt()));
    } else {
        mMenuMode.setHsMenuMode(NormalHsMenuMode);
    }

    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::onEntry");
}

/*!
 Sets up Toolbar.
 */
void HsAppLibraryState::constructToolbar()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::constructToolbar");

    connect(mMenuViewBuilder.oviStoreAction(), SIGNAL(triggered()),
            this, SLOT(oviStoreAction()));

    HsOperatorHandler *const operatorHandler = new HsOperatorHandler(this);

    const bool operatorActionAvailable = !operatorHandler->icon().isNull();

    if (operatorActionAvailable) {

        //TODO HbToolBarExtension is not supported in docml currently
        //should be changed in future
        bool loaded = HbStyleLoader::registerFilePath(
                     ":/css/hsapplibrarystateplugin.css");
        Q_ASSERT(loaded);
        HbAction *const operatorAction(mMenuViewBuilder.operatorAction());

        HbToolBarExtension *const extension(
            mMenuViewBuilder.toolBarExtension());
        HbAction *const extensionAction(
            mMenuViewBuilder.toolBar()->addExtension(extension));

        extensionAction->setIcon(HbIcon("qtg_mono_store"));

        operatorAction->setIcon(operatorHandler->icon());
        operatorAction->setText(hbTrId(operatorHandler->text().toLatin1()));

        //TODO: no locstring for ovi store currently
        mMenuViewBuilder.oviStoreAction()->setText("Ovi Store");

        connect(mMenuViewBuilder.operatorAction(), SIGNAL(triggered()),
                operatorHandler, SLOT(action()));

        extension->addAction(mMenuViewBuilder.oviStoreAction());
        extension->addAction(operatorAction);

    } else {
        mMenuViewBuilder.toolBar()->addAction(
            mMenuViewBuilder.oviStoreAction());
    }


    HbAction *const allCollectionsAction(mMenuViewBuilder.allCollectionsAction());

    mAllAppsState->addTransition(
        allCollectionsAction, SIGNAL(triggered()), mAllCollectionsState);
    mCollectionState->addTransition(
        allCollectionsAction, SIGNAL(triggered()), mAllCollectionsState);

    HbAction *const allAppsAction(mMenuViewBuilder.allAppsAction());

    mAllCollectionsState->addTransition(
        allAppsAction, SIGNAL(triggered()), mAllAppsState);
    mCollectionState->addTransition(
        allAppsAction, SIGNAL(triggered()), mAllAppsState);

    mAllAppsState->assignProperty(allAppsAction, "checked", true);
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
        mMenuViewBuilder.toolBarActionGroup()->checkedAction();

    if (checkedAction != NULL) {
        checkedAction->setChecked(false);
    }
    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::clearToolbarLatch");
}

/*!
 Ovi Store Action slot
*/
bool HsAppLibraryState::oviStoreAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::oviStoreAction");
    CaEntry oviEntry;
    oviEntry.setEntryTypeName(urlTypeName());
    oviEntry.setAttribute(urlEntryKey(),
                          QString("https://store.ovi.com/applications/"));

    bool result = CaService::instance()->executeCommand(oviEntry);
    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::oviStoreAction");
    return result;
}

/*!
 All apps stete entered.
 */
void HsAppLibraryState::allAppsStateEntered()
{
    if (mMenuMode.getHsMenuMode() == NormalHsMenuMode) {
        mHistoryTransaction->setTargetState(mAllAppsState);
    }
}

/*!
 All collections state entered.
 */
void HsAppLibraryState::allCollectionsStateEntered()
{
    if (mMenuMode.getHsMenuMode() == NormalHsMenuMode) {
        mHistoryTransaction->setTargetState(mAllCollectionsState);
    }
}

