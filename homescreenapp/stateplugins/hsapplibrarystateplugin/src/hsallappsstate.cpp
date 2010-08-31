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

#include <QStateMachine>
#include <HbMenu>
#include <HbAction>
#include <HbAbstractViewItem>
#include <HbIndexFeedback>
#include <HbListView>
#include <HbScrollBar>
#include <HbView>

#include <cadefs.h>
#include <caentry.h>
#include <caitemmodel.h>
#include <caservice.h>

#include "hsapp_defs.h"
#include "hsmenueventfactory.h"
#include "hsmenuitemmodel.h"
#include "hsallappsstate.h"
#include "hsmenumodewrapper.h"
#include "hsmenuviewbuilder.h"
#include "hsmainwindow.h"
#include "hsaddmodeproxymodel.h"
#include "hsapplibstateutils.h"

/*!
 \class HsAllAppsState
 \ingroup group_hsmenustateplugin
 \brief All Applications State.
 Displays all applications present on the device in a sorted list.
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsAllAppsState::mSortAttribute
 Sort order
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode Menu mode object(add mode on/add mode off).
 \param mainWindow Main window wrapper.
 \param parent Owner.
 */
HsAllAppsState::HsAllAppsState(HsMenuViewBuilder &menuViewBuilder,
                               HsMenuModeWrapper &menuMode,
                               HsMainWindow &mainWindow,
                               QState *parent) :
    HsBaseViewState(mainWindow, menuMode, parent),
    mAddModeProxyModel(0),
    mSortAttribute(Hs::AscendingNameHsSortAttribute)
{
    initialize(menuViewBuilder, HsAllAppsContext);
    construct();
    mAddModeProxyModel = new HsAddModeProxyModel(this);
    mAddModeProxyModel->setSourceModel(mModel);
}

/*!
 Constructs contained objects.
 */
void HsAllAppsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::construct");

    defineTransitions();

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/allappsstate");

    connect(mBackKeyAction, SIGNAL(triggered()), SIGNAL(toAppLibraryState()));

    mModel = HsMenuService::getAllApplicationsModel(mSortAttribute);
    mMenuView->setModel(mModel);

    mModel->preloadIcons();

    mMenuView->listView()->verticalScrollBar()->setInteractive(true);
    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mMenuView->view());
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mMenuView->listView());

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::construct");
}


/*!
 Creates and installs menu options for the view
 */
void HsAllAppsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::setMenuOptions");
    mViewOptions->clearActions();
    mViewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                           static_cast<HsBaseViewState*>(this), SLOT(openTaskSwitcher()));
    mViewOptions->addAction(hbTrId("txt_applib_opt_add_to_collection"),
                           this, SLOT(addToCollection()));
    mViewOptions->addAction(hbTrId("txt_applib_opt_check_software_updates"),
                           static_cast<HsBaseViewState*>(this), SLOT(checkSoftwareUpdates()));

    HbMenu *const sortMenu = mViewOptions->addMenu(hbTrId(
                                 "txt_applib_opt_sort_by"));
    //Grouped options are exclusive by default.
    QActionGroup *sortGroup = new QActionGroup(this);

    sortGroup->addAction(
        sortMenu->addAction(hbTrId("txt_applib_opt_sub_ascending"),
                            this, SLOT(ascendingMenuAction())));
    sortGroup->addAction(
        sortMenu->addAction(hbTrId("txt_applib_opt_sub_descending"),
                            this, SLOT(descendingMenuAction())));

    mViewOptions->addAction(hbTrId("txt_applib_opt_view_installed_applications"),
                           this, SLOT(openInstalledView()));

    foreach(QAction *action, sortMenu->actions()) {
        action->setCheckable(true);
    }
    int currentSortingPosition(-1);
    switch (mSortAttribute) {
    case Hs::AscendingNameHsSortAttribute:
        currentSortingPosition = 0;
        break;
    case Hs::DescendingNameHsSortAttribute:
        currentSortingPosition = 1;
        break;
    default:
        break;
    }
    if (currentSortingPosition >= 0) {
        sortGroup->actions().at(currentSortingPosition)->setChecked(true);
    }

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::setMenuOptions");
}

/*!
 Destructor.
 */

HsAllAppsState::~HsAllAppsState()
{
}


/*!
 Slot invoked when normal mode entered.
 */
void HsAllAppsState::normalModeEntered()
{
    HsBaseViewState::normalModeEntered();
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            static_cast<HsBaseViewState*>(this), SLOT(launchItem(QModelIndex)));
}

/*!
 Sets model for this view in depends on menuMode.
 \param menuMode menu mode
 */
void HsAllAppsState::setModel(Hs::HsMenuMode menuMode)
{
    if (menuMode == Hs::AddHsMenuMode) {
        if (mMenuView->model() != mAddModeProxyModel) {
            mMenuView->setModel(mAddModeProxyModel);
        }
    } else {
        if (mMenuView->model() != mModel) {
            mMenuView->setModel(mModel);
        }
    }
}


/*!
 Scrolls view to first item at top
 */
void HsAllAppsState::scrollToBeginning()
{
    QAbstractItemModel* model = mMenuView->model();
    if (model != NULL ) {
        mMenuView->listView()->scrollTo(
                model->index(0,0), HbAbstractItemView::PositionAtTop);
    }
}

/*!
 Slot invoked when a state is exited.
 */
void HsAllAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::stateExited");

    HsBaseViewState::stateExited();

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
  Slot called when application is adding to collection.
 */
void HsAllAppsState::addToCollection()
{
    // Add one/many applications to
    // a new/an existing collection via the All view
    machine()->postEvent(
        HsMenuEventFactory::createAddAppsFromApplicationsViewEvent(
            mSortAttribute));
}

/*!
 Triggers event so that a installed applications state is entered.
 \retval void
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAllAppsState::openInstalledView()
{
    machine()->postEvent(HsMenuEventFactory::createOpenInstalledViewEvent());
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Method seting context menu options.
 */
void HsAllAppsState::setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags)
{
    HbAction *openAction = mContextMenu->addAction(hbTrId(
        "txt_common_menu_open"));
    openAction->setData(Hs::OpenAppContextAction);

    HbAction *addToHomeScreenAction = mContextMenu->addAction(
        hbTrId("txt_applib_menu_add_to_home_screen"));
    addToHomeScreenAction->setData(Hs::AddToHomeScreenContextAction);

    HbAction *addToCollectionAction = mContextMenu->addAction(
        hbTrId("txt_applib_menu_add_to_collection"));

    addToCollectionAction->setData(
        Hs::AddToCollectionFromApplicationsViewContextAction);

    HbAction *uninstallAction = mContextMenu->addAction(
        hbTrId("txt_common_menu_delete"));
    uninstallAction->setData(Hs::UninstallContextAction);
    HbAction *appSettingsAction(NULL);
    HbAction *appDetailsAction(NULL);

    // check conditions and hide irrelevant menu items
    QSharedPointer<const CaEntry> entry = mModel->entry(item->modelIndex());

    if (!(entry->attribute(Hs::appSettingsPlugin).isEmpty())) {
        appSettingsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_settings"));
        appSettingsAction->setData(Hs::AppSettingContextAction);
    }
    if (!(entry->attribute(Hs::componentIdAttributeName).isEmpty()) &&
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_details"));
        appDetailsAction->setData(Hs::AppDetailsContextAction);
    }

    if (!(flags & RemovableEntryFlag)) {
        uninstallAction->setVisible(false);
    }

    addToHomeScreenAction->setVisible(
        !HsAppLibStateUtils::isCWRTWidgetOnHomeScreen(entry.data()));
}

/*!
  Menu ascending sort action slot.
 */
void HsAllAppsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::ascendingMenuAction");
    mSortAttribute = Hs::AscendingNameHsSortAttribute;
    mModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::ascendingMenuAction");
}

/*!
 Menu descending sort action slot.
 */
void HsAllAppsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::descendingMenuAction");
    mSortAttribute = Hs::DescendingNameHsSortAttribute;
    mModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::descendingMenuAction");

}
