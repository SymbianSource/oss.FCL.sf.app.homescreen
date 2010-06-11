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

#include <qstatemachine.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbabstractviewitem.h>
#include <hsmenueventfactory.h>
#include <hbinstance.h>
#include <caentry.h>
#include <hbsearchpanel.h>

#include "hsmenuitemmodel.h"
#include "cadefs.h"
#include "hsallappsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapp_defs.h"
#include "hsmenumodewrapper.h"
#include "hsmenuviewbuilder.h"
#include "hsmenumodetransition.h"
#include "caentry.h"
#include "caservice.h"
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
 \var HsAllAppsState::mCollectionsSortAttribute
 Collections sort order
 */

/*!
 \var HsAllAppsState::mMenuView
 Wrapper for All Applications View.
 */

/*!
 \var HsAllAppsState::mMenuMode
 Menu view mode wrapper.
 Not Own.
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode reference to object representing menu mode (add mode on/add mode off).
 \param parent Owner.
 */
HsAllAppsState::HsAllAppsState(HsMenuViewBuilder &menuViewBuilder,
                               HsMenuModeWrapper &menuMode,
                               HsMainWindow &mainWindow,
                               QState *parent) :
    QState(parent), mSortAttribute(AscendingNameHsSortAttribute),
    mCollectionsSortAttribute(LatestOnTopHsSortAttribute),
    mMenuView(menuViewBuilder, HsAllAppsContext),
    mMenuMode(menuMode),
    mAllAppsModel(0),
    mMainWindow(mainWindow),
    mContextModelIndex(), mContextMenu(0)
{
    construct();
}

/*!
 Constructs contained objects.
 */
void HsAllAppsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::construct");

    QState *initialState = new QState(this);
    setInitialState(initialState);

    QState *addModeState = new QState(this);
    connect(addModeState, SIGNAL(entered()),SLOT(addModeEntered()));

    QState *normalModeState = new QState(this);
    connect(normalModeState, SIGNAL(entered()),SLOT(normalModeEntered()));

    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuMode, NormalHsMenuMode, normalModeState));
    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuMode, AddHsMenuMode, addModeState));

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/allappsstate");

    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));

    mSecondarySoftkeyAction = new HbAction(Hb::BackNaviAction, this);

    mMenuView.view()->setNavigationAction(
        mSecondarySoftkeyAction);

    connect(mSecondarySoftkeyAction, SIGNAL(triggered()),
            SIGNAL(toAppLibraryState()));

    mAllAppsModel = HsMenuService::getAllApplicationsModel(mSortAttribute);
    mMenuView.setModel(mAllAppsModel);

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
                           this, SLOT(openTaskSwitcher()));
    viewOptions->addAction(hbTrId("txt_applib_opt_add_to_collection"),
                           this, SLOT(addToCollection()));
    viewOptions->addAction(hbTrId("txt_applib_opt_check_software_updates"),
                           this, SLOT(checkSoftwareUpdates()));

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
    mMenuView.view()->setMenu(viewOptions.take());

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::setMenuOptions");
}

/*!
 Destructor.
 */

HsAllAppsState::~HsAllAppsState()
{
    mMenuView.setModel(NULL);
    delete mAllAppsModel;
}

/*!
 Scrolls view to first item at top
 */
void HsAllAppsState::scrollToBeginning()
{
    mMenuView.listView()->scrollTo(
        mAllAppsModel->index(0), HbAbstractItemView::PositionAtTop);
}

/*!
 Slot invoked when a state is entered.
 */


void HsAllAppsState::stateEntered()
{
    qDebug("AllAppsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::stateEntered");

    mMainWindow.setCurrentView(mMenuView);
    mMenuView.activate();

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::stateEntered");
}

/*!
 Slot invoked when add mode entered.
 */
void HsAllAppsState::normalModeEntered()
{
    setMenuOptions();

    connect(&mMenuView,
            SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Add mode entered.
 */
void HsAllAppsState::addModeEntered()
{
    connect(&mMenuView,
            SIGNAL(activated(QModelIndex)),
            SLOT(addActivated(QModelIndex)));
    connect(&mMenuView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(addLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Slot invoked when a state is exited.
 */
void HsAllAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::stateExited");
    mMenuView.setSearchPanelVisible(false);

    mMenuView.disconnect(this);

    mMenuView.view()->setMenu(NULL);

    mMenuView.inactivate();

    if (mContextMenu)
        mContextMenu->close();

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsAllAppsState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

/*!
 Check software updates.
 \retval true if operation is successful.
 */
bool HsAllAppsState::checkSoftwareUpdates()
{
    return HsMenuService::launchSoftwareUpdate();
}

/*!
 Slot connected to List widget in normal mode.
 \param index Model index of the activated item.
 */
void HsAllAppsState::listItemActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::listItemActivated");

    QSharedPointer<const CaEntry> entry = mAllAppsModel->entry(index);

    if (entry->entryTypeName() == widgetTypeName()) {
        machine()->postEvent(
            HsMenuEventFactory::createPreviewHSWidgetEvent(
                entry->id(), entry->entryTypeName(), entry->attribute(
                    widgetUriAttributeName()), entry->attribute(
                    widgetLibraryAttributeName())));

        const int itemId = index.data(CaItemModel::IdRole).toInt();
        HsMenuService::touch(itemId);
    } else {
        QVariant data = mAllAppsModel->data(index, CaItemModel::IdRole);
        HsMenuService::executeAction(data.toInt());
    }

    mMenuView.setSearchPanelVisible(false);

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::listItemActivated");
}

/*!
 Slot connected to List widget in add mode.
 \param index Model index of the activated item.
 */
void HsAllAppsState::addActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addActivated");
    addToHomeScreen(index);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addActivated");
}

/*!
 Handles long-item-pressed event in all apps view by showing context menu
 \param item View item
 \param coords Press point coordinates
 */
void HsAllAppsState::listItemLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::listItemLongPressed");


    // create context menu
    mContextMenu = new HbMenu;

    HbAction *addToHomeScreenAction = mContextMenu->addAction(hbTrId(
                                          "txt_applib_menu_add_to_home_screen"));
    addToHomeScreenAction->setData(AddToHomeScreenContextAction);

    HbAction *addToCollectionAction = mContextMenu->addAction(hbTrId(
                                          "txt_applib_menu_add_to_collection"));
    addToCollectionAction->setData(AddToCollectionContextAction);

    HbAction *uninstallAction = mContextMenu->addAction(hbTrId(
                                    "txt_common_menu_delete"));
    uninstallAction->setData(UninstallContextAction);
    HbAction *appSettingsAction(NULL);
    HbAction *appDetailsAction(NULL);

    // check conditions and hide irrelevant menu items

    QSharedPointer<const CaEntry> entry = mAllAppsModel->entry(item->modelIndex());
    EntryFlags flags = item->modelIndex().data(
                           CaItemModel::FlagsRole).value<EntryFlags> ();

    if (!(entry->attribute(appSettingsPlugin()).isEmpty())) {
        appSettingsAction = mContextMenu->addAction(hbTrId(
                                                "txt_common_menu_settings"));
        appSettingsAction->setData(AppSettingContextAction);
    }
    if (!(entry->attribute(componentIdAttributeName()).isEmpty()) && 
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(hbTrId(
                                                "txt_common_menu_details"));
        appDetailsAction->setData(AppDetailsContextAction);
    }        

    if (!(flags & RemovableEntryFlag)) {
        uninstallAction->setVisible(false);
    }

    mContextModelIndex = item->modelIndex();
    mContextMenu->setPreferredPos(coords);
    mContextMenu->setAttribute(Qt::WA_DeleteOnClose);
    mContextMenu->open(this, SLOT(contextMenuAction(HbAction*)));


    HSMENUTEST_FUNC_EXIT("HsAllAppsState::listItemLongPressed");
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
                    mSortAttribute, mCollectionsSortAttribute, itemId));
            break;
        case UninstallContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createUninstallApplicationEvent(
                    itemId));
            break;
        case AppSettingContextAction: 
            machine()->postEvent(
                HsMenuEventFactory::createAppSettingsViewEvent(itemId));
            break;
        case AppDetailsContextAction: 
            machine()->postEvent(
                HsMenuEventFactory::createAppDetailsViewEvent(itemId));
            break;            
        default:
            break;
    }
                                    
    mMenuView.setSearchPanelVisible(false);
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
            mSortAttribute, mCollectionsSortAttribute));
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
 \param index of an item to be added to homescreen
 \retval void
 */
void HsAllAppsState::addToHomeScreen(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addToHomeScreen");
    QSharedPointer<const CaEntry> entry = mAllAppsModel->entry(index);

    machine()-> postEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                              entry->id(), mMenuMode.getHsMenuMode()));

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addToHomeScreen");
}

/*!
  Menu ascending sort action slot.
 */
void HsAllAppsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::ascendingMenuAction");
    mSortAttribute = AscendingNameHsSortAttribute;
    mAllAppsModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::ascendingMenuAction");
}

/*!
 Sets collections sort order
 /param sortOrder sort order.
 */
void HsAllAppsState::collectionsSortOrder(
    HsSortAttribute sortOrder)
{
    mCollectionsSortAttribute = sortOrder;
}

/*!
 Menu descending sort action slot.
 */
void HsAllAppsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::descendingMenuAction");
    mSortAttribute = DescendingNameHsSortAttribute;
    mAllAppsModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::descendingMenuAction");

}
