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
 * Description: Menu All Collections state.
 *
 */
#include <QScopedPointer>
#include <QStateMachine>
#include <HbView>
#include <HbListView>
#include <HbMainWindow>
#include <HbMenu>
#include <HbAction>
#include <HbAbstractViewItem>
#include <HbInstance>
#include <HbSearchPanel>

#include <hsmenueventfactory.h>
#include "hsmenumodewrapper.h"
#include "hsmenuviewbuilder.h"
#include "hsmenuitemmodel.h"
#include "cadefs.h"
#include "hsallcollectionsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsmenumodetransition.h"
#include "hsmainwindow.h"

/*!
 \class HsAllCollectionsState
 \ingroup group_hsmenustateplugin
 \brief All Collections State.
 Displays all collections present on the device.
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsAllCollectionsState::mSortAttribute
 Sort order
 */

/*!
 \var HsAllCollectionsState::mMenuView
 Wrapper for All Collections View.
 */

/*!
 \var HsAllCollectionsState::mAllCollectionsModel
 Item Model for the List.
 Own.
 */

/*!
 \fn void HsAllCollectionsState::sortOrderChanged(HsSortAttribute sortAttribute);
 Signal emitted when sort order is changed.
 \param sortOrder new sort order.
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode reference to object representing menu mode (add mode on/add mode off).
 \param parent Owner.
 */
HsAllCollectionsState::HsAllCollectionsState(
    HsMenuViewBuilder &menuViewBuilder,
    HsMenuModeWrapper &menuMode,
    HsMainWindow &mainWindow,
    QState *parent):
    HsBaseViewState(parent),
    mSortAttribute(CustomHsSortAttribute),
    mMenuView(menuViewBuilder, HsAllCollectionsContext),
    mMenuMode(menuMode),
    mAllCollectionsModel(0),
    mMainWindow(mainWindow),
    mContextModelIndex(), mContextMenu(0)
{
    construct();
}

/*!
 Constructs contained objects.
 */
void HsAllCollectionsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::construct");

    QState *initialState = new QState(this);
    setInitialState(initialState);

    QState *addModeState = new QState(this);
    connect(addModeState, SIGNAL(entered()),SLOT(addModeEntered()));

    QState *normalModeState = new QState(this);
    connect(normalModeState, SIGNAL(entered()),SLOT(normalModeEntered()));
    connect(normalModeState, SIGNAL(exited()),SLOT(normalModeExited()));

    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuMode, NormalHsMenuMode, normalModeState));
    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuMode, AddHsMenuMode, addModeState));

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/allcollectionsstate");
    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));

    mSecondarySoftkeyAction = new HbAction(Hb::BackNaviAction, this);

    mMenuView.view()->setNavigationAction(
        mSecondarySoftkeyAction);

    connect(mSecondarySoftkeyAction, SIGNAL(triggered()),
            SIGNAL(toAppLibraryState()));

    mAllCollectionsModel = HsMenuService::getAllCollectionsModel(
                               mSortAttribute);

    mMenuView.setModel(mAllCollectionsModel);

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::construct");
}

/*!
 Creates and installs menu options for the view
 */
void HsAllCollectionsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::setMenuOptions");

    QScopedPointer<HbMenu> viewOptions(new HbMenu);

    viewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                           this,
                           SLOT(openTaskSwitcher()));
    viewOptions->addAction(hbTrId("txt_applib_opt_new_collection"),
                           this, SLOT(createNewCollection()));

    HbMenu *const sortMenu = viewOptions->addMenu(
                                 hbTrId("txt_applib_opt_sort_by"));

    //Grouped options are exclusive by default.
    QActionGroup *sortGroup = new QActionGroup(viewOptions.data());
    sortGroup->addAction(sortMenu->addAction(
                             hbTrId("txt_applib_opt_sub_custom"),
                             this, SLOT(customMenuAction())));
    sortGroup->addAction(sortMenu->addAction(
                             hbTrId("txt_applib_opt_sub_ascending"),
                             this, SLOT(ascendingMenuAction())));
    sortGroup->addAction(sortMenu->addAction(
                             hbTrId("txt_applib_opt_sub_descending"),
                             this, SLOT(descendingMenuAction())));

    if (mSortAttribute == CustomHsSortAttribute) {
        viewOptions->addAction(hbTrId("txt_applib_opt_arrange"),
                               this, SLOT(createArrangeCollection()));
    }

    foreach(QAction *action, sortMenu->actions()) {
        action->setCheckable(true);
    }

    switch (mSortAttribute) {
    case AscendingNameHsSortAttribute:
        sortGroup->actions().at(1)->setChecked(true);
        break;
    case DescendingNameHsSortAttribute:
        sortGroup->actions().at(2)->setChecked(true);
        break;
    case CustomHsSortAttribute:
    default:
        sortGroup->actions().at(0)->setChecked(true);
        break;
    }

    mMenuView.view()->setMenu(viewOptions.take());

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::setMenuOptions");
}

/*!
 Destructor.
 */
HsAllCollectionsState::~HsAllCollectionsState()
{
    mMenuView.setModel(NULL);
    delete mAllCollectionsModel;
}

/*!
 Populates all folders
 */
void HsAllCollectionsState::scrollToBeginning()
{

    mMenuView.listView()->scrollTo(
        mAllCollectionsModel->index(0), HbAbstractItemView::PositionAtTop);
}

/*!
 Slot invoked when a state is entered.
 */


void HsAllCollectionsState::stateEntered()
{
    qDebug("AllCollectionsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::stateEntered");

    mMainWindow.setCurrentView(mMenuView);
    mMenuView.activate();

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::stateExited");
}

/*!
 Slot invoked when normal mode is entered.
*/
void HsAllCollectionsState::normalModeEntered()
{
    setMenuOptions();
    connect(&mMainWindow, SIGNAL(viewIsReady()),
        &mMainWindow, SLOT(saveActivity()),
        Qt::UniqueConnection);
    connect(&mMenuView,
            SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Slot invoked when normal mode is exited.
*/
void HsAllCollectionsState::normalModeExited()
{
    mMenuView.view()->setMenu(NULL);
}

/*!
 Slot invoked when add mode is entered.
 */
void HsAllCollectionsState::addModeEntered()
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
void HsAllCollectionsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::stateExited");
    
    disconnect(&mMainWindow, SIGNAL(viewIsReady()),
               &mMainWindow, SLOT(saveActivity()));
    
    mMenuView.setSearchPanelVisible(false);

    mMenuView.disconnect(this);

    mMenuView.inactivate();

    if (mContextMenu)
        mContextMenu->close();
    
    HsBaseViewState::stateExited();

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::stateExited");
    qDebug("AllCollectionsState::stateExited()");
}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsAllCollectionsState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

/*!
 Slot connected to List widget in normal mode.
 \param index Model index of the activated item.
 */
void HsAllCollectionsState::listItemActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::listItemActivated");
    QVariant data = mAllCollectionsModel->data(index, CaItemModel::IdRole);
    int id = data.toInt();
    QString collectionType =
        mAllCollectionsModel->data(index, CaItemModel::TypeRole).toString();
    qDebug("AllCollectionsState::listItemActivated - MCS ID: %d",
           data.toInt());

    mMenuView.setSearchPanelVisible(false);

    machine()->postEvent(HsMenuEventFactory::createOpenCollectionFromAppLibraryEvent(id,
                         collectionType));
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::listItemActivated");
}

/*!
 Slot connected to List widget in add mode.
 \param index Model index of the activated item.
 */
void HsAllCollectionsState::addActivated(const QModelIndex &index)
{
    const int itemId = index.data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(
            itemId, mMenuMode.getHsMenuMode(), mMenuMode.getHsToken()));
}

/*!
 Slot connected to List widget in add mode.
 Called when item long pressed.
 \param item View item.
 \param coords Press point coordinates.
 */
void HsAllCollectionsState::addLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    Q_UNUSED(coords);
    const int itemId = item->modelIndex().data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(itemId,
                mMenuMode.getHsMenuMode(), mMenuMode.getHsToken()));
}

/*!
 Slot connected to List widget in normal mode.
 Called when item long pressed.
 \param item View item.
 \param coords Press point coordinates.
 */
void HsAllCollectionsState::listItemLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::listItemLongPressed");

    mContextMenu = new HbMenu;

    // create context menu
    HbAction *addShortcutAction = mContextMenu->addAction(hbTrId(
                                      "txt_applib_menu_add_to_home_screen"));
    addShortcutAction->setData(AddToHomeScreenContextAction);
    HbAction *renameAction = NULL;
    HbAction *deleteAction = NULL;

    EntryFlags flags = item->modelIndex().data(CaItemModel::FlagsRole).value<
                       EntryFlags> ();

    if ((flags & RemovableEntryFlag)) {
        renameAction = mContextMenu->addAction(
                           hbTrId("txt_common_menu_rename_item"));
        renameAction->setData(RenameContextAction);
        deleteAction = mContextMenu->addAction(hbTrId("txt_common_menu_delete"));
        deleteAction->setData(DeleteContextAction);
    }

    mContextModelIndex = item->modelIndex();
    mContextMenu->setPreferredPos(coords);
    mContextMenu->setAttribute(Qt::WA_DeleteOnClose);
    mContextMenu->open(this, SLOT(contextMenuAction(HbAction*)));

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::listItemLongPressed");
}

/*!
 Handles context menu actions
 */
void HsAllCollectionsState::contextMenuAction(HbAction *action)
{
    HsContextAction command = 
        static_cast<HsContextAction>(action->data().toInt());

    const int itemId = mContextModelIndex.data(CaItemModel::IdRole).toInt();

    switch (command) {
        case AddToHomeScreenContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createAddToHomeScreenEvent(
                    itemId, mMenuMode.getHsMenuMode(), mMenuMode.getHsToken()));
            break;
        case RenameContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createRenameCollectionEvent(itemId));
            break;
        case DeleteContextAction:
             machine()->postEvent(
                HsMenuEventFactory::createDeleteCollectionEvent(itemId));
            break;      
        default:
            break;
    }
                                    
    mMenuView.setSearchPanelVisible(false);
}

/*!
 Slot connected to constructMenu.
 */
void HsAllCollectionsState::createNewCollection()
{
    // Adding a new collection via the Collections view
    machine()->postEvent(HsMenuEventFactory::createNewCollectionEvent());
}

/*!
 Slot connected to constructMenu.
 */
void HsAllCollectionsState::createArrangeCollection()
{
    // Arrange collection via the Arrange view
    int topItemId(0);

    const QList<HbAbstractViewItem *> array =
        mMenuView.listView()->visibleItems();

    if (array.count() >= 1) {
        QModelIndex idx = array[0]->modelIndex();
        topItemId = idx.data(CaItemModel::IdRole).toInt();
    }

    machine()->postEvent(
        HsMenuEventFactory::createArrangeCollectionEvent(topItemId));
}

/*!
 Menu custom sort action slot.
 */
void HsAllCollectionsState::customMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::customMenuAction");
    mSortAttribute = CustomHsSortAttribute;
    setMenuOptions();
    mAllCollectionsModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::customMenuAction");
}

/*!
 Menu ascending sort action slot.
 */
void HsAllCollectionsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::ascendingMenuAction");
    mSortAttribute = AscendingNameHsSortAttribute;
    setMenuOptions();
    mAllCollectionsModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::ascendingMenuAction");
}

/*!
 Menu descending sort action slot.
 */
void HsAllCollectionsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::descendingMenuAction");
    mSortAttribute = DescendingNameHsSortAttribute;
    setMenuOptions();
    mAllCollectionsModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::descendingMenuAction");
}

