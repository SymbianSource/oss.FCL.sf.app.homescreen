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
    mSortAttribute(AscendingNameHsSortAttribute)
{
    initialize(menuViewBuilder, HsAllAppsContext);
    construct();
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
    QScopedPointer<HbMenu> viewOptions(new HbMenu);
    viewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                           static_cast<HsBaseViewState*>(this), SLOT(openTaskSwitcher()));
    viewOptions->addAction(hbTrId("txt_applib_opt_add_to_collection"),
                           this, SLOT(addToCollection()));
    viewOptions->addAction(hbTrId("txt_applib_opt_check_software_updates"),
                           static_cast<HsBaseViewState*>(this), SLOT(checkSoftwareUpdates()));

    HbMenu *const sortMenu = viewOptions->addMenu(hbTrId(
                                 "txt_applib_opt_sort_by"));
    //Grouped options are exclusive by default.
    QActionGroup *sortGroup = new QActionGroup(viewOptions.data());

    sortGroup->addAction(
        sortMenu->addAction(hbTrId("txt_applib_opt_sub_ascending"),
                            this, SLOT(ascendingMenuAction())));
    sortGroup->addAction(
        sortMenu->addAction(hbTrId("txt_applib_opt_sub_descending"),
                            this, SLOT(descendingMenuAction())));

    viewOptions->addAction(hbTrId("txt_applib_subtitle_installed"),
                           this, SLOT(openInstalledView()));

    foreach(QAction *action, sortMenu->actions()) {
        action->setCheckable(true);
    }
    int currentSortingPosition(-1);
    switch (mSortAttribute) {
    case AscendingNameHsSortAttribute:
        currentSortingPosition = 0;
        break;
    case DescendingNameHsSortAttribute:
        currentSortingPosition = 1;
        break;
    default:
        break;
    }
    if (currentSortingPosition >= 0) {
        sortGroup->actions().at(currentSortingPosition)->setChecked(true);
    }
    mMenuView->view()->setMenu(viewOptions.take());

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::setMenuOptions");
}

/*!
 Destructor.
 */

HsAllAppsState::~HsAllAppsState()
{
}


/*!
 Slot invoked when add mode entered.
 */
void HsAllAppsState::normalModeEntered()
{
    HsBaseViewState::normalModeEntered();
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            static_cast<HsBaseViewState*>(this), SLOT(launchItem(QModelIndex)));
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
 Slot connected to List widget in add mode.
 \param index Model index of the activated item.
 */
void HsAllAppsState::addActivated(const QModelIndex &index)
{
    mMenuView->disconnect(this);
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addActivated");
    addToHomeScreen(index);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addActivated");
}


/*!
 Handles context menu actions
 */
void HsAllAppsState::contextMenuAction(HbAction *action)
{
    HsContextAction command =
        static_cast<HsContextAction>(action->data().toInt());

    const int itemId = mContextModelIndex.data(CaItemModel::IdRole).toInt();

    switch (command) {
        case AddToHomeScreenContextAction:
            addToHomeScreen(mContextModelIndex);
            break;
        case AddToCollectionContextAction:
            // Addding a specific application to
            // an existing collection via item specific menu.
            machine()->postEvent(
                HsMenuEventFactory::createAddAppsFromApplicationsViewEvent(
                    mSortAttribute, itemId));
            break;
        case UninstallContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createUninstallApplicationEvent(itemId));
            break;
        case AppSettingContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createAppSettingsViewEvent(itemId));
            break;
        case AppDetailsContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createAppDetailsViewEvent(itemId));
            break;
        case OpenContextAction:
            launchItem(mContextModelIndex);
            break;
        default:
            break;
    }
    mMenuView->hideSearchPanel();
}

/*!
 Slot connected to List widget in add mode.
 Called when item long pressed.
 \param item View item.
 \param coords Press point coordinates.
 */
void HsAllAppsState::addLongPressed(HbAbstractViewItem *item,
                                    const QPointF &coords)
{
    Q_UNUSED(coords);
    mMenuView->disconnect(this);
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addLongPressed");
    addToHomeScreen(item->modelIndex());
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addLongPressed");

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
 Triggers event so that a state adding to Home Screen is reached
 \param index of an item to be added to homescreen.
 \retval void
 */
void HsAllAppsState::addToHomeScreen(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addToHomeScreen");
    QSharedPointer<const CaEntry> entry = mModel->entry(index);

    machine()-> postEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                              entry->id(), mMenuMode->getHsMenuMode(),
                              mMenuMode->getHsToken()));

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addToHomeScreen");
}

/*!
 Method seting context menu options.
 */
void HsAllAppsState::setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags)
{
    HbAction *openAction = mContextMenu->addAction(hbTrId(
        "txt_common_menu_open"));
    openAction->setData(OpenContextAction);

    HbAction *addToHomeScreenAction = mContextMenu->addAction(
        hbTrId("txt_applib_menu_add_to_home_screen"));
    addToHomeScreenAction->setData(AddToHomeScreenContextAction);

    HbAction *addToCollectionAction = mContextMenu->addAction(
        hbTrId("txt_applib_menu_add_to_collection"));
    addToCollectionAction->setData(AddToCollectionContextAction);

    HbAction *uninstallAction = mContextMenu->addAction(
        hbTrId("txt_common_menu_delete"));
    uninstallAction->setData(UninstallContextAction);
    HbAction *appSettingsAction(NULL);
    HbAction *appDetailsAction(NULL);

    // check conditions and hide irrelevant menu items
    QSharedPointer<const CaEntry> entry = mModel->entry(item->modelIndex());

    if (!(entry->attribute(appSettingsPlugin()).isEmpty())) {
        appSettingsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_settings"));
        appSettingsAction->setData(AppSettingContextAction);
    }
    if (!(entry->attribute(componentIdAttributeName()).isEmpty()) &&
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_details"));
        appDetailsAction->setData(AppDetailsContextAction);
    }

    if (!(flags & RemovableEntryFlag)) {
        uninstallAction->setVisible(false);
    }
}

/*!
  Menu ascending sort action slot.
 */
void HsAllAppsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::ascendingMenuAction");
    mSortAttribute = AscendingNameHsSortAttribute;
    mModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::ascendingMenuAction");
}

/*!
 Menu descending sort action slot.
 */
void HsAllAppsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::descendingMenuAction");
    mSortAttribute = DescendingNameHsSortAttribute;
    mModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::descendingMenuAction");

}
